#if !defined(AFX_OPTIONSWINLEVELDLG_H__5EB5F4C0_5A7B_4258_A7BA_40953ACE36EE__INCLUDED_)
#define AFX_OPTIONSWINLEVELDLG_H__5EB5F4C0_5A7B_4258_A7BA_40953ACE36EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsWinLevelDlg.h : header file
//

#include "resource.h"

typedef struct 
{
	UINT nID;
	TCHAR sName[21];
	int nWidth;
	int nCenter;
	TCHAR sCaption[41];
} CustomWinLevelStruct;


/////////////////////////////////////////////////////////////////////////////
// COptionsWinLevelDlg dialog

class COptionsWinLevelDlg : public CXTPDialog
{
// Construction
public:
	COptionsWinLevelDlg(CWnd* pParent = NULL);   // standard constructor

private:
	void *m_pDcmImageViewObj;
	CustomWinLevelStruct m_CustomWinLevel[10];
	
public:
	void SetDcmImageViewObj(void *p) { m_pDcmImageViewObj = p;}

	CustomWinLevelStruct *GetCustomWinLevel();
	void SetCustomWinLevel(CustomWinLevelStruct *);
	
// Dialog Data
	//{{AFX_DATA(COptionsWinLevelDlg)
	enum { IDD = IDD_OPTIONS_WINLEVEL };
	CString	m_sNameA;
	CString	m_sName1;
	CString	m_sName2;
	CString	m_sName4;
	CString	m_sName3;
	CString	m_sName5;
	CString	m_sName6;
	CString	m_sName7;
	CString	m_sName8;
	CString	m_sName9;
	int		m_nWidth1;
	int		m_nWidthA;
	int		m_nWidth2;
	int		m_nWidth3;
	int		m_nWidth4;
	int		m_nWidth5;
	int		m_nWidth6;
	int		m_nWidth7;
	int		m_nWidth8;
	int		m_nWidth9;
	int		m_nCenter1;
	int		m_nCenterA;
	int		m_nCenter2;
	int		m_nCenter3;
	int		m_nCenter4;
	int		m_nCenter5;
	int		m_nCenter6;
	int		m_nCenter7;
	int		m_nCenter8;
	int		m_nCenter9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsWinLevelDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsWinLevelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK() { };
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSWINLEVELDLG_H__5EB5F4C0_5A7B_4258_A7BA_40953ACE36EE__INCLUDED_)
