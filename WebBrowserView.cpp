// WebBrowserView.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "WebBrowserView.h"
#include "WebBrowserDoc.h"


// CWebBrowserView

IMPLEMENT_DYNCREATE(CWebBrowserView, CHtmlView)

CWebBrowserView::CWebBrowserView()
{

}

CWebBrowserView::~CWebBrowserView()
{
}

void CWebBrowserView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWebBrowserView, CHtmlView)
	ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()


// CWebBrowserView diagnostics

#ifdef _DEBUG
void CWebBrowserView::AssertValid() const
{
	CHtmlView::AssertValid();
}

CWebBrowserDoc* CWebBrowserView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWebBrowserDoc)));
	return (CWebBrowserDoc*)m_pDocument;
}

void CWebBrowserView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CWebBrowserView message handlers


void CWebBrowserView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();
}


void CWebBrowserView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CString sHeader("Content-Type: application/x-www-form-urlencoded");
	char szPostData[255];
	CString sWebViewerInitParams;
	CString sActionUrl(""), sWebViewerUserId(""), sWebViewerPassword(""), sInitParams(""), sStudyUID("");

	GetDocument()->GetDocParams(sActionUrl, sWebViewerUserId, sWebViewerPassword, sInitParams, sStudyUID);

	sWebViewerInitParams.Format(sInitParams, sStudyUID);
	
	memset(szPostData, 0 , 255);
	sprintf(szPostData, "username=%s&password=%s&liststudies=true&WebInitParams=%s",
		sWebViewerUserId, sWebViewerPassword, CStringA(sWebViewerInitParams));

	Navigate2(sActionUrl, 0, NULL, sHeader, (LPVOID) szPostData, strlen(szPostData) + 1);
}



void CWebBrowserView::OnParentNotify(UINT message, LPARAM lParam)
{
	if ((LOWORD(message) == WM_DESTROY) && ((HWND) lParam == m_wndBrowser)) 
		GetParentFrame()->SendMessage(WM_CLOSE, 0, 0);
	else
		CHtmlView::OnParentNotify(message, lParam);
}
