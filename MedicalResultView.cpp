// MedicalResultView.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "MedicalResultView.h"


extern BOOL IsImageViewerAvailable();
extern BOOL IsWebViewerAvailable();
extern BOOL IsDicomRetrieveAvailable();


// CMedicalResultView

IMPLEMENT_DYNCREATE(CMedicalResultView, CXTPResizeFormView)

CMedicalResultView::CMedicalResultView()
	: CXTPResizeFormView(CMedicalResultView::IDD)
{
	m_bInitialized = FALSE;
	m_bPositiveFlag = FALSE;
	m_bCriticalFlag = FALSE;
}

CMedicalResultView::~CMedicalResultView()
{
}

void CMedicalResultView::DoDataExchange(CDataExchange* pDX)
{
	CXTPResizeFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXT_TITLE,			m_txtTitle);
	DDX_Control(pDX, IDC_TXT_PATIENTNAME,	m_txtPatientName);
	DDX_Control(pDX, IDC_TXT_PATIENTSEX,	m_txtPatientSex);
	DDX_Control(pDX, IDC_TXT_PATIENTAGE,	m_txtPatientAge);
	DDX_Control(pDX, IDC_TXT_DOCID,			m_txtDocID);
	DDX_Control(pDX, IDC_TXT_INPATIENTNO,	m_txtInpatientNo);
	DDX_Control(pDX, IDC_TXT_BEDNO,			m_txtBedNo);
	DDX_Control(pDX, IDC_TXT_STUDYDATE,		m_txtStudyDate);
	DDX_Control(pDX, IDC_TXT_STUDYDETAILS,	m_txtStudyDetails);
	DDX_Control(pDX, IDC_EDT_FINDINGS,		m_edtFindings);
	DDX_Control(pDX, IDC_EDT_CONCLUSIONS,	m_edtConclusions);
	DDX_Control(pDX, IDC_TXT_REPORTDATE,	m_txtReportDate);
	DDX_Control(pDX, IDC_TXT_REPORTDOCTORNAME,m_txtReportDoctorName);
	DDX_Control(pDX, IDC_TXT_REVIEWDATE,	m_txtReviewDate);
	DDX_Control(pDX, IDC_TXT_REVIEWDOCTORNAME,m_txtReviewDoctorName);
	DDX_Control(pDX, IDC_TXT_POSITIVEFLAG, m_txtPositiveFlag);
	DDX_Control(pDX, IDC_TXT_CRITICALFLAG, m_txtCriticalFlag);
	DDX_Control(pDX, IDC_TXT_WARNCONTENT, m_txtWarnContent);
	DDX_Control(pDX, IDC_TXT_INFECTIOUSFLAG, m_txtInfectiousFlag);
	DDX_Control(pDX, IDC_TXT_ENHANCEDFLAG, m_txtEnhancedFlag);
}

BEGIN_MESSAGE_MAP(CMedicalResultView, CXTPResizeFormView)
	ON_UPDATE_COMMAND_UI_RANGE(1000, 65535, OnUpdateActions)
	ON_COMMAND_RANGE(1000, 65535, OnExecuteActions)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMedicalResultView diagnostics

#ifdef _DEBUG
void CMedicalResultView::AssertValid() const
{
	CXTPResizeFormView::AssertValid();
}

CPublishedReportDoc* CMedicalResultView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPublishedReportDoc)));
	return (CPublishedReportDoc*)m_pDocument;
}

#ifndef _WIN32_WCE
void CMedicalResultView::Dump(CDumpContext& dc) const
{
	CXTPResizeFormView::Dump(dc);
}
#endif
#endif //_DEBUG


void CMedicalResultView::PopulateFields()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	const TCHAR *lpszTitle = _T("影像检查报告");
	CString sTitle;

	CMedicalResultObject *pCurrentMedicalResultObject = GetDocument()->GetCurrentMedicalResultObject();

	if (pCurrentMedicalResultObject == NULL) {

		m_txtTitle.SetWindowText(lpszTitle);
		m_txtPatientName.SetWindowText(_T(""));
		m_txtPatientSex.SetWindowText(_T(""));
		m_txtPatientAge.SetWindowText(_T(""));
		m_txtDocID.SetWindowText(_T(""));
		m_txtInpatientNo.SetWindowText(_T(""));
		m_txtBedNo.SetWindowText(_T(""));

		m_txtStudyDate.SetWindowText(_T(""));
		m_txtStudyDetails.SetWindowText(_T(""));
		m_edtFindings.SetWindowText(_T(""));
		m_edtConclusions.SetWindowText(_T(""));
		m_txtReportDate.SetWindowText(_T(""));
		m_txtReportDoctorName.SetWindowText(_T(""));
		m_txtReviewDate.SetWindowText(_T(""));
		m_txtReviewDoctorName.SetWindowText(_T(""));
		m_txtPositiveFlag.SetWindowText(_T(""));
		m_txtCriticalFlag.SetWindowText(_T(""));
		m_txtWarnContent.SetWindowText(_T(""));

		m_bPositiveFlag = FALSE;
		m_bCriticalFlag = FALSE;
		m_bInfectiousFlag = FALSE;
		m_bEnhancedFlag = FALSE;

	}
	else {

		m_bPositiveFlag = (pCurrentMedicalResultObject->m_nPositiveFlag == 1);
		m_bCriticalFlag = (pCurrentMedicalResultObject->m_nCriticalFlag == 1);
		m_bInfectiousFlag = (pCurrentMedicalResultObject->m_nInfectiousFlag == 1);
		m_bEnhancedFlag = (pCurrentMedicalResultObject->m_nEnhancedFlag == 1);

		if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("UIS")) == 0)
			sTitle.Format(_T("%s%s"), _T("超声"), lpszTitle);
		else if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("EIS")) == 0)
			sTitle.Format(_T("%s%s"), _T("内镜"), lpszTitle);
		else if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("RIS")) == 0)
			sTitle.Format(_T("%s%s"), _T("放射"), lpszTitle);
		else if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("PET")) == 0)
			sTitle.Format(_T("%s%s"), _T("PET-CT"), lpszTitle);
		else if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("PIS")) == 0)
			sTitle.Format(_T("%s%s"), _T("病理"), lpszTitle);
		else
			sTitle = lpszTitle;

		m_txtTitle.SetWindowText(sTitle);

		m_txtPatientName.SetWindowText(pCurrentMedicalResultObject->m_sPatientName);
		m_txtPatientSex.SetWindowText(pCurrentMedicalResultObject->m_sPatientSex);
		m_txtPatientAge.SetWindowText(pCurrentMedicalResultObject->m_sPatientAge);
		m_txtDocID.SetWindowText(pCurrentMedicalResultObject->m_sDocID);
		m_txtInpatientNo.SetWindowText(pCurrentMedicalResultObject->m_sInpatientNo);
		m_txtBedNo.SetWindowText(pCurrentMedicalResultObject->m_sBedNo);

		m_txtStudyDate.SetWindowText(pCurrentMedicalResultObject->m_sStudyDate);
		m_txtStudyDetails.SetWindowText(pCurrentMedicalResultObject->m_sStudyDetails);

		m_txtWarnContent.SetWindowText(pCurrentMedicalResultObject->m_sWarnContent);

		if (pCurrentMedicalResultObject->m_bFinalReport) {
			m_edtFindings.SetWindowText(pCurrentMedicalResultObject->m_sFindings);
			m_edtConclusions.SetWindowText(pCurrentMedicalResultObject->m_sConclusions);
		}
		else {
			m_edtFindings.SetWindowText(L"目前此报告还不可用，报告医生还没有提交此报告！");
			m_edtConclusions.SetWindowText(L"目前此报告还不可用，报告医生还没有提交此报告！");
		}

		m_txtReportDate.SetWindowText(pCurrentMedicalResultObject->m_sReportDate);
		m_txtReportDoctorName.SetWindowText(pCurrentMedicalResultObject->m_sReportDoctorName);
		m_txtReviewDate.SetWindowText(pCurrentMedicalResultObject->m_sReviewDate);
		m_txtReviewDoctorName.SetWindowText(pCurrentMedicalResultObject->m_sReviewDoctorName);

		if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("PIS")) == 0) {
			m_txtPositiveFlag.SetWindowText(_T(""));
			m_txtCriticalFlag.SetWindowText(_T(""));
		}
		else {
			if (pCurrentMedicalResultObject->m_sSystemCode.CompareNoCase(_T("EIS")) == 0) {
				if (pApp->PositiveFlagValidForEIS())
					m_txtPositiveFlag.SetWindowText(pCurrentMedicalResultObject->m_nPositiveFlag == 1 ? _T("阳性") : _T("阴性"));
				else 
					m_txtPositiveFlag.SetWindowText(_T(" "));
			}
			else {
				m_txtPositiveFlag.SetWindowText(pCurrentMedicalResultObject->m_nPositiveFlag == 1 ? _T("阳性") : _T("阴性"));
			}

			m_txtCriticalFlag.SetWindowText(pCurrentMedicalResultObject->m_nCriticalFlag == 1 ? _T("危急") : _T(""));
			m_txtInfectiousFlag.SetWindowText(pCurrentMedicalResultObject->m_nInfectiousFlag == 1 ? _T("是") : _T(" "));
			m_txtEnhancedFlag.SetWindowText(pCurrentMedicalResultObject->m_nEnhancedFlag == 1 ? _T("是") : _T(" "));
		}
	}

	Invalidate();
}

// CMedicalResultView message handlers
void CMedicalResultView::OnInitialUpdate()
{
	CString sPatientNameLabel("");
	CWnd *pWnd;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	CXTPResizeFormView::OnInitialUpdate();

	sPatientNameLabel = pApp->GetPatientNameLabel();

	SetResize(IDC_TXT_TITLE, SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_LINE1, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_LBL_PATIENTNAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_PATIENTNAME, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_PATIENTSEX, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_PATIENTSEX, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_PATIENTAGE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_DOCID, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_DOCID, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_INPATIENTNO, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_INPATIENTNO, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_STUDYDATE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_STUDYDATE, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LBL_STUDYDETAILS, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TXT_STUDYDETAILS, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_LBL_FINDINGS, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_EDT_FINDINGS, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LBL_CONCLUSIONS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDT_CONCLUSIONS, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_LBL_REPORTDATE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_REPORTDATE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_LBL_REPORTDOCTORNAME, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_REPORTDOCTORNAME, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_LBL_REVIEWDATE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_REVIEWDATE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_LBL_REVIEWDOCTORNAME, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_REVIEWDOCTORNAME, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_LBL_WARNCONTENT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_TXT_WARNCONTENT, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_LBL_POSITIVEFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TXT_POSITIVEFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_LBL_CRITICALFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TXT_CRITICALFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_LBL_INFECTIOUSFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TXT_INFECTIOUSFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_LBL_ENHANCEDFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TXT_ENHANCEDFLAG, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	PopulateFields();

	pWnd = GetDlgItem(IDC_LBL_PATIENTNAME);
	if (pWnd) 
		pWnd->SetWindowText(sPatientNameLabel);

	m_bInitialized = TRUE;
}

void CMedicalResultView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	if (m_bInitialized)
		PopulateFields();
}

HBRUSH CMedicalResultView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CXTPResizeFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	CString sValue("");

	if (pWnd != NULL) {
			
		if (pWnd->GetDlgCtrlID() == IDC_TXT_TITLE) {
			if (m_bPositiveFlag || m_bCriticalFlag)
				pDC->SetTextColor(RGB(255, 0, 0));
			else
				pDC->SetTextColor(RGB(0, 0, 255));
		}

		if (pWnd->GetDlgCtrlID() == IDC_TXT_POSITIVEFLAG) {
			pWnd->GetWindowText(sValue);
			if (sValue.Compare(_T("阳性")) == 0) 
				pDC->SetTextColor(RGB(255, 0, 0));
		}

		if (pWnd->GetDlgCtrlID() == IDC_TXT_CRITICALFLAG) {
			pWnd->GetWindowText(sValue);
			if (sValue.Compare(_T("危急")) == 0) 
				pDC->SetTextColor(RGB(255, 0, 0));
		}
	}

	return hbr;
}


void CMedicalResultView::OnUpdateActions(CCmdUI *pCmdUI)
{
	BOOL bEnabled = FALSE;
	CMedicalResultObject *pCurrentMedicalResultObj = NULL;

	switch(pCmdUI->m_nID) {
	case ID_MEDICALRESULT_VIEWIMAGES:
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("UIS")) == 0  
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("EIS")) == 0
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PIS")) == 0
						|| pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("PET")) == 0
						|| (pCurrentMedicalResultObj->m_sSystemCode.CompareNoCase(_T("RIS")) == 0 && (IsDicomRetrieveAvailable() || IsImageViewerAvailable() || IsWebViewerAvailable()) ) 
						);						
		}
		break;
	case ID_VIEWREPORT_ORIGINAL:
		pCurrentMedicalResultObj = GetDocument()->GetCurrentMedicalResultObject();
		bEnabled = (pCurrentMedicalResultObj != NULL);
		if (bEnabled) {
			bEnabled = (!pCurrentMedicalResultObj->m_sReportFileUrls.IsEmpty());
			if (!bEnabled) 
				bEnabled = (pCurrentMedicalResultObj->m_bFromXIS && !pCurrentMedicalResultObj->m_bReportFileUrlsSearched);
		}		
		break;
	case ID_MEDICALRESULT_CLOSE:
		bEnabled = TRUE;
		break;
	}

	pCmdUI->Enable(bEnabled);
}

void CMedicalResultView::OnExecuteActions(UINT nCommandID)
{
	CString sStudyGUID(""), sSystemCode(""), sAccessionNo(""), sStudyUID(""), sPatientName(""), sDocId(""), sPatientSex(""), sStudyDate("");
	CMedicalResultObject *pCurrentObj;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	bool bLegacy = false;

	switch(nCommandID) {
	case ID_MEDICALRESULT_VIEWIMAGES:
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
			bLegacy		= (pCurrentObj->m_nLegacyFlag == 1);

			pApp->ViewImageMode1(pApp->GetDefaultCMoveMode(), sStudyGUID, sSystemCode, sAccessionNo, sStudyUID, sPatientName, sDocId, sPatientSex, sStudyDate, bLegacy);
		}
		break;
	case ID_VIEWREPORT_ORIGINAL:
		pCurrentObj = GetDocument()->GetCurrentMedicalResultObject();	
		pApp->ViewOriginalReport(pCurrentObj);	
		break;
	case ID_MEDICALRESULT_CLOSE:
		GetParentFrame()->SendMessage(WM_CLOSE);
		break;
	}
}