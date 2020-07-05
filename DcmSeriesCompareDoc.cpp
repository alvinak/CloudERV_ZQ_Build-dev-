// DcmSeriesCompareDoc.cpp : implementation of the CDcmSeriesCompareDoc class
//

#include "stdafx.h"
#include "DcmSeriesCompareDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDcmSeriesCompareDoc

IMPLEMENT_DYNCREATE(CDcmSeriesCompareDoc, CDocument)

BEGIN_MESSAGE_MAP(CDcmSeriesCompareDoc, CDocument)
END_MESSAGE_MAP()


// CDcmSeriesCompareDoc construction/destruction

CDcmSeriesCompareDoc::CDcmSeriesCompareDoc()
{
	m_bInitialized	= FALSE;
	m_pCurrentSeriesObject = NULL;
	m_sStudyGUID = _T("");
}

CDcmSeriesCompareDoc::~CDcmSeriesCompareDoc()
{
	Cleanup();
}

BOOL CDcmSeriesCompareDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	SetTitle(_T("影像序列对比"));

	return TRUE;
}

BOOL CDcmSeriesCompareDoc::IsStudyUIDExisted(const TCHAR *szStudyUID)
{
	POSITION pos;
	CSeriesObject *pSeriesObj;
	BOOL bFound = FALSE;

	bFound = FALSE;
	pos = m_SeriesObjectList.GetHeadPosition();
	while (pos != NULL && !bFound) {
		pSeriesObj = (CSeriesObject *) m_SeriesObjectList.GetNext(pos);
		if (pSeriesObj) {
			if (pSeriesObj->m_sStudyUID.Compare(szStudyUID) == 0)
				bFound = TRUE;
		}
	}

	return bFound;
}



void CDcmSeriesCompareDoc::AppendSeriesObject(CSeriesObject *pObj, bool bHeadPosition) 
{
	CString sTitle("");
	CSeriesObject *pNewSeriesObj, *pSeriesObj;
	POSITION pos;

	if (pObj == NULL)
		return;

	//  复制SeriesObject

	pNewSeriesObj = new CSeriesObject();
	pNewSeriesObj->CloneFrom(pObj);
	
	if (bHeadPosition) {

		while (m_SeriesObjectList.GetCount() >= 2) {
			pos = m_SeriesObjectList.GetTailPosition();
			if (pos) { 
				pSeriesObj = (CSeriesObject *) m_SeriesObjectList.GetAt(pos);
				delete pSeriesObj;
				m_SeriesObjectList.RemoveAt(pos);
			}
		}

		m_SeriesObjectList.AddHead(pNewSeriesObj);

	}
	else {

		while (m_SeriesObjectList.GetCount() >= 2) {
			pos = m_SeriesObjectList.GetHeadPosition();
			if (pos) {
				pSeriesObj = (CSeriesObject *) m_SeriesObjectList.GetAt(pos);
				delete pSeriesObj;
				m_SeriesObjectList.RemoveAt(pos);
			}
		}

		m_SeriesObjectList.AddTail(pNewSeriesObj);
	}

	m_pCurrentSeriesObject = pNewSeriesObj;

	sTitle = _T("影像序列对比");

	SetTitle(sTitle);

	UpdateAllViews(NULL);
}



void CDcmSeriesCompareDoc::AppendSeriesObjectList(CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear)
{
	CString sTitle(""), sPatientName(""), sPatientNameList("");
	CSeriesObject *pSeriesObj, *pFirstObj = NULL;
	POSITION pos;

	if (bClear) {
		pos = m_SeriesObjectList.GetHeadPosition();
		while (pos != NULL) {
			pSeriesObj = (CSeriesObject *) m_SeriesObjectList.GetNext(pos);
			if (pSeriesObj) 
				delete pSeriesObj;
		}

		m_SeriesObjectList.RemoveAll();
		m_pCurrentSeriesObject = NULL;
	}

	if (pSeriesObjList != NULL) {
		if (pSeriesObjList != &m_SeriesObjectList) {
			pos = pSeriesObjList->GetHeadPosition();
			while (pos != NULL) {
				pSeriesObj = (CSeriesObject *) pSeriesObjList->GetNext(pos);
				if (pSeriesObj != NULL) {
					if (pSeriesObj->m_DicomObjectList.GetCount() > 0) {
						m_SeriesObjectList.AddTail(pSeriesObj);

						if (pFirstObj == NULL)
							pFirstObj = pSeriesObj;
					}
					else {
						delete pSeriesObj;
					}	
				}
			}

			if (pFirstObj != NULL)
				m_pCurrentSeriesObject = pFirstObj;
		}
	}


	sPatientNameList = _T("");

	pos = m_SeriesObjectList.GetHeadPosition();
	while (pos != NULL) {
		pSeriesObj = (CSeriesObject *) pSeriesObjList->GetNext(pos);
		if (pSeriesObj != NULL) {

			sPatientName = pSeriesObj->m_sPatientName_CN;
			if (!sPatientName.IsEmpty()) {
				if (sPatientNameList.Find(sPatientName) == -1) {
					if (!sPatientNameList.IsEmpty()) 
						sPatientNameList = sPatientNameList + _T(",");
					sPatientNameList = sPatientNameList + sPatientName;
				}
			}
		}
	}

	sTitle = _T("影像序列对比");

	SetTitle(sTitle);

	UpdateAllViews(NULL, bSeriesMode  ? 1 : 0);
}

// CDcmSeriesCompareDoc serialization

void CDcmSeriesCompareDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CDcmSeriesCompareDoc diagnostics

#ifdef _DEBUG
void CDcmSeriesCompareDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDcmSeriesCompareDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


void CDcmSeriesCompareDoc::Cleanup()
{
	POSITION pos;
	CSeriesObject *pSeriesObj;


	pos = m_SeriesObjectList.GetHeadPosition();
	while (pos != NULL) {
		pSeriesObj = (CSeriesObject *)m_SeriesObjectList.GetNext(pos);
		if (pSeriesObj != NULL) {
			delete pSeriesObj;
		}
	}

	m_SeriesObjectList.RemoveAll();
}

// CDcmSeriesCompareDoc commands


void CDcmSeriesCompareDoc::OnCloseDocument()
{
	Cleanup();

	CDocument::OnCloseDocument();
}
