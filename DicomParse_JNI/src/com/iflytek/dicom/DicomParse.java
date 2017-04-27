package com.iflytek.dicom;
import  com.iflytek.dicom.DicomInfo;

public class DicomParse {
	
	public long       mInst = 0;
	
	
	static {
		System.loadLibrary("jni");
	}
	
	public native static int JniIdicomInit(String path);
	
	public native static int JniIdicomUninit();
	
	public native int JniIdicomCreateInst();
	
	public native int JniIdicomDestroyInst();
	
	public native int JniIdicomReset();
	
	public native int JniIdicomLoadFile(String path);
	
	public native int JniIdicomLoadBuf(byte[] data, int size);
	
	public native int JniIdicomParse(DicomInfo info);
	
	public native int JniIdicomGetData(short[] data);
}
