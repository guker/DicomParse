#include "DICOMParser.h"
#include "iDicom.h"
#include <fstream>


int IMRAPI IdicomInit(void* reserved)
{
    return 0;
}

int IMRAPI IdicomUninit()
{
    return 0;
}

int IMRAPI IdicomCreateInst(void** p_inst)
{
    DICOMParser* inst = new DICOMParser();
    *p_inst = inst;
    return 0;
}

int IMRAPI IdicomDestroyInst(void* inst)
{
    DICOMParser* sv_inst = (DICOMParser*)inst;
    delete sv_inst;
    return 0;
}

int IMRAPI IdicomReset(void* inst)
{
    DICOMParser* sv_inst = (DICOMParser*)inst;
    sv_inst->Reset();
    return 0;
}

int IMRAPI IdicomLoadFile(void* inst,const char* path)
{
    DICOMParser* sv_inst = (DICOMParser*)inst;
    sv_inst->OpenFile(path);
    sv_inst->ReadHeader();

    return 0;
}

int IMRAPI IdicomLoadBuf(void* inst,char* data,int size)
{
     DICOMParser* sv_inst = (DICOMParser*)inst;
     sv_inst->SetBuf(data,size);
     sv_inst->ReadHeader_ex();
     return 0;
}

int IMRAPI IdicomGetData(void* inst,char** data,int* width,int* height)
{
    DICOMParser* sv_inst = (DICOMParser*)inst;
    sv_inst->GetDicomData(data,width,height);
    return 0;
}


int IMRAPI IdicomParse(void* inst,DicomParserInfo* dicom_info)
{
    DICOMParser* sv_inst = (DICOMParser*)inst;
    sv_inst->GetDicomInfo(dicom_info);
    return 0; 
}


//int main(int argc,char* argv[])
//{
//    std::string filename = argv[1];
//    FILE* fp = fopen(filename.c_str(),"rb");
//    if(NULL == fp)
//    {
//        std::cout<<"cannot open dicom file" << std::endl;
//        return  -1;
//    }
//    fseek(fp,0,SEEK_END);
//    
//    int len = ftell(fp);
//    fseek(fp,0,SEEK_SET);
//    char* buffer = new char[len];
//    fread((void*)buffer,sizeof(char),len,fp);
//
//    IdicomInit(NULL);
//    void* inst;
//    IdicomCreateInst(&inst);
//    IdicomLoadBuf(inst,buffer,len);
//    DicomParserInfo info;
//    IdicomParse(inst,&info);
//
//    //IdicomInit(NULL);
//    //void* inst;
//    //IdicomCreateInst(&inst);
//    //IdicomLoadFile(inst,filename.c_str());
//
//    //DicomParserInfo info;
//    //IdicomParse(inst,&info);
//    char* data = NULL;
//    int   width = 0;
//    int   heigth = 0;
//    IdicomGetData(inst,&data,&width,&heigth);
//    short* p = (short*)data;
//    IdicomDestroyInst(inst);
//
//    return 0;
//}
