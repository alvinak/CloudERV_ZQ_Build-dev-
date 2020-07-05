// NewBookingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "NewBookingDlg.h"
#include "afxdialogex.h"


// CNewBookingDlg dialog

IMPLEMENT_DYNAMIC(CNewBookingDlg, CDialog)

CNewBookingDlg::CNewBookingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewBookingDlg::IDD, pParent)
{
	m_sInputValue = _T("");
}

CNewBookingDlg::~CNewBookingDlg()
{
}

void CNewBookingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT, m_ctlEdit);
	DDX_Text(pDX, IDC_EDIT, m_sInputValue);
}


BEGIN_MESSAGE_MAP(CNewBookingDlg, CDialog)
END_MESSAGE_MAP()


// CNewBookingDlg message handlers

const TCHAR * CNewBookingDlg::GetInputValue()
{
	return m_sInputValue;
}
