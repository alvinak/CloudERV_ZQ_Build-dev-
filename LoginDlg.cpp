// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_sUserId(_T(""))
	, m_sPassword(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UserID, m_sUserId);
	DDX_Text(pDX, IDC_EDIT_Password, m_sPassword);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CLoginDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CLoginDlg::OnBnClickedBtnCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLoginDlg message handlers


void CLoginDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
		UpdateData(TRUE);

	if(m_sUserId.IsEmpty () || m_sPassword.IsEmpty ())
	{
		MessageBox(TEXT("用户名和密码都不能为空"));
		return ;
	}

	//获取正确的值
	//Get USERNAME and Password
	CString sUserId,sPassword;
	sUserId = "admin";
	sPassword = "1234";

	if(sUserId == m_sUserId)//用户名一致
	{
		if(sPassword == m_sPassword)
		{
			//关闭当前对话框
			CDialog::OnCancel ();
		}
		else
		{
			MessageBox(TEXT("密码有误"));
		}
	}
	else
	{
		MessageBox(TEXT("用户名有误"));
	}
}


void CLoginDlg::OnBnClickedBtnCancel()
{
	// TODO: Add your control notification handler code here
	exit(0);
}


void CLoginDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnOK();
}


void CLoginDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	//CDialogEx::OnClose();

	//退出程序
	exit(0);
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	//默认登录信息
	CString sUserId,sPassword;
	sUserId = "admin";
	sPassword = "1234";

	m_sUserId = sUserId;
	m_sPassword = sPassword;

	//同步到控件中
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
