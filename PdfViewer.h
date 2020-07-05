#pragma once

extern "C" {
	#include "mupdf/fitz.h"
	#include "mupdf/pdf.h"
}

#define MINRES 34
#define MAXRES 360

#define PDFVIEWER_CLASSNAME _T("MFCPdfViewerCtrl")  // Window class name
// CPdfViewer

enum { ARROW, HAND, WAIT, CARET };
//enum { DISCARD, SAVE, CANCEL };

typedef struct pdfapp_s pdfapp_t;
struct pdfapp_s
{
	/* current document params */
	fz_document *doc;
	char *docpath;
	char *doctitle;
	fz_outline *outline;
	int outline_deferred;
	int pagecount;

	fz_stream *m_pSourceStream;
	bool	m_bFromStream;

	/* current view params */
	int resolution;
	int rotate;
	fz_pixmap *image;
	int grayscale;
	fz_colorspace *colorspace;
	int invert;

	/* presentation mode */
	int presentation_mode;
	int transitions_enabled;
	fz_pixmap *old_image;
	fz_pixmap *new_image;
	clock_t start_time;
	int in_transit;
	float duration;
	fz_transition transition;

	/* current page params */
	int pageno;
	fz_page *page;
	fz_rect page_bbox;
	fz_display_list *page_list;
	fz_display_list *annotations_list;
	fz_text_page *page_text;
	fz_text_sheet *page_sheet;
	fz_link *page_links;
	int errored;
	int incomplete;

	/* snapback history */
	int hist[256];
	int histlen;
	int marks[10];

	/* window system sizes */
	int winw, winh;
	int scrw, scrh;
	int shrinkwrap;
	int fullscreen;

	/* event handling state */
	char number[256];
	int numberlen;

	int ispanning;
	int panx, pany;

	int iscopying;
	int selx, sely;

	int beyondy;
	fz_rect selr;

	int nowaitcursor;

	/* search state */
	int isediting;
	int searchdir;
	char search[512];
	int searchpage;
	fz_rect hit_bbox[512];
	int hit_count;

	/* client context storage */
	void *userdata;

	fz_context *ctx;
};


class CPdfViewer : public CWnd
{
	DECLARE_DYNAMIC(CPdfViewer)

public:
	CPdfViewer();
	virtual ~CPdfViewer();

	void LoadFromFile(const char *filename);
	void LoadFromBuffer(unsigned char *buffer, int buffersize, char *password, int reload);
	int GetPdfPageCount();
	void PrintToDC(CDC *pDC, int nPageNo, int nWidth, int nHeight) { };

	BOOL IsZoominEnabled();
	BOOL IsZoomoutEnabled();
	void Zoomin();
	void Zoomout();
	BOOL IsPrevPageEnabled();
	BOOL IsNextPageEnabled();
	void PrevPage();
	void NextPage();
private:

	BITMAPINFO	*m_pDibInfo;
	fz_context *m_ctx;
	pdfapp_t m_pdfapp;

	HCURSOR m_hArrowcurs, m_hHandcurs, m_hWaitcurs, m_hCaretcurs;
	HBRUSH m_hBgbrush;
	HBRUSH m_hShbrush;

	int justcopied;
	int timer_pending;

	int oldx;
	int oldy;

	void pdfapp_init(fz_context *ctx, pdfapp_t *app);
	void pdfapp_viewctm(fz_matrix *mat, pdfapp_t *app);
	void pdfapp_inverthit(pdfapp_t *app);
	void pdfapp_invert(pdfapp_t *app, const fz_rect *rect);
	void pdfapp_postblit(pdfapp_t *app);

	void pdfapp_loadpage(pdfapp_t *app);
	void pdfapp_reloadpage(pdfapp_t *app);

	void pdfapp_open(pdfapp_t *app, char *filename, int reload);	
	void pdfapp_open_progressive(pdfapp_t *app, char *filename, int reload, int bps);

	void pdfapp_showpage(pdfapp_t *app, int loadpage, int drawpage, int repaint, int transition);
	void pdfapp_runpage(pdfapp_t *app, fz_device *dev, const fz_matrix *ctm, const fz_rect *rect, fz_cookie *cookie);

	void pdfapp_updatepage(pdfapp_t *app);
	void pdfapp_recreate_annotationslist(pdfapp_t *app);

	void pdfapp_panview(pdfapp_t *app, int newx, int newy);
	void pdfapp_gotouri(pdfapp_t *app, char *uri);
	void pdfapp_gotopage(pdfapp_t *app, int number);
	void pdfapp_oncopy(pdfapp_t *app, unsigned short *ucsbuf, int ucslen);
	void pdfapp_onresize(pdfapp_t *app, int w, int h);
	void pdfapp_onkey(pdfapp_t *app, int c);
	void pdfapp_onmouse(pdfapp_t *app, int x, int y, int btn, int modifiers, int state);

	void pdfapp_close(pdfapp_t *app);
	void wincursor(pdfapp_t *app, int curs);
	void winblit(pdfapp_t *app,HDC hdc);
	void winadvancetimer(pdfapp_t *app, float delay);
	void windocopy(pdfapp_t *app);
	void handlekey(int c);
	void handlemouse(int x, int y, int btn, int state);
	

protected:
    BOOL RegisterWindowClass();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle=WS_VISIBLE );

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


