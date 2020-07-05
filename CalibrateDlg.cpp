// CalibrateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "CalibrateDlg.h"


// CCalibrateDlg dialog

IMPLEMENT_DYNAMIC(CCalibrateDlg, CDialog)

CCalibrateDlg::CCalibrateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalibrateDlg::IDD, pParent)
{
	m_nLineLength = 0;
}

CCalibrateDlg::~CCalibrateDlg()
{
}

void CCalibrateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCalibrateDlg, CDialog)
	ON_EN_CHANGE(IDC_VALUE, &CCalibrateDlg::OnEnChangeValue)
	ON_BN_CLICKED(IDOK, &CCalibrateDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCalibrateDlg message handlers

BOOL CCalibrateDlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CCalibrateDlg::OnInitDialog()
{
	CWnd *pWnd;

	CDialog::OnInitDialog();

	pWnd = GetDlgItem(IDOK);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_VALUE);
	pWnd->SetWindowText(_T(""));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCalibrateDlg::OnEnChangeValue()
{
	CWnd *pWnd;
	CString sValue("");
	int  nValue;
	
	pWnd = GetDlgItem(IDC_VALUE);
	pWnd->GetWindowText(sValue);

	nValue = _ttoi(sValue);

	pWnd = GetDlgItem(IDOK);

	pWnd->EnableWindow(nValue > 0);
}

void CCalibrateDlg::OnBnClickedOk()
{
	CWnd *pWnd;
	CString sValue;
	int nValue;

	pWnd = GetDlgItem(IDC_VALUE);
	pWnd->GetWindowText(sValue);

	nValue = _ttoi(sValue);

	m_nLineLength = nValue;

	CDialog::OnOK();
}
