//#include <iostream>
//#include "DICOMParser.h"
//#include <fstream>
//#include <time.h>
//
//int main(int argc,char* argv[])
//{
//    DICOMParser* dp = new DICOMParser();
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
//    dp->SetBuf(buffer,len);
//    dp->ReadHeader_ex();
//    DicomParserInfo info;
//    dp->GetDicomInfo(&info);
//    std::cout<< info.Height<< std::endl;



    //// ²âÊÔÐ§ÂÊ
    //FILE* fp = fopen("E:\\medical\\DicomParse\\DicomParse\\1.2.840.113619.2.80.2804147847.10246.1484368108.11.dcm","rb");
    //if(NULL == fp)
    //{
    //    std::cout<< "cannot open dicom file "  << std::endl;
    //}
    //fseek(fp,0,SEEK_END);
    //
    //int len = ftell(fp);
    //fseek(fp,0,SEEK_SET);
    //char* buffer = new char[len];
    //fread((void*)buffer,sizeof(char),len,fp);
    //clock_t s = clock();
    //int cnt = 10000;
    //while(cnt > 0)
    //{

    //   dp->Reset();

    //   dp->SetBuf(buffer,len);
    //   dp->ReadHeader_ex();
    //   cnt--;
    //}
    //clock_t e = clock();
    //std::cout<< (e-s)*1.0/10000<<std::endl;
    //DicomParserInfo info;
    //dp->GetDicomInfo(&info);
    //int w =0 ;
    //int h = 0; 
    //char* data = NULL;
    //dp->GetDicomData(&data,&w,&h);

    //bool is_open = dp->OpenFile("E:\\medical\\DicomParse\\DicomParse\\1.2.840.113619.2.289.3.3233826508.595.1488291785.134.157.dcm");
    //if(!is_open)
    //{
    //    std::cout << " Canot open dicom file" << std::endl;
    //}
    //dp->ReadHeader();
    //DicomParserInfo info;
    //dp->GetDicomInfo(&info);
    //int w =0 ;
    //int h = 0; 
    //char* data = NULL;
    //dp->GetDicomData(&data,&w,&h);
//    return 0;
//}