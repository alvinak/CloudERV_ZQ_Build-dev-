#pragma once


// CSensitivityDlg dialog

class CSensitivityDlg : public CXTPDialog
{
public:
	CSensitivityDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSensitivityDlg();

	
// Dialog Data
	enum { IDD = IDD_SENSITIVITY };

private:
	int m_nSensitivity;
	void *m_pDcmImageViewObj;

public:
	void SetDcmImageViewObj(void *pDcmImageViewObj);
	void SetSensitivity(int nValue);
	int  GetSensitivity();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual BOOL Create(CWnd* pParentWnd = NULL);
};
