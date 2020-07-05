// BookingOfRISView.cpp : implementation of the CBookingOfRISView class
//

#include "stdafx.h"
#include "CloudERV.h"

#include "BookingOfRISDoc.h"
#include "BookingOfRISView.h"
#include "NewBookingDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Class CDayFontColor is derived from CCmdTarget.
// This class is used to associate a "day" with a font
// and a font color.
struct CDayFontColor : CCmdTarget
{
	CFont fnt;
	COLORREF color;
};


// struct CStaticFontMap is used to associate each 
// visible "day" with a CDayFontColor class.

struct CStaticFontMap : public CMap<int,int,CDayFontColor*,CDayFontColor*>
{
	~CStaticFontMap()
	{
		POSITION pos = GetStartPosition();
		int nKey;
		CDayFontColor* pFC = NULL;
		while (pos != NULL)
		{
			GetNextAssoc(pos, nKey, pFC);
			if (pFC) {
				pFC->InternalRelease();// Decrement reference count by one.
			}
		}
	}
};

static CStaticFontMap mapFontColor;

void CALLBACK DayMetricsCallback(CXTPDatePickerControl* pControl, const COleDateTime &dtDay, XTP_DAYITEM_METRICS* pDayItemMetrics, void *lpParams)
{
    // This function is used to determine if a given day is special.
    // If the day is special then the day is highlighted.

    // Get the number of days from December 30, 1899 to present day.
    // COleDateTime operator =(DATE dtSrc) Note: DATE is a double.
    // The DATE value is copied into this COleDateTime object and its 
    // status is set to valid.
	COleDateTime dtNow = (double)long(COleDateTime::GetCurrentTime());
	COleDateTimeSpan span = dtDay - dtNow;
	DayMetricsCallbackParamsInfo *pParam;

	pParam = (DayMetricsCallbackParamsInfo *) lpParams;

	static CFont fntUnderline;
	
	if (fntUnderline.GetSafeHandle() == 0)
	{
		LOGFONT lf;
		pControl->GetPaintManager()->GetDayTextFont()->GetLogFont(&lf);
		
		//lf.lfWeight = FW_BOLD;
		lf.lfUnderline = true;
		fntUnderline.CreateFontIndirect(&lf);
	}

	if (span.GetDays() > 0 && span.GetDays() <= pParam->nMaxBookingDays) {
		if (dtDay.GetDayOfWeek() == 1 || dtDay.GetDayOfWeek() == 7)
			pDayItemMetrics->clrForeground = RGB(0x7F, 0x7F, 0x7F);
		else
			pDayItemMetrics->clrForeground = RGB(0, 0, 0xFF);

		pDayItemMetrics->SetFont(&fntUnderline);
	}
	else {
		pDayItemMetrics->clrForeground = RGB(0x7F, 0x7F, 0x7F);
	}

/*
	if (dtNow.GetMonth() == dtDay.GetMonth() - 1 && dtDay.GetDay() == dtNow.GetDay())
	{
		pDayItemMetrics->SetFont(&fntBold);
		pDayItemMetrics->clrForeground = RGB(0, 0, 0xFF);
	}
*/

	CDayFontColor* pFontColor;
	if (mapFontColor.Lookup((int)dtDay, pFontColor))
	{
		pDayItemMetrics->SetFont(&pFontColor->fnt);
		pDayItemMetrics->clrForeground = pFontColor->color;
	}
}


class CQueueGroupReportRecordOfRIS : public CXTPReportRecord
{
private:
	std::vector <CQueueGroupInfoOfRIS *> *m_pQueueGroupInfoList;
	int	m_nQueueGroupInfoListSize;
public:
	CQueueGroupReportRecordOfRIS(std::vector <CQueueGroupInfoOfRIS *> *pQueueGroupInfoList, int nQueueGroupInfoListSize)
	{
		int ni;

		AddItem(new CXTPReportRecordItemText());	// 0	序号
		AddItem(new CXTPReportRecordItemText());	// 1	代码
		AddItem(new CXTPReportRecordItemText());	// 2	检查类型
		AddItem(new CXTPReportRecordItemText());	// 3	资源组名称
		for (ni = 0; ni < 7; ni ++) 		
			AddItem(new CXTPReportRecordItemText());// 4..10	星期X
			
		AddItem(new CXTPReportRecordItemText());	//11	检查地点
		AddItem(new CXTPReportRecordItemText());	//12	备注

		m_pQueueGroupInfoList = pQueueGroupInfoList;
		m_nQueueGroupInfoListSize = nQueueGroupInfoListSize;
	}

	void GetItemMetrics (XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
	{
		int nRow,nCol;
		CQueueGroupInfoOfRIS *pObj;
		CXTPReportColumnOrder* pSortOrder = pDrawArgs->pControl->GetColumns()->GetSortOrder();
		CString sText("");

		CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

		pItemMetrics->pFont = &pPaintManager->m_fontText;    
		pItemMetrics->clrForeground = pPaintManager->m_clrWindowText;    
		pItemMetrics->clrBackground = XTP_REPORT_COLOR_DEFAULT;    
		pItemMetrics->nColumnAlignment = pDrawArgs->nTextAlign;    
		pItemMetrics->nItemIcon = XTP_REPORT_NOICON;  

		nCol = pDrawArgs->pColumn->GetIndex();
		nRow = pDrawArgs->pRow->GetIndex();

		if (nRow >= 0 &&  nRow < (int) m_pQueueGroupInfoList->size()) 
		{
			pObj = (*m_pQueueGroupInfoList)[nRow];
			switch(nCol) {
			case 0:		// 序号
				sText.Format(_T("%4d"), nRow + 1);
				pItemMetrics->strText = sText;
				break;
			case 1:		// 代码
				pItemMetrics->strText = pObj->m_sQueueGroupID;
				break;
			case 2:		// 检查类型
				pItemMetrics->strText = pObj->m_sExamType;
				break;
			case 3:		// 资源名称
				pItemMetrics->strText = pObj->m_sQueueGroupName;
				break;
			case 4:		// 星期一
				if (pObj->m_nCapacityD2_AM > 0 && pObj->m_nCapacityD2_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD2_AM, pObj->m_nCapacityD2_PM);
				else if (pObj->m_nCapacityD2_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD2_AM);
				else if (pObj->m_nCapacityD2_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD2_PM);
				else 
					sText = L"";

				pItemMetrics->strText = sText;
				break;
			case 5:		// 星期二
				if (pObj->m_nCapacityD3_AM > 0 && pObj->m_nCapacityD3_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD3_AM, pObj->m_nCapacityD3_PM);
				else if (pObj->m_nCapacityD3_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD3_AM);
				else if (pObj->m_nCapacityD3_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD3_PM);
				else 
					sText = L"";
				pItemMetrics->strText = sText;
				break;
			case 6:		// 星期三
				if (pObj->m_nCapacityD4_AM > 0 && pObj->m_nCapacityD4_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD4_AM, pObj->m_nCapacityD4_PM);
				else if (pObj->m_nCapacityD4_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD4_AM);
				else if (pObj->m_nCapacityD4_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD4_PM);
				else 
					sText = L"";
				pItemMetrics->strText = sText;
				break;
			case 7:		// 星期四
				if (pObj->m_nCapacityD5_AM > 0 && pObj->m_nCapacityD5_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD5_AM, pObj->m_nCapacityD5_PM);
				else if (pObj->m_nCapacityD5_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD5_AM);
				else if (pObj->m_nCapacityD5_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD5_PM);
				else 
					sText = L"";
				pItemMetrics->strText = sText;
				break;
			case 8:		// 星期五
				if (pObj->m_nCapacityD6_AM > 0 && pObj->m_nCapacityD6_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD6_AM, pObj->m_nCapacityD6_PM);
				else if (pObj->m_nCapacityD6_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD6_AM);
				else if (pObj->m_nCapacityD6_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD6_PM);
				else 
					sText = L"";
				pItemMetrics->strText = sText;
				break;
			case 9:		// 星期六
				if (pObj->m_nCapacityD7_AM > 0 && pObj->m_nCapacityD7_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD7_AM, pObj->m_nCapacityD7_PM);
				else if (pObj->m_nCapacityD7_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD7_AM);
				else if (pObj->m_nCapacityD7_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD7_PM);
				else 
					sText = L"";
				pItemMetrics->strText = sText;
				break;
			case 10:	// 星期日
				if (pObj->m_nCapacityD1_AM > 0 && pObj->m_nCapacityD1_PM > 0)
					sText.Format(_T("上:%3d 下:%3d"), pObj->m_nCapacityD1_AM, pObj->m_nCapacityD1_PM);
				else if (pObj->m_nCapacityD1_AM > 0)
					sText.Format(_T("上:%3d"), pObj->m_nCapacityD1_AM);
				else if (pObj->m_nCapacityD1_PM > 0)
					sText.Format(_T("%6c 下:%3d"), _T(' '), pObj->m_nCapacityD1_PM);
				else 
					sText = L"";
				pItemMetrics->strText = sText;
				break;
			case 11:	// 检查地点
				pItemMetrics->strText = pObj->m_sLocation;
				break;
			case 12:	// 备注
				pItemMetrics->strText = pObj->m_sMemo;
				break;
			}
		}
	}
};



class CQueueGroupBookingReportRecordOfRIS : public CXTPReportRecord
{
private:
	std::vector <CQueueGroupBookingInfoOfRIS *> *m_pQueueGroupBookingInfoList;
	int	m_nQueueGroupBookingInfoListSize;
	int m_nMaxBookingDays;
	CString *m_pExamTypeOfMedicalOrder;

public:
	CQueueGroupBookingReportRecordOfRIS(std::vector <CQueueGroupBookingInfoOfRIS *> *pQueueGroupBookingInfoList, int nQueueGroupBookingInfoListSize, int nMaxBookingDays, CString *pExamType)
	{
		int ni;
		CXTPReportRecordItem* pItem;
		CXTPReportRecordItemControl* pButton;

		m_pQueueGroupBookingInfoList = pQueueGroupBookingInfoList;
		m_nQueueGroupBookingInfoListSize = nQueueGroupBookingInfoListSize;
		m_nMaxBookingDays = nMaxBookingDays;
		m_pExamTypeOfMedicalOrder = pExamType;

		AddItem(new CXTPReportRecordItemText());	// 0	序号
		AddItem(new CXTPReportRecordItemText());	// 1	代码
		AddItem(new CXTPReportRecordItemText());	// 2	检查类型
		AddItem(new CXTPReportRecordItemText());	// 3	资源名称

		for (ni = 0; ni < nMaxBookingDays; ni ++) {
			pItem = AddItem(new CXTPReportRecordItemText());// ni + 4 日期

			pButton = pItem->GetItemControls()->AddControl(xtpItemControlTypeButton);
			if(!pButton)
				continue;

			pButton->SetAlignment(xtpItemControlRight);
			pButton->SetIconIndex(0, 1);
			pButton->SetSize(CSize(22, 0));

			pButton = pItem->GetItemControls()->AddControl(xtpItemControlTypeButton);
			if(!pButton)
				continue;

			pButton->SetAlignment(xtpItemControlRight);
			pButton->SetIconIndex(0, 0);
			pButton->SetSize(CSize(22, 0));
		}

	}

	void GetItemMetrics (XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
	{
		int nRow,nCol, nIndex1, nIndex2, nAMLeft, nPMLeft;
		COleDateTime dt;
		CQueueGroupBookingInfoOfRIS *pObj;
		CXTPReportColumnOrder* pSortOrder = pDrawArgs->pControl->GetColumns()->GetSortOrder();
		CString sText("");

		CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

		pItemMetrics->pFont = &pPaintManager->m_fontText;    
		pItemMetrics->clrForeground = pPaintManager->m_clrWindowText;    
		pItemMetrics->clrBackground = XTP_REPORT_COLOR_DEFAULT;    
		pItemMetrics->nColumnAlignment = pDrawArgs->nTextAlign;    
		pItemMetrics->nItemIcon = XTP_REPORT_NOICON;  

		nCol = pDrawArgs->pColumn->GetIndex();
		nRow = pDrawArgs->pRow->GetIndex();

		if (nRow >= 0 &&  nRow < (int) m_pQueueGroupBookingInfoList->size()) 
		{
			pObj = (*m_pQueueGroupBookingInfoList)[nRow];

			if (pObj->m_sExamType.CompareNoCase(*m_pExamTypeOfMedicalOrder) == 0)
				pItemMetrics->clrBackground = RGB(200, 200, 100);

			if (nCol == 0) {
				sText.Format(_T("%4d"), nRow + 1);
				pItemMetrics->strText = sText;
			}
			else if (nCol == 1) {
				pItemMetrics->strText = pObj->m_sQueueGroupID;
			}
			else if (nCol == 2) {
				pItemMetrics->strText = pObj->m_sExamType;
			}
			else if (nCol == 3) {
				pItemMetrics->strText = pObj->m_sQueueGroupName;
			}
			else if (nCol >= 4 && nCol < (m_nMaxBookingDays + 4))
			{
				nIndex1 = (nCol - 4);				
				dt = pObj->m_dtFirstDay + COleDateTimeSpan(nIndex1, 0, 0 , 0);
				nIndex2 = (dt.GetDayOfWeek() - 1); 

				nAMLeft = pObj->m_nCapacity_AM[nIndex2] - pObj->m_nBookingAmount_AM[nIndex1];
				nPMLeft = pObj->m_nCapacity_PM[nIndex2] - pObj->m_nBookingAmount_PM[nIndex1];

				if (nAMLeft > 0 && nPMLeft > 0) {
					sText.Format(_T("上:%3d 下:%3d"), nAMLeft, nPMLeft);
				}
				else if (nAMLeft > 0) {
					sText.Format(_T("上:%3d"), nAMLeft, nPMLeft);
				}
				else if (nPMLeft > 0) {
					sText.Format(_T("%6c 下:%3d"), _T(' '), nPMLeft);
				}

				pItemMetrics->strText = sText;

				if ((nAMLeft + nPMLeft) > 0 && (pObj->m_nCapacity_AM[nIndex2] + pObj->m_nCapacity_PM[nIndex2]) > 0) {
					pItemMetrics->clrForeground = RGB(255, 0, 0);
				}
			}
		}
	}
};


CQueueGroupInfoOfRIS::CQueueGroupInfoOfRIS()
{
	m_sQueueGroupID		= _T("");
	m_sQueueGroupName	= _T("");
	m_sExamType			= _T("");
	m_sLocation			= _T("");
	m_sMemo				= _T("");
	m_nCapacityD1_AM	= 0;	// 周日上午
	m_nCapacityD1_PM	= 0;	// 周日下午
	m_nCapacityD2_AM	= 0;
	m_nCapacityD2_PM	= 0;
	m_nCapacityD3_AM	= 0;
	m_nCapacityD3_PM	= 0;
	m_nCapacityD4_AM	= 0;
	m_nCapacityD4_PM	= 0;
	m_nCapacityD5_AM	= 0;
	m_nCapacityD5_PM	= 0;
	m_nCapacityD6_AM	= 0;
	m_nCapacityD6_PM	= 0;
	m_nCapacityD7_AM	= 0;
	m_nCapacityD7_PM	= 0;
}


CQueueGroupInfoOfRIS::~CQueueGroupInfoOfRIS()
{

}


void CQueueGroupInfoOfRIS::ReadFromQuery(SACommand &cmd)
{
	m_sQueueGroupID		= cmd.Field("QueueGroupID").asString();
	m_sQueueGroupName	= cmd.Field("QueueGroupName").asString();
	m_sExamType			= cmd.Field("ExamType").asString();
	m_sLocation			= cmd.Field("Location").asString();
	m_sMemo				= cmd.Field("Memo").asString();
	m_nCapacityD1_AM	= cmd.Field("CapacityOfD1AM").asShort();	// 周日上午
	m_nCapacityD1_PM	= cmd.Field("CapacityOfD1PM").asShort();	// 周日下午
	m_nCapacityD2_AM	= cmd.Field("CapacityOfD2AM").asShort();	// 周一上午
	m_nCapacityD2_PM	= cmd.Field("CapacityOfD2PM").asShort();
	m_nCapacityD3_AM	= cmd.Field("CapacityOfD3AM").asShort();
	m_nCapacityD3_PM	= cmd.Field("CapacityOfD3PM").asShort();
	m_nCapacityD4_AM	= cmd.Field("CapacityOfD4AM").asShort();
	m_nCapacityD4_PM	= cmd.Field("CapacityOfD4PM").asShort();
	m_nCapacityD5_AM	= cmd.Field("CapacityOfD5AM").asShort();
	m_nCapacityD5_PM	= cmd.Field("CapacityOfD5PM").asShort();
	m_nCapacityD6_AM	= cmd.Field("CapacityOfD6AM").asShort();
	m_nCapacityD6_PM	= cmd.Field("CapacityOfD6PM").asShort();
	m_nCapacityD7_AM	= cmd.Field("CapacityOfD7AM").asShort();
	m_nCapacityD7_PM	= cmd.Field("CapacityOfD7PM").asShort();
}


CQueueGroupBookingInfoOfRIS::CQueueGroupBookingInfoOfRIS(const TCHAR *szQueueGroupID, const TCHAR *szQueueGroupName, const TCHAR *szExamType, const TCHAR *szLocation, const TCHAR *szMemo,
		int nCapacity_AM[7], int nCapacity_PM[7], COleDateTime dtFirstDay)
{
	int ni;

	m_sQueueGroupID = szQueueGroupID;
	m_sQueueGroupName = szQueueGroupName;
	m_sExamType = szExamType;
	m_sLocation = szLocation;
	m_sMemo = szMemo;

	m_dtFirstDay = dtFirstDay;

	for (ni = 0; ni < 28; ni ++) { 
		m_nBookingAmount_AM[ni] = 0;
		m_nBookingAmount_PM[ni] = 0;
	}

	for (ni = 0; ni < 7; ni ++) {
		m_nCapacity_AM[ni] = nCapacity_AM[ni];
		m_nCapacity_PM[ni] = nCapacity_PM[ni];
	}
}

CQueueGroupBookingInfoOfRIS::~CQueueGroupBookingInfoOfRIS()
{

}


// CBookingOfRISView

IMPLEMENT_DYNCREATE(CBookingOfRISView, CXTPResizeFormView)

BEGIN_MESSAGE_MAP(CBookingOfRISView, CXTPResizeFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(XTP_NM_REPORT_SELCHANGED, IDC_GRID_BOOKINGINFO, OnBookingReportSelChanged)
	ON_NOTIFY(XTP_NM_REPORT_ITEMBUTTONCLICK, IDC_GRID_BOOKINGINFO, OnBookingReportItemButtonClick)
	ON_UPDATE_COMMAND_UI_RANGE(1000, 65535, OnUpdateActions)
	ON_COMMAND_RANGE(1000, 65535, OnExecuteActions)
END_MESSAGE_MAP()

// CBookingRISView construction/destruction

CBookingOfRISView::CBookingOfRISView()
		: CXTPResizeFormView(CBookingOfRISView::IDD)

{
	m_sExamTypeOfMedicalOrder = _T("");
}

CBookingOfRISView::~CBookingOfRISView()
{
	std::vector <CQueueGroupInfoOfRIS *> ::iterator itQueueGroupInfo;
	std::vector <CQueueGroupBookingInfoOfRIS *> ::iterator itQueueGroupBookingInfo;

	for (itQueueGroupInfo = m_lstQueueGroupInfoOfRIS.begin(); itQueueGroupInfo != m_lstQueueGroupInfoOfRIS.end(); itQueueGroupInfo ++) 
		delete (*itQueueGroupInfo);

	m_lstQueueGroupInfoOfRIS.clear();

	for (itQueueGroupBookingInfo = m_lstQueueGroupBookingInfoOfRIS.begin(); itQueueGroupBookingInfo != m_lstQueueGroupBookingInfoOfRIS.end(); itQueueGroupBookingInfo ++) 
		delete (*itQueueGroupBookingInfo);

	m_lstQueueGroupBookingInfoOfRIS.clear();
}


BOOL CBookingOfRISView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CXTPResizeFormView::PreCreateWindow(cs);
}

void CBookingOfRISView::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeFormView::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_DATEPICKER, m_ctlDatePicker);
	DDX_Control(pDX, IDC_GRID_QUEUEGROUPS, m_ctlQueueGroups);
	DDX_Control(pDX, IDC_GRID_BOOKINGINFO, m_ctlQueueGroupsBookingInfo);

	DDX_Control(pDX, IDC_TXT_ORDERNO, m_ctlOrderNo);
	DDX_Control(pDX, IDC_TXT_EXAMTYPE, m_ctlExamType);
	DDX_Control(pDX, IDC_TXT_PATIENTNAME, m_ctlPatientName);
	DDX_Control(pDX, IDC_TXT_SERVICENAMES, m_ctlServiceNames);
	DDX_Control(pDX, IDC_TXT_QUEUEGROUPID, m_ctlQueueGroupID);
	DDX_Control(pDX, IDC_TXT_SCHEDULEDATE, m_ctlScheduleDate);
	DDX_Control(pDX, IDC_TXT_SCHEDULETIME, m_ctlScheduleTime);

	
}

// CBookingOfRISView drawing

void CBookingOfRISView::OnDraw(CDC* pDC)
{
	CBookingOfRISDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CBookingOfRISView diagnostics

#ifdef _DEBUG
void CBookingOfRISView::AssertValid() const
{
	CXTPResizeFormView::AssertValid();
}

void CBookingOfRISView::Dump(CDumpContext& dc) const
{
	CXTPResizeFormView::Dump(dc);
}

CBookingOfRISDoc* CBookingOfRISView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBookingOfRISDoc)));
	return (CBookingOfRISDoc*)m_pDocument;
}
#endif //_DEBUG


// CBookingOfRISView message handlers


int CBookingOfRISView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPResizeFormView::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}


BOOL CBookingOfRISView::OnPreparePrinting(CPrintInfo* pInfo)
{
	CPrintDialog *dlg = new CPrintDialog(FALSE, PD_PAGENUMS);

	pInfo->m_pPD = dlg;
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(1);

	pInfo->m_strPageDesc = GetDocument()->GetTitle(); 

	return DoPreparePrinting(pInfo);
}

void CBookingOfRISView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    if( pInfo )
    {
        CClientDC dc( this );
        pDC->SetMapMode(MM_ANISOTROPIC);

        CSize sz( dc.GetDeviceCaps(LOGPIXELSX), 
                    dc.GetDeviceCaps(LOGPIXELSY) );
        pDC->SetWindowExt( sz );
        sz = CSize( pDC->GetDeviceCaps(LOGPIXELSX),
                        pDC->GetDeviceCaps(LOGPIXELSY) );

        pDC->SetViewportExt( sz );
	}

}

void CBookingOfRISView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{	
	int nCurPage;
	int nDestWidth, nDestHeight;
	int nImageWidth = 0, nImageHeight = 0, nDataSize;
	unsigned char *pBits = NULL;
	BITMAPINFO bi;

	nDestWidth = pInfo->m_rectDraw.Width(); 
	nDestHeight = pInfo->m_rectDraw.Height();


    pDC->SetMapMode(MM_ANISOTROPIC);

	CSize sz;
   sz = CSize( pDC->GetDeviceCaps(LOGPIXELSX),
                        pDC->GetDeviceCaps(LOGPIXELSY) );
    
	pDC->SetViewportExt( sz );
	
	nCurPage = pInfo->m_nCurPage;


	CXTPResizeFormView::OnPrint(pDC, pInfo);
}

void CBookingOfRISView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{

}

void CBookingOfRISView::OnInitialUpdate()
{
	COleDateTime dtNow, dt;
	CString sHeaderTitle;
	int ni;
	CBitmap bitmap;

	CXTPResizeFormView::OnInitialUpdate();

	VERIFY(bitmap.LoadBitmap(IDB_BOOKINGREPORTITEMTB));

	m_ilIcons.Create(16, 15, ILC_COLOR24 | ILC_MASK, 0, 1);

	// load the image bitmap and set the image list for the 
	// report control Item.
	m_ilIcons.Add(&bitmap, RGB(255, 0, 255));
	m_ctlQueueGroupsBookingInfo.SetImageList(&m_ilIcons);

	m_nMaxBookingDays = 15;

	SetupScheduledDates();

	dtNow = (double)long(COleDateTime::GetCurrentTime());

	m_DayMetricsCallbackParams.nMaxBookingDays = m_nMaxBookingDays;

	SetResize(IDC_GRPBOX_REGISTERINFO, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LBL_ORDERNO, SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TXT_ORDERNO, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_EXAMTYPE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_EXAMTYPE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_PATIENTNAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_PATIENTNAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_SERVICENAMES, SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TXT_SERVICENAMES, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LINE1, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LBL_QUEUEGROUPID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_QUEUEGROUPID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_LBL_SCHEDULEDATE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_SCHEDULEDATE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_LBL_SCHEDULETIME, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_SCHEDULETIME, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_GRPBOX_DATES, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_DATEPICKER, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_GRPBOX_QUEUEGROUPS, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_GRID_QUEUEGROUPS, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_GRPBOX_BOOKINGINFO, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_GRID_BOOKINGINFO, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);


	m_ctlDatePicker.SetBorderStyle(xtpDatePickerBorderStatic);
	m_ctlDatePicker.SetFirstDayOfWeek(1);
	m_ctlDatePicker.SetHighlightToday(TRUE);
	m_ctlDatePicker.SetShowNonMonthDays(FALSE);
	m_ctlDatePicker.SetMaxSelCount(1);
	m_ctlDatePicker.SetButtonsVisible(FALSE, FALSE);
	m_ctlDatePicker.SetDesiredVisibleAndFullRange(dtNow + COleDateTimeSpan(1, 0, 0,0) , dtNow + COleDateTimeSpan(1 + m_nMaxBookingDays, 0, 0,0), dtNow + COleDateTimeSpan(1, 0, 0,0) , dtNow + COleDateTimeSpan(1 + m_nMaxBookingDays, 0, 0,0));
	m_ctlDatePicker.SetCallbackDayMetrics(DayMetricsCallback, &m_DayMetricsCallbackParams);

	m_ctlQueueGroups.GetPaintManager()->SetColumnOffice2007CustomThemeBaseColor(RGB(0,0,255));
	m_ctlQueueGroups.SetGridStyle(TRUE, xtpGridSolid);
	m_ctlQueueGroups.AllowEdit(FALSE);
	m_ctlQueueGroups.GetReportHeader()->AllowColumnRemove(FALSE);
	m_ctlQueueGroups.GetReportHeader()->AllowColumnSort(FALSE);
	m_ctlQueueGroups.GetReportHeader()->AllowColumnReorder(FALSE);
	m_ctlQueueGroups.SetMultipleSelection(FALSE);

	m_ctlQueueGroups.GetReportHeader()->SetAutoColumnSizing(FALSE); 
	m_ctlQueueGroups.GetPaintManager()->RefreshMetrics();

	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 0, _T("序号"),	 40 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 1, _T("代码"),	 80 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 2, _T("检查类型"),	100 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 3, _T("资源组名称"),160 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 4, _T("星期一"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 5, _T("星期二"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 6, _T("星期三"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 7, _T("星期四"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 8, _T("星期五"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn( 9, _T("星期六"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn(10, _T("星期日"),	 140 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn(11, _T("检查地点"),  120 ));
	m_ctlQueueGroups.AddColumn(new CXTPReportColumn(12, _T("备注"),		 180 ));

	m_ctlQueueGroupsBookingInfo.GetPaintManager()->SetColumnOffice2007CustomThemeBaseColor(RGB(0,0,255));
	m_ctlQueueGroupsBookingInfo.SetGridStyle(TRUE, xtpGridSolid);
	m_ctlQueueGroupsBookingInfo.FocusSubItems(TRUE);
	m_ctlQueueGroupsBookingInfo.AllowEdit(FALSE);
	
	m_ctlQueueGroupsBookingInfo.GetReportHeader()->AllowColumnRemove(FALSE);
	m_ctlQueueGroupsBookingInfo.GetReportHeader()->AllowColumnSort(FALSE);
	m_ctlQueueGroupsBookingInfo.GetReportHeader()->AllowColumnReorder(FALSE);
	m_ctlQueueGroupsBookingInfo.SetMultipleSelection(FALSE);
	m_ctlQueueGroupsBookingInfo.GetReportHeader()->SetAutoColumnSizing(FALSE); 
	m_ctlQueueGroupsBookingInfo.GetPaintManager()->RefreshMetrics();
	
	m_ctlQueueGroupsBookingInfo.AddColumn(new CXTPReportColumn( 0, _T("序号"),	 40 ));
	m_ctlQueueGroupsBookingInfo.AddColumn(new CXTPReportColumn( 1, _T("代码"),	 80 ));
	m_ctlQueueGroupsBookingInfo.AddColumn(new CXTPReportColumn( 2, _T("检查类型"),	100 ));
	m_ctlQueueGroupsBookingInfo.AddColumn(new CXTPReportColumn( 3, _T("资源组名称"), 160 ));

	dt = dtNow;
	for (ni = 0; ni < m_nMaxBookingDays; ni ++) {
		dt = dtNow + COleDateTimeSpan(ni + 1, 0, 0, 0);

		sHeaderTitle.Format(_T("%02d月%02d日"), dt.GetMonth(), dt.GetDay());

		m_ctlQueueGroupsBookingInfo.AddColumn(new CXTPReportColumn( ni + 4, sHeaderTitle, 180 ));
	}

	LoadQueueGroupsInfo();
	LoadBookingRecords();
}

void CBookingOfRISView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
	CBookingOfRISDoc *pDoc = GetDocument();
	
	if (lHint == 0) {
		m_ctlOrderNo.SetWindowText(pDoc->m_objMedicalOrder.m_sOrderNo);
		m_ctlExamType.SetWindowText(pDoc->m_objMedicalOrder.m_sExamType);
		m_ctlPatientName.SetWindowText(pDoc->m_objMedicalOrder.m_sPatientName);
		m_ctlServiceNames.SetWindowText(pDoc->m_objMedicalOrder.m_sServiceNames);

		m_ctlQueueGroupID.SetWindowText(pDoc->GetQueueGroupID());
		m_ctlScheduleDate.SetWindowText(pDoc->GetScheduledDate());
		m_ctlScheduleTime.SetWindowText(pDoc->GetScheduledTime());

		m_sExamTypeOfMedicalOrder = pDoc->GetExamTypeOfMedicalOrder();
	}
	else if (lHint == 1) {
		m_ctlQueueGroupID.SetWindowText(pDoc->GetQueueGroupID());
		m_ctlScheduleDate.SetWindowText(pDoc->GetScheduledDate());
		m_ctlScheduleTime.SetWindowText(pDoc->GetScheduledTime());
	}
}


void CBookingOfRISView::OnSize(UINT nType, int cx, int cy)
{
	CXTPResizeFormView::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
		return;

}


void CBookingOfRISView::OnBookingReportSelChanged(NMHDR * pNotifyStruct, LRESULT *result)
{
	CBookingOfRISDoc *pDoc = GetDocument();
	CXTPReportSelectedRows *pSelectedRows;
	CXTPReportRow *pSelectedRow;
	CQueueGroupBookingInfoOfRIS *pObject;
	CString sQueueGroupID;
	int nIndex;

	pSelectedRows = m_ctlQueueGroupsBookingInfo.GetSelectedRows();
	if (pSelectedRows != NULL) {
		pSelectedRow = pSelectedRows->GetAt(0);

		if (pSelectedRow != NULL)  {
			nIndex = pSelectedRow->GetIndex();

			if (nIndex >= 0 && nIndex < m_lstQueueGroupBookingInfoOfRIS.size()) {
				pObject = m_lstQueueGroupBookingInfoOfRIS[nIndex];

				//sQueueGroupID = pObject->m_sQueueGroupID;
				//pDoc->m_objMedicalOrder.m_sQueueGroupID = sQueueGroupID;
				//GetDocument()->UpdateAllViews(NULL, 1);
			}
		}
	}
}

void CBookingOfRISView::OnBookingReportItemButtonClick(NMHDR * pNotifyStruct, LRESULT*)
{
	CBookingOfRISDoc *pDoc = GetDocument();
	XTP_NM_REPORTITEMCONTROL* pItemNotify = (XTP_NM_REPORTITEMCONTROL*) pNotifyStruct;
	int nRowIndex, nColIndex, nDayIndex, nDayOfWeek, nControlItemIndex, nAvailNum = 0;
	CString sQueueGroupID, sScheduledDate, sScheduledTime, sMsg;
	COleDateTime dt;
	BOOL bNotAvailable; 
	CQueueGroupBookingInfoOfRIS *pBookingInfoObj; 

	if(!(pItemNotify->pRow && pItemNotify->pItem && pItemNotify->pItemControl))
		return;

	m_ctlQueueGroupsBookingInfo.SetFocusedRow(pItemNotify->pRow);

	nRowIndex = pItemNotify->pRow->GetIndex();
	nColIndex = pItemNotify->pColumn->GetIndex();
	nDayIndex = nColIndex - 4;
	nControlItemIndex = pItemNotify->pItemControl->GetIndex();

	if ( (nRowIndex >= 0 && nRowIndex < m_lstQueueGroupBookingInfoOfRIS.size()) 
			&& (nControlItemIndex == 0 || nControlItemIndex == 1) 
			&& (nColIndex >= 4 && nColIndex < (4 + m_nMaxBookingDays))) {
		
		pBookingInfoObj = m_lstQueueGroupBookingInfoOfRIS[nRowIndex];

		dt = m_dtFirstDay + COleDateTimeSpan(nDayIndex, 0, 0, 0);
		nDayOfWeek = (dt.GetDayOfWeek() - 1);

		sScheduledDate.Format(_T("%d-%02d-%02d"), dt.GetYear(), dt.GetMonth(), dt.GetDay());
		sScheduledTime = (nControlItemIndex == 0 ? _T("下午") : _T("上午"));
		sQueueGroupID = pBookingInfoObj->m_sQueueGroupID;

		if (nControlItemIndex == 0) {	
			nAvailNum = pBookingInfoObj->m_nCapacity_PM[nDayOfWeek] - pBookingInfoObj->m_nBookingAmount_PM[nDayIndex];
			bNotAvailable = (pBookingInfoObj->m_nCapacity_PM[nDayOfWeek] <= 0);
		}
		else {
			nAvailNum = pBookingInfoObj->m_nCapacity_AM[nDayOfWeek] - pBookingInfoObj->m_nBookingAmount_AM[nDayIndex];
			bNotAvailable = (pBookingInfoObj->m_nCapacity_AM[nDayOfWeek] <= 0);
		}

		if (pBookingInfoObj->m_sExamType.CompareNoCase(m_sExamTypeOfMedicalOrder) != 0) {
			sMsg.Format(_T("申请单记录的检查类型(%s)与所选资源的检查类型(%s)不一样!"),
				pBookingInfoObj->m_sExamType, m_sExamTypeOfMedicalOrder);

			::MessageBox(GetParentFrame()->m_hWnd, sMsg, _T("预约排期"), MB_ICONEXCLAMATION | MB_OK);		
			return;
		}

		if (nAvailNum <= 0) {
			sMsg.Format(_T("不能预约%d年%02d月%02d日%s, %s!"),
				dt.GetYear(), dt.GetMonth(), dt.GetDay(), sScheduledTime, 
				bNotAvailable ? _T("没有排期") : _T("预约已满"));

			::MessageBox(GetParentFrame()->m_hWnd, sMsg, _T("预约排期"), MB_ICONEXCLAMATION | MB_OK);		
		}
		else {
			pDoc->SetQueueGroupID(sQueueGroupID);
			pDoc->SetScheduledDate(sScheduledDate);
			pDoc->SetScheduledTime(sScheduledTime);

			pDoc->UpdateAllViews(NULL, 1);
		}
	}

}


void CBookingOfRISView::SetupScheduledDates()
{
	int ni;
	COleDateTime dt;
	CString sScheduledDate;

	m_dtToday = (double)long(COleDateTime::GetCurrentTime());
	m_dtFirstDay = m_dtToday + COleDateTimeSpan(1, 0, 0, 0);
	m_dtLastDay = m_dtFirstDay + COleDateTimeSpan(m_nMaxBookingDays - 1, 0, 0, 0);

	for (ni = 0; ni < 28; ni ++) {
		dt = m_dtToday + COleDateTimeSpan(ni + 1, 0, 0, 0);
		sScheduledDate.Format(_T("%d-%02d-%02d"), dt.GetYear(), dt.GetMonth(), dt.GetDay());
		m_sScheduledDates[ni] = sScheduledDate;
	}

}


int CBookingOfRISView::GetIndexOfScheduledDate(const TCHAR *szScheduledDate)
{
	int ni, nIndex = -1;

	for (ni = 0; ni < m_nMaxBookingDays && (nIndex == -1); ni ++) {
		if (m_sScheduledDates[ni].Compare(szScheduledDate) == 0) 
			nIndex = ni;
	}

	return nIndex;
}


void CBookingOfRISView::LoadQueueGroupsInfo()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	TCHAR *szSystemCode = _T("RIS");
	CString sCmdText;
	SAConnection con;
	SACommand cmd;
	CQueueGroupInfoOfRIS *pQueueGroupInfoOfRIS;
	std::vector <CQueueGroupInfoOfRIS *> ::iterator it;
	CWaitCursor wait;

	for (it = m_lstQueueGroupInfoOfRIS.begin(); it != m_lstQueueGroupInfoOfRIS.end(); it ++)
		delete (*it);

	m_lstQueueGroupInfoOfRIS.clear();
	m_ctlQueueGroups.SetVirtualMode(NULL, 0);
	m_ctlQueueGroups.Populate();
	
	sCmdText.Format(_T("SELECT * FROM  %s WHERE BookEnabled = 1 ORDER BY ExamType, QueueGroupID "),
		pConfig->GetRISQueueGroupsTableName());

	try {
		con.Connect(pConfig->GetXISDBName(szSystemCode), pConfig->GetXISDBUserId(szSystemCode), pConfig->GetXISDBPassword(szSystemCode), pConfig->GetXISDBType(szSystemCode));

		cmd.setConnection(&con);
		cmd.setCommandText(SAString(sCmdText));
		cmd.Execute();

		while (cmd.FetchNext()) {
			pQueueGroupInfoOfRIS = new CQueueGroupInfoOfRIS();
			pQueueGroupInfoOfRIS->ReadFromQuery(cmd);
			m_lstQueueGroupInfoOfRIS.push_back(pQueueGroupInfoOfRIS);
		}

		con.Commit();
	}
	catch(SAException &e) {
		
		try {
			con.Rollback();
		}
		catch(...)
		{
		}

		::MessageBox(GetParentFrame()->m_hWnd, e.ErrText(), _T("错误信息"), MB_OK | MB_ICONEXCLAMATION); 
	}

	m_ctlQueueGroups.SetVirtualMode(new CQueueGroupReportRecordOfRIS(&m_lstQueueGroupInfoOfRIS, m_lstQueueGroupInfoOfRIS.size()), m_lstQueueGroupInfoOfRIS.size());
	m_ctlQueueGroups.Populate();

}


void CBookingOfRISView::LoadBookingRecords()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	TCHAR *szSystemCode = _T("RIS");
	SAConnection con;
	SACommand cmd;
	CString sDateExp(""), sCmdText;
	CString sOldQueueGroupID(""), sQueueGroupID, sScheduledDate, sScheduledTime;
	int nRow, nCapacity_AM[7], nCapacity_PM[7], nIndex = -1, nSelectedIndex = -1;
	BOOL bFound = FALSE;
	std::vector <CQueueGroupBookingInfoOfRIS *> ::iterator itQueueGroupBookingInfo, itFound;
	std::vector <CQueueGroupInfoOfRIS *> ::iterator itQueueGroupInfo;
	CQueueGroupBookingInfoOfRIS *pQueueGroupBookingInfoObject;
	CXTPReportSelectedRows *pSelectedRows;
	CXTPReportRow *pSelectedRow;
	CWaitCursor wait;

	sDateExp.Format(_T("(ScheduledDate >= '%d-%02d-%02d' AND ScheduledDate <= '%d-%02d-%02d' ) "), 
		m_dtFirstDay.GetYear(), m_dtFirstDay.GetMonth(), m_dtFirstDay.GetDay(), 
		m_dtLastDay.GetYear(), m_dtLastDay.GetMonth(), m_dtLastDay.GetDay());

	sCmdText.Format(_T("SELECT QueueGroupID, SCHEDULEDDATE, SCHEDULEDTIME FROM %s \
			WHERE %s AND StudyStatus < 4 ORDER BY QueueGroupID"),
			pConfig->GetRISStudiesTableName(),
			sDateExp);


	pSelectedRows = m_ctlQueueGroupsBookingInfo.GetSelectedRows();
	if (pSelectedRows != NULL) {
		pSelectedRow = pSelectedRows->GetAt(0);
		if (pSelectedRow != NULL) 
		{
			nIndex = pSelectedRow->GetIndex();
			if (nIndex >= 0 && nIndex < m_lstQueueGroupBookingInfoOfRIS.size())
			{
				pQueueGroupBookingInfoObject = (m_lstQueueGroupBookingInfoOfRIS)[nIndex];
				if (pQueueGroupBookingInfoObject != NULL) 
					sOldQueueGroupID = pQueueGroupBookingInfoObject->m_sQueueGroupID;
			}
		}
	}


	for (itQueueGroupBookingInfo = m_lstQueueGroupBookingInfoOfRIS.begin(); itQueueGroupBookingInfo != m_lstQueueGroupBookingInfoOfRIS.end(); itQueueGroupBookingInfo ++)
		delete (*itQueueGroupBookingInfo);

	m_lstQueueGroupBookingInfoOfRIS.clear();
	m_ctlQueueGroupsBookingInfo.ResetContent();

	for (itQueueGroupInfo = m_lstQueueGroupInfoOfRIS.begin(); itQueueGroupInfo != m_lstQueueGroupInfoOfRIS.end(); itQueueGroupInfo ++) {

		nCapacity_AM[0] = (*itQueueGroupInfo)->m_nCapacityD1_AM;
		nCapacity_AM[1] = (*itQueueGroupInfo)->m_nCapacityD2_AM;
		nCapacity_AM[2] = (*itQueueGroupInfo)->m_nCapacityD3_AM;
		nCapacity_AM[3] = (*itQueueGroupInfo)->m_nCapacityD4_AM;
		nCapacity_AM[4] = (*itQueueGroupInfo)->m_nCapacityD5_AM;
		nCapacity_AM[5] = (*itQueueGroupInfo)->m_nCapacityD6_AM;
		nCapacity_AM[6] = (*itQueueGroupInfo)->m_nCapacityD7_AM;
	
		nCapacity_PM[0] = (*itQueueGroupInfo)->m_nCapacityD1_PM;
		nCapacity_PM[1] = (*itQueueGroupInfo)->m_nCapacityD2_PM;
		nCapacity_PM[2] = (*itQueueGroupInfo)->m_nCapacityD3_PM;
		nCapacity_PM[3] = (*itQueueGroupInfo)->m_nCapacityD4_PM;
		nCapacity_PM[4] = (*itQueueGroupInfo)->m_nCapacityD5_PM;
		nCapacity_PM[5] = (*itQueueGroupInfo)->m_nCapacityD6_PM;
		nCapacity_PM[6] = (*itQueueGroupInfo)->m_nCapacityD7_PM;

		pQueueGroupBookingInfoObject = new CQueueGroupBookingInfoOfRIS((*itQueueGroupInfo)->m_sQueueGroupID, 
			(*itQueueGroupInfo)->m_sQueueGroupName,
			(*itQueueGroupInfo)->m_sExamType,
			(*itQueueGroupInfo)->m_sLocation,
			(*itQueueGroupInfo)->m_sMemo,
			nCapacity_AM, 
			nCapacity_PM,
			m_dtFirstDay);

		if (!sOldQueueGroupID.IsEmpty() && (*itQueueGroupInfo)->m_sQueueGroupID.Compare(sOldQueueGroupID) == 0)
			nSelectedIndex = m_lstQueueGroupBookingInfoOfRIS.size();

		m_lstQueueGroupBookingInfoOfRIS.push_back(pQueueGroupBookingInfoObject);
	}

	try {
		con.Connect(pConfig->GetXISDBName(szSystemCode), pConfig->GetXISDBUserId(szSystemCode), pConfig->GetXISDBPassword(szSystemCode), pConfig->GetXISDBType(szSystemCode));

		cmd.setConnection(&con);
		cmd.setCommandText(SAString(sCmdText));
		cmd.Execute();

		while (cmd.FetchNext()) {
			
			sQueueGroupID = cmd.Field("QueueGroupID").asString();
			sScheduledDate = cmd.Field("ScheduledDate").asString();
			sScheduledTime = cmd.Field("ScheduledTime").asString();
			sScheduledTime = sScheduledTime.Trim();
			bFound = FALSE;
			for (itQueueGroupBookingInfo = m_lstQueueGroupBookingInfoOfRIS.begin(); itQueueGroupBookingInfo != m_lstQueueGroupBookingInfoOfRIS.end() && !bFound; itQueueGroupBookingInfo ++) {
				if (sQueueGroupID.Compare((*itQueueGroupBookingInfo)->m_sQueueGroupID) == 0) {
					bFound = TRUE;
					itFound = itQueueGroupBookingInfo;
				}
			}

			if (bFound) {
				nIndex = GetIndexOfScheduledDate(sScheduledDate);
				if (nIndex >= 0) {

					if (sScheduledTime.CompareNoCase(_T("下午")) == 0 || sScheduledTime.CompareNoCase(_T("PM")) == 0 || sScheduledTime.Compare(_T("11:59")) > 0)
						(*itFound)->m_nBookingAmount_PM[nIndex] ++;
					else
						(*itFound)->m_nBookingAmount_AM[nIndex] ++;
				}
			}
		}

		con.Commit();
	}
	catch(SAException &e) {
		
		try {
			con.Rollback();
		}
		catch(...)
		{
		}

		::MessageBox(GetParentFrame()->m_hWnd, e.ErrText(), _T("错误信息"), MB_OK | MB_ICONEXCLAMATION); 
	}
	
	m_ctlQueueGroupsBookingInfo.ResetContent();

	for (nRow = 0; nRow < m_lstQueueGroupBookingInfoOfRIS.size(); nRow ++) {
		m_ctlQueueGroupsBookingInfo.AddRecord(new CQueueGroupBookingReportRecordOfRIS(&m_lstQueueGroupBookingInfoOfRIS, m_lstQueueGroupBookingInfoOfRIS.size(), m_nMaxBookingDays, &m_sExamTypeOfMedicalOrder));
	}
	
	m_ctlQueueGroupsBookingInfo.Populate();

	if (m_lstQueueGroupBookingInfoOfRIS.size() > 0) {
		if (nSelectedIndex == -1)
			nSelectedIndex = 0;

		m_ctlQueueGroupsBookingInfo.GetSelectedRows()->Select(m_ctlQueueGroupsBookingInfo.GetRows()->GetAt(nSelectedIndex));

		pSelectedRow = m_ctlQueueGroupsBookingInfo.GetSelectedRows()->GetAt(0);
		pSelectedRow->EnsureVisible();
		m_ctlQueueGroupsBookingInfo.SetFocusedRow(pSelectedRow);
	}
}

void CBookingOfRISView::NewBooking()
{
	CBookingOfRISDoc *pDoc = GetDocument();
	CNewBookingDlg dlg;
	CString sInputValue("");

	if (dlg.DoModal() != IDOK)
		return;

	sInputValue = dlg.GetInputValue();
	sInputValue = sInputValue.Trim();
	if (sInputValue.IsEmpty())
		return;

	if (!pDoc->OpenOrder(sInputValue))
		return;


	pDoc->UpdateAllViews(NULL);
}


void CBookingOfRISView::OnUpdateActions(CCmdUI *pCmdUI)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	BOOL bEnabled = FALSE;
	switch(pCmdUI->m_nID) {
	case ID_BOOKINGRIS_NEW:
		bEnabled = TRUE;
	case ID_BOOKINGRIS_QUERY:
		bEnabled = TRUE;
		break;
	case ID_BOOKINGRIS_RETURN:
		bEnabled = pApp->IsHISProgramIsRunning();
		break;
	case ID_CHILDFRAME_CLOSE:
		bEnabled = TRUE;
		break;
	}

	pCmdUI->Enable(bEnabled);
}


void CBookingOfRISView::OnExecuteActions(UINT nCommandID)
{
	CString sExamType(""), sLocation("");
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	switch(nCommandID) {
	case ID_BOOKINGRIS_NEW:
		NewBooking();
		break;
	case ID_BOOKINGRIS_QUERY:
		LoadBookingRecords();
		break;
	case ID_BOOKINGRIS_RETURN:
		pApp->ReturnToHISDesktop();
		break;
	case ID_CHILDFRAME_CLOSE:
		GetParent()->SendMessage(WM_CLOSE);
		break;
	}
}


