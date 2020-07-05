// DcmImageDoc.cpp : implementation of the CDcmImageDoc class
//

#include "stdafx.h"
#include "DcmImageDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDcmImageDoc

IMPLEMENT_DYNCREATE(CDcmImageDoc, CDocument)

BEGIN_MESSAGE_MAP(CDcmImageDoc, CDocument)
END_MESSAGE_MAP()


// CDcmImageDoc construction/destruction

CDcmImageDoc::CDcmImageDoc()
{
	m_bInitialized	= FALSE;
	m_pCurrentSeriesObject = NULL;
	m_sStudyGUID = _T("");
}

CDcmImageDoc::~CDcmImageDoc()
{
	Cleanup();
}

BOOL CDcmImageDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	SetTitle(_T("影像资料"));

	return TRUE;
}

BOOL CDcmImageDoc::IsStudyUIDExisted(const TCHAR *szStudyUID)
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



void CDcmImageDoc::AppendSeriesObjectList(CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear)
{
	CString sTitle(""), sPatientName(""), sPatientNameList("");
	CSeriesObject *pSeriesObj, *pFirstObj = NULL;
	POSITION pos;
	BOOL bFromExtFile;

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
	bFromExtFile = FALSE; 

	pos = m_SeriesObjectList.GetHeadPosition();
	while (pos != NULL) {
		pSeriesObj = (CSeriesObject *) pSeriesObjList->GetNext(pos);
		if (pSeriesObj != NULL) {
			if (pSeriesObj->IsFromExtFile())
				bFromExtFile = TRUE;

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

	sTitle = _T("影像资料");

	if (bFromExtFile)
		sTitle = sTitle + _T("-外部文件");
	else
		if (!sPatientNameList.IsEmpty())
			sTitle = sTitle + _T("-") + sPatientNameList;

	SetTitle(sTitle);

	UpdateAllViews(NULL, (bSeriesMode || bFromExtFile) ? 1 : 0);
}

// CDcmImageDoc serialization

void CDcmImageDoc::Serialize(CArchive& ar)
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


// CDcmImageDoc diagnostics

#ifdef _DEBUG
void CDcmImageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDcmImageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CDcmImageDoc::Cleanup() 
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

// CDcmImageDoc commands


void CDcmImageDoc::OnCloseDocument()
{
	Cleanup();

	CDocument::OnCloseDocument();
}
