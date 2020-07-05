// PdfReportFrm.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "PublishedReportFrm.h"
#include "PublishedReportView.h"
#include "MedicalResultView.h"

// CPublishedReportFrame

IMPLEMENT_DYNCREATE(CPublishedReportFrame, CChildFrame)

CPublishedReportFrame::CPublishedReportFrame()
{
	m_bSplitterColumn0WidthChanged = FALSE;
}

CPublishedReportFrame::~CPublishedReportFrame()
{
}


BEGIN_MESSAGE_MAP(CPublishedReportFrame, CChildFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


const CChildFrame::CaptionToolBarButtonInfo g_lstButtonsOfPdfReport[] = {
	{ID_MEDICALRESULT_FIND,			TRUE},
	{ID_MEDICALRESULT_VIEWIMAGES,	TRUE},
	{ID_VIEWREPORT_ORIGINAL,		TRUE},
	{ID_MEDICALRESULT_SAVEPOS1,		TRUE},
	{ID_MEDICALRESULT_CLOSE,		TRUE},
	{0, FALSE}
};

CChildFrame::CaptionToolBarButtonInfo * CPublishedReportFrame::GetCaptionToolBarButtonInfoList() 
{ 
	return (CChildFrame::CaptionToolBarButtonInfo *)g_lstButtonsOfPdfReport;
}

// CPublishedReportFrame message handlers

int CPublishedReportFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CChildFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	
  // Create ToolBar 
     CXTPToolBar* lpxtpToolBar = (CXTPToolBar*)m_lpxtpCmdBars->Add( L"Standard", xtpBarTop ) ; 

     if( ! lpxtpToolBar || !lpxtpToolBar->LoadToolBar( IDR_PUBLISHEDREPORT )) 
     { 
		 AfxMessageBox(L"Failed to create toolbar");

		 TRACE0( "Failed to create toolbar\n" ) ; 
          return -1 ; 
     } 

     lpxtpToolBar->SetShowGripper( FALSE ) ; 
     lpxtpToolBar->ShowExpandButton( FALSE ) ; 
     lpxtpToolBar->EnableDocking( xtpFlagAlignBottom ) ; 
	 

	return 0;
}


BOOL CPublishedReportFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	const TCHAR *szSectionName1 = _T("Settings_GUI");
	const TCHAR *szKeyName1 = _T("PublishedReportFrameSplitterColumn0WidthRatio");
	CString sRatio;
	CRect r;
	int nIdealWidth = 0;

	sRatio = pApp->GetProfileString(szSectionName1, szKeyName1, _T("0.4"));

	m_fSplitterColumn0Ratio = _ttof(sRatio);
	if (m_fSplitterColumn0Ratio <= 0 || m_fSplitterColumn0Ratio >= 1.0)
		m_fSplitterColumn0Ratio = 0.4;


	// Create the splitter window with two columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0,0, RUNTIME_CLASS(CPublishedReportView),
		CSize(0, 0), pContext))
	{
		TRACE0("Failed to create CView1\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0,1, RUNTIME_CLASS(CMedicalResultView),
		CSize(0, 0), pContext))
	{
		TRACE0("Failed to create CView2\n");
		return FALSE;
	}
	
	AfxGetMainWnd()->GetClientRect(&r);

	nIdealWidth = r.Width() * m_fSplitterColumn0Ratio - 15;
	if (nIdealWidth <= 20)
		nIdealWidth = 460;

	m_wndSplitter.SetColumnInfo( 0, nIdealWidth, 0 );

	return TRUE;
}


void CPublishedReportFrame::OnDestroy()
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	const TCHAR *szSectionName1 = _T("Settings_GUI");
	const TCHAR *szKeyName1 = _T("PublishedReportFrameSplitterColumn0WidthRatio");
	
	int nCol0CX, nCol0Min;
	CRect r;
	float fRatio;
	CString sRatio;

	AfxGetMainWnd()->GetClientRect(&r);
	m_wndSplitter.GetColumnInfo(0, nCol0CX, nCol0Min);
	
	fRatio = (float) (nCol0CX + 15.0) / (float) r.Width();

	if (abs(fRatio - m_fSplitterColumn0Ratio) > 0.01) {
		sRatio.Format(_T("%6.2f"), fRatio);
		pApp->WriteProfileString(szSectionName1, szKeyName1, sRatio);
	}

	CChildFrame::OnDestroy();

	// TODO: Add your message handler code here
}


