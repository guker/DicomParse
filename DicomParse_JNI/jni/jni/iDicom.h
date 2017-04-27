#ifndef ___I_DICOM_H___
#define ___I_DICOM_H___


/**
* DICOM API type
*/
#if defined (_MSC_VER)                  /* Microsoft Visual C++ */
#if !defined(IMRAPI)
#define IMRAPI __stdcall
#endif
#pragma pack(push, 8)

#elif defined (__BORLANDC__)          /* Borland C++ */
#if !defined(IMRAPI)
#define IMRAPI __stdcall
#endif
#pragma option -a8

#elif defined(__WATCOMC__)           /* Watcom C++ */
#if !defined(IMRAPI)
#define IMRAPI __stdcall
#endif
#pragma pack(push, 8)

#else                               /* Linux and Unix*/
#if !defined(IMRAPI)
#define IMRAPI
#endif
#endif

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

    int IMRAPI IdicomInit(void* reserved);
    typedef int (IMRAPI *Proc_IdicomInit)(void* reserved);

    int IMRAPI IdicomUninit();
    typedef int (IMRAPI *Proc_IdicomUninit)(); 

    int IMRAPI IdicomCreateInst(void** p_inst);
    typedef int (IMRAPI *Proc_IdicomCreateInst)(void** p_inst);

    int IMRAPI IdicomDestroyInst(void* inst);
    typedef int (IMRAPI *Proc_IdicomDestroyInst)(void* inst);
    
    int IMRAPI IdicomReset(void* inst);
    typedef int (IMRAPI *Proc_IdicomReset)(void* inst);

    int IMRAPI IdicomLoadFile(void* inst,const char* path);
    typedef int (IMRAPI *Proc_IdicomLoadFile)(void* inst,const char* path);
    
    int IMRAPI IdicomLoadBuf(void* inst, char* data,int size);
    typedef int (IMRAPI *Proc_IdicomLoadBuf)(void* inst,char* data,int size);

    int IMRAPI IdicomGetData(void* inst,char** data,int* width,int* heigth);
    typedef int (IMRAPI *Proc_IdicomGetData)(void* inst, char** data,int* width,int* heigth);

    int IMRAPI IdicomParse(void* inst,DicomParserInfo* dicom_info);
    typedef int (IMRAPI *Proc_IdicomParse)(void* inst,DicomParserInfo* dicom_info);


#ifdef __cplusplus
};
#endif

/* reset the structure packing alignments for different compiler */
#if defined(_MSC_VER)                    /* Microsoft Visual C++ */
#pragma pack(pop);                       
#elif defined(__BORLANDC__)               /* Borland C++ */
#pragma option -a.
#elif defined(__WATCOMC__)               /* Watcom C++ */
#pragma pack(pop)
#endif

#endif