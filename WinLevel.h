#if !defined(AFX_WINLEVEL_H__741C19AA_9435_4847_AB7C_F3F13DAA3BCD__INCLUDED_)
#define AFX_WINLEVEL_H__741C19AA_9435_4847_AB7C_F3F13DAA3BCD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinLevel.h : header file
//

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWinLevel dialog

class CWinLevel : public CDialog
{
// Construction
public:
	CWinLevel(CWnd* pParent = NULL);   // standard constructor

private:
	void *m_pDcmImageViewObj;
public:
	void SetDcmImageViewObj(void *p) { m_pDcmImageViewObj = p;}

// Dialog Data
	//{{AFX_DATA(CWinLevel)
	enum { IDD = IDD_WINLEVEL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


private:
	CEdit m_edtCen,m_edtWid;
	CSpinButtonCtrl m_spnCen,m_spnWid;
	int m_nCen,m_nWid;
public:

	void SetWinLevel(double fcen,double fwid);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinLevel)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK() {};
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWinLevel)
	virtual BOOL OnInitDialog();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINLEVEL_H__741C19AA_9435_4847_AB7C_F3F13DAA3BCD__INCLUDED_)
