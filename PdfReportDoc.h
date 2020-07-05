// PdfReportDoc.h : interface of the CPdfReportDoc class
//


#pragma once


class CPdfReportDoc : public CDocument
{
protected: // create from serialization only
	CPdfReportDoc();
	DECLARE_DYNCREATE(CPdfReportDoc)

// Attributes
public:
	BOOL	m_bInitialized;
	BOOL	m_bExternalFile;
	unsigned char * m_pPdfData;
	int		m_nPdfDataLen;
	
	char m_szPdfPassword[101];

	CString	m_sTitle;
	CString m_sReportPdfGuid;
// Operations
public:

	void SetDocParams(unsigned char *pPdfData, long nPdfDataLen, const TCHAR *sTitle, const TCHAR *sReportPdfGuid, const char *szPdfPassword, BOOL bExternalFile);
	BOOL IsInitialized() { return m_bInitialized; }
	unsigned char *GetPdfData() { return m_pPdfData; }
	long GetPdfDataLen() { return m_nPdfDataLen; }
	char *GetPdfPassword() { return m_szPdfPassword; }
	BOOL IsExternalFile() { return m_bExternalFile; }

	const TCHAR *GetTitle() { return m_sTitle; }
	const TCHAR *GetReportPdfGuid() { return m_sReportPdfGuid; }

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CPdfReportDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
