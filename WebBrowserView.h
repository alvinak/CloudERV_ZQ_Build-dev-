#pragma once

#ifdef _WIN32_WCE
#error "CHtmlView is not supported for Windows CE."
#endif 

class CWebBrowserDoc;
// CWebBrowserView html view

class CWebBrowserView : public CHtmlView
{
	DECLARE_DYNCREATE(CWebBrowserView)

protected:
	CWebBrowserView();           // protected constructor used by dynamic creation
	virtual ~CWebBrowserView();

	CWebBrowserDoc *GetDocument() const;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
};




#ifndef _DEBUG  // debug version in WebBrowserView.cpp
inline CWebBrowserDoc* CWebBrowserView::GetDocument() const
   { return reinterpret_cast<CWebBrowserDoc*>(m_pDocument); }
#endif


