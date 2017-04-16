#ifndef __DICOM_CONFIG_H_
#define __DICOM_CONFIG_H_

//
// BEGIN Toolkit (ITK,VTK, etc) specific
// 
#ifdef vtkDICOMParser_EXPORTS
#define DICOM_EXPORT_SYMBOLS
#endif

#ifdef ITKDICOMParser_EXPORTS
#define DICOM_EXPORT_SYMBOLS
#endif

#ifdef DICOM_NO_STD_NAMESPACE
#define dicom_stl
#else
#define dicom_stl std
#endif

#ifdef DICOM_ANSI_STDLIB
#define dicom_stream std

#include <iostream>
#include <fstream>
#include <iomanip>
#else
#define dicom_stream 

#include <iostream>
#include <fstream>
#include <iomanip>
#endif

#ifdef DICOM_DLL
#ifdef DICOM_EXPORT_SYMBOLS
#define DICOM_EXPORT __declspec(dllexport)
#define DICOM_EXPIMP_TEMPLATE 
#else
#define DICOM_EXPORT __declspec(dllimport)
#define DICOM_EXPIMP_TEMPLATE extern
#endif
#else
#define DICOM_EXPORT 
#endif

#endif // __DICOM_CONFIG_H_
