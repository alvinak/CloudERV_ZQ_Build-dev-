// ChildFrm.h : interface of the CChildFrame class
//


#pragma once


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

	typedef struct {
		unsigned short	nID;
		BOOL bCaptionOnly;
	} CaptionToolBarButtonInfo;

	virtual CaptionToolBarButtonInfo *GetCaptionToolBarButtonInfoList() { return NULL; }

// Attributes
public:
	CXTPCommandBars	*m_lpxtpCmdBars; 

// Operations
public:

// Overrides

	virtual void ActivateFrame(int nCmdShow);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
};
