#ifndef __DICOMPARSE_H_
#define __DICOMPARSE_H_

#ifdef WIN32
#include <Windows.h>
#else
#include "dlfcn.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include "iDicom.h"
#include "common.h"

class DicomParse
{
public:
    static Proc_IdicomInit& IdicomInit()
    {
        static Proc_IdicomInit jz;
        return jz;
    }

    static Proc_IdicomUninit& IdicomUninit()
    {
        static Proc_IdicomUninit jz;
        return jz;
    }

    static Proc_IdicomCreateInst& IdicomCreateInst()
    {
        static Proc_IdicomCreateInst jz;
        return jz;
    }

    static Proc_IdicomDestroyInst& IdicomDestroyInst()
    {
        static Proc_IdicomDestroyInst jz;
        return jz;
    }

    static Proc_IdicomReset& IdicomReset()
    {
        static Proc_IdicomReset jz;
        return jz;
    }

    static Proc_IdicomLoadFile& IdicomLoadFile()
    {
        static Proc_IdicomLoadFile jz;
        return jz;
    }
    
    static Proc_IdicomLoadBuf& IdicomLoadBuf()
    {
        static Proc_IdicomLoadBuf jz;
        return jz;
    }

    static Proc_IdicomGetData& IdicomGetData()
    {
        static Proc_IdicomGetData jz;
        return jz;
    }
    
    static Proc_IdicomParse& IdicomParse()
    {
        static Proc_IdicomParse jz;
        return jz;
    }

#ifdef WIN32

#define LOADMETHOD(var) do \
{ \
    var##() = (Proc_ ## var)GetProcAddress(hDll, #var); \
    if( NULL == var##() ) { \
    LOG("load method error : %s",#var);  \
    return DICOM_JNI_ERROR_GET_METHOD; \
    } \
}while(0);

#else

#define LOADMETHOD(var) do \
{ \
    var() = (Proc_##var)dlsym(hDll,#var);   \
    pszErr = dlerror(); \
    if( NULL != pszErr) { \
    LOG("Load method error : %s",#var);    \
    return DICOM_JNI_ERROR_GET_METHOD; \
    } \
}while(0);

#endif

    static int loadDll(const char* path)
    {
#ifdef WIN32
        HINSTANCE hDll = LoadLibraryA(path);
#else
        void* hDll = dlopen(path,RTLD_LAZY);
        char* pszErr = NULL;
#endif
        if( NULL == hDll)
        {
            LOG("load module error : %s",path);
            return DICOM_JNI_ERROR_OPENMODULE;
        }
        LOADMETHOD(IdicomInit);
        LOADMETHOD(IdicomUninit);
        LOADMETHOD(IdicomCreateInst);
        LOADMETHOD(IdicomDestroyInst);
        LOADMETHOD(IdicomReset);
        LOADMETHOD(IdicomLoadFile);
        LOADMETHOD(IdicomLoadBuf);
        LOADMETHOD(IdicomGetData);
        LOADMETHOD(IdicomParse);
        return 0;
    }
};

#endif