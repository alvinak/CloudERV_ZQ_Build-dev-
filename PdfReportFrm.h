#pragma once

// CPdfReportFrame frame
#include "ChildFrm.h"

class CPdfReportFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CPdfReportFrame)
protected:
	CPdfReportFrame();           // protected constructor used by dynamic creation
	virtual ~CPdfReportFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


