// SetImgLayout.cpp : implementation file
//

#include "stdafx.h"
#include "SetImgLayout.h"
#include "CloudERV.h"
#include "DcmImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HMODULE GetCurrentModule();

/////////////////////////////////////////////////////////////////////////////
// CSetImgLayout dialog


CSetImgLayout::CSetImgLayout(CWnd* pParent /*=NULL*/)
	: CDialog(CSetImgLayout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetImgLayout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetImgLayout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetImgLayout)
	DDX_Control(pDX, IDC_IMGLAYOUT_SPINCOLS, m_ctlSpinCols);
	DDX_Control(pDX, IDC_IMGLAYOUT_SPINROWS, m_ctlSpinRows);


	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT1,m_btnLayout1);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT2,m_btnLayout2);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT3,m_btnLayout3);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT4,m_btnLayout4);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT5,m_btnLayout5);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT6,m_btnLayout6);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT7,m_btnLayout7);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT8,m_btnLayout8);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT9,m_btnLayout9);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT10,m_btnLayout10);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT11,m_btnLayout11);
	DDX_Control(pDX,IDC_IMGLAYOUT_LAYOUT12,m_btnLayout12);

	DDX_Control(pDX,IDC_IMGLAYOUT_APPLY,m_btnApply);


	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetImgLayout, CDialog)
	//{{AFX_MSG_MAP(CSetImgLayout)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_IMGLAYOUT_APPLY, OnApply)
	
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT1, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT2, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT3, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT4, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT5, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT6, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT7, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT8, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT9, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT10, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT11, OnBtn)
	ON_BN_CLICKED(IDC_IMGLAYOUT_LAYOUT12, OnBtn)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSetImgLayout::SetRowsCols(int nRows2,int nCols2) 
{
	int nRows,nCols;

	m_ctlSpinRows.SetRange32(1,64);
	m_ctlSpinCols.SetRange32(1,64);

	nRows = nRows2;
	nCols = nCols2;

	if (nRows < 1) nRows = 1;
	if (nRows > 64) nRows = 64;

	if (nCols < 1) nCols = 1;
	if (nCols > 64) nCols = 64;

	m_ctlSpinRows.SetPos(nRows);
	m_ctlSpinCols.SetPos(nCols);
	
	UpdateData(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CSetImgLayout message handlers

BOOL CSetImgLayout::Create(CWnd* pParentWnd) 
{	
	return CDialog::Create(IDD, pParentWnd);
}

int CSetImgLayout::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}


BOOL CSetImgLayout::OnInitDialog() 
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	HINSTANCE hInst;

	hInst = pApp->GetAppInstance();

	CDialog::OnInitDialog();
	
	hInst = GetCurrentModule();

	m_btnLayout1.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT1)));
	m_btnLayout2.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT2)));	
	m_btnLayout1.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT1)));
	m_btnLayout2.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT2)));
	m_btnLayout3.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT3)));
	m_btnLayout4.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT4)));
	m_btnLayout5.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT5)));
	m_btnLayout6.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT6)));
	m_btnLayout7.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT7)));
	m_btnLayout8.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT8)));
	m_btnLayout9.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT9)));
	m_btnLayout10.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT10)));
	m_btnLayout11.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT11)));
	m_btnLayout12.SetBitmap(::LoadBitmap(hInst,MAKEINTRESOURCE(IDB_IMGLAYOUT_LAYOUT12)));
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetImgLayout::OnBtn() 
{
	CDcmImageView *pDcmImageView;
	CWnd *pWnd;
	UINT nID;
	int nRows,nCols,nIdx,nLayout[12][2] = {{1,1},{1,2},{2,1},{2,2},{2,4},{4,2},{4,6},{6,4},{4,8},{3,3},{3,4},{5,6}};

	pWnd = GetFocus();
	
	if (pWnd == NULL) return;

	nID = pWnd->GetDlgCtrlID();

	nIdx = 0;

	if (nID == IDC_IMGLAYOUT_LAYOUT1)
	{
		nIdx = 0;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT2)
	{
		nIdx = 1;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT3)
	{
		nIdx = 2;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT4)
	{
		nIdx = 3;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT5)
	{
		nIdx = 4;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT6)
	{
		nIdx = 5;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT7)
	{
		nIdx = 6;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT8)
	{
		nIdx = 7;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT9)
	{
		nIdx = 8;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT10)
	{
		nIdx = 9;
	}
	else if (nID == IDC_IMGLAYOUT_LAYOUT11)
	{
		nIdx = 10;
	}
	else if (nID = IDC_IMGLAYOUT_LAYOUT12)
	{
		nIdx = 11;
	}

	nRows = nLayout[nIdx][0];
	nCols = nLayout[nIdx][1];
			
	pDcmImageView = (CDcmImageView *) m_pDcmImageViewObj;

	pDcmImageView->SetImgLayout(nRows,nCols,FALSE);

	//CDialog::OnOK();
}


void CSetImgLayout::OnApply() 
{
	int nRows,nCols;
	CDcmImageView *pDcmImageView;
	
	UpdateData();

	nRows = m_ctlSpinRows.GetPos();
	nCols = m_ctlSpinCols.GetPos();
			
	pDcmImageView = (CDcmImageView *) m_pDcmImageViewObj;
	pDcmImageView->SetImgLayout(nRows,nCols,TRUE);

	//CDialog::OnOK();	
}



void CSetImgLayout::SetLabelText()
{
	CDcmImageView *pDcmImageView;
	BOOL bSeriesMode;
	CWnd *pWndLabel;

	pDcmImageView = (CDcmImageView *) m_pDcmImageViewObj;

	if (pDcmImageView == NULL) return;

	bSeriesMode = pDcmImageView->GetDicomViewer()->GetSeriesMode();

	pWndLabel = (CWnd *) GetDlgItem(IDC_IMGLAYOUT_LABEL);

	if (pWndLabel != NULL)
	{
		pWndLabel->SetWindowText(bSeriesMode ? _T("序列框显示格式") : _T("影像框显示格式"));
	}
}


