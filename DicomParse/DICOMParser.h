#ifndef __DICOMParser_h_
#define __DICOMParser_h_


#ifdef _MSC_VER
#pragma warning ( disable : 4514 )
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4503 )
#pragma warning ( disable : 4710 )
#pragma warning ( disable : 4702 )
#pragma warning ( push, 3 )
#endif

#include <map>
#include <utility> 
#include <vector>

#include "DICOMConfig.h"
#include "DICOMFile.h"
#include "DICOMTypes.h"
#include "DICOMBuffer.h"
#include "jpg_0XC3.h"


typedef struct
{
    char PatientName[50];
    char PatientId[20];
    char StudyDate[20];
    char StudyId[20];
    char ImageType[50];
    char RefSopId[128];
    char SopId[128];

    int         WinCenter;
    int         WinWidth;

    float       SliceThikness;
    float       SliceLoc;

    float       PosX;
    float       PosY;
    float       PoxZ;

    float       SpaceX;
    float       SpaceY;

    int         Width;
    int         Height;

    float      ImageOrientation[6];

}DicomParserInfo;

class  DICOMParserImplementation;

class DICOM_EXPORT DICOMParser
{
public:
    DICOMParser();

    virtual ~DICOMParser();

    bool OpenFile(const dicom_stl::string& filename);

    void SetBuf(char* buffer,size_t size);

    bool IsDICOMData();

    void Reset();

    bool ReadHeader_ex();

    const dicom_stl::string&  GetFileName();

    bool ReadHeader();

    static bool IsDICOMFile( DICOMFile* file);

    bool IsDICOMFile()
    {
        if(!this->DataFile)
        {
            return false;
        }
        return DICOMParser::IsDICOMFile(this->DataFile);
    }

    // 获取dicom信息
    void GetDicomInfo(DicomParserInfo* dicom_info)
    {
        memcpy(dicom_info,&info,sizeof(DicomParserInfo));
    }

    // 获取dicom图像数据
    void GetDicomData(char** data,int* width,int* heigth)
    {
        DicomParserInfo dicom_info;
        GetDicomInfo(&dicom_info);

        *width  = dicom_info.Width;
        *heigth = dicom_info.Height;
        if(!strncmp(photometric.c_str(),"RGB",3))
        {
            
            if(0 == rgbconf)
            {
                ///  RGBRGBRGB...RGBRGBRGB
                *data = img_data_.data();
                unsigned char* tmp = (unsigned char*)img_data_.data();
                int size = img_data_.size();
                int j = 0; 
                for(int i = 0; i < size;)
                {
                    (*data)[j] =(char)((30*tmp[i] + 59*tmp[i+1] + 11*tmp[i+2])/100);
                    (*data)[j+1] = 0;
                    j+=2;
                    i+=3;
                }
            }
            else
            {
                /// RRRRRRRRRR...GGGGGGGGG...BBBBBBBB
                unsigned char* tmp = (unsigned char*)img_data_.data();
                int step = dicom_info.Width * dicom_info.Height;
                int j = 0;
                std::vector<char> res;
                res.reserve(2*step);
                for(int i = 0; i < step;i++)
                {
                    char gray = (char)((30*tmp[i] + 59*tmp[step + i] + 11*tmp[2*step + 1])/100);
                    res.push_back(gray);
                    res.push_back(0);
                }
                *data = res.data();
            }
            return;
        }

        if(is_jpeg70)
        {
            int height_new = 0;
            int width_new  = 0;
            int bitss      = 0;
            int frame      = 0;
            unsigned char* res = decode_JPEG_SOF_0XC3(img_data_.data(),img_data_.size(),0,&width_new,&height_new,&bitss,&frame);
            int dim = 2*(*width)*(*heigth);
            jpeg_data_.resize(dim);
            memcpy(&jpeg_data_[0],res,dim*sizeof(char));
            *data = jpeg_data_.data();
            delete res;
            return ;
        }
        /// MONOCHROME2情况
        short* im_data = (short*)(img_data_.data());
        int  size = (dicom_info.Height * dicom_info.Width);
        for(int i = 0; i < size;i++)
        {
            im_data[i] =  im_data[i] * rescale_scope;
            im_data[i] += rescale_intercept;
        }
        *data = (char*)im_data;
    }

    static bool CheckMagic(char* magic_number);

        enum VRTypes 
    {
        VR_UNKNOWN = 0x0,
        VR_OB=0x424f, // Other byte string (string of bytes, insensitive to byte order)
        VR_AW=0x5741, 
        VR_AE=0x4541, // Application Entity (char string)
        VR_AS=0x5341, // Age string (char string)
        VR_CS=0x5343, // Code string (char string, leading/trailing spaces insignificant)
        VR_UI=0x4955, // UID (character string)
        VR_DA=0x4144, // Date (char string)
        VR_DS=0x5344, // Decimal string (char string representing a fixed/floating pt number)
        VR_DT=0x5444, // Date Time (char string)
        VR_IS=0x5349, // Integer string (char string representing an integer)
        VR_LO=0x4f4c, // Long string (character string padded with leading/trailing spaces)
        VR_LT=0x544c, // Long text (character string with one or more paragraphs.)
        VR_OW=0x574f, // Other word string (sensitive to byte order, transfer syntax)
        VR_PN=0x4e50, // Person name (character string)
        VR_ST=0x5453, // Short text (character string, one or more paragraphs)
        VR_TM=0x4d54, // Time (character string)
        VR_UN=0x4e55,
        VR_UT=0x5455, // Unlimited text (character string)
        VR_SQ=0x5153, // Sequence of items
        VR_SH=0x4853, // Short string (character string with leading/trailing spaces).
        VR_FL=0x4c46, // Floating point single precision
        VR_SL=0x4c53, // Signed long, (signed 32bits, 2's complement form.)
        VR_AT=0x5441, // Attribute tag (ordered pair 16 bit unsigned)
        VR_UL=0x4c55, // Unsigned long (unsigned 32 bits)
        VR_US=0x5355, // Unsigned short (unsigned 16 bits)
        VR_SS=0x5353, // Signed short, (signed 16bits, 2's complement)
        VR_FD=0x4446  // Floating point double precision
    };

    DICOMFile*  GetDICOMFile()
    {
        return this->DataFile;
    }

    void GetGroupsElementsDatatypes(dicom_stl::vector<doublebyte>& groups,
        dicom_stl::vector<doublebyte>& elements,
        dicom_stl::vector<VRTypes>& datatypes);

private:
    // 保存到DICOMParserInfo
     void  DumgTag(doublebyte group, doublebyte elment,unsigned char* temdata,quadbyte length);
     //void DumpTag(dicom_stream::ostream& out, doublebyte group, doublebyte element, VRTypes vrtype, unsigned char* tempdata, quadbyte length);

    struct DICOMRecord 
    {
        doublebyte group;
        doublebyte element;
        VRTypes datatype;
    };
    /// 读取tag
    void ReadNextRecord(doublebyte& group, doublebyte& element, DICOMParser::VRTypes& mytype);

    void ReadNextRecord_ex(doublebyte& group, doublebyte& element,DICOMParser::VRTypes& mytype);

    void InitTypeMap();

    bool ByteSwap;

    bool ByteSwapData;

    dicom_stream::ofstream ParserOutputFile;

    DICOMFile* DataFile;

    dicom_stl::string FileName;

    DICOMBuffer*    buf_;

    DicomParserInfo  info;

    DICOMParserImplementation* Implementation;

    bool isExplicitVR;

    bool isLitteEndian;
    // 图像数据
    std::vector<char>   img_data_;
    // 字段数据
    std::vector<char>   tag_data_; 
    // 
    int                 rescale_intercept;
    int                 rescale_scope;
    /// 是否RGB图像
    std::string         photometric;
    /// RGB排列方式
    int                 rgbconf;
    /// 关联,存在多个相同的tag，只取(0x0008,0x1140)下的（0x0008,0x1155）
    bool                is_reference;

    bool                is_jpeg70;
    /* 保存jpeg解压数据 */
    std::vector<char>   jpeg_data_;
private:
    DICOMParser(const DICOMParser&);  
    void operator=(const DICOMParser&); 
};

#ifdef _MSC_VER
#pragma warning ( pop )
#endif
#endif