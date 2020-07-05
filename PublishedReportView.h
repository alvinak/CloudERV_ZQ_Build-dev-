#pragma once

#include "PublishedReportDoc.h"

// CPublishedReportView form view

class CPublishedReportView : public CXTPResizeFormView
{
	DECLARE_DYNCREATE(CPublishedReportView)

protected:
	CPublishedReportView();           // protected constructor used by dynamic creation
	virtual ~CPublishedReportView();

private:
	BOOL m_bReportColumnWidthChanged;

public:
	enum { IDD = IDD_PUBLISHEDREPORT };

	CPublishedReportDoc* GetDocument() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	BOOL	m_bInitialized, m_bDataLoading;

	CString			m_sStudyGUIDSaved;

	CString			m_sFieldsOfUIS;
	CString			m_sFieldsOfEIS;
	CString			m_sFieldsOfPIS;
	CString			m_sFieldsOfPIS_1;
	CString			m_sFieldsOfRIS;
	CString			m_sFieldsOfPET;

	CDateTimeCtrl m_dtpReportDateFrom;
	CDateTimeCtrl m_dtpReportDateTo;

	CXTPComboBox	m_cboExamType;
	CXTPComboBox	m_cboFieldName;
	CXTPBrowseEdit	m_edtFieldValue;

	CXTPReportControl	m_ctlResult;

	void	ViewImageMode1(int nTag);
	void	ViewImageMode2();
	void	ViewImageMode3(int nTag);

public:
	void	DoQuery(const TCHAR *szSystem, const TCHAR *szOrderNo, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szPatientID, const TCHAR *szDocID);
private:
	
	void	GetCommandTextByReportDate_XIS(const TCHAR *szSystem, const TCHAR *szDtFrom, const TCHAR *szDtTo, CString &sCommandText);
	void	GetCommandTextByOrderNo_XIS(const TCHAR *szSystem, const TCHAR *szOrderNo, CString &sCommandText);
	void	GetCommandTextByInpatientNoOrOutpatientNo_XIS(const TCHAR *szSystem, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sCommandText);
	void	GetCommandTextByPatientID_XIS(const TCHAR *szSystem, const TCHAR *szPatientID, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sCommandText);
	void    GetCommandTextByDocID_XIS(const TCHAR *szSystem, const TCHAR *szDocID,  CString &sCommandText);
	void	GetCommandTextByPatientName_XIS(const TCHAR *szSystem, const TCHAR *szPatientName, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sCommandText);
	void	GetCommandTextByRefDoctor_XIS(const TCHAR *szSystem, const TCHAR *szRefDoctor, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sCommandText);
	void	GetCommandTextByRefDept_XIS(const TCHAR *szSystem, const TCHAR *szRefDept, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sCommandText);

	void	GetSystemCodeExp(const TCHAR *szSystem, CString &sSystemCodeExp);

	void	GetWhereExpByReportDate_HCS(const TCHAR *szSystem, const TCHAR *szDtFrom, const TCHAR *szDtTo, CString &sWhereExp);
	void	GetWhereExpByOrderNo_HCS(const TCHAR *szSystem, const TCHAR *szOrderNo, CString &sWhereExp);
	void	GetWhereExpByInpatientNoOrOutpatientNo_HCS(const TCHAR *szSystem, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sWhereExp);
	void	GetWhereExpByPatientID_HCS(const TCHAR *szSystem, const TCHAR *szPatientID, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sWhereExp);
	void    GetWhereExpByDocID_HCS(const TCHAR *szSystem, const TCHAR *szDocID,  CString &sCommandText);
	void	GetWhereExpByPatientName_HCS(const TCHAR *szSystem, const TCHAR *szPatientName, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sWhereExp);
	void	GetWhereExpByRefDoctor_HCS(const TCHAR *szSystem, const TCHAR *szRefDoctor, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sWhereExp);
	void	GetWhereExpByRefDept_HCS(const TCHAR *szSystem, const TCHAR *szRefDept, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sWhereExp);

	void	ViewOriginalReport();

	void	LoadHeaderWidthOfReportControl(CXTPReportControl &wndReport, const TCHAR *szSectionName, const TCHAR *szKeyName);
	void	SaveHeaderWidthOfReportControl(CXTPReportControl &wndReport, const TCHAR *szSectionName, const TCHAR *szKeyName);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	afx_msg void OnFieldValueBrowse();
	afx_msg void OnReportSelChanged(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnReportPresortOrderChanged(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnReportSortOrderChanged(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnReportItemDblClick(NMHDR * pNotifyStruct, LRESULT * /*result*/);
	afx_msg void OnReportItemRClick(NMHDR * pNotifyStruct, LRESULT * /*result*/);
	afx_msg void OnReportColumnWidthChanged(NMHDR *pNotifyStruct, LRESULT * /*result*/);

	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewImageMode2(CCmdUI *pCmdUI);
	afx_msg void OnExecuteActions(UINT nCommandID);
	afx_msg void OnCbnSelchangeCboFieldname();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnDestroy();
};


#ifndef _DEBUG  // debug version in PublishedReportView.cpp
inline CPublishedReportDoc* CPublishedReportView::GetDocument() const
   { return reinterpret_cast<CPublishedReportDoc*>(m_pDocument); }
#endif

