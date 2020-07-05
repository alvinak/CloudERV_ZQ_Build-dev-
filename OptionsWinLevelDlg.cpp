// OptionsWinLevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsWinLevelDlg.h"
#include "DcmIMageView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsWinLevelDlg dialog


COptionsWinLevelDlg::COptionsWinLevelDlg(CWnd* pParent /*=NULL*/)
	: CXTPDialog(COptionsWinLevelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsWinLevelDlg)
	m_sNameA = _T("");
	m_sName1 = _T("");
	m_sName2 = _T("");
	m_sName4 = _T("");
	m_sName3 = _T("");
	m_sName5 = _T("");
	m_sName6 = _T("");
	m_sName7 = _T("");
	m_sName8 = _T("");
	m_sName9 = _T("");
	m_nWidth1 = 0;
	m_nWidthA = 0;
	m_nWidth2 = 0;
	m_nWidth3 = 0;
	m_nWidth4 = 0;
	m_nWidth5 = 0;
	m_nWidth6 = 0;
	m_nWidth7 = 0;
	m_nWidth8 = 0;
	m_nWidth9 = 0;
	m_nCenter1 = 0;
	m_nCenterA = 0;
	m_nCenter2 = 0;
	m_nCenter3 = 0;
	m_nCenter4 = 0;
	m_nCenter5 = 0;
	m_nCenter6 = 0;
	m_nCenter7 = 0;
	m_nCenter8 = 0;
	m_nCenter9 = 0;
	//}}AFX_DATA_INIT
}


void COptionsWinLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsWinLevelDlg)
	DDX_Text(pDX, IDC_NAME10, m_sNameA);
	DDV_MaxChars(pDX, m_sNameA, 40);
	DDX_Text(pDX, IDC_NAME1, m_sName1);
	DDV_MaxChars(pDX, m_sName1, 40);
	DDX_Text(pDX, IDC_NAME2, m_sName2);
	DDV_MaxChars(pDX, m_sName2, 40);
	DDX_Text(pDX, IDC_NAME4, m_sName4);
	DDV_MaxChars(pDX, m_sName4, 40);
	DDX_Text(pDX, IDC_NAME3, m_sName3);
	DDV_MaxChars(pDX, m_sName3, 40);
	DDX_Text(pDX, IDC_NAME5, m_sName5);
	DDV_MaxChars(pDX, m_sName5, 40);
	DDX_Text(pDX, IDC_NAME6, m_sName6);
	DDV_MaxChars(pDX, m_sName6, 40);
	DDX_Text(pDX, IDC_NAME7, m_sName7);
	DDV_MaxChars(pDX, m_sName7, 40);
	DDX_Text(pDX, IDC_NAME8, m_sName8);
	DDV_MaxChars(pDX, m_sName8, 40);
	DDX_Text(pDX, IDC_NAME9, m_sName9);
	DDV_MaxChars(pDX, m_sName9, 40);
	DDX_Text(pDX, IDC_WIDTH1, m_nWidth1);
	DDV_MinMaxInt(pDX, m_nWidth1, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH10, m_nWidthA);
	DDV_MinMaxInt(pDX, m_nWidthA, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH2, m_nWidth2);
	DDV_MinMaxInt(pDX, m_nWidth2, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH3, m_nWidth3);
	DDV_MinMaxInt(pDX, m_nWidth3, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH4, m_nWidth4);
	DDV_MinMaxInt(pDX, m_nWidth4, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH5, m_nWidth5);
	DDV_MinMaxInt(pDX, m_nWidth5, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH6, m_nWidth6);
	DDV_MinMaxInt(pDX, m_nWidth6, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH7, m_nWidth7);
	DDV_MinMaxInt(pDX, m_nWidth7, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH8, m_nWidth8);
	DDV_MinMaxInt(pDX, m_nWidth8, 0, 4096);
	DDX_Text(pDX, IDC_WIDTH9, m_nWidth9);
	DDV_MinMaxInt(pDX, m_nWidth9, 0, 4096);
	DDX_Text(pDX, IDC_CENTER1, m_nCenter1);
	DDV_MinMaxInt(pDX, m_nCenter1, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER10, m_nCenterA);
	DDV_MinMaxInt(pDX, m_nCenterA, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER2, m_nCenter2);
	DDV_MinMaxInt(pDX, m_nCenter2, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER3, m_nCenter3);
	DDV_MinMaxInt(pDX, m_nCenter3, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER4, m_nCenter4);
	DDV_MinMaxInt(pDX, m_nCenter4, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER5, m_nCenter5);
	DDV_MinMaxInt(pDX, m_nCenter5, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER6, m_nCenter6);
	DDV_MinMaxInt(pDX, m_nCenter6, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER7, m_nCenter7);
	DDV_MinMaxInt(pDX, m_nCenter7, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER8, m_nCenter8);
	DDV_MinMaxInt(pDX, m_nCenter8, -4096, 4096);
	DDX_Text(pDX, IDC_CENTER9, m_nCenter9);
	DDV_MinMaxInt(pDX, m_nCenter9, -4096, 4096);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsWinLevelDlg, CXTPDialog)
	//{{AFX_MSG_MAP(COptionsWinLevelDlg)
	ON_BN_CLICKED(IDC_APPLY, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL COptionsWinLevelDlg::Create(CWnd* pParentWnd) 
{	
	return CXTPDialog::Create(IDD, pParentWnd);
}


BOOL COptionsWinLevelDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 13)
		{
			pMsg->wParam = 9;
		}

	}

	return CXTPDialog::PreTranslateMessage(pMsg);
}



void COptionsWinLevelDlg::SetCustomWinLevel(CustomWinLevelStruct *p)
{
	int ni;

	for (ni = 0; ni < 10; ni ++)
	{
		m_CustomWinLevel[ni] = *(p + ni);
	}

	m_sName1 = m_CustomWinLevel[0].sName;
	m_nWidth1 = m_CustomWinLevel[0].nWidth;
	m_nCenter1 = m_CustomWinLevel[0].nCenter;

	m_sName2 = m_CustomWinLevel[1].sName;
	m_nWidth2 = m_CustomWinLevel[1].nWidth;
	m_nCenter2 = m_CustomWinLevel[1].nCenter;

	m_sName3 = m_CustomWinLevel[2].sName;
	m_nWidth3 = m_CustomWinLevel[2].nWidth;
	m_nCenter3 = m_CustomWinLevel[2].nCenter;

	m_sName4 = m_CustomWinLevel[3].sName;
	m_nWidth4 = m_CustomWinLevel[3].nWidth;
	m_nCenter4 = m_CustomWinLevel[3].nCenter;

	m_sName5 = m_CustomWinLevel[4].sName;
	m_nWidth5 = m_CustomWinLevel[4].nWidth;
	m_nCenter5 = m_CustomWinLevel[4].nCenter;

	m_sName6 = m_CustomWinLevel[5].sName;
	m_nWidth6 = m_CustomWinLevel[5].nWidth;
	m_nCenter6 = m_CustomWinLevel[5].nCenter;

	m_sName7 = m_CustomWinLevel[6].sName;
	m_nWidth7 = m_CustomWinLevel[6].nWidth;
	m_nCenter7 = m_CustomWinLevel[6].nCenter;

	m_sName8 = m_CustomWinLevel[7].sName;
	m_nWidth8 = m_CustomWinLevel[7].nWidth;
	m_nCenter8 = m_CustomWinLevel[7].nCenter;

	m_sName9 = m_CustomWinLevel[8].sName;
	m_nWidth9 = m_CustomWinLevel[8].nWidth;
	m_nCenter9 = m_CustomWinLevel[8].nCenter;


	m_sNameA = m_CustomWinLevel[9].sName;
	m_nWidthA = m_CustomWinLevel[9].nWidth;
	m_nCenterA = m_CustomWinLevel[9].nCenter;

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// COptionsWinLevelDlg message handlers

BOOL COptionsWinLevelDlg::OnInitDialog() 
{
	CXTPDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void COptionsWinLevelDlg::OnSave() 
{
	CDcmImageView *pDcmImageView;
	int ni;

	if (!UpdateData(TRUE)) return;
	
	_tcsncpy(m_CustomWinLevel[0].sName,m_sName1,40);
	m_CustomWinLevel[0].nWidth = m_nWidth1;
	m_CustomWinLevel[0].nCenter = m_nCenter1;

	_tcsncpy(m_CustomWinLevel[1].sName,m_sName2,40);
	m_CustomWinLevel[1].nWidth = m_nWidth2;
	m_CustomWinLevel[1].nCenter = m_nCenter2;

	_tcsncpy(m_CustomWinLevel[2].sName,m_sName3,40);
	m_CustomWinLevel[2].nWidth = m_nWidth3;
	m_CustomWinLevel[2].nCenter = m_nCenter3;

	_tcsncpy(m_CustomWinLevel[3].sName,m_sName4,40);
	m_CustomWinLevel[3].nWidth = m_nWidth4;
	m_CustomWinLevel[3].nCenter = m_nCenter4;

	_tcsncpy(m_CustomWinLevel[4].sName,m_sName5,40);
	m_CustomWinLevel[4].nWidth = m_nWidth5;
	m_CustomWinLevel[4].nCenter = m_nCenter5;

	_tcsncpy(m_CustomWinLevel[5].sName,m_sName6,40);
	m_CustomWinLevel[5].nWidth = m_nWidth6;
	m_CustomWinLevel[5].nCenter = m_nCenter6;

	_tcsncpy(m_CustomWinLevel[6].sName,m_sName7,40);
	m_CustomWinLevel[6].nWidth = m_nWidth7;
	m_CustomWinLevel[6].nCenter = m_nCenter7;

	_tcsncpy(m_CustomWinLevel[7].sName,m_sName8,40);
	m_CustomWinLevel[7].nWidth = m_nWidth8;
	m_CustomWinLevel[7].nCenter = m_nCenter8;

	_tcsncpy(m_CustomWinLevel[8].sName,m_sName9,40);
	m_CustomWinLevel[8].nWidth = m_nWidth9;
	m_CustomWinLevel[8].nCenter = m_nCenter9;

	_tcsncpy(m_CustomWinLevel[9].sName,m_sNameA,40);
	m_CustomWinLevel[9].nWidth = m_nWidthA;
	m_CustomWinLevel[9].nCenter = m_nCenterA;
	
	pDcmImageView = (CDcmImageView *) m_pDcmImageViewObj;
	pDcmImageView->SetWinLevelOptions(&m_CustomWinLevel[0]);

	CDialog::OnOK();
}



