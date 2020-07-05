#pragma once

// CWebBrowserFrame frame
#include "ChildFrm.h"

class CWebBrowserFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CWebBrowserFrame)
protected:
	CWebBrowserFrame();           // protected constructor used by dynamic creation
	virtual ~CWebBrowserFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


