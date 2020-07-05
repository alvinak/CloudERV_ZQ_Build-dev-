// BitmapViewerDoc.cpp : implementation of the CBitmapViewerDoc class
//

#include "stdafx.h"
#include "BitmapViewerDoc.h"

#include "JPEGIJG.H"


///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

// CBitmapViewerDoc

IMPLEMENT_DYNCREATE(CBitmapViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CBitmapViewerDoc, CDocument)
END_MESSAGE_MAP()


// CBitmapViewerDoc construction/destruction

CBitmapViewerDoc::CBitmapViewerDoc()
{
	m_nImageIndex = -1;
}

CBitmapViewerDoc::~CBitmapViewerDoc()
{
	ClearImages();
}

BOOL CBitmapViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}


int CBitmapViewerDoc::GetImageIndex()
{
	if (m_lstBmpImages.size() == 0) 
		m_nImageIndex = -1;

	return m_nImageIndex;
}


int CBitmapViewerDoc::GetImageCount()
{
	return m_lstBmpImages.size();
}


int CBitmapViewerDoc::SetImageIndex(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_lstBmpImages.size())
		m_nImageIndex = nIndex;

	return m_nImageIndex;
}


BOOL CBitmapViewerDoc::IsMoveToPrevImageEnabled()
{
	if (m_nImageIndex >0 && m_lstBmpImages.size() > 1)
		return TRUE;

	return FALSE;
}


BOOL CBitmapViewerDoc::IsMoveToNextImageEnabled()
{
	int nImageCount;

	nImageCount = m_lstBmpImages.size();
	
	if ((m_nImageIndex < (nImageCount - 1)) && nImageCount > 1)
		return TRUE;

	return FALSE;
}


void CBitmapViewerDoc::MoveToPrevImage()
{
	if (m_nImageIndex > 0)
		m_nImageIndex --;

	return;
}


void CBitmapViewerDoc::MoveToNextImage()
{
	if (m_nImageIndex < (m_lstBmpImages.size() - 1))
		m_nImageIndex ++;

	return;
}


CBitmap * CBitmapViewerDoc::GetImage()
{
	int nImageCount;
	CBitmap *pBmp = NULL;

	nImageCount = m_lstBmpImages.size();

	if (nImageCount == 0)
		return NULL;

	if (m_nImageIndex < 0)
		m_nImageIndex = 0;
	
	if (m_nImageIndex >= nImageCount)
		m_nImageIndex = nImageCount - 1;

	pBmp = m_lstBmpImages[m_nImageIndex];

	return pBmp;
}


BOOL CBitmapViewerDoc::IsValidImage()
{
	return m_lstBmpImages.size() > 0;
}


CSize CBitmapViewerDoc::GetImageSize()
{
	CSize size(0, 0);	
	CBitmap *pBmp = NULL;
	BITMAP bmp = {0};

   if (IsValidImage()) {
	   pBmp  = GetImage();
	   if (pBmp != NULL) {
			pBmp->GetBitmap(&bmp);
			size.cx = bmp.bmWidth;
			size.cy = bmp.bmHeight;
	   }
   }

   return size;
}


void CBitmapViewerDoc::ClearImages()
{
	std::vector <CBitmap *> ::iterator it;

	for (it = m_lstBmpImages.begin(); it != m_lstBmpImages.end(); it ++)
		delete (*it);

	m_lstBmpImages.clear();

	m_nImageIndex = -1;

}


void CBitmapViewerDoc::SetDocInfo(const TCHAR *szStudyGUID, const TCHAR *szTitle)
{
	m_sStudyGUID = szStudyGUID;
	m_sTitle = szTitle;
	SetTitle(szTitle);
}


BOOL CBitmapViewerDoc::AppendBmpFromFile(const TCHAR *szFileName, bool bKeepCurrentIndex)
{	
	CBitmap *pBmp = NULL;   
	HBITMAP hBitmap = NULL;
	
	hBitmap = (HBITMAP)::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(NULL == hBitmap) {
		CString strMessage;
		strMessage.Format(_T("LoadImage failed\nError: %u"), ::GetLastError());
		AfxMessageBox(strMessage);
	}
	else {
		pBmp = new CBitmap;
		pBmp->Attach(hBitmap);
		m_lstBmpImages.push_back(pBmp);

		if (bKeepCurrentIndex) {
			if (m_nImageIndex < 0)
				m_nImageIndex = 0;
		}
		else 
			m_nImageIndex = m_lstBmpImages.size() - 1;
	}
	
	return NULL != hBitmap;
 }


BOOL CBitmapViewerDoc::AppendDibFromBuffer(unsigned char *lpBits, int nImageWidth, int nImageHeight, int nBitCount, bool bKeepCurrentIndex)
{
    LPBITMAPINFOHEADER lpBMIH;				// @cmember A buffer containing the BITMAPINFOHEADER.
    LPVOID lpvColorTable;					// @cmember Pointer to the color table
	int	nColorTableEntries;
	CBitmap *pBmp = NULL;

	if (nBitCount == 8) 
		nColorTableEntries = 256;
	else 
		nColorTableEntries = 0;
	
	lpBMIH = (LPBITMAPINFOHEADER) malloc(sizeof(BITMAPINFOHEADER) + sizeof (RGBQUAD) * (nColorTableEntries + 4));
 
    // Setup BitmapInfoHeader
    lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
    lpBMIH->biWidth = nImageWidth;
    lpBMIH->biHeight = -nImageHeight;
    lpBMIH->biPlanes = 1;
    lpBMIH->biBitCount = nBitCount;
    lpBMIH->biSizeImage = (nImageWidth * nBitCount + 31)/32 * 4 * nImageHeight;
    lpBMIH->biXPelsPerMeter = 0;
    lpBMIH->biYPelsPerMeter = 0;
    lpBMIH->biClrUsed = nColorTableEntries;
    lpBMIH->biClrImportant = nColorTableEntries;

	lpvColorTable = (LPBYTE) lpBMIH + sizeof(BITMAPINFOHEADER);
    
    LPRGBQUAD pDibQuad = (LPRGBQUAD) lpvColorTable;
    memset(lpvColorTable, 0, sizeof(RGBQUAD) * (nColorTableEntries + 4));

	if (nColorTableEntries > 0) {
		for (int i=0; i < nColorTableEntries; i++) {
            pDibQuad->rgbRed        = i;
            pDibQuad->rgbGreen      = i;
            pDibQuad->rgbBlue       = i;
            pDibQuad->rgbReserved   = 0;
            pDibQuad++;
        }
	}
	else {
		DWORD *pdwbmiColors = (DWORD*) lpvColorTable;

        switch (nBitCount) {
            case 16:
                *(pdwbmiColors++)   = 0xF800;
                *(pdwbmiColors++)   = 0x07E0;
                *(pdwbmiColors)     = 0x001F;
                lpBMIH->biCompression = BI_BITFIELDS;
                break;
			case 15:
                *(pdwbmiColors++)   = 0x7c00;
                *(pdwbmiColors++)   = 0x03e0;
                *(pdwbmiColors)     = 0x001F;
                lpBMIH->biCompression = BI_RGB;
                break;
            case 24:
                *(pdwbmiColors++)   = 0;
                *(pdwbmiColors++)   = 0;
                *(pdwbmiColors)     = 0;
                lpBMIH->biCompression = BI_RGB;
                break;
            case 32:
                *(pdwbmiColors++)   = 0x00ff0000;
                *(pdwbmiColors++)   = 0x0000ff00;
                *(pdwbmiColors)     = 0x000000ff;
                lpBMIH->biCompression = BI_RGB;
                break;
		}
	}
	
	CWindowDC dc(NULL);
    HBITMAP hBmp = CreateDIBitmap(dc.m_hDC, 
			lpBMIH,
			CBM_INIT,
			lpBits,
		    (BITMAPINFO *) lpBMIH, 
			DIB_RGB_COLORS);

	free(lpBMIH);

	if (hBmp != NULL) {
		pBmp = new CBitmap;

		pBmp->Attach(hBmp);
		m_lstBmpImages.push_back(pBmp);

		if (bKeepCurrentIndex) {
			if (m_nImageIndex < 0)
				m_nImageIndex = 0;
		}
		else 
			m_nImageIndex = m_lstBmpImages.size() - 1;

		return TRUE;
	}

	return FALSE;
}



BOOL CBitmapViewerDoc::AppendJpgFromBuffer(unsigned char *jpg_buffer, long jpg_size, bool bKeepCurrentIndex)
{
	unsigned char *pImageData = NULL;
	int nImageWidth = 0, nImageHeight = 0;
	BOOL bOk = FALSE;

	pImageData = NULL;

	if (JpegIJGDeompress(jpg_buffer, jpg_size, nImageWidth, nImageHeight,&pImageData)) {
		bOk = AppendDibFromBuffer(pImageData, nImageWidth, nImageHeight, 24, bKeepCurrentIndex);
	}

	if (pImageData != NULL)
		free(pImageData);

	return bOk;
}


// CBitmapViewerDoc serialization

void CBitmapViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CBitmapViewerDoc diagnostics

#ifdef _DEBUG
void CBitmapViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBitmapViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CBitmapViewerDoc commands
