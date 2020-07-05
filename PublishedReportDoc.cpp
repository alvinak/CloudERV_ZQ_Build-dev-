// PublishedReportDoc.cpp : implementation of the CPublishedReportDoc class
//

#include "stdafx.h"
#include "PublishedReportDoc.h"
#include <vector>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPublishedReportDoc

IMPLEMENT_DYNCREATE(CPublishedReportDoc, CDocument)

BEGIN_MESSAGE_MAP(CPublishedReportDoc, CDocument)
END_MESSAGE_MAP()


// CPublishedReportDoc construction/destruction

CPublishedReportDoc::CPublishedReportDoc()
{
	m_pCurrentMedicalResultObject = NULL;
}

CPublishedReportDoc::~CPublishedReportDoc()
{
}

BOOL CPublishedReportDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle(_T("影像检查结果"));

	return TRUE;
}




// CPublishedReportDoc serialization

void CPublishedReportDoc::Serialize(CArchive& ar)
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


// CPublishedReportDoc diagnostics

#ifdef _DEBUG
void CPublishedReportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPublishedReportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



int CPublishedReportDoc::GetIndexByStudyGUID(const TCHAR *szStudyGUID)
{
	int ni, nIndex = -1;
	
	for (ni = 0, nIndex = -1; (ni < m_lstMedicalResultObject.size()) && (nIndex == -1); ni ++) {
		if ((m_lstMedicalResultObject)[ni]->m_sStudyGUID.Compare(szStudyGUID) == 0)
			nIndex = ni;
	}

	return nIndex;
}


// CPublishedReportDoc commands

void CPublishedReportDoc::LocateExamReport(const TCHAR *szStudyGUID)
{
	std::vector <CMedicalResultObject *> ::iterator itResult;
	BOOL bFound;

	if (szStudyGUID == NULL || wcslen(szStudyGUID) == 0)
		return;
	
	bFound = FALSE;
	for (itResult = m_lstMedicalResultObject.begin(); itResult != m_lstMedicalResultObject.end() && !bFound; itResult ++) {
		if ((*itResult)->m_sStudyGUID.Compare(szStudyGUID) == 0) {
			m_pCurrentMedicalResultObject = (*itResult);
			bFound = TRUE;
		}
	}
	
	if (bFound)
		UpdateAllViews(NULL);

}


