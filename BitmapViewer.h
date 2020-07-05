// BitmapViewer.h : header file

#pragma once

#include <vector>

class CBitmapViewerDoc;

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewer view

class CMemoryDC;

class CBitmapViewer : public CScrollView
{
   DECLARE_DYNAMIC(CBitmapViewer)

// Construction
protected:
	CBitmapViewer();
   virtual ~CBitmapViewer() = 0;

// Attributes
public:
	CBitmapViewerDoc* GetDocument() const;

// Operations
public:
   
   BOOL SetBackPatternBrush(UINT nBitmapResID);
   BOOL SetBackSolidBrush(COLORREF crColor);
   void SetHandCursor(UINT nCursorResID);
   void SetHandDragCursor(UINT nCursorResID);

   void ZoomIn();
   void ZoomOut();
   void ZoomToOriginalSize();
   void ZoomToWindowWidth();
   void ZoomToWindow();

   void MoveToPrevImage();
   void MoveToNextImage();

   BOOL IsMoveToPrevEnabled();
   BOOL IsMoveToNextEnabled();
   
   int GetZoomMode() { return m_nZoomMode; }
   void ZoomToDefault();

   void PrintImage();

// Overrides
protected:

   virtual void ZoomToRectangle(const CRect& rect);
   virtual void DrawBackgound(CMemoryDC* pDCDraw);
   virtual void DrawImage(CMemoryDC* pDCDraw, CDC* pDCView);
   virtual void DrawOverlay(CMemoryDC* pDCDraw, CDC* pDCView) = 0;

// Attributes
private:

	int m_nZoomMode;
	double m_fZoom;
	const double m_fZoomStep;
	const double m_fMaxZoom;
	const double m_fMinZoom;
	HCURSOR m_hCursorHand;
	HCURSOR m_hCursorHandDrag;
	CPoint m_ptStartDrag;
	CPoint m_ptCurrDrag;
	CBitmap m_bmpBack;
	CBrush m_brushBack;

// Implementation
private:
	void _SetScrollSizes();
	void _HandDrag(CPoint point);
	void _SelectRectangle(CPoint point);
   

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapViewer)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDCView); 
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapViewer)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


#ifndef _DEBUG  // debug version in BitmapViewer.cpp
inline CBitmapViewerDoc* CBitmapViewer::GetDocument() const
   { return reinterpret_cast<CBitmapViewerDoc*>(m_pDocument); }
#endif
