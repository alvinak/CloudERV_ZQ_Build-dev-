// PresetWL.cpp : implementation file
//

#include "stdafx.h"
#include "DcmImageView.h"
#include "PresetWL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


typedef struct
{
	TCHAR sName[20];
	TCHAR sWValue[30];
	TCHAR sLValue[30];
} PresetWLStruct;


PresetWLStruct PresetWL[] = {
	{_T("ÄÔ×éÖ¯"),_T("80¡«120"),_T("35¡«50")},
	{_T("Èí×éÖ¯"),_T("250¡«300"),_T("3550¡«3550")},
	{_T("¼¹×µ¹Ç"),_T("1500¡«3000"),_T("200¡«500")},
	{_T("×Ý¸ô"),_T("300¡«450"),_T("30¡«50")},
	{_T("·Î´°"),_T("800¡«1200"),_T("-600¡«-900")},
	{_T("¸ÎÆ¢"),_T("100¡«200"),_T("30¡«50")},
	{_T("ÅèÇ»"),_T("200¡«350"),_T("30¡«30")},
	{_T("°òë×"),_T("250¡«300"),_T("30¡«40")},
	{_T("Ç°ÁÐÏÙ"),_T("250¡«350"),_T("35¡«50")},
	{_T("ÖÐ¶ú"),_T("2000¡«3000"),_T("200¡«300")},
	{_T("°°Çø"),_T("200¡«250"),_T("45¡«50")},
	{_T("ÒÈÏÙ"),_T("200¡«350"),_T("35¡«50")},
	{_T("ÉöÔà"),_T("200¡«300"),_T("25¡«35")},
	{_T("ÉöÉÏÏÙ"),_T("250¡«350"),_T("0¡«30")},
	{_T("¼××´ÏÙ"),_T("300¡«400"),_T("40¡«60")},
	{_T("ºí"),_T("350¡«400"),_T("30¡«40")},
	{_T("Â­µ×"),_T("2000¡«3000"),_T("250¡«350")},
	{_T("±ÇÑÊ²¿"),_T("300¡«350"),_T("30¡«50")},
	{_T("±Çñ¼"),_T("350¡«400"),_T("30¡«35")},
	{_T("ÑÛ"),_T("300¡«400"),_T("30¡«40")},
	{NULL,NULL,NULL}
	};


/////////////////////////////////////////////////////////////////////////////
// CPresetWL dialog


CPresetWL::CPresetWL(CWnd* pParent /*=NULL*/)
	: CXTPResizeDialog(CPresetWL::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPresetWL)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPresetWL::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPresetWL)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPresetWL, CXTPResizeDialog)
	//{{AFX_MSG_MAP(CPresetWL)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPresetWL::AdjustWinLevel()
{
	CListCtrl *pList = (CListCtrl *) GetDlgItem(IDC_LIST);
	int nIdx,nPos,nLen,nMin,nMax,nWidth,nLevel,nMode;
	TCHAR buf[40];
	CString sValue,sMin,sMax;
	CDcmImageView *pDcmImageView;
	CButton *pBtn1,*pBtn2;	


	pBtn1 = (CButton *) GetDlgItem(IDC_RADIO1);
	pBtn2 = (CButton *) GetDlgItem(IDC_RADIO2);

	if (m_pDcmImageViewObj == NULL)
		return;

	pDcmImageView = (CDcmImageView *) m_pDcmImageViewObj;


	if (pBtn1->GetCheck() == 1)
		nMode = 1;
	else if (pBtn2->GetCheck() == 1)
		nMode = 2;
	else 
		nMode = 3;


	nIdx = pList->GetNextItem(-1,LVNI_SELECTED);

	if (nIdx >= 0)
	{
		memset(buf,0,40);
		pList->GetItemText(nIdx,2,buf,40);

		sValue = buf;
		sMin = sValue;
		sMax = sValue;

		nPos = sValue.Find(_T("¡«"),0);
		nLen = sValue.GetLength();

		if (nPos > 0)
		{
			sMin = sValue.Mid(0,nPos);
			sMax = sValue.Mid(nPos + 2,nLen - nPos);
		}

		nMin = _ttoi(sMin);
		nMax = _ttoi(sMax);

		if (nMode == 1)
			nWidth = nMin;
		else if (nMode == 2)
			nWidth = (nMin + nMax) >> 1;
		else
			nWidth = nMax;

		memset(buf,0,40);
		pList->GetItemText(nIdx,3,buf,40);

		sValue = buf;
		sMin = sValue;
		sMax = sValue;

		nPos = sValue.Find(_T("¡«"),0);
		nLen = sValue.GetLength();

		if (nPos > 0)
		{
			sMin = sValue.Mid(0,nPos);
			sMax = sValue.Mid(nPos + 2,nLen - nPos);
		}

		nMin = _ttoi(sMin);
		nMax = _ttoi(sMax);

		if (nMode == 1)
			nLevel = nMin;
		else if (nMode == 2)
			nLevel = (nMin + nMax) >> 1;
		else
			nLevel = nMax;

		pDcmImageView->GetDicomViewer()->SetWinLevel((double) nLevel,(double) nWidth);
		pDcmImageView->GetDicomViewer()->Invalidate();

	}	
}


/////////////////////////////////////////////////////////////////////////////
// CPresetWL message handlers

BOOL CPresetWL::Create(CWnd* pParentWnd) 
{	
	return CXTPResizeDialog::Create(IDD, pParentWnd);
}

BOOL CPresetWL::OnInitDialog() 
{
	CListCtrl *pList;
	CButton *pBtn;
	int ni;
	CString sItem;

	
	SetResize(IDC_VALUELBL,SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_RADIO1,SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_RADIO2,SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_RADIO3,SZ_TOP_LEFT, SZ_TOP_LEFT);

	SetResize(IDC_LIST,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	CXTPResizeDialog::OnInitDialog();
	
	pList = (CListCtrl *) GetDlgItem(IDC_LIST);

	pList->InsertColumn(0,_T("ÐòºÅ"),LVCFMT_LEFT,60);
	pList->InsertColumn(1,_T("×éÖ¯Ãû³Æ"),LVCFMT_CENTER,100);
	pList->InsertColumn(2,_T("´°¿í"),LVCFMT_CENTER,100);
	pList->InsertColumn(3,_T("´°Î»"),LVCFMT_CENTER,100);

	ni = 0;
	while (PresetWL[ni].sName[0] != 0) {
		sItem.Format(_T("%2d"),ni + 1);
		pList->InsertItem(ni,sItem,0);

		pList->SetItemText(ni,1,PresetWL[ni].sName);
		pList->SetItemText(ni,2,PresetWL[ni].sWValue);
		pList->SetItemText(ni,3,PresetWL[ni].sLValue);

		ni ++;
	}

	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);


	pBtn = (CButton *) GetDlgItem(IDC_RADIO2);
	pBtn->SetCheck(1);
	
	return TRUE;  
}

void CPresetWL::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AdjustWinLevel();	
	*pResult = 0;
}
