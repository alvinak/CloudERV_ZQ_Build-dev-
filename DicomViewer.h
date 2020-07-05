#if !defined(AFX_DICOMVIEWER_H__17DD8B65_10CC_400A_96CB_71B832D31560__INCLUDED_)
#define AFX_DICOMVIEWER_H__17DD8B65_10CC_400A_96CB_71B832D31560__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DicomViewer.h : header file
//
#include "dcmtk/dcmdata/dcdeftag.h"      /* for DCM_StudyInstanceUID */

#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimage/diregist.h"  /* include to support color images */
#include "dcmtk/dcmdata/dcdatset.h"

#include "dcmtk/dcmdata/dcfilefo.h"

#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */


#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpeg/ddpiimpl.h"

#include "dcmtk/dcmjpeg/djrplol.h"   /* for DJ_RPLossless */
#include "dcmtk/dcmjpeg/djrploss.h"  /* for DJ_RPLossy */

#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */

#include "cdxcRot90DC.h"

#include "DicomObject.h"

// notification messages
#define NM_CELLWNDCHANGED		1
#define NM_CELLWNDCLICK			2
#define NM_CELLWNDDBL			3
#define NM_CELLWNDRBCLICK		4
#define NM_MAYBEMINIBAR			5
#define NM_DICOMVIEWERMODECHANGED 10


#define WM_MYMESSAGE01			WM_USER + 200
#define WM_PRINTSELECT			WM_USER + 201


#define WM_SHOWMESSAGE			WM_USER + 620


typedef struct 
{
	NMHDR nmhdr;
	CPoint point;
}NMHDREX;

class CPointObj : public CObject
{
public:
	CPoint m_ptPoint;
};



class CsliceProcessing  
{
public:
	CsliceProcessing();
	virtual ~CsliceProcessing();
	bool getAxisPoints (int &TopX, int &TopY, int &BottomX, int &BottomY, int &LeftX, int &LeftY, int &RightX, int &RightY);
	bool getBoundingBox (int &Ulx, int &Uly, int &Urx, int &Ury, int &Llx, int &Lly, int &Lrx, int &Lry);
	bool ProjectSlice(CString scoutPos, 
				      CString scoutOrient, 
					  CString scoutPixSpace, 
					  int scoutRows, 
					  int scoutCols, 
					  CString imgPos, 
					  CString imgOrient, 
					  CString imgPixSpace, 
					  int imgRows, 
					  int imgCols);

private:
	void calculateAxisPoints(void);
	void calculateBoundingBox(void);
	bool setImgDimensions(void);
	bool setScoutDimensions(void);
	bool rotateImage(float imgPosx, float imgPosy, float imgPosz, float &scoutPosx, float &scoutPosy, float &scoutPosz);
	bool setImgSpacing(const TCHAR* Spacing);
	bool setScoutSpacing(const TCHAR* Spacing);
	bool normalizeScout(void);
	bool setImgOrientation(const TCHAR* Pos);
	bool setImgPosition (const TCHAR* Pos);
	bool checkImgVector(void);
	void clearImg(void);
	void clearScout(void);
	bool checkVector(float CosX, float CosY, float CosZ);
	bool checkScoutVector(void);
	bool setScoutOrientation (const TCHAR* Pos);
	bool checkVectorString (const TCHAR* Pos);
	bool checkPosString(const TCHAR* Pos);
	bool setScoutPosition(const TCHAR* Pos);
	//	scout parameters
	bool   _mScoutValid;
	float _mScoutx;
	float _mScouty;
	float _mScoutz;
	float _mScoutRowCosx;
	float _mScoutRowCosy;
	float _mScoutRowCosz;
	float _mScoutColCosx;
	float _mScoutColCosy;
	float _mScoutColCosz;
	float _mScoutxSpacing;
	float _mScoutySpacing;
	float _mScoutRowLen;
	float _mScoutColLen;
	int	   _mScoutRows;
	int    _mScoutCols;
	// Image parameters
	bool   _mImgValid;
	float _mImgx;
	float _mImgy;
	float _mImgz;
	float _mImgRowCosx;
	float _mImgRowCosy;
	float _mImgRowCosz;
	float _mImgColCosx;
	float _mImgColCosy;
	float _mImgColCosz;
	float _mImgxSpacing;
	float _mImgySpacing;
	float _mImgRowLen;
	float _mImgColLen;
	int	   _mImgRows;
	int    _mImgCols;
	// normal vector
	float _mNrmCosX;
	float _mNrmCosY;
	float _mNrmCosZ;

	int _mBoxUlx;
	int _mBoxUly;
	int _mBoxUrx;
	int _mBoxUry;
	int _mBoxLlx;
	int _mBoxLly;
	int _mBoxLrx;
	int _mBoxLry;

	int _mAxisTopx;
	int _mAxisTopy;
	int _mAxisLeftx;
	int _mAxisLefty;
	int _mAxisBottomx;
	int _mAxisBottomy;
	int _mAxisRightx;
	int _mAxisRighty;
};


class CDicomViewer;

class CCellWnd : public CObject
{
public:
	CCellWnd(CDicomViewer *pParent);
	~CCellWnd();

public:

	void SetRegister(BOOL bRegister) { m_bRegistered = bRegister;}

	void SetSeriesMode(BOOL bSeriesMode) { m_bSeriesMode = bSeriesMode;}

	void SetPrintPreviewMode(BOOL bPrintPreviewMode) { m_bPrintPreviewMode = bPrintPreviewMode;} 

	void SetBorder(CRect rect);
	CRect GetBorder();
	CRect GetImageViewRect();

	CRect GetProcessViewRect();

	void SetSeriesObject(CSeriesObject *pSeriesObject);
	CSeriesObject * GetSeriesObject() { return m_pSeriesObject;}

	void SetDicomObject(CSeriesObject *pSeriesObj, CDicomObject *pObject);

	void SetFont(CFont *lpFont) { m_lpFont = lpFont;}
	void SetDrawOverlay(BOOL bValue) { m_bDrawOverlayInfo = bValue;}

	double GetZoomFactor() { return m_fZoomFactor;}

	void GetZoomFactor(double &fFactor) { fFactor = m_fZoomFactor;}

	void SetZoomFactor(double fZoomFactor) { m_fZoomFactor = fZoomFactor;ComputeImageView();}

	void GetReversed(BOOL &bReversed) {  bReversed = m_bReversed;}
	void SetReversed(BOOL bReversed) { m_bReversed = bReversed ;}

	CDicomObject * GetDicomObject();

	double GetRealZoomFactor();

	BOOL DicomObjectIsExisting();

	void DrawMagnifier(CDC *pDC,CRect rtBorder,CPoint pt,double fZoomFactor);

	void DrawObject(CDC *pDC,BOOL bSelected);
	void DrawObjectForExport(CDC *pDC,int nWidth,int nHeight);

	BOOL Draw(HDC hDC,int x,int y,int cx,int cy);

	void MakeBitmap(BOOL bFast);

	void ComputeImageView();

	void ComputeExportImageSize(int &nWidth,int &nHeight);

	void SetShift(int nX,int nY);
	void GetShift(int &nX,int &nY);

	void AddMeasurePoint(CPoint pt);
	void ClearMeasurePointsList();

	void SetMeasurePoint1(CPoint pt) { m_ptMeasurePoint1 = pt; m_ptMeasurePoint2 = pt; m_ptMeasurePoint = pt;}
	void SetMeasurePoint2(CPoint pt) { m_ptMeasurePoint2 = pt; m_ptMeasurePoint = pt;}

	void SetMeasureAnglePoint1(CPoint pt) { m_ptMeasureAnglePoint1 = pt; m_ptMeasureAnglePoint2 = pt; m_ptMeasurePoint = pt;}
	void SetMeasureAnglePoint2(CPoint pt) { m_ptMeasureAnglePoint2 = pt; m_ptMeasurePoint = pt;}

	void SetMeasureCobbPoint1(CPoint pt) { m_ptMeasureCobbPoint1 = pt; m_ptMeasurePoint = pt;}
	void SetMeasureCobbPoint2(CPoint pt) { m_ptMeasureCobbPoint2 = pt;}
	void SetMeasureCobbPoint3(CPoint pt) { m_ptMeasureCobbPoint3 = pt; m_ptMeasurePoint = pt;}
	void SetMeasureCobbPoint4(CPoint pt) { m_ptMeasureCobbPoint4 = pt;}



	CPoint GetMeasureCobbPoint1() { return m_ptMeasureCobbPoint1;}
	CPoint GetMeasureCobbPoint2() { return m_ptMeasureCobbPoint2;}
	CPoint GetMeasureCobbPoint3() { return m_ptMeasureCobbPoint3;}
	CPoint GetMeasureCobbPoint4() { return m_ptMeasureCobbPoint4;}

	double GetLineLengthOfPoint1AndPoint2();

	void SetMeasurePoint(CPoint pt)  { m_ptMeasurePoint = pt;}

	void SetDrawMeasure(BOOL bDraw) { m_bDrawMeasure = bDraw;}

	void SetCTValuePoint(CPoint pt) { m_ptCTValue = pt;}
	void SetDrawCTValue(BOOL bDraw) { m_bDrawCTValue = bDraw;}
	
	void SetMagnifierPoint(CPoint pt) { m_ptMagnifier = pt;}
	void SetDrawMagnifier(BOOL bDraw) { m_bDrawMagnifier = bDraw;}


	void SetFlip(BOOL bHFlip,BOOL bVFlip);  ///{ m_bHFlip = bHFlip;m_bVFlip = bVFlip;}
	void GetFlip(BOOL &bHFlip,BOOL &bVFlip);

	void SetRotateAngle(int nAngle) { m_nRotateAngle = nAngle; ComputeImageView();}
	void GetRotateAngle(int &nAngle) { nAngle = m_nRotateAngle;} 

	void SetSmoothFilterNo(int nSmoothFilterNo) { m_nSmoothFilterNo = nSmoothFilterNo;}
	void SetSharpFilterNo(int nSharpFilterNo) { m_nSharpFilterNo = nSharpFilterNo;}

	void SetExtLut(BOOL bUseExtLut,BYTE *lpLut);

	void Reset();

	BOOL IsLocalizer() { return m_bLocalizer;}
	BOOL IsCTMR() { return m_bCTMR;}

	int GetCineMode() { return m_nCineMode; }
	void SetCineMode(int nMode) { m_nCineMode = nMode; }


private:

	void DrawOverlayInfo(CRect rt,CDC *pDC);

	void DrawMeasure(CRect rtBorder,CDC *pDC);

	void DrawLineMeasure(CRect rt,CDC *pDC);
	void DrawRectangleMeasure(CRect rt,CDC *pDC);
	void DrawEllipseMeasure(CRect rt,CDC *pDC);
	void DrawPolygonMeasure(CRect rt,CDC *pDC);
	void DrawAngleMeasure(CRect rt,CDC *pDC);
	void DrawCobbMeasure(CRect rt,CDC *pDC);

	void DrawCTValue(CRect rt,CDC *pDC);

	void DrawLocalizer(CRect rt,CDC *pDC);

	void DrawProcessView(CDC *pDC);

	double Circumference(double semimajor, double semiminor);


private:
	CDicomViewer *m_pDicomViewer;
	CString m_sUnregistered;
	int m_nMeasureType;
	int m_nMagnifierSize;
	int m_nSmoothFilterNo,m_nSharpFilterNo;

	int m_nCineMode; // 0 -- Stop, 1 -- Play , 2 -- Play reverse, 3 -- Play bounce
	
	BOOL  m_bLocalizer,m_bCTMR;

	CRect m_rtBorder,m_rtView;
	
	CRect m_rtProcessView;

	BOOL m_bSeriesMode,m_bPrintPreviewMode;

	BOOL m_bDrawOverlayInfo,m_bDrawCTValue;
	BOOL m_bDrawMeasure;
	BOOL m_bDrawProcessView;
	BOOL m_bDrawMagnifier;


	CPoint m_ptCTValue;
	CPoint m_ptMagnifier;

	CPoint m_ptMeasurePoint1,m_ptMeasurePoint2,m_ptMeasureAnglePoint1,m_ptMeasureAnglePoint2;;
	CPoint m_ptMeasurePoint;

	CObList m_MeasurePointsList;

	CPoint m_ptMeasureCobbPoint1,m_ptMeasureCobbPoint2,m_ptMeasureCobbPoint3,m_ptMeasureCobbPoint4;


	CDicomObject *m_pDicomObject;
	CSeriesObject *m_pSeriesObject;

	DicomImage *m_lpDicomImage;

	double m_fZoomFactor;

	int m_nImageWidth,m_nImageHeight;
	int m_nShiftX,m_nShiftY;
	
	BOOL m_bHFlip,m_bVFlip;
	int m_nRotateAngle;
	BOOL m_bReversed;
	
	int m_nFrameIndex;

	BOOL m_bRegistered;

private:
	LPBITMAPINFOHEADER	m_lpBMIH;
    LPVOID	m_lpvColorTable;
    void *  m_lpBits;      
	CFont * m_lpFont;
	BYTE   m_cExtLut[768];
	BOOL m_bUseExtLut;
};



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMagnWnd window

/////////////////////////////////////////////////////////////////////////////
// CDicomViewer window

class CDicomViewer : public CWnd
{
// Construction
public:
	CDicomViewer();

// Attributes
	
private:
	int m_nRows,m_nCols;
	int m_nSeriesRows,m_nSeriesCols;
	BOOL m_bOneCellWndMode;
	BOOL m_bShowScrollBar;
	int m_nOldRows,m_nOldCols,m_nOldCurrentIndex;
	
	int m_nMeasureType,m_nMeasureTypePrev;

	BOOL m_bMagnifyActive;

	BOOL m_bCancelCalibrate;

	BOOL m_bExchangePosMode;

	CCellWnd *m_pExchangePosWnd1,*m_pExchangePosWnd2;

	CObList *m_pSeriesObjectList;	// 所有影像保存在SeriesObjectList列表中

	CSeriesObject *m_pSeriesObject;	// 当前影像序列

	CObList *m_pDicomObjectList;

	CObList *m_pCellWndList;
	
	int m_nCurrentIndex,m_nMaxIndex;
	CCellWnd *m_pCellWndSelected;

	BOOL  m_bMeasureAngleOk;

	BOOL m_bLBDown,m_bRBDown,m_bCursor;
	int m_nLBFuncNo,m_nRBFuncNo;
	DWORD m_dwLBDownTickCount,m_dwRBDownTickCount,m_dwLBUpTickCount,m_dwRBUpTickCount;
	int m_nButtonDelayFactor;
	CPoint m_ptStart;
	CPoint m_ptFirstPointAfterRightButtonDown;
	CPoint m_ptFirst,m_ptSecond,m_ptThird;
	CPoint m_ptLineMeasureStart,m_ptLineMeasureEnd;
	CPoint m_ptMeasurePoint1,m_ptMeasurePoint2;
	CPoint m_ptOrigin;
	double m_fRadius;

	HCURSOR m_hLBFunc1Cursor,m_hLBFunc2Cursor;
	HCURSOR m_hRBFunc1Cursor,m_hRBFunc2Cursor;
	HCURSOR m_hMeasureCursor;

	CFont m_font;
	LOGFONT m_lpLogFont;
	BOOL m_bDrawOverlay;

	double m_fMagnifierZoomFactor;
	
	BOOL m_bUseExtLut;

	BYTE m_cExtLut[768];

	int m_nCineMode, m_nCineSpeed;
	int m_nCineMaxTimes,m_nCineEscapedTimes;
	int m_nBrowseMaxTimes,m_nBrowseEscapedTimes;
	BOOL m_bRegistered;
	BOOL m_bAutoBrowseMode;

	int m_nTextColor,m_nBkColor, m_nBkMode;

	int m_nSensitivity;

	BOOL m_bPopupMenuWorking;

	BOOL m_bSeriesMode;

	BOOL m_bPrintPreviewMode;

	HWND m_wndPrintSCU;
// Operations


private:
	void ExchangeObjectPos(CCellWnd *pCellWnd1,CCellWnd * pCellWnd2);
public:

	CObList * GetCellWndList() { return m_pCellWndList;}


	void SetMatrix(int nRows,int nCols,BOOL bSeriesMode);
	void CreateCellWnds(BOOL bSeriesMode);

	void SetPrintPreviewMode(BOOL bPreviewMode = TRUE) { m_bPrintPreviewMode = bPreviewMode;}

	void SetPrintSCUWnd(HWND  hWnd) { m_wndPrintSCU = hWnd;}

	void AdjustMatrix(int nRows,int nCols);

	BOOL PopupMenuIsWorking() { return m_bPopupMenuWorking;}

	BOOL InAutoBrowsing() { return m_bAutoBrowseMode;}
	BOOL CanAutoBrowse() { return m_nMaxIndex > 0;}

	BOOL IsExchangePosMode() { return m_bExchangePosMode;}

	BOOL ExchangePosAvailable();

	void SetExchangePosMode(BOOL bValue);

	BOOL IsOneCellWndMode() { return m_bOneCellWndMode;}

	int	 GetMeasureType() { return m_nMeasureType;}
	void SetMeasureType(int nType) { m_nMeasureTypePrev = m_nMeasureType; m_nMeasureType = nType; }
	void SetRegistered(BOOL bRegistered) { m_bRegistered = bRegistered;}

	void SetDicomObjectList(CObList *pList) { m_pDicomObjectList = pList;}
	void SetSeriesObjectList(CObList *pList) { m_pSeriesObjectList = pList;}

	void DisplaySeriesObjectList(CSeriesObject *pSeriesObject);
	void DisplaySeriesObject(CSeriesObject *pSeriesObject);

	BOOL GetSeriesMode() { return m_bSeriesMode;}

	void SetSensitivity(int nValue) { m_nSensitivity = nValue;}

	void ResizeCellWnd();

	void SetShowScrollBar(BOOL bShow);
	BOOL GetShowScrollBar() { return m_bShowScrollBar;}

	void SetCurrentIndex(int nIdx);
	int GetCurrentIndex();

	void AdjustVScrollBar();
	void SetVScrollBarPos(int nPos);

	void BindSeriesObject2CellWnd(int nIndex,CSeriesObject *pCurObj);
	void BindDicomObject2CellWnd(int nIndex,CSeriesObject *pSeriesObject,CDicomObject *pDicomObject=NULL);

	BOOL SelectCellWnd(CPoint point);

	CCellWnd *GetCellWndSelected() { return m_pCellWndSelected;}

	int  GetLBFuncNo() { return m_nLBFuncNo;}

	void SetLBFuncNo(int nFuncNo) { m_nLBFuncNo = nFuncNo;}
	void SetRBFuncNo(int nFuncNo) { m_nRBFuncNo = nFuncNo;}

	void SetZoomFactor(double fZoomFactor);

	double GetMagnifierZoomFactor() { return m_fMagnifierZoomFactor; }

	void SetMagnifierZoomFactor( double fZoomFactor) { m_fMagnifierZoomFactor = fZoomFactor;}

	void ChangeSelectedWindowLevel(int nDeltaWid,int nDeltaCen);

	void ReBuildBitmap();

	void SetDrawOverlay(BOOL bValue);

	void SetReversed();
	
	void EnterBrowseMode();
	void ExitBrowseMode();

	void SetCineMode(int nCineMode);
	int GetCineMode() { return m_nCineMode;}

	void SetCineSpeed(int nCineSpeed);
	int GetCineSpeed() { return m_nCineSpeed;}

	void SetWinLevel(double fcen,double fwid);
	BOOL GetWinLevel(double &fcen,double &fwid);

	void SetLBFunc1Cursor(HCURSOR hCursor) {m_hLBFunc1Cursor = hCursor;}
	void SetLBFunc2Cursor(HCURSOR hCursor) {m_hLBFunc2Cursor = hCursor;}
	void SetMeasureCursor(HCURSOR hCursor) {m_hMeasureCursor = hCursor;}
	void SetRBFunc1Cursor(HCURSOR hCursor) {m_hRBFunc1Cursor = hCursor;}
	void SetRBFunc2Cursor(HCURSOR hCursor) {m_hRBFunc2Cursor = hCursor;}

	void HFlip();
	void VFlip();

	void RotateAngle90(int nDirection);

	void SetExtLut(BOOL bUseExtLut,BYTE *lpLut);

	void SetSmoothFilterNo(int nFilterNo);
	void SetSharpFilterNo(int nFilterNo);

	void SaveDicomObjectToFile(const TCHAR *lpszDir);
	void SaveDicomObjectToFileEx(const TCHAR *lpszDir);

	void ExportAllDicomObjectsToJpegFiles(const TCHAR *lpszDir);
	void ExportOneDicomObjectToJpegFile(const TCHAR *lpszFileName);
	void CineMove(int nStep);

	void SetFakeColorSetNo(int nIndex);

	BYTE *GetExtLut() { return m_cExtLut;}

	int GetTextColor() { return m_nTextColor;}
	int GetBkColor() { return m_nBkColor;}
	int GetBkMode() { return m_nBkMode;}

	void SetTextColor(int nColor) { m_nTextColor = nColor;}
	void SetBkColor(int nColor) { m_nBkColor = nColor;}
	void SetBkMode(int nMode) { m_nBkMode = nMode;}

	int GetDicomObjectCount();
	int GetSeriesObjectCount();

	CSeriesObject *GetSeriesObject() { return m_pSeriesObject;}

	void ResetAll();
		
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDicomViewer)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDicomViewer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDicomViewer)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

  
/////////////////////////////////////////////////////////////////////////////



extern HMODULE GetCurrentModule();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DICOMVIEWER_H__17DD8B65_10CC_400A_96CB_71B832D31560__INCLUDED_)
