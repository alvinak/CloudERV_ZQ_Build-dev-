// MainFrm.h : interface of the CMainFrame class
//
#pragma once

#include "MsgBox.h"
#include "ProgressWnd.h"

class CCloudERVApp;

class CMainFrame : public CXTPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
		
	CMsgBox			m_wndMsgBox;
	CProgressWnd	m_wndProgress;
	void ShowProgressEx(BOOL bShow,int nRemaining,int nCompleted,const TCHAR *szMsg = _T(""));
	void ShowMessageEx(const TCHAR *szMsg);
	void ShowMessageEx(int nCode);
// Operations
public:
	CCloudERVApp * GetApp() { return (CCloudERVApp *) AfxGetApp(); }

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CXTPStatusBar			m_wndStatusBar;

	CXTPTabClientWnd		m_wndMDITabClient;
	CXTPDockingPaneManager	m_paneManager;
	CXTPDockingPane *		m_pPaneFeatures;
	CXTPTaskPanel 			m_wndFeaturesTaskPane;

	int						m_nPaneCount;
	CMap<UINT,UINT, CWnd* , CWnd*> m_mapPanes;

	CXTPPopupControl		*m_pImageLoadFailedPopup;
	CXTPPopupControl		*m_pWaitingPriorThreadPopup;

	BOOL m_bFirstShow;

	HWND					m_hCallerhWnd;

public:
	void SetTaskPaneAutoHide(BOOL bAutoHide);
	HWND	GetCallerhWnd() { return m_hCallerhWnd; }

private:
	void CreateFeaturesTaskPane(void);

	void CreateRTFPopup(CXTPPopupControl **pPopup, UINT nID);

	CFrameWnd* GetParentDockingFrame(CWnd* pWnd);
	BOOL IsActionEnabled(UINT nID);
	afx_msg void OnUpdateActions(CCmdUI *pCmdUI);

	void QueryReport();		;
	void ViewExtReport();
	void ViewExtDcmImage();
	void ViewExtBmpImage();
	void BookingOfRIS();
	void BookingOfUIS();
	void BookingOfEIS();
	void EncodeString();
	void AppExit();

public:
    virtual BOOL LoadFrame( UINT nIDResource,DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL );
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* /*pContext*/);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskPanelNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	/*
	afx_msg void OnUpdateWindowAutohide(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowHideall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowCloseAll(CCmdUI *pCmdUI);
	afx_msg void OnWindowHide();
	afx_msg void OnWindowHideall();
	afx_msg void OnWindowAutohide();
	*/

	/*
	afx_msg void OnQueryPublishedReport();
	afx_msg void OnViewPdfReport();
	afx_msg void OnViewExtReport();
	afx_msg void OnViewExtDcmImage();
	afx_msg void OnViewExtBmpImage();
	afx_msg void OnAppExit();
	afx_msg void OnTest();
	*/
};
