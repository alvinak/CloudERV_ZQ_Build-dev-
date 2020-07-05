// DcmSeriesCompareFrm.cpp : implementation file
//

#include "stdafx.h"
#include "DcmSeriesCompareFrm.h"


// CDcmSeriesCompareFrame

IMPLEMENT_DYNCREATE(CDcmSeriesCompareFrame, CChildFrame)

CDcmSeriesCompareFrame::CDcmSeriesCompareFrame()
{

}

CDcmSeriesCompareFrame::~CDcmSeriesCompareFrame()
{
}


BEGIN_MESSAGE_MAP(CDcmSeriesCompareFrame, CChildFrame)
	ON_WM_CREATE()
	ON_XTP_CREATECONTROL()
END_MESSAGE_MAP()



CChildFrame::CaptionToolBarButtonInfo * CDcmSeriesCompareFrame::GetCaptionToolBarButtonInfoList() 
{ 
	return NULL;
}

// CDcmSeriesCompareFrame message handlers

int CDcmSeriesCompareFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  // Create ToolBar 
     CXTPToolBar* lpxtpToolBar = (CXTPToolBar*)m_lpxtpCmdBars->Add( L"Standard", xtpBarTop ) ; 

     if( ! lpxtpToolBar || !lpxtpToolBar->LoadToolBar( IDR_DCMSERIESCMP )) 
     { 
		 AfxMessageBox(L"Failed to create toolbar");

		 TRACE0( "Failed to create toolbar\n" ) ; 
          return -1 ; 
     } 

     lpxtpToolBar->SetShowGripper( FALSE ) ; 
     lpxtpToolBar->ShowExpandButton( FALSE ) ; 
     lpxtpToolBar->EnableDocking( xtpFlagAlignBottom ) ; 
	 
	 //LoadAccelTable(MAKEINTRESOURCE(IDR_DCMSERIESCMP));

	return 0;
}

int CDcmSeriesCompareFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
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
