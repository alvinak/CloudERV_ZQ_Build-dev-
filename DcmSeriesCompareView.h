#pragma once

#include "DicomViewer.h"
#include "SetImgLayout.h"
#include "WinLevel.h"
#include "PresetWL.h"

#include "OptionsWinLevelDlg.h"
#include "SensitivityDlg.h"

// CDcmSeriesCompareView view
class CDcmSeriesCompareDoc;
class CDcmSeriesCompareView : public CView
{
	DECLARE_DYNCREATE(CDcmSeriesCompareView)

protected:
	CDcmSeriesCompareView();           // protected constructor used by dynamic creation
	virtual ~CDcmSeriesCompareView();

public:

	CDicomViewer m_wndDicomViewer;
	CDcmSeriesCompareDoc* GetDocument() const;
	CDicomViewer *GetDicomViewer() { return &m_wndDicomViewer; }
	void SetImgLayout(int nRows, int nCols, BOOL bCustom);
	BOOL IsDicomViewerEmpty();
	void SetWinLevel(double fCen, double fWid);
private:
	int	m_nTextColor, m_nBkColor, m_nBkMode;
	int m_nSeriesRows, m_nSeriesCols, m_nRows,m_nCols;
	int m_nSeriesRows2,m_nSeriesCols2,m_nRows2,m_nCols2;
	int		m_nLBFuncNo;
	int		m_nRBFuncNo;
	int		m_nSensitivity;
	BOOL	m_bDrawOverlay;
	BOOL	m_bInitialized;

	UINT	m_nSeriesGoIDs[40];

	int		m_nFakeColorSet;
	CustomWinLevelStruct m_CustomWinLevel[10];

	CSetImgLayout	m_wndImgLayout;
	CWinLevel		m_wndWinLevel;
	CPresetWL		m_wndPresetWL;
	COptionsWinLevelDlg m_wndWinLevelOptions;
	CSensitivityDlg	m_wndSensitivityDlg;

	void	ShowMessage(int nCode);
	void	ShowMessage(const TCHAR *szMsg);

	void	ReadIniValues();
	void	ShowOptionsPopupMenu();
	void	SeriesPopupMenu();
	void	SeriesGo(UINT nID);
	void	SetFakeColor(UINT nID);
	void	ExportImages();
public:
	void SetWinLevelOptions(CustomWinLevelStruct *pWinLevel);
	void SetSensitivity(int nValue);

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCellWndClick(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnCellWndRBClick(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);
	afx_msg void OnExecuteActions(UINT nCommandID);
};




#ifndef _DEBUG  // debug version in DcmSeriesCompareView.cpp
inline CDcmSeriesCompareDoc* CDcmSeriesCompareView::GetDocument() const
   { return reinterpret_cast<CDcmSeriesCompareDoc*>(m_pDocument); }
#endif

