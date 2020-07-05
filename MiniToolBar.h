#if !defined(AFX_MINITOOLBAR_H__6396DDE8_00EB_49AD_B9BF_B3450ED16D98__INCLUDED_)
#define AFX_MINITOOLBAR_H__6396DDE8_00EB_49AD_B9BF_B3450ED16D98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiniToolBar.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CMiniToolBar dialog

class CMiniToolBar : public CDialog
{
// Construction
public:
	CMiniToolBar(CWnd* pParent = NULL);   // standard constructor
	
private:
	void	*m_pMainFormObj;
	CToolBar m_wndMiniToolBar;
	CRect	m_rtWindowPos;

public:
	void SetMainFormObj(void *p){ m_pMainFormObj = p;}

	void    GetWindowPos(CRect &rect) { rect = m_rtWindowPos;}

	CToolBarCtrl *GetToolBarCtrl() {return &m_wndMiniToolBar.GetToolBarCtrl();}
// Dialog Data
	//{{AFX_DATA(CMiniToolBar)
	enum { IDD = IDD_MINITOOLBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniToolBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMiniToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnUpdateAction(CCmdUI *pCmdUI);
	afx_msg void OnExecuteAction(UINT nID);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolbarDropDown(NMHDR* pnmtb, LRESULT *plr);
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINITOOLBAR_H__6396DDE8_00EB_49AD_B9BF_B3450ED16D98__INCLUDED_)
