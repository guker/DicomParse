#ifndef  _COMMON_H_
#define  _COMMON_H_

extern FILE* fp_log;

#define  LOG_FILE  (1)

#if (LOG_FILE)
#define LOG  LOG_OUTPUT_FILE
#else
#define LOG
#endif 

#define  DICOM_JNI_ERROR                (4000)
#define  DICOM_JNI_ERROR_INVALIDPARA    (4001)
#define  DICOM_JNI_ERROR_NULLPTR        (4002)
#define  DICOM_JNI_ERROR_OPENMODULE     (4003)
#define  DICOM_JNI_ERROR_GET_METHOD     (4004)

void LOG_OUTPUT_FILE(const char* pFormat,...)
{
    if( NULL == pFormat)
    {
        return;
    }

    va_list va;
    va_start(va,pFormat);
    const int BUFLEN = 256;
    char buf[BUFLEN + 1] = {0};
    vsnprintf(buf,BUFLEN,pFormat,va);
    fprintf(fp_log,"[JNI  %s]\n",buf);
    fflush(fp_log);
    va_end(va);
}

void LOG_OUTPUT(const char* pFormat,...)
{
    if( NULL == pFormat)
    {
        return ;
    }
    va_list va;
    va_start(va,pFormat);
    const int BUFLEN = 256;
    char buf[BUFLEN + 1] = {0};
    vsnprintf(buf,BUFLEN,pFormat,va);
    printf("[JNI  %s]\n",buf);
    va_end(va);
}



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
    float       PosZ;

    float       SpaceX;
    float       SpaceY;

    int         Width;
    int         Height;

    float       ImgOrientation[6];

}DicomParserInfo;
#endif