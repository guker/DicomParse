package com.iflytek.test;

import com.iflytek.dicom.*;

public class dicom_test {

	public static void main(String[] args) {
		
		int err = DicomParse.JniIdicomInit("F:\\DicomParse_JNI\\dicom_parse\\DicomParse.dll");
		System.out.println(err);
		
		DicomParse dp = new DicomParse();
		dp.JniIdicomCreateInst();
		
		dp.JniIdicomLoadFile("F:\\DicomParse-master\\DicomParse-master\\DicomParse\\1.2.840.113619.2.289.3.3233826508.595.1488291785.134.157.dcm");
		DicomInfo info = new DicomInfo();
		dp.JniIdicomParse(info);
		short[] data = new short[info.Width*info.Height];

		dp.JniIdicomGetData(data);
		dp.JniIdicomReset();
		dp.JniIdicomLoadFile("F:\\DicomParse-master\\DicomParse-master\\DicomParse\\1.2.840.113619.2.289.3.3233826508.595.1488291785.134.157.dcm");
		dp.JniIdicomParse(info);
        dp.JniIdicomGetData(data);
        dp.JniIdicomDestroyInst();
 //       dp.JniIdicomUninit();
	}

}
