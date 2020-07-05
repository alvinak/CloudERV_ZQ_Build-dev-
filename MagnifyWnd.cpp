// MagnifyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MagnifyWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMagnifyWnd

CMagnifyWnd::CMagnifyWnd()
{
	//Create a "lens" object
	pMagnGlass = new CMagnWnd;

	//Default values of Magnify Glass properties
	m_nShape = 0;//RECTANGULAR;
	m_bCaputred = FALSE;
	m_nZoom = 20;
	m_nSize = 240;
}

CMagnifyWnd::~CMagnifyWnd()
{
	m_bmp.DeleteObject();
	delete pMagnGlass;
}


BEGIN_MESSAGE_MAP(CMagnifyWnd, CWnd)
	//{{AFX_MSG_MAP(CMagnifyWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMagnifyWnd message handlers

BOOL CMagnifyWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.lpszClass = AfxRegisterWndClass( NULL, NULL, (HBRUSH) NULL);
	cs.style = WS_CLIPCHILDREN | WS_CHILD ;
	return CWnd::PreCreateWindow(cs);
}

int CMagnifyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	//Prepare cursor
	///m_curMgnfy = AfxGetApp()->LoadCursor( IDC_CURSOR_MGNFY ) ;
	
	//Create a "lens"
	pMagnGlass->Create(
		AfxRegisterWndClass( NULL, NULL, (HBRUSH) GetStockObject(NULL_BRUSH)), 
		NULL, WS_CHILD,  CRect(0,0,0,0), this, 0xffff);
	
	return 0;
}

void CMagnifyWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//Save the image of a view to a bitmap member 
	RefreshMagnify(TRUE);

	SaveMagnify();	

	m_CurPoint = CPoint(point.x - m_nSize / 2,point.y - m_nSize / 2); //Save current cursor 

	SetCapture();
	m_bCaputred = TRUE;

	//Reposition and show "lens"
	pMagnGlass->MoveWindow(CRect(point.x - m_nSize / 2,point.y - m_nSize / 2,point.x - m_nSize / 2 + m_nSize, point.y - m_nSize / 2 + m_nSize));
	pMagnGlass->ShowWindow(SW_SHOW);

	CWnd::OnLButtonDown(nFlags, point);

}

void CMagnifyWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bCaputred) //"Lens" is active 
	{
		//SetCursor(NULL);  //Hide a cursor

		//Reposition a "lens"
		m_CurPoint = CPoint(point.x - m_nSize / 2,point.y - m_nSize / 2);
		pMagnGlass->MoveWindow(CRect(point.x - m_nSize / 2,point.y - m_nSize / 2,point.x - m_nSize / 2 + m_nSize, point.y - m_nSize / 2 + m_nSize),FALSE);

		//Force to redraw
		Invalidate(FALSE);
		UpdateWindow();
		pMagnGlass->Invalidate(FALSE);
		pMagnGlass->UpdateWindow();
			
	}
	///else			 //"Lens" is hidden - restore cursor
	///	SetCursor(m_curMgnfy);

	CWnd::OnMouseMove(nFlags, point);
}

void CMagnifyWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//Inactivate a "lens"

	pMagnGlass->ShowWindow(SW_HIDE); //Hide a "lens" 
	ReleaseCapture();				 //Restore mouse input
	m_bCaputred = FALSE;			 //Flag
	RefreshMagnify(TRUE);				 //Redraw a view 
	Invalidate(FALSE);				 //Redraw itself
	UpdateWindow();
	CWnd::OnLButtonUp(nFlags, point);
	
}

void CMagnifyWnd::OnPaint() 
{
	//We do not need to draw when a "lens" is inactive
	if(m_bCaputred)
	{
		//Draw m_bmp if we have a "lens" visible

		CPaintDC dc(this); // device context for painting
		
		CRect rc;
		GetClientRect(rc);
		
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(memDC.m_hDC, m_bmp.m_hObject);

		dc.BitBlt(0,0,rc.right,rc.bottom,&memDC,0,0,SRCCOPY);

		::SelectObject(memDC.m_hDC, hOldBmp);
	}
	// Do not call CWnd::OnPaint() for painting messages
}

void CMagnifyWnd::RefreshMagnify(BOOL bEraseBackgroud)
{
	//Force View to redwaw

	CWnd* pView = CWnd::FromHandle(GetParent()->m_hWnd); //Get  a view

	pView->Invalidate(bEraseBackgroud);

	pView->UpdateWindow();
}

void CMagnifyWnd::SaveMagnify()
{
	//Saves an image from a view bellow this window into member m_bmp
	
	//Get device context to draw from
   	CDC dcImage;
	HDC hdc = ::GetDC(GetParent()->m_hWnd);
	dcImage.Attach(hdc);

	//Get size of bitmap
	CRect rc;
	GetParent()->GetClientRect(rc);

	//Reinitialize member m_bmp.
	m_bmp.DeleteObject(); //It is safe to call DeleteObject even if there is no object
	m_bmp.CreateCompatibleBitmap(&dcImage,rc.right,rc.bottom);

	//Get memory device context to draw to m_bmp
	CDC memDC;
	memDC.CreateCompatibleDC(&dcImage);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(memDC.m_hDC, m_bmp.m_hObject);

	//Draw
	memDC.BitBlt(0,0,rc.right,rc.bottom,&dcImage,0,0,SRCCOPY);


	//Clean up
	::SelectObject(memDC.m_hDC, hOldBmp);
	dcImage.Detach();
	::ReleaseDC(GetParent()->m_hWnd,hdc);
}

void CMagnifyWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
/*
	//Create a property page dialog
	CPropMagn prop("Magnifying Glass Properties",NULL,0) ;
	prop.SetSettings(m_nShape, m_nSize, m_nZoom);
	prop.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	if (prop.DoModal() != IDOK) return;

	//Read values for all properties user choosen and save them in member variables

	int nSize;
	int nZoom;
	prop.GetSettings(m_nShape, nSize, nZoom);

	switch (nSize)
	{
		case S100:	m_nSize = 100; break;
		case S120:	m_nSize = 120; break;
		case S150:	m_nSize = 150; break;
		case S180:	m_nSize = 180; break;
	}
	
	switch (nZoom)
	{
		case Z15:	m_nZoom = 18; break;
		case Z20:	m_nZoom = 20; break;
		case Z25:	m_nZoom = 25; break;
		case Z30:	m_nZoom = 30; break;
	}
*/
	//CWnd::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CMagnWnd
/* Class CMagnWnd represents a "lens" for Manify Glass. It has two different draw routines
one for rectangular another one for round shape
*/

BEGIN_MESSAGE_MAP(CMagnWnd, CWnd)
	//{{AFX_MSG_MAP(CMagnWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMagnWnd message handlers

void CMagnWnd::OnPaint() 
{

	CPaintDC dc(this); // device context for painting

	//Read a "lens" shape and a route for drawing 
	//Two different draw routines for now 
	//if (((CMagnifyWnd*)GetParent())->m_nShape == ROUND)
	//	DrawRoundMagnify(&dc);
	///else
		DrawRectMagnify(&dc);
	// Do not call CWnd::OnPaint() for painting messages
}


void CMagnWnd::DrawRectMagnify(CDC* pDC)
{
	//Ok. Here we are to draw a rectangular region from given bitmap with 
	//magnification. We've got three steps to perform.

	//1.To simplify notation get a copy (references) 
	//of all the necessary information from CImageWnd object localy

	//a bitmap - copy of View window bitmap 
	CBitmap& bmp = ((CMagnifyWnd*)GetParent())->m_bmp;     

    //current point - a center of area to magnify 
	CPoint&  pnt = ((CMagnifyWnd*)GetParent())->m_CurPoint;

	//Property size - default value or changed by user
	int nMagnSize = ((CMagnifyWnd*)GetParent())->m_nSize;

	//Property zoom (power of magnification) - default value or changed by user
	int nMagnZoom = (int) ((CMagnifyWnd*)GetParent())->m_nZoom;

	//2.Draw rectangular region
	//We do not need any region functions in this case.
	//StretchBlt manages rect region by default.
	//Select a source bitmap into memory DC.
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(memDC.m_hDC, bmp.m_hObject);
	
	//Draw!
	pDC->StretchBlt(0,
					0,
					nMagnSize,
					nMagnSize,
					&memDC,
					pnt.x + nMagnSize / 2 - (nMagnSize /nMagnZoom * 10 / 2),
					pnt.y + nMagnSize / 2 - (nMagnSize /nMagnZoom * 10 / 2),
					nMagnSize /nMagnZoom * 10,
					nMagnSize /nMagnZoom * 10, 
					SRCCOPY);

	::SelectObject(memDC.m_hDC, hOldBmp); //some clean up
	
	//3.Draw a rectangular frame around the "lens"
	pDC->MoveTo(0,0);pDC->LineTo(nMagnSize - 1,0);pDC->LineTo(nMagnSize - 1,nMagnSize - 1);pDC->LineTo(0,nMagnSize - 1);pDC->LineTo(0,0);
	pDC->MoveTo(2,2);pDC->LineTo(nMagnSize - 3,2);pDC->LineTo(nMagnSize - 3,nMagnSize - 3);pDC->LineTo(2,nMagnSize - 3);pDC->LineTo(2,2);
}

void CMagnWnd::DrawRoundMagnify(CDC* pDC)
{
	//Here we are to draw a round (eliptical) region from given bitmap with 
	//magnification in four steps.

	//1.To simplify notation get a copy (references) 
	//of all the necessary information from CImageWnd object localy

	//a bitmap - copy of View window bitmap 
	CBitmap& bmp = ((CMagnifyWnd*)GetParent())->m_bmp;     

    //current point - a center of area to manify 
	CPoint&  pnt = ((CMagnifyWnd*)GetParent())->m_CurPoint;

	//Property size - default value or changed by user
	int nMagnSize = ((CMagnifyWnd*)GetParent())->m_nSize;

	//Property zoom (power of magnification) - default value or changed by user
	int nMagnZoom = (int) ((CMagnifyWnd*)GetParent())->m_nZoom;


	//2.Draw round region.
	//Take a source bitmap into memory DC
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(memDC.m_hDC, bmp.m_hObject);

	//Create round region and select it into device context for painting
	CRgn regRound;
	regRound.CreateEllipticRgn(0,0,nMagnSize,nMagnSize);
	pDC->SelectClipRgn(&regRound, RGN_COPY );

	//Draw!
	pDC->StretchBlt(0,
					0,
					nMagnSize,
					nMagnSize,
					&memDC,
					pnt.x + nMagnSize / 2 - (nMagnSize /nMagnZoom * 10 / 2),
					pnt.y + nMagnSize / 2 - (nMagnSize /nMagnZoom * 10 / 2),
					nMagnSize /nMagnZoom * 10,
					nMagnSize/nMagnZoom * 10, 
					SRCCOPY);

	//3.Draw a frame around the "lens"
	pDC->Arc( CRect(1,1,nMagnSize - 2,nMagnSize - 2), CPoint(0,0), CPoint(0,0) );
	pDC->Arc( CRect(2,2,nMagnSize - 3,nMagnSize - 3), CPoint(0,0), CPoint(0,0) );

	//4. Redraw all bits in "lens" window wich are outside a frame.
	//We have a tail after the "lens" when moving without this step.
	CRgn regRect;
	regRect.CreateRectRgn(0,0,nMagnSize,nMagnSize);
   	pDC->SelectClipRgn(&regRect, RGN_XOR );
	pDC->BitBlt(0,0,nMagnSize + 2,nMagnSize + 2,&memDC,pnt.x,pnt.y, SRCCOPY);
	

	::SelectObject(memDC.m_hDC, hOldBmp);  //Some clean up.

}
