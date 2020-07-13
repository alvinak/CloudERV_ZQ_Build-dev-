#pragma once

#include "PublishedReportDoc.h"

// CMedicalResultView form view

class CMedicalResultView : public CXTPResizeFormView
{
	DECLARE_DYNCREATE(CMedicalResultView)

protected:
	CMedicalResultView();           // protected constructor used by dynamic creation
	virtual ~CMedicalResultView();

public:
	enum { IDD = IDD_MEDICALRESULT };

	CPublishedReportDoc* GetDocument() const;

	void PopulateFields();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	BOOL	m_bInitialized;
	BOOL	m_bPositiveFlag;
	BOOL	m_bCriticalFlag;
	BOOL	m_bInfectiousFlag;
	BOOL	m_bEnhancedFlag;

	CStatic m_txtTitle;
	CStatic m_txtPatientName;
	CStatic m_txtPatientSex;
	CStatic m_txtPatientAge;
	CStatic m_txtOrderNo;
	CStatic	m_txtDocID;
	CStatic m_txtInpatientNo;
	CStatic m_txtBedNo;
	CStatic m_txtStudyDate;
	CStatic m_txtStudyDetails;
	CXTPEdit m_edtFindings;
	CXTPEdit m_edtConclusions;
	CStatic m_txtReportDate;
	CStatic m_txtReportDoctorName;
	CStatic m_txtReviewDate;
	CStatic m_txtReviewDoctorName;
	CStatic m_txtPositiveFlag;
	CStatic m_txtCriticalFlag;
	CStatic m_txtWarnContent;
	CStatic m_txtInfectiousFlag;
	CStatic m_txtEnhancedFlag;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);
	afx_msg void OnExecuteActions(UINT nCommandID);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

};


#ifndef _DEBUG  // debug version in PdfReportView.cpp
inline CPublishedReportDoc* CMedicalResultView::GetDocument() const
   { return reinterpret_cast<CPublishedReportDoc*>(m_pDocument); }
#endif

