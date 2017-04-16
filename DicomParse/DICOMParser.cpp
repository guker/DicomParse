#ifdef _MSC_VER
#pragma warning ( disable : 4514 )
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4503 )
#pragma warning ( disable : 4710 )
#pragma warning ( push, 3 )
#endif 

#include <stdlib.h>
#if !defined(__MWERKS__)
#include <math.h>
#endif
#include <time.h>
#include <assert.h>
#if !defined(__MWERKS__)
#include <sys/types.h>
#endif

#include <string>
#include <map>
#include <vector>

#include "DICOMConfig.h"
#include "DICOMParser.h"

#define DICOMPARSER_IGNORE_MAGIC_NUMBER

#ifdef DEBUG_DICOM
#define DICOM_DBG_MSG(x) {dicom_stream::cout x}
#else 
#define DICOM_DBG_MSG(x)
#endif

static const char*   DICOM_MAGIC  = "DICM";
static const int     OPTIONAL_SKIP = 128;

std::vector<std::string> splitEx(const std::string& src, std::string separate_character)
{
    std::vector<std::string> strs;
    
  int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
    int lastPosition = 0,index = -1;
    while (-1 != (index = src.find(separate_character,lastPosition)))
    {
        strs.push_back(src.substr(lastPosition,index - lastPosition));
        lastPosition = index + separate_characterLen;
    }
    std::string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
    if (!lastString.empty())
        strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
    return strs;
}

class DICOMParserImplementation
{
public:
    DICOMParserImplementation() : Groups(),Elements(),Datatypes()
    {
    };

    dicom_stl::vector<doublebyte> Groups;
    dicom_stl::vector<doublebyte> Elements;
    dicom_stl::vector<DICOMParser::VRTypes> Datatypes; 
};

DICOMParser::DICOMParser() : ParserOutputFile()
{
    this->Implementation = new DICOMParserImplementation();
    this->buf_           = new DICOMBuffer();
    this->DataFile       = NULL;
    this->InitTypeMap();
    this->FileName       = "";
    this->isExplicitVR   = true;
    this->isLitteEndian  = true;
    this->rescale_intercept = 0;
    this->rescale_scope     = 1;
    memset(&info,0,sizeof(DicomParserInfo));
    // 若dicom文件不存在窗宽窗位tag
    info.WinCenter = 128;
    info.WinWidth  = 256;

    photometric = "";
    rgbconf     = 0;
}

DICOMParser::~DICOMParser()
{
    delete this->Implementation;
    delete this->buf_;
}

void DICOMParser::InitTypeMap()
{
    DICOMRecord dicom_tags[] = {{0x0002, 0x0002, DICOMParser::VR_UI}, // Media storage SOP class uid
    {0x0002, 0x0003, DICOMParser::VR_UI}, // Media storage SOP inst uid
    {0x0002, 0x0010, DICOMParser::VR_UI}, // Transfer syntax uid
    {0x0002, 0x0012, DICOMParser::VR_UI}, // Implementation class uid
    {0x0008, 0x0018, DICOMParser::VR_UI}, // Image UID
    {0x0008, 0x0020, DICOMParser::VR_DA}, // Series date
    {0x0008, 0x0030, DICOMParser::VR_TM}, // Series time
    {0x0008, 0x0060, DICOMParser::VR_SH}, // Modality
    {0x0008, 0x0070, DICOMParser::VR_SH}, // Manufacturer
    {0x0008, 0x1060, DICOMParser::VR_SH}, // Physician
    {0x0018, 0x0050, DICOMParser::VR_FL}, // slice thickness
    {0x0018, 0x0060, DICOMParser::VR_FL}, // kV
    {0x0018, 0x0088, DICOMParser::VR_FL}, // slice spacing
    {0x0018, 0x1100, DICOMParser::VR_SH}, // Recon diameter
    {0x0018, 0x1151, DICOMParser::VR_FL}, // mA
    {0x0018, 0x1210, DICOMParser::VR_SH}, // Recon kernel
    {0x0020, 0x000d, DICOMParser::VR_UI}, // Study UID
    {0x0020, 0x000e, DICOMParser::VR_UI}, // Series UID
    {0x0020, 0x0013, DICOMParser::VR_IS}, // Image number
    {0x0020, 0x0032, DICOMParser::VR_SH}, // Patient position
    {0x0020, 0x0037, DICOMParser::VR_SH}, // Patient position cosines
    {0x0028, 0x0010, DICOMParser::VR_US}, // Num rows
    {0x0028, 0x0011, DICOMParser::VR_US}, // Num cols
    {0x0028, 0x0030, DICOMParser::VR_FL}, // pixel spacing
    {0x0028, 0x0100, DICOMParser::VR_US}, // Bits allocated
    {0x0028, 0x0120, DICOMParser::VR_UL}, // pixel padding
    {0x0028, 0x1052, DICOMParser::VR_FL}, // pixel offset
    {0x7FE0, 0x0010, DICOMParser::VR_OW}   // pixel data
    };
    
    int num_tags = sizeof(dicom_tags)/sizeof(DICOMRecord);

    doublebyte group;
    doublebyte element;
    VRTypes datatype;

    for (int i = 0; i < num_tags; i++)
    {
        group = dicom_tags[i].group;
        element = dicom_tags[i].element;
        datatype = (VRTypes) dicom_tags[i].datatype;
    }
}

/// 重置
void DICOMParser::Reset()
{
    this->buf_->reset();
    this->isExplicitVR  = true;
    this->isLitteEndian = true;
    this->FileName = "";
    this->Implementation->Groups.clear();
    this->Implementation->Elements.clear();
    this->Implementation->Datatypes.clear();
    memset(&info,0,sizeof(DicomParserInfo));
    // 若dicom文件不存在窗宽窗位tag,默认值
    info.WinCenter = 128;
    info.WinWidth  = 256;

    this->rescale_intercept = 0;
    this->rescale_scope     = 1;

    photometric = "";
    rgbconf     = 0;
} 


///  设置Buffer
void DICOMParser::SetBuf(char* buffer,size_t size)
{
    buf_->set(buffer,size);
}

/// 判断是否为DICOM
bool DICOMParser::IsDICOMData()
{
    char magic_number[4];
    buf_->SkipToStart();
    buf_->Read((void*)magic_number,4);
    if( CheckMagic(magic_number) )
    {
        return true;
    }
    else
    {
        buf_->Skip(OPTIONAL_SKIP - 4);
        buf_->Read((void*)magic_number,4);
        if(  CheckMagic(magic_number) )
        {
            return true;
        }
        else
        {
#ifndef DICOMPARSER_IGNORE_MAGIC_NUMBER
            return false;
#else
            buf_->SkipToStart();

            doublebyte group = buf_->ReadDoubleByte();
            bool dicom;
            if (group == 0x0002 || group == 0x0008)
            {
                dicom_stream::cerr << "No DICOM magic number found, but file appears to be DICOM." << dicom_stream::endl;
                dicom_stream::cerr << "Proceeding without caution."  << dicom_stream::endl;
                dicom = true;
            }
            else
            {
                dicom = false;
            }
            buf_->SkipToStart();
            return dicom;
#endif 
        }
    }
}


bool DICOMParser::ReadHeader_ex()
{
    bool dicom = this->IsDICOMData();
    if (!dicom)
    {
        return false;
    }

    doublebyte group = 0;
    doublebyte element = 0;
    DICOMParser::VRTypes datatype = DICOMParser::VR_UNKNOWN;

    this->Implementation->Groups.clear();
    this->Implementation->Elements.clear();
    this->Implementation->Datatypes.clear();

    long fileSize = buf_->size();
    do 
    {
        this->ReadNextRecord_ex(group, element, datatype);

        this->Implementation->Groups.push_back(group);
        this->Implementation->Elements.push_back(element);
        this->Implementation->Datatypes.push_back(datatype);
        //std::cout << group << "    " << element << std::endl; 

    } while ((buf_->Tell() >= 0) && (buf_->Tell() < fileSize));

    return true;
}

void DICOMParser::ReadNextRecord_ex(doublebyte& group, doublebyte& element,DICOMParser::VRTypes& mytype)
{
    group = buf_->ReadDoubleByte();
    element = buf_->ReadDoubleByte();
    // VR
    char VR[3];
    quadbyte length = 0;
    bool enDir = false;
    int  level = 0;

    if( group == 0x0002)
    {
        doublebyte vr = buf_->ReadDoubleByteAsLittleEndian();
        memcpy(VR,&vr,2*sizeof(char));
        VR[2] = '\0';
        if( !strcmp(VR,"OB") ||  !strcmp(VR,"OW")  ||  !strcmp(VR,"SQ")  |  !strcmp(VR,"OF") ||  !strcmp(VR,"UT") |  !strcmp(VR,"UN"))
        {
            buf_->Skip(2);
            length = buf_->ReadQuadByte();
        }
        else
        {
            length = buf_->ReadDoubleByte();
        }
    }
    else if( (group == 0xfffe && element == 0xe000) || ( group == 0xfffe && element == 0xe00d) || (group == 0xfffe && element == 0xe0dd))
    {
        VR == "**";
        length = buf_->ReadQuadByte();
    }
    else if(isExplicitVR == true)
    {
        doublebyte vr = buf_->ReadDoubleByteAsLittleEndian();
        memcpy(VR,&vr,2*sizeof(char));
        VR[2] = '\0';
        if( !strcmp(VR,"OB") ||  !strcmp(VR,"OW")  ||  !strcmp(VR,"SQ")  |  !strcmp(VR,"OF") ||  !strcmp(VR,"UT") |  !strcmp(VR,"UN"))
        {
            buf_->Skip(2);
            length = buf_->ReadQuadByte();
        }
        else
        {
            length = buf_->ReadDoubleByte();
        }
    }
    else if(isExplicitVR == false)
    {
        ///  隐式VR根据tag一个一个
       // VR = "*";  
        length = buf_->ReadQuadByte();
    }
    
    if(group == 0x7fe0 && element == 0x0010)
    {
        img_data_.resize(length+1);
        img_data_[length] = 0;
        if (length > 0) 
        {
            buf_->Read(img_data_.data(),length);
        }
    }
    else if( (!strcmp(VR,"SQ") && length == 0xffffffff) || ( group == 0xfffe && element == 0xe000) && length == 0xffffffff )
    {
        /// 遇到文件夹开始标签
        if( enDir == false)
        {
            enDir = true;
        }
        else
        {
            level++;
        }
    }
    else if((group == 0xfffe && element == 0xe00d && length == 0xffffffff) || (group == 0xfffe && element == 0xe0dd && length == 0xffffffff))
    {
        if(enDir == true)
        {
            enDir = false;
        }
        else
        {
            level--;
        }
    }
    else
    {
        tag_data_.resize(length+1);
        tag_data_[length] = 0;
        if (length > 0) 
        {
            buf_->Read(tag_data_.data(),length);
        }
        if(group == 0x0002 && element == 0x0010)
        {
            if(!strcmp(tag_data_.data(),"1.2.840.10008.1.2.1"))
            {
                 isLitteEndian = true;
                 isExplicitVR = true;
            }

            if(!strcmp(tag_data_.data(),"1.2.840.10008.1.2.2"))
            {
                 isLitteEndian = false;
                 isExplicitVR = true;
            }
            if(!strcmp(tag_data_.data(),"1.2.840.10008.1.2"))
            {
                 isLitteEndian = true;
                 isExplicitVR = false;
            }
        }
        DumgTag(group,element,(unsigned char*)tag_data_.data(),length);
    }
}


bool DICOMParser::OpenFile(const dicom_stl::string& filename)
{
    if(this->DataFile)
    {
        delete this->DataFile;
    }
    this->DataFile = new DICOMFile();

    bool  val = this->DataFile->Open(filename);

    if(val)
    {
        this->FileName = filename;
    }
    return val;
}

bool DICOMParser::IsDICOMFile(DICOMFile* file) {
    char magic_number[4];    
    file->SkipToStart();
    file->Read((void*)magic_number,4);
    if (CheckMagic(magic_number)) 
    {
        return(true);
    } 
    // try with optional skip
    else 
    { 
        file->Skip(OPTIONAL_SKIP-4);
        file->Read((void*)magic_number,4);
        if (CheckMagic(magic_number)) 
        {
            return true;
        }
        else
        {

#ifndef DICOMPARSER_IGNORE_MAGIC_NUMBER
            return false;
#else
            file->SkipToStart();

            doublebyte group = file->ReadDoubleByte();
            bool dicom;
            if (group == 0x0002 || group == 0x0008)
            {
                dicom_stream::cerr << "No DICOM magic number found, but file appears to be DICOM." << dicom_stream::endl;
                dicom_stream::cerr << "Proceeding without caution."  << dicom_stream::endl;
                dicom = true;
            }
            else
            {
                dicom = false;
            }
            file->SkipToStart();

            return dicom;
#endif

        }
    }
}


bool DICOMParser::ReadHeader() 
{
    bool dicom = this->IsDICOMFile(this->DataFile);
    if (!dicom)
    {
        return false;
    }

    doublebyte group = 0;
    doublebyte element = 0;
    DICOMParser::VRTypes datatype = DICOMParser::VR_UNKNOWN;

    this->Implementation->Groups.clear();
    this->Implementation->Elements.clear();
    this->Implementation->Datatypes.clear();

    long fileSize = DataFile->GetSize();
    do 
    {
        this->ReadNextRecord(group, element, datatype);

        this->Implementation->Groups.push_back(group);
        this->Implementation->Elements.push_back(element);
        this->Implementation->Datatypes.push_back(datatype);

    } while ((DataFile->Tell() >= 0) && (DataFile->Tell() < fileSize));

    return true;
}

void DICOMParser::ReadNextRecord(doublebyte& group, doublebyte& element, DICOMParser::VRTypes& mytype)
{
    group = DataFile->ReadDoubleByte();
    element = DataFile->ReadDoubleByte();
    // VR
    char VR[3];
    quadbyte length = 0;
    bool enDir = false;
    int  level = 0;

    if( group == 0x0002)
    {
        doublebyte vr = DataFile->ReadDoubleByteAsLittleEndian();
        memcpy(VR,&vr,2*sizeof(char));
        VR[2] = '\0';
        if( !strcmp(VR,"OB") ||  !strcmp(VR,"OW")  ||  !strcmp(VR,"SQ")  |  !strcmp(VR,"OF") ||  !strcmp(VR,"UT") |  !strcmp(VR,"UN"))
        {
            DataFile->Skip(2);
            length = DataFile->ReadQuadByte();
        }
        else
        {
            length = DataFile->ReadDoubleByte();
        }
    }
    else if( (group == 0xfffe && element == 0xe000) || ( group == 0xfffe && element == 0xe00d) || (group == 0xfffe && element == 0xe0dd))
    {
        VR == "**";
        length = DataFile->ReadQuadByte();
    }
    else if(isExplicitVR == true)
    {
        doublebyte vr = DataFile->ReadDoubleByteAsLittleEndian();
        memcpy(VR,&vr,2*sizeof(char));
        VR[2] = '\0';
        if( !strcmp(VR,"OB") ||  !strcmp(VR,"OW")  ||  !strcmp(VR,"SQ")  |  !strcmp(VR,"OF") ||  !strcmp(VR,"UT") |  !strcmp(VR,"UN"))
        {
            DataFile->Skip(2);
            length = DataFile->ReadQuadByte();
        }
        else
        {
            length = DataFile->ReadDoubleByte();
        }
    }
    else if(isExplicitVR == false)
    {
        ///  隐式VR根据tag一个一个
       // VR = "*";  
        length = DataFile->ReadQuadByte();
    }
    
    if(group == 0x7fe0 && element == 0x0010)
    {
        img_data_.resize(length+1);
        img_data_[length] = 0;
        if (length > 0) 
        {
            DataFile->Read(img_data_.data(),length);
        }
    }
    else if( (!strcmp(VR,"SQ") && length == 0xffffffff) || ( group == 0xfffe && element == 0xe000) && length == 0xffffffff )
    { 
        /// 遇到文件夹开始标签
        if( enDir == false)
        {
            enDir = true;
        }
        else
        {
            level++;
        }
    }
    else if((group == 0xfffe && element == 0xe00d && length == 0xffffffff) || (group == 0xfffe && element == 0xe0dd && length == 0xffffffff))
    {
        if(enDir == true)
        {
            enDir = false;
        }
        else
        {
            level--;
        }
    }
    else
    {
        tag_data_.resize(length+1);
        tag_data_[length] = 0;
        if (length > 0) 
        {
            DataFile->Read(tag_data_.data(),length);
        }
        if(group == 0x0002 && element == 0x0010)
        {
            if(!strcmp(tag_data_.data(),"1.2.840.10008.1.2.1"))
            {
                 isLitteEndian = true;
                 isExplicitVR = true;
            }

            if(!strcmp(tag_data_.data(),"1.2.840.10008.1.2.2"))
            {
                 isLitteEndian = false;
                 isExplicitVR = true;
            }
            if(!strcmp(tag_data_.data(),"1.2.840.10008.1.2"))
            {
                 isLitteEndian = true;
                 isExplicitVR = false;
            }
        }
        DumgTag(group,element,(unsigned char*)tag_data_.data(),length);
    }
}


bool DICOMParser::CheckMagic(char* magic_number) 
{
    return (
        (magic_number[0] == DICOM_MAGIC[0]) &&
        (magic_number[1] == DICOM_MAGIC[1]) &&
        (magic_number[2] == DICOM_MAGIC[2]) &&
        (magic_number[3] == DICOM_MAGIC[3])
        );
}

void DICOMParser::DumgTag(doublebyte group, doublebyte element,unsigned char* temdata,quadbyte length)
{
    const int len = length+1;
    if(0x0008 == group && 0x0008 == element)
    {
        char* imageType = new char[len]; 
        memcpy(imageType,temdata,len-1);
        imageType[len-1] = '\0';
        strcpy(info.ImageType,imageType);
        //info.ImageType =  imageType;
        delete[] imageType;
    }

    if( 0x0008 == group && 0x0020 == element)
    {
        char* study_date = new char[len];
        memcpy(study_date,temdata,len-1);
        study_date[len-1] = '\0';
        //info.StudyDate = study_date;
        strcpy(info.StudyDate,study_date);
        delete[] study_date;
    }

    if(0x0010 == group && 0x0010 == element)
    {
        char* patient_name = new char[len];
        memcpy(patient_name,temdata,len-1);
        patient_name[len-1] = '\0';
        //info.PatientName = patient_name;
        strcpy(info.PatientName,patient_name);
        delete[] patient_name;
    }

    if(0x0010 == group && 0x0020 == element)
    {
        char* patient_id = new char[len];
        memcpy(patient_id,temdata,len-1);
        patient_id[len-1] = '\0';
        //info.PatientId = patient_id;
        strcpy(info.PatientId,patient_id);
        delete[] patient_id;
    }

    if(0x0018 == group && 0x0050 == element)
    {
        char* sliceThickness = new char[len];
        memcpy(sliceThickness,temdata,len-1);
        sliceThickness[len-1] = '\0';
        info.SliceThikness = atof(sliceThickness);
        delete[] sliceThickness;
    }

    if(0x0020 == group && 0x0010 == element)
    {
        char* study_id = new char[len];;
        memcpy(study_id,temdata,len-1);
        study_id[len-1] = '\0';
        strcpy(info.StudyId,study_id);
        //info.StudyId = study_id;
        delete[]  study_id;
    }

    if(0x0020 == group && 0x0032 == element)
    {
        char* imagePosition = new char[len];
        memcpy(imagePosition,temdata,len-1);
        imagePosition[len-1] = '\0';
        std::string img_pos = imagePosition;
        std::vector<std::string> res = splitEx(img_pos,"\\");
        if(3 == res.size())
        {
            info.PosX = atof(res[0].c_str());
            info.PosY = atof(res[1].c_str());
            info.PoxZ = atof(res[2].c_str());
        }
        delete[] imagePosition;
    }

    if(0x0020 == group && 0x1041 == element)
    {
        char* sliceLocation = new char[len];
        memcpy(sliceLocation,temdata,len-1);
        sliceLocation[len-1] = '\0';
        info.SliceLoc = atof(sliceLocation);
        delete[] sliceLocation;
    }

    if(0x0028 == group && 0x0004 == element)
    {
        char* gray = new char[len];
        memcpy(gray,temdata,len-1);
        gray[len-1] = '\0';
        photometric = gray;
        delete[] gray;
    }

    if(0x0028 == group && 0x0006 == element)
    {
        char* conf = new char[len];
        memcpy(conf,temdata,len-1);
        conf[len-1] = '\0';
        rgbconf = atoi(conf);
        delete[] conf;
    }
    if(0x0028 == group && 0x0010 == element)
    {
        char* row = new char[len];
        memcpy(row,temdata,len-1);
        unsigned short val = *((unsigned short*)row);
        info.Height = val;
        delete[] row;
    }

    if(0x0028 == group && 0x0011 == element)
    {
        char* col = new char[len];
        memcpy(col,temdata,len-1);
        unsigned short val = *((unsigned short*)col);
        info.Width = val;
        delete[] col;
    }

    if(0x0028 == group && 0x0030 == element)
    {
        char* pixelSpacing = new char[len];
        memcpy(pixelSpacing,temdata,len-1);
        pixelSpacing[len-1] = '\0';
        std::string spacing = pixelSpacing;
        std::vector<std::string> res = splitEx(spacing,"\\");
        if(2 == res.size())
        {
            info.SpaceX = atof(res[1].c_str());
            info.SpaceY = atof(res[0].c_str());
        }
        delete[] pixelSpacing;
    }
    if(0x0028 == group && 0x1050 == element)
    {
        char* wincenter = new char[len];
        memcpy(wincenter,temdata,len-1);
        wincenter[len-1] = '\0';
        info.WinCenter = atoi(wincenter);
        delete[] wincenter;
    }

    if(0x0028 == group && 0x1051 == element)
    {
        char* winwidth = new char[len];
        memcpy(winwidth,temdata,len-1);
        winwidth[len-1] = '\0';
        info.WinWidth = atoi(winwidth);
        delete winwidth;
    }

    if(0x0028 == group && 0x1052 == element)
    {
        char*  rescale_b = new char[len];
        memcpy(rescale_b,temdata,len-1);
        rescale_b[len-1] = '\0';
        rescale_intercept = atoi(rescale_b);
        delete rescale_b;
    }

    if(0x0028 == group && 0x1053 == element)
    {
        char* rescale_m = new char[len];
        memcpy(rescale_m,temdata,len-1);
        rescale_m[len-1] = '\0';
        rescale_scope = atoi(rescale_m);
        delete rescale_m;
    }
}

void DICOMParser::GetGroupsElementsDatatypes(dicom_stl::vector<doublebyte>& groups,
    dicom_stl::vector<doublebyte>& elements,
    dicom_stl::vector<DICOMParser::VRTypes>& datatypes)
{
    groups.clear();
    elements.clear();
    datatypes.clear();

    dicom_stl::vector<doublebyte>::iterator giter; // = this->Groups.begin();
    dicom_stl::vector<doublebyte>::iterator eiter; // = this->Elements.begin();
    dicom_stl::vector<DICOMParser::VRTypes>::iterator diter; // = this->Datatypes.begin();

    for (giter = this->Implementation->Groups.begin(), eiter = this->Implementation->Elements.begin(), diter = this->Implementation->Datatypes.begin();
        giter != this->Implementation->Groups.end(), eiter != this->Implementation->Elements.end(), diter != this->Implementation->Datatypes.end();
        giter++, eiter++, diter++)
    {
        groups.push_back(*giter);
        elements.push_back(*eiter);
        datatypes.push_back(*diter);
    }
}

DICOMParser::DICOMParser(const DICOMParser&)
{
    dicom_stream::cerr << "DICOMParser copy constructor should not be called!" << dicom_stream::endl;
}

void DICOMParser::operator=(const DICOMParser&)
{
    dicom_stream::cerr << "DICOMParser assignment operator should not be called!" << dicom_stream::endl;
}


#ifdef _MSC_VER
#pragma warning ( pop )
#endif