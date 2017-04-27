#include "com_iflytek_dicom_DicomParse.h"
#include "DicomParse.h"

FILE* fp_log = NULL;

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomInit
    (JNIEnv * env , jclass obj, jstring jpath)
{
#if LOG_FILE
    fp_log = fopen("dicom_parse_log.txt","w");
#endif
    const char* path = env->GetStringUTFChars(jpath,0);
    int err = DicomParse::loadDll(path);
    if( 0 != err)
    {
        return err;
    }
    LOG("IdicomInit  enter");
    err = DicomParse::IdicomInit()(NULL);
    if(0 != err)
    {
        return err;
    }
    LOG("IdicomInit leave");

    env->ReleaseStringUTFChars(jpath,path);

    return err;
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomUninit
    (JNIEnv *env, jclass obj)
{
    LOG("IdicomUninit enter and leave");
#if LOG_FILE
    fclose(fp_log);
#endif
    return DicomParse::IdicomUninit()();
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomCreateInst
    (JNIEnv * env, jobject obj)
{
    LOG("IdicomCreateInst  enter");
    void*  inst;
    jclass cls = env->GetObjectClass(obj);
    jfieldID  fid = env->GetFieldID(cls,"mInst","J");
    
    int err = DicomParse::IdicomCreateInst()(&inst);
    LOG("inst %p",inst);
    if(NULL == inst)
    {
         return DICOM_JNI_ERROR_NULLPTR;
    }
    LOG("IdicomCreateInst leave");
    env->SetLongField(obj,fid,(jlong)inst);
    return 0;
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomDestroyInst
    (JNIEnv * env, jobject obj)
{
     LOG("IdicomDestroyInst  enter");
     jclass cls = env->GetObjectClass(obj);
     jfieldID fid = env->GetFieldID(cls,"mInst","J");

     void* inst = (void*)env->GetLongField(obj,fid);
     if(NULL == inst)
     {
         return DICOM_JNI_ERROR_NULLPTR;
     }

     int err = DicomParse::IdicomDestroyInst()(inst);
     if(0 != err)
     {
         LOG("DestroyInst failed");
         return err;
     }
     LOG("IdicomDestroyInst  leave");
     return err;
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomReset
    (JNIEnv * env, jobject obj)
{
    LOG("IdicomReset  enter");
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls,"mInst","J");

    void* inst = (void*)env->GetLongField(obj,fid);
    if( NULL == inst)
    {
        return DICOM_JNI_ERROR_NULLPTR;
    }
    int err = DicomParse::IdicomReset()(inst);
    if(0 != err)
    {
        LOG("IdicomReset failed");
        return err;
    }
    LOG("IdicomReset  leave");
    return err;
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomLoadFile
    (JNIEnv * env, jobject obj, jstring jpath)
{
    LOG("IdicomLoadFile  enter");
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls,"mInst","J");

    void* inst = (void*)env->GetLongField(obj,fid);

    if(NULL == inst)
    {
        return DICOM_JNI_ERROR_NULLPTR;
    }

    const char* path = env->GetStringUTFChars(jpath,0);

    int err = DicomParse::IdicomLoadFile()(inst,path);
    if( 0 != err)
    {
        LOG("IdicomLoadFile failed");
        return err;
    }
    LOG("IdicomLoadFile  leave");
    env->ReleaseStringUTFChars(jpath,path);

    return err;
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomLoadBuf
    (JNIEnv * env, jobject obj, jbyteArray jbuffer, jint size)
{
    LOG("IdicomLoadBuf enter");
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls,"mInst","J");

    void* inst = (void*)env->GetLongField(obj,fid);

    if(NULL == inst)
    {
        return DICOM_JNI_ERROR_NULLPTR;
    }
    
    jbyte* buffer = env->GetByteArrayElements(jbuffer,0);

    int err = DicomParse::IdicomLoadBuf()(inst,(char*)buffer,size);
    if(0 != err)
    {
        LOG("IdicomLoadBuf failed");
        return err;
    }
    LOG("IdicomLoadBuf leave");
    env->ReleaseByteArrayElements(jbuffer,buffer,0);
    return err;
}

JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomParse
    (JNIEnv *env, jobject obj, jobject jobj)
{
    LOG("IdicomParse enter");
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls,"mInst","J");

    void* inst = (void*)env->GetLongField(obj,fid);

    if(NULL == inst)
    {
        return DICOM_JNI_ERROR_NULLPTR;
    }
    DicomParserInfo info;
    int err = DicomParse::IdicomParse()(inst,&info);
    if( 0 != err)
    {
        LOG("IdicomParse failed");
        return err;
    }
    jclass jf = env->FindClass("com/iflytek/dicom/DicomInfo");


    jfieldID PatientName = env->GetFieldID(jf,"PatientName","[B");
    jfieldID PatientId   = env->GetFieldID(jf,"PatientId","[B");
    jfieldID StudyDate   = env->GetFieldID(jf,"StudyDate","[B");
    jfieldID StudyId     = env->GetFieldID(jf,"StudyId","[B");
    jfieldID ImageType   = env->GetFieldID(jf,"ImageType","[B");
    jfieldID RefSopId   = env->GetFieldID(jf,"RefSopId","[B");
    jfieldID SopId   = env->GetFieldID(jf,"SopId","[B");

    jfieldID WinCenter   = env->GetFieldID(jf,"WinCenter","I");
    jfieldID WinWidth   = env->GetFieldID(jf,"WinWidth","I");

    jfieldID SliceThikness   = env->GetFieldID(jf,"SliceThikness","F");
    jfieldID SliceLoc   = env->GetFieldID(jf,"SliceLoc","F");
    jfieldID PosX   = env->GetFieldID(jf,"PosX","F");
    jfieldID PosY   = env->GetFieldID(jf,"PosY","F");
    jfieldID PosZ   = env->GetFieldID(jf,"PosZ","F");
    jfieldID SpaceX   = env->GetFieldID(jf,"SpaceX","F");
    jfieldID SpaceY   = env->GetFieldID(jf,"SpaceY","F");

    jfieldID Width   = env->GetFieldID(jf,"Width","I");
    jfieldID Height   = env->GetFieldID(jf,"Height","I");

    jfieldID ImgOrientation   = env->GetFieldID(jf,"ImgOrientation","[F");

    env->SetIntField(jobj,WinCenter,info.WinCenter);
    env->SetIntField(jobj,WinWidth,info.WinWidth);
    env->SetIntField(jobj,Width,info.Width);
    env->SetIntField(jobj,Height,info.Height);
    env->SetFloatField(jobj,SliceThikness,info.SliceThikness);
    env->SetFloatField(jobj,SliceLoc,info.SliceLoc);
    env->SetFloatField(jobj,PosX,info.PosX);
    env->SetFloatField(jobj,PosY,info.PosY);
    env->SetFloatField(jobj,PosZ,info.PosZ);
    env->SetFloatField(jobj,SpaceX,info.SpaceX);
    env->SetFloatField(jobj,SpaceY,info.SpaceY);

    jfloatArray jorient = (jfloatArray)env->GetObjectField(jobj,ImgOrientation);
    int size = sizeof(info.ImgOrientation)/sizeof(info.ImgOrientation[0]);
    env->SetFloatArrayRegion(jorient,0,size,info.ImgOrientation);

    jbyteArray jPatientName = (jbyteArray)env->GetObjectField(jobj,PatientName);
    size = sizeof(info.PatientName)/sizeof(info.PatientName[0]);
    env->SetByteArrayRegion(jPatientName,0,size,(jbyte*)info.PatientName);

    jbyteArray jPatientId = (jbyteArray)env->GetObjectField(jobj,PatientId);
    size = sizeof(info.PatientId)/sizeof(info.PatientId[0]);
    env->SetByteArrayRegion(jPatientId,0,size,(jbyte*)info.PatientId);

    jbyteArray jStudyDate = (jbyteArray)env->GetObjectField(jobj,StudyDate);
    size = sizeof(info.StudyDate)/sizeof(info.StudyDate[0]);
    env->SetByteArrayRegion(jStudyDate,0,size,(jbyte*)info.StudyDate);

    jbyteArray jStudyId = (jbyteArray)env->GetObjectField(jobj,StudyId);
    size = sizeof(info.StudyId)/sizeof(info.StudyId[0]);
    env->SetByteArrayRegion(jStudyId,0,size,(jbyte*)info.StudyId);

    jbyteArray jImageType = (jbyteArray)env->GetObjectField(jobj,ImageType);
    size = sizeof(info.ImageType)/sizeof(info.ImageType[0]);
    env->SetByteArrayRegion(jImageType,0,size,(jbyte*)info.ImageType);

    jbyteArray jRefSopId = (jbyteArray)env->GetObjectField(jobj,RefSopId);
    size = sizeof(info.RefSopId)/sizeof(info.RefSopId[0]);
    env->SetByteArrayRegion(jRefSopId,0,size,(jbyte*)info.RefSopId);

    jbyteArray jSopId = (jbyteArray)env->GetObjectField(jobj,SopId);
    size = sizeof(info.SopId)/sizeof(info.SopId[0]);
    env->SetByteArrayRegion(jSopId,0,size,(jbyte*)info.SopId);
    LOG("IdicomParse leave");

    return 0;
}


 JNIEXPORT jint JNICALL Java_com_iflytek_dicom_DicomParse_JniIdicomGetData
    (JNIEnv * env, jobject obj, jshortArray jshortarr)
{
    LOG("IdicomGetData enter");
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls,"mInst","J");

    void* inst = (void*)env->GetLongField(obj,fid);

    if(NULL == inst)
    {
        return DICOM_JNI_ERROR_NULLPTR;
    }
    char* img_data = NULL;
    int width = 0;
    int height = 0;
    int err = DicomParse::IdicomGetData()(inst,(char**)&img_data,(int*)&width,(int*)&height);
    if(0 != err)
    {
        LOG("IdicomGetData failed");
        return err;
    }
    short* img = (short*)img_data;
    env->SetShortArrayRegion(jshortarr,0,width*height,img);
    LOG("IdicomGetData leave");
    return 0;
}