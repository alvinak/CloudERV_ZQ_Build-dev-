#pragma once

// CBitmapViewerFrame frame
#include "ChildFrm.h"

class CBitmapViewerFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CBitmapViewerFrame)
protected:
	CBitmapViewerFrame();           // protected constructor used by dynamic creation
	virtual ~CBitmapViewerFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


