// BitmapViewerFrm.cpp : implementation file
//

#include "stdafx.h"
#include "BitmapViewerFrm.h"


// CBitmapViewerFrame

IMPLEMENT_DYNCREATE(CBitmapViewerFrame, CChildFrame)

CBitmapViewerFrame::CBitmapViewerFrame()
{

}

CBitmapViewerFrame::~CBitmapViewerFrame()
{
}


BEGIN_MESSAGE_MAP(CBitmapViewerFrame, CChildFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()


const CChildFrame::CaptionToolBarButtonInfo g_lstButtonsOfBmpViewer[] = {
	{ID_BMPVIEWER_FITWIDTH,		TRUE},
	{ID_BMPVIEWER_FITWINDOW,	TRUE},
	{ID_BMPVIEWER_ORIGINALSIZE, TRUE},
	{ID_BMPVIEWER_ZOOMIN,		TRUE},
	{ID_BMPVIEWER_ZOOMOUT,		TRUE},
	{ID_BMPVIEWER_MOVETOPREV,	TRUE},
	{ID_BMPVIEWER_MOVETONEXT,	TRUE},
	{ID_BMPVIEWER_PRINT,		TRUE},
	{ID_CHILDFRAME_CLOSE,		TRUE},
	{0, FALSE}
};

CChildFrame::CaptionToolBarButtonInfo * CBitmapViewerFrame::GetCaptionToolBarButtonInfoList() 
{ 
	return (CChildFrame::CaptionToolBarButtonInfo *)g_lstButtonsOfBmpViewer;
}

// CBitmapViewerFrame message handlers

int CBitmapViewerFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	
  // Create ToolBar 
     CXTPToolBar* lpxtpToolBar = (CXTPToolBar*)m_lpxtpCmdBars->Add( L"Standard", xtpBarTop ) ; 

     if( ! lpxtpToolBar || !lpxtpToolBar->LoadToolBar( IDR_BITMAPVIEWER )) 
     { 
		 AfxMessageBox(L"Failed to create toolbar");

		 TRACE0( "Failed to create toolbar\n" ) ; 
          return -1 ; 
     } 

     lpxtpToolBar->SetShowGripper( FALSE ) ; 
     lpxtpToolBar->ShowExpandButton( FALSE ) ; 
     lpxtpToolBar->EnableDocking( xtpFlagAlignBottom ) ; 
	 

	return 0;
}
