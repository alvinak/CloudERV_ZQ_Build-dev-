#if !defined(AFX_SETIMGLAYOUT_H__21114C84_0CC7_47F3_BB16_DC15727E32C8__INCLUDED_)
#define AFX_SETIMGLAYOUT_H__21114C84_0CC7_47F3_BB16_DC15727E32C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetImgLayout.h : header file
//

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CSetImgLayout dialog

class CSetImgLayout : public CDialog
{
// Construction
public:
	CSetImgLayout(CWnd* pParent = NULL);   // standard constructor

private:
	void *m_pDcmImageViewObj;


public:
	void SetDcmImageViewObj(void *p) { m_pDcmImageViewObj = p;}

	void SetLabelText();


// Dialog Data
	//{{AFX_DATA(CSetImgLayout)
	enum { IDD = IDD_IMGLAYOUT };

	CButton m_btnLayout1;
	CButton m_btnLayout2;
	CButton m_btnLayout3;
	CButton m_btnLayout4;
	CButton m_btnLayout5;
	CButton m_btnLayout6;
	CButton m_btnLayout7;
	CButton m_btnLayout8;
	CButton m_btnLayout9;
	CButton m_btnLayout10;
	CButton m_btnLayout11;
	CButton m_btnLayout12;

	CButton m_btnApply;

	CSpinButtonCtrl	m_ctlSpinCols;
	CSpinButtonCtrl	m_ctlSpinRows;

	//}}AFX_DATA

private:	

public:

	void SetRowsCols(int nRows2,int nCols2);
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetImgLayout)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK() { };
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetImgLayout)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtn();
	afx_msg void OnApply();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETMATRIX_H__21114C84_0CC7_47F3_BB16_DC15727E32C8__INCLUDED_)
