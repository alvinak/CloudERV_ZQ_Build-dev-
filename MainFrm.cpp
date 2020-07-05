// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CloudERV.h"

#include "MainFrm.h"
#include "DicomObject.h"

#include "EncodeStringDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDR_PANE_FEATURES	10

#define ID_QUERYREPORT					32994
#define ID_VIEWPDFREPORT				33010
#define ID_VIEWEXTREPORT				33011
#define ID_VIEWEXTDCMIMAGE				33012
#define ID_VIEWEXTBMPIMAGE				33013
#define ID_BOOKINGOFRIS					33014
#define ID_BOOKINGOFUIS					33015
#define ID_BOOKINGOFEIS					33016
#define ID_ENCODESTRING					33020

#define ID_APP_QUIT						33098
#define ID_TEST							33099

typedef enum 
{
	IDX_QUERY_REPORT,
	IDX_BOOKING,
	IDX_OTHER_TOOLS,
	IDX_APP_QUIT,
} INDEX_OF_PANE_FEATURES;


typedef struct
{
	INDEX_OF_PANE_FEATURES   m_nIndex;
	UINT	m_nIDCommand;
	TCHAR * m_pszText;
	UINT    m_nIDIcon;
} FOLDER_ENTRY_OF_FEATURES;


typedef struct
{
	UINT						m_nFolderID;
	TCHAR *						m_sTitle;
	UINT						m_nEntries;
	FOLDER_ENTRY_OF_FEATURES*   m_pEntries;
} FOLDER_DEF_OF_FEATURES;



static FOLDER_ENTRY_OF_FEATURES AdminToolShortcuts[] =
{
	{ IDX_QUERY_REPORT,				ID_QUERYREPORT,					_T("影像检查结果"),			IDI_SEARCH		},	
	{ IDX_QUERY_REPORT,				ID_VIEWEXTREPORT,				_T("外部PDF报告"),			IDI_SEARCH		},
	{ IDX_QUERY_REPORT,				ID_VIEWEXTDCMIMAGE,				_T("外部DICOM图像"),		IDI_SEARCH		},
	{ IDX_QUERY_REPORT,				ID_VIEWEXTBMPIMAGE,				_T("外部JPG/BMP图像"),		IDI_SEARCH		},
	{ IDX_APP_QUIT,					ID_APP_QUIT,					_T("结束操作"),				IDI_SEARCH		},
	
};

static FOLDER_ENTRY_OF_FEATURES BookingToolShortcuts[] =
{
	{ IDX_BOOKING,				ID_BOOKINGOFRIS,				_T("放射检查预约"),			IDI_SEARCH		},	
	{ IDX_BOOKING,				ID_BOOKINGOFUIS,				_T("超声检查预约"),			IDI_SEARCH		},
	{ IDX_BOOKING,				ID_BOOKINGOFEIS,				_T("内镜检查预约"),			IDI_SEARCH		},	
};


static FOLDER_ENTRY_OF_FEATURES OtherToolShortcuts[] =
{
	{ IDX_OTHER_TOOLS,				ID_ENCODESTRING,				_T("加密字符串"),			IDI_SEARCH		},	

};



static FOLDER_DEF_OF_FEATURES  FolderDefsOfFeatures[] =
{
	{ 500,		_T("影像检查报告"),		_countof(AdminToolShortcuts),	AdminToolShortcuts},
	{ 501,		_T("影像检查预约"),		_countof(BookingToolShortcuts),	BookingToolShortcuts},
	{ 510,		_T("其他功能"),			_countof(OtherToolShortcuts),	OtherToolShortcuts},
};

static unsigned char masktable[] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };


extern SharedDataStruct SharedData;
extern const TCHAR *g_lpszClassName;

class CDockingFrameWnd : public CFrameWnd
{
public:
	CDockingFrameWnd(){};
	DECLARE_DYNAMIC(CDockingFrameWnd);
};
IMPLEMENT_DYNAMIC(CDockingFrameWnd, CFrameWnd);

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(XTPWM_TASKPANEL_NOTIFY, OnTaskPanelNotify)
	//ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)

	ON_XTP_CREATECONTROL()

	ON_UPDATE_COMMAND_UI(ID_QUERYREPORT, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_VIEWPDFREPORT, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_VIEWEXTREPORT, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_VIEWEXTDCMIMAGE, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_VIEWEXTBMPIMAGE, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_BOOKINGOFRIS, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_BOOKINGOFUIS, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_BOOKINGOFEIS, OnUpdateActions)

	ON_UPDATE_COMMAND_UI(ID_ENCODESTRING, OnUpdateActions)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	XTPSkinManager()->GetResourceFile()->SetModuleHandle(AfxGetInstanceHandle());
	XTPSkinManager()->LoadSkin(NULL, _T("NORMALBLUE_INI"));
	CXTPPaintManager::SetTheme((XTPPaintTheme) xtpThemeResource);

	m_pImageLoadFailedPopup = NULL;
	m_pWaitingPriorThreadPopup = NULL;

	m_bFirstShow = TRUE;

	m_hCallerhWnd = NULL;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (!m_wndMsgBox.Create(this))
		return -1;

	if (!m_wndProgress.Create(this))
		return -1;

	if (!InitCommandBars())
		return -1;

	m_paneManager.InstallDockingPanes(this);
	m_paneManager.SetTheme( (XTPDockingPanePaintTheme) xtpThemeResource );

	VERIFY(m_wndMDITabClient.Attach(this,true));
	m_wndMDITabClient.EnableToolTips();
	m_wndMDITabClient.GetPaintManager()->SetPosition(xtpTabPositionBottom);

	m_pPaneFeatures = m_paneManager.CreatePane(IDR_PANE_FEATURES, CRect(0,0, 220, 120), xtpPaneDockLeft);
	m_pPaneFeatures->SetTitle(_T("功能导航条"));
	m_pPaneFeatures->SetOptions(xtpPaneNoCloseable | xtpPaneNoFloatable);

	if (!m_wndFeaturesTaskPane.Create(WS_VISIBLE | WS_CHILD, CRect(0,50, 180, 520), this, 1))
		return -1;

	m_wndFeaturesTaskPane.SetOwner(this);

	m_pPaneFeatures->Attach(&m_wndFeaturesTaskPane);
	m_wndFeaturesTaskPane.SetTheme(xtpTaskPanelThemeOffice2003Plain);

	m_wndFeaturesTaskPane.GetImageManager()->SetIcon(IDI_SEARCH, 0);
	m_wndFeaturesTaskPane.GetImageManager()->SetIcon(IDI_SEARCH, 1);
	m_wndFeaturesTaskPane.GetImageManager()->SetIcon(IDI_SEARCH, 2);
	m_wndFeaturesTaskPane.SetMargins(10,10,10,10,10);

	m_wndFeaturesTaskPane.SetSelectItemOnFocus(TRUE);
	m_wndFeaturesTaskPane.AllowDrag(FALSE);
	m_wndFeaturesTaskPane.SetAnimation(xtpTaskPanelAnimationYes);

	CreateFeaturesTaskPane();

	CreateRTFPopup(&m_pImageLoadFailedPopup, IDR_FAILEDTOLOADIMAGE);
	CreateRTFPopup(&m_pWaitingPriorThreadPopup, IDR_WAITINGPRIORTHREAD);


	return 0;
}


void CMainFrame::CreateRTFPopup(CXTPPopupControl **pPopup, UINT nID)
{
	CXTPPopupItem *pItemText;
	CXTPPopupItem *pItemIcon;
	CSize szPopup(0, 0);

	if (*pPopup == NULL)
		*pPopup = new CXTPPopupControl(FALSE);
	
	(*pPopup)->RemoveAllItems();
	(*pPopup)->SetTransparency(255);
	(*pPopup)->AllowMove(TRUE);
	(*pPopup)->SetAnimateDelay(300);
	(*pPopup)->SetPopupAnimation((XTPPopupAnimation)xtpPopupAnimationSlide);
	(*pPopup)->SetShowDelay(8000);

	pItemText = (*pPopup)->AddItem(new CXTPPopupItem( CRect(8, 12, 500, 130)));
	pItemText->SetRTFText(nID);
	pItemText->FitToContent();
	szPopup = pItemText->GetRect().Size();

	pItemIcon = (CXTPPopupItem*) (*pPopup)->AddItem(
		new CXTPPopupItem(CRect(szPopup.cx + 2, 10, szPopup.cx + 2 + 16, 10 + 16)));

	(*pPopup)->GetImageManager()->SetIcon(IDI_POPUP_CLOSE, IDI_POPUP_CLOSE);
	pItemIcon->SetIconIndex(IDI_POPUP_CLOSE);

	pItemIcon->SetButton(TRUE);
	pItemIcon->SetID(XTP_ID_POPUP_CLOSE);

	szPopup.cx += 20;
	szPopup.cy += 20;

	(*pPopup)->SetPopupSize(szPopup);

	(*pPopup)->SetTheme(xtpPopupThemeOffice2007);
	
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hMapFile;
	void  *pBuf;
	int nResult, nSize;
	CString sMapName("");
	CMoveParam *pInfo;
	BOOL	bShow;
	int nRemaining,nCompleted;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();


	if (message == 0x3888)
	{
		nResult = 1;
		sMapName.Format(_T("MAP3888%u"),wParam);
		
		nSize = sizeof(SharedDataStruct);

		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, sMapName);
		
		if (hMapFile != INVALID_HANDLE_VALUE)
		{
			pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nSize);
			
			if (pBuf != NULL)
			{
				memcpy(&SharedData,pBuf,nSize);

				UnmapViewOfFile(pBuf);

				nResult = 0;
			}

			CloseHandle(hMapFile);
		}

		return (nResult == 0);
	}
	else if (message == WM_REFRESHPARAMS)
	{
		HWND hWnd = NULL;
		// 影像浏览器模式
		// 有2个参数控制: sdt, ivm
		// sdt	= 0,  影像浏览器模式无效
		//		= 1， 载入老系统的影像资料
		//		= 2， 载入新系统的影像资料
		// ivm	= 0,  采用CloudERV配置的参数决定所用的浏览器类型
		//		= 1,  采用简易浏览器显示
		//		= 2， 采用专业浏览器显示
		//		= 3， 老资料用简易浏览器，新资料用专业浏览器


		hWnd = (HWND) SharedData.hWnd;

		if (::IsWindow(hWnd) && !::IsWindow(m_hCallerhWnd))
			m_hCallerhWnd = hWnd;

		if (SharedData.nSDT == 1 || SharedData.nSDT == 2) {
			pApp->LoadImagesOfStudy( (SharedData.nSDT == 1), SharedData.nIVM, SharedData.szStudyGUID, SharedData.szStudyUID, SharedData.szAccessionNo, SharedData.szDocID, SharedData.szStudyDate);
			return true;
		}

		// 报告查询模式
		if (_tcslen(SharedData.szOrderNo) > 0 || _tcslen(SharedData.szInpatientNo) > 0 || _tcslen(SharedData.szOutpatientNo) > 0 || _tcslen(SharedData.szPatientID) > 0 || _tcslen(SharedData.szDocID) > 0 ) {
			pApp->ViewMedicalReport(SharedData.szSystem, SharedData.szOrderNo, SharedData.szInpatientNo, SharedData.szOutpatientNo, SharedData.szPatientID, SharedData.szDocID);
		}

		return true;
	}
	else if (message == 0x1010) { // 显示打开PACS图像失败
		
		if (m_pImageLoadFailedPopup) {
			if (wParam == 0) {
				m_pImageLoadFailedPopup->Show(this);
			}
			else
				m_pImageLoadFailedPopup->Hide();
		}
		
		return true;
	}
	else if (message == 0x1011) {  //前一个装入PACS图像的线程还在运行

		if (m_pWaitingPriorThreadPopup) {
			if (wParam == 0) {
				m_pWaitingPriorThreadPopup->Show(this);
			}
			else
				m_pWaitingPriorThreadPopup->Hide();
		}
	
		return true;
	}
	else if (message == WM_IMAGEISCOMING)
	{
		pInfo = (CMoveParam *) wParam;
		
		pApp->ViewDcmImages(CString(pInfo->m_sStudyGUID), pInfo->m_pSeriesObjectList, TRUE, TRUE);

		return true;
	}
	else if (message == WM_SHOWMESSAGE)
	{
		ShowMessageEx(wParam);

		return true;
	}
	else if (message == WM_SHOWPROGRESS)
	{
		nRemaining = (int) wParam;
		nCompleted = (int) lParam;

		if (nRemaining == 0 || (nRemaining == 0 && nCompleted == 0))
		{
			bShow = FALSE;
		}
		else
		{
			bShow = TRUE;
		}

		ShowProgressEx(bShow,nRemaining,nCompleted);

		return true;
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}



void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	int nLeft,nTop,nWidth,nHeight;
	CString sNotifyMsg("");
	CRect rect,rt;

	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	if (m_bFirstShow && !bMinimized)
	{
		///RebuildCustomWinLevelMenu(NULL);

		m_bFirstShow = FALSE;
		
		GetClientRect(&rect);
		ClientToScreen(&rect);

		nWidth = (int) (rect.Width() * 0.8);
		nHeight = (int) (rect.Height() * 0.8);

		nWidth = (int) (rect.Width() * 0.75);
		nHeight = (int) (rect.Height() * 0.75);

		nLeft = (int) (rect.left + nWidth * 0.125);
		nTop = (int) (rect.top + nHeight * 0.125 + 20);

		/*
		m_wndQuery.MoveWindow(nLeft,nTop,nWidth,nHeight);

		m_wndPrintSCU.GetWindowRect(&rt);

		if (g_bPrintSCURectIsValid && m_rtPrintSCUDlg.Width() >= rt.Width() && m_rtPrintSCUDlg.Height() >= rt.Height())
		{
			m_wndPrintSCU.MoveWindow(&m_rtPrintSCUDlg);
		}
		else
		{
			nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
			nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);	
			m_wndPrintSCU.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());
		}


		m_wndPresetWL.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);	
		m_wndPresetWL.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());

		m_wndSetMatrix.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);	
		m_wndSetMatrix.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());

		m_wndWinLevel.GetWindowRect(&rt);
		nLeft =  rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop =  rect.top + ((rect.Height() - rt.Height()) >> 1);
		m_wndWinLevel.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());	

		m_wndMeasureOptions.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);
		m_wndMeasureOptions.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());

		m_wndWinLevelOptions.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);
		m_wndWinLevelOptions.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());

		m_wndSensitivity.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);
		m_wndSensitivity.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());

		*/

		m_wndProgress.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);
		m_wndProgress.MoveWindow(nLeft,nTop + 20,rt.Width(),rt.Height());
	

		m_wndMsgBox.GetWindowRect(&rt);
		nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
		nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);
		m_wndMsgBox.MoveWindow(nLeft,nTop,rt.Width(),rt.Height());

		/*
		m_wndOutput.GetWindowRect(&rt);

		nWidth = (int) (rect.Width() * 0.8);
		nHeight = (int) (rect.Height() * 0.8);

		nLeft = (int) (rect.left + nWidth * 0.1);
		nTop = (int) (rect.top + nHeight * 0.1 + 20);
		
		m_wndOutput.MoveWindow(nLeft,nTop,nWidth,nHeight);

		m_wndMiniBar.GetWindowRect(&rt);

		if (m_nMiniBarPosLeft < rect.left || m_nMiniBarPosTop < rect.top || m_nMiniBarPosLeft > (rect.left + rect.Width() - 20) || m_nMiniBarPosTop > (rect.top + rect.Height() - 20))
		{
			nLeft = rect.left + ((rect.Width() - rt.Width()) >> 1);
			nTop = rect.top + ((rect.Height() - rt.Height()) >> 1);
		}
		else
		{
			nLeft = m_nMiniBarPosLeft;
			nTop  = m_nMiniBarPosTop;
		}

		m_wndMiniBar.MoveWindow(nLeft,nTop + 10,rt.Width(),rt.Height());
		
		m_wndNotify.GetWindowRect(&rt);

		nLeft = rect.left + (rect.Width() - rt.Width());
		nTop  = rect.bottom  - rt.Height();

		m_wndNotify.MoveWindow(nLeft,nTop, rt.Width() , rt.Height());

		sNotifyMsg = m_ServerInfo.GetSystemNotify();
		
		sNotifyMsg.Trim();

		if (sNotifyMsg != "")
		{
			m_wndNotify.SetMessage(sNotifyMsg);
			m_wndNotify.ShowWindow(SW_SHOW);
		}

		m_wndQuery.ShowWindow(SW_SHOW);

		if (::IsWindow(m_hCallerhWnd))
		{
			::BringWindowToTop(m_hCallerhWnd);
		}
		*/
	}
}

//	
// 为创建无菜单的MDI框架,还需要覆盖下列2个方法
//
 BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	return CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd,pContext);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* /*pContext*/)
{
	return CreateClient(lpcs,NULL);
}


void CMainFrame::ShowProgressEx(BOOL bShow,int nRemaining,int nCompleted,const TCHAR *szMsg)
{
	if (bShow)
	{
		m_wndProgress.ShowWindow(SW_SHOW);
		m_wndProgress.ChangePos(nRemaining,nCompleted,szMsg);
		m_wndProgress.UpdateWindow();
		m_wndProgress.Invalidate();

//		m_wndProgress.BringWindowToTop();
	}
	else
	{
		m_wndProgress.ShowWindow(SW_HIDE);
	}

}


void CMainFrame::ShowMessageEx(int nCode)
{
	CString sMsg("");

	switch(nCode)
	{
	case 0:
	case 100:
		sMsg = _T("");
		break;
	case 101:
		sMsg.Format(_T("正在从影像服务器中查找所需的影像资料,请等待..."));
		break;
	case 102:
		sMsg.Format(_T("Failed to Initialize Network!"));
		break;
	case 103:
		sMsg.Format(_T("Failed to create Association Parameters !"));
		break;
	case 104: 
		sMsg.Format(_T("连接服务器失败,再次重试时系统将尝试连接备用服务器!"));
		break;
	case 105:
		sMsg.Format(_T("请求被DICOM服务器拒绝!"));
		break;
	case 106:
		sMsg.Format(_T("AcceptedPresentationContextID = 0!"));
		break;
	case 107:
		sMsg.Format(_T("DICOM服务器中找不到所需要的影像资料!"));
		break;
	case 201:
		sMsg.Format(_T("交换位置: 请用鼠标左键单击需要交换位置的第1个序列或影像"));
		break;
	case 202:
		sMsg.Format(_T("交换位置: 请用鼠标左键单击需要交换位置的第2个序列或影像"));
		break;
	case 401:
		sMsg.Format(_T("定标: 当前窗格中的没有影像，无法进行定标操作!"));
		break;
	case 402:
		sMsg.Format(_T("定标: 当前窗口中的影像已经具有定标数据，不需要手工定标!"));
		break;
	case 403:
		sMsg.Format(_T("定标: 请按住鼠标右键沿着影像上所带的标尺拉出一条线段，并输入线段的长度"));
		break;
	default:
		sMsg.Format(_T("未知的提示信息代码: %d !"),nCode);
	}

	ShowMessageEx(sMsg);
}

void CMainFrame::ShowMessageEx(const TCHAR *szMsg)
{
	if (_tcslen(szMsg) > 0)
	{
		m_wndMsgBox.ShowMessage(szMsg);
		m_wndMsgBox.Invalidate();
	}
	else
	{
		m_wndMsgBox.ShowWindow(SW_HIDE);
	}
}


CFrameWnd* CMainFrame::GetParentDockingFrame(CWnd* pWnd)
{
	if (pWnd)
	{
		return DYNAMIC_DOWNCAST(CDockingFrameWnd, pWnd->GetParentFrame());
	}
	return NULL;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.x = 0;
	cs.y = 0;
	cs.cx = ::GetSystemMetrics(SM_CXFULLSCREEN);
	cs.cy = ::GetSystemMetrics(SM_CYFULLSCREEN);
	cs.style &=~FWS_ADDTOTITLE;

	cs.lpszClass = g_lpszClassName;

	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));


	/////////////////////////////////////////////////////////////
    
	if (cs.hMenu!=NULL) {
		::DestroyMenu(cs.hMenu);      // delete menu if loaded
		cs.hMenu = NULL;              // no menu for this window
	}

	/////////////////////////////////////////////////////////////
	return TRUE;
}


void CMainFrame::CreateFeaturesTaskPane(void)
{
	bool bFirst = true;
	int nFolder;

	m_wndFeaturesTaskPane.GetGroups()->Clear();

	// Add folders to the outlook bar. Input values are folder text
	// and lParam value.
	for (nFolder = 0; nFolder < _countof(FolderDefsOfFeatures); nFolder ++)
	{
		// Get pointer to folder definition
		FOLDER_DEF_OF_FEATURES* pFolder = &FolderDefsOfFeatures[nFolder];

		CXTPTaskPanelGroup* pGroup;
		
		pGroup = m_wndFeaturesTaskPane.AddGroup(pFolder->m_nFolderID);
		pGroup->SetCaption(pFolder->m_sTitle);

		if (bFirst) {
			if (pGroup) {
				pGroup->SetExpanded(true);
				bFirst = false;
			}
		}

		// Add folder entries
		UINT nEntry;
		for (nEntry = 0; nEntry < pFolder->m_nEntries; nEntry++)
		{
			// Get pointer to entry, load text and add entry
			FOLDER_ENTRY_OF_FEATURES* pDef = &pFolder->m_pEntries[nEntry];
			CXTPTaskPanelGroupItem *pItem;
			pItem = pGroup->AddLinkItem(pDef->m_nIDCommand, pDef->m_nIndex);
			pItem->SetCaption(pDef->m_pszText);
		
			pItem->SetEnabled(IsActionEnabled(pItem->GetID()));

			m_wndFeaturesTaskPane.GetImageManager()->SetIcon(pDef->m_nIDCommand, pDef->m_nIndex);
		}
	}

	return;
}

void CMainFrame::SetTaskPaneAutoHide(BOOL bAutoHide)
{
	if (bAutoHide) {
		if (!m_pPaneFeatures->IsHidden()) { 
			m_paneManager.ToggleAutoHide(m_pPaneFeatures);
		}
		else {
			m_pPaneFeatures->Hide();
		}
		m_paneManager.Invalidate();
	}

	return;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnClose()
{
	CString sMsg;

	sMsg.Format(_T("你是否要结束操作并退出程序?"));

	if (::MessageBox(m_hWnd, sMsg, GetTitle(), MB_ICONQUESTION | MB_YESNO) != IDYES) 
		return;


	CMDIFrameWnd::OnClose();
}


LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		// get a pointer to the docking pane being shown.
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
		if (!pPane->IsValid())
		{
			CWnd* pWnd = NULL;
			if (m_mapPanes.Lookup(pPane->GetID(), pWnd))
			{
				pPane->Attach(pWnd);
			}
		}

		return TRUE; // handled
	}

	if (wParam == XTP_DPN_CONTEXTMENU)
	{
		/*
		XTP_DOCKINGPANE_CLICK* pClick = (XTP_DOCKINGPANE_CLICK*)lParam;

		CXTPDockingPane* pPopupPane = pClick->pPane;
		if (!pPopupPane) 
			return FALSE;

		CPoint pos;
		GetCursorPos(&pos);

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_PANES));
		CMenu* pPopup = menu.GetSubMenu(0);
		pPopupPane->SetFocus();

		CXTPCommandBars::TrackPopupMenu(pPopup, TPM_RIGHTBUTTON, pos.x, pos.y, this);		
		*/

		return TRUE;
	}

	if (wParam == XTP_DPN_ACTION)
	{
		XTP_DOCKINGPANE_ACTION* pAction = (XTP_DOCKINGPANE_ACTION*)lParam;
		
		if (pAction->action == xtpPaneActionActivated)
		{
			CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, pAction->pPane->GetChild());

			if (pFrame)
			{
				CDocument* pDocument = pFrame->GetActiveDocument();
				if (pDocument)
				{
					HMENU hMenuAlt = ((CMultiDocTemplate*)pDocument->GetDocTemplate())->m_hMenuShared;
					::SendMessage(m_hWndMDIClient, WM_MDISETMENU, (WPARAM)hMenuAlt, NULL);
				}
			}

			return TRUE;
		}
		
		if (pAction->action == xtpPaneActionDeactivated)
		{
			CMDIChildWnd* pFrame = MDIGetActive();
			if (pFrame)
			{
				pFrame->OnUpdateFrameMenu(TRUE, pFrame, 0);
			}
			else
			{
				OnUpdateFrameMenu(0);
			}

			return TRUE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////
// m_wndTaskPanel->SetOwner(this);                     
// 确保创建CXTPTaskPanel消息能够传给MainFrame窗口
//////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTaskPanelNotify(WPARAM wParam, LPARAM lParam)
{
	switch(wParam) {
	case XTP_TPN_CLICK:
		{
			CXTPTaskPanelGroupItem* pItem = (CXTPTaskPanelGroupItem*)lParam;
			SendMessage(WM_COMMAND, pItem->GetID());
			SetTaskPaneAutoHide(TRUE);
		}

		break;
	/*
	case XTP_TPN_RCLICK:
		{
			CXTPTaskPanelItem* pItem = (CXTPTaskPanelItem*)lParam;
            CString str;
            str.Format(_T("RClick Event: pItem.Caption = %s, pItem.ID = %i\n"), pItem->GetCaption(), pItem->GetID());
            MessageBox(str);
		}
		break;
		*/
	}

	return 0;
}



BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	CFrameWnd* pFrame = GetParentDockingFrame(GetFocus());

	if (pFrame)
	{
		// let the view have first crack at the command
		if (pFrame->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;

		return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}


	return CXTPMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nID;
	CFrameWnd* pFrame = GetParentDockingFrame(GetFocus());
	
	if (pFrame)
	{
		// let the view have first crack at the command
		if (AfxCallWndProc(pFrame,
			pFrame->m_hWnd,
			WM_COMMAND, wParam, lParam) != 0)
			return TRUE; // handled by child
	}
	else {
		nID = LOWORD(wParam);
		switch(nID) {
		case ID_QUERYREPORT:
			GetApp()->LaunchQueryPublishedReport();
			break;
		case ID_VIEWEXTREPORT:
			ViewExtReport();
			break;
		case ID_VIEWEXTDCMIMAGE:
			ViewExtDcmImage();
			break;
		case ID_VIEWEXTBMPIMAGE:
			ViewExtBmpImage();
			break;
		case ID_ENCODESTRING:
			EncodeString();
			break;
		case ID_BOOKINGOFRIS:
			BookingOfRIS();
			break;
		case ID_BOOKINGOFUIS:
			BookingOfRIS();
			break;
		case ID_BOOKINGOFEIS:
			BookingOfEIS();
			break;
		case ID_APP_QUIT:
			CMDIFrameWnd::OnClose();
			break;
		}
	}

	return TRUE;
	//return CMDIFrameWnd::OnCommand(wParam, lParam);
}




/*

void CMainFrame::OnWindowHide()
{
	CFrameWnd* pFrame = GetParentDockingFrame(GetFocus());

	if (pFrame)
	{
		CXTPDockingPaneTabbedContainer* pContainer = (CXTPDockingPaneTabbedContainer*)pFrame->GetParent();
		CXTPDockingPane* pPane = pContainer->GetSelected();

		pPane->Close();
	}
	else
	{
		CWnd* pActiveChildWnd = MDIGetActive(NULL);
		if (pActiveChildWnd)
			pActiveChildWnd->SendMessage(WM_COMMAND, ID_FILE_CLOSE);
	}
}

void CMainFrame::OnWindowHideall()
{
	CXTPDockingPaneInfoList& lstPanes = m_paneManager.GetPaneList();
	POSITION pos = lstPanes.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = lstPanes.GetNext(pos);
		if (!pPane->IsClosed() && !pPane->IsHidden())
			m_paneManager.HidePane(pPane);
	}

}

void CMainFrame::OnWindowAutohide()
{
	CFrameWnd* pFrame = GetParentDockingFrame(GetFocus());

	if (pFrame)
	{
		CXTPDockingPaneTabbedContainer* pContainer = (CXTPDockingPaneTabbedContainer*)pFrame->GetParent();
		CXTPDockingPane* pPane = pContainer->GetSelected();

		pPane->Hide();
	}
}

void CMainFrame::OnUpdateWindowAutohide(CCmdUI* pCmdUI)
{

	pCmdUI->Enable(GetParentDockingFrame(GetFocus()) != NULL);

}
void CMainFrame::OnUpdateWindowCloseAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(MDIGetActive() != NULL);
}

void CMainFrame::OnUpdateWindowHideall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_paneManager.GetPaneList().GetCount() > 0);

}
*/


int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	return FALSE;
}

void CMainFrame::OnDestroy()
{
	CMDIFrameWnd::OnDestroy();

	if (m_pImageLoadFailedPopup)
		delete m_pImageLoadFailedPopup;

	if (m_pWaitingPriorThreadPopup)
		delete m_pWaitingPriorThreadPopup;

	m_wndMDITabClient.Detach();

}


BOOL CMainFrame::IsActionEnabled(UINT nID)
{
	BOOL bEnabled = FALSE;

	switch(nID) {
	case ID_QUERYREPORT:	//最近报告查询
		bEnabled = TRUE;
		break;
	case ID_VIEWPDFREPORT:
		bEnabled = TRUE;
		break;
	case ID_VIEWEXTREPORT:
		bEnabled = TRUE;
		break;
	case ID_VIEWEXTDCMIMAGE:
		bEnabled = TRUE;
		break;
	case ID_VIEWEXTBMPIMAGE:
		bEnabled = TRUE;
		break;
	case ID_BOOKINGOFRIS:
		bEnabled = FALSE;
		break;
	case ID_BOOKINGOFUIS:
		bEnabled = FALSE;
		break;
	case ID_BOOKINGOFEIS:
		bEnabled = FALSE;
		break;
	case ID_ENCODESTRING:
		bEnabled = TRUE;
		break;
	case ID_TEST:
		bEnabled = TRUE;
		break;
	case ID_APP_QUIT:
		bEnabled = TRUE;
		break;
	}
	
	return bEnabled;
	
}

///////////////////////////////////////////////////////////////////////////////
// 功能可用状态更新
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateActions(CCmdUI *pCmdUI)
{
	BOOL bEnabled = FALSE;

	bEnabled = IsActionEnabled(pCmdUI->m_nID);

	pCmdUI->Enable(bEnabled);

	return;
}
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::ViewExtReport()
{
	TCHAR szBuf[32 * MAX_PATH + 1];
	CString strFilePath;
	char szPdfPswd[40] = {'C','l','o','u','d','P','A','C','S','I','s','S','o','C','o','o','l','!',0,0,0,0,0,};
	FILE *fp;
	unsigned char *pPdfBuffer;
	int nPdfBufLen;
  
	CFileDialog dlg(TRUE, _T("pdf"), 
			_T(""), 
			OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, 
			_T("Pdf文件(*.pdf)|*.pdf|Jpeg文件(*.jpg)|*.jpg|XPS文件(*.xps)|*.xps|"));

	memset(szBuf, 0, (32 * MAX_PATH + 1) * sizeof(TCHAR));
	dlg.GetOFN().lpstrFile = szBuf;
    dlg.GetOFN().nMaxFile = 32 * MAX_PATH;
 
	if (dlg.DoModal() != IDOK)
		return;

    POSITION pos = dlg.GetStartPosition();
    while(NULL != pos) {
		strFilePath = dlg.GetNextPathName(pos);
		fp = _tfopen(strFilePath,  _T("rb"));
		if (fp) {
			fseek(fp, 0, SEEK_END);
			nPdfBufLen = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			pPdfBuffer =(unsigned char *) malloc(nPdfBufLen + 1);
			fread(pPdfBuffer, 1, nPdfBufLen, fp);
			fclose(fp);

			GetApp()->ViewPDFReport(strFilePath,_T("外部报告/文档"), pPdfBuffer, nPdfBufLen, szPdfPswd, TRUE);

			free(pPdfBuffer);
		}
	}
}

void CMainFrame::ViewExtDcmImage()
{
	CObList *pSeriesObjectList;
	TCHAR szBuf[1280 * MAX_PATH + 1];
	CString strFilePath;
	CDicomObject *pNewDicomObj;
	CDicomObject *pDicomObject;
	CSeriesObject *pSeriesObject;
	CSeriesObject *pCurSeriesObj;
	int nImageWidth, nImageHeight;
	CString sPatientId("");
	CString sPatientName("");
	CString sStudyId("");
	CString sSeriesNo("");
	CString sSeriesUID("");
	CString sInstanceUID("");
	CString sModality("");
	CString sSequenceName("");
	BOOL bSeriesFound, bImageFound, bNewList;
	POSITION pos1,pos2,pos3;

	CFileDialog dlg(TRUE, _T("dcm"), 
			_T(""), 
			OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, 
			_T("Dicom影像文件(*.dcm)|*.dcm|全部文件类型(*.*)|*.*|"));

	memset(szBuf,0,(1280 * MAX_PATH  + 1) * sizeof(TCHAR));

	dlg.GetOFN().lpstrFile = szBuf;
    dlg.GetOFN().nMaxFile = 1280 * MAX_PATH;
 
	if (dlg.DoModal() != IDOK)
		return;
	
	bNewList = TRUE;
	pSeriesObjectList = new CObList;

    pos1 = dlg.GetStartPosition();

    while(NULL != pos1) {
		strFilePath = dlg.GetNextPathName(pos1);
		pNewDicomObj = new CDicomObject(strFilePath);
		nImageWidth = pNewDicomObj->GetImageWidth();
		nImageHeight = pNewDicomObj->GetImageHeight();

		if (nImageWidth > 0 && nImageHeight > 0) {

			sPatientId = pNewDicomObj->GetPatientId();

			sPatientName = pNewDicomObj->GetPatientName();

			sStudyId = pNewDicomObj->GetStudyId();

			sSeriesNo = pNewDicomObj->GetSeriesNumber();

			sSeriesUID = pNewDicomObj->GetSeriesInstanceUID();

			sInstanceUID = pNewDicomObj->GetInstanceUID();

			sModality = pNewDicomObj->GetModality();

			if (sModality == "MR") {
				sSequenceName = pNewDicomObj->GetString(DCM_SequenceName);
			}
			
			bSeriesFound = FALSE;
			bImageFound = FALSE;

			pos2 = pSeriesObjectList->GetHeadPosition();

			while (pos2 != NULL && bSeriesFound == FALSE) {
				pSeriesObject = (CSeriesObject *) pSeriesObjectList->GetNext(pos2);
				if (pSeriesObject->m_sSeriesUID == sSeriesUID) {
					bSeriesFound = TRUE;
					pCurSeriesObj = pSeriesObject;
				}
			}

			if (bSeriesFound) {
				pos3 = pCurSeriesObj->m_DicomObjectList.GetHeadPosition();

				while (pos3 != NULL && bImageFound == FALSE) {
					pDicomObject = (CDicomObject *) pCurSeriesObj->m_DicomObjectList.GetNext(pos3);

					if (sInstanceUID == pDicomObject->GetInstanceUID()) {
						bImageFound = TRUE;
					}
				}
			}
			else  {
				
				pCurSeriesObj = new CSeriesObject;

				pCurSeriesObj->m_bFromRIS = FALSE;
				pCurSeriesObj->m_bFromExtFile = TRUE;
				pCurSeriesObj->m_sPatientId = sPatientId;
				pCurSeriesObj->m_sPatientName = sPatientName;
				pCurSeriesObj->m_sStudyId = sStudyId;
				pCurSeriesObj->m_sSeriesNo = sSeriesNo;
				pCurSeriesObj->m_sSeriesUID = sSeriesUID;
				pCurSeriesObj->m_nCurrentIndex = 0;
				pCurSeriesObj->m_dwThreadId = 0;
				pCurSeriesObj->m_dwTickCount = 0;

				pCurSeriesObj->m_sModality = sModality;

				if (sModality == "MR") {
					pCurSeriesObj->m_sSequenceName = sSequenceName;
				}


				pSeriesObjectList->AddTail(pCurSeriesObj);
			
			}

			if (!bImageFound) {
				pCurSeriesObj->m_DicomObjectList.AddTail(pNewDicomObj);

				pNewDicomObj = NULL;
			}
			else	// Image Existings
			{
				delete pNewDicomObj;
				pNewDicomObj = NULL;
			}
		}
		else	// (if) nImageWidth > 0 && nImageHeight 
		{
			delete pNewDicomObj;
			pNewDicomObj = NULL;
		}
	}

	GetApp()->ViewExtDcmImages(pSeriesObjectList, FALSE, FALSE);
}

void CMainFrame::ViewExtBmpImage()
{
	TCHAR szBuf[64 * MAX_PATH + 1];
	CString strExt, strFilePath;
	int nImageWidth, nImageHeight;
	unsigned char *lpJpegData = NULL;
	long nJpegDataLen = 0;
	FILE *fp = NULL;
	POSITION pos1,pos2,pos3;
	BOOL bFirstCall = TRUE;

	CFileDialog dlg(TRUE, _T("jpg"), 
			_T(""), 
			OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, 
			_T("Jpeg文件(*.jpg)|*.jpg|Jpeg文件(*.jpeg)|*.jpeg|Bitmap文件(*.bmp)|*.bmp|"));

	memset(szBuf,0,(64 * MAX_PATH  + 1) * sizeof(TCHAR));

	dlg.GetOFN().lpstrFile = szBuf;
    dlg.GetOFN().nMaxFile = 64 * MAX_PATH;
 
	if (dlg.DoModal() != IDOK)
		return;
	
    pos1 = dlg.GetStartPosition();
	bFirstCall = TRUE;
    while(NULL != pos1) {
		strFilePath = dlg.GetNextPathName(pos1);
		strExt = strFilePath.Right(4);
		if (strExt.CompareNoCase(_T(".bmp")) == 0) {
			GetApp()->ViewExtBmpImages(strFilePath, NULL, 0, bFirstCall == TRUE);
		}
		else {
			fp = fopen(CStringA(strFilePath), "rb");
			if (fp) {

				fseek(fp, 0L, SEEK_END);
				nJpegDataLen = ftell(fp);
				fseek(fp, 0L, SEEK_SET);

				if (nJpegDataLen > 1024) {
					lpJpegData = (unsigned char *) malloc(nJpegDataLen + 1);
					fread(lpJpegData, nJpegDataLen, 1, fp);
					if (lpJpegData) {
						GetApp()->ViewExtBmpImages(NULL, lpJpegData, nJpegDataLen, bFirstCall == TRUE);
						free(lpJpegData);
						lpJpegData = NULL;
					}
				}

				fclose(fp);
			}
		}
		
		if (bFirstCall)
			bFirstCall = FALSE;
	}
}


void CMainFrame::BookingOfRIS()
{
	GetApp()->BookingOfRIS();
}

void CMainFrame::BookingOfUIS()
{
	GetApp()->BookingOfUIS();
}

void CMainFrame::BookingOfEIS()
{
	GetApp()->BookingOfEIS();
}


void CMainFrame::EncodeString()
{
	CEncodeStringDlg dlg;

	dlg.DoModal();
}