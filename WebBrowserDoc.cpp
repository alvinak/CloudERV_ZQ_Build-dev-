// WebBrowserDoc.cpp : implementation of the CWebBrowserDoc class
//

#include "stdafx.h"
#include "WebBrowserDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebBrowserDoc

IMPLEMENT_DYNCREATE(CWebBrowserDoc, CDocument)

BEGIN_MESSAGE_MAP(CWebBrowserDoc, CDocument)
END_MESSAGE_MAP()


// CWebBrowserDoc construction/destruction

CWebBrowserDoc::CWebBrowserDoc()
{
	m_sWebViewerUrl			= _T("");
	m_sWebViewerUserId		= _T("");
	m_sWebViewerPassword	= _T("");
	m_sWebViewerInitParams	= _T("");
	m_sStudyUID				= _T("");
}

CWebBrowserDoc::~CWebBrowserDoc()
{

}

BOOL CWebBrowserDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle(_T("Ó°Ïñä¯ÀÀ-WebViewer"));

	return TRUE;
}

void CWebBrowserDoc::SetDocParams(const TCHAR *szActionUrl, const TCHAR *szUserId, const TCHAR *szPassword, const TCHAR *szInitParams, const TCHAR *szStudyUID)
{
	m_sWebViewerUrl			= szActionUrl;
	m_sWebViewerUserId		= szUserId;
	m_sWebViewerPassword	= szPassword;
	m_sWebViewerInitParams	= szInitParams;
	m_sStudyUID				= szStudyUID;

	UpdateAllViews(NULL);
}


void CWebBrowserDoc::GetDocParams(CString &sActionUrl, CString &sUserId, CString &sPassword, CString &sInitParams, CString &sStudyUID)
{
	sActionUrl		= m_sWebViewerUrl;
	sUserId			= m_sWebViewerUserId;
	sPassword		= m_sWebViewerPassword;
	sInitParams		= m_sWebViewerInitParams;
	sStudyUID		= m_sStudyUID;
}


// CWebBrowserDoc serialization

void CWebBrowserDoc::Serialize(CArchive& ar)
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


// CWebBrowserDoc diagnostics

#ifdef _DEBUG
void CWebBrowserDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWebBrowserDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CWebBrowserDoc commands
