#pragma once

#include "SystemConfiguration.h"
#include "SQLAPI.h"
#include "md5.h"

// CLoginDlg dialog

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();

// Dialog Data
	enum { IDD = IDD_LOGIN };

	bool DoLogin(const TCHAR *szSystem, const TCHAR *szUserID, const TCHAR *szPassword);
	//bool LoadPermissionData( CString *szRoleGUIDs, unsigned long  &nPermission, CString &sErrMsg);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CString m_txtUserId;
	CString m_txtPassword;
public:
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnCancel();
	virtual void OnOK();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();

 
};
