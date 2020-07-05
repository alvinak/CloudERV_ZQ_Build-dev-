// MagnifyWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMagnWnd window

// CMagnWnd is a "lens" to show magnifyed image

class CMagnWnd : public CWnd
{
// Construction
public:
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagnWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
protected:
	void DrawRoundMagnify(CDC* pDC);
	void DrawRectMagnify(CDC* pDC);
	//{{AFX_MSG(CMagnWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMagnifyWnd window

// CMagnifyWnd suppose to keep and redraw a copy of bitmap of magnifyed window.

class CMagnifyWnd : public CWnd
{
	friend class CMagnWnd;
// Construction
public:
	CMagnifyWnd();

// Attributes
public:
   
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagnifyWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMagnifyWnd();
	// Generated message map functions
protected:
	//BOOL PointInside();
	void SaveMagnify();
	void RefreshMagnify(BOOL bEraseBackgroud);
	//{{AFX_MSG(CMagnifyWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bCaputred;
	CMagnWnd* pMagnGlass;
	HCURSOR m_curMgnfy;
	CPoint m_CurPoint;
	CBitmap m_bmp;
	
	int m_nShape;
	int m_nZoom;
	int m_nSize;
};

