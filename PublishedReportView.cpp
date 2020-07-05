// PublishedReportView.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "PublishedReportView.h"
#include "SQLAPI.h"
#include "resource.h"
#include <vector>
#include <algorithm>



extern BOOL IsImageViewerAvailable();
extern BOOL IsWebViewerAvailable();
extern BOOL IsDicomRetrieveAvailable();


typedef struct SystemInfoStruct
{
	const TCHAR *lpszSystemCode1;
	const TCHAR *lpszSystemCode2;
	const TCHAR *lpszSystemName;
};

SystemInfoStruct g_SystemInfo[] = {
	{_T("RIS"), _T("CLOUDRIS"), _T("放射影像检查") },
	{_T("UIS"),	_T("CLOUDUIS"), _T("超声影像检查") },
	{_T("EIS"),	_T("CLOUDEIS"), _T("内镜影像检查") },
	{_T("PIS"), _T("CLOUDPIS"), _T("病理影像检查") },
	{_T("PET"), _T("PET"),		_T("PET-CT影像检查")},
	{_T("ALL"),	_T("ALL"),		_T("任意影像检查") },
	{_T(""), _T(""), _T("")}
};


typedef struct SearchFieldInfoStruct
{
	const TCHAR *lpszFieldName;
	const TCHAR *lpszFieldLabel;
	bool	bWithReportDate;
	bool	bMustHaveReportDate;
};

SearchFieldInfoStruct g_SearchFieldInfo[] = {
	{_T("RefDoctorName"),	_T("送检医生姓名"),	true,	true},
	{_T("RefDeptName"),		_T("送检科室名称"),	true,	true},
	{_T(" "),				_T("(无)"),			true,	true},
	{_T("InpatientNo"),		_T("住院号"),		true,	true},
	{_T("DocID"),			_T("影像编号"),		false,	false},
	{_T("PatientID"),		_T("患者编号"),		true,	true},
	{_T("PatientName"),		_T("患者姓名"),		true,	true},
	{_T("OrderNo"),			_T("申请单号"),		false,	false},
	{_T(""), _T(""), false,	false},
};


class CQueryParams
{
public:
	CQueryParams(const TCHAR *szSystemCode, const TCHAR *szCommandText) { m_sSystemCode = szSystemCode; m_sCommandText = szCommandText; }
	~CQueryParams() { };

	CString m_sSystemCode;
	CString m_sCommandText;
};


class CMedicalResultRecord : public CXTPReportRecord
{
private:
	std::vector <CMedicalResultObject *> *m_pMedicalResultObjectList;
	int	m_nMedicalResultObjectListSize;
public:
	CMedicalResultRecord(std::vector <CMedicalResultObject *> *pMedicalResultObjectList, int nMedicalResultObjectListSize)
	{
		//日期、姓名、性别、年龄、检查项目、检查方法、住院号、床号、门诊号、体检号、影像号、报告医生、检查时间、。。。

		AddItem(new CXTPReportRecordItemText());	// 0	序号
		AddItem(new CXTPReportRecordItemText());	// 1	检查日期
		AddItem(new CXTPReportRecordItemText());	// 2	患者姓名
		AddItem(new CXTPReportRecordItemText());	// 3	性别
		AddItem(new CXTPReportRecordItemText());	// 4	年龄
		AddItem(new CXTPReportRecordItemText());	// 5	检查项目
		AddItem(new CXTPReportRecordItemText());	// 6	送检科室
		AddItem(new CXTPReportRecordItemText());	// 7	住院号
		AddItem(new CXTPReportRecordItemText());	// 8	床号
		AddItem(new CXTPReportRecordItemText());	// 9	门诊号
		AddItem(new CXTPReportRecordItemText());	//10	体检号
		AddItem(new CXTPReportRecordItemText());	//11	影像编号
		AddItem(new CXTPReportRecordItemText());	//12	检查时间
		AddItem(new CXTPReportRecordItemText());	//13	患者来源
		AddItem(new CXTPReportRecordItemText());	//14	检查类型
		AddItem(new CXTPReportRecordItemText());	//15	检查方法	
		AddItem(new CXTPReportRecordItemText());	//16	送检医师
		AddItem(new CXTPReportRecordItemText());	//17	申请单号
		AddItem(new CXTPReportRecordItemText());	//18	病区
		AddItem(new CXTPReportRecordItemText());	//19	医嘱号
		AddItem(new CXTPReportRecordItemText());	//20	存取号
		AddItem(new CXTPReportRecordItemText());	//21	报告日期
		AddItem(new CXTPReportRecordItemText());	//22	报告时间
		AddItem(new CXTPReportRecordItemText());	//23	报告医师
		AddItem(new CXTPReportRecordItemText());	//24	审核日期
		AddItem(new CXTPReportRecordItemText());	//25	审核时间
		AddItem(new CXTPReportRecordItemText());	//26	审核医师
		AddItem(new CXTPReportRecordItemText());	//27	危急值标记
		AddItem(new CXTPReportRecordItemText());	//28	阳性/阴性
		AddItem(new CXTPReportRecordItemText());	//29	临床路径标记
		AddItem(new CXTPReportRecordItemText());	//30	提交日期
		AddItem(new CXTPReportRecordItemText());	//31	提交时间
		AddItem(new CXTPReportRecordItemText());	//32	版本号
		AddItem(new CXTPReportRecordItemText());	//33	传染性疾病
		AddItem(new CXTPReportRecordItemText());	//34	建议补增强

	

		m_pMedicalResultObjectList = pMedicalResultObjectList;
		m_nMedicalResultObjectListSize = nMedicalResultObjectListSize;
	}

	void GetItemMetrics (XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics)
	{
		int nRow,nCol;
		CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
		CMedicalResultObject *pObj;
		CXTPReportColumnOrder* pSortOrder = pDrawArgs->pControl->GetColumns()->GetSortOrder();
		CString sText(""), sDateFrom(""), sDateTo("");

		CXTPReportPaintManager* pPaintManager = pDrawArgs->pControl->GetPaintManager();

		pItemMetrics->pFont = &pPaintManager->m_fontText;    
		pItemMetrics->clrForeground = pPaintManager->m_clrWindowText;    
		pItemMetrics->clrBackground = XTP_REPORT_COLOR_DEFAULT;    
		pItemMetrics->nColumnAlignment = pDrawArgs->nTextAlign;    
		pItemMetrics->nItemIcon = XTP_REPORT_NOICON;  

		nCol = pDrawArgs->pColumn->GetIndex();
		nRow = pDrawArgs->pRow->GetIndex();

		if (nRow >= 0 &&  nRow < (int) m_pMedicalResultObjectList->size()) 
		{

			// 0	序号
			// 1	检查日期
			// 2	患者姓名
			// 3	性别
			// 4	年龄
			// 5	检查项目
			// 6	送检科室
			// 7	住院号
			// 8	床号
			// 9	门诊号
			//10	体检号
			//11	影像编号
			//12	检查时间
			//13	患者来源
			//14	检查类型
			//15	检查方法	
			//16	送检医师
			//17	申请单号
			//18	病区
			//19	医嘱号
			//20	存取号
			//21	报告日期
			//22	报告时间
			//23	报告医师
			//24	审核日期
			//25	审核时间
			//26	审核医师
			//27	危急值标记
			//28	阳性/阴性
			//29	临床路径标记
			//30	提交日期
			//31	提交时间
			//32	版本号
			//33	传染性疾病
			//34	建议补增强

			pObj = (*m_pMedicalResultObjectList)[nRow];
			switch(nCol) 
			{
			case 0:		// 序号
				sText.Format(_T("%4d"), nRow + 1);
				pItemMetrics->strText = sText;
				break;
			case 1:		// 检查日期
				pItemMetrics->strText = pObj->m_sStudyDate;
				break;

			case 2:		// 患者姓名
				pItemMetrics->strText = pObj->m_sPatientName;
				break;
			case 3:		// 性别
				pItemMetrics->strText = pObj->m_sPatientSex;
				break;	

			case 4:		// 年龄
				pItemMetrics->strText = pObj->m_sPatientAge;
				break;
			case 5:		// 检查项目
				pItemMetrics->strText = pObj->m_sStudyDetails;
				break;

			case 6:		//  送检科室 
				pItemMetrics->strText = pObj->m_sRefDeptName;
				break;
			case 7:	// 住院号 
				pItemMetrics->strText = pObj->m_sInpatientNo;
				break;

			case 8:	// 床号 
				pItemMetrics->strText = pObj->m_sBedNo;
				break;

			case 9:	// 门诊号 
				pItemMetrics->strText = pObj->m_sOutpatientNo;
				break;

			case 10:	// 体检号 
				pItemMetrics->strText = pObj->m_sCheckupNo;
				break;

			case 11:	// 影像编号 
				pItemMetrics->strText = pObj->m_sDocID;
				break;

			case 12:	// 报告医师 
				pItemMetrics->strText = pObj->m_sReportDoctorName;
				break;

			case 13:		// 检查时间
				pItemMetrics->strText = pObj->m_sStudyTime;
				break;

			case 14:		// 患者来源			
				pItemMetrics->strText = pObj->m_sOrderType;
				break;
			case 15: // 检查类型
				if (pObj->m_sSystemCode.CompareNoCase(_T("UIS")) == 0)
					sText = "超声";
				else if (pObj->m_sSystemCode.CompareNoCase(_T("EIS")) == 0)
					sText = "内镜";
				else if (pObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0)
					sText = "PET-CT";
				else if (pObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0) {
					if (pObj->m_sExamType.IsEmpty())
						sText = "放射";
					else 
						sText.Format(_T("放射_%s"), pObj->m_sExamType);

				}
				else if (pObj->m_sSystemCode.CompareNoCase(_T("PIS")) == 0)
					sText = "病理";
				else
					sText = "";

				pItemMetrics->strText = sText;
				break;

			case 16:	// 检查方法 
				pItemMetrics->strText = pObj->m_sExamMethods;
				break;
			case 17:	// 送检医师 
				pItemMetrics->strText = pObj->m_sRefDoctorName;
				break;
			case 18:	// 申请单号 
				pItemMetrics->strText = pObj->m_sOrderNo;
				break;
			case 19:	// 病区 
				pItemMetrics->strText = pObj->m_sWard;
				break;
			case 20:	// 医嘱号 
				pItemMetrics->strText = pObj->m_sMANo;
				break;
			case 21:	// 存取号 
				pItemMetrics->strText = pObj->m_sAccessionNo;
				break;
			case 22:	// 报告日期 
				pItemMetrics->strText = pObj->m_sReportDate;
				break;
			case 23:	// 报告时间 
				pItemMetrics->strText = pObj->m_sReprotTime;
				break;
			case 24:	// 审核日期 
				pItemMetrics->strText = pObj->m_sReviewDate;
				break;
			case 25:	// 审核时间 
				pItemMetrics->strText = pObj->m_sReviewTime;
				break;
			case 26:	// 审核医师 
				pItemMetrics->strText = pObj->m_sReviewDoctorName;
				break;
			case 27:	// 危急值标记 
				pItemMetrics->strText = pObj->m_nCriticalFlag > 0 ? _T("X") : _T(" ");
				break;
			case 28:	// 阳性/阴性 
				if (pObj->m_sSystemCode.CompareNoCase(_T("EIS")) == 0) {
					if (pApp->PositiveFlagValidForEIS())
						pItemMetrics->strText = pObj->m_nPositiveFlag > 0 ? _T("+") : _T(" ");
					else
						pItemMetrics->strText = _T(" ");
				}
				else {
					pItemMetrics->strText = pObj->m_nPositiveFlag > 0 ? _T("+") : _T(" ");
				}

				break;
			case 29:	// 临床路径标记 
				pItemMetrics->strText = pObj->m_nClinicPathFlag > 0 ? _T("X") : _T(" ");
				break;
			case 30:	// 提交日期 
				pItemMetrics->strText = pObj->m_sSubmitDate;
				break;
			case 31:	// 提交时间 
				pItemMetrics->strText = pObj->m_sSubmitTime;
				break;
			case 32:	// 版本号 
				if (pObj->m_nVersionNo > 0)
					sText.Format(_T("%d"), pObj->m_nVersionNo);
				else
					sText = _T("");
				pItemMetrics->strText = sText;
				break;
			case 33:	// 传染性疾病 
				pItemMetrics->strText = pObj->m_nInfectiousFlag > 0 ? _T("是") : _T(" ");
				break;
			case 34:	// 建议补增强 
				pItemMetrics->strText = pObj->m_nEnhancedFlag > 0 ? _T("是") : _T(" ");
				break;
			}

			if (!pObj->m_bFinalReport) 
				pItemMetrics->clrForeground = RGB(0,0, 255);
		}
	}
};




// CPublishedReportView

IMPLEMENT_DYNCREATE(CPublishedReportView, CXTPResizeFormView)

CPublishedReportView::CPublishedReportView()
	: CXTPResizeFormView(CPublishedReportView::IDD)
{
	m_bDataLoading	= FALSE;
	m_bInitialized	= FALSE;

	m_bReportColumnWidthChanged = FALSE;

	m_sStudyGUIDSaved = _T("");

	m_sFieldsOfUIS	= _T("*");
	m_sFieldsOfPIS	= _T("*");
	m_sFieldsOfEIS	= _T("*");

	m_sFieldsOfRIS	= _T("StudyGUID, StudyUID, RegisteredDate, ExamType, ExamMethods, DocID, PatientName, PatientNamePy, PatientDOB, PatientSex, PatientAge, PatientAgeUnit, MRN, ServiceGUIDs, ServiceNames, ScheduledDate, ScheduledTime, \
		StudyDate, StudyTime, Location, AccessionNo, OrderNo, MRN,OrderDetails, ClinicDiag, ContactInfo, Fee, SendFlag, SendTimes, OrderType, InpatientNo, Ward, BedNo, OutpatientNo, BodyCheckupNo, RefDept, RefDoctor, \
		StudyStatus, JuniorDoctorEmpNo, JuniorDoctorName, JuniorReport, ReportDate, ReportTime, ReportDoctorEmpNo, ReportDoctorName,ReportDoctorSL,DeviceAET, DeviceName, \
		ReviewDate, ReviewTime, ReviewDoctorEmpNo, ReviewDoctorName, ReviewDoctorSL, CreatorName, TechnicianName, StudyID, RegisteredTime, Findings, Conclusions, PositiveFlag, InfectiousFlag, CriticalFlag, EnhancedFlag, WarnContent,SendFlag, SendTimes ");


	m_sFieldsOfPET	= _T("StudyGUID, StudyUID, RegisteredDate, ExamType, ExamMethods, DocID, PatientName, PatientNamePy, PatientDOB, PatientSex, PatientAge, PatientAgeUnit, MRN, ServiceGUIDs, ServiceNames, ScheduledDate, ScheduledTime, \
		StudyDate, StudyTime, Location, AccessionNo, OrderNo, MRN,OrderDetails, ClinicDiag, ContactInfo, Fee, SendFlag, SendTimes, OrderType, InpatientNo, Ward, BedNo, OutpatientNo, BodyCheckupNo, RefDept, RefDoctor, \
		StudyStatus, JuniorDoctorEmpNo, JuniorDoctorName, JuniorReport, ReportDate, ReportTime, ReportDoctorEmpNo, ReportDoctorName,ReportDoctorSL,DeviceAET, DeviceName, \
		ReviewDate, ReviewTime, ReviewDoctorEmpNo, ReviewDoctorName, ReviewDoctorSL, CreatorName, TechnicianName, StudyID, RegisteredTime, Findings, Conclusions, PositiveFlag, InfectiousFlag, CriticalFlag, EnhancedFlag, SendFlag, SendTimes ");
	
	//新PIS
	m_sFieldsOfPIS_1 = _T("CASENO,BBJSRQ,BGSJ,SHSJ,BLH,HISID,PATIENTNAME,SEX,NL,PATIENTTYPE,MZH,ZYH,TJH,BEDNO,REQDEPT,REQDOCT,BBMC,BGYS,SHYS,DTSJ,JXSJ,BLZD,SHFLAG,BRID,WJZ");

	//新EIS
	/*
	m_sFieldsOfEIS	= _T("StudyGUID, StudyUID, RegisteredDate, ExamType, ExamMethods, DocID, PatientName, PatientNamePy, PatientDOB, PatientSex, PatientAge, PatientAgeUnit, MRN, ServiceGUIDs, ServiceNames, ScheduledDate, ScheduledTime, \
		StudyDate, StudyTime, Location, AccessionNo, OrderNo, MRN,OrderDetails, ClinicDiag, ContactInfo, Fee, SendFlag, SendTimes, OrderType, InpatientNo, Ward, BedNo, OutpatientNo, BodyCheckupNo, RefDept, RefDoctor, \
		StudyStatus, JuniorDoctorEmpNo, JuniorDoctorName, JuniorReport, ReportDate, ReportTime, ReportDoctorEmpNo, ReportDoctorName,ReportDoctorSL, DeviceName, \
		ReviewDate, ReviewTime, ReviewDoctorEmpNo, ReviewDoctorName, ReviewDoctorSL, CreatorName, TechnicianName, StudyID, RegisteredTime, Findings, Conclusions, PositiveFlag, CriticalFlag, SendFlag, SendTimes ");
	*/
}

CPublishedReportView::~CPublishedReportView()
{
}

void CPublishedReportView::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeFormView::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_CTL_REPORTDATEFROM, m_dtpReportDateFrom);
	DDX_Control(pDX, IDC_CTL_REPORTDATETO,	m_dtpReportDateTo);

	DDX_Control(pDX, IDC_CBO_EXAMTYPE, m_cboExamType);
	DDX_Control(pDX, IDC_CBO_FIELDNAME, m_cboFieldName);
	DDX_Control(pDX, IDC_EDT_FIELDVALUE, m_edtFieldValue);

	DDX_Control(pDX, IDC_GRD_RESULT, m_ctlResult);

}

BEGIN_MESSAGE_MAP(CPublishedReportView, CXTPResizeFormView)
	ON_UPDATE_COMMAND_UI_RANGE(1000, 65535, OnUpdateActions)
	ON_COMMAND_RANGE(1000, 65535, OnExecuteActions)

	ON_NOTIFY(XTP_NM_REPORT_SELCHANGED, IDC_GRD_RESULT, OnReportSelChanged)   
	ON_NOTIFY(XTP_NM_REPORT_PRESORTORDERCHANGED, IDC_GRD_RESULT, OnReportPresortOrderChanged)
	ON_NOTIFY(XTP_NM_REPORT_SORTORDERCHANGED, IDC_GRD_RESULT, OnReportSortOrderChanged)
	ON_NOTIFY(XTP_NM_REPORT_COLUMNWIDTHCHANGED, IDC_GRD_RESULT, OnReportColumnWidthChanged)

	ON_NOTIFY(NM_DBLCLK, IDC_GRD_RESULT, OnReportItemDblClick)

	ON_NOTIFY(NM_RCLICK, IDC_GRD_RESULT, OnReportItemRClick)
	ON_CBN_SELCHANGE(IDC_CBO_FIELDNAME, OnCbnSelchangeCboFieldname)
	ON_WM_INITMENUPOPUP()

	ON_LBN_XTP_ONBROWSE(IDC_EDT_FIELDVALUE, OnFieldValueBrowse)

	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPublishedReportView diagnostics

#ifdef _DEBUG
void CPublishedReportView::AssertValid() const
{
	CXTPResizeFormView::AssertValid();
}

CPublishedReportDoc* CPublishedReportView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPublishedReportDoc)));
	return (CPublishedReportDoc*)m_pDocument;
}


#ifndef _WIN32_WCE
void CPublishedReportView::Dump(CDumpContext& dc) const
{
	CXTPResizeFormView::Dump(dc);
}
#endif
#endif //_DEBUG


BOOL CPublishedReportView::PreTranslateMessage(MSG* pMsg) 
{
	//  如果在检索项内容输入框中按回车键，　转送查询的消息
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN && m_edtFieldValue.GetSafeHwnd() == pMsg->hwnd) {
		pMsg->message = WM_COMMAND;
		pMsg->wParam = ID_MEDICALRESULT_FIND;
		pMsg->hwnd = m_hWnd;
	}

	return CXTPResizeFormView::PreTranslateMessage(pMsg);
}


void CPublishedReportView::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
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


void CPublishedReportView::OnFieldValueBrowse()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	int nItemIndex, nTag;
	UINT nCommandIDFrom = 1;
	SearchFieldInfoStruct * pFieldInfo = NULL;
	CString sFieldName("");
	CPoint point;
	CMenu menu;
	CString sItem("");
	DWORD dwSelect;

	nItemIndex = m_cboFieldName.GetCurSel();
	pFieldInfo = (SearchFieldInfoStruct *) m_cboFieldName.GetItemData(nItemIndex);		
		
	sFieldName = pFieldInfo->lpszFieldName;
	if (sFieldName.CompareNoCase(_T("RefDeptName")) == 0)
		nTag = 1;
	else if (sFieldName.CompareNoCase(_T("RefDoctorName")) == 0)
		nTag = 2;
	else if (sFieldName.CompareNoCase(_T("PatientName")) == 0)
		nTag = 3;
	else if (sFieldName.CompareNoCase(_T("InpatientNo")) == 0)
		nTag = 4;
	else if (sFieldName.CompareNoCase(_T("DocID")) == 0)
		nTag = 5;
	else
		return;

	::GetCursorPos(&point);

	menu.CreatePopupMenu();

	pApp->MakeMenuOfRecentRefDeptOrRefDoctorList(nTag, nCommandIDFrom, &menu);

	dwSelect = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, 
      point.y, this);

	if (dwSelect > 0) {
		sItem = _T("");
		menu.GetMenuString(dwSelect, sItem, MF_BYCOMMAND);
		if (!sItem.IsEmpty())
			m_edtFieldValue.SetWindowText(sItem);
	}

	menu.DestroyMenu();
}

// CPublishedReportView message handlers
void CPublishedReportView::OnReportSelChanged(NMHDR * pNotifyStruct, LRESULT *result)
{
	std::vector <CMedicalResultObject *> *pMedicalResultObjectList = GetDocument()->GetMedicalResultObjectList();
	int nMedicalResultObjectListSize = GetDocument()->GetMedicalResultObjectListSize();
	CMedicalResultObject *pCurrentMedicalResultObject = NULL;
	XTP_NM_REPORTRECORDITEM *pSelNotify = (XTP_NM_REPORTRECORDITEM *) pNotifyStruct;
	CXTPReportSelectedRows *pSelectedRows;
	CXTPReportRow *pSelectedRow;
	int nIndex;

	if (m_bDataLoading || !m_bInitialized)
		return;

	pSelectedRows = m_ctlResult.GetSelectedRows();
	if (pSelectedRows != NULL) 
	{
		pSelectedRow = pSelectedRows->GetAt(0);

		if (pSelectedRow != NULL) 
		{
			nIndex = pSelectedRow->GetIndex();

			if (nIndex >= 0 && nIndex < nMedicalResultObjectListSize)
			{
				pCurrentMedicalResultObject = (*pMedicalResultObjectList)[nIndex];
				GetDocument()->SetCurrentMedicalResultObject(pCurrentMedicalResultObject);
				GetDocument()->UpdateAllViews(NULL);
			}
		}
	}
}


void CPublishedReportView::OnReportColumnWidthChanged(NMHDR *pNotifyStruct, LRESULT *result)
{
	m_bReportColumnWidthChanged = TRUE;
}


void CPublishedReportView::OnReportPresortOrderChanged(NMHDR * pNotifyStruct, LRESULT *result)
{
	std::vector <CMedicalResultObject *> *pObjList = GetDocument()->GetMedicalResultObjectList();
	int ni, nIndex;
	CString sColumnName("");
	CXTPReportColumn *pColumn;
	CXTPReportRow *pSelectedRow;
	XTP_NM_REPORTRECORDITEM *pItemNotify = (XTP_NM_REPORTRECORDITEM*)pNotifyStruct;
	ASSERT(pItemNotify != NULL);
			
	CXTPReportColumnOrder *pSortOrder = m_ctlResult.GetColumns()->GetSortOrder();

	m_sStudyGUIDSaved = _T("");
	if (m_ctlResult.GetSelectedRows()) {
		pSelectedRow = m_ctlResult.GetSelectedRows()->GetAt(0);
		if (pSelectedRow) {				
			nIndex = pSelectedRow->GetIndex();

			if (nIndex >= 0 && nIndex < pObjList->size()) {
				m_sStudyGUIDSaved = (*pObjList)[nIndex]->m_sStudyGUID;
			}
		}
	}

	for (ni = 0; ni < pSortOrder->GetCount(); ni ++) {
		pColumn = pSortOrder->GetAt(ni);
		sColumnName = pColumn->GetCaption();
		if (sColumnName == _T("检查项目"))
			std::stable_sort((*pObjList).begin(), (*pObjList).end(), pColumn->IsSortedIncreasing() ? CMedicalResultObject::SortByStudyDetail_Asc : CMedicalResultObject::SortByStudyDetail_Desc);
		else if (sColumnName == _T("患者姓名")) 
			std::stable_sort((*pObjList).begin(), (*pObjList).end(), pColumn->IsSortedIncreasing() ? CMedicalResultObject::SortByPatientName_Asc : CMedicalResultObject::SortByPatientName_Desc);
		else if (sColumnName == _T("检查方法"))
			std::stable_sort((*pObjList).begin(), (*pObjList).end(), pColumn->IsSortedIncreasing() ? CMedicalResultObject::SortByExamMethod_Asc : CMedicalResultObject::SortByExamMethod_Desc);
	}

}



void CPublishedReportView::OnReportSortOrderChanged(NMHDR * pNotifyStruct, LRESULT *result)
{
	int nIndex = -1;
	CXTPReportRow *pReportRow;

	if (!m_sStudyGUIDSaved.IsEmpty()) {
		nIndex = GetDocument()->GetIndexByStudyGUID(m_sStudyGUIDSaved);

		if (nIndex >= 0 && nIndex < m_ctlResult.GetRows()->GetCount()) {
			
			pReportRow = m_ctlResult.GetRows()->GetAt(nIndex);
			m_ctlResult.SetFocusedRow(pReportRow);
		}
	}
}



void CPublishedReportView::OnReportItemDblClick(NMHDR * pNotifyStruct, LRESULT * /*result*/)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CXTPReportRow *pReportRow;
	POINT pt;
	CRect rect;
	CXTPReportHitTestInfo info;
	CMedicalResultObject *pObj;

	if (m_ctlResult.GetSelectedRows() != NULL) {
		if (m_ctlResult.GetSelectedRows()->GetCount() >= 1) {

			pReportRow = m_ctlResult.GetSelectedRows()->GetAt(0);
			
			::GetCursorPos(&pt);
			::ScreenToClient(m_ctlResult.m_hWnd, &pt);

			m_ctlResult.HitTest(pt, &info);
			if (info.m_htCode == xtpReportHitTestHeader) {

			}
			else  if (info.m_htCode == xtpReportHitTestBodyRows) {
				pObj = GetDocument()->GetCurrentMedicalResultObject();
				if (pObj != NULL && pObj->m_sSystemCode.CompareNoCase(_T("PIS")) != 0)
					ViewImageMode1(pApp->GetDefaultCMoveMode());
							
			}
		}
	}
}


void CPublishedReportView::OnReportItemRClick(NMHDR * pNotifyStruct, LRESULT * /*result*/)
{
	CPoint ptCurrent;
	CMenu popupMenu, *pContextMenu;

	if (m_ctlResult.GetSelectedRows() != NULL) {
		if (m_ctlResult.GetSelectedRows()->GetCount() == 1)  {

			::GetCursorPos(&ptCurrent);
			popupMenu.LoadMenu(IDR_VIEWIMAGE);
			pContextMenu = popupMenu.GetSubMenu(0);
			pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, ptCurrent.x, ptCurrent.y, this);
		}
	}
}


void CPublishedReportView::OnCbnSelchangeCboFieldname()
{
	int nItemIndex = -1;
	SearchFieldInfoStruct *pFieldInfo;
	CString sFieldName(""), sLockedRefFromDept("");
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	if (m_cboFieldName.GetSafeHwnd() == NULL)
		return;

	nItemIndex = m_cboFieldName.GetCurSel();

	if (nItemIndex >= 0) {
		
		pFieldInfo = (SearchFieldInfoStruct *) m_cboFieldName.GetItemDataPtr(nItemIndex);

		if (pFieldInfo && pFieldInfo->bWithReportDate)  {
			if (pFieldInfo->bMustHaveReportDate) {
				m_dtpReportDateFrom.EnableWindow(TRUE);
				m_dtpReportDateTo.EnableWindow(TRUE);
			}
			else {
				m_dtpReportDateFrom.EnableWindow(!pApp->IsIgnoreStudyDateWhileSearchingByIdentity());
				m_dtpReportDateTo.EnableWindow(!pApp->IsIgnoreStudyDateWhileSearchingByIdentity());
			}
		}
		else {
			m_dtpReportDateFrom.EnableWindow(FALSE);
			m_dtpReportDateTo.EnableWindow(FALSE);
		}

		if (pFieldInfo && (_tcslen(pFieldInfo->lpszFieldName) > 1)) {
			m_edtFieldValue.EnableWindow(TRUE);
			m_edtFieldValue.SetWindowText(_T(""));
			sFieldName = pFieldInfo->lpszFieldName;
			if (sFieldName.CompareNoCase(_T("RefDeptName")) == 0) {
				sLockedRefFromDept = pApp->GetLockedRefFromDept();
				if (!sLockedRefFromDept.IsEmpty()) {
					m_edtFieldValue.SetWindowText(sLockedRefFromDept);
					m_edtFieldValue.EnableWindow(FALSE);
				}
				else 
					m_edtFieldValue.SetFocus();
			}
			else 
				m_edtFieldValue.SetFocus();

		}
		else {
			m_edtFieldValue.EnableWindow(FALSE);
		}
	}
}


void CPublishedReportView::OnInitialUpdate()
{
	int ni, nIndex;
	COleDateTime dtFrom, dtTo;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	std::vector <CMedicalResultObject *> *pMedicalResultList = GetDocument()->GetMedicalResultObjectList();
	int nMedicalResultListSize = GetDocument()->GetMedicalResultObjectListSize();
	CSystemConfiguration *pConfig = ((CCloudERVApp *) AfxGetApp())->GetSystemConfiguration();
	CButton *pBtnFromHCS, *pBtnFromXIS;
	const TCHAR *szSectionName1 = _T("Settings_GUI");
	const TCHAR *szKeyName1 = _T("HeaderWidthOfReportResultControl");
	CString sPatientNameLabel(""), sFieldName(""), sLockedRefFromDept("");


	CXTPResizeFormView::OnInitialUpdate();

	sPatientNameLabel = pApp->GetPatientNameLabel();

	sLockedRefFromDept = pApp->GetLockedRefFromDept();

	m_edtFieldValue.Initialize(this, BES_XTP_BROWSE);
	m_dtpReportDateFrom.SetFormat(_T("yyyy-MM-dd"));
	m_dtpReportDateTo.SetFormat(_T("yyyy-MM-dd"));

	SetResize(IDC_LBL_REPORTDATEFROM, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_CTL_REPORTDATEFROM, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_REPORTDATETO, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_CTL_REPORTDATETO, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_NOTE, SZ_TOP_LEFT, SZ_TOP_RIGHT);

	SetResize(IDC_BTN_HCS, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_BTN_HCS, SZ_TOP_LEFT, SZ_TOP_LEFT);

	SetResize(IDC_LBL_EXAMTYPE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_CBO_EXAMTYPE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_FIELDNAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_CBO_FIELDNAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_FIELDVALUE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_EDT_FIELDVALUE, SZ_TOP_LEFT, SZ_TOP_RIGHT);

	SetResize(IDC_LINE1, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_GRD_RESULT, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	m_ctlResult.GetPaintManager()->SetColumnOffice2007CustomThemeBaseColor(RGB(0,0,255));

	m_ctlResult.SetGridStyle(TRUE, xtpGridSolid);
	m_ctlResult.AllowEdit(FALSE);
	m_ctlResult.GetReportHeader()->AllowColumnRemove(FALSE);
	m_ctlResult.GetReportHeader()->AllowColumnSort(TRUE);
	m_ctlResult.GetReportHeader()->AllowColumnReorder(FALSE);
	m_ctlResult.SetMultipleSelection(FALSE);
	m_ctlResult.GetReportHeader()->SetAutoColumnSizing(FALSE); 
	m_ctlResult.GetPaintManager()->RefreshMetrics();
	
			// 0	序号
			// 1	检查日期
			// 2	患者姓名
			// 3	性别
			// 4	年龄
			// 5	检查项目
			// 6	送检科室
			// 7	住院号
			// 8	床号
			// 9	门诊号
			//10	体检号
			//11	影像编号
			//12	报告医师
			//13	检查时间
			//14	患者来源
			//15	检查类型
			//16	检查方法	
			//17	送检医师
			//18	申请单号
			//19	病区
			//20	医嘱号
			//21	存取号
			//22	报告日期
			//23	报告时间
			//24	审核日期
			//25	审核时间
			//26	审核医师
			//27	危急值标记
			//28	阳性/阴性
			//29	临床路径标记
			//30	提交日期
			//31	提交时间
			//32	版本号
			//33	传染性疾病
			//34	建议补增强


	m_ctlResult.AddColumn(new CXTPReportColumn( 0, _T("序号"),	 60, TRUE, -1, FALSE, TRUE ));
	m_ctlResult.AddColumn(new CXTPReportColumn( 1, _T("检查日期"), 116, TRUE, -1, FALSE, TRUE ));
	m_ctlResult.AddColumn(new CXTPReportColumn( 2, sPatientNameLabel, 100, TRUE, -1, TRUE, TRUE ));
	m_ctlResult.AddColumn(new CXTPReportColumn( 3, _T("性别"),60,  TRUE, -1, FALSE, TRUE ));
	m_ctlResult.AddColumn(new CXTPReportColumn( 4, _T("年龄"),80,  TRUE, -1, FALSE, TRUE ));
	m_ctlResult.AddColumn(new CXTPReportColumn( 5, _T("检查项目"), 180,  TRUE, -1, TRUE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn( 6, _T("送检科室"),120,  TRUE, -1, TRUE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn( 7, _T("住院号"), 140,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn( 8, _T("床号"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn( 9, _T("门诊号"), 140, TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(10, _T("体检号"), 140,  TRUE, -1, FALSE, TRUE));

	m_ctlResult.AddColumn(new CXTPReportColumn(11, _T("影像编号"),120,  TRUE, -1, TRUE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(12, _T("报告医师"), 120,  TRUE, -1, FALSE, TRUE));

	m_ctlResult.AddColumn(new CXTPReportColumn(13, _T("检查时间"), 100, TRUE, -1, FALSE, TRUE ));
	m_ctlResult.AddColumn(new CXTPReportColumn(14, _T("患者来源"), 100, TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(15, _T("检查类型"),100,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(16, _T("检查方法"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(17, _T("送检医师"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(18, _T("申请单号"),180,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(19, _T("病区"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(20, _T("医嘱号"),180,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(21, _T("存取号"),180,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(22, _T("报告日期"), 110,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(23, _T("报告时间"), 100,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(24, _T("审核日期"), 110,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(25, _T("审核时间"), 100,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(26, _T("审核医师"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(27, _T("危急值标记"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(28, _T("阳性/阴性"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(29, _T("临床路径"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(30, _T("提交日期"), 120,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(31, _T("提交时间"), 100,  TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(32, _T("版本号"), 90, TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(33, _T("传染性疾病"), 100, TRUE, -1, FALSE, TRUE));
	m_ctlResult.AddColumn(new CXTPReportColumn(34, _T("建议补增强"), 100, TRUE, -1, FALSE, TRUE));

	LoadHeaderWidthOfReportControl(m_ctlResult, szSectionName1, szKeyName1);

	m_ctlResult.SetVirtualMode(new  CMedicalResultRecord(pMedicalResultList, nMedicalResultListSize), nMedicalResultListSize);
	m_ctlResult.Populate();

	//
	dtTo = COleDateTime::GetCurrentTime();
	dtFrom = dtTo - COleDateTimeSpan(3, 0 ,0 ,0);

	m_dtpReportDateFrom.SetTime(dtFrom);
	m_dtpReportDateTo.SetTime(dtTo);

	m_cboExamType.Clear();
	for (ni = 0; _tcslen(g_SystemInfo[ni].lpszSystemCode1) > 0; ni ++) {
		// 只有LicServer中有定义的子系统才列出来

		if ((_tcsicmp(g_SystemInfo[ni].lpszSystemCode1, _T("All")) == 0) 
				|| (_tcslen(pConfig->GetSystemName(g_SystemInfo[ni].lpszSystemCode1)) > 0 ) 
				|| (_tcslen(pConfig->GetSystemName(g_SystemInfo[ni].lpszSystemCode2)) > 0 )
				|| (pApp->IsZQPETSystemValid() && (_tcsicmp(g_SystemInfo[ni].lpszSystemCode1, _T("PET")) == 0 || _tcsicmp(g_SystemInfo[ni].lpszSystemCode2, _T("PET")) == 0 ) )
				) {  
			nIndex = m_cboExamType.AddString(g_SystemInfo[ni].lpszSystemName);
			m_cboExamType.SetItemData(nIndex,(DWORD_PTR) g_SystemInfo[ni].lpszSystemCode1);
		}
	}

	if (m_cboExamType.GetCount() > 0)
		m_cboExamType.SetCurSel(0);

	m_cboFieldName.Clear();
	for (ni = 0; _tcslen(g_SearchFieldInfo[ni].lpszFieldName) > 0; ni ++) {
		sFieldName = g_SearchFieldInfo[ni].lpszFieldName;
		sFieldName = sFieldName.Trim();
		
		if (sLockedRefFromDept.IsEmpty() || !sFieldName.IsEmpty()) { 

			nIndex = m_cboFieldName.AddString(g_SearchFieldInfo[ni].lpszFieldLabel);
			m_cboFieldName.SetItemData(nIndex, (DWORD_PTR) & g_SearchFieldInfo[ni]);
		}
	}

	m_cboFieldName.SetCurSel(0);


	pBtnFromHCS = (CButton *) GetDlgItem(IDC_BTN_HCS);
	pBtnFromXIS = (CButton *) GetDlgItem(IDC_BTN_XIS);

	if (pBtnFromHCS) {
		pBtnFromHCS->SetCheck(pApp->IsDefaultSearchFromHCS() ? 1 : 0);
		pBtnFromHCS->EnableWindow(pApp->IsLockSearchMode() ? FALSE : TRUE);
	}

	if (pBtnFromXIS) {
		pBtnFromXIS->SetCheck(pApp->IsDefaultSearchFromHCS() ? 0 : 1);
		pBtnFromXIS->EnableWindow(pApp->IsLockSearchMode() ? FALSE : TRUE);
	}


	m_bInitialized = TRUE;
}


void CPublishedReportView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO

}


void CPublishedReportView::OnDestroy()
{
	const TCHAR *szSectionName1 = _T("Settings_GUI");
	const TCHAR *szKeyName1 = _T("HeaderWidthOfReportResultControl");
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	if (pApp->GetReportResultGridColumnWidthSaveMode() != 0)
		SaveHeaderWidthOfReportControl(m_ctlResult, szSectionName1, szKeyName1);

	CXTPResizeFormView::OnDestroy();
}


void  CPublishedReportView::LoadHeaderWidthOfReportControl(CXTPReportControl &wndReport, const TCHAR *szSectionName, const TCHAR *szKeyName)
{
	CString sHeaderTitle, sItem, sSubItem;
	int ni, nPos, nLen1, nLen2, nWidth;

	sHeaderTitle = AfxGetApp()->GetProfileString(szSectionName, szKeyName, _T("")); 
	nLen1 = sHeaderTitle.GetLength();

	for (ni = 0; ni < wndReport.GetColumns()->GetCount(); ni ++) {

		sItem.Format(_T("%s:"), wndReport.GetColumns()->GetAt(ni)->GetCaption());
		nLen2 = sItem.GetLength();

		nPos = sHeaderTitle.Find(sItem);
		if (nPos >= 0) {
			sSubItem = sHeaderTitle.Mid(nPos + nLen2, nLen1 - nPos);

			nPos = sSubItem.Find(_T(";"));
			if (nPos > 0)
				sSubItem = sSubItem.Mid(0, nPos);

			nWidth = _ttoi(sSubItem);

			if (nWidth > 0 && nWidth < 400)
				wndReport.GetColumns()->GetAt(ni)->SetWidth(nWidth);
		}
	}
}


void  CPublishedReportView::SaveHeaderWidthOfReportControl(CXTPReportControl &wndReport, const TCHAR *szSectionName, const TCHAR *szKeyName)
{
	CString sHeaderTitle, sItem;
	int ni;

	sHeaderTitle = _T("");

	for (ni = 0; ni < wndReport.GetColumns()->GetCount(); ni ++) {
		sItem.Format(_T("%s:%d"), 
			wndReport.GetColumns()->GetAt(ni)->GetCaption(),
			wndReport.GetColumns()->GetAt(ni)->GetWidth());

		if (!sHeaderTitle.IsEmpty())
			sHeaderTitle = sHeaderTitle + _T(";");

		sHeaderTitle = sHeaderTitle + sItem;
	}

	AfxGetApp()->WriteProfileString(szSectionName, szKeyName, sHeaderTitle); 

	m_bReportColumnWidthChanged = FALSE;

}



void CPublishedReportView::GetCommandTextByReportDate_XIS(const TCHAR *szSystem, const TCHAR *szDtFrom, const TCHAR *szDtTo, CString &sCommandText)
{
	CString sWhereExp("");
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();


	if (_tcsicmp(szSystem, _T("UIS")) == 0) {
		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0) { 
			//sWhereExp.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' AND IsFinished = 'T' ) "), szDtFrom, szDtTo);
			sWhereExp.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szDtFrom, szDtTo);
		}
		else if (_tcslen(szDtFrom) > 0) {
			//sWhereExp.Format(_T(" ( ExamDate = '%s' AND IsFinished = 'T' ) "), szDtFrom);
			sWhereExp.Format(_T(" ( ExamDate = '%s' AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szDtFrom);
		}
		else if (_tcslen(szDtTo) > 0) {
			//sWhereExp.Format(_T(" ( ExamDate = '%s' AND IsFinished = 'T' ) "), szDtTo);
			sWhereExp.Format(_T(" ( ExamDate = '%s' AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szDtTo);
		}

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, NAME"), m_sFieldsOfUIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {

		// 老的EIS
		
		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem)) {
				//sWhereExp.Format(_T(" ( ReportDate >= to_date('%s', 'YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') AND IsFinished = 'T' ) "), szDtFrom, szDtTo);
				sWhereExp.Format(_T(" ( ReportDate >= to_date('%s', 'YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') AND ( IsFinished = 'T' OR ImgCount > 0 ) ) "), szDtFrom, szDtTo);
			}
			else {
				//sWhereExp.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' AND IsFinished = 'T' ) "), szDtFrom, szDtTo);
				sWhereExp.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' AND ( IsFinished = 'T' OR ImgCount > 0 )  ) "), szDtFrom, szDtTo);
			}
		}
		else if (_tcslen(szDtFrom) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem)) {
				//sWhereExp.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') AND IsFinished = 'T' ) "), szDtFrom);
				sWhereExp.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') AND  ( IsFinished = 'T' OR ImgCount > 0)  ) "), szDtFrom);
			}
			else {
				//sWhereExp.Format(_T(" ( ReportDate = '%s' AND IsFinished = 'T' ) "), szDtFrom);
				sWhereExp.Format(_T(" ( ReportDate = '%s' AND ( IsFinished = 'T' OR ImgCount > 0)  ) "), szDtFrom);
			}

		}
		else if (_tcslen(szDtTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem)) {
				//sWhereExp.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') AND IsFinished = 'T' ) "), szDtTo);
				sWhereExp.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') AND ( IsFinished = 'T' OR ImgCount > 0)  ) "), szDtTo);
			}
			else {
				//sWhereExp.Format(_T(" ( ReportDate = '%s' AND IsFinished = 'T' ) "), szDtTo);
				sWhereExp.Format(_T(" ( ReportDate = '%s' AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szDtTo);
			}
		}

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ReportDATE DESC, NAME"), m_sFieldsOfEIS, sWhereExp); 
		

		// 新的EIS
		/*
		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s') "), szDtFrom, szDtTo);
		else if (_tcslen(szDtFrom) > 0)
			sWhereExp.Format(_T(" ( StudyDate = '%s') "), szDtFrom);
		else if (_tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( StudyDate = '%s') "), szDtTo );

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies Where %s  AND StudyStatus >= 4 ORDER BY ReportDate Desc, PatientName"), m_sFieldsOfEIS, sWhereExp);
		*/
	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {

		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0) 
			sWhereExp.Format(_T(" ( Convert(char(10), Bgrq, 120) >= '%s' AND Convert(char(10), Bgrq, 120) <= '%s' ) "), szDtFrom, szDtTo);
		else if (_tcslen(szDtFrom) > 0)
			sWhereExp.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szDtFrom);
		else if (_tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szDtTo);

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);
	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {

		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0) 
			sWhereExp.Format(_T(" ( Convert(char(10), BGSJ, 120) >= '%s' AND Convert(char(10), BGSJ, 120) <= '%s' ) "), szDtFrom, szDtTo);
		else if (_tcslen(szDtFrom) > 0)
			sWhereExp.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szDtFrom);
		else if (_tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szDtTo);

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);
	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {

		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szDtFrom, szDtTo);
		else if (_tcslen(szDtFrom) > 0)
			sWhereExp.Format(_T(" ( StudyDate = '%s' "), szDtFrom);
		else if (_tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( StudyDate = '%s' ) "), szDtTo );

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies Where %s  AND StudyStatus >= 4 ORDER BY StudyDate Desc, StudyTime,  PatientName"), m_sFieldsOfRIS, sWhereExp);

	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {

		if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szDtFrom, szDtTo);
		else if (_tcslen(szDtFrom) > 0)
			sWhereExp.Format(_T(" ( StudyDate = '%s' "), szDtFrom);
		else if (_tcslen(szDtTo) > 0)
			sWhereExp.Format(_T(" ( StudyDate = '%s' ) "), szDtTo );

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs Where %s  AND StudyStatus >= 4 ORDER BY StudyDate Desc, StudyTime,  PatientName"), m_sFieldsOfPET, sWhereExp);

	}

}


void CPublishedReportView::GetCommandTextByOrderNo_XIS(const TCHAR *szSystem, const TCHAR *szOrderNo, CString &sCommandText)
{
	CString sWhereExp("");

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {
		//sWhereExp.Format(_T(" ( HISID = '%s' OR OrderNo = '%s' ) and IsFinished = 'T' "), szOrderNo, szOrderNo);
		sWhereExp.Format(_T(" ( HISID = '%s' OR OrderNo = '%s' ) and ( IsFinished = 'T' OR ImgCount > 0)  "), szOrderNo, szOrderNo);

		sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, NAME"), m_sFieldsOfUIS, sWhereExp); 
	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		sWhereExp.Format(_T(" ( HISID = '%s') and ( IsFinished = 'T' OR ImgCount > 0) "), szOrderNo);

		sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ReportDATE DESC, NAME"), m_sFieldsOfEIS, sWhereExp);
		

		// 新EIS
		/*
		sWhereExp.Format(_T("( OrderNo = '%s' And StudyStatus >= 4 )"), szOrderNo);
		sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate Desc, PatientName "), m_sFieldsOfEIS, sWhereExp);
		*/
	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sWhereExp.Format(_T(" ( HISID = '%s' AND bgrq is not null ) "), szOrderNo);

		sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);
	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sWhereExp.Format(_T(" ( HISID = '%s' AND BGSJ is not null ) "), szOrderNo);

		sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);
	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0){
		sWhereExp.Format(_T("( OrderNo = '%s' And StudyStatus >= 4 )"), szOrderNo);
		sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate Desc,StudyTime, PatientName "), m_sFieldsOfRIS, sWhereExp);
	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0){
		sWhereExp.Format(_T("( OrderNo = '%s' And StudyStatus >= 4 )"), szOrderNo);
		sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate Desc,StudyTime, PatientName "), m_sFieldsOfPET, sWhereExp);
	}



}


void CPublishedReportView::GetCommandTextByInpatientNoOrOutpatientNo_XIS(const TCHAR *szSystem, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szReportDateFrom ,const TCHAR * szReportDateTo, CString &sCommandText)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	CString sExp1(""), sExp2(""), sWhereExp("");

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {
		
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			//sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( IsFinished = 'T' ) "), szInpatientNo, szOutpatientNo); 
			sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( IsFinished = 'T' OR ImgCount > 0 ) "), szInpatientNo, szOutpatientNo); 
		}
		else if (_tcslen(szInpatientNo) > 0) {
			//sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( IsFinished = 'T' ) "), szInpatientNo); 
			sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( IsFinished = 'T' OR ImgCount > 0 ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			//sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( IsFinished = 'T' ) "), szOutpatientNo); 
			sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( IsFinished = 'T' OR ImgCount > 0 ) "), szOutpatientNo); 
		}


		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, NAME"), m_sFieldsOfUIS, sWhereExp); 


	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			//sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( IsFinished = 'T' ) "), szInpatientNo, szOutpatientNo); 
			sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( IsFinished = 'T' OR ImgCount > 0 ) "), szInpatientNo, szOutpatientNo); 
		}
		else if (_tcslen(szInpatientNo) > 0) {
			//sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( IsFinished = 'T' ) "), szInpatientNo); 
			sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( IsFinished = 'T' OR ImgCount > 0 ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			//sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( IsFinished = 'T' ) "), szOutpatientNo); 
			sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( IsFinished = 'T' OR ImgCount > 0 ) "), szOutpatientNo); 
		}


		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0)  {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate >= to_date('%s', 'YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom, szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		}
		else if (_tcslen(szReportDateFrom) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
		}
		else if (_tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);
		}


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ReportDate DESC, NAME"), m_sFieldsOfEIS, sWhereExp); 
		

		// 新EIS
		/*
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szInpatientNo, szOutpatientNo); 
		}
		else if (_tcslen(szInpatientNo) > 0) {
			sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szOutpatientNo); 
		}


		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, PatientName"), m_sFieldsOfEIS, sWhereExp); 
		*/
	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" ( ( zyh = '%s' OR  mzh = '%s') AND ( bgrq is not null ) ) "), szInpatientNo, szOutpatientNo);
		}
		else if (_tcslen(szInpatientNo) > 0) {
			sExp1.Format(_T(" ( zyh = '%s' AND bgrq is not null ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" ( mzh = '%s' AND bgrq is not null ) "), szOutpatientNo);
		}

		
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Bgrq >= '%s' AND Bgrq <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Bgrq = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Bgrq = '%s' ) "), szReportDateTo);

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);

	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" ( ( ZYH = '%s' OR  MZH = '%s') AND ( BGSJ is not null ) ) "), szInpatientNo, szOutpatientNo);
		}
		else if (_tcslen(szInpatientNo) > 0) {
			sExp1.Format(_T(" ( ZYH = '%s' AND BGSJ is not null ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" ( MZH = '%s' AND BGSJ is not null ) "), szOutpatientNo);
		}

		
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( BGSJ >= '%s' AND BGSJ <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( BGSJ = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( BGSJ = '%s' ) "), szReportDateTo);

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);

	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {
		
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szInpatientNo, szOutpatientNo); 
		}
		else if (_tcslen(szInpatientNo) > 0) {
			sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szOutpatientNo); 
		}


		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfRIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {
		
		if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szInpatientNo, szOutpatientNo); 
		}
		else if (_tcslen(szInpatientNo) > 0) {
			sExp1.Format(_T(" (InpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szInpatientNo); 
		}
		else if (_tcslen(szOutpatientNo) > 0) {
			sExp1.Format(_T(" (OutpatientNo = '%s' ) AND ( StudyStatus >= 4 ) "), szOutpatientNo); 
		}


		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfPET, sWhereExp); 

	}

}


void CPublishedReportView::GetCommandTextByPatientID_XIS(const TCHAR *szSystem, const TCHAR *szPatientID, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sCommandText)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	CString sExp1(""), sExp2(""), sWhereExp("");

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {
		//sExp1.Format(_T(" ( PatientID = '%s' AND IsFinished = 'T' ) "), szPatientID); 
		sExp1.Format(_T(" ( PatientID = '%s' AND (IsFinished = 'T' OR ImgCount > 0) ) "), szPatientID); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, NAME"), m_sFieldsOfUIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		sExp1.Format(_T(" ( PatientID = '%s' AND (IsFinished = 'T' OR ImgCount > 0) ) "), szPatientID); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate >= to_date('%s', 'YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom, szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		}
		else if (_tcslen(szReportDateFrom) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);

		}
		else if (_tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);
		}


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ReportDate DESC, NAME"), m_sFieldsOfEIS, sWhereExp); 
		

		// 新EIS
		/*
		sExp1.Format(_T(" ( MRN = '%s' AND StudyStatus >= 4 ) "), szPatientID); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC,  PatientName"), m_sFieldsOfEIS, sWhereExp);
		*/

	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( brid = '%s' AND bgrq is not null ) "), szPatientID);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Bgrq >= '%s' AND Bgrq <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Bgrq = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Bgrq = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);

	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( BRID = '%s' AND BGSJ is not null ) "), szPatientID);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( BGSJ >= '%s' AND BGSJ <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( BGSJ = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( BGSJ = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);

	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {
		sExp1.Format(_T(" ( MRN = '%s' AND StudyStatus >= 4 ) "), szPatientID); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC,StudyTime,  PatientName"), m_sFieldsOfRIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {
		sExp1.Format(_T(" ( MRN = '%s' AND StudyStatus >= 4 ) "), szPatientID); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate DESC,StudyTime,  PatientName"), m_sFieldsOfPET, sWhereExp); 

	}

}



void CPublishedReportView::GetCommandTextByDocID_XIS(const TCHAR *szSystem, const TCHAR *szDocID,  CString &sCommandText)
{
	CString sWhereExp("");

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {

		//sWhereExp.Format(_T(" ( DocID = '%s' ) and IsFinished = 'T' "), szDocID);
		sWhereExp.Format(_T(" ( DocID = '%s' ) and (IsFinished = 'T' OR ImgCount > 0 ) "), szDocID);

		sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, EXAMTIME, NAME"), m_sFieldsOfUIS, sWhereExp); 
	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		sWhereExp.Format(_T(" ( DocID = '%s' ) and ( IsFinished = 'T' OR ImgCount > 0) "), szDocID);

		sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, EXAMTIME, NAME"), m_sFieldsOfEIS, sWhereExp);
		

		// 新EIS
		/*
		sWhereExp.Format(_T(" ( DocID = '%s' OR OldDocID = '%s' ) and StudyStatus >= 4 "), szDocID, szDocID);	// 放射子系统查询时， 用拍片日期,因为要看还没完成报告的图像

		sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, PatientNAME"), m_sFieldsOfEIS, sWhereExp); 
		*/
	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sWhereExp.Format(_T(" ( Medicalno = '%s' AND bgrq is not null ) "), szDocID);

		sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);
	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sWhereExp.Format(_T(" ( BLH = '%s' AND BGSJ is not null ) "), szDocID);

		sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);
	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {
		sWhereExp.Format(_T(" ( DocID = '%s' OR OldDocID = '%s' ) and StudyStatus >= 4 "), szDocID, szDocID);	// 放射子系统查询时， 用拍片日期,因为要看还没完成报告的图像

		sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientNAME"), m_sFieldsOfRIS, sWhereExp); 
	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {
		sWhereExp.Format(_T(" ( DocID = '%s' OR OldDocID = '%s' ) and StudyStatus >= 4 "), szDocID, szDocID);	// PET子系统查询时， 用拍片日期,因为要看还没完成报告的图像

		sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientNAME"), m_sFieldsOfPET, sWhereExp); 
	}
}


void CPublishedReportView::GetCommandTextByPatientName_XIS(const TCHAR *szSystem, const TCHAR *szPatientName, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sCommandText)
{
	CString sExp1(""), sExp2(""), sWhereExp("");
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {

		//sExp1.Format(_T(" ( Name = '%s' AND IsFinished = 'T' ) "), szPatientName); 
		sExp1.Format(_T(" ( Name = '%s' AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szPatientName); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, EXAMTIME, NAME"), m_sFieldsOfUIS, sWhereExp); 

	}
	if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		sExp1.Format(_T(" ( Name = '%s' AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szPatientName); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) { 
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate >= to_date('%s', 'YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom, szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		}
		else if (_tcslen(szReportDateFrom) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
		}
		else if (_tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);
		}

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ExamDate DESC, ExamTime, NAME"), m_sFieldsOfEIS, sWhereExp); 
		

		// 新EIS
		/*
		sExp1.Format(_T(" ( PatientName = '%s' AND StudyStatus >= 4 ) "), szPatientName); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfEIS, sWhereExp); 
		*/
	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( xm = '%s' AND bgrq is not null ) "), szPatientName);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) >= '%s' AND Convert(char(10), Bgrq, 120) <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szReportDateTo);

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);
	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( PATIENTNAME = '%s' AND BGSJ is not null ) "), szPatientName);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) >= '%s' AND Convert(char(10), BGSJ, 120) <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szReportDateTo);

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS, sWhereExp);
	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {
		sExp1.Format(_T(" ( PatientName = '%s' AND StudyStatus >= 4 ) "), szPatientName); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfRIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {
		sExp1.Format(_T(" ( PatientName = '%s' AND StudyStatus >= 4 ) "), szPatientName); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfPET, sWhereExp); 

	}


}

void CPublishedReportView::GetCommandTextByRefDoctor_XIS(const TCHAR *szSystem, const TCHAR *szRefDoctor, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sCommandText)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	CString sExp1(""), sExp2(""), sWhereExp("");

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {
		//sExp1.Format(_T(" ( FromDoctor = '%s' AND IsFinished = 'T' ) "), szRefDoctor); 
		sExp1.Format(_T(" ( FromDoctor = '%s' AND ( IsFinished = 'T' OR ImgCount > 0 ) ) "), szRefDoctor); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, EXAMTIME, NAME"), m_sFieldsOfUIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		sExp1.Format(_T(" ( FromDoctor = '%s' AND ( IsFinished = 'T' OR ImgCount > 0 ) ) "), szRefDoctor); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem)) 
				sExp2.Format(_T(" ( ReportDate >= to_date('%s','YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom, szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		}
		else if (_tcslen(szReportDateFrom) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s','YYYY-MM-DD') ) "), szReportDateFrom);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
		}
		else if (_tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);
		}

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ExamDate DESC, ExamTime, NAME"), m_sFieldsOfEIS, sWhereExp); 
		

		// 新EIS
		/*
		sExp1.Format(_T(" ( RefDoctor = '%s' AND StudyStatus >= 4 ) "), szRefDoctor); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, PatientName"), m_sFieldsOfEIS, sWhereExp); 
		*/

	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( Sjys = '%s' ) "), szRefDoctor);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) >= '%s' AND Convert(char(10), Bgrq, 120) <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szReportDateTo);
		else
			sExp2.Format(_T(" (Bgrq is not null ) "));


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);

	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( REQDOCT = '%s' ) "), szRefDoctor);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) >= '%s' AND Convert(char(10), BGSJ, 120) <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szReportDateTo);
		else
			sExp2.Format(_T(" (BGSJ is not null ) "));


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);

	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {
		sExp1.Format(_T(" ( RefDoctor = '%s' AND StudyStatus >= 4 ) "), szRefDoctor); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfRIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {
		sExp1.Format(_T(" ( RefDoctor = '%s' AND StudyStatus >= 4 ) "), szRefDoctor); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfPET, sWhereExp); 

	}

}


void CPublishedReportView::GetCommandTextByRefDept_XIS(const TCHAR *szSystem, const TCHAR *szRefDept, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sCommandText)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	CString sExp1(""), sExp2(""), sWhereExp("");

	if (_tcsicmp(szSystem, _T("UIS")) == 0) {
		//sExp1.Format(_T(" ( (FromDept = '%s' OR FromDept Like '%s%%' ) AND IsFinished = 'T' ) "), szRefDept, szRefDept); 
		sExp1.Format(_T(" ( (FromDept = '%s' OR FromDept Like '%s%%' ) AND ( IsFinished = 'T' OR ImgCount > 0) ) "), szRefDept, szRefDept); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( ExamDate >= '%s' AND ExamDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( ExamDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY EXAMDATE DESC, EXAMTIME, NAME"), m_sFieldsOfUIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("EIS")) == 0) {
		// 老EIS
		
		sExp1.Format(_T(" ( (FromDept = '%s' OR FromDept Like '%s%%')  AND (IsFinished = 'T' OR ImgCount > 0) ) "), szRefDept, szRefDept); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem)) 
				sExp2.Format(_T(" ( ReportDate >= to_date('%s','YYYY-MM-DD') AND ReportDate <= to_date('%s', 'YYYY-MM-DD') ) "), szReportDateFrom, szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		}
		else if (_tcslen(szReportDateFrom) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s','YYYY-MM-DD') ) "), szReportDateFrom);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
		}
		else if (_tcslen(szReportDateTo) > 0) {
			if (pConfig->XISDBTypeIsOracle(szSystem))
				sExp2.Format(_T(" ( ReportDate = to_date('%s', 'YYYY-MM-DD') ) "), szReportDateTo);
			else
				sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);
		}

		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM EXAMRECORD WHERE %s ORDER BY ExamDate DESC, ExamTime, NAME"), m_sFieldsOfEIS, sWhereExp); 
		

		// 新EIS
		/*
		sExp1.Format(_T(" ( (RefDept = '%s' OR RefDept Like '%s%%')  AND StudyStatus >= 4 ) "), szRefDept, szRefDept); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, PatientName"), m_sFieldsOfEIS, sWhereExp); 
		*/


	}
	/*
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( Sjks = '%s' OR Sjks Like '%s%%' ) "), szRefDept, szRefDept);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) >= '%s' AND Convert(char(10), Bgrq, 120) <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Convert(char(10), Bgrq, 120) = '%s' ) "), szReportDateTo);
		else
			sExp2.Format(_T(" (Bgrq is not null ) "));


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM cmipsmain WHERE %s ORDER BY Bgrq DESC, xm"), m_sFieldsOfPIS, sWhereExp);

	}
	*/
	else if (_tcsicmp(szSystem, _T("PIS")) == 0) {
		sExp1.Format(_T(" ( REQDEPT = '%s' OR REQDEPT Like '%s%%' ) "), szRefDept, szRefDept);

		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) >= '%s' AND Convert(char(10), BGSJ, 120) <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( Convert(char(10), BGSJ, 120) = '%s' ) "), szReportDateTo);
		else
			sExp2.Format(_T(" (BGSJ is not null ) "));


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;

		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM T_Patients WHERE %s ORDER BY BGSJ DESC, PATIENTNAME"), m_sFieldsOfPIS_1, sWhereExp);

	}
	else if (_tcsicmp(szSystem, _T("RIS")) == 0) {
		sExp1.Format(_T(" ( (RefDept = '%s' OR RefDept Like '%s%%')  AND StudyStatus >= 4 ) "), szRefDept, szRefDept); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM Studies WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfRIS, sWhereExp); 

	}
	else if (_tcsicmp(szSystem, _T("PET")) == 0) {
		sExp1.Format(_T(" ( (RefDept = '%s' OR RefDept Like '%s%%')  AND StudyStatus >= 4 ) "), szRefDept, szRefDept); 
	
		if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
			sExp2.Format(_T(" ( StudyDate >= '%s' AND StudyDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
		else if (_tcslen(szReportDateFrom) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateFrom);
		else if (_tcslen(szReportDateTo) > 0)
			sExp2.Format(_T(" ( StudyDate = '%s' ) "), szReportDateTo);


		if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
			sWhereExp = sExp1 + _T(" AND ") + sExp2;
		else if (!sExp1.IsEmpty())
			sWhereExp = sExp1;
		else if (!sExp2.IsEmpty())
			sWhereExp = sExp2;


		if (!sWhereExp.IsEmpty())
			sCommandText.Format(_T("SELECT %s FROM v_NMView_pacs WHERE %s ORDER BY StudyDate DESC, StudyTime, PatientName"), m_sFieldsOfPET, sWhereExp); 

	}

}


///////////////////////////////////////////////////////////////////////////////

void CPublishedReportView::GetSystemCodeExp(const TCHAR *szSystem, CString &sSystemCodeExp)
{
	if (_tcsicmp(szSystem, _T("UIS")) == 0  || _tcsicmp(szSystem, _T("CLOUDUIS")) == 0)
		sSystemCodeExp = L" AND (SystemCode = 'UIS' ) ";
	else if (_tcsicmp(szSystem, _T("RIS")) == 0  || _tcsicmp(szSystem, _T("CLOUDRIS")) == 0)
		sSystemCodeExp = L" AND (SystemCode = 'RIS' ) ";
	else if (_tcsicmp(szSystem, _T("EIS")) == 0  || _tcsicmp(szSystem, _T("CLOUDEIS")) == 0)
		sSystemCodeExp = L" AND (SystemCode = 'EIS' ) ";
	else if (_tcsicmp(szSystem, _T("PET")) == 0  || _tcsicmp(szSystem, _T("PET")) == 0)
		sSystemCodeExp = L" AND (SystemCode = 'PET' ) ";
	else if (_tcsicmp(szSystem, _T("PIS")) == 0  || _tcsicmp(szSystem, _T("CLOUDPIS")) == 0)
		sSystemCodeExp = L" AND (SystemCode = 'PIS' ) ";
	else
		sSystemCodeExp = L"";
}


void CPublishedReportView::GetWhereExpByReportDate_HCS(const TCHAR *szSystem, const TCHAR *szDtFrom, const TCHAR *szDtTo, CString &sWhereExp)
{
	CString sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);

	if (_tcslen(szDtFrom) > 0 && _tcslen(szDtTo) > 0) 
		sWhereExp.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s') %s "), szDtFrom, szDtTo, sSystemCodeExp);
	else if (_tcslen(szDtFrom) > 0)
		sWhereExp.Format(_T(" ( ReportDate = '%s' ) %s "), szDtFrom, sSystemCodeExp);
	else if (_tcslen(szDtTo) > 0)
		sWhereExp.Format(_T(" ( ReportDate = '%s' ) %s "), szDtTo, sSystemCodeExp);
}


void CPublishedReportView::GetWhereExpByOrderNo_HCS(const TCHAR *szSystem, const TCHAR *szOrderNo, CString &sWhereExp)
{
	CString sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);

	sWhereExp.Format(_T(" ( OrderNo = '%s' ) "), szOrderNo);
}


void CPublishedReportView::GetWhereExpByInpatientNoOrOutpatientNo_HCS(const TCHAR *szSystem, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szReportDateFrom ,const TCHAR * szReportDateTo, CString &sWhereExp)
{
	CString sExp1(""), sExp2(""), sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);

	if (_tcslen(szInpatientNo) > 0 && _tcslen(szOutpatientNo) > 0) {
		sExp1.Format(_T(" (InpatientNo = '%s' OR OutpatientNo = '%s' OR CheckupNo = '%s' ) "), szInpatientNo, szOutpatientNo, szOutpatientNo); 
	}
	else if (_tcslen(szInpatientNo) > 0) {
		sExp1.Format(_T(" (InpatientNo = '%s') "), szInpatientNo); 
	}
	else if (_tcslen(szOutpatientNo) > 0) {
		sExp1.Format(_T(" (OutpatientNo = '%s' OR CheckupNo = '%s' ) "), szOutpatientNo, szOutpatientNo); 
	}

	if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
		sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
	else if (_tcslen(szReportDateFrom) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
	else if (_tcslen(szReportDateTo) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);


	if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
		sWhereExp.Format(_T("%s AND %s %s "), sExp1, sExp2, sSystemCodeExp);
	else if (!sExp1.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp1, sSystemCodeExp);
	else if (!sExp2.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp2, sSystemCodeExp);

}


void CPublishedReportView::GetWhereExpByPatientID_HCS(const TCHAR *szSystem, const TCHAR *szPatientID, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sWhereExp)
{
	CString sExp1(""), sExp2(""), sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);

	sExp1.Format(_T(" ( PatientID = '%s' ) "), szPatientID); 
	
	if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
		sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
	else if (_tcslen(szReportDateFrom) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
	else if (_tcslen(szReportDateTo) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);


	if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
		sWhereExp.Format(_T("%s AND %s %s"), sExp1, sExp2, sSystemCodeExp);
	else if (!sExp1.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp1, sSystemCodeExp);
	else if (!sExp2.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp2, sSystemCodeExp);

}



void CPublishedReportView::GetWhereExpByDocID_HCS(const TCHAR *szSystem, const TCHAR *szDocID,  CString &sWhereExp)
{
	CString sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);
	
	sWhereExp.Format(_T(" ( DocID = '%s' ) "), szDocID);
}


void CPublishedReportView::GetWhereExpByPatientName_HCS(const TCHAR *szSystem, const TCHAR *szPatientName, const TCHAR *szReportDateFrom, const TCHAR *szReportDateTo, CString &sWhereExp)
{
	CString sExp1(""), sExp2(""), sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);


	sExp1.Format(_T(" ( PatientName = '%s') "), szPatientName); 
	
	if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
		sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
	else if (_tcslen(szReportDateFrom) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
	else if (_tcslen(szReportDateTo) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);


	if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
		sWhereExp.Format(_T("%s AND %s %s"), sExp1, sExp2, sSystemCodeExp);
	else if (!sExp1.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp1, sSystemCodeExp);
	else if (!sExp2.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp2, sSystemCodeExp);

}

void CPublishedReportView::GetWhereExpByRefDoctor_HCS(const TCHAR *szSystem, const TCHAR *szRefDoctor, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sWhereExp)
{
	CString sExp1(""), sExp2(""), sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);

	sExp1.Format(_T(" ( RefDoctorID = '%s' OR RefDoctorName = '%s' ) "), szRefDoctor, szRefDoctor); 
	
	if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
		sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
	else if (_tcslen(szReportDateFrom) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
	else if (_tcslen(szReportDateTo) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);


	if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
		sWhereExp.Format(_T("%s AND %s %s"), sExp1, sExp2, sSystemCodeExp);
	else if (!sExp1.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp1, sSystemCodeExp);
	else if (!sExp2.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp2, sSystemCodeExp);

}

void CPublishedReportView::GetWhereExpByRefDept_HCS(const TCHAR *szSystem, const TCHAR *szRefDept, const TCHAR *szReportDateFrom ,const TCHAR *szReportDateTo, CString &sWhereExp)
{
	CString sExp1(""), sExp2(""), sSystemCodeExp("");

	GetSystemCodeExp(szSystem, sSystemCodeExp);

	sExp1.Format(_T(" ( RefDeptID = '%s' OR  RefDeptName = '%s' OR RefDeptName Like '%s%%' ) "), szRefDept, szRefDept, szRefDept); 
	
	if (_tcslen(szReportDateFrom) > 0 && _tcslen(szReportDateTo) > 0) 
		sExp2.Format(_T(" ( ReportDate >= '%s' AND ReportDate <= '%s' ) "), szReportDateFrom, szReportDateTo);
	else if (_tcslen(szReportDateFrom) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateFrom);
	else if (_tcslen(szReportDateTo) > 0)
		sExp2.Format(_T(" ( ReportDate = '%s' ) "), szReportDateTo);


	if (!sExp1.IsEmpty() && !sExp2.IsEmpty()) 
		sWhereExp.Format(_T("%s AND %s %s"), sExp1, sExp2, sSystemCodeExp);
	else if (!sExp1.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp1, sSystemCodeExp);
	else if (!sExp2.IsEmpty())
		sWhereExp.Format(_T("%s %s"), sExp2, sSystemCodeExp);

}


///////////////////////////////////////////////////////////////////////////////
void CPublishedReportView::DoQuery(const TCHAR *szSystem, const TCHAR *szOrderNo, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szPatientID, const TCHAR *szDocID)
{
	BOOL bInteractived = FALSE;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();
	CButton *pBtnFromHCS;
	BOOL bSearchFromHCS;
	BOOL bIgnoreStudyDate;
	SAConnection con;
	SACommand cmd;
	SAString strCmdText("");
	CString sSystemCode(""), sOrderNo(""), sInpatientNo(""), sOutpatientNo(""), sPatientID(""), sDocID("");
	int nPos, nLen, nRecentListFieldTag = 0;
	const TCHAR *lpszSystemCode=NULL, *lpszCommandText = NULL;
	CString sMsg,sText(""), sTableName, sWhere(""), sHospitalID(""), sHospitalIDExp("");
	CString sOldStudyGUID("");
	COleDateTime dtFrom, dtTo, dtTmp;
	CString sDtFrom(""), sDtTo(""), sFieldName(""), sFieldValue("");
	COleDateTime dtFromX, dtToX;
	CString sDtFromX(""), sDtToX("");
	int nItemIndex = 0, ni;
	const TCHAR *lpszItemData;
	SearchFieldInfoStruct *pFieldInfo;
	CString sWhereExp(""), sCommandText("");
	CQueryParams *pQueryParams;
	std::vector <CQueryParams *> lstQueryParams;
	std::vector <CQueryParams *> ::iterator itQueryParam;
	std::vector <CMedicalResultObject *> *pMedicalResultObjectList = GetDocument()->GetMedicalResultObjectList();
	std::vector <CMedicalResultObject *> ::iterator itResult;
	CMedicalResultObject *pMedicalResultObject;
	CMedicalResultObject *pCurrentMedicalResultObject = NULL;
	int nSelectedRowIndex = 0;
	int nIndex;
	CXTPReportRow *pReportRow;
	CWaitCursor wait;

	bIgnoreStudyDate = pApp->IsIgnoreStudyDateWhileSearchingByIdentity();

	m_sStudyGUIDSaved = _T("");

	sOldStudyGUID = _T("");
	if (m_ctlResult.GetSelectedRows()) {
		pReportRow = m_ctlResult.GetSelectedRows()->GetAt(0);
		if (pReportRow) {				
			nIndex = pReportRow->GetIndex();

			if (nIndex >= 0 && nIndex < pMedicalResultObjectList->size()) {
				sOldStudyGUID = (*pMedicalResultObjectList)[nIndex]->m_sStudyGUID;
			}
		}
	}


	nRecentListFieldTag = 0; 

	m_bDataLoading = TRUE;

	// 处理输入参数 

	sSystemCode = szSystem;

	sOrderNo = szOrderNo;
	nLen = sOrderNo.GetLength();
	if (nLen > 0) {
		nPos = sOrderNo.Find(_T("@"));
		if (nPos > 0) {
			sSystemCode = sOrderNo.Mid(nPos + 1);
			sOrderNo = sOrderNo.Mid(0, nPos);
		}
	}

	sInpatientNo = szInpatientNo;
	nLen = sInpatientNo.GetLength();
	if (nLen > 0) {
		nPos = sInpatientNo.Find(_T("@"));
		if (nPos > 0) {
			sSystemCode = sInpatientNo.Mid(nPos + 1);
			sInpatientNo = sInpatientNo.Mid(0, nPos);
		}
	}

	sOutpatientNo = szOutpatientNo;
	nLen = sOutpatientNo.GetLength();
	if (nLen > 0) {
		nPos = sOutpatientNo.Find(_T("@"));
		if (nPos > 0) {
			sSystemCode = sOutpatientNo.Mid(nPos + 1);
			sOutpatientNo = sOutpatientNo.Mid(0, nPos);
		}
	}

	sPatientID = szPatientID;
	nLen = sPatientID.GetLength();
	if (nLen > 0) {
		nPos = sPatientID.Find(_T("@"));
		if (nPos > 0) {
			sSystemCode = sPatientID.Mid(nPos + 1);
			sPatientID = sPatientID.Mid(0, nPos);
		}
	}

	sDocID = szDocID;
	nLen = sDocID.GetLength();
	if (nLen > 0) {
		nPos = sDocID.Find(_T("@"));
		if (nPos > 0) { 
			sSystemCode = sDocID.Mid(nPos + 1);
			sDocID = sDocID.Mid(0, nPos);
		}
	}

	sHospitalID = pApp->GetHospitalID();
	if (!sHospitalID.IsEmpty() && pApp->IsSearchOnlyWithinHospital())
		sHospitalIDExp.Format(_T(" AND HospitalID = '%s' "), sHospitalID);
	else
		sHospitalIDExp = L"";
	//

	pBtnFromHCS = (CButton *) GetDlgItem(IDC_BTN_HCS);
	bSearchFromHCS = (pBtnFromHCS->GetCheck() == 1);

	GetDocument()->SetCurrentMedicalResultObject(NULL);
	GetDocument()->UpdateAllViews(NULL);

	for (itResult = pMedicalResultObjectList->begin(); itResult != pMedicalResultObjectList->end(); itResult ++) {
		delete (*itResult);
		(*itResult) = NULL;
	}

	pMedicalResultObjectList->clear();

	m_ctlResult.SetVirtualMode(new  CMedicalResultRecord(pMedicalResultObjectList, 0), 0);
	m_ctlResult.Populate();

	m_dtpReportDateFrom.GetTime(dtFrom);
	m_dtpReportDateTo.GetTime(dtTo);

	if (dtFrom > dtTo) {
		dtTmp = dtFrom;
		dtFrom = dtTo;
		dtTo = dtTmp;
	}

	sDtFrom.Format(_T("%d-%02d-%02d"), dtFrom.GetYear(), dtFrom.GetMonth(), dtFrom.GetDay());
	sDtTo.Format(_T("%d-%02d-%02d"), dtTo.GetYear(), dtTo.GetMonth(), dtTo.GetDay());


	dtToX = COleDateTime::GetCurrentTime();
	dtFromX = dtToX - COleDateTimeSpan(pApp->GetValidDaysOfCmdLineQuery(), 0 ,0 ,0);

	sDtFromX.Format(_T("%d-%02d-%02d"), dtFromX.GetYear(), dtFromX.GetMonth(), dtFromX.GetDay());
	sDtToX.Format(_T("%d-%02d-%02d"), dtToX.GetYear(), dtToX.GetMonth(), dtToX.GetDay());

	// 获得检查类型
	nItemIndex = m_cboExamType.GetCurSel();
	if (nItemIndex < 0)
		return;

	// 可以不包含SystemCode 参数， 只要有OrderNo, InpatientNo, OutpatientNo, PatientID 之一，则是调用参数查询
	if (!sOrderNo.IsEmpty() || !sInpatientNo.IsEmpty() || !sOutpatientNo.IsEmpty() || !sPatientID.IsEmpty() || !sDocID.IsEmpty())  {
	
		bInteractived = FALSE;
	
		if (!bSearchFromHCS) {
			// 从各个子系统查找
			for (ni = 0; ni < m_cboExamType.GetCount(); ni ++ ) {
				lpszItemData = (const TCHAR *) m_cboExamType.GetItemData(ni);
			
				sCommandText = _T("");
			
				if (_tcsicmp(lpszItemData, _T("All")) != 0) {
					if ((sSystemCode.CompareNoCase(lpszItemData) == 0) || sSystemCode.IsEmpty()) {
						if (!sOrderNo.IsEmpty()) 
							GetCommandTextByOrderNo_XIS(lpszItemData, sOrderNo, sCommandText);
						else if (!sInpatientNo.IsEmpty())
							GetCommandTextByInpatientNoOrOutpatientNo_XIS(lpszItemData, sInpatientNo, _T(""), bIgnoreStudyDate ? _T("") : sDtFromX, bIgnoreStudyDate ? _T("") : sDtToX, sCommandText);
						else if (!sOutpatientNo.IsEmpty())
							GetCommandTextByInpatientNoOrOutpatientNo_XIS(lpszItemData, _T(""), sOutpatientNo, sDtFromX, sDtToX, sCommandText);
						else if (!sPatientID.IsEmpty())
							GetCommandTextByPatientID_XIS(lpszItemData, sPatientID, bIgnoreStudyDate ? _T("") : sDtFromX, bIgnoreStudyDate ? _T("") : sDtToX, sCommandText);
						else if (!sDocID.IsEmpty())
							GetCommandTextByDocID_XIS(lpszItemData, sDocID, sCommandText);

						if (!sCommandText.IsEmpty()) {
							pQueryParams = new CQueryParams(lpszItemData, sCommandText);
							lstQueryParams.push_back(pQueryParams);
						}
					}
				}
			}
		}
		else { // Search From HCS
			// 从平台查找

			if (!sOrderNo.IsEmpty())
				GetWhereExpByOrderNo_HCS(sSystemCode, sOrderNo, sWhereExp);
			else if (!sInpatientNo.IsEmpty())
				GetWhereExpByInpatientNoOrOutpatientNo_HCS(sSystemCode, sInpatientNo, _T(""), bIgnoreStudyDate ? _T("") : sDtFromX, bIgnoreStudyDate ? _T("") : sDtToX, sWhereExp);
			else if (!sOutpatientNo.IsEmpty())
				GetWhereExpByInpatientNoOrOutpatientNo_HCS(sSystemCode, _T(""), sOutpatientNo, bIgnoreStudyDate ? _T("") : sDtFromX, bIgnoreStudyDate ? _T("") : sDtToX, sWhereExp);
			else if (!sPatientID.IsEmpty())
				GetWhereExpByPatientID_HCS(sSystemCode, sPatientID, bIgnoreStudyDate ? _T("") : sDtFromX, bIgnoreStudyDate ? _T("") : sDtToX, sWhereExp);
			else if (!sDocID.IsEmpty())
				GetWhereExpByDocID_HCS(sSystemCode, sDocID, sWhereExp);

			
			if (!sWhereExp.IsEmpty()) {

				sCommandText.Format(_T("SELECT * FROM %s WHERE %s %s ORDER BY STUDYDATE DESC, STUDYTIME, PATIENTNAME"), 
					pConfig->GetMedicalResultTableName(),
					sWhereExp, sHospitalIDExp);

				pQueryParams = new CQueryParams(lpszSystemCode, sCommandText);
				lstQueryParams.push_back(pQueryParams);
			}
		}
	}
	else {
	
		bInteractived = TRUE;

		nItemIndex = m_cboFieldName.GetCurSel();
		pFieldInfo = (SearchFieldInfoStruct *) m_cboFieldName.GetItemData(nItemIndex);		
		
		sFieldName = pFieldInfo->lpszFieldName;
		sFieldName = sFieldName.Trim();
		
		m_edtFieldValue.GetWindowText(sFieldValue);
		sFieldValue = sFieldValue.Trim();

		nItemIndex = m_cboExamType.GetCurSel();
		lpszSystemCode = (const TCHAR *) m_cboExamType.GetItemData(nItemIndex); 

		if (!bSearchFromHCS) {

			for (ni = 0; ni < m_cboExamType.GetCount(); ni ++ ) {

				lpszItemData = (const TCHAR *) m_cboExamType.GetItemData(ni);

				sCommandText = _T("");
			
				if (_tcsicmp(lpszItemData, _T("All")) != 0) {
					if ((_tcsicmp(lpszSystemCode, lpszItemData) == 0) || (_tcsicmp(lpszSystemCode, _T("All")) == 0)) {
					
						if (sFieldName.IsEmpty()) 
							GetCommandTextByReportDate_XIS(lpszItemData, sDtFrom, sDtTo, sCommandText);
						else if (sFieldName.CompareNoCase(_T("OrderNo")) == 0)
							GetCommandTextByOrderNo_XIS(lpszItemData, sFieldValue, sCommandText);
						else if (sFieldName.CompareNoCase(_T("DocId")) == 0) {
							nRecentListFieldTag = 5;
							GetCommandTextByDocID_XIS(lpszItemData, sFieldValue, sCommandText);
						}
						else if (sFieldName.CompareNoCase(_T("PatientID")) == 0)
							GetCommandTextByPatientID_XIS(lpszItemData, sFieldValue,  sDtFrom, sDtTo, sCommandText);
							//GetCommandTextByPatientID_XIS(lpszItemData, sFieldValue, bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sCommandText);
						else if (sFieldName.CompareNoCase(_T("PatientName")) == 0) {
							nRecentListFieldTag = 3;
							GetCommandTextByPatientName_XIS(lpszItemData, sFieldValue, sDtFrom, sDtTo, sCommandText);
							//GetCommandTextByPatientName_XIS(lpszItemData, sFieldValue, bIgnoreStudyDate? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sCommandText);
						}
						else if (sFieldName.CompareNoCase(_T("InpatientNo")) == 0) {
							nRecentListFieldTag = 4;
							GetCommandTextByInpatientNoOrOutpatientNo_XIS(lpszItemData, sFieldValue, _T(""), sDtFrom, sDtTo, sCommandText);
							//GetCommandTextByInpatientNoOrOutpatientNo_XIS(lpszItemData, sFieldValue, _T(""), bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sCommandText);
						}
						else if (sFieldName.CompareNoCase(_T("OutpatientNo")) == 0)
							GetCommandTextByInpatientNoOrOutpatientNo_XIS(lpszItemData, _T(""), sFieldValue, bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sCommandText);
						else if (sFieldName.CompareNoCase(_T("RefDoctorName")) == 0) {
							nRecentListFieldTag = 2;
							GetCommandTextByRefDoctor_XIS(lpszItemData, sFieldValue, sDtFrom, sDtTo, sCommandText);
						}
						else if (sFieldName.CompareNoCase(_T("RefDeptName")) == 0) {
							nRecentListFieldTag = 1;			
							GetCommandTextByRefDept_XIS(lpszItemData, sFieldValue, sDtFrom, sDtTo, sCommandText);
						}

						if (!sCommandText.IsEmpty()) {
							pQueryParams = new CQueryParams(lpszItemData, sCommandText);
							lstQueryParams.push_back(pQueryParams);
						}
					}
				}
			}
		}
		else {

			sCommandText = _T("");
			sWhereExp = _T("");

			if (sFieldName.IsEmpty()) 
				GetWhereExpByReportDate_HCS(lpszSystemCode, sDtFrom, sDtTo, sWhereExp);
			else if (sFieldName.CompareNoCase(_T("OrderNo")) == 0)
				GetWhereExpByOrderNo_HCS(lpszSystemCode, sFieldValue, sWhereExp);
			else if (sFieldName.CompareNoCase(_T("DocId")) == 0) {
				nRecentListFieldTag = 5;
				GetWhereExpByDocID_HCS(lpszSystemCode, sFieldValue, sWhereExp);
			}
			else if (sFieldName.CompareNoCase(_T("PatientID")) == 0)
				GetWhereExpByPatientID_HCS(lpszSystemCode, sFieldValue,  sDtFrom, sDtTo, sWhereExp);
				//GetWhereExpByPatientID_HCS(lpszSystemCode, sFieldValue, bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sWhereExp);
			else if (sFieldName.CompareNoCase(_T("PatientName")) == 0) {
				nRecentListFieldTag = 3;
				GetWhereExpByPatientName_HCS(lpszSystemCode, sFieldValue,  sDtFrom, sDtTo, sWhereExp);
				//GetWhereExpByPatientName_HCS(lpszSystemCode, sFieldValue, bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sWhereExp);
			}
			else if (sFieldName.CompareNoCase(_T("InpatientNo")) == 0) {
				nRecentListFieldTag = 4;
				GetWhereExpByInpatientNoOrOutpatientNo_HCS(lpszSystemCode, sFieldValue, _T(""), sDtFrom, sDtTo, sWhereExp);
				//肇庆要加上时间过滤；
				//GetWhereExpByInpatientNoOrOutpatientNo_HCS(lpszSystemCode, sFieldValue, _T(""), bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sWhereExp);
			}
			else if (sFieldName.CompareNoCase(_T("OutpatientNo")) == 0)
				GetWhereExpByInpatientNoOrOutpatientNo_HCS(lpszSystemCode, _T(""), sFieldValue, bIgnoreStudyDate ? _T("") : sDtFrom, bIgnoreStudyDate ? _T("") : sDtTo, sWhereExp);
			else if (sFieldName.CompareNoCase(_T("RefDoctorName")) == 0) {
				nRecentListFieldTag = 2;			
				GetWhereExpByRefDoctor_HCS(lpszSystemCode, sFieldValue, sDtFrom, sDtTo, sWhereExp);
			}
			else if (sFieldName.CompareNoCase(_T("RefDeptName")) == 0) {
				nRecentListFieldTag = 1;
				GetWhereExpByRefDept_HCS(lpszSystemCode, sFieldValue, sDtFrom, sDtTo, sWhereExp);
			}

			if (!sWhereExp.IsEmpty()) {

				sCommandText.Format(_T("SELECT * FROM %s WHERE %s %s ORDER BY STUDYDATE DESC, PATIENTNAME"), 
					pConfig->GetMedicalResultTableName(),
					sWhereExp,
					sHospitalIDExp);

				pQueryParams = new CQueryParams(lpszSystemCode, sCommandText);
				lstQueryParams.push_back(pQueryParams);
			}
		}
	}


	if (lstQueryParams.size() == 0) {
		return;
	}

	for (itQueryParam = lstQueryParams.begin(); itQueryParam != lstQueryParams.end(); itQueryParam ++) {
		lpszSystemCode = (*itQueryParam)->m_sSystemCode;
		lpszCommandText = (*itQueryParam)->m_sCommandText; 
		
		try {
			if (con.isConnected())
				con.Disconnect();

			if (!bSearchFromHCS)
				con.Connect(pConfig->GetXISDBName(lpszSystemCode), pConfig->GetXISDBUserId(lpszSystemCode), pConfig->GetXISDBPassword(lpszSystemCode), pConfig->GetXISDBType(lpszSystemCode));
			else 
				con.Connect(pConfig->GetCloudERVDBName(), pConfig->GetCloudERVDBUserId(), pConfig->GetCloudERVDBPassword(), pConfig->GetCloudERVDBType());


			cmd.setConnection(&con);
			cmd.setCommandText(SAString(lpszCommandText));
			cmd.Execute();

			while (cmd.FetchNext()) {
			
				pMedicalResultObject = new CMedicalResultObject();
				
				if (!bSearchFromHCS) {
					if (_tcsicmp(lpszSystemCode, _T("UIS")) == 0)
						pMedicalResultObject->ReadFromUIS(cmd);
					else if (_tcsicmp(lpszSystemCode, _T("PIS")) == 0)
						pMedicalResultObject->ReadFromPIS_1(cmd);
					else if (_tcsicmp(lpszSystemCode, _T("RIS")) == 0)
						pMedicalResultObject->ReadFromRIS(cmd);
					else if (_tcsicmp(lpszSystemCode, _T("PET")) == 0)
						pMedicalResultObject->ReadFromPET(cmd);
					else if (_tcsicmp(lpszSystemCode, _T("EIS")) == 0)
						pMedicalResultObject->ReadFromEIS(cmd);

				}
				else {
					pMedicalResultObject->ReadFromHCS(cmd);
				}

				pMedicalResultObjectList->push_back(pMedicalResultObject);
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
	}

	// 记忆输入的送检科室和送检医生
	if (nRecentListFieldTag > 0) 
		pApp->InsertNewItemToRecentDeptOrRefDoctorList(nRecentListFieldTag, sFieldValue);

	m_ctlResult.SetVirtualMode(new  CMedicalResultRecord(pMedicalResultObjectList, pMedicalResultObjectList->size()), pMedicalResultObjectList->size());
	m_ctlResult.Populate();

	nIndex = GetDocument()->GetIndexByStudyGUID(sOldStudyGUID);
	if (nIndex == -1)
		nIndex = 0;

	pCurrentMedicalResultObject = NULL;
	if (nIndex >= 0 && nIndex < pMedicalResultObjectList->size()) {
		pCurrentMedicalResultObject = (*pMedicalResultObjectList)[nIndex];		
			
		pReportRow = m_ctlResult.GetRows()->GetAt(nIndex);
		m_ctlResult.SetFocusedRow(pReportRow);
	}

	GetDocument()->SetCurrentMedicalResultObject(pCurrentMedicalResultObject);

	GetDocument()->UpdateAllViews(NULL);
	m_bDataLoading = FALSE;

	if (!bInteractived  && (pMedicalResultObjectList->size() >= 1) && pApp->IsAutoLoadImagesForSingleResultOfCmdLineMode()) {
		ViewImageMode1(pApp->GetDefaultCMoveMode());
	}

}



void CPublishedReportView::ViewOriginalReport()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CMedicalResultObject *pObj = NULL;

	pObj = GetDocument()->GetCurrentMedicalResultObject();
	
	pApp->ViewOriginalReport(pObj);	
}

void CPublishedReportView::ViewImageMode1(int nTag)
{
	CString sStudyGUID(""), sSystemCode(""), sAccessionNo(""), sStudyUID(""), sPatientName(""), sDocId(""), sPatientSex(""), sStudyDate("");
	CMedicalResultObject *pCurrentObj;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	bool bLegacy = false;
	CWaitCursor wait;

	pCurrentObj = GetDocument()->GetCurrentMedicalResultObject();

	if (pCurrentObj) {

		sStudyGUID = pCurrentObj->m_sStudyGUID;

		sSystemCode = pCurrentObj->m_sSystemCode;
		sAccessionNo = pCurrentObj->m_sAccessionNo;
		sStudyUID = pCurrentObj->m_sStudyUID;
		sPatientName = pCurrentObj->m_sPatientName;
		sDocId = pCurrentObj->m_sDocID;
		sPatientSex = pCurrentObj->m_sPatientSex;
		sStudyDate = pCurrentObj->m_sStudyDate;
		bLegacy	= (pCurrentObj->m_nLegacyFlag == 1);

		pApp->ViewImageMode1(nTag, sStudyGUID, sSystemCode, sAccessionNo, sStudyUID, sPatientName, sDocId, sPatientSex, sStudyDate, bLegacy);
	}
}

void CPublishedReportView::ViewImageMode2()
{
	CString sStudyUID("");
	CMedicalResultObject *pCurrentObj;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	pCurrentObj = GetDocument()->GetCurrentMedicalResultObject();

	if (pCurrentObj) {
		sStudyUID = pCurrentObj->m_sStudyUID;
		pApp->ViewImageMode2(sStudyUID);
	}
}


void CPublishedReportView::ViewImageMode3(int nTag)
{
	CString sStudyUID(""), sAccessionNo(""), sDocId(""), sStudyDate("");
	CMedicalResultObject *pCurrentObj;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	pCurrentObj = GetDocument()->GetCurrentMedicalResultObject();

	if (pCurrentObj) {
		sStudyUID = pCurrentObj->m_sStudyUID;
		sAccessionNo = pCurrentObj->m_sAccessionNo;
		sDocId = pCurrentObj->m_sDocID;
		sStudyDate = pCurrentObj->m_sStudyDate;
		pApp->ViewImageMode3(nTag, sStudyUID, sAccessionNo, sDocId, sStudyDate);
	}
}



void CPublishedReportView::OnUpdateActions(CCmdUI *pCmdUI)
{
	BOOL bEnabled = FALSE, bChecked = FALSE, bCheckedValid = FALSE;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CMedicalResultObject *pCurrentMedicalResultObj = NULL;
	SearchFieldInfoStruct *pFieldInfo;
	CString sFieldValue("");


	switch(pCmdUI->m_nID) {
	case ID_MEDICALRESULT_FIND:
		bEnabled = (m_cboExamType.GetCurSel() >= 0) && (pApp->GetValidServerInfoCount() > 0);
		if (bEnabled) {
			pFieldInfo = (SearchFieldInfoStruct *) m_cboFieldName.GetItemData(m_cboFieldName.GetCurSel());
			// 检查项目为无时，　送入的字段名是一个空格，　空的字段为结束标记
			if (pFieldInfo && _tcslen(pFieldInfo->lpszFieldName) > 1) {
				m_edtFieldValue.GetWindowText(sFieldValue);
				bEnabled = !sFieldValue.IsEmpty();
			}
		}
		break;
	case ID_MEDICALRESULT_VIEWIMAGES:
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("UIS")) == 0  
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("EIS")) == 0
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PISXXX")) == 0
						|| (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && (IsDicomRetrieveAvailable() || IsImageViewerAvailable() || IsWebViewerAvailable()) ) 
						|| (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && (IsDicomRetrieveAvailable() || IsImageViewerAvailable() || IsWebViewerAvailable()) ) 
						);
		}

		break;
	case ID_VIEWIMAGE_MODE1A:	// 简易浏览器(按检查实例UID查找）
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("UIS")) == 0  
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("EIS")) == 0
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PIS")) == 0
						|| (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable()) 
						|| (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable()) 
						);
		}

		bCheckedValid = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable())
			||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable()) ;
		bChecked = (pApp->GetDefaultCMoveMode() == 1);
		
		break;

	case ID_VIEWIMAGE_MODE1B:	// 简易浏览器（按存取号查找),只适用于放射
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable())
			||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable());
		}

		bCheckedValid = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable())
		||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable()) ;
		bChecked = (pApp->GetDefaultCMoveMode() == 2);

		break;

	case ID_VIEWIMAGE_MODE1C:	// 简易浏览器（按影像号+检查日期查找）,只适用于放射
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable())
				||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable());
		}

		bCheckedValid = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable())
			||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable());
		bChecked = (pApp->GetDefaultCMoveMode() == 3);

		break;

	case ID_VIEWIMAGE_MODE1D:	// 简易浏览器（按影像号）,只适用于放射
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable());
		}

		bCheckedValid = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && IsDicomRetrieveAvailable() && !IsImageViewerAvailable()) ;
		bChecked = (pApp->GetDefaultCMoveMode() == 4);

		break;

	case ID_VIEWIMAGE_MODE2:	// Web浏览器
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0)||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0);
			if (bEnabled) 
				bEnabled = (!pCurrentMedicalResultObj->m_sStudyUID.IsEmpty() && IsWebViewerAvailable() );
		}
		break;
	case ID_VIEWIMAGE_MODE3A:	// 专业浏览器_检查号
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0)||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0);
			if (bEnabled)
				bEnabled = (!pCurrentMedicalResultObj->m_sStudyUID.IsEmpty() && IsImageViewerAvailable() );
		}

		bCheckedValid = bEnabled;
		bChecked = (pApp->GetDefaultCMoveMode() == 1);

		break;

	case ID_VIEWIMAGE_MODE3B:	// 专业浏览器_存取号
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0)||(pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0);
			if (bEnabled)
				bEnabled = (!pCurrentMedicalResultObj->m_sStudyUID.IsEmpty() && IsImageViewerAvailable() );
		}

		bCheckedValid = bEnabled;
		bChecked = (pApp->GetDefaultCMoveMode() == 2);

		break;

	case ID_VIEWIMAGE_MODE3C:	// 专业浏览器_影像号+检查日期
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0)|| (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0);
			if (bEnabled)
				bEnabled = (!pCurrentMedicalResultObj->m_sStudyUID.IsEmpty() && IsImageViewerAvailable() );
		}

		bCheckedValid = bEnabled;
		bChecked = (pApp->GetDefaultCMoveMode() == 3);

		break;

	case ID_VIEWIMAGE_MODE3D:	// 专业浏览器_影像号
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0);
			if (bEnabled)
				bEnabled = (!pCurrentMedicalResultObj->m_sStudyUID.IsEmpty() && IsImageViewerAvailable() );
		}

		bCheckedValid = bEnabled;
		bChecked = (pApp->GetDefaultCMoveMode() == 4);

		break;
	case ID_VIEWREPORT_ORIGINAL:	// 查看原始上传的报告
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (!pCurrentMedicalResultObj->m_sReportFileUrls.IsEmpty());
			if (!bEnabled) 
				bEnabled = (pCurrentMedicalResultObj->m_bFromXIS && !pCurrentMedicalResultObj->m_bReportFileUrlsSearched);
		}		
		break;
	case ID_MEDICALRESULT_SAVEPOS1:		// 保存列表列宽
		bEnabled = m_bReportColumnWidthChanged && (pApp->GetReportResultGridColumnWidthSaveMode() == 0);
		break;
	case ID_MEDICALRESULT_CLOSE:
		bEnabled = TRUE;
		break;
	}

	if (bCheckedValid)
		pCmdUI->SetCheck(bChecked ? 1 : 0);

	pCmdUI->Enable(bEnabled);
}



void CPublishedReportView::OnExecuteActions(UINT nCommandID)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	const TCHAR *szSectionName1 = _T("Settings_GUI");
	const TCHAR *szKeyName1 = _T("HeaderWidthOfReportResultControl");

	switch(nCommandID) {
	case ID_MEDICALRESULT_FIND:
		DoQuery(_T(""), _T(""), _T(""), _T(""), _T(""), _T(""));
		break;
	case ID_MEDICALRESULT_VIEWIMAGES:
		ViewImageMode1(pApp->GetDefaultCMoveMode());
		break;
	case ID_VIEWIMAGE_MODE1A:
		ViewImageMode1(1);
		break;
	case ID_VIEWIMAGE_MODE1B:
		ViewImageMode1(2);
		break;
	case ID_VIEWIMAGE_MODE1C:
		ViewImageMode1(3);
		break;
	case ID_VIEWIMAGE_MODE1D:
		ViewImageMode1(4);
		break;
	case ID_VIEWIMAGE_MODE2:
		ViewImageMode2();
		break;
	case ID_VIEWIMAGE_MODE3A:
		ViewImageMode3(1);
		break;
	case ID_VIEWIMAGE_MODE3B:
		ViewImageMode3(2);
		break;
	case ID_VIEWIMAGE_MODE3C:
		ViewImageMode3(3);
		break;
	case ID_VIEWIMAGE_MODE3D:
		ViewImageMode3(4);
		break;
	case ID_VIEWREPORT_ORIGINAL:
		ViewOriginalReport();
		break;
	case ID_MEDICALRESULT_SAVEPOS1:
		SaveHeaderWidthOfReportControl(m_ctlResult, szSectionName1, szKeyName1);
		break;
	case ID_MEDICALRESULT_CLOSE:
		GetParentFrame()->SendMessage(WM_CLOSE);
		break;
	}
}


