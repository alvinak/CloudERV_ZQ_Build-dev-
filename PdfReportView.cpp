// PdfReportView.cpp : implementation of the CPdfReportView class
//

#include "stdafx.h"
#include "CloudERV.h"

#include "PdfReportDoc.h"
#include "PdfReportView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPdfReportView

IMPLEMENT_DYNCREATE(CPdfReportView, CView)

BEGIN_MESSAGE_MAP(CPdfReportView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_UPDATE_COMMAND_UI_RANGE(1000, 65535, OnUpdateActions)
	ON_COMMAND_RANGE(1000, 65535, OnExecuteActions)
END_MESSAGE_MAP()

// CPdfReportView construction/destruction

CPdfReportView::CPdfReportView()
{
	m_bInitialized = FALSE;
	m_pPDF = NULL;
}

CPdfReportView::~CPdfReportView()
{
	if (m_pPDF) 
		delete m_pPDF;
}


BOOL CPdfReportView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPdfReportView drawing

void CPdfReportView::OnDraw(CDC* pDC)
{
	CPdfReportDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CPdfReportView diagnostics

#ifdef _DEBUG
void CPdfReportView::AssertValid() const
{
	CView::AssertValid();
}

void CPdfReportView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPdfReportDoc* CPdfReportView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPdfReportDoc)));
	return (CPdfReportDoc*)m_pDocument;
}
#endif //_DEBUG


// CPdfReportView message handlers


int CPdfReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndPdfViewer.Create(this, CRect(0,0,0,0), 1))
		return -1;

	return 0;
}

BOOL CPdfReportView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CPdfReportView::OnPreparePrinting(CPrintInfo* pInfo)
{
	CPrintDialog *dlg = new CPrintDialog(FALSE, PD_PAGENUMS);

	pInfo->m_pPD = dlg;
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(m_wndPdfViewer.GetPdfPageCount());
	
	pInfo->m_strPageDesc = GetDocument()->GetTitle(); 

	return DoPreparePrinting(pInfo);
}

void CPdfReportView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
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

	if (!m_pPDF) { 
		m_pPDF = new MuPDFClass;
	}

	m_pPDF->LoadPdfFromStream(GetDocument()->GetPdfData(), GetDocument()->GetPdfDataLen(),(char *) GetDocument()->GetPdfPassword());
	
}

void CPdfReportView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{	
	int nCurPage;
	int nDestWidth, nDestHeight;
	int nImageWidth = 0, nImageHeight = 0, nDataSize;
	int nScaleLeft, nScaleTop, nScaleWidth, nScaleHeight;
	double fRatio;
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

	if (m_pPDF->LoadPage(nCurPage) == nCurPage) {

		pBits =m_pPDF->GetBitmap(nImageWidth, nImageHeight, 150, 150, 0, 0, false, false, nDataSize, 0);

	
		nScaleTop = 0;
		nScaleLeft = 0;
		nScaleWidth = nDestWidth;
		nScaleHeight = nDestHeight;

		
		if (nImageWidth > 0 && nImageHeight > 0) {
			fRatio = (double) nImageWidth / (double) nImageHeight;

			if (fRatio < 1.0) {
				// 图像的宽度 < 图像的高度

				nScaleHeight = nDestHeight;
				nScaleTop = 0;
				nScaleWidth = fRatio * nScaleHeight;
				nScaleLeft = ((nDestWidth - nScaleWidth) >> 1);
			}
			else {
				// 图像的宽度 > 图像的高度

				nScaleWidth = nDestWidth;
				nScaleLeft = 0;
				nScaleHeight = nDestWidth / fRatio;
				nScaleTop = 0;
			}
		}

		memset(&bi, 0, sizeof(BITMAPINFO)); 
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
		bi.bmiHeader.biWidth = nImageWidth; 
		bi.bmiHeader.biHeight = -nImageHeight; // top-down 
		bi.bmiHeader.biPlanes = 1; 
		bi.bmiHeader.biBitCount = 32;  
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biSizeImage = nImageWidth * nImageHeight * 4;

		//StretchDIBits(pDC->GetSafeHdc(), 0, 0, nDestWidth, nDestHeight, 0, 0, nImageWidth, nImageHeight, pBits, &bi,DIB_RGB_COLORS,SRCCOPY);
		StretchDIBits(pDC->GetSafeHdc(), nScaleLeft, nScaleTop, nScaleWidth, nScaleHeight, 0, 0, nImageWidth, nImageHeight, pBits, &bi,DIB_RGB_COLORS,SRCCOPY);
	}

	CView::OnPrint(pDC, pInfo);
}

void CPdfReportView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_pPDF) {
		delete m_pPDF;
		m_pPDF = NULL;
	}
}

void CPdfReportView::OnInitialUpdate()
{
	CRect rect;

	CView::OnInitialUpdate();
	GetParentFrame()->GetClientRect(&rect);
	
	m_wndPdfViewer.MoveWindow(0,0, rect.Width(), rect.Height());
	m_bInitialized = TRUE;
}

void CPdfReportView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	unsigned char *pBuffer;
	long nBufferSize;
	const TCHAR *szTitle;
	CPdfReportDoc *pDoc = GetDocument();
	if (pDoc->IsInitialized()) {
		m_wndPdfViewer.LoadFromBuffer(pDoc->GetPdfData(), pDoc->GetPdfDataLen(), pDoc->GetPdfPassword(), 0);
		InvalidateRect(NULL);
	}
}


void CPdfReportView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED || !m_bInitialized)
		return;

	//TODO , pdfviewer 需要检查空指针， 如果不加m_bInitialized 控制，XP 下会Crash
	m_wndPdfViewer.MoveWindow(0,0, cx,cy);
}



void CPdfReportView::OnUpdateActions(CCmdUI *pCmdUI)
{
	BOOL bEnabled = FALSE;

	switch(pCmdUI->m_nID) {
	case ID_PDFREPORT_ZOOMIN:
		bEnabled = m_wndPdfViewer.IsZoominEnabled();
		break;
	case ID_PDFREPORT_ZOOMOUT:
		bEnabled = m_wndPdfViewer.IsZoomoutEnabled();
		break;
	case ID_PDFREPORT_PREVPAGE:
		bEnabled = m_wndPdfViewer.IsPrevPageEnabled();
		break;
	case ID_PDFREPORT_NEXTPAGE:
		bEnabled = m_wndPdfViewer.IsNextPageEnabled();
		break;
	case ID_PDFREPORT_PRINT:
		bEnabled = (m_wndPdfViewer.GetPdfPageCount() > 0);
		break;
	case ID_PDFREPORT_EXPORT:
		bEnabled = (m_wndPdfViewer.GetPdfPageCount() > 0) && !GetDocument()->IsExternalFile();
		break;
	case ID_CHILDFRAME_CLOSE:
		bEnabled = TRUE;
		break;
	}

	pCmdUI->Enable(bEnabled);
}


void CPdfReportView::OnExecuteActions(UINT nCommandID)
{
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();

	switch(nCommandID) {
	case ID_PDFREPORT_ZOOMIN:
		m_wndPdfViewer.Zoomin();
		break;
	case ID_PDFREPORT_ZOOMOUT:
		m_wndPdfViewer.Zoomout();
		break;
	case ID_PDFREPORT_PREVPAGE:
		m_wndPdfViewer.PrevPage();
		break;
	case ID_PDFREPORT_NEXTPAGE:
		m_wndPdfViewer.NextPage();
		break;
	case ID_PDFREPORT_PRINT:
		CView::OnFilePrint();
		break;
	case ID_PDFREPORT_EXPORT:
		{
			CPdfReportDoc *pDoc = GetDocument();
			const TCHAR *szDefExts[] = { {_T("pdf")} ,{_T("xps")}, {_T("jpg")},  0 };
			const TCHAR *szFileNames[] = { {_T("*.pdf")} ,{_T("*.xps")}, {_T("*.jpg")},  0 };
			const TCHAR *szFilters[] = { {_T("Pdf文件(*.pdf)|*.pdf")}, {_T("XPS文件(*.xps)|*.xps")}, {_T("Jpeg文件(*.jpg)|*.jpg")}, 0};
			int nTypeIdx = 0;
			CFileDialog *pDlg;
			unsigned char *pPdfData;
			int nPdfDataLen;
			CString sFileName;
			CString sPswd(""), sMsg;
			FILE *fp;

			pPdfData = pDoc->GetPdfData();
			nPdfDataLen = pDoc->GetPdfDataLen();
			sPswd = pDoc->GetPdfPassword();

			if (nPdfDataLen <= 4) return;

			if (pPdfData[0] == 0x25 && pPdfData[1] == 0x50 && pPdfData[2] == 0x44 && pPdfData[3] == 0x46)
				nTypeIdx = 0;
			else if (pPdfData[0] == 0xFF && pPdfData[1] == 0xD8 && pPdfData[2] == 0xFF && pPdfData[3] == 0xE0)
				nTypeIdx = 1;
			else if (pPdfData[0] == 0x50 && pPdfData[1] == 0x4B && pPdfData[2] == 0x03 && pPdfData[3] == 0x04)
				nTypeIdx =  2;
			else
				nTypeIdx = 0;

			pDlg = new CFileDialog(FALSE, szDefExts[nTypeIdx],
					_T(""), 
					OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
					szFilters[nTypeIdx]);

				pDlg->m_ofn.lpstrTitle = _T("当前报告导出到文件");

			if (pDlg->DoModal() == IDOK) {

				sFileName = pDlg->GetPathName();
				fp = _tfopen(sFileName, _T("wb"));
				if (fp) {
					fwrite(pPdfData, nPdfDataLen, 1, fp);
					fclose(fp);

					if (!sPswd.IsEmpty()) {
						sMsg.Format(_T("为保护病人隐私，此医学文档已做加密处理，密码是:\n\t%s\n密码已经复制到系统的剪贴板中."), sPswd);
						MessageBox(sMsg, _T("提示信息"), MB_ICONEXCLAMATION | MB_OK);
						
						pApp->TextToClipboard(::GetDesktopWindow(), CStringA(sPswd));
					}
				}
			}	

			delete pDlg;

		}
		break;
	case ID_CHILDFRAME_CLOSE:
		GetParent()->SendMessage(WM_CLOSE);
		break;
	}
}

