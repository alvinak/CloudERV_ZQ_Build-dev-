// BitmapViewer.cpp : implementation file
//

#include "stdafx.h"
#include "BitmapViewer.h"
#include "BitmapViewerDoc.h"
#include "MemoryDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewer

IMPLEMENT_DYNAMIC(CBitmapViewer, CScrollView)

CBitmapViewer::CBitmapViewer()
   : m_fZoom(1.0),
     m_fZoomStep(0.1),
     m_fMinZoom(0.2),
     m_fMaxZoom(5.0),
	 m_nZoomMode(0)
{
   SetBackSolidBrush(::GetSysColor(COLOR_APPWORKSPACE));
}

CBitmapViewer::~CBitmapViewer()
{

}



BOOL CBitmapViewer::SetBackPatternBrush(UINT nBitmapResID)
{
   if(NULL != m_brushBack.GetSafeHandle())
      m_brushBack.DeleteObject();
   if(NULL != m_bmpBack.GetSafeHandle())
      m_bmpBack.DeleteObject();
   
   BOOL bRet = m_bmpBack.LoadBitmap(nBitmapResID);
   if(bRet)
      bRet = m_brushBack.CreatePatternBrush(&m_bmpBack);

   if(NULL != m_hWnd)
      Invalidate();
   
   return bRet;
}

BOOL CBitmapViewer::SetBackSolidBrush(COLORREF crColor)
{
	if(NULL != m_brushBack.GetSafeHandle())
		m_brushBack.DeleteObject();
	if(NULL != m_bmpBack.GetSafeHandle())
		m_bmpBack.DeleteObject();

	BOOL bRet = m_brushBack.CreateSolidBrush(crColor);

	if(NULL != m_hWnd)
		Invalidate();
   
	return bRet;
}

void CBitmapViewer::SetHandCursor(UINT nCursorResID)
{
	CWinApp* pApp = AfxGetApp();
	m_hCursorHand = pApp->LoadCursor(nCursorResID);
}

void CBitmapViewer::SetHandDragCursor(UINT nCursorResID)
{
	CWinApp* pApp = AfxGetApp();
	m_hCursorHandDrag = pApp->LoadCursor(nCursorResID);
}


void CBitmapViewer::MoveToPrevImage()
{
	CBitmapViewerDoc *pDoc = GetDocument();
	pDoc->MoveToPrevImage();
	pDoc->UpdateAllViews(NULL);
}


void CBitmapViewer::MoveToNextImage()
{
	CBitmapViewerDoc *pDoc = GetDocument();
	
	pDoc->MoveToNextImage();
	pDoc->UpdateAllViews(NULL);

}


BOOL CBitmapViewer::IsMoveToPrevEnabled()
{
	return GetDocument()->IsMoveToPrevImageEnabled();
}

BOOL CBitmapViewer::IsMoveToNextEnabled()
{
	return GetDocument()->IsMoveToNextImageEnabled();
}

void CBitmapViewer::_SetScrollSizes()
{
	CBitmapViewerDoc *pDoc = GetDocument();
	CSize sizeView(100, 100);
  
   if (pDoc->IsValidImage())
   {
      CSize sizeImage = pDoc->GetImageSize();
      sizeView.cx = (int)(sizeImage.cx * m_fZoom);
      sizeView.cy = (int)(sizeImage.cy * m_fZoom);
   }

   SetScrollSizes(MM_TEXT, sizeView);
}

void CBitmapViewer::ZoomIn()
{
   m_fZoom += m_fZoomStep;
   if(m_fZoom > m_fMaxZoom)
   {
      m_fZoom = m_fMaxZoom;
   }

   _SetScrollSizes();
   Invalidate(TRUE);
}

void CBitmapViewer::ZoomOut()
{
   m_fZoom -= m_fZoomStep;
   if(m_fZoom < m_fMinZoom)
   {
      m_fZoom = m_fMinZoom;
   }

   _SetScrollSizes();
   Invalidate();
}

void CBitmapViewer::ZoomToOriginalSize()
{
	m_fZoom = 1.0;

	m_nZoomMode = 2;

	_SetScrollSizes();
	SetScrollPos(SB_HORZ, 0, FALSE);
	SetScrollPos(SB_VERT, 0, FALSE);
	Invalidate();
}

void CBitmapViewer::ZoomToWindowWidth()
{
	CBitmapViewerDoc *pDoc = GetDocument();

	CSize size = pDoc->GetImageSize();
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);

	m_nZoomMode = 1;

	const int bx = size.cx;        // source bitmap width
	const int cx = rcClient.right; // view client width
	if(bx > 10 && cx > 10)
	{
		m_fZoom = (double)cx / bx;
		_SetScrollSizes();
		Invalidate();
	}
}

void CBitmapViewer::ZoomToWindow()
{
	CBitmapViewerDoc *pDoc = GetDocument();

	CSize size = pDoc->GetImageSize();
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);

	m_nZoomMode = 0;

	const int bx = size.cx;             // source bitmap width
	const int by = size.cy;             // source bitmap height
	const int cx = rcClient.Width();    // view client width
	const int cy = rcClient.Height();   // view client height
	const double fx = (double)cx / bx;  // horizontal sizes ratio
	const double fy = (double)cy / by;  // vertical sizes ratio

	if(bx > 10 && by > 10 && cx > 10 && cy > 10)
	{
		if(fx < fy)
		{
			m_fZoom = fx;
		}
		else 
		{
			m_fZoom = fy;
		}
		_SetScrollSizes();
		Invalidate();
	}
}

void CBitmapViewer::ZoomToRectangle(const CRect& rect)
{
   // NOTE: Override this method in order to change the default.
	CBitmapViewerDoc *pDoc = GetDocument();

	CSize size = pDoc->GetImageSize();
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);

	CPoint ptOld = GetScrollPosition();

	const int bx = size.cx;             // original bitmap width
	const int by = size.cy;             // original bitmap height 
	const int cx = rcClient.Width();    // view client width
	const int cy = rcClient.Height();   // view client height
	const int sx = rect.Width();        // view client height
	const int xPos = ptOld.x;           // scroll horizontal position
	const int yPos = ptOld.y;           // scroll vertical position
	const int vx = (int)(bx * m_fZoom); // virtual view width 
	const int vy = (int)(by * m_fZoom); // virtual view height

	double fx = (double) cx / sx;
	m_fZoom *= fx;

	CPoint ptNew(0, 0);
	if(vx > cx)
	{
		ptNew.x = (int)((xPos + rect.left) * fx);
      
	}
	else
	{
		ptNew.x = (int)((rect.left - (cx - vx) / 2) * fx);

	}
   
	if(vy > cy)
	{
		ptNew.y = (int)((yPos + rect.top) * fx);
	}
	else
	{
		ptNew.y = (int)((rect.top - (cy - vy) / 2) * fx);
	}
	
	_SetScrollSizes();
	ScrollToPosition(ptNew);
   
	Invalidate();
}


void CBitmapViewer::ZoomToDefault()
{
	switch(m_nZoomMode) {
	case 0:	// ZoomToWindow
		ZoomToWindow();
		break;
	case 1:	// ZoomToWidth
		ZoomToWindowWidth();
		break;
	case 2:	// 1:1
		ZoomToOriginalSize();
		break;
	default:
		ZoomToWindow();
		break;
	}
}

void CBitmapViewer::_HandDrag(CPoint point)
{

	::SetCursor(m_hCursorHandDrag);

	DWORD dwStyle = GetStyle();

	if((WS_HSCROLL & dwStyle) || (WS_VSCROLL & dwStyle))
	{	
		const int dmin = 2;
		CPoint ptScrollPos = GetScrollPosition();
		CSize ptDocSize = GetTotalSize();
		if(WS_HSCROLL & dwStyle)
		{
			const int dx = point.x - m_ptCurrDrag.x;
			if((dx > dmin) || (dx < dmin))
			{
				const int x = max(0, (min((ptScrollPos.x - point.x + m_ptCurrDrag.x), ptDocSize.cx)));
				SetScrollPos(SB_HORZ, x, FALSE);
				Invalidate(FALSE);
			}
		}
		if(WS_VSCROLL & dwStyle)
		{
			const int dy = point.y - m_ptCurrDrag.y;
			if((dy > dmin) || (dy < -dmin))
			{
				const int y = max(0, (min((ptScrollPos.y - point.y + m_ptCurrDrag.y), ptDocSize.cy)));
				SetScrollPos(SB_VERT, y, FALSE);
				Invalidate(FALSE);
			}
		}
	}
	m_ptCurrDrag = point;
}

void CBitmapViewer::_SelectRectangle(CPoint point)
{
	CClientDC dc(this);
	const int nSavedDC = dc.SaveDC();

	dc.SetROP2(R2_NOT);
	dc.SelectStockObject(NULL_BRUSH);

	// remove old selection
	CRect rcOld(m_ptStartDrag, m_ptCurrDrag);
	rcOld.NormalizeRect();
	dc.Rectangle(rcOld);

	// draw new selection
	CRect rcNew(m_ptStartDrag, point);
	rcNew.NormalizeRect();
	dc.Rectangle(rcNew);

	dc.RestoreDC(nSavedDC);
	m_ptCurrDrag = point;
}

BEGIN_MESSAGE_MAP(CBitmapViewer, CScrollView)
	//{{AFX_MSG_MAP(CBitmapViewer)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapViewer drawing

void CBitmapViewer::OnDraw(CDC* pDCView)
{
	ASSERT(MM_TEXT == pDCView->GetMapMode());
	ASSERT(CPoint(0, 0) == pDCView->GetViewportOrg());

	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	const int cx = rcClient.right;  // view client area width
	const int cy = rcClient.bottom; // view client area height

	CMemoryDC dcDraw(pDCView, cx, cy);

	DrawBackgound(&dcDraw);
	
	DrawImage(&dcDraw, pDCView);
	DrawOverlay(&dcDraw, pDCView);

	CRect rcClip(0, 0, 0, 0);
	pDCView->GetClipBox(rcClip);
	pDCView->BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), 
		&dcDraw, 
		rcClip.left, rcClip.top, 
		SRCCOPY);
}

void CBitmapViewer::DrawBackgound(CMemoryDC* pDCDraw)
{
	ASSERT_VALID(pDCDraw);
	ASSERT(NULL != m_brushBack.GetSafeHandle());

	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	pDCDraw->SelectObject(&m_brushBack);
	pDCDraw->PatBlt(0, 0, rcClient.right, rcClient.bottom, PATCOPY);
}

void CBitmapViewer::DrawImage(CMemoryDC* pDCDraw, CDC* pDCView)
{
	CBitmapViewerDoc *pDoc = GetDocument();
	CBitmap *pBmp = NULL;
	ASSERT_VALID(pDCDraw);
	if (!pDoc->IsValidImage())
		return;

	pBmp = pDoc->GetImage();
	if (pBmp == NULL) 
		return;

	CSize size = pDoc->GetImageSize();
	CPoint point = GetScrollPosition();

	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);

	const int cx = rcClient.right;      // view client area width
	const int cy = rcClient.bottom;     // view client area height
	const int bx = size.cx;             // source bitmap width
	const int by = size.cy;             // source bitmap height
	const int vx = (int)(bx * m_fZoom); // virtual document width
	const int vy = (int)(by * m_fZoom); // virtual document height
	const int xPos = point.x;           // horizontal scroll position
	const int yPos = point.y;           // vertical scroll position

	// source and destination cordinates and sizes
	int xSrc, ySrc, nSrcWidth, nSrcHeight, xDst, yDst, nDstWidth, nDstHeight;
   

	if(vx > cx) {
		xSrc = (int)(xPos / m_fZoom);
		nSrcWidth = bx - xSrc;
		xDst = 0;
		nDstWidth = vx - xPos;
	}
	else 
	{
		xSrc = 0;
		nSrcWidth = bx;
		xDst = cx / 2 - vx / 2;
		nDstWidth = vx;
	}

	if(vy > cy) {
		ySrc = (int)(yPos / m_fZoom);
		nSrcHeight = by - ySrc;
		yDst = 0;
		nDstHeight = vy - yPos;
	}
	else  {
		ySrc = 0;
		nSrcHeight = by;
		yDst = cy / 2 - vy / 2;
		nDstHeight = vy;
	}

	CMemoryDC dcSource(pDCView, pBmp);
	pDCDraw->SetStretchBltMode(HALFTONE);
	pDCDraw->StretchBlt(xDst, yDst, nDstWidth, nDstHeight, &dcSource, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY); 

}

void CBitmapViewer::PrintImage()
{
	CBitmap *pImage = GetDocument()->GetImage();
	CPrintDialog printDlg(FALSE);

	if (pImage == NULL)
		return;

	if (printDlg.DoModal() == IDCANCEL)   
		return; 

	CDC dc;
	if (!dc.Attach(printDlg.GetPrinterDC())) {
		AfxMessageBox(_T("没有安装打印机!")); 
		return;
	} 
 
	dc.m_bPrinting = TRUE; 
	DOCINFO di;    
	// Initialise print document details
	::ZeroMemory (&di, sizeof (DOCINFO));
	di.cbSize = sizeof (DOCINFO);
	di.lpszDocName = GetDocument()->GetTitle();

	// Begin a new print job 
	BOOL bPrintingOK = dc.StartDoc(&di); 
	// Get the printing extents
	// and store in the m_rectDraw field of a 
	// CPrintInfo object
	CPrintInfo info;
	info.SetMaxPage(1); // just one page 
	int maxw = dc.GetDeviceCaps(HORZRES);
	int maxh = dc.GetDeviceCaps(VERTRES); 
	info.m_rectDraw.SetRect(0, 0, maxw, maxh); 
	
	for (UINT page = info.GetMinPage(); page <= info.GetMaxPage() && bPrintingOK; page++) {
		// begin new page
		dc.StartPage();    
		info.m_nCurPage = page;

		BITMAP bm;
		pImage->GetBitmap(&bm);
		int w = bm.bmWidth; 
		int h = bm.bmHeight; 
		// create memory device context
		CDC memDC; 
		memDC.CreateCompatibleDC(&dc);
		CBitmap *pBmp = memDC.SelectObject(pImage);
		memDC.SetMapMode(dc.GetMapMode());
		dc.SetStretchBltMode(HALFTONE);
		// now stretchblt to maximum width on page
		dc.StretchBlt(0, 0, maxw, maxh, &memDC, 0, 0, w, h, SRCCOPY); 
		// clean up
		memDC.SelectObject(pBmp);
		bPrintingOK = (dc.EndPage() > 0);   // end page
	} 
	
	if (bPrintingOK)
		dc.EndDoc(); // end a print job
	else 
		dc.AbortDoc();           // abort job. 
}


/////////////////////////////////////////////////////////////////////////////
// CBitmapViewer diagnostics

#ifdef _DEBUG
void CBitmapViewer::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBitmapViewer::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CBitmapViewerDoc* CBitmapViewer::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBitmapViewerDoc)));
	return (CBitmapViewerDoc*)m_pDocument;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CBitmapViewer message handlers

BOOL CBitmapViewer::OnEraseBkgnd(CDC* pDC) 
{
	// just prevent default processing
	// the background will be filled in OnDraw
	//return CScrollView::OnEraseBkgnd(pDC);
	return FALSE;
}

void CBitmapViewer::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	m_fZoom = 1.0;
	_SetScrollSizes();
}

void CBitmapViewer::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
   _SetScrollSizes();
   Invalidate(TRUE);
}


BOOL CBitmapViewer::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	int xOrig, x;
	int yOrig, y;

	// don't scroll if there is no valid scroll range (ie. no scroll bar)
	CScrollBar* pBar;
	DWORD dwStyle = GetStyle();
	pBar = GetScrollBarCtrl(SB_VERT);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_VSCROLL)))
	{
		// vertical scroll bar not enabled
		sizeScroll.cy = 0;
	}
	pBar = GetScrollBarCtrl(SB_HORZ);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_HSCROLL)))
	{
		// horizontal scroll bar not enabled
		sizeScroll.cx = 0;
	}

	// adjust current x position
	xOrig = x = GetScrollPos(SB_HORZ);
	int xMax = GetScrollLimit(SB_HORZ);
	x += sizeScroll.cx;
	if (x < 0)
		x = 0;
	else if (x > xMax)
		x = xMax;

	// adjust current y position
	yOrig = y = GetScrollPos(SB_VERT);
	int yMax = GetScrollLimit(SB_VERT);
	y += sizeScroll.cy;
	if (y < 0)
		y = 0;
	else if (y > yMax)
		y = yMax;

	// did anything change?
	if (x == xOrig && y == yOrig)
		return FALSE;

	if (bDoScroll)
	{
		// do scroll and update scroll positions
		/// ScrollWindow(-(x-xOrig), -(y-yOrig)); // <-- removed
      Invalidate(); // <-- added // Invalidates whole client area
		if (x != xOrig)
			SetScrollPos(SB_HORZ, x);
		if (y != yOrig)
			SetScrollPos(SB_VERT, y);
	}
	return TRUE;
}

void CBitmapViewer::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnPrepareDC(pDC, pInfo);
	pDC->SetMapMode(MM_TEXT);          // force map mode to MM_TEXT
	pDC->SetViewportOrg(CPoint(0, 0)); // force viewport origin to zero
}

BOOL CBitmapViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(MK_CONTROL & nFlags)
	{
		if(zDelta > 0)
		{
			ZoomIn();
		}
		else
		{
			ZoomOut();
		}
		Invalidate();
		return FALSE;
	}
	
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CBitmapViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetCursor(m_hCursorHand);
	m_ptStartDrag = point;
	m_ptCurrDrag = point;
	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	::ClipCursor(rcClient);

	CScrollView::OnLButtonDown(nFlags, point);
}

void CBitmapViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ClipCursor(NULL);
	CScrollView::OnLButtonUp(nFlags, point);
}

void CBitmapViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_ptStartDrag = point;
	m_ptCurrDrag = point;
	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	::ClipCursor(rcClient);

	CScrollView::OnRButtonDown(nFlags, point);
}

void CBitmapViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	::ClipCursor(NULL);

	CClientDC dc(this);
	const int nSavedDC = dc.SaveDC();
   
	dc.SetROP2(R2_NOT);
	dc.SelectStockObject(NULL_BRUSH);
   
	CRect rcOld(m_ptStartDrag, m_ptCurrDrag);
	rcOld.NormalizeRect();
	dc.Rectangle(rcOld);

	CRect rcNew(m_ptStartDrag, point);
	rcNew.NormalizeRect();
	if(rcNew.Width() > 1, rcNew.Height() > 1)
	{
		ZoomToRectangle(rcNew);
	}
   
	dc.RestoreDC(nSavedDC);

	CScrollView::OnRButtonUp(nFlags, point);
}

void CBitmapViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	if(MK_LBUTTON & nFlags)
	{
		_HandDrag(point);
	}
	else if(MK_RBUTTON & nFlags)
	{
		_SelectRectangle(point);
	}

	CScrollView::OnMouseMove(nFlags, point);
}

void CBitmapViewer::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ZoomToOriginalSize();
	CScrollView::OnLButtonDblClk(nFlags, point);
}
