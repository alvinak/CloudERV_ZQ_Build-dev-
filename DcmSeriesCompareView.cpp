// DcmSeriesCompareView.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "DcmSeriesCompareView.h"
#include "DcmSeriesCompareDoc.h"
#include "MainFrm.h"
#include <vector>
#include <algorithm>    // std::stable_sort

#include "DirDialog.h"

struct SeriesObjectSort1XData {
	std::string key;
	std::string caption;
	int index;
};

int SeriesObjectSort1XData_Compare(const SeriesObjectSort1XData *a, const SeriesObjectSort1XData *b)
{
    // smallest comes first
	return (strcmp(a->key.c_str(), b->key.c_str()) < 0 ? 1: 0);
}


// CDcmSeriesCompareView

IMPLEMENT_DYNCREATE(CDcmSeriesCompareView, CView)

CDcmSeriesCompareView::CDcmSeriesCompareView()
{
	int ni;
	UINT nIDs[40] = { ID_SERIESGO_00, ID_SERIESGO_01, ID_SERIESGO_02, ID_SERIESGO_03, ID_SERIESGO_04, ID_SERIESGO_05,
					  ID_SERIESGO_06, ID_SERIESGO_07, ID_SERIESGO_08, ID_SERIESGO_09, ID_SERIESGO_10, ID_SERIESGO_11,
					  ID_SERIESGO_12, ID_SERIESGO_13, ID_SERIESGO_14, ID_SERIESGO_15, ID_SERIESGO_16, ID_SERIESGO_17,
					  ID_SERIESGO_18, ID_SERIESGO_19, ID_SERIESGO_20, ID_SERIESGO_21, ID_SERIESGO_22, ID_SERIESGO_23,
					  ID_SERIESGO_24, ID_SERIESGO_25, ID_SERIESGO_26, ID_SERIESGO_27, ID_SERIESGO_28, ID_SERIESGO_29,
					  ID_SERIESGO_30, ID_SERIESGO_31, ID_SERIESGO_32, ID_SERIESGO_33, ID_SERIESGO_34, ID_SERIESGO_35,
					  ID_SERIESGO_36, ID_SERIESGO_37, ID_SERIESGO_38, ID_SERIESGO_39 };


	//
	for (ni = 0; ni < 40; ni ++) 
		m_nSeriesGoIDs[ni] = nIDs[ni];
	//

	m_nSeriesRows	= 1;
	m_nSeriesCols	= 2;

	m_nRows	= 1;
	m_nCols	= 1;

	m_nTextColor = RGB(0,0,255);
	m_nBkColor   = RGB(255,255,255);
	m_nBkMode    = TRANSPARENT;

	m_nLBFuncNo = 0;
	m_nRBFuncNo = 1;
	m_bDrawOverlay = TRUE;

	for (ni = 0; ni < 10; ni ++) {
		memset(&m_CustomWinLevel[ni], 0 , sizeof(CustomWinLevelStruct));
	}


	m_bInitialized = FALSE;
}


CDcmSeriesCompareView::~CDcmSeriesCompareView()
{
}

BEGIN_MESSAGE_MAP(CDcmSeriesCompareView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()

	ON_NOTIFY(NM_CELLWNDCLICK, 100, OnCellWndClick)
	ON_NOTIFY(NM_CELLWNDRBCLICK, 100, OnCellWndRBClick)

	ON_UPDATE_COMMAND_UI_RANGE(1000, 65535, OnUpdateActions)
	ON_COMMAND_RANGE(1000, 65535, OnExecuteActions)
	ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()


// CDcmSeriesCompareView drawing

void CDcmSeriesCompareView::OnDraw(CDC* pDC)
{
	CDcmSeriesCompareDoc* pDoc = GetDocument();
	// TODO: add draw code here
}


// CDcmSeriesCompareView diagnostics

#ifdef _DEBUG
void CDcmSeriesCompareView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CDcmSeriesCompareView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif

CDcmSeriesCompareDoc* CDcmSeriesCompareView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDcmSeriesCompareDoc)));
	return (CDcmSeriesCompareDoc*)m_pDocument;
}

#endif //_DEBUG



void CDcmSeriesCompareView::ShowMessage(int nCode)
{
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetMainWnd();
	pMainFrame->ShowMessageEx(nCode);
}

void CDcmSeriesCompareView::ShowMessage(const TCHAR *szMsg)
{
	CMainFrame *pMainFrame = (CMainFrame *) AfxGetMainWnd();
	pMainFrame->ShowMessageEx(szMsg);
}

BOOL CDcmSeriesCompareView::IsDicomViewerEmpty()
{
	BOOL bSeriesMode;
	int nCount;

	bSeriesMode = m_wndDicomViewer.GetSeriesMode();

	if (bSeriesMode)
		nCount  = m_wndDicomViewer.GetSeriesObjectCount();
	else
		nCount  = m_wndDicomViewer.GetDicomObjectCount();

	return (nCount > 0);
}

void CDcmSeriesCompareView::SetWinLevel(double fCen, double fWid)
{
	m_wndDicomViewer.SetWinLevel(fCen, fWid);
	m_wndDicomViewer.InvalidateRect(NULL);
}

void CDcmSeriesCompareView::SetImgLayout(int nRows, int nCols, BOOL bCustom)
{
	BOOL bSeriesMode;
	int nIndex;

	if (nRows <= 0 || nCols <= 0 || nRows > 64 || nCols > 64) return;

	bSeriesMode = m_wndDicomViewer.GetSeriesMode();

	if (bCustom)
	{
		if (bSeriesMode)
		{
			if (m_nSeriesRows2 != nRows)
			{
				m_nSeriesRows2 = nRows;
				//AfxGetApp()->WriteProfileInt("DisplayLayout","SeriesRows2",m_nSeriesRows2);
			}

			if (m_nSeriesCols2 != nCols)
			{
				m_nSeriesCols2 = nCols;
				//AfxGetApp()->WriteProfileInt("DisplayLayout","SeriesCols2",m_nSeriesCols2);
			}
		}
		else
		{
			if (m_nRows2 != nRows)
			{
				m_nRows2 = nRows;
				//AfxGetApp()->WriteProfileInt("DisplayLayout","Rows2",m_nRows2);
			}

			if (m_nCols2 != nCols)
			{
				m_nCols2 = nCols;
				//AfxGetApp()->WriteProfileInt("DisplayLayout","Cols2",m_nCols2);
			}
		}
	}
	else
	{
		if (bSeriesMode)
		{
			if (m_nSeriesRows != nRows)
			{
				m_nSeriesRows = nRows;
				//AfxGetApp()->WriteProfileInt("DisplayLayout","SeriesRows1",m_nSeriesRows);
			}
		
			if (m_nSeriesCols != nCols)
			{
				m_nSeriesCols = nCols;

				//AfxGetApp()->WriteProfileInt("DisplayLayout","SeriesCols1",m_nSeriesCols);
			}

		}
		else
		{

			if (m_nRows != nRows)
			{
				m_nRows = nRows;
				//AfxGetApp()->WriteProfileInt("DisplayLayout","Rows1",m_nRows);
			}
		
			if (m_nCols != nCols)
			{
				m_nCols = nCols;

				//AfxGetApp()->WriteProfileInt("DisplayLayout","Cols1",m_nCols);
			}
		}
	}

	m_wndDicomViewer.AdjustMatrix(nRows,nCols);
	
	m_wndDicomViewer.Invalidate();
}

// CDcmImageCompareView message handlers


int CDcmSeriesCompareView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
			return -1;

	if (!m_wndDicomViewer.Create(this, 100))
		return -1;

	if (!m_wndImgLayout.Create(this))
		return -1;

	if (!m_wndWinLevel.Create(this))
		return -1;

	if (!m_wndPresetWL.Create(this))
		return -1;

	if (m_wndWinLevelOptions.Create(this) == -1)
		return -1;

	if (m_wndSensitivityDlg.Create(this) == -1)
		return -1;

	m_wndImgLayout.SetDcmImageViewObj(this);
	m_wndWinLevel.SetDcmImageViewObj(this);
	m_wndPresetWL.SetDcmImageViewObj(this);

	m_wndWinLevelOptions.SetDcmImageViewObj(this);
	m_wndSensitivityDlg.SetDcmImageViewObj(this);

	ReadIniValues();

	return 0;
}


void CDcmSeriesCompareView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
		return;

	m_wndDicomViewer.MoveWindow(0,0,cx,cy);
}


BOOL CDcmSeriesCompareView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDcmSeriesCompareView::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
 {
     ASSERT(pPopupMenu != NULL);
     // Check the enabled state of various menu items.
 
     CCmdUI state;
     state.m_pMenu = pPopupMenu;
     ASSERT(state.m_pOther == NULL);
     ASSERT(state.m_pParentMenu == NULL);
 
    // Determine if menu is popup in top-level menu and set m_pOther to
     // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
     HMENU hParentMenu;
     if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
         state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
     else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
     {
         CWnd* pParent = this;
            // Child Windows don't have menus--need to go to the top!
         if (pParent != NULL &&
            (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
         {
            int nIndexMax = ::GetMenuItemCount(hParentMenu);
            for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
            {
             if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
             {
                 // When popup is found, m_pParentMenu is containing menu.
                 state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                 break;
             }
            }
         }
     }


    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
     for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
       state.m_nIndex++)
     {
         state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
         if (state.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.


        ASSERT(state.m_pOther == NULL);
         ASSERT(state.m_pMenu != NULL);
         if (state.m_nID == (UINT)-1)
         {
            // Possibly a popup menu, route to first item of that popup.
            state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
            if (state.m_pSubMenu == NULL ||
             (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
             state.m_nID == (UINT)-1)
            {
             continue;       // First item of popup can't be routed to.
            }
            state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
         }
         else
         {
            // Normal menu item.
            // Auto enable/disable if frame window has m_bAutoMenuEnable
            // set and command is _not_ a system command.
            state.m_pSubMenu = NULL;
            state.DoUpdate(this, FALSE);
         }
 

        // Adjust for menu deletions and additions.
         UINT nCount = pPopupMenu->GetMenuItemCount();
         if (nCount < state.m_nIndexMax)
         {
            state.m_nIndex -= (state.m_nIndexMax - nCount);
            while (state.m_nIndex < nCount &&
             pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
            {
             state.m_nIndex++;
            }
         }
         state.m_nIndexMax = nCount;
     }
 }


void CDcmSeriesCompareView::OnCellWndClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UINT nID;

	nID = pNMHDR->idFrom;

	if (nID == 100) {

		if (m_wndImgLayout.IsWindowVisible())
			m_wndImgLayout.ShowWindow(SW_HIDE);

		if (m_wndWinLevel.IsWindowVisible())
			m_wndWinLevel.ShowWindow(SW_HIDE);

		if (m_wndPresetWL.IsWindowVisible())
			m_wndPresetWL.ShowWindow(SW_HIDE);
	}
}


void CDcmSeriesCompareView::OnCellWndRBClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UINT nID;
	CPoint ptCurrent;
	CMenu popupMenu, *pContextMenu;

	nID = pNMHDR->idFrom;

	if (nID == 100) {

		if (m_wndImgLayout.IsWindowVisible())
			m_wndImgLayout.ShowWindow(SW_HIDE);

		if (m_wndWinLevel.IsWindowVisible())
			m_wndWinLevel.ShowWindow(SW_HIDE);

		if (m_wndPresetWL.IsWindowVisible())
			m_wndPresetWL.ShowWindow(SW_HIDE);

		
		::GetCursorPos(&ptCurrent);
		popupMenu.LoadMenu(IDR_DCMVIEW_POPUPMENU);
		pContextMenu = popupMenu.GetSubMenu(0);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, this);
	}
}



void CDcmSeriesCompareView::ReadIniValues()
{
	CWinApp *pApp = (CCloudERVApp *) AfxGetApp();
	const TCHAR *szSettingsSection = _T("Settings");
	const TCHAR *szCustomWinLevelSection = _T("CustomWinLevel");
	CString szSKey1("DisplayLayout"),szSKey2("Settings"),sValue;
	CString sName,szKey1,szKey2,szKey3;
	int ni,nWinWidth,nWinCenter,nLen,nErrNo,nR,nG,nB;
	UINT nID;


	m_nSensitivity	= pApp->GetProfileInt(szSettingsSection, _T("Sensitivity"), 1);


	m_nTextColor = pApp->GetProfileInt(szSettingsSection,_T("TextColor"),RGB(0,0,255));
	m_nBkColor   = pApp->GetProfileInt(szSettingsSection,_T("BkColor"),RGB(255,255,255));
	m_nBkMode    = pApp->GetProfileInt(szSettingsSection,_T("BkMode"),TRANSPARENT);

	nR = GetRValue(m_nTextColor);
	nG = GetGValue(m_nTextColor);
	nB = GetBValue(m_nTextColor);

	m_nTextColor = RGB(nR,nG,nB);

	nR = GetRValue(m_nBkColor);
	nG = GetGValue(m_nBkColor);
	nB = GetBValue(m_nBkColor);

	m_nBkColor = RGB(nR,nG,nB);

	ni = 0;
	while (ni < 10) {

		switch(ni) {
		case 0:
			nID = ID_WINLEVEL_CUSTOM1;
			break;
		case 1:
			nID = ID_WINLEVEL_CUSTOM2;
			break;
		case 2:
			nID = ID_WINLEVEL_CUSTOM3;
			break;
		case 3:
			nID = ID_WINLEVEL_CUSTOM4;
			break;
		case 4:
			nID = ID_WINLEVEL_CUSTOM5;
			break;
		case 5:
			nID = ID_WINLEVEL_CUSTOM6;
			break;
		case 6:
			nID = ID_WINLEVEL_CUSTOM7;
			break;
		case 7:
			nID = ID_WINLEVEL_CUSTOM8;
			break;
		case 8:
			nID = ID_WINLEVEL_CUSTOM9;
			break;
		case 9:
			nID = ID_WINLEVEL_CUSTOM10;
			break;
		}

		sName.Format(_T("窗%02d"),ni + 1);
		szKey1.Format(_T("CustomWindowName%02d"),ni + 1);
		szKey2.Format(_T("CustomWindowWidth%02d"), ni + 1);
		szKey3.Format(_T("CustomWindowCenter%02d"), ni + 1);

		sValue = pApp->GetProfileString(szCustomWinLevelSection,szKey1,sName);
		sValue.TrimLeft();
		sValue.TrimRight();
		
		nWinWidth = pApp->GetProfileInt(szCustomWinLevelSection,szKey2,1500);
		if (nWinWidth > 4096) nWinWidth = 4096;
		
		nWinCenter = pApp->GetProfileInt(szCustomWinLevelSection,szKey3,550);	

		if (sValue.GetLength() > 0 && nWinWidth > 0) {
			_tcsncpy(m_CustomWinLevel[ni].sName,sValue,20);
			m_CustomWinLevel[ni].nWidth = nWinWidth;
			m_CustomWinLevel[ni].nCenter = nWinCenter;
		}
		
		m_CustomWinLevel[ni].nID = nID;

		ni ++;
	}
}


void CDcmSeriesCompareView::SetWinLevelOptions(CustomWinLevelStruct *pWinLevel)
{
	int ni;
	const TCHAR *szCustomWinLevelSection = _T("CustomWinLevel");
	CString szKey1,szKey2,szKey3;
	CWinApp *pApp = AfxGetApp();

	for (ni = 0; ni < 10; ni ++)
	{
		szKey1.Format(_T("CustomWindowName%02d"),ni + 1);
		szKey2.Format(_T("CustomWindowWidth%02d"), ni + 1);
		szKey3.Format(_T("CustomWindowCenter%02d"), ni + 1);

		if (_tcscmp((pWinLevel + ni)->sName, m_CustomWinLevel[ni].sName) != 0) {
			_tcscpy(m_CustomWinLevel[ni].sName,(pWinLevel + ni)->sName);

			pApp->WriteProfileString(szCustomWinLevelSection,szKey1,m_CustomWinLevel[ni].sName);
		}

		if ((pWinLevel + ni)->nWidth != m_CustomWinLevel[ni].nWidth) {
			m_CustomWinLevel[ni].nWidth = (pWinLevel + ni)->nWidth;
			pApp->WriteProfileInt(szCustomWinLevelSection,szKey2,m_CustomWinLevel[ni].nWidth);
		}

		if ((pWinLevel + ni)->nCenter != m_CustomWinLevel[ni].nCenter) {
			m_CustomWinLevel[ni].nCenter = (pWinLevel + ni)->nCenter;
			pApp->WriteProfileInt(szCustomWinLevelSection,szKey3,m_CustomWinLevel[ni].nCenter);
		}
	}
}


void CDcmSeriesCompareView::SetSensitivity(int nValue)
{
	const TCHAR *szSettingsSection = _T("Settings");
	CWinApp *pApp = AfxGetApp();

	if (nValue > 0) 
	{
		m_nSensitivity = nValue;
		m_wndDicomViewer.SetSensitivity(m_nSensitivity);
		pApp->WriteProfileInt(szSettingsSection, _T("Sensitivity"), nValue);
	}
}


void CDcmSeriesCompareView::ShowOptionsPopupMenu()
{
	CPoint ptCurrent;
	CMenu popupMenu, *pContextMenu;

	::GetCursorPos(&ptCurrent);

	popupMenu.LoadMenu(ID_DCMVIEW_OPTIONS);
	pContextMenu = popupMenu.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, this);
}



void CDcmSeriesCompareView::SeriesPopupMenu()
{
	CMenu menu, *pPopupMenu;
	CRect rc(0,0,0,0);
	CObList *pSeriesObjectList;
	CSeriesObject *pSeriesObject;
	POSITION pos;
	CString sCaption(""), sKey("");
	SeriesObjectSort1XData *pSeriesSort1XObj;
	std::vector <SeriesObjectSort1XData *> lstSeriesSort1XObj;
	std::vector <SeriesObjectSort1XData *> ::iterator itSeriesSort1XObj; 
	int nIndex, nSeriesNo;

	menu.CreatePopupMenu();

	pSeriesObjectList = GetDocument()->GetSeriesObjectList();

	for (nIndex = 0; (nIndex < pSeriesObjectList->GetCount() && (nIndex < 40)); nIndex ++) {
			
		pos = pSeriesObjectList->FindIndex(nIndex);
		pSeriesObject = (CSeriesObject *) pSeriesObjectList->GetAt(pos);

		if (pSeriesObject != NULL) {
			if (pSeriesObject->m_sModality.Compare(_T("MR")) == 0) {
				sCaption.Format(_T("%s %s %6s %20s -- %s"), 
						pSeriesObject->m_sStudyDate, 
						pSeriesObject->m_sPatientName, 
						pSeriesObject->m_sSeriesNo,
						pSeriesObject->m_sSequenceName,
						pSeriesObject->m_sSeriesDescription);					
			}
			else {
					sCaption.Format(_T("%s %s %s"), pSeriesObject->m_sStudyDate, 
						pSeriesObject->m_sPatientName, 
						pSeriesObject->m_sSeriesNo);
			}
				
			nSeriesNo = _ttoi(pSeriesObject->m_sSeriesNo);

			if (nSeriesNo > 0)
				sKey.Format(_T("%s:%10d"), pSeriesObject->m_sStudyUID, nSeriesNo);
			else
				sKey.Format(_T("%s:%10s"), pSeriesObject->m_sStudyUID, pSeriesObject->m_sSeriesNo);
					
			pSeriesSort1XObj = new SeriesObjectSort1XData;
			pSeriesSort1XObj->index = nIndex;
			pSeriesSort1XObj->key = CStringA(sKey);
			pSeriesSort1XObj->caption = CStringA(sCaption);

			lstSeriesSort1XObj.push_back(pSeriesSort1XObj);
		}
	}

	std::stable_sort(lstSeriesSort1XObj.begin(), lstSeriesSort1XObj.end(), SeriesObjectSort1XData_Compare);

	for (itSeriesSort1XObj = lstSeriesSort1XObj.begin(); itSeriesSort1XObj != lstSeriesSort1XObj.end(); itSeriesSort1XObj ++) {
		menu.AppendMenu(MF_STRING, m_nSeriesGoIDs[(*itSeriesSort1XObj)->index], CString((*itSeriesSort1XObj)->caption.c_str()));
		delete (*itSeriesSort1XObj);
	}

	lstSeriesSort1XObj.clear();

    GetParentFrame()->SendMessage(TB_GETRECT, ID_SERIESGO, (LPARAM)&rc);
 
    ClientToScreen(&rc);

	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, this, &rc);
}

void CDcmSeriesCompareView::SeriesGo(UINT nID)
{
	int	ni, nIndex = -1;
	POSITION pos;
	CObList *pSeriesObjectList;
	CSeriesObject *pSeriesObject;

	pSeriesObjectList = GetDocument()->GetSeriesObjectList();

	for (ni = 0; ni < (sizeof(m_nSeriesGoIDs) / sizeof(UINT))  && nIndex == -1; ni ++) {
		if (m_nSeriesGoIDs[ni] == nID) 
			nIndex = ni;
	}
	
	if (nIndex >= 0 && nIndex < pSeriesObjectList->GetCount()) {
		pos = pSeriesObjectList->FindIndex(nIndex);
		if (pos != NULL) {
			pSeriesObject = (CSeriesObject *) pSeriesObjectList->GetAt(pos);

			if (pSeriesObject != m_wndDicomViewer.GetSeriesObject() && pSeriesObject != NULL) {				
				m_wndDicomViewer.DisplaySeriesObject(pSeriesObject);
					m_wndImgLayout.SetLabelText();
			}
		}
	}
}

void CDcmSeriesCompareView::SetFakeColor(UINT nID)
{
	int nColorSetNo = 0;

	switch(nID) {
	case ID_FAKECOLOR_SET0:
		nColorSetNo = 0; 
		break;
	case ID_FAKECOLOR_SET1:
		nColorSetNo = 1; 
		break;
	case ID_FAKECOLOR_SET2:
		nColorSetNo = 2; 
		break;
	case ID_FAKECOLOR_SET3:
		nColorSetNo = 3; 
		break;
	case ID_FAKECOLOR_SET4:
		nColorSetNo = 4; 
		break;
	case ID_FAKECOLOR_SET5:
		nColorSetNo = 5; 
		break;
	case ID_FAKECOLOR_SET6:
		nColorSetNo = 6; 
		break;
	case ID_FAKECOLOR_SET7:
		nColorSetNo = 7; 
		break;
	case ID_FAKECOLOR_SET8:
		nColorSetNo = 8; 
		break;
	case ID_FAKECOLOR_SET9:
		nColorSetNo = 9; 
		break;
	case ID_FAKECOLOR_SETA:
		nColorSetNo = 10; 
		break;
	case ID_FAKECOLOR_SETB:
		nColorSetNo = 11; 
		break;
	case ID_FAKECOLOR_SETC:
		nColorSetNo = 12; 
		break;
	case ID_FAKECOLOR_SETD:
		nColorSetNo = 13; 
		break;
	}

	m_wndDicomViewer.SetFakeColorSetNo(nColorSetNo);
	m_nFakeColorSet = nColorSetNo;

}

void CDcmSeriesCompareView::ExportImages()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CDirDialog *pDirDlg = NULL;
	CFileDialog *pFileSaveDlg = NULL;
	CString sPathName(""), sFilter(""), sFileName("");

	if (!pApp->IsExportImagesEnabled()) { 
		AfxMessageBox(_T("本机没有导出影像资料的权限!"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}


	if (m_wndDicomViewer.GetSeriesMode()) {
		
		pDirDlg = new CDirDialog();
		pDirDlg->m_strTitle = _T("将当前序列影像资料以JPEG文件格式保存到指定目录:");

		if (IDCANCEL == pDirDlg->DoBrowse(this))
		{
			delete pDirDlg;
			return ;
		}

		sPathName = pDirDlg->m_strPath;
		
		delete pDirDlg;

		if (sPathName.IsEmpty()) return;

		m_wndDicomViewer.ExportAllDicomObjectsToJpegFiles(sPathName);
	}
	else {

		if (m_wndDicomViewer.InAutoBrowsing())
		{
			m_wndDicomViewer.ExitBrowseMode();
		}

		sFilter = _T("JPEG格式(*.JPG)|*.JPG|");

		pFileSaveDlg = new CFileDialog(FALSE,
			_T("JPG"),
			_T(""),
			OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			(LPCTSTR)sFilter,this);

		if (IDCANCEL == pFileSaveDlg->DoModal())
		{
			delete pFileSaveDlg;
			return;
		}

		sFileName = pFileSaveDlg->GetPathName();

		delete pFileSaveDlg;

		m_wndDicomViewer.ExportOneDicomObjectToJpegFile(sFileName);	
	}

	return;
}


void CDcmSeriesCompareView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_wndDicomViewer.SetSeriesObjectList(GetDocument()->GetSeriesObjectList());

	m_wndDicomViewer.SetTextColor(m_nTextColor);
	m_wndDicomViewer.SetBkColor(m_nBkColor);
	m_wndDicomViewer.SetBkMode(m_nBkMode);

	m_wndDicomViewer.SetMatrix(m_nSeriesRows,m_nSeriesCols,TRUE);
	m_wndDicomViewer.SetMatrix(m_nRows,m_nCols,FALSE);

	m_wndDicomViewer.SetRegistered(TRUE);
	
	m_wndDicomViewer.SetSensitivity(m_nSensitivity);

	m_wndDicomViewer.CreateCellWnds(TRUE);

	m_wndDicomViewer.SetDrawOverlay(m_bDrawOverlay);
	m_wndDicomViewer.SetShowScrollBar(TRUE); //m_bShowScrollBar);

	m_wndDicomViewer.SetLBFunc1Cursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_DCMIMAGE_SCROLL)));
	m_wndDicomViewer.SetLBFunc2Cursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_DCMIMAGE_MOVE)));
	m_wndDicomViewer.SetMeasureCursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_DCMIMAGE_MEASURE)));
	m_wndDicomViewer.SetRBFunc1Cursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_DCMIMAGE_WINLEVEL)));
	m_wndDicomViewer.SetRBFunc2Cursor(AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_DCMIMAGE_ZOOM)));

	m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
	m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);

	m_wndDicomViewer.SetCineSpeed(10);




	m_bInitialized	= TRUE;
}


void CDcmSeriesCompareView::OnUpdate(CView* pSender, LPARAM lHint, CObject* /*pHint*/)
{
	CCellWnd *pCellWnd = NULL;
	CSeriesObject *pCurrentSeriesObj = NULL;

	if (!m_bInitialized) 
		return;

	if (!m_wndDicomViewer.GetSeriesMode()) {
		
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();

		if (m_wndDicomViewer.GetCineMode() != 0)
			m_wndDicomViewer.SetCineMode(0);
	}

	pCurrentSeriesObj = GetDocument()->GetCurrrentSeriesObject();
	if (pCurrentSeriesObj == NULL) {
		pCellWnd = m_wndDicomViewer.GetCellWndSelected();
		if (pCellWnd) 
			pCurrentSeriesObj = pCellWnd->GetSeriesObject();
	}

	m_wndDicomViewer.DisplaySeriesObject(pCurrentSeriesObj);

	//if (m_wndDicomViewer.GetSeriesMode() || lHint == 1)
	m_wndDicomViewer.DisplaySeriesObjectList(pCurrentSeriesObj);
	//else
	//	m_wndDicomViewer.DisplaySeriesObject(pCurrentSeriesObj);
		
	m_wndDicomViewer.Invalidate();
}


void CDcmSeriesCompareView::OnUpdateActions(CCmdUI *pCmdUI)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	BOOL bSeriesMode, bEmpty, bEnabled, bChecked, bCheckButton;
	CCellWnd *pSelectedWnd;
	CDicomObject *pDicomObject;
	int ni, nCount, nFrameIndex, nFrameCount, nIndex;
	POSITION pos;
	CString sText;
	CustomWinLevelStruct *pCustomWinLevel;
	CObList *pSeriesObjectList;
	CSeriesObject *pSeriesObject;

	bEnabled = FALSE;
	bCheckButton = FALSE;
	bChecked = FALSE;
	bSeriesMode = m_wndDicomViewer.GetSeriesMode();


	if (bSeriesMode)
		nCount  = m_wndDicomViewer.GetSeriesObjectCount();
	else
		nCount  = m_wndDicomViewer.GetDicomObjectCount();
	bEmpty = (nCount == 0);

	switch(pCmdUI->m_nID) {
	case ID_DCMVIEW_SERIESMODE:
		bEnabled = TRUE;
		break;
	case ID_SERIESGO:
	case ID_SERIESGO_00:
	case ID_SERIESGO_01:
	case ID_SERIESGO_02:
	case ID_SERIESGO_03:
	case ID_SERIESGO_04:
	case ID_SERIESGO_05:
	case ID_SERIESGO_06:
	case ID_SERIESGO_07:
	case ID_SERIESGO_08:
	case ID_SERIESGO_09:
	case ID_SERIESGO_10:
	case ID_SERIESGO_11:
	case ID_SERIESGO_12:
	case ID_SERIESGO_13:
	case ID_SERIESGO_14:
	case ID_SERIESGO_15:
	case ID_SERIESGO_16:
	case ID_SERIESGO_17:
	case ID_SERIESGO_18:
	case ID_SERIESGO_19:
	case ID_SERIESGO_20:
	case ID_SERIESGO_21:
	case ID_SERIESGO_22:
	case ID_SERIESGO_23:
	case ID_SERIESGO_24:
	case ID_SERIESGO_25:
	case ID_SERIESGO_26:
	case ID_SERIESGO_27:
	case ID_SERIESGO_28:
	case ID_SERIESGO_29:
	case ID_SERIESGO_30:
	case ID_SERIESGO_31:
	case ID_SERIESGO_32:
	case ID_SERIESGO_33:
	case ID_SERIESGO_34:
	case ID_SERIESGO_35:
	case ID_SERIESGO_36:
	case ID_SERIESGO_37:
	case ID_SERIESGO_38:
	case ID_SERIESGO_39:

		nIndex = -1;
		for (ni = 0; ni < (sizeof(m_nSeriesGoIDs) / sizeof(UINT)) && nIndex == -1; ni ++) {
			if (m_nSeriesGoIDs[ni] == pCmdUI->m_nID) 
				nIndex = ni;
		}

		pSeriesObjectList = GetDocument()->GetSeriesObjectList();
		if (nIndex >= 0 && nIndex < pSeriesObjectList->GetCount()) {
			pos = pSeriesObjectList->FindIndex(nIndex);

			if (pos != NULL) {
				pSeriesObject = (CSeriesObject *) pSeriesObjectList->GetAt(pos);

				if (pSeriesObject == m_wndDicomViewer.GetSeriesObject())
					bChecked = TRUE;
			}	
		}

		bEnabled = (pSeriesObjectList->GetCount() >= 1);
		bCheckButton = TRUE;

		break;
	case ID_DCMVIEW_IMGLAYOUT:
		bEnabled = TRUE;
		break;
	case ID_DCMVIEW_OVERLAY:
		bEnabled = !bEmpty;
		break;
	case ID_DCMVIEW_COLORMAP:
		bEnabled = TRUE;
		break;
	case ID_FAKECOLOR_SET0:
		bEnabled = (m_nFakeColorSet > 0);
		break;
	case ID_FAKECOLOR_SET1:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 1);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET2:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 2);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET3:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 3);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET4:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 4);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET5:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 5);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET6:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 6);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET7:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 7);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET8:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 8);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SET9:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 9);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SETA:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 10);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SETB:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 11);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SETC:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 12);
		bCheckButton = TRUE;
		break;
	case ID_FAKECOLOR_SETD:
		bEnabled = !bEmpty;
		bChecked = (m_nFakeColorSet == 13);
		bCheckButton = TRUE;
		break;
	case ID_CINE_PREV:
		bEnabled = (nCount > 0) && !bSeriesMode;
		if (bEnabled) {
			pSelectedWnd = m_wndDicomViewer.GetCellWndSelected();
			if (pSelectedWnd == NULL) bEnabled = FALSE;
			if (bEnabled) {
				bEnabled = pSelectedWnd->DicomObjectIsExisting();
		
				if (bEnabled)
				{
					pDicomObject = pSelectedWnd->GetDicomObject();

					bEnabled = pDicomObject->IsMultiFrame();

					if (bEnabled)
					{
						nFrameIndex = pDicomObject->GetFrameIndex();
						bEnabled = (nFrameIndex >= 1);
					}
				}
			}
		}
		break;
	case ID_CINE_NEXT:
		bEnabled = (nCount > 0) && !bSeriesMode;

		if (bEnabled)
		{
			pSelectedWnd = m_wndDicomViewer.GetCellWndSelected();

			if (pSelectedWnd == NULL) bEnabled = FALSE;

			if (bEnabled)
			{
				bEnabled = pSelectedWnd->DicomObjectIsExisting();
		
				if (bEnabled)
				{
					pDicomObject = pSelectedWnd->GetDicomObject();

					bEnabled = pDicomObject->IsMultiFrame();
					if (bEnabled)
					{
						nFrameIndex = pDicomObject->GetFrameIndex();
						nFrameCount = pDicomObject->GetFrameCount();
						bEnabled = (nFrameIndex < nFrameCount - 1);
					}
				}
			}
		}

		break;
	case ID_CINE_PLAY:
	case ID_CINE_PLAYR:
	case ID_CINE_PLAYB:
		bEnabled = (nCount > 0) && !bSeriesMode;

		if (bEnabled) {
			pSelectedWnd = m_wndDicomViewer.GetCellWndSelected();

			if (pSelectedWnd == NULL) bEnabled = FALSE;

			if (bEnabled) {
				bEnabled = pSelectedWnd->DicomObjectIsExisting();
		
				if (bEnabled) {
					pDicomObject = pSelectedWnd->GetDicomObject();
					bEnabled = pDicomObject->IsMultiFrame();
				}

				if ( pCmdUI->m_nID == ID_CINE_PLAY)
					bChecked = (pSelectedWnd->GetCineMode() == 1);
				else if (pCmdUI->m_nID == ID_CINE_PLAYR)
					bChecked = (pSelectedWnd->GetCineMode() == 3);
				else if (pCmdUI->m_nID == ID_CINE_PLAYB)
					bChecked = (pSelectedWnd->GetCineMode() == 3);

			}
		}
		bCheckButton = TRUE;
		break;

	case ID_CINE_STOP:
		bEnabled = (nCount > 0) && !bSeriesMode;

		if (bEnabled) {
			pSelectedWnd = m_wndDicomViewer.GetCellWndSelected();

			if (pSelectedWnd == NULL) bEnabled = FALSE;

			if (bEnabled) {
				bEnabled = pSelectedWnd->DicomObjectIsExisting();
		
				if (bEnabled) {
					pDicomObject = pSelectedWnd->GetDicomObject();
					bEnabled = pDicomObject->IsMultiFrame();
				}

				if (bEnabled)
					bEnabled = (pSelectedWnd->GetCineMode() > 0);
			}
		}
		break;
	case ID_DCMVIEW_LBFUNC0:
		bEnabled = !bEmpty;
		bChecked = (m_nLBFuncNo == 0);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_LBFUNC1:
		bEnabled = !bEmpty;
		bChecked = (m_nLBFuncNo == 1);
		bCheckButton = TRUE;
		break;
	case ID_WINLEVEL_INPUT:
	case ID_WINLEVEL_PRESETWL:
	case ID_WINLEVEL_AUTO:
	case ID_WINLEVEL_CHEST:
	case ID_WINLEVEL_ABDOMEN:
	case ID_WINLEVEL_LUNG:
	case ID_WINLEVEL_BRAIN:
	case ID_WINLEVEL_BONE:
	case ID_WINLEVEL_HEAD:
		bEnabled = !bEmpty;
		break;
	case ID_WINLEVEL_CUSTOM1:
	case ID_WINLEVEL_CUSTOM2:
	case ID_WINLEVEL_CUSTOM3:
	case ID_WINLEVEL_CUSTOM4:
	case ID_WINLEVEL_CUSTOM5:
	case ID_WINLEVEL_CUSTOM6:
	case ID_WINLEVEL_CUSTOM7:
	case ID_WINLEVEL_CUSTOM8:
	case ID_WINLEVEL_CUSTOM9:
	case ID_WINLEVEL_CUSTOM10:

		if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM1)
			nIndex = 0;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM2)
			nIndex = 1;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM3)
			nIndex = 2;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM4)
			nIndex = 3;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM5)
			nIndex = 4;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM6)
			nIndex = 5;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM7)
			nIndex = 6;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM8)
			nIndex = 7;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM9)
			nIndex = 8;
		else if (pCmdUI->m_nID == ID_WINLEVEL_CUSTOM10)
			nIndex = 9;

		pCustomWinLevel = &m_CustomWinLevel[nIndex];

		if (_tcslen(pCustomWinLevel->sName) > 0)
			sText.Format(_T("W=%4d,L=%4d - %s\t\t(ALT + %c)"),pCustomWinLevel->nWidth,pCustomWinLevel->nCenter, pCustomWinLevel->sName, _T('0') + nIndex);
		else
			sText.Format(_T("W=%4d,L=%4d - %s\t\t(ALT + %c)"),pCustomWinLevel->nWidth,pCustomWinLevel->nCenter, _T("未定义"), _T('0') + nIndex);

		pCmdUI->SetText(sText);
		bEnabled = !bEmpty;

		break;
	case ID_DCMVIEW_LBFUNC2:
		bEnabled = !bEmpty;
		bChecked = (m_nLBFuncNo == 2);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_LBFUNC3:
		bEnabled = !bEmpty;
		bChecked = (m_nLBFuncNo == 3);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_LBFUNC4:
		bEnabled = !bEmpty;
		bChecked = (m_nLBFuncNo == 4);
		bCheckButton = TRUE;
		break;
	case ID_MAGNIFY_100:
		bEnabled = !bEmpty;
		bChecked = ((m_wndDicomViewer.GetMagnifierZoomFactor() == 1.0));
		bCheckButton = TRUE;
		break;
	case ID_MAGNIFY_200:
		bEnabled = !bEmpty;
		bChecked = ((m_wndDicomViewer.GetMagnifierZoomFactor() == 2.0));
		bCheckButton = TRUE;
		break;
	case ID_MAGNIFY_300:
		bEnabled = !bEmpty;
		bChecked = ((m_wndDicomViewer.GetMagnifierZoomFactor() == 3.0));
		bCheckButton = TRUE;
		break;
	case ID_MAGNIFY_400:
		bEnabled = !bEmpty;
		bChecked = ((m_wndDicomViewer.GetMagnifierZoomFactor() == 4.0));
		bCheckButton = TRUE;
		break;
	case ID_MAGNIFY_800:
		bEnabled = !bEmpty;
		bChecked = ((m_wndDicomViewer.GetMagnifierZoomFactor() == 8.0));
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_RBFUNC0:
		bEnabled = !bEmpty;
		bChecked = (m_nRBFuncNo == 0);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_RBFUNC1:
		bEnabled = !bEmpty;
		bChecked = (m_nRBFuncNo == 1);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_RBFUNC2:
		bEnabled = !bEmpty;
		bChecked = (m_nRBFuncNo == 2);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_RBFUNC3:
		bEnabled = !bEmpty;
		bChecked = (m_nRBFuncNo == 3);
		bCheckButton = TRUE;
		break;
	case ID_ZOOM_AUTO:
	case ID_ZOOM_100:
	case ID_ZOOM_50:
	case ID_ZOOM_25:
	case ID_ZOOM_125:
	case ID_ZOOM_150:
	case ID_ZOOM_175:
	case ID_ZOOM_200:
	case ID_ZOOM_250:
	case ID_ZOOM_300:
		bEnabled = !bEmpty;
		break;
	case ID_MEASURE_LINE:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 0);
		bCheckButton = TRUE;
		break;
	case ID_MEASURE_RECT:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 1);
		bCheckButton = TRUE;
		break;
	case ID_MEASURE_ELLIPSE:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 2);
		bCheckButton = TRUE;
		break;
	case ID_MEASURE_POLYGON:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 3);
		bCheckButton = TRUE;
		break;
	case ID_MEASURE_ANGLE:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 4);
		bCheckButton = TRUE;
		break;
	case ID_MEASURE_COBB:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 5);
		bCheckButton = TRUE;
		break;
	case ID_MEASURE_CALIBRATE:
		bEnabled = !bEmpty;
		bChecked = (m_wndDicomViewer.GetMeasureType() == 99);
		bCheckButton = TRUE;
		break;
	case ID_DCMVIEW_HFLIP:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_VFLIP:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_ROTATE90:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_ROTATE270:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_FILTER_SMOOTH1:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_FILTER_SMOOTH2:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_FILTER_SMOOTH3:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_FILTER_SHARP1:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_FILTER_CLEAR:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_RESET:
		bEnabled = !bEmpty;
		break;		
	case ID_DCMVIEW_VIEWREPORT:
		bEnabled = pApp->IsPulishedReportViewOpened();
		break;
	case ID_DCMVIEW_OPTIONS:
	case ID_DCMVIEW_OPTIONS_WINLEVEL:
	case ID_WINLEVEL_SENSITIVITY:
		bEnabled = TRUE;
		break;
	case ID_DCMVIEW_EXPORT:
		bEnabled = (nCount > 0);
		if (bEnabled) {
			pSelectedWnd = m_wndDicomViewer.GetCellWndSelected();

			if (pSelectedWnd == NULL) bEnabled = FALSE;

			if (bEnabled) { 
				bEnabled = pSelectedWnd->DicomObjectIsExisting();
			
				if (bEnabled) {
				
					pDicomObject = pSelectedWnd->GetDicomObject();

					if (pDicomObject->IsMultiFrame())
						bEnabled = false;
				}
			}
		}
		break;
	case ID_CHILDFRAME_CLOSE:
		bEnabled = TRUE;
		break;
	}

	pCmdUI->Enable(bEnabled);

	if (bCheckButton)
		pCmdUI->SetCheck(bChecked ? 1 : 0);
}

void CDcmSeriesCompareView::OnExecuteActions(UINT nCommandID)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSeriesObject *pSeriesObject = NULL;
	CCellWnd *pCellWndSelected = NULL;
	int nIndex = 0;

	switch(nCommandID) {
	case ID_DCMVIEW_SERIESMODE:
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();

		if (m_wndDicomViewer.GetCineMode() != 0)
			m_wndDicomViewer.SetCineMode(0);

		if (m_wndDicomViewer.GetSeriesMode()) {
	
		}
		else {
			pCellWndSelected = m_wndDicomViewer.GetCellWndSelected();

			if (pCellWndSelected != NULL) 
				pSeriesObject = pCellWndSelected->GetSeriesObject();
			else 
				pSeriesObject = m_wndDicomViewer.GetSeriesObject();

			m_wndDicomViewer.DisplaySeriesObjectList(pSeriesObject);
			m_wndImgLayout.SetLabelText();
		}
		break;
	case ID_SERIESGO:
		SeriesPopupMenu();
		break;
	case ID_SERIESGO_00:
	case ID_SERIESGO_01:
	case ID_SERIESGO_02:
	case ID_SERIESGO_03:
	case ID_SERIESGO_04:
	case ID_SERIESGO_05:
	case ID_SERIESGO_06:
	case ID_SERIESGO_07:
	case ID_SERIESGO_08:
	case ID_SERIESGO_09:
	case ID_SERIESGO_10:
	case ID_SERIESGO_11:
	case ID_SERIESGO_12:
	case ID_SERIESGO_13:
	case ID_SERIESGO_14:
	case ID_SERIESGO_15:
	case ID_SERIESGO_16:
	case ID_SERIESGO_17:
	case ID_SERIESGO_18:
	case ID_SERIESGO_19:
	case ID_SERIESGO_20:
	case ID_SERIESGO_21:
	case ID_SERIESGO_22:
	case ID_SERIESGO_23:
	case ID_SERIESGO_24:
	case ID_SERIESGO_25:
	case ID_SERIESGO_26:
	case ID_SERIESGO_27:
	case ID_SERIESGO_28:
	case ID_SERIESGO_29:
	case ID_SERIESGO_30:
	case ID_SERIESGO_31:
	case ID_SERIESGO_32:
	case ID_SERIESGO_33:
	case ID_SERIESGO_34:
	case ID_SERIESGO_35:
	case ID_SERIESGO_36:
	case ID_SERIESGO_37:
	case ID_SERIESGO_38:
	case ID_SERIESGO_39:
		SeriesGo(nCommandID);
		break;
	case ID_DCMVIEW_IMGLAYOUT:
		if (m_wndDicomViewer.GetSeriesMode())
			m_wndImgLayout.SetRowsCols(m_nSeriesRows2,m_nSeriesCols2);
		else
			m_wndImgLayout.SetRowsCols(m_nRows2,m_nCols2);
		
		m_wndImgLayout.ShowWindow(SW_SHOW);
		m_wndImgLayout.SetLabelText();
		break;
	case ID_DCMVIEW_OVERLAY:
		m_bDrawOverlay = !m_bDrawOverlay;
		m_wndDicomViewer.SetDrawOverlay(m_bDrawOverlay);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_COLORMAP:
		m_wndDicomViewer.SetFakeColorSetNo(m_nFakeColorSet);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_FAKECOLOR_SET0:
	case ID_FAKECOLOR_SET1:
	case ID_FAKECOLOR_SET2:
	case ID_FAKECOLOR_SET3:
	case ID_FAKECOLOR_SET4:
	case ID_FAKECOLOR_SET5:
	case ID_FAKECOLOR_SET6:
	case ID_FAKECOLOR_SET7:
	case ID_FAKECOLOR_SET8:
	case ID_FAKECOLOR_SET9:
	case ID_FAKECOLOR_SETA:
	case ID_FAKECOLOR_SETB:
	case ID_FAKECOLOR_SETC:
	case ID_FAKECOLOR_SETD:
		SetFakeColor(nCommandID);
		break;
	case ID_CINE_PREV:
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();

		m_wndDicomViewer.CineMove(-1);
		m_wndDicomViewer.Invalidate();

		break;
	case ID_CINE_NEXT:
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();

		m_wndDicomViewer.CineMove(+1);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_CINE_PLAY:
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();
		
		m_wndDicomViewer.SetCineMode(1);
		break;

	case ID_CINE_PLAYR:
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();
		
		m_wndDicomViewer.SetCineMode(2);
		break;
	case ID_CINE_PLAYB:
		if (m_wndDicomViewer.InAutoBrowsing())
			m_wndDicomViewer.ExitBrowseMode();
		
		m_wndDicomViewer.SetCineMode(3);
		break;
	case ID_CINE_STOP:
		m_wndDicomViewer.SetCineMode(0);
		break;
	case ID_DCMVIEW_LBFUNC0:
		m_nLBFuncNo = 0;
		m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
		break;
	case ID_DCMVIEW_LBFUNC1:
		m_nLBFuncNo = 1;
		m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
		break;
	case ID_DCMVIEW_LBFUNC2:
		m_nLBFuncNo = 2;
		m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
		break;
	case ID_WINLEVEL_INPUT:
		{
			double fcen = 0, fwid = 0;

			m_wndDicomViewer.GetWinLevel(fcen,fwid);
			m_wndWinLevel.SetWinLevel(fcen,fwid);

			m_wndWinLevel.ShowWindow(SW_SHOW);
		
		}
		break;
	case ID_WINLEVEL_PRESETWL:
		m_wndPresetWL.ShowWindow(SW_SHOW);
		break;
	case ID_WINLEVEL_AUTO:
		m_wndDicomViewer.SetWinLevel(0,0);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_CHEST:
		m_wndDicomViewer.SetWinLevel(40,350);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_ABDOMEN:
		m_wndDicomViewer.SetWinLevel(40,350);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_LUNG:
		m_wndDicomViewer.SetWinLevel(-600,1500);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_BRAIN:
		m_wndDicomViewer.SetWinLevel(40,80);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_BONE:
		m_wndDicomViewer.SetWinLevel(480,2500);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_HEAD:
		m_wndDicomViewer.SetWinLevel(90,350);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}
		break;
	case ID_WINLEVEL_CUSTOM1:
	case ID_WINLEVEL_CUSTOM2:
	case ID_WINLEVEL_CUSTOM3:
	case ID_WINLEVEL_CUSTOM4:
	case ID_WINLEVEL_CUSTOM5:
	case ID_WINLEVEL_CUSTOM6:
	case ID_WINLEVEL_CUSTOM7:
	case ID_WINLEVEL_CUSTOM8:
	case ID_WINLEVEL_CUSTOM9:
	case ID_WINLEVEL_CUSTOM10:

		if (nCommandID == ID_WINLEVEL_CUSTOM1) 
			nIndex = 0;
		else if (nCommandID == ID_WINLEVEL_CUSTOM2)
			nIndex = 1;
		else if (nCommandID == ID_WINLEVEL_CUSTOM3)
			nIndex = 2;
		else if (nCommandID == ID_WINLEVEL_CUSTOM4)
			nIndex = 3;
		else if (nCommandID == ID_WINLEVEL_CUSTOM5)
			nIndex = 4;
		else if (nCommandID == ID_WINLEVEL_CUSTOM6)
			nIndex = 5;
		else if (nCommandID == ID_WINLEVEL_CUSTOM7)
			nIndex = 6;
		else if (nCommandID == ID_WINLEVEL_CUSTOM8)
			nIndex = 7;
		else if (nCommandID == ID_WINLEVEL_CUSTOM9)
			nIndex = 8;
		else if (nCommandID == ID_WINLEVEL_CUSTOM10)
			nIndex = 9;
		else 
			nIndex = 0;

		m_wndDicomViewer.SetWinLevel(m_CustomWinLevel[nIndex].nCenter,m_CustomWinLevel[nIndex].nWidth);
		m_wndDicomViewer.Invalidate();
		if (m_nRBFuncNo != 1 && !m_wndDicomViewer.PopupMenuIsWorking()) {
			m_nRBFuncNo = 1;
			m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		}

		break;
	case ID_DCMVIEW_LBFUNC3:
		m_nLBFuncNo = 3;
		m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
		break;
	case ID_DCMVIEW_LBFUNC4:
		m_nLBFuncNo = 4;
		m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
		break;
	case ID_MAGNIFY_100:
	case ID_MAGNIFY_200:
	case ID_MAGNIFY_300:
	case ID_MAGNIFY_400:
	case ID_MAGNIFY_800:
		{
			float factor;
			if (nCommandID == ID_MAGNIFY_100)
				factor = 1.0;
			else if (nCommandID == ID_MAGNIFY_200)
				factor = 2.0;
			else if (nCommandID == ID_MAGNIFY_300)
				factor = 3.0;
			else if (nCommandID == ID_MAGNIFY_400)
				factor = 4.0;
			else if (nCommandID == ID_MAGNIFY_800)
				factor = 8.0;
			else
				factor = 2.0;

			m_wndDicomViewer.SetMagnifierZoomFactor(factor);
		
			if (m_nLBFuncNo != 4 && !m_wndDicomViewer.PopupMenuIsWorking()) {
				m_nLBFuncNo = 4;
				m_wndDicomViewer.SetLBFuncNo(m_nLBFuncNo);
			}
		}
		break;
	case ID_DCMVIEW_RBFUNC0:
		m_nRBFuncNo = 0;
		m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		break;
	case ID_DCMVIEW_RBFUNC1:
		m_nRBFuncNo = 1;
		m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		break;
	case ID_DCMVIEW_RBFUNC2:
		m_nRBFuncNo = 2;
		m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		break;
	case ID_DCMVIEW_RBFUNC3:
		m_nRBFuncNo = 3;
		m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
		break;
	case ID_ZOOM_AUTO:
	case ID_ZOOM_100:
	case ID_ZOOM_50:
	case ID_ZOOM_25:
	case ID_ZOOM_125:
	case ID_ZOOM_150:
	case ID_ZOOM_175:
	case ID_ZOOM_200:
	case ID_ZOOM_250:
	case ID_ZOOM_300:
		{
			float fZoomFactor = 0;
			if (nCommandID == ID_ZOOM_AUTO)
				fZoomFactor = 0;
			else if (nCommandID == ID_ZOOM_100)
				fZoomFactor = 1;
			else if (nCommandID == ID_ZOOM_50)
				fZoomFactor = 0.5;
			else if (nCommandID == ID_ZOOM_25)
				fZoomFactor = 0.25;
			else if (nCommandID == ID_ZOOM_125)
				fZoomFactor = 1.25;
			else if (nCommandID == ID_ZOOM_150)
				fZoomFactor = 1.5;
			else if (nCommandID == ID_ZOOM_175)
				fZoomFactor = 1.75;
			else if (nCommandID == ID_ZOOM_200)
				fZoomFactor = 2.0;
			else if (nCommandID == ID_ZOOM_250)
				fZoomFactor = 2.5;
			else if (nCommandID == ID_ZOOM_300)
				fZoomFactor = 3.0;
			else
				fZoomFactor = 0;

			m_wndDicomViewer.SetZoomFactor(fZoomFactor);
			m_wndDicomViewer.Invalidate();

			if (m_nRBFuncNo != 2 && !m_wndDicomViewer.PopupMenuIsWorking()) {
				m_nRBFuncNo = 2;
				m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
			}
		}
		break;
	case ID_MEASURE_LINE:
	case ID_MEASURE_RECT:
	case ID_MEASURE_ELLIPSE:
	case ID_MEASURE_POLYGON:
	case ID_MEASURE_ANGLE:
	case ID_MEASURE_COBB:
	case ID_MEASURE_CALIBRATE: 
		{
			int nToolType = 0;
			if (nCommandID == ID_MEASURE_LINE)
				nToolType = 0;
			else if (nCommandID == ID_MEASURE_RECT)
				nToolType = 1;
			else if (nCommandID == ID_MEASURE_ELLIPSE)
				nToolType = 2;
			else if (nCommandID == ID_MEASURE_POLYGON)
				nToolType = 3;
			else if (nCommandID == ID_MEASURE_ANGLE)
				nToolType = 4;
			else if (nCommandID == ID_MEASURE_COBB)
				nToolType = 5;
			else if (nCommandID == ID_MEASURE_CALIBRATE)
				nToolType = 99;
			else 
				nToolType = 0;

			m_wndDicomViewer.SetMeasureType(nToolType);
			if (m_nRBFuncNo != 3 && !m_wndDicomViewer.PopupMenuIsWorking()) {
				m_nRBFuncNo = 3;
				m_wndDicomViewer.SetRBFuncNo(m_nRBFuncNo);
			}

			if (nToolType == 99)
				ShowMessage(403);
		}
		break;
	case ID_DCMVIEW_HFLIP:
		m_wndDicomViewer.HFlip();
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_VFLIP:
		m_wndDicomViewer.VFlip();
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_ROTATE90:
		m_wndDicomViewer.RotateAngle90(1);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_ROTATE270:
		m_wndDicomViewer.RotateAngle90(-1);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_FILTER_SMOOTH1:
		m_wndDicomViewer.SetSmoothFilterNo(1);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_FILTER_SMOOTH2:
		m_wndDicomViewer.SetSmoothFilterNo(2);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_FILTER_SMOOTH3:
		m_wndDicomViewer.SetSmoothFilterNo(3);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_FILTER_SHARP1:
		m_wndDicomViewer.SetSharpFilterNo(1);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_FILTER_CLEAR:
		m_wndDicomViewer.SetSmoothFilterNo(0);
		m_wndDicomViewer.SetSharpFilterNo(0);
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_RESET:
		m_wndDicomViewer.ResetAll();
		m_wndDicomViewer.Invalidate();
		break;
	case ID_DCMVIEW_VIEWREPORT:
		pApp->LocateExamReport(this->GetDocument()->GetStudyGUID());
		break;
	case ID_DCMVIEW_OPTIONS:
		ShowOptionsPopupMenu();
		break;
	case ID_DCMVIEW_OPTIONS_WINLEVEL:
		m_wndWinLevelOptions.SetCustomWinLevel(&m_CustomWinLevel[0]);
		m_wndWinLevelOptions.ShowWindow(SW_SHOW);
		break;
	case ID_WINLEVEL_SENSITIVITY:
		m_wndSensitivityDlg.SetSensitivity(m_nSensitivity);
		m_wndSensitivityDlg.ShowWindow(SW_SHOW);
		break;
	case ID_DCMVIEW_EXPORT:
		ExportImages();
		break;
	case ID_CHILDFRAME_CLOSE:
		GetParent()->SendMessage(WM_CLOSE);
		break;
	}
}

