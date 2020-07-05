#pragma once


// CNewBookingDlg dialog

class CNewBookingDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewBookingDlg)

public:
	CNewBookingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewBookingDlg();

	CEdit	m_ctlEdit;
// Dialog Data
	enum { IDD = IDD_NEWBOOKING };

	const TCHAR *GetInputValue();
	
private:
	CString m_sInputValue;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
