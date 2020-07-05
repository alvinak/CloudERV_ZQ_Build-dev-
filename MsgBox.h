#if !defined(AFX_MSGBOX_H__54F06ACE_302F_47FC_B759_E3160B522A57__INCLUDED_)
#define AFX_MSGBOX_H__54F06ACE_302F_47FC_B759_E3160B522A57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgBox dialog

class CMsgBox : public CDialog
{
// Construction
public:
	CMsgBox(CWnd* pParent = NULL);   // standard constructor
	~CMsgBox();

private:
	BOOL m_bInited;
	int  m_nTimerId;

public:
	void ShowMessage(const TCHAR *szMsg);

// Dialog Data
	//{{AFX_DATA(CMsgBox)
	enum { IDD = IDD_MSGBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBox)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMsgBox)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGBOX_H__54F06ACE_302F_47FC_B759_E3160B522A57__INCLUDED_)
