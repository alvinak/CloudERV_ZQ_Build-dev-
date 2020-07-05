// MiniToolBar.cpp : implementation file
//

#include "stdafx.h"

#include "MiniToolBar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiniToolBar dialog


CMiniToolBar::CMiniToolBar(CWnd* pParent /*=NULL*/)
	: CDialog(CMiniToolBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMiniToolBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMiniToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiniToolBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMiniToolBar, CDialog)
	//{{AFX_MSG_MAP(CMiniToolBar)
	ON_WM_CREATE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(32771,65535,OnExecuteAction)
	ON_UPDATE_COMMAND_UI_RANGE(32771,65535, OnUpdateAction)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)
END_MESSAGE_MAP()



BOOL CMiniToolBar::PreTranslateMessage(MSG* pMsg) 
{
	static BOOL bDoIdle = TRUE;
	MSG msg;

	if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && bDoIdle)
	{
		m_wndMiniToolBar.OnUpdateCmdUI((CFrameWnd *)this, TRUE);

		bDoIdle = FALSE;
	}
	else
	{
		if(AfxGetApp()->IsIdleMessage(pMsg) && pMsg->message != 0x3FC)
		{
			bDoIdle = TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CMiniToolBar::OnMove(int x, int y)
{	
	CDialog::OnMove(x, y);

	GetWindowRect(&m_rtWindowPos);
}



void CMiniToolBar::OnUpdateAction(CCmdUI *pCmdUI)
{
	/*
	CMainForm *pMainForm;

	pMainForm = (CMainForm *) m_pMainFormObj;

	if (pMainForm != NULL)
	{
		pMainForm->OnUpdateAction(pCmdUI);
	}
	*/
}


void CMiniToolBar::OnExecuteAction(UINT nID)
{
	/*
	CMainForm *pMainForm;

	pMainForm = (CMainForm *) m_pMainFormObj;

	if (pMainForm != NULL)
	{
		pMainForm->OnExecuteAction(nID);
	}
	*/

	return;
}

BOOL CMiniToolBar::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	/*
	CMainForm *pMainForm;

	pMainForm = (CMainForm *) m_pMainFormObj;

	if (pMainForm != NULL)
	{
		return pMainForm->OnToolTipText(nID,pNMHDR,pResult);
	}
	
	*/
	return FALSE;
}


void CMiniToolBar::OnToolbarDropDown(NMHDR* pnmtb, LRESULT *plr)
{
	/*
	CMainForm *pMainForm;

	pMainForm = (CMainForm *) m_pMainFormObj;

	if (pMainForm != NULL)
	{
		pMainForm->OnToolbarDropDown(pnmtb,plr);
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CMiniToolBar message handlers

BOOL CMiniToolBar::Create(CWnd* pParentWnd) 
{
	return CDialog::Create(IDD, pParentWnd);
}

int CMiniToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	DWORD dwStyle,dwExStyle;

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndMiniToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |  WS_TABSTOP | CBRS_TOP 
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndMiniToolBar.LoadToolBar(IDR_MINITOOLBAR))
	{
		return -1;
	}
	

	dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_wndMiniToolBar.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);

	dwStyle = m_wndMiniToolBar.GetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_RBFUNC1));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_wndMiniToolBar.SetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_RBFUNC1), dwStyle);

	dwStyle = m_wndMiniToolBar.GetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_RBFUNC2));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_wndMiniToolBar.SetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_RBFUNC2), dwStyle);

	dwStyle = m_wndMiniToolBar.GetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_RBFUNC3));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_wndMiniToolBar.SetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_RBFUNC3), dwStyle);


	dwStyle = m_wndMiniToolBar.GetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_LBFUNC4));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_wndMiniToolBar.SetButtonStyle(m_wndMiniToolBar.CommandToIndex(ID_DCMVIEW_LBFUNC4), dwStyle);
	


	return 0;
}

BOOL CMiniToolBar::OnInitDialog() 
{
	CRect rt1,rt2;
	CSize sz;

	CDialog::OnInitDialog();
	
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_wndMiniToolBar.GetToolBarCtrl().GetMaxSize(&sz);
	GetWindowRect(&rt1);
	GetClientRect(&rt2);

	rt1.right = rt1.left + rt1.Width() - rt2.Width() + sz.cx + 90;
	rt1.bottom = rt1.top + rt1.Height() - rt2.Height() + sz.cy + 4; 
	
	MoveWindow(&rt1);

	m_wndMiniToolBar.OnUpdateCmdUI((CFrameWnd *)this, TRUE);

	Invalidate();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

