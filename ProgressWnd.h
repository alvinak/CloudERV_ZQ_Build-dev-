#if !defined(AFX_PROGRESSWND_H__BC1A888A_67B3_46F0_B473_B65B14946C0C__INCLUDED_)
#define AFX_PROGRESSWND_H__BC1A888A_67B3_46F0_B473_B65B14946C0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressWnd.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd dialog

class CProgressWnd : public CXTPDialog
{
// Construction
public:
	CProgressWnd(CWnd* pParent = NULL);   // standard constructor
	void ChangePos(int nRemaining,int nCompleted,const TCHAR *szMsg); 
// Dialog Data
	//{{AFX_DATA(CProgressWnd)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_ctlProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressWnd)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressWnd)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSWND_H__BC1A888A_67B3_46F0_B473_B65B14946C0C__INCLUDED_)
