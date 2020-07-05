#pragma once

// CBookingOfRISFrame frame
#include "ChildFrm.h"

class CBookingOfRISFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CBookingOfRISFrame)
protected:
	CBookingOfRISFrame();           // protected constructor used by dynamic creation
	virtual ~CBookingOfRISFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


