
#include "stdafx.h"
#include "DicomObject.h"



void CObListEx::Sort(int (*CompareFunc)(CObject* pFirstObj, CObject* pSecondObj))
{
	// CompareFunc is expected to return a positive integer if pFirstObj
	// should follow pSecondObj (is greater than)

	// Uses Insertion Sort

	// The Shell Sort is much faster than a straight insertion sort, however, it cannot
	//  be performed on a linked list (it COULD, but the resulting code would probably be
	//  much slower as a Shell Sort jumps all around the reletive positions of elements).

	// An Insertion Sort works by evaluating an item, if that item should
	// precede the item in front of it, than it shifts all the items that
	// should follow that item up one place until it finds the correct position
	// for the item, whereby it then 'inserts' that item.

	ASSERT_VALID(this);

	// If the list contains no items, the HEAD position will be NULL
	if (m_pNodeHead == NULL)
		return;

	CObject *pOtemp;
	CObList::CNode *pNi,*pNj;

	// Walk the list
	for (pNi = m_pNodeHead->pNext; pNi != NULL; pNi = pNi->pNext)
	{
		// Save data pointer
		pOtemp = pNi->data;

		// Walk the list backwards from pNi to the beginning of the list or until
		// the CompareFunc() determines that this item is in it's correct position
		// shifting all items upwards as it goes
		for (pNj = pNi; pNj->pPrev != NULL && CompareFunc(pNj->pPrev->data,pOtemp) > 0; pNj = pNj->pPrev)
			pNj->data = pNj->pPrev->data;

		// Insert data pointer into it's proper position
		pNj->data = pOtemp;
	}

}

void CObListEx::Sort(POSITION posStart, int iElements, int (*CompareFunc)(CObject* pFirstObj, CObject* pSecondObj))
{
	// This variation allows you to sort only a portion of the list

	// iElements can be larger than the number of remaining elements without harm
	// iElements can be -1 which will always sort to the end of the list

	ASSERT_VALID(this);
	ASSERT( AfxIsValidAddress((CObList::CNode*)posStart, sizeof(CObList::CNode)) );

	// Make certain posStart is a position value obtained by a GetHeadPosition or Find member function call
	//  as there is no way to test whether or not posStart is a valid CNode pointer from this list.
	// Ok, there is one way, we could walk the entire list and verify that posStart is in the chain, but even
	//  for debug builds that's a bit much.

	// If the list contains no items, the HEAD position will be NULL
	if (m_pNodeHead == NULL)
		return;

	CObject *pOtemp;
	CObList::CNode *pNi,*pNj;

	// Walk the list
	for (pNi = (CObList::CNode*)posStart; pNi != NULL && iElements != 0; pNi = pNi->pNext, iElements--)
	{
		// Save data pointer
		pOtemp = pNi->data;

		// Walk the list backwards from pNi to the beginning of the sort or until
		// the CompareFunc() determines that this item is in it's correct position
		// shifting all items upwards as it goes
		for (pNj = pNi; pNj->pPrev != NULL && pNj->pPrev != ((CObList::CNode*)posStart)->pPrev && CompareFunc(pNj->pPrev->data,pOtemp) > 0; pNj = pNj->pPrev)
			pNj->data = pNj->pPrev->data;

		// Insert data pointer into it's proper position
		pNj->data = pOtemp;
	}

}


CSeriesObject::CSeriesObject()
{

	m_sSystemId = _T("");
	m_sStudyId = _T("");
	m_sPatientId = _T("");
	m_sPatientName = _T("");
	m_sSex = _T("");
	m_sBirthday = _T("");
	m_sAge = _T("");
	m_sModality = _T("");
	m_sSeriesNo = _T("");
	m_sSeriesUID = _T("");
	m_sStudyDate = _T("");
	m_sStudyTime = _T("");
	m_sStudyUID = _T("");
	m_sFieldsNameList = _T("");

	m_sSequenceName = _T("");
	m_sSeriesDescription = _T("");

	m_bCurrentSeries = FALSE;
	m_bFromRIS = FALSE;
	m_bFromExtFile = FALSE;

	m_sPatientName_CN = _T("");
	
	m_nCurrentIndex = 0;
}


BOOL CSeriesObject::CloneFrom(CSeriesObject *pSeriesObj)
{
	CDicomObject *pDicomObj, *pNewDicomObj;
	POSITION pos;

	if (pSeriesObj == NULL)
		return FALSE;

	while (m_DicomObjectList.GetHeadPosition() != NULL) {
		pDicomObj = dynamic_cast <CDicomObject *> (m_DicomObjectList.RemoveHead());
		if (pDicomObj != NULL) {
			delete pDicomObj;
			pDicomObj = NULL;
		}
	}

	// 
	m_sSystemId = pSeriesObj->m_sSystemId;
	m_sStudyId = pSeriesObj->m_sStudyId;
	m_sPatientId = pSeriesObj->m_sPatientId;
	m_sPatientName = pSeriesObj->m_sPatientName;
	m_sSex = pSeriesObj->m_sSex;
	m_sBirthday = pSeriesObj->m_sBirthday;
	m_sAge = pSeriesObj->m_sAge;
	m_sModality = pSeriesObj->m_sModality;
	m_sSeriesNo = pSeriesObj->m_sSeriesNo;
	m_sSeriesUID = pSeriesObj->m_sSeriesUID;
	m_sStudyDate = pSeriesObj->m_sStudyDate;
	m_sStudyTime = pSeriesObj->m_sStudyTime;
	m_sStudyUID = pSeriesObj->m_sStudyUID;
	m_sFieldsNameList = pSeriesObj->m_sFieldsNameList;

	m_sSequenceName = pSeriesObj->m_sSequenceName;
	m_sSeriesDescription = pSeriesObj->m_sSeriesDescription;

	m_bCurrentSeries = FALSE;
	m_bFromRIS = pSeriesObj->m_bFromRIS;
	m_bFromExtFile = pSeriesObj->m_bFromExtFile;

	m_sPatientName_CN = pSeriesObj->m_sPatientName_CN;
	
	m_nCurrentIndex = 0;

	pos = pSeriesObj->m_DicomObjectList.GetHeadPosition();
	while (pos != NULL) {
		pDicomObj = pSeriesObj->m_DicomObjectList.GetNext(pos);
		if (pDicomObj != NULL) {
			pNewDicomObj = new CDicomObject(pDicomObj->GetDicomDataset(), false);
			if (pNewDicomObj) 
				m_DicomObjectList.AddTail(pNewDicomObj);
		}
	}

	return TRUE;
}



CSeriesObject::~CSeriesObject()
{
	CDicomObject *pObj;

	while (m_DicomObjectList.GetHeadPosition() != NULL)
	{
		pObj = (CDicomObject *) m_DicomObjectList.RemoveHead();

		delete pObj;

		pObj = NULL;
	}	

}


BOOL CSeriesObject::CanUnite(const TCHAR *sStudyUID,const TCHAR *sModality,const TCHAR *sPatientId,const TCHAR *sName,const TCHAR *sStudyDate)
{
	BOOL bRet = FALSE;

	if (m_sStudyUID.CompareNoCase(sStudyUID) == 0 &&  m_sModality.CompareNoCase(sModality) == 0 && m_sModality.CompareNoCase(_T("CR")) == 0)
		bRet = TRUE;
	else if (m_sPatientId.CompareNoCase(sPatientId) == 0 && m_sPatientName.CompareNoCase(sName) == 0 && m_sStudyDate.CompareNoCase(sStudyDate) == 0 && m_sModality.CompareNoCase(sModality) == 0 && m_sModality.CompareNoCase(_T("CR")) == 0)
		bRet = TRUE;

	return bRet;
}


void CSeriesObject::Sort()
{
	m_DicomObjectList.Sort(CDicomObject::Compare);
}


void CSeriesObject::SaveBitmapToBuf(int nIdx,unsigned char *&pBMIH,int &nBMIHLen,unsigned char *&pBits,int &nBitsLen)
{
	CDicomObject *pDicomObject;
	POSITION pos;

	if (this->m_DicomObjectList.GetCount() == 0) return;
	

	pos = m_DicomObjectList.GetHeadPosition();
	pDicomObject = (CDicomObject *) m_DicomObjectList.GetAt(pos);

	pDicomObject->SaveBitmapToBuf(pBMIH,nBMIHLen,pBits,nBitsLen);
}



CDicomObject::CDicomObject(const TCHAR *lpszName)
{
	OFCondition status = EC_Normal;
	unsigned short nBitsAllocated,nSamplesPerPixel;
	DcmFileFormat dfile;
	double vo[6];
	int ni;
	long nWidthRatio,nHeightRatio;
	const char *lpszValue = NULL;
	OFString photometric;
	CString sErrMsg;
	
	m_nPlayMode = 0;

	m_nImageWidth = 0;
	m_nImageHeight = 0;

	memset(m_szOrientation,0,sizeof(m_szOrientation));

	status = dfile.loadFile(CStringA(lpszName), EXS_Unknown, EGL_withoutGL, DCM_MaxReadLength, ERM_autoDetect);
	
	if (status != EC_Normal)
	{
		sErrMsg.Format(_T("装入文件%s失败:\n%s"),lpszName,CString(dcmErrorConditionToString(status)));
		AfxMessageBox(sErrMsg);
	}
	
	m_fCalibrateValueX_M = 0.0;
	m_fCalibrateValueY_M = 0.0;

	m_fCalibrateValueX_A = 0.0;
	m_fCalibrateValueY_A = 0.0;

	m_pDicomDataset = new DcmDataset(*dfile.getDataset());

	m_pDicomDataset->findAndGetString(DCM_Modality,lpszValue);
	m_sModality = lpszValue;

	m_bHaveCTValue = (m_sModality.CompareNoCase(_T("CT")) == 0);

	m_pDicomDataset->findAndGetUint16(DCM_Columns,m_nImageWidth);
	m_pDicomDataset->findAndGetUint16(DCM_Rows,m_nImageHeight);
	m_pDicomDataset->findAndGetUint16(DCM_BitsAllocated,nBitsAllocated);
	m_pDicomDataset->findAndGetUint16(DCM_SamplesPerPixel,nSamplesPerPixel);
	m_pDicomDataset->findAndGetSint32(DCM_NumberOfFrames, m_nFrameCount);

	if (m_nFrameCount <= 0) m_nFrameCount = 1;

	m_fCalibrateValueX_A = GetFloat64(DCM_PixelSpacing,0);
	m_fCalibrateValueY_A = GetFloat64(DCM_PixelSpacing,1);

	if (m_fCalibrateValueX_A == 0.0)
	{
		m_fCalibrateValueX_A = GetFloat64(DCM_ImagerPixelSpacing,0);
	}

	if (m_fCalibrateValueY_A == 0.0)
	{
		m_fCalibrateValueY_A = GetFloat64(DCM_ImagerPixelSpacing,1);
	}

	nWidthRatio = 0;
	nHeightRatio = 0;

	m_pDicomDataset->findAndGetSint32(DCM_PixelAspectRatio,nWidthRatio,0);
	m_pDicomDataset->findAndGetSint32(DCM_PixelAspectRatio,nHeightRatio,1);


	if (nWidthRatio > 0 && nHeightRatio > 0)
	{
		if (nWidthRatio > nHeightRatio)
		{
			m_fPixelAspectRatio = (float) nHeightRatio / nWidthRatio;
		}
		else
		{
			m_fPixelAspectRatio = (float) nWidthRatio / nHeightRatio;
		}
	}
	else
	{
		m_fPixelAspectRatio = 1.0;
	}

	m_sPhotometricInterpretation = _T("");

    if ((m_pDicomDataset->findAndGetOFString(DCM_PhotometricInterpretation, photometric)).good() && (photometric.length() > 0))
    {
      const char *c = photometric.c_str(); // guaranteed to be zero-terminated
      char cp[17]; // legal CS cannot be larger than 16 characters plus 0 byte
      int i=0; // current character
      while (*c && (i<16))
      {
        if (isalpha(*c)) cp[i++] = toupper(*c);
        else if (isdigit(*c)) cp[i++] = *c;
        c++;
      }
      cp[i] = 0; // write terminating zero
      photometric = cp; // copy back into OFString
	  m_sPhotometricInterpretation = photometric.c_str();
	}

	////////////////////////////////////////////////////////
	// 有些公司的RGB图像缺失(0028,0006)属性,导致DicomImage->createWindowsDIB函数失败, 因此,运行时临时增加此属性
	unsigned short nPlanarConfig = 0;
	if ((m_sPhotometricInterpretation.CompareNoCase(_T("RGB")) == 0) && (m_pDicomDataset->findAndGetUint16(DCM_PlanarConfiguration, nPlanarConfig)).bad()) {
		m_pDicomDataset->putAndInsertUint16(DCM_PlanarConfiguration, 0);
	}
	///////////////////////////////////////////////////////

	for (ni = 0; ni < 6; ni ++)
	{
		m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,vo[ni],ni);
	}

	_tcscpy(m_szOrientation[0],GetOrientation(vo[0],vo[1],vo[2]));
	_tcscpy(m_szOrientation[1],GetOrientation(vo[3],vo[4],vo[5]));

	_tcscpy(m_szOrientation[2],GetOppositeOrientation(m_szOrientation[0]));
	_tcscpy(m_szOrientation[3],GetOppositeOrientation(m_szOrientation[1]));

	if (nBitsAllocated == 24)
	{
		m_nColorDepth = 24;
		m_nBytesPerPixel = 3;
	}
	else
	{
		if (nSamplesPerPixel == 3)
		{
			m_nColorDepth = 24;
			m_nBytesPerPixel = 3;
		}
		else
		{
			m_nColorDepth = 8;
			m_nBytesPerPixel = 1;
		}
	}


	m_nColorTableEntries = (m_nColorDepth == 8) ? 256 : 0;

	m_nScanLineBytes =  ((m_nImageWidth * ((m_nColorDepth + 7) >> 3) + 3) / 4) * 4;

	m_nImageSize =  m_nScanLineBytes * m_nImageHeight;

	m_fWindowCenter = 0;
	m_fWindowWidth = 0;

	m_nFrameIndex = 0;

	m_bSelected = FALSE;

	m_bFromRIS = FALSE;

}

CDicomObject::CDicomObject(DcmDataset *old,BOOL bUseIt)
{
	unsigned short nBitsAllocated,nSamplesPerPixel;
	long nWidthRatio,nHeightRatio;
	double vo[6];
	int ni;
    OFString photometric;
	
	const char *lpszPhotoMetric=NULL;
	const char *lpszValue=NULL;
	CString sPhotoMetric("");

	m_nPlayMode = 0;
	
	m_nImageWidth = 0;
	m_nImageHeight = 0;

	m_fCalibrateValueX_M = 0.0;
	m_fCalibrateValueY_M = 0.0;

	m_fCalibrateValueX_A = 0.0;
	m_fCalibrateValueY_A = 0.0;

	if (bUseIt)
	{
		m_pDicomDataset = old;
	}
	else
	{
		m_pDicomDataset = new DcmDataset(*old);
	}

	
	m_pDicomDataset->findAndGetString(DCM_Modality,lpszValue);
	m_sModality = lpszValue;


	m_bHaveCTValue = (m_sModality.CompareNoCase(_T("CT")) == 0);

	m_pDicomDataset->findAndGetUint16(DCM_Columns,m_nImageWidth);
	m_pDicomDataset->findAndGetUint16(DCM_Rows,m_nImageHeight);

	m_pDicomDataset->findAndGetUint16(DCM_BitsAllocated,nBitsAllocated);
	m_pDicomDataset->findAndGetUint16(DCM_SamplesPerPixel,nSamplesPerPixel);
	m_pDicomDataset->findAndGetSint32(DCM_NumberOfFrames, m_nFrameCount);
	
	if (m_nFrameCount <= 0) m_nFrameCount = 1;

	m_fCalibrateValueX_A = GetFloat64(DCM_PixelSpacing,0);
	m_fCalibrateValueY_A = GetFloat64(DCM_PixelSpacing,1);

	if (m_fCalibrateValueX_A == 0.0)
	{
		m_fCalibrateValueX_A = GetFloat64(DCM_ImagerPixelSpacing,0);
	}

	if (m_fCalibrateValueY_A == 0.0)
	{
		m_fCalibrateValueY_A = GetFloat64(DCM_ImagerPixelSpacing,1);
	}


	nWidthRatio = 0;
	nHeightRatio = 0;

	m_pDicomDataset->findAndGetSint32(DCM_PixelAspectRatio,nWidthRatio,0);
	m_pDicomDataset->findAndGetSint32(DCM_PixelAspectRatio,nHeightRatio,1);


	if (nWidthRatio > 0 && nHeightRatio > 0)
	{
		if (nWidthRatio > nHeightRatio)
		{
			m_fPixelAspectRatio = (float) nHeightRatio / nWidthRatio;
		}
		else
		{
			m_fPixelAspectRatio = (float) nWidthRatio / nHeightRatio;
		}

	}
	else
	{
		m_fPixelAspectRatio = 1.0;
	}


	m_sPhotometricInterpretation = _T("");

	m_pDicomDataset->findAndGetString(DCM_PhotometricInterpretation,lpszPhotoMetric);
	sPhotoMetric = lpszPhotoMetric;

	if (sPhotoMetric.GetLength() > 0)
	{
    //if ((m_pDicomDataset->findAndGetOFString(DCM_PhotometricInterpretation, photometric)).good() && (photometric.length() > 0))
    //{
      const char *c;
	  //= photometric.c_str(); // guaranteed to be zero-terminated
      char cp[17]; // legal CS cannot be larger than 16 characters plus 0 byte
      int i=0; // current character

	  c = CStringA(sPhotoMetric);
      while (*c && (i<16))
      {
        if (isalpha(*c)) cp[i++] = toupper(*c);
        else if (isdigit(*c)) cp[i++] = *c;
        c++;
      }
      cp[i] = 0; // write terminating zero
      photometric = cp; // copy back into OFString
	  m_sPhotometricInterpretation = photometric.c_str();
	}

	////////////////////////////////////////////////////////
	// 有些公司的RGB图像缺失(0028,0006)属性,导致DicomImage->createWindowsDIB函数失败, 因此,运行时临时增加此属性
	unsigned short nPlanarConfig = 0;
	if ((m_sPhotometricInterpretation.CompareNoCase(_T("RGB")) == 0) && (m_pDicomDataset->findAndGetUint16(DCM_PlanarConfiguration, nPlanarConfig)).bad()) {
		m_pDicomDataset->putAndInsertUint16(DCM_PlanarConfiguration, 0);
	}
	///////////////////////////////////////////////////////
	
	
	for (ni = 0; ni < 6; ni ++)
	{
		m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,vo[ni],ni);
	}

	_tcscpy(m_szOrientation[0],GetOrientation(vo[0],vo[1],vo[2]));
	_tcscpy(m_szOrientation[1],GetOrientation(vo[3],vo[4],vo[5]));

	_tcscpy(m_szOrientation[2],GetOppositeOrientation(m_szOrientation[0]));
	_tcscpy(m_szOrientation[3],GetOppositeOrientation(m_szOrientation[1]));

	

	if (nBitsAllocated == 24)
	{
		m_nColorDepth = 24;
		m_nBytesPerPixel = 3;
	}
	else
	{
		if (nSamplesPerPixel == 3)
		{
			m_nColorDepth = 24;
			m_nBytesPerPixel = 3;
		}
		else
		{
			m_nColorDepth = 8;
			m_nBytesPerPixel = 1;
		}
	}

	m_nColorTableEntries = (m_nColorDepth == 8) ? 256 : 0;
 
	m_nScanLineBytes =  ((m_nImageWidth * ((m_nColorDepth + 7) >> 3) + 3) / 4) * 4;
	
	m_nImageSize =  m_nScanLineBytes * m_nImageHeight;

	m_fWindowCenter = 0;
	m_fWindowWidth = 0;

	m_bSelected = FALSE;

	m_nFrameIndex = 0;

	m_bFromRIS = FALSE;

}


CDicomObject::~CDicomObject()
{

	if (m_pDicomDataset != NULL)
		delete m_pDicomDataset;

}

int CDicomObject::GetBitsStored()
{
	unsigned short nBits = 8;

	m_pDicomDataset->findAndGetUint16(DCM_BitsStored,nBits);
	
	return nBits;
}


void CDicomObject::GetCalibrateValue(double &fx,double &fy)
{
	if (m_fCalibrateValueX_A != 0 && m_fCalibrateValueY_A != 0)
	{
		fx = m_fCalibrateValueX_A;
		fy = m_fCalibrateValueY_A;
	}
	else 
	{
		fx = m_fCalibrateValueX_M;
		fy = m_fCalibrateValueY_M;
	}

}

void CDicomObject::GetCalibrateValueA(double &fx,double &fy)
{
	fx = m_fCalibrateValueX_A;
	fy = m_fCalibrateValueY_A;
}

void CDicomObject::SetCalibrateValueM(double fx,double fy)
{
	m_fCalibrateValueX_M = fx;
	m_fCalibrateValueY_M = fy;
}


double CDicomObject::GetRescaleSlope()
{
	double fRet = 1;

	m_pDicomDataset->findAndGetFloat64(DCM_RescaleSlope,fRet);

	return fRet;
}


double CDicomObject::GetRescaleIntercept()
{
	double fRet = -1024;

	m_pDicomDataset->findAndGetFloat64(DCM_RescaleIntercept,fRet);

	return fRet;
}

CString CDicomObject::GetPatientAge()
{
	CString sAge(""),sBirthday,sYear(""),sMonth(""),sDay(""),sAgeUnit(""),sValue("");
	const char *lpszValue = NULL;
	int nLen,nYear,nMonth,nDay,nAge;
	COleDateTime dtBirthday,dtToday;
	COleDateTimeSpan span;

	m_pDicomDataset->findAndGetString(DCM_PatientAge,lpszValue);
	sAge = lpszValue;

	sAge.Trim();

	if (sAge == "")
	{
		m_pDicomDataset->findAndGetString(DCM_PatientBirthDate,lpszValue);

		sBirthday = lpszValue;
		nLen = sBirthday.GetLength();

		if (nLen == 8)
		{
			sYear = sBirthday.Mid(0,4);
			sMonth = sBirthday.Mid(4,2);
			sDay = sBirthday.Mid(6,2);
		}
		else if (nLen == 10)
		{
			sYear = sBirthday.Mid(0,4);
			sMonth = sBirthday.Mid(5,2);
			sDay = sBirthday.Mid(8,2);
			
		}
		else 
		{
			sYear = "";
			sMonth = "";
			sDay = "";
		}


		nYear = _ttoi(sYear);
		nMonth = _ttoi(sMonth);
		nDay = _ttoi(sDay);

		if (nYear >= 1907 && nMonth >= 1 && nMonth <= 12 && nDay >= 1 && nDay <= 31)
		{
			dtBirthday.SetDate(nYear,nMonth,nDay);
			dtToday = COleDateTime::GetCurrentTime();

			span = dtToday - dtBirthday;

			nAge = (int) (span.GetDays() / 365.0 + 0.5);
			
			sAge.Format(_T("%dY"),nAge);
		}
	}
	else
	{
		sAgeUnit = sAge.Right(1);

		if (sAgeUnit == _T("Y") || sAgeUnit == _T("M") || sAgeUnit == _T("W") || sAgeUnit == _T("D") )
		{
			sValue = sAge.Left(sAge.GetLength() - 1);
			nAge = _ttoi(sValue);

			if (nAge > 0) sAge.Format(_T("%d%s"),nAge,sAgeUnit);
		}
	}

	return sAge;

}

CString CDicomObject::GetOrientation(double x,double y,double z)
{
	CString sValue("");
	TCHAR orientation[4]; 
	TCHAR *optr,oX,oY,oZ;
	double absX,absY,absZ;
	int ni;

	memset((void *) orientation,0,4);

	optr = (TCHAR *) orientation;
	
	oX = (x < 0) ? _T('R') : _T('L');
	oY = (y < 0) ? _T('A') : _T('P');
	oZ = (z < 0) ? _T('I') : _T('S');

	absX = fabs(x);
	absY = fabs(y);
	absZ = fabs(z);

	for (ni = 0; ni < 3; ni ++)
	{
		if ((absX > .0001) && (absX > absY) && (absX > absZ))
		{
			*optr ++ = oX;
			absX = 0;
		}
		
		else if ((absY > .0001) && (absY > absX) && (absY > absZ))
		{
			*optr ++ = oY;
			absY = 0;
		}
		
		else if ((absZ > 0.0001) && (absZ > absX) && (absZ > absY))
		{
			*optr ++ = oZ;
			absZ = 0;
		}
		else break;

		*optr = _T('\0');
	}

	sValue = orientation;

	return sValue;
}


CString CDicomObject::GetOppositeOrientation(TCHAR *s)
{
	CString sValue("");
	TCHAR orientation[4];
	TCHAR ch1,ch2;
	int ni;

	memset((void *) orientation,0,4 * sizeof(TCHAR));

	ni = 0;
	while (ni < (int) _tcslen(s))
	{
		ch1 = s[ni];

		switch(ch1)
		{
		case _T('A'):
			ch2 = _T('P');
			break;
		case _T('P'):
			ch2 = _T('A');
			break;
		case _T('R'):
			ch2 = _T('L');
			break;
		case _T('L'):
			ch2 = _T('R');
			break;
		case _T('S'):
			ch2 = _T('I');
			break;
		case _T('I'):
			ch2 = _T('S');
			break;
		}

		orientation[ni] = ch2;

		ni ++;
	}

	sValue = orientation;

	return sValue;
}

CString CDicomObject::GetOrientationText(int nIdx,BOOL bHFlip,BOOL bVFlip,int nRotateAngle)
{
	CString sValue("");
	int nRealIdx;

	nRealIdx = (nIdx + ((360 - nRotateAngle) % 360) / 90) % 4;


	if (bHFlip)
	{
		if (nRealIdx == 0)
		{
			nRealIdx = 2;
		}
		else if (nRealIdx == 2)
		{
			nRealIdx = 0;
		}
	}

	if (bVFlip)
	{
		if (nRealIdx == 1)
		{
			nRealIdx = 3;
		}
		else if (nRealIdx == 3)
		{
			nRealIdx = 1;
		}
	}


	sValue = m_szOrientation[nRealIdx];
	
	return sValue;
}

void CDicomObject::GetWinLevel(double &fcen,double &fwid)
{
	fcen = m_fWindowCenter;
	fwid = m_fWindowWidth;
}

void CDicomObject::SetWinLevel(double fcen,double fwid)
{
	m_fWindowCenter = fcen;
	m_fWindowWidth = fwid;
}

CString CDicomObject::GetString(const DcmTagKey & tagKey)
{
	CString sValue("");
	const char *lpszValue = NULL;
	
	m_pDicomDataset->findAndGetString(tagKey,lpszValue);
	sValue = lpszValue;

	
	return sValue;
}

int CDicomObject::GetUint16(const DcmTagKey &tagKey,int nPos)
{
	unsigned short nValue = 0;

	m_pDicomDataset->findAndGetUint16(tagKey,nValue,nPos);

	return nValue;
}


int CDicomObject::GetSint32(const DcmTagKey &tagKey,int nPos)
{
	Sint32  nValue = 0;

	m_pDicomDataset->findAndGetSint32(tagKey,nValue,nPos);

	return nValue;
}


double CDicomObject::GetFloat64(const DcmTagKey &tagKey,int nPos)
{
	double fValue = 0.0;

	m_pDicomDataset->findAndGetFloat64(tagKey,fValue,nPos);

	return fValue;
}


void CDicomObject::GetImageOrientationPatient(double &fAx,double &fAy,double &fAz,double &fBx,double &fBy,double &fBz)
{
	double fax = 0,fay = 0,faz = 0,fbx = 0,fby = 0,fbz = 0;

	m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,fax,0);
	m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,fay,1);
	m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,faz,2);
	m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,fbx,3);
	m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,fby,4);
	m_pDicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,fbz,5);

	fAx = fax;
	fAy = fay;
	fAz = faz;

	fBx = fbx;
	fBy = fby;
	fBz = fbz;
}



void CDicomObject::GetImagePositionPatient(double &fx,double &fy,double &fz)
{
	double x = 0,y = 0,z = 0;

	m_pDicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,x,0);
	m_pDicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,y,1);
	m_pDicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,z,2);

	fx = x;
	fy = y;
	fz = z;
}





CString CDicomObject::GetModality()
{
	CString sValue("");
	const char *lpszValue=NULL;

	m_pDicomDataset->findAndGetString(DCM_Modality,lpszValue);
	sValue = lpszValue;
	
	return sValue;
}


BOOL CDicomObject::IsLocalizer()
{
	CString sValue("");
	const char *lpszValue = NULL;
	BOOL bValue = FALSE;

	m_pDicomDataset->findAndGetString(DCM_ImageType,lpszValue);

	sValue = lpszValue;

	if (sValue.Find(_T("LOCALIZER")) >= 0 )
		bValue = TRUE;
	else
		bValue = FALSE;

	return bValue;
}


int CDicomObject::GetInstanceNumber()
{
	const char *lpszValue = NULL;
	int nIndex;

	m_pDicomDataset->findAndGetString(DCM_InstanceNumber,lpszValue);

	if (lpszValue == NULL) 
		nIndex = 0;
	else
		nIndex = atoi(lpszValue);

	return nIndex;
}

void CDicomObject::GetWinLevelRange(int &nMinWidth,int &nMaxWidth,int &nMinCenter,int &nMaxCenter)
{



	return ;
}


CString CDicomObject::GetPatientId()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_PatientID,lpszValue);
	sValue = lpszValue;

	return sValue;
}

CString CDicomObject::GetPatientName()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_PatientName,lpszValue);
	sValue = lpszValue;
	return sValue;
}

CString CDicomObject::GetStudyId()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_StudyID,lpszValue);
	sValue = lpszValue;

	return sValue;
}

CString CDicomObject::GetSeriesNumber()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_SeriesNumber,lpszValue);
	sValue = lpszValue;

	return sValue;
}

CString CDicomObject::GetSeriesInstanceUID()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_SeriesInstanceUID,lpszValue);
	sValue = lpszValue;

	return sValue;
}

CString CDicomObject::GetInstanceUID()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_SOPInstanceUID,lpszValue);
	sValue = lpszValue;

	return sValue;
}



CString CDicomObject::GetFrameOfReferenceUID()
{
	CString sValue("");
	const char *lpszValue = NULL;

	m_pDicomDataset->findAndGetString(DCM_FrameOfReferenceUID,lpszValue);

	sValue = lpszValue;

	return sValue;
}



void CDicomObject::Cine(int nCineMode)
{
	switch(nCineMode)
	{
	case 1:
		m_nFrameIndex ++;
		if (m_nFrameIndex >= m_nFrameCount) m_nFrameIndex = 0;

		m_nPlayMode = 0;
		break;
	case 2:
		m_nFrameIndex --;
		if (m_nFrameIndex < 0) m_nFrameIndex = m_nFrameCount - 1;
		m_nPlayMode = 1;
		break;
	case 3:
		if (m_nPlayMode == 0)
		{
			m_nFrameIndex ++;

			if (m_nFrameIndex >= m_nFrameCount)
			{
				m_nFrameIndex = m_nFrameCount - 1;
				m_nPlayMode = 1;
			}
		}
		else
		{
			m_nFrameIndex --;

			if (m_nFrameIndex < 0)
			{
				m_nFrameIndex = 0;

				m_nPlayMode = 0;
			}
		}

		break;
	}
}


void CDicomObject::CineMove(int nStep)
{
	m_nFrameIndex = m_nFrameIndex + nStep;

	if (m_nFrameIndex < 0) m_nFrameIndex = 0;
	if (m_nFrameIndex >= m_nFrameCount) m_nFrameIndex = m_nFrameCount - 1;
}

void CDicomObject::SaveFile(const TCHAR *lpszPath)
{
	E_TransferSyntax optWriteXfer,optOriginalXfer;
	OFCondition status = EC_Normal,cond;
	CString sFileName,sErrMsg;
	int nLen;
	DcmFileFormat dcmff(m_pDicomDataset);
    DcmDataset *dset = dcmff.getDataset();

	sFileName = lpszPath;

	nLen = sFileName.GetLength();
	if (sFileName.GetAt(nLen - 1) != _T('\\'))
	{
		sFileName = sFileName + _T("\\");
	}

	sFileName = sFileName + GetInstanceUID() + _T(".dcm");

	optOriginalXfer = dset->getOriginalXfer();

	DcmXfer original_xfer(optOriginalXfer);

	if (!original_xfer.isEncapsulated())
	{
		DJ_RPLossless rp_lossless(6,0);

		//optWriteXfer = EXS_JPEGProcess14TransferSyntax;
		optWriteXfer = EXS_JPEGProcess14SV1TransferSyntax;

		dset->chooseRepresentation(optWriteXfer, &rp_lossless);
		//m_pDicomDataset->putAndInsertString(DCM_TransferSyntaxUID,UID_JPEGProcess14TransferSyntax);
		dset->putAndInsertString(DCM_TransferSyntaxUID,UID_JPEGProcess14SV1TransferSyntax);

	}
	else
	{
		optWriteXfer = optOriginalXfer;
	}


	cond = 	dcmff.saveFile(CStringA(sFileName),optWriteXfer, EET_ExplicitLength, 
						EGL_recalcGL, EPD_withoutPadding, (Uint32)0, 
							(Uint32)0);


	//m_pDicomDataset->saveFile(sFileName,optWriteXfer, EET_ExplicitLength, 
	//					EGL_recalcGL, EPD_withoutPadding);
	
	//if (status != EC_Normal)
	//{
	//	sErrMsg.Format("存盘时出错!\n%s",dcmErrorConditionToString(status));
	//	ShowErrMsg(sErrMsg);
	//}
}



void CDicomObject::SaveFileEx(const TCHAR *lpszPath)
{
	E_TransferSyntax OriginalXfer,writeXfer;
	OFCondition status = EC_Normal,cond;
	CString sFileName,sErrMsg;
	int nLen;

	DcmFileFormat dcmff(m_pDicomDataset);

    DcmDataset *dset = dcmff.getDataset();

	sFileName = lpszPath;

	nLen = sFileName.GetLength();

	if (sFileName.GetAt(nLen - 1) != '\\')
	{
		sFileName = sFileName + _T("\\");
	}

	sFileName = sFileName + GetInstanceUID() + _T(".dcm");

	OriginalXfer = dset->getOriginalXfer();
	DcmXfer original_xfer(OriginalXfer);

	if ((OriginalXfer == EXS_RLELossless) || 
		(OriginalXfer == EXS_JPEGProcess1TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess2_4TransferSyntax) || 
		(OriginalXfer == EXS_JPEGProcess3_5TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess6_8TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess7_9TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess10_12TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess11_13TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess14TransferSyntax)  ||
		(OriginalXfer == EXS_JPEGProcess15TransferSyntax)  ||
		(OriginalXfer == EXS_JPEGProcess16_18TransferSyntax)  ||
		(OriginalXfer == EXS_JPEGProcess17_19TransferSyntax)  ||
		(OriginalXfer == EXS_JPEGProcess20_22TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess21_23TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess24_26TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess25_27TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess28TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess29TransferSyntax) ||
		(OriginalXfer == EXS_JPEGProcess14SV1TransferSyntax) ||
		(OriginalXfer == EXS_RLELossless) ||
		(OriginalXfer == EXS_JPEGLSLossless) ||
		(OriginalXfer == EXS_JPEGLSLossy) ||
    	(OriginalXfer == EXS_JPEG2000LosslessOnly) ||
		(OriginalXfer == EXS_JPEG2000) ||
		(OriginalXfer == EXS_MPEG2MainProfileAtMainLevel) ||
		(OriginalXfer == EXS_JPEG2000MulticomponentLosslessOnly) ||
		(OriginalXfer == EXS_JPEG2000Multicomponent))
	{
		writeXfer = EXS_LittleEndianExplicit;
		dset->chooseRepresentation(writeXfer, NULL);

		// force meta-header to refresh SOP Class/Instance UIDs.
		DcmItem *metaInfo = (DcmItem *) dcmff.getMetaInfo();

		if (metaInfo)
		{
			delete metaInfo->remove(DCM_MediaStorageSOPClassUID);
			delete metaInfo->remove(DCM_MediaStorageSOPInstanceUID);
		}
				
	}
	else
	{
		writeXfer =  OriginalXfer;
	}
    
	cond = 	dcmff.saveFile(CStringA(sFileName),writeXfer, EET_ExplicitLength, 
						EGL_recalcGL, EPD_withoutPadding, (Uint32)0, 
							(Uint32)0);

	//if (cond != EC_Normal)
	//{
	//	sErrMsg.Format("存盘时出错!\n%s",dcmErrorConditionToString(status));
	//	ShowErrMsg(sErrMsg);
	//}
}

void CDicomObject::ExportToJpegFile(const TCHAR *lpszPath,const TCHAR *lpszFileName)
{
	CString sFileName;
	int nLen,nWid,nBitsStored;
	DicomImage *di;
	double fcen,fwid;
	CWaitCursor wait;
	

	if (lpszFileName == NULL || _tcslen(lpszFileName) == 0)
	{
		sFileName = lpszPath;

		nLen = sFileName.GetLength();

		if (sFileName.GetAt(nLen - 1) != _T('\\'))
		{
			sFileName = sFileName + _T("\\");
		}

		sFileName = sFileName + GetInstanceUID() + _T(".jpg");
	}
	else
	{
		sFileName = lpszFileName;
	}


	nBitsStored = GetBitsStored();

	di = new DicomImage(m_pDicomDataset,m_pDicomDataset->getOriginalXfer());

	GetWinLevel(fcen,fwid);
	
	if (fwid == 0)
	{
		if (di->getWindowCount() > 0)
		{
			di->setWindow(0);
		}
	
		di->getWindow(fcen,fwid);

		if (fwid == 0)
		{
			nWid = 1 << nBitsStored;
				
			fwid = nWid - 1;
			fcen = nWid >> 1;
		}
	}

	di->setWindow(fcen,fwid);

    DiJPEGPlugin plugin;
    plugin.setQuality(95);
    plugin.setSampling( ESS_422 );

    di->writePluginFormat(&plugin, CStringA(sFileName), 0);

	delete di;
}


void CDicomObject::SaveBitmapToBuf(unsigned char *&lpBMIH,int &nBMIHLen,unsigned char *&lpBits,int &nBitsLen)
{
	DicomImage	*lpDicomImage;
	OFCondition status = EC_Normal;
	int ni,nBytesPerPixel,nScanLineBytes,nImageWidth,nImageHeight,nBitsBufferSize;
	int nFrameIndex;
	int nBitsStored,nWid;
	unsigned long nSize=0;
	LPRGBQUAD pDibQuad;
	unsigned char *lpOrginBits=NULL;
	double fcen,fwid;
	CString sPhotometric;
	unsigned char pLutData8[255],*pLut8;
	unsigned short pLutData16[255],*pLut16;
	BITMAPINFOHEADER *pbih;
	BYTE *lpvColorTable;


	nFrameIndex = GetFrameIndex();

	pLut8 = (unsigned char * )&pLutData8;
	pLut16 = (unsigned short *) &pLutData16;
	      
	sPhotometric = GetPhotometricInterpretation();
	
	nImageWidth = GetImageWidth();
	nImageHeight = GetImageHeight();

	nBitsStored = GetBitsStored();

	nBytesPerPixel = GetBytesPerPixel();
	nScanLineBytes = GetScanLineBytes(); 
	nBitsBufferSize = GetImageSize();

	if (lpBits != NULL)
	{
		delete [] lpBits;
		lpBits = NULL;
	}

	nBitsLen = nBitsBufferSize;

	lpBits = new unsigned char[nBitsBufferSize];

	if (lpBMIH != NULL)
	{
		delete lpBMIH;
		lpBMIH = NULL;
	}
	
	nBMIHLen = sizeof (BITMAPINFOHEADER) + 
			sizeof (RGBQUAD) * (GetColorTableEntries() + 4);

	lpBMIH =  new unsigned char[nBMIHLen];

	pbih = (BITMAPINFOHEADER *) lpBMIH;

	// Setup BitmapInfoHeader
	pbih->biSize = sizeof(BITMAPINFOHEADER);
	pbih->biWidth = nImageWidth;
	pbih->biHeight = nImageHeight;
	pbih->biPlanes = 1;
	pbih->biBitCount = GetColorDepth();
	pbih->biSizeImage = nBitsBufferSize;
	pbih->biXPelsPerMeter = 0;
	pbih->biYPelsPerMeter = 0;
	pbih->biClrUsed = GetColorTableEntries();
	pbih->biClrImportant = GetColorTableEntries();
	pbih->biCompression = BI_RGB;
	
	lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);
    
	pDibQuad = (LPRGBQUAD) lpvColorTable;
	memset(lpvColorTable, 0, sizeof(RGBQUAD) * (GetColorTableEntries() + 4));
	
	for (ni = 0; ni <  GetColorTableEntries(); ni++)
	{
		pDibQuad->rgbRed		= ni;
		pDibQuad->rgbGreen      = ni;
		pDibQuad->rgbBlue       = ni;
		pDibQuad->rgbReserved   = 0;

		pDibQuad++;
	}


	if (sPhotometric == "MONOCHROME1" || sPhotometric == "MONOCHROME2" || sPhotometric == "RGB" || sPhotometric == "YBRFULL" || sPhotometric == "YBRFULL422")
	{
	
		lpDicomImage = new DicomImage(GetDicomDataset(),GetDicomDataset()->getOriginalXfer());//CIF_KeepYCbCrColorModel);
					
		GetWinLevel(fcen,fwid);
	
		if (fwid == 0)
		{
			if (lpDicomImage->getWindowCount() > 0)
			{
				lpDicomImage->setWindow(0);
			}
	
			lpDicomImage->getWindow(fcen,fwid);

			if (fwid == 0)
			{
				nWid = 1 << nBitsStored;
				
				fwid = nWid - 1;
				fcen = nWid >> 1;
			}
		}

		lpDicomImage->setWindow(fcen,fwid);
		
		lpDicomImage->createWindowsDIB((void *&)lpBits,GetImageSize(),nFrameIndex,GetColorDepth(),1,1);		

		delete lpDicomImage;
	}

	
}


void CDicomObject::Print(std::ostream *pos)
{
	if (m_pDicomDataset != NULL)
	{
		m_pDicomDataset->print(*pos,DCMTypes::PF_shortenLongTagValues);// | DCMTypes::PF_showTreeStructure);
	}
}


void CDicomObject::Reset()
{
	m_fWindowCenter = 0;
	m_fWindowWidth = 0;

}
