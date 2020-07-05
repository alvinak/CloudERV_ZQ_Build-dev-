// PdfReportView.h : interface of the CPdfReportView class
#pragma once

#include "PdfViewer.h"

#include "MuPDFClass.h"

class CPdfReportDoc;
class CPdfReportView : public CView
{
protected: // create from serialization only
	CPdfReportView();
	DECLARE_DYNCREATE(CPdfReportView)

// Attributes
public:
	CPdfReportDoc* GetDocument() const;

	MuPDFClass	*m_pPDF;
	CPdfViewer	m_wndPdfViewer;
// Operations
public:

	/////////////////////////////////////////////////////
// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

	BOOL	m_bInitialized;
// Implementation
public:
	virtual ~CPdfReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);
	afx_msg void OnExecuteActions(UINT nCommandID);
	
};

#ifndef _DEBUG  // debug version in PdfReportView.cpp
inline CPdfReportDoc* CPdfReportView::GetDocument() const
   { return reinterpret_cast<CPdfReportDoc*>(m_pDocument); }
#endif

