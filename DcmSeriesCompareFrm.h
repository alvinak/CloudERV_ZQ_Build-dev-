#pragma once

// CDcmSeriesCompareFrame frame
#include "ChildFrm.h"

class CDcmSeriesCompareFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CDcmSeriesCompareFrame)
protected:
	CDcmSeriesCompareFrame();           // protected constructor used by dynamic creation
	virtual ~CDcmSeriesCompareFrame();

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

};


