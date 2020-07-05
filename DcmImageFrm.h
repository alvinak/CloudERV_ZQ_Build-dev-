#pragma once

// CDcmImageFrame frame
#include "ChildFrm.h"

class CDcmImageFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CDcmImageFrame)
protected:
	CDcmImageFrame();           // protected constructor used by dynamic creation
	virtual ~CDcmImageFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

};


