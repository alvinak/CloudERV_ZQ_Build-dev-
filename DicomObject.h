#pragma once

#include "dcmtk/dcmdata/dcdeftag.h"      /* for DCM_StudyInstanceUID */

#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimage/diregist.h"  /* include to support color images */
#include "dcmtk/dcmdata/dcdatset.h"

#include "dcmtk/dcmdata/dcfilefo.h"

#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */


#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpeg/ddpiimpl.h"

#include "dcmtk/dcmjpeg/djrplol.h"   /* for DJ_RPLossless */
#include "dcmtk/dcmjpeg/djrploss.h"  /* for DJ_RPLossy */

#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */


class CObListEx : public CObList
{
public:
	CObListEx(int nBlockSize = 10) : CObList(nBlockSize) { }

	void Sort(int(*CompareFunc)(CObject* pFirstObj, CObject*pSecondObj));
	void Sort(POSITION posStart, int iElements, int (*CompareFunc)(CObject* pFirstObj, CObject* pSecondObj));
};


class CDicomObject;

template< class TYPE >
class CTypedObListEx : public CObListEx
{
public:
// Construction
	CTypedObListEx(int nBlockSize = 10) : CObListEx(nBlockSize) { }

	// peek at head or tail
	TYPE& GetHead()
		{ return (TYPE&)CObListEx::GetHead(); }
	TYPE GetHead() const
		{ return (TYPE)CObListEx::GetHead(); }
	TYPE& GetTail()
		{ return (TYPE&)CObListEx::GetTail(); }
	TYPE GetTail() const
		{ return (TYPE)CObListEx::GetTail(); }

	// get head or tail (and remove it) - don't call on empty list!
	TYPE RemoveHead()
		{ return (TYPE)CObListEx::RemoveHead(); }
	TYPE RemoveTail()
		{ return (TYPE)CObListEx::RemoveTail(); }

	// add before head or after tail
	POSITION AddHead(TYPE newElement)
		{ return CObListEx::AddHead(newElement); }
	POSITION AddTail(TYPE newElement)
		{ return CObListEx::AddTail(newElement); }

	// add another list of elements before head or after tail
	void AddHead(CTypedObListEx< TYPE >* pNewList)
		{ CObListEx::AddHead(pNewList); }
	void AddTail(CTypedObListEx< TYPE >* pNewList)
		{ CObListEx::AddTail(pNewList); }

	// iteration
	TYPE& GetNext(POSITION& rPosition)
		{ return (TYPE&)CObListEx::GetNext(rPosition); }
	TYPE GetNext(POSITION& rPosition) const
		{ return (TYPE)CObListEx::GetNext(rPosition); }
	TYPE& GetPrev(POSITION& rPosition)
		{ return (TYPE&)CObListEx::GetPrev(rPosition); }
	TYPE GetPrev(POSITION& rPosition) const
		{ return (TYPE)CObListEx::GetPrev(rPosition); }

	// getting/modifying an element at a given position
	TYPE& GetAt(POSITION position)
		{ return (TYPE&)CObListEx::GetAt(position); }
	TYPE GetAt(POSITION position) const
		{ return (TYPE)CObListEx::GetAt(position); }
	void SetAt(POSITION pos, TYPE newElement)
		{ CObListEx::SetAt(pos, newElement); }

	void Sort( int(*CompareFunc)(TYPE pFirstObj, TYPE pSecondObj) )
		{ CObListEx::Sort((int(*)(CObject*,CObject*))CompareFunc); }
	void Sort( POSITION posStart, int iElements, int(*CompareFunc)(TYPE pFirstObj, TYPE pSecondObj) )
		{ CObListEx::Sort(posStart, iElements, (int(*)(CObject*,CObject*))CompareFunc); }
};

class CSeriesObject : public CObject
{
public:
	CSeriesObject();
	~CSeriesObject();
public:

	CString m_sSystemId;
	CString m_sStudyId;
	CString m_sPatientId;
	CString m_sPatientName;
	CString m_sSex;
	CString m_sBirthday;
	CString m_sAge;
	CString m_sModality;
	CString m_sSeriesNo;
	CString m_sSeriesUID;
	CString m_sStudyDate;
	CString m_sStudyTime;
	CString m_sStudyUID;
	CString m_sFieldsNameList;
	CString m_sSequenceName;
	CString m_sSeriesDescription;
	BOOL	m_bCurrentSeries;
	BOOL	m_bFromRIS;
	BOOL	m_bFromExtFile;

	CString	m_sPatientName_CN;	// 中文姓名

	void SetCurrentIndex(int nIndex) { m_nCurrentIndex = nIndex;}
	int GetCurrentIndex() { return m_nCurrentIndex;}
	int GetDicomObjectCount() { return m_DicomObjectList.GetCount();}
	
	int m_nCurrentIndex;

	DWORD	m_dwThreadId,m_dwTickCount;

	BOOL CanUnite(const TCHAR *sStudyUID,const TCHAR *sModality,const TCHAR *sPatientId,const TCHAR *sName,const TCHAR *sStudyDate);

	BOOL CloneFrom(CSeriesObject *pSeriesObj);

	//CObList m_DicomObjectList;
	CTypedObListEx < CDicomObject* > m_DicomObjectList;

	
	void Sort();

public:
	

	const TCHAR *GetPatientId() { return m_sPatientId;}

	void SaveBitmapToBuf(int nIdx,unsigned char *&pBMIH,int &nBMIHLen,unsigned char *&pBits,int &nBitsLen);

	BOOL IsFromRIS() { return m_bFromRIS;}
	BOOL IsFromExtFile() { return m_bFromExtFile; }
	void SetFromExtFile(BOOL bValue) { m_bFromExtFile = bValue; }
	
	void SetFieldsNameList(const char *sList) { m_sFieldsNameList = sList;};
	const TCHAR *GetFieldsNameList() { return m_sFieldsNameList;};
};




class CDicomObject : public CObject
{

public:
	CDicomObject(const TCHAR *lpszFileName);
	CDicomObject(DcmDataset *old,BOOL bUseIt);
	~CDicomObject();

	DcmDataset * GetDicomDataset() {return m_pDicomDataset;}
	
	void GetWinLevel(double &fCen,double &fWid);
	void SetWinLevel(double fCen,double fWid);

	int GetImageWidth() { return m_nImageWidth;}
	int GetImageHeight() { return m_nImageHeight;}
	
	int GetColorDepth() { return m_nColorDepth;}
	int GetColorTableEntries() { return m_nColorTableEntries;}
	int GetImageSize() { return m_nImageSize;}
	int GetScanLineBytes() { return m_nScanLineBytes;}
	int GetBytesPerPixel() { return m_nBytesPerPixel;}

	int GetBitsStored();

	void GetWinLevelRange(int &nMinWidth,int &nMaxWidth,int &nMinCenter,int &nMaxCenter);

	double GetRescaleSlope();
	double GetRescaleIntercept();

	void GetCalibrateValue(double &fx,double &fy);
	void GetCalibrateValueA(double &fx,double &fy);
	void SetCalibrateValueM(double fx,double fy);
	

	BOOL HaveCTValue() { return m_bHaveCTValue;}

	void Select(BOOL bSelected) { m_bSelected = bSelected;}

	BOOL IsSelected() { return m_bSelected;}

	BOOL IsMultiFrame() { return m_nFrameCount > 1;}

	BOOL IsLocalizer();

	int GetFrameIndex() { return m_nFrameIndex;}
	int GetFrameCount() { return m_nFrameCount;}

	void Cine(int nMode);

	void CineMove(int nStep);

	void SaveFile(const TCHAR *lpszPath);
	void SaveFileEx(const TCHAR *lpszPath);
	void ExportToJpegFile(const TCHAR *lpszPath,const TCHAR *lpszFileName=NULL);

	void SaveBitmapToBuf(unsigned char *&lpBMIH,int &nBMIHLen,unsigned char *&lpBits,int &nBitsLen);
	
	void Print(std::ostream *pos);

	void Reset();

public:
	CString GetPatientAge();
	CString GetModality();
	CString GetPhotometricInterpretation() {return m_sPhotometricInterpretation;}
	CString GetOrientationText(int nIdx,BOOL bHFlip,BOOL bVFlip,int nRotateDegree);
		
	double GetPixelAspectRatio() { return m_fPixelAspectRatio;}

	int GetInstanceNumber();

	CString GetPatientId();
	CString GetPatientName();
	CString GetStudyId();
	CString GetSeriesNumber();
	CString GetSeriesInstanceUID();
	CString GetInstanceUID();

	CString GetFrameOfReferenceUID();

	void GetImageOrientationPatient(double &fAx,double &fAy,double &fAz,double &fBx,double &fBy,double &fBz);

	void GetImagePositionPatient(double &fx,double &fy,double &fz);


	CString GetString(const DcmTagKey & tagKey);
	int GetSint32(const DcmTagKey &tagKey,int nPos = 0);
	double GetFloat64(const DcmTagKey &tagKey,int nPos = 0);
	int GetUint16(const DcmTagKey &tagkey,int nPos = 0);

	void SetFromRIS(BOOL bValue) { m_bFromRIS = bValue;};
	BOOL GetFromRIS() { return m_bFromRIS;}
	BOOL IsFromRIS() { return m_bFromRIS;}


private:
	CString GetOrientation(double x,double y,double z);
	CString GetOppositeOrientation(TCHAR *s);

private:

	DcmDataset *m_pDicomDataset;
	BOOL m_bHaveCTValue;
	CString m_sModality,m_sPhotometricInterpretation;

	double m_fPixelAspectRatio;

	double m_fCalibrateValueX_A,m_fCalibrateValueY_A;
	double m_fCalibrateValueX_M,m_fCalibrateValueY_M;

	unsigned short  m_nImageWidth,m_nImageHeight;
	int m_nColorDepth;
	
	long  m_nFrameCount;
	int m_nFrameIndex;
	int m_nColorTableEntries;
	int m_nImageSize;
	int m_nScanLineBytes;

	int m_nBytesPerPixel;
	int m_nPlayMode;

	BOOL m_bSelected;

	BOOL m_bFromRIS;
	
	TCHAR m_szOrientation[4][11];

	double m_fWindowWidth,m_fWindowCenter;

	//////////////////////////////////////////


	/////////////////////////////////////////
	//下列属性和方法用于排序 CObListEx
	
public:
	static int Compare(CDicomObject* pFirstObj, CDicomObject* pSecondObj)
    {
		return pFirstObj->GetInstanceNumber() - pSecondObj->GetInstanceNumber();
    }
};
