#pragma once

// CPublishedReportFrame frame
#include "ChildFrm.h"

class CPublishedReportFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CPublishedReportFrame)
protected:
	CPublishedReportFrame();           // protected constructor used by dynamic creation
	virtual ~CPublishedReportFrame();

private:
	float	m_fSplitterColumn0Ratio;
	BOOL	m_bSplitterColumn0WidthChanged;

protected:
	DECLARE_MESSAGE_MAP()
	CChildFrame::CaptionToolBarButtonInfo * GetCaptionToolBarButtonInfoList(); 

	CXTPSplitterWnd  m_wndSplitter;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg void OnDestroy();
};


