// ProgressWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd dialog


CProgressWnd::CProgressWnd(CWnd* pParent /*=NULL*/)
	: CXTPDialog(CProgressWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProgressWnd::DoDataExchange(CDataExchange* pDX)
{
	CXTPDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressWnd)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressWnd, CXTPDialog)
	//{{AFX_MSG_MAP(CProgressWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CProgressWnd::Create(CWnd* pParentWnd) 
{	
	return CXTPDialog::Create(IDD, pParentWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd message handlers

BOOL CProgressWnd::OnInitDialog() 
{
	CXTPDialog::OnInitDialog();
	
	return TRUE;  
}

/////////////////////////////////////////////////////////////////////////////

void CProgressWnd::ChangePos(int nRemaining,int nCompleted,const TCHAR *szMsg)
{
	int nMax;
	CString sText;
	CWnd *pWnd;
	
	nMax = nRemaining + nCompleted;


	SetWindowText(_T("正在传输影像资料..."));

	if (_tcslen(szMsg) == 0)
	{
		sText.Format(_T("共计: %d, 已完成: %d   还剩: %d"),nMax,nCompleted,nRemaining);
	}
	else
	{
		sText.Format(_T("%s"),szMsg);
	}


	pWnd = GetDlgItem(IDC_MSG);
	if (pWnd != NULL) pWnd->SetWindowText(sText);


	m_ctlProgress.SetRange(1,nMax);
	m_ctlProgress.SetPos(nCompleted);
	m_ctlProgress.Invalidate();

}

/////////////////////////////////////////////////////////////////////////////
