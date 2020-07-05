// WinLevel.cpp : implementation file
//

#include "stdafx.h"
#include "WinLevel.h"
#include "DcmImageView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinLevel dialog


CWinLevel::CWinLevel(CWnd* pParent /*=NULL*/)
	: CDialog(CWinLevel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinLevel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCen = 0;
	m_nWid = 0;
}


void CWinLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinLevel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_WINLEVEL_SPINCEN, m_spnCen);
  DDX_Control(pDX, IDC_WINLEVEL_SPINWID, m_spnWid);
  DDX_Control(pDX, IDC_WINLEVEL_CEN, m_edtCen);
  DDX_Control(pDX, IDC_WINLEVEL_WID, m_edtWid);
}


BEGIN_MESSAGE_MAP(CWinLevel, CDialog)
	//{{AFX_MSG_MAP(CWinLevel)
	ON_BN_CLICKED(IDC_WINLEVEL_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CWinLevel::Create(CWnd* pParentWnd) 
{	
	return CDialog::Create(IDD, pParentWnd);
}


void CWinLevel::SetWinLevel(double fcen,double fwid)
{
	CString strcen,strwid;
	TCHAR sch[20];

	m_nCen = (int) fcen;
	m_nWid = (int) fwid;

	if (m_nCen == 0 && m_nWid == 0)
	{
		m_nWid = 255;
		m_nCen = 0;
	}

	m_edtCen.SetWindowText(_itot(m_nCen, sch, 10));
	m_edtWid.SetWindowText(_itot(m_nWid, sch, 10));

	UpdateData(FALSE);

}

/////////////////////////////////////////////////////////////////////////////
// CWinLevel message handlers

BOOL CWinLevel::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_spnWid.SetRange32(1,65537);
	m_spnCen.SetRange32(-65537,65537);

	m_spnWid.SetPos(m_nWid);
	m_spnCen.SetPos(m_nCen);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWinLevel::OnApply() 
{
	CDcmImageView *pDcmImageView;
	UpdateData(TRUE);

	m_nCen = m_spnCen.GetPos();
	m_nWid = m_spnWid.GetPos();	


	pDcmImageView = (CDcmImageView *) m_pDcmImageViewObj;

	pDcmImageView->SetWinLevel((double) m_nCen,(double) m_nWid);

	CDialog::OnOK();	
}


