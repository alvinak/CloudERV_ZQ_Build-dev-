// WebBrowserFrm.cpp : implementation file
//

#include "stdafx.h"
#include "WebBrowserFrm.h"


// CWebBrowserFrame

IMPLEMENT_DYNCREATE(CWebBrowserFrame, CChildFrame)

CWebBrowserFrame::CWebBrowserFrame()
{

}

CWebBrowserFrame::~CWebBrowserFrame()
{
}


BEGIN_MESSAGE_MAP(CWebBrowserFrame, CChildFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()


const CChildFrame::CaptionToolBarButtonInfo g_lstButtonsOfWebBrowser[] = {
/*
	{ID_PDFREPORT_ZOOMIN,	TRUE},
	{ID_PDFREPORT_ZOOMOUT,	TRUE},
	{ID_PDFREPORT_PREVPAGE, TRUE},
	{ID_PDFREPORT_NEXTPAGE, TRUE},
	{ID_PDFREPORT_PRINT,	TRUE},
	{ID_PDFREPORT_EXPORT,	TRUE},
	{ID_CHILDFRAME_CLOSE,	TRUE},
	*/
	{0, FALSE}
};

CChildFrame::CaptionToolBarButtonInfo * CWebBrowserFrame::GetCaptionToolBarButtonInfoList() 
{ 
	return (CChildFrame::CaptionToolBarButtonInfo *)g_lstButtonsOfWebBrowser;
}

// CWebBrowserFrame message handlers

int CWebBrowserFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	/*
	
  // Create ToolBar 
     CXTPToolBar* lpxtpToolBar = (CXTPToolBar*)m_lpxtpCmdBars->Add( L"Standard", xtpBarTop ) ; 

     if( ! lpxtpToolBar || !lpxtpToolBar->LoadToolBar( IDR_PDFREPORT )) 
     { 
		 AfxMessageBox(L"Failed to create toolbar");

		 TRACE0( "Failed to create toolbar\n" ) ; 
          return -1 ; 
     } 

     lpxtpToolBar->SetShowGripper( FALSE ) ; 
     lpxtpToolBar->ShowExpandButton( FALSE ) ; 
     lpxtpToolBar->EnableDocking( xtpFlagAlignBottom ) ; 
	 */

	return 0;
}
