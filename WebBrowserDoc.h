// WebBrowserDoc.h : interface of the CWebBrowserDoc class
//


#pragma once


class CWebBrowserDoc : public CDocument
{
protected: // create from serialization only
	CWebBrowserDoc();
	DECLARE_DYNCREATE(CWebBrowserDoc)

// Attributes
public:
	CString	m_sWebViewerUrl;
	CString m_sWebViewerUserId;
	CString m_sWebViewerPassword;
	CString m_sWebViewerInitParams;
	CString m_sStudyUID;
// Operations
public:

	void SetDocParams(const TCHAR *szActionUrl, const TCHAR *szUserId, const TCHAR *szPassword, const TCHAR *szInitParams, const TCHAR *szStudyUID);
	void GetDocParams(CString &sActionUrl, CString &sUserId, CString &sPassword, CString &sInitParams, CString &sStudyUID);
// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CWebBrowserDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
