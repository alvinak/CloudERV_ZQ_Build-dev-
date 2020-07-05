#pragma once


#include "BitmapViewer.h"
#include "MemoryDC.h"



class CBitmapViewerView : public CBitmapViewer
{
protected: // create from serialization only
	CBitmapViewerView();
	DECLARE_DYNCREATE(CBitmapViewerView)

// Attributes
private:

	BOOL m_bShowText;
	int	m_nZoomMode;
// Operations
public:

// Overrides
protected:
   virtual void DrawOverlay(CMemoryDC* pDCDraw, CDC* pDCView);
   
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapViewerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapViewerView)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);
	afx_msg void OnExecuteActions(UINT nCommandID);
};

