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
		MessageBox(TEXT("�û��������붼����Ϊ��"));
		return ;
	}

	//��ȡ��ȷ��ֵ
	//Get USERNAME and Password
	CString sUserId,sPassword;
	sUserId = "admin";
	sPassword = "1234";

	if(sUserId == m_sUserId)//�û���һ��
	{
		if(sPassword == m_sPassword)
		{
			//�رյ�ǰ�Ի���
			CDialog::OnCancel ();
		}
		else
		{
			MessageBox(TEXT("��������"));
		}
	}
	else
	{
		MessageBox(TEXT("�û�������"));
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

	//�˳�����
	exit(0);
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	//Ĭ�ϵ�¼��Ϣ
	CString sUserId,sPassword;
	sUserId = "admin";
	sPassword = "1234";

	m_sUserId = sUserId;
	m_sPassword = sPassword;

	//ͬ�����ؼ���
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
