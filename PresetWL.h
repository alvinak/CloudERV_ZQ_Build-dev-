#if !defined(AFX_PRESETWL_H__9E898C2A_A167_44BA_A396_BBFA71D96899__INCLUDED_)
#define AFX_PRESETWL_H__9E898C2A_A167_44BA_A396_BBFA71D96899__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PresetWL.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPresetWL dialog

class CPresetWL : public CXTPResizeDialog
{
// Construction
public:
	CPresetWL(CWnd* pParent = NULL);   // standard constructor

private:
	void *m_pDcmImageViewObj;


public:
	void SetDcmImageViewObj(void *p) { m_pDcmImageViewObj = p;}

// Dialog Data
	//{{AFX_DATA(CPresetWL)
	enum { IDD = IDD_PRESETWL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void AdjustWinLevel();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPresetWL)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK() {};
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPresetWL)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRESETWL_H__9E898C2A_A167_44BA_A396_BBFA71D96899__INCLUDED_)
