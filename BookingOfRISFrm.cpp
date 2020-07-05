// BookingOfRISFrm.cpp : implementation file
//

#include "stdafx.h"
#include "BookingOfRISFrm.h"


// CBookingOfRISFrame

IMPLEMENT_DYNCREATE(CBookingOfRISFrame, CChildFrame)

CBookingOfRISFrame::CBookingOfRISFrame()
{

}

CBookingOfRISFrame::~CBookingOfRISFrame()
{
}


BEGIN_MESSAGE_MAP(CBookingOfRISFrame, CChildFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()


const CChildFrame::CaptionToolBarButtonInfo g_lstButtonsOfBookingOfRIS[] = {
	{ID_BOOKINGRIS_NEW,		TRUE},
	{ID_BOOKINGRIS_APPLY,	TRUE},
	{ID_BOOKINGRIS_CANCEL,	TRUE},
	{ID_BOOKINGRIS_QUERY,	TRUE},
	{ID_BOOKINGRIS_RETURN,	TRUE},
	{ID_CHILDFRAME_CLOSE,	TRUE},
	{0, FALSE}
};

CChildFrame::CaptionToolBarButtonInfo * CBookingOfRISFrame::GetCaptionToolBarButtonInfoList() 
{ 
	return (CChildFrame::CaptionToolBarButtonInfo *)g_lstButtonsOfBookingOfRIS;
}

// CBookingOfRISFrame message handlers

int CBookingOfRISFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	
  // Create ToolBar 
     CXTPToolBar* lpxtpToolBar = (CXTPToolBar*)m_lpxtpCmdBars->Add( L"Standard", xtpBarTop ) ; 

     if( ! lpxtpToolBar || !lpxtpToolBar->LoadToolBar( IDR_BOOKINGOFRIS )) 
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
