// PdfReportDoc.cpp : implementation of the CPdfReportDoc class
//

#include "stdafx.h"
#include "PdfReportDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPdfReportDoc

IMPLEMENT_DYNCREATE(CPdfReportDoc, CDocument)

BEGIN_MESSAGE_MAP(CPdfReportDoc, CDocument)
END_MESSAGE_MAP()


// CPdfReportDoc construction/destruction

CPdfReportDoc::CPdfReportDoc()
{
	m_pPdfData		= NULL;
	m_nPdfDataLen	= 0;
	m_sTitle		= _T("");
	m_bInitialized	= FALSE;
	memset(m_szPdfPassword, 0, 100);

	m_bExternalFile = FALSE;
}

CPdfReportDoc::~CPdfReportDoc()
{
	if (m_pPdfData)
		free(m_pPdfData);
}

BOOL CPdfReportDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

void CPdfReportDoc::SetDocParams(unsigned char *pPdfData, long nPdfDataLen, const TCHAR *sTitle, const TCHAR *sReportPdfGuid, const char *szPdfPassword, BOOL bExternalFile)
{
	if (m_pPdfData)
		free(m_pPdfData);

	memset(m_szPdfPassword, 0, 100);

	m_pPdfData = (unsigned char *) malloc(nPdfDataLen + 1);
	memcpy(m_pPdfData, pPdfData, nPdfDataLen);
	m_nPdfDataLen = nPdfDataLen;

	strcpy(m_szPdfPassword, szPdfPassword);

	m_sTitle = sTitle;
	m_sReportPdfGuid = sReportPdfGuid;

	m_bInitialized = TRUE;
	m_bExternalFile = bExternalFile;

	SetTitle(sTitle);

	UpdateAllViews(NULL);
}

// CPdfReportDoc serialization

void CPdfReportDoc::Serialize(CArchive& ar)
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


// CPdfReportDoc diagnostics

#ifdef _DEBUG
void CPdfReportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPdfReportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPdfReportDoc commands
