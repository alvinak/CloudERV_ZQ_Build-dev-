// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "CloudERV.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_XTP_CREATECONTROL()
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
	if (m_lpxtpCmdBars)
		m_lpxtpCmdBars->InternalRelease(); 
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.x = cs.y = 0;
	cs.cx = cs.cy = 32767;

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_lpxtpCmdBars = CXTPCommandBars::CreateCommandBars(); 
	if (!m_lpxtpCmdBars)
		return -1;

	m_lpxtpCmdBars->GetToolTipContext()->SetStyle( xtpToolTipStandard ) ; 
	m_lpxtpCmdBars->SetSite( this ) ;

    m_lpxtpCmdBars->EnableCustomization( FALSE ) ; 
	m_lpxtpCmdBars->EnableDocking();

	return 0;
}


int CChildFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	BOOL bCaptionOnly = TRUE, bFound = FALSE;
	CaptionToolBarButtonInfo *pTBInfo;

	if (lpCreateControl->bToolBar) {

		bFound = FALSE;
		pTBInfo = GetCaptionToolBarButtonInfoList();
		while (pTBInfo && pTBInfo->nID > 0 && !bFound) {

			if (pTBInfo->nID == lpCreateControl->nID) {
				bFound = TRUE;
				bCaptionOnly = pTBInfo->bCaptionOnly;
			}
			else
				pTBInfo ++;
		}
		
		if (bFound) {

			CXTPControlButton* pButton = (CXTPControlButton*)CXTPControlButton::CreateObject();
		
			lpCreateControl->pControl = pButton;
			pButton->SetID(lpCreateControl->nID);
			pButton->SetStyle(bCaptionOnly ? xtpButtonCaption : xtpButtonIconAndCaption);

			return TRUE;
		}
	}

	return FALSE;
}

void CChildFrame::ActivateFrame(int nCmdShow)
{
	if (GetParent()->GetWindow(GW_CHILD) == this)
	{
//		nCmdShow = SW_SHOWMAXIMIZED;
	}

	CMDIChildWnd::ActivateFrame(nCmdShow);
}


#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers
