// SensitivityDlg.cpp : implementation file
//

#include "stdafx.h"

#include "SensitivityDlg.h"
#include "DcmImageView.h"


// CSensitivityDlg dialog

CSensitivityDlg::CSensitivityDlg(CWnd* pParent /*=NULL*/)
	: CXTPDialog(CSensitivityDlg::IDD, pParent)
{
	m_nSensitivity = 1;
	m_pDcmImageViewObj = NULL;
}

CSensitivityDlg::~CSensitivityDlg()
{
}

BOOL CSensitivityDlg::Create(CWnd* pParentWnd)
{
	return CXTPDialog::Create(IDD, pParentWnd);
}

void CSensitivityDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSensitivityDlg, CXTPDialog)
	ON_BN_CLICKED(IDOK, &CSensitivityDlg::OnBnClickedOk)
END_MESSAGE_MAP()


void CSensitivityDlg::SetDcmImageViewObj(void *pDcmImageViewObj)
{
	m_pDcmImageViewObj = pDcmImageViewObj;
}


void CSensitivityDlg::SetSensitivity(int nValue)
{
	CWnd *pWnd;
	CString sValue;

	if (nValue <= 0) return;

	m_nSensitivity = nValue;

	pWnd = GetDlgItem(IDC_VALUE);

	if (pWnd != NULL)
	{
		sValue.Format(_T("%d"),m_nSensitivity);
		pWnd->SetWindowText(sValue);
	}

}


int  CSensitivityDlg::GetSensitivity()
{
	return m_nSensitivity;
}

// CSensitivityDlg message handlers

BOOL CSensitivityDlg::OnInitDialog()
{
	CWnd *pWnd;
	CString sValue;
	CSpinButtonCtrl *pSpinCtrl;
	

	CXTPDialog::OnInitDialog();

	pWnd = GetDlgItem(IDC_VALUE);

	pSpinCtrl = (CSpinButtonCtrl *) GetDlgItem(IDC_SPIN);
	pSpinCtrl->SetBuddy(pWnd);
	pSpinCtrl->SetRange(1,5);
	
	if (m_nSensitivity > 0 && m_nSensitivity <= 5)
	{
		sValue.Format(_T("%d"),m_nSensitivity);
	}
	else
	{
		sValue = _T("5");
	}

	pSpinCtrl->SetPos(m_nSensitivity);

	pWnd->SetWindowText(sValue);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSensitivityDlg::OnBnClickedOk()
{
	int nValue;
	CString sValue("");
	CWnd *pWnd;
	CDcmImageView *pDcmImageViewObj;

	pWnd = GetDlgItem(IDC_VALUE);

	pWnd->GetWindowText(sValue);
	
	nValue = _ttoi(sValue);

	if (nValue > 0)
	{
		m_nSensitivity = nValue;

		pDcmImageViewObj = (CDcmImageView *) m_pDcmImageViewObj;

		pDcmImageViewObj->SetSensitivity(m_nSensitivity);

		OnOK();
	}
}

