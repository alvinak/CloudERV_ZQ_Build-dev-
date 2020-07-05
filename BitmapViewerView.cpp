// BitmapViewerView.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "BitmapViewerView.h"
#include "BitmapViewerDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView

IMPLEMENT_DYNCREATE(CBitmapViewerView, CBitmapViewer)

BEGIN_MESSAGE_MAP(CBitmapViewerView, CBitmapViewer)
	ON_UPDATE_COMMAND_UI_RANGE(1000, 65535, OnUpdateActions)
	ON_COMMAND_RANGE(1000, 65535, OnExecuteActions)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView construction/destruction
 
CBitmapViewerView::CBitmapViewerView() 
   : m_bShowText(TRUE)
{
   SetBackPatternBrush(IDB_BMPVIEW_BKGROUND);
   SetHandCursor(IDC_CURSOR_HAND);
   SetHandDragCursor(IDC_CURSOR_HANDDRAG);
 }

CBitmapViewerView::~CBitmapViewerView()
{
	
}

BOOL CBitmapViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CBitmapViewer::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView drawing

void CBitmapViewerView::OnDraw(CDC* pDC)
{
	CBitmapViewer::OnDraw(pDC);
}

void CBitmapViewerView::DrawOverlay(CMemoryDC* pDCDraw, CDC* pDCView)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CString strText("");
	CBitmapViewerDoc *pDoc = GetDocument();

	// draw demo text
	if(m_bShowText) {
		int nImageCount, nImageIndex;
		CRect rcClient(0, 0, 0, 0);
		GetClientRect(rcClient);
      
		pDCDraw->SetBkMode(TRANSPARENT);
		const int x1 = rcClient.left + 10;
		const int y1 = rcClient.top + 10;
		const int x2 = rcClient.left + 10;
		const int y2 = rcClient.bottom - 25;

		strText = pApp->GetCompanyName();

		pDCDraw->SetTextColor(RGB(0, 0, 0));
		pDCDraw->TextOut(x2 + 1, y2 + 1, strText);

		pDCDraw->SetTextColor(RGB(255, 255, 255));
		pDCDraw->TextOut(x2, y2, strText);

		nImageCount = pDoc->GetImageCount();
		nImageIndex = pDoc->GetImageIndex();

		if (nImageCount > 1) {
			strText.Format(_T("%d/%d"), nImageIndex + 1, nImageCount);

			pDCDraw->SetTextColor(RGB(0, 0, 0));
			pDCDraw->TextOut(x1 + 1, y1 + 1, strText);

			pDCDraw->SetTextColor(RGB(255, 255, 255));
			pDCDraw->TextOut(x1, y1, strText);
		}
   }

}

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView diagnostics

#ifdef _DEBUG
void CBitmapViewerView::AssertValid() const
{
	CBitmapViewer::AssertValid();
}

void CBitmapViewerView::Dump(CDumpContext& dc) const
{
	CBitmapViewer::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewerView message handlers

void CBitmapViewerView::OnInitialUpdate() 
{
	CBitmapViewer::OnInitialUpdate();

	ZoomToWindow();
}

void CBitmapViewerView::OnUpdateActions(CCmdUI *pCmdUI)
{
	CBitmapViewerDoc *pDoc = GetDocument();
	BOOL bEnabled;
	switch(pCmdUI->m_nID) {
	case ID_BMPVIEWER_FITWIDTH:
		bEnabled = pDoc->IsValidImage();
		break;
	case ID_BMPVIEWER_FITWINDOW:
		bEnabled = pDoc->IsValidImage();
		break;
	case ID_BMPVIEWER_ORIGINALSIZE:
		bEnabled = pDoc->IsValidImage();
		break;
	case ID_BMPVIEWER_ZOOMIN:
		bEnabled = pDoc->IsValidImage();
		break;
	case ID_BMPVIEWER_ZOOMOUT:
		bEnabled = pDoc->IsValidImage();
		break;
	case ID_BMPVIEWER_MOVETOPREV:
		bEnabled = IsMoveToPrevEnabled();
		break;
	case ID_BMPVIEWER_MOVETONEXT:
		bEnabled = IsMoveToNextEnabled();
		break;
	case ID_BMPVIEWER_PRINT:
		bEnabled = pDoc->IsValidImage();
		break;
	case ID_CHILDFRAME_CLOSE:
		bEnabled = TRUE;
		break;
	}

	pCmdUI->Enable(bEnabled);
}


void CBitmapViewerView::OnExecuteActions(UINT nCommandID)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	switch(nCommandID) {
	case ID_BMPVIEWER_FITWIDTH:
		ZoomToWindowWidth();
		break;
	case ID_BMPVIEWER_FITWINDOW:
		ZoomToWindow();
		break;
	case ID_BMPVIEWER_ORIGINALSIZE:
		ZoomToOriginalSize();
		break;
	case ID_BMPVIEWER_ZOOMIN:
		ZoomIn();
		break;
	case ID_BMPVIEWER_ZOOMOUT:
		ZoomOut();
		break;
	case ID_BMPVIEWER_MOVETOPREV:
		MoveToPrevImage();
		break;
	case ID_BMPVIEWER_MOVETONEXT:
		MoveToNextImage();
		break;
	case ID_BMPVIEWER_PRINT:
		PrintImage();
		break;
	case ID_CHILDFRAME_CLOSE:
		GetParent()->SendMessage(WM_CLOSE);
		break;
	}
}


