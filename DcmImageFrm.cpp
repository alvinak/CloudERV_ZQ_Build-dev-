// DcmImageFrm.cpp : implementation file
//

#include "stdafx.h"
#include "DcmImageFrm.h"


// CDcmImageFrame

IMPLEMENT_DYNCREATE(CDcmImageFrame, CChildFrame)

CDcmImageFrame::CDcmImageFrame()
{

}

CDcmImageFrame::~CDcmImageFrame()
{
}


BEGIN_MESSAGE_MAP(CDcmImageFrame, CChildFrame)
	ON_WM_CREATE()
	ON_XTP_CREATECONTROL()
END_MESSAGE_MAP()

/*
const CChildFrame::CaptionToolBarButtonInfo g_lstButtonsOfDcmImage[] = {
	{ID_CHILDFRAME_CLOSE,	TRUE},
	{0, FALSE}
};
*/

CChildFrame::CaptionToolBarButtonInfo * CDcmImageFrame::GetCaptionToolBarButtonInfoList() 
{ 
	return NULL;//(CChildFrame::CaptionToolBarButtonInfo *)g_lstButtonsOfDcmImage;
}

// CDcmImageFrame message handlers

int CDcmImageFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  // Create ToolBar 
     CXTPToolBar* lpxtpToolBar = (CXTPToolBar*)m_lpxtpCmdBars->Add( L"Standard", xtpBarTop ) ; 

     if( ! lpxtpToolBar || !lpxtpToolBar->LoadToolBar( IDR_DCMIMAGE )) 
     { 
		 AfxMessageBox(L"Failed to create toolbar");

		 TRACE0( "Failed to create toolbar\n" ) ; 
          return -1 ; 
     } 

     lpxtpToolBar->SetShowGripper( FALSE ) ; 
     lpxtpToolBar->ShowExpandButton( FALSE ) ; 
     lpxtpToolBar->EnableDocking( xtpFlagAlignBottom ) ; 
	 
	 //LoadAccelTable(MAKEINTRESOURCE(IDR_DCMIMAGE));

	return 0;
}

int CDcmImageFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	BOOL bHandled = FALSE;

	if (lpCreateControl->bToolBar) {

		switch(lpCreateControl->nID) {
		case ID_CINE_PLAY:
		case ID_DCMVIEW_LBFUNC4:
		case ID_DCMVIEW_RBFUNC1:
		case ID_DCMVIEW_RBFUNC2:
		case ID_DCMVIEW_RBFUNC3:
		case ID_DCMVIEW_COLORMAP:
		case ID_DCMVIEW_OPTIONS:
			lpCreateControl->controlType = xtpControlSplitButtonPopup;
			bHandled = TRUE;
			break;
		}

	}

	if (!bHandled) 
		return CChildFrame::OnCreateControl(lpCreateControl);

	return TRUE;
}
