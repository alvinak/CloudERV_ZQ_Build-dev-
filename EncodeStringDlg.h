#pragma once


// CEncodeStringDlg dialog

class CEncodeStringDlg : public CXTPDialog
{

public:
	CEncodeStringDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEncodeStringDlg();

// Dialog Data
	enum { IDD = IDD_ENCODESTR };



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedEncode();
};
