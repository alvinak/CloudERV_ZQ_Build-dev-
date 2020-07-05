#pragma once


// CCalibrateDlg dialog

class CCalibrateDlg : public CDialog
{
	DECLARE_DYNAMIC(CCalibrateDlg)

public:
	CCalibrateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCalibrateDlg();

// Dialog Data
	enum { IDD = IDD_CALIBRATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int m_nLineLength;

public:

	virtual BOOL Create(CWnd* pParentWnd = NULL);
	virtual void OnOK() {}

	int GetLineLength() { return m_nLineLength; }

public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnEnChangeValue();
public:
	afx_msg void OnBnClickedOk();
};
