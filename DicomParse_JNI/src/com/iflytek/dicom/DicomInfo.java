package com.iflytek.dicom;


public class DicomInfo {
	
	public byte[]    PatientName;
	public byte[]    PatientId;
	public byte[]    StudyDate;
	public byte[]    StudyId;
	public byte[]    ImageType;
	public byte[]    RefSopId;
	public byte[]    SopId;
	
    public int       WinCenter;
    public int       WinWidth;
    
    public float     SliceThikness;
    public float     SliceLoc;
    
    public float     PosX;
    public float     PosY;
    public float     PosZ;
    
    public float     SpaceX;
    public float     SpaceY;
    
    public int       Width;
    public int       Height;
    
    public float[]     ImgOrientation;
    
    public DicomInfo(){
    	PatientName    = new byte[50];
    	PatientId      = new byte[20];
    	StudyDate      = new byte[20];
    	StudyId        = new byte[20];
    	ImageType      = new byte[50];
    	RefSopId       = new byte[128];
    	SopId          = new byte[128];
    	
    	ImgOrientation = new float[6];
    	
    }
}
