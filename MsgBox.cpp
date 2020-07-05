// MsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "MsgBox.h"
#include "CloudERV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgBox dialog


CMsgBox::CMsgBox(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bInited = FALSE;

	m_nTimerId = 0;
}


CMsgBox::~CMsgBox()
{

}


void CMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgBox, CDialog)
	//{{AFX_MSG_MAP(CMsgBox)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgBox message handlers

BOOL CMsgBox::Create(CWnd* pParentWnd) 
{
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CMsgBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bInited = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CMsgBox::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN)
	{
		ShowWindow(SW_HIDE);
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CMsgBox::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimerId)
	{
		ShowWindow(SW_HIDE);
	}
	
	CDialog::OnTimer(nIDEvent);
}



void CMsgBox::ShowMessage(const TCHAR *szMsg)
{
	CWnd *pWnd;

	if (!m_bInited)
	{
		OnInitDialog();
	}

	pWnd = GetDlgItem(IDC_MSG);

	if (pWnd != NULL)
	{
		pWnd->SetWindowText(szMsg);
	}

	if (m_nTimerId > 0)
	{
		KillTimer(m_nTimerId);
	}

	m_nTimerId = 1;

	SetTimer(m_nTimerId,1000 * 30,NULL);

	ShowWindow(SW_SHOW);
	UpdateWindow();
	BringWindowToTop();

}


void CMsgBox::OnDestroy()
{
	if (m_nTimerId > 0)
	{
		KillTimer(m_nTimerId);
	}

	CDialog::OnDestroy();
}

