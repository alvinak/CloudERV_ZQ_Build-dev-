// PdfViewer.cpp : implementation file
//

#include "stdafx.h"
#include "PdfViewer.h"

#define ZOOMSTEP 1.142857
#define BEYOND_THRESHHOLD 40
#ifndef PATH_MAX
#define PATH_MAX (1024)
#endif

#define MAX_TITLE 256

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

///enum { ARROW, HAND, WAIT, CARET };

enum panning
{
	DONT_PAN = 0,
	PAN_TO_TOP,
	PAN_TO_BOTTOM
};

enum
{
	PDFAPP_OUTLINE_DEFERRED = 1,
	PDFAPP_OUTLINE_LOAD_NOW = 2
};

// CPdfViewer

IMPLEMENT_DYNAMIC(CPdfViewer, CWnd)

CPdfViewer::CPdfViewer()
{
	m_pDibInfo = NULL;
	m_ctx = NULL;

	oldx = 0;
	oldy = 0;

	justcopied = 0;
    RegisterWindowClass();
}

CPdfViewer::~CPdfViewer()
{
	pdfapp_close(&m_pdfapp);

	if (m_pDibInfo)  
		free(m_pDibInfo);
	
	if (m_ctx)
		fz_free_context(m_ctx);

}

BEGIN_MESSAGE_MAP(CPdfViewer, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


BOOL CPdfViewer::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, PDFVIEWER_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = PDFVIEWER_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

void CPdfViewer::LoadFromFile(const char *filename)
{
	pdfapp_close(&m_pdfapp);
	pdfapp_open(&m_pdfapp, (char *) filename, 0);
}


void CPdfViewer::LoadFromBuffer(unsigned char *buffer, int buffersize, char *password, int reload)
{
	const char * types[] = {".pdf", ".jpg", ".xps", 0};
	int nTypeIdx = 0;
	fz_context *ctx = m_pdfapp.ctx;
	pdfapp_t *app = &m_pdfapp;

	pdfapp_close(app);

	if (buffersize > 4) {
		if (buffer[0] == 0x25 && buffer[1] == 0x50 && buffer[2] == 0x44 && buffer[3] == 0x46)
			nTypeIdx = 0;
		else if (buffer[0] == 0xFF && buffer[1] == 0xD8 && buffer[2] == 0xFF && buffer[3] == 0xE0)
			nTypeIdx = 1;
		else if (buffer[0] == 0x50 && buffer[1] == 0x4B && buffer[2] == 0x03 && buffer[3] == 0x04)
			nTypeIdx =  2;
		else
			nTypeIdx = 0;
	}
	else 
		return;


	fz_try(ctx)
	{
		pdf_document *idoc;
		//fz_stream *stream = fz_open_memory(ctx, buffer, buffersize);
		app->m_pSourceStream = fz_open_memory(ctx, buffer, buffersize);
	
		while (1)
		{
			fz_try(ctx)
			{
				fz_seek(app->m_pSourceStream, 0, SEEK_SET);  
				app->doc = fz_open_document_with_stream(ctx, types[nTypeIdx], app->m_pSourceStream);
				app->m_bFromStream = true;
			}
			fz_catch(ctx)
			{
				if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
				{
					//pdfapp_warn(app, "not enough data to open yet");
					continue;
				}
				fz_rethrow(ctx);
			}
			break;
		}
	
		//idoc = pdf_specifics(app->doc);

		//if (idoc)
		//	pdf_set_doc_event_callback(idoc, event_cb, app);

		if (fz_needs_password(app->doc))
		{
			int okay = fz_authenticate_password(app->doc, password);
			/*
			while (!okay)
			{
				password = winpassword(app, filename);
				if (!password)
					fz_throw(ctx, FZ_ERROR_GENERIC, "Needs a password");
				okay = fz_authenticate_password(app->doc, password);
				if (!okay)
					pdfapp_warn(app, "Invalid password.");
			}
			*/
		}

		app->docpath = fz_strdup(ctx, "dummy");
		app->doctitle = fz_strdup(ctx, "dummy");
		
		//filename;
		/*
		if (strrchr(app->doctitle, '\\'))
			app->doctitle = strrchr(app->doctitle, '\\') + 1;
		if (strrchr(app->doctitle, '/'))
			app->doctitle = strrchr(app->doctitle, '/') + 1;
		app->doctitle = fz_strdup(ctx, app->doctitle);
		*/
		while (1)
		{
			fz_try(ctx)
			{
				app->pagecount = fz_count_pages(app->doc);
			}
			fz_catch(ctx)
			{
				if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
				{
					//pdfapp_warn(app, "not enough data to count pages yet");
					continue;
				}
				fz_rethrow(ctx);
			}
			break;
		}
		while (1)
		{
			fz_try(ctx)
			{
				app->outline = fz_load_outline(app->doc);
			}
			fz_catch(ctx)
			{
				if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
				{
					app->outline_deferred = PDFAPP_OUTLINE_DEFERRED;
				}
				else
					fz_rethrow(ctx);
			}
			break;
		}
	}
	fz_catch(ctx)
	{
		//pdfapp_error(app, "cannot open document");
	}

	if (app->pageno < 1)
		app->pageno = 1;
	if (app->pageno > app->pagecount)
		app->pageno = app->pagecount;
	if (app->resolution < MINRES)
		app->resolution = MINRES;
	if (app->resolution > MAXRES)
		app->resolution = MAXRES;

	if (!reload)
	{
		app->shrinkwrap = 1;
		app->rotate = 0;
		app->panx = 0;
		app->pany = 0;
	}

	pdfapp_showpage(app, 1, 1, 1, 0);

}


void CPdfViewer::pdfapp_init(fz_context *ctx, pdfapp_t *app)
{
	memset(app, 0, sizeof(pdfapp_t));
	app->scrw = 640;
	app->scrh = 480;
	app->resolution = 96;
	app->ctx = ctx;
	app->colorspace = fz_device_bgr(ctx);

	app->m_bFromStream = false;
	app->m_pSourceStream = NULL;
}

void CPdfViewer::pdfapp_viewctm(fz_matrix *mat, pdfapp_t *app)
{
	if (app == NULL)
		return;

	fz_pre_rotate(fz_scale(mat, app->resolution/72.0f, app->resolution/72.0f), app->rotate);
}

void CPdfViewer::pdfapp_inverthit(pdfapp_t *app)
{
	fz_rect bbox;
	fz_matrix ctm;
	int i;

	if (app == NULL)
		return;

	pdfapp_viewctm(&ctm, app);

	for (i = 0; i < app->hit_count; i++)
	{
		bbox = app->hit_bbox[i];
		pdfapp_invert(app, fz_transform_rect(&bbox, &ctm));
	}
}

void CPdfViewer::pdfapp_invert(pdfapp_t *app, const fz_rect *rect)
{
	fz_irect b;
	if (app == NULL)
		return;

	fz_invert_pixmap_rect(app->image, fz_round_rect(&b, rect));
}

void CPdfViewer::pdfapp_postblit(pdfapp_t *app)
{
	clock_t time;
	float seconds;
	int llama;

	if (app == NULL)
		return;


	app->transitions_enabled = 1;
	if (!app->in_transit)
		return;
	time = clock();
	seconds = (float)(time - app->start_time) / CLOCKS_PER_SEC;
	llama = seconds * 256 / app->transition.duration;
	if (llama >= 256)
	{
		/* Completed. */
		fz_drop_pixmap(app->ctx, app->image);
		app->image = app->new_image;
		app->new_image = NULL;
		fz_drop_pixmap(app->ctx, app->old_image);
		app->old_image = NULL;
		if (app->duration != 0)
			winadvancetimer(app, app->duration);
	}
	else
		fz_generate_transition(app->image, app->old_image, app->new_image, llama, &app->transition);
	InvalidateRect(NULL);
	if (llama >= 256)
	{
		/* Completed. */
		app->in_transit = 0;
	}
}


void CPdfViewer::pdfapp_loadpage(pdfapp_t *app)
{
	fz_device *mdev = NULL;
	int errored = 0;
	fz_cookie cookie = { 0 };

	if (app == NULL)
		return;

	fz_var(mdev);

	fz_drop_display_list(app->ctx, app->page_list);
	fz_drop_display_list(app->ctx, app->annotations_list);
	fz_free_text_page(app->ctx, app->page_text);
	fz_free_text_sheet(app->ctx, app->page_sheet);
	fz_drop_link(app->ctx, app->page_links);
	fz_free_page(app->doc, app->page);

	app->page_list = NULL;
	app->annotations_list = NULL;
	app->page_text = NULL;
	app->page_sheet = NULL;
	app->page_links = NULL;
	app->page = NULL;
	app->page_bbox.x0 = 0;
	app->page_bbox.y0 = 0;
	app->page_bbox.x1 = 100;
	app->page_bbox.y1 = 100;

	app->incomplete = 0;

	fz_try(app->ctx)
	{
		app->page = fz_load_page(app->doc, app->pageno - 1);

		fz_bound_page(app->doc, app->page, &app->page_bbox);
	}
	fz_catch(app->ctx)
	{
		if (fz_caught(app->ctx) == FZ_ERROR_TRYLATER)
			app->incomplete = 1;
		else
			///pdfapp_warn(app, "Cannot load page");
		return;
	}

	fz_try(app->ctx)
	{
		fz_annot *annot;
		/* Create display lists */
		app->page_list = fz_new_display_list(app->ctx);
		mdev = fz_new_list_device(app->ctx, app->page_list);
		cookie.incomplete_ok = 1;
		fz_run_page_contents(app->doc, app->page, mdev, &fz_identity, &cookie);
		fz_free_device(mdev);
		mdev = NULL;
		app->annotations_list = fz_new_display_list(app->ctx);
		mdev = fz_new_list_device(app->ctx, app->annotations_list);
		for (annot = fz_first_annot(app->doc, app->page); annot; annot = fz_next_annot(app->doc, annot))
			fz_run_annot(app->doc, app->page, annot, mdev, &fz_identity, &cookie);
		if (cookie.incomplete)
		{
			app->incomplete = 1;
			//pdfapp_warn(app, "Incomplete page rendering");
		}
		else if (cookie.errors)
		{
			///pdfapp_warn(app, "Errors found on page");
			errored = 1;
		}
	}
	fz_always(app->ctx)
	{
		fz_free_device(mdev);
	}
	fz_catch(app->ctx)
	{
		if (fz_caught(app->ctx) == FZ_ERROR_TRYLATER)
			app->incomplete = 1;
		else
		{
			///pdfapp_warn(app, "Cannot load page");
			errored = 1;
		}
	}

	fz_try(app->ctx)
	{
		app->page_links = fz_load_links(app->doc, app->page);
	}
	fz_catch(app->ctx)
	{
		if (fz_caught(app->ctx) == FZ_ERROR_TRYLATER)
			app->incomplete = 1;
		///else if (!errored)
		//	pdfapp_warn(app, "Cannot load page");
	}

	app->errored = errored;
}

void CPdfViewer::pdfapp_reloadpage(pdfapp_t *app)
{
	if (app == NULL)
		return;

	if (app->outline_deferred == PDFAPP_OUTLINE_LOAD_NOW)
	{
		fz_try(app->ctx)
		{
			app->outline = fz_load_outline(app->doc);
		}
		fz_catch(app->ctx)
		{
			/* Ignore any error now */
		}
		app->outline_deferred = 0;
	}
	pdfapp_showpage(app, 1, 1, 1, 0);
}


void CPdfViewer::pdfapp_open(pdfapp_t *app, char *filename, int reload)	
{
	if (app == NULL)
		return;

	pdfapp_open_progressive(app, filename, reload, 0);
}

void CPdfViewer::pdfapp_open_progressive(pdfapp_t *app, char *filename, int reload, int bps)
{
	if (app == NULL)
		return;

	fz_context *ctx = app->ctx;
	char *password = "";

	fz_try(ctx)
	{
		pdf_document *idoc;

		if (bps == 0)
		{
			app->doc = fz_open_document(ctx, filename);
		}
		else
		{
			fz_stream *stream = fz_open_file_progressive(ctx, filename, bps);
			while (1)
			{
				fz_try(ctx)
				{
					fz_seek(stream, 0, SEEK_SET);
					app->doc = fz_open_document_with_stream(ctx, filename, stream);
				}
				fz_catch(ctx)
				{
					if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
					{
						///pdfapp_warn(app, "not enough data to open yet");
						continue;
					}
					fz_rethrow(ctx);
				}
				break;
			}
		}

		///idoc = pdf_specifics(app->doc);

		///if (idoc)
		///	pdf_set_doc_event_callback(idoc, event_cb, app);

		/*
		if (fz_needs_password(app->doc))
		{
			int okay = fz_authenticate_password(app->doc, password);
			while (!okay)
			{
				password = winpassword(app, filename);
				if (!password)
					fz_throw(ctx, FZ_ERROR_GENERIC, "Needs a password");
				okay = fz_authenticate_password(app->doc, password);
				if (!okay)
					///pdfapp_warn(app, "Invalid password.");
			}
		}
		*/

		app->docpath = fz_strdup(ctx, filename);
		app->doctitle = filename;
		
		if (strrchr(app->doctitle, '\\'))
			app->doctitle = strrchr(app->doctitle, '\\') + 1;
		if (strrchr(app->doctitle, '/'))
			app->doctitle = strrchr(app->doctitle, '/') + 1;
		app->doctitle = fz_strdup(ctx, app->doctitle);

		while (1)
		{
			fz_try(ctx)
			{
				app->pagecount = fz_count_pages(app->doc);
			}
			fz_catch(ctx)
			{
				if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
				{
					///pdfapp_warn(app, "not enough data to count pages yet");
					continue;
				}
				fz_rethrow(ctx);
			}
			break;
		}

		while (1)
		{
			fz_try(ctx)
			{
				app->outline = fz_load_outline(app->doc);
			}
			fz_catch(ctx)
			{
				if (fz_caught(ctx) == FZ_ERROR_TRYLATER)
				{
					app->outline_deferred = PDFAPP_OUTLINE_DEFERRED;
				}
				else
					fz_rethrow(ctx);
			}
			break;
		}
	}
	fz_catch(ctx)
	{
		///pdfapp_error(app, "cannot open document");
	}

	if (app->pageno < 1)
		app->pageno = 1;
	if (app->pageno > app->pagecount)
		app->pageno = app->pagecount;
	if (app->resolution < MINRES)
		app->resolution = MINRES;
	if (app->resolution > MAXRES)
		app->resolution = MAXRES;

	if (!reload)
	{
		app->shrinkwrap = 1;
		app->rotate = 0;
		app->panx = 0;
		app->pany = 0;
	}

	pdfapp_showpage(app, 1, 1, 1, 0);
}

void CPdfViewer::pdfapp_showpage(pdfapp_t *app, int loadpage, int drawpage, int repaint, int transition)
{
	char buf[MAX_TITLE];
	fz_device *idev;
	fz_device *tdev;
	fz_colorspace *colorspace;
	fz_matrix ctm;
	fz_rect bounds;
	fz_irect ibounds;
	fz_cookie cookie = { 0 };

	if (app == NULL)
		return;

	if (!app->nowaitcursor)
		wincursor(app, WAIT);

	if (!app->transitions_enabled || !app->presentation_mode)
		transition = 0;

	if (transition)
	{
		app->old_image = app->image;
		app->image = NULL;
	}

	if (loadpage)
	{
		pdfapp_loadpage(app);

		/* Zero search hit position */
		app->hit_count = 0;

		/* Extract text */
		app->page_sheet = fz_new_text_sheet(app->ctx);
		app->page_text = fz_new_text_page(app->ctx);

		if (app->page_list || app->annotations_list)
		{
			tdev = fz_new_text_device(app->ctx, app->page_sheet, app->page_text);
			pdfapp_runpage(app, tdev, &fz_identity, &fz_infinite_rect, &cookie);
			fz_free_device(tdev);
		}
	}

	if (drawpage)
	{
		char buf2[64];
		int len;

		sprintf(buf2, " - %d/%d (%d dpi)",
				app->pageno, app->pagecount, app->resolution);
		len = MAX_TITLE-strlen(buf2);
		if ((int)strlen(app->doctitle) > len)
		{
			snprintf(buf, len-3, "%s", app->doctitle);
			strcat(buf, "...");
			strcat(buf, buf2);
		}
		else
			sprintf(buf, "%s%s", app->doctitle, buf2);

		///wintitle(app, buf);

		pdfapp_viewctm(&ctm, app);
		bounds = app->page_bbox;
		fz_round_rect(&ibounds, fz_transform_rect(&bounds, &ctm));
		fz_rect_from_irect(&bounds, &ibounds);

		/* Draw */
		if (app->image)
			fz_drop_pixmap(app->ctx, app->image);
		if (app->grayscale)
			colorspace = fz_device_gray(app->ctx);
		else
			colorspace = app->colorspace;
		app->image = NULL;
		app->image = fz_new_pixmap_with_bbox(app->ctx, colorspace, &ibounds);
		fz_clear_pixmap_with_value(app->ctx, app->image, 255);
		if (app->page_list || app->annotations_list)
		{
			idev = fz_new_draw_device(app->ctx, app->image);
			pdfapp_runpage(app, idev, &ctm, &bounds, &cookie);
			fz_free_device(idev);
		}
		if (app->invert)
			fz_invert_pixmap(app->ctx, app->image);
	}

	if (transition)
	{
		fz_transition *new_trans;
		app->new_image = app->image;
		app->image = NULL;
		app->image = fz_new_pixmap_with_bbox(app->ctx, colorspace, &ibounds);
		app->duration = 0;
		new_trans = fz_page_presentation(app->doc, app->page, &app->duration);
		if (new_trans)
			app->transition = *new_trans;
		else
		{
			/* If no transition specified, use a default one */
			memset(&app->transition, 0, sizeof(*new_trans));
			app->transition.duration = 1.0;
			app->transition.type = FZ_TRANSITION_WIPE;
			app->transition.vertical = 0;
			app->transition.direction = 0;
		}
		if (app->duration == 0)
			app->duration = 5;
		app->in_transit = fz_generate_transition(app->image, app->old_image, app->new_image, 0, &app->transition);
		if (!app->in_transit)
		{
			///if (app->duration != 0)
			///	winadvancetimer(app, app->duration);
		}
		app->start_time = clock();
	}

	if (repaint)
	{
		pdfapp_panview(app, app->panx, app->pany);

		if (app->shrinkwrap)
		{
			int w = fz_pixmap_width(app->ctx, app->image);
			int h = fz_pixmap_height(app->ctx, app->image);
			if (app->winw == w)
				app->panx = 0;
			if (app->winh == h)
				app->pany = 0;
			if (w > app->scrw * 90 / 100)
				w = app->scrw * 90 / 100;
			if (h > app->scrh * 90 / 100)
				h = app->scrh * 90 / 100;
			///if (w != app->winw || h != app->winh)
				///winresize(app, w, h);
		}

		InvalidateRect(NULL);

		wincursor(app, ARROW);
	}

	if (cookie.errors && app->errored == 0)
	{
		app->errored = 1;
		//pdfapp_warn(app, "Errors found on page. Page rendering may be incomplete.");
	}

	fz_flush_warnings(app->ctx);
}

void CPdfViewer::pdfapp_runpage(pdfapp_t *app, fz_device *dev, const fz_matrix *ctm, const fz_rect *rect, fz_cookie *cookie)
{
	if (app == NULL)
		return;

	fz_begin_page(dev, rect, ctm);
	if (app->page_list)
		fz_run_display_list(app->page_list, dev, ctm, rect, cookie);
	if (app->annotations_list)
		fz_run_display_list(app->annotations_list, dev, ctm, rect, cookie);
	fz_end_page(dev);
}

void CPdfViewer::pdfapp_updatepage(pdfapp_t *app)
{
	if (app == NULL)
		return;

	pdf_document *idoc = pdf_specifics(app->doc);
	fz_device *idev;
	fz_matrix ctm;
	fz_annot *annot;

	pdfapp_viewctm(&ctm, app);
	pdf_update_page(idoc, (pdf_page *)app->page);
	pdfapp_recreate_annotationslist(app);

	while ((annot = (fz_annot *)pdf_poll_changed_annot(idoc, (pdf_page *)app->page)) != NULL)
	{
		fz_rect bounds;
		fz_irect ibounds;
		fz_transform_rect(fz_bound_annot(app->doc, annot, &bounds), &ctm);
		fz_rect_from_irect(&bounds, fz_round_rect(&ibounds, &bounds));
		fz_clear_pixmap_rect_with_value(app->ctx, app->image, 255, &ibounds);
		idev = fz_new_draw_device_with_bbox(app->ctx, app->image, &ibounds);
		pdfapp_runpage(app, idev, &ctm, &bounds, NULL);
		fz_free_device(idev);
	}

	pdfapp_showpage(app, 0, 0, 1, 0);
}


void CPdfViewer::pdfapp_recreate_annotationslist(pdfapp_t *app)
{
	fz_device *mdev = NULL;
	int errored = 0;
	fz_cookie cookie = { 0 };

	if (app == NULL)
		return;


	fz_var(mdev);

	fz_drop_display_list(app->ctx, app->annotations_list);
	app->annotations_list = NULL;

	fz_try(app->ctx)
	{
		fz_annot *annot;
		/* Create display list */
		app->annotations_list = fz_new_display_list(app->ctx);
		mdev = fz_new_list_device(app->ctx, app->annotations_list);
		for (annot = fz_first_annot(app->doc, app->page); annot; annot = fz_next_annot(app->doc, annot))
			fz_run_annot(app->doc, app->page, annot, mdev, &fz_identity, &cookie);
		if (cookie.incomplete)
		{
			app->incomplete = 1;
			//pdfapp_warn(app, "Incomplete page rendering");
		}
		else if (cookie.errors)
		{
			//pdfapp_warn(app, "Errors found on page");
			errored = 1;
		}
	}
	fz_always(app->ctx)
	{
		fz_free_device(mdev);
	}
	fz_catch(app->ctx)
	{
		//pdfapp_warn(app, "Cannot load page");
		errored = 1;
	}

	app->errored = errored;
}

void CPdfViewer::pdfapp_panview(pdfapp_t *app, int newx, int newy)
{
	if (app == NULL)
		return;

	int image_w = fz_pixmap_width(app->ctx, app->image);
	int image_h = fz_pixmap_height(app->ctx, app->image);

	if (newx > 0)
		newx = 0;
	if (newy > 0)
		newy = 0;

	if (newx + image_w < app->winw)
		newx = app->winw - image_w;
	if (newy + image_h < app->winh)
		newy = app->winh - image_h;

	if (app->winw >= image_w)
		newx = (app->winw - image_w) / 2;
	if (app->winh >= image_h)
		newy = (app->winh - image_h) / 2;

	if (newx != app->panx || newy != app->pany)
		InvalidateRect(NULL);
	
	app->panx = newx;
	app->pany = newy;
}

void CPdfViewer::pdfapp_gotouri(pdfapp_t *app, char *uri)
{
	if (app == NULL)
		return;

	//winopenuri(app, uri);
}

void CPdfViewer::pdfapp_gotopage(pdfapp_t *app, int number)
{
	if (app == NULL)
		return;

	app->isediting = 0;
	InvalidateRect(NULL);

	if (app->histlen + 1 == 256)
	{
		memmove(app->hist, app->hist + 1, sizeof(int) * 255);
		app->histlen --;
	}
	app->hist[app->histlen++] = app->pageno;
	app->pageno = number + 1;
	pdfapp_showpage(app, 1, 1, 1, 0);
}



void CPdfViewer::pdfapp_onresize(pdfapp_t *app, int w, int h)
{
	if (app == NULL)
		return;

	if (app->winw != w || app->winh != h)
	{
		app->winw = w;
		app->winh = h;
		pdfapp_panview(app, app->panx, app->pany);
		InvalidateRect(NULL);
	}
}

void CPdfViewer::pdfapp_oncopy(pdfapp_t *app, unsigned short *ucsbuf, int ucslen)
{
	if (app == NULL)
		return;

	fz_rect hitbox;
	fz_matrix ctm;
	fz_text_page *page = app->page_text;
	int c, i, p;
	int seen = 0;
	int block_num;

	int x0 = app->selr.x0;
	int x1 = app->selr.x1;
	int y0 = app->selr.y0;
	int y1 = app->selr.y1;

	pdfapp_viewctm(&ctm, app);

	p = 0;

	for (block_num = 0; block_num < page->len; block_num++)
	{
		fz_text_line *line;
		fz_text_block *block;
		fz_text_span *span;

		if (page->blocks[block_num].type != FZ_PAGE_BLOCK_TEXT)
			continue;
		block = page->blocks[block_num].u.text;

		for (line = block->lines; line < block->lines + block->len; line++)
		{
			for (span = line->first_span; span; span = span->next)
			{
				if (seen)
				{
#ifdef _WIN32
					if (p < ucslen - 1)
						ucsbuf[p++] = '\r';
#endif
					if (p < ucslen - 1)
						ucsbuf[p++] = '\n';
				}

				seen = 0;

				for (i = 0; i < span->len; i++)
				{
					fz_text_char_bbox(&hitbox, span, i);
					fz_transform_rect(&hitbox, &ctm);
					c = span->text[i].c;
					if (c < 32)
						c = '?';
					if (hitbox.x1 >= x0 && hitbox.x0 <= x1 && hitbox.y1 >= y0 && hitbox.y0 <= y1)
					{
						if (p < ucslen - 1)
							ucsbuf[p++] = c;
						seen = 1;
					}
				}

				seen = (seen && span == line->last_span);
			}
		}
	}

	ucsbuf[p] = 0;
}

void CPdfViewer::pdfapp_onkey(pdfapp_t *app, int c)
{
	int oldpage = app->pageno;
	enum panning panto = PAN_TO_TOP;
	int loadpage = 1;

	if (app == NULL)
		return;

	if (app->pagecount <= 0) return;

	if (app->isediting)
	{
		int n = strlen(app->search);
		if (c < ' ')
		{
			if (c == '\b' && n > 0)
			{
				app->search[n - 1] = 0;
				///winrepaintsearch(app);
			}
			if (c == '\n' || c == '\r')
			{
				app->isediting = 0;
				if (n > 0)
				{
					///winrepaintsearch(app);

					if (app->searchdir < 0)
					{
						if (app->pageno == 1)
							app->pageno = app->pagecount;
						else
							app->pageno--;
						pdfapp_showpage(app, 1, 1, 0, 0);
					}

					pdfapp_onkey(app, 'n');
				}
				else
					InvalidateRect(NULL);
			}
			if (c == '\033')
			{
				app->isediting = 0;
				InvalidateRect(NULL);
			}
		}
		else
		{
			if (n + 2 < sizeof app->search)
			{
				app->search[n] = c;
				app->search[n + 1] = 0;
				//winrepaintsearch(app);
			}
		}
		return;
	}

	/*
	 * Save numbers typed for later
	 */

	if (c >= '0' && c <= '9')
	{
		app->number[app->numberlen++] = c;
		app->number[app->numberlen] = '\0';
	}

	switch (c)
	{

	//case 'q':
	//	winclose(app);
	//	break;

	/*
	 * Zoom and rotate
	 */

	case '+':
	case '=':
		app->resolution *= ZOOMSTEP;
		if (app->resolution > MAXRES)
			app->resolution = MAXRES;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;
	case '-':
		app->resolution /= ZOOMSTEP;
		if (app->resolution < MINRES)
			app->resolution = MINRES;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;

	case 'W':
		app->resolution *= (double) app->winw / (double) fz_pixmap_width(app->ctx, app->image);
		if (app->resolution > MAXRES)
			app->resolution = MAXRES;
		else if (app->resolution < MINRES)
			app->resolution = MINRES;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;
	case 'H':
		app->resolution *= (double) app->winh / (double) fz_pixmap_height(app->ctx, app->image);
		if (app->resolution > MAXRES)
			app->resolution = MAXRES;
		else if (app->resolution < MINRES)
			app->resolution = MINRES;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;

	case 'L':
		app->rotate -= 90;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;
	case 'R':
		app->rotate += 90;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;

	case 'c':
		app->grayscale ^= 1;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;

	case 'i':
		app->invert ^= 1;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;

#ifndef NDEBUG
	case 'a':
		app->rotate -= 15;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;
	case 's':
		app->rotate += 15;
		pdfapp_showpage(app, 0, 1, 1, 0);
		break;
#endif

	/*
	 * Pan view, but don't need to repaint image
	 */
		/*
	case 'f':
		app->shrinkwrap = 0;
		winfullscreen(app, !app->fullscreen);
		app->fullscreen = !app->fullscreen;
		break;

	case 'w':
		if (app->fullscreen)
		{
			winfullscreen(app, 0);
			app->fullscreen = 0;
		}
		app->shrinkwrap = 1;
		app->panx = app->pany = 0;
		pdfapp_showpage(app, 0, 0, 1, 0);
		break;
		*/
	case 'h':
		app->panx += fz_pixmap_width(app->ctx, app->image) / 10;
		pdfapp_showpage(app, 0, 0, 1, 0);
		break;

	case 'j':
		app->pany -= fz_pixmap_height(app->ctx, app->image) / 10;
		pdfapp_showpage(app, 0, 0, 1, 0);
		break;

	case 'k':
		app->pany += fz_pixmap_height(app->ctx, app->image) / 10;
		pdfapp_showpage(app, 0, 0, 1, 0);
		break;

	case 'l':
		app->panx -= fz_pixmap_width(app->ctx, app->image) / 10;
		pdfapp_showpage(app, 0, 0, 1, 0);
		break;

	/*
	 * Page navigation
	 */

	case 'g':
	case '\n':
	case '\r':
		if (app->numberlen > 0)
			app->pageno = atoi(app->number);
		else
			app->pageno = 1;
		break;

	case 'G':
		app->pageno = app->pagecount;
		break;

	case 'm':
		if (app->numberlen > 0)
		{
			int idx = atoi(app->number);

			if (idx >= 0 && idx < nelem(app->marks))
				app->marks[idx] = app->pageno;
		}
		else
		{
			if (app->histlen + 1 == 256)
			{
				memmove(app->hist, app->hist + 1, sizeof(int) * 255);
				app->histlen --;
			}
			app->hist[app->histlen++] = app->pageno;
		}
		break;

	case 't':
		if (app->numberlen > 0)
		{
			int idx = atoi(app->number);

			if (idx >= 0 && idx < nelem(app->marks))
				if (app->marks[idx] > 0)
					app->pageno = app->marks[idx];
		}
		else if (app->histlen > 0)
			app->pageno = app->hist[--app->histlen];
		break;

	case 'p':
		app->presentation_mode = !app->presentation_mode;
		break;

	/*
	 * Back and forth ...
	 */

	case ',':
		panto = PAN_TO_BOTTOM;
		if (app->numberlen > 0)
			app->pageno -= atoi(app->number);
		else
			app->pageno--;
		break;

	case '.':
		panto = PAN_TO_TOP;
		if (app->numberlen > 0)
			app->pageno += atoi(app->number);
		else
			app->pageno++;
		break;

	case '\b':
	case 'b':
		panto = DONT_PAN;
		if (app->numberlen > 0)
			app->pageno -= atoi(app->number);
		else
			app->pageno--;
		break;

	case ' ':
		panto = DONT_PAN;
		if (app->numberlen > 0)
			app->pageno += atoi(app->number);
		else
			app->pageno++;
		break;

	case '<':
		panto = PAN_TO_TOP;
		app->pageno -= 10;
		break;
	case '>':
		panto = PAN_TO_TOP;
		app->pageno += 10;
		break;

	/*
	 * Saving the file
	 */
	//case 'S':
	//	pdfapp_save(app);
	//	break;

	/*
	 * Reloading the file...
	 */

	//case 'r':
	//	panto = DONT_PAN;
	//	oldpage = -1;
	//	winreloadfile(app);
	//	break;

	/*
	 * Searching
	 */

	case '?':
		app->isediting = 1;
		app->searchdir = -1;
		app->search[0] = 0;
		app->hit_count = 0;
		app->searchpage = -1;
		//winrepaintsearch(app);
		break;

	case '/':
		app->isediting = 1;
		app->searchdir = 1;
		app->search[0] = 0;
		app->hit_count = 0;
		app->searchpage = -1;
		//winrepaintsearch(app);
		break;
/*
	case 'n':
		if (app->searchdir > 0)
			pdfapp_search_in_direction(app, &panto, 1);
		else
			pdfapp_search_in_direction(app, &panto, -1);
		loadpage = 0;
		break;

	case 'N':
		if (app->searchdir > 0)
			pdfapp_search_in_direction(app, &panto, -1);
		else
			pdfapp_search_in_direction(app, &panto, 1);
		loadpage = 0;
		break;
		*/
	}

	if (c < '0' || c > '9')
		app->numberlen = 0;

	if (app->pageno < 1)
		app->pageno = 1;
	if (app->pageno > app->pagecount)
		app->pageno = app->pagecount;

	if (app->pageno != oldpage)
	{
		switch (panto)
		{
		case PAN_TO_TOP:
			app->pany = 0;
			break;
		case PAN_TO_BOTTOM:
			app->pany = -2000;
			break;
		case DONT_PAN:
			break;
		}
		pdfapp_showpage(app, loadpage, 1, 1, 1);
	}
}


void CPdfViewer::pdfapp_onmouse(pdfapp_t *app, int x, int y, int btn, int modifiers, int state)
{
	fz_context *ctx = app->ctx;
	fz_irect irect;
	fz_link *link;
	fz_matrix ctm;
	fz_point p;
	int processed = 0;

	if (app == NULL)
		return;

	if (app->pagecount <= 0) return;

	fz_pixmap_bbox(app->ctx, app->image, &irect);
	p.x = x - app->panx + irect.x0;
	p.y = y - app->pany + irect.y0;

	pdfapp_viewctm(&ctm, app);
	fz_invert_matrix(&ctm, &ctm);

	fz_transform_point(&p, &ctm);

	if (btn == 1 && (state == 1 || state == -1))
	{
		pdf_ui_event event;
		pdf_document *idoc = pdf_specifics(app->doc);

		event.etype = PDF_EVENT_TYPE_POINTER;
		event.event.pointer.pt = p;
		if (state == 1)
			event.event.pointer.ptype = PDF_POINTER_DOWN;
		else /* state == -1 */
			event.event.pointer.ptype = PDF_POINTER_UP;

		if (idoc && pdf_pass_event(idoc, (pdf_page *)app->page, &event))
		{
			pdf_widget *widget;

			widget = pdf_focused_widget(idoc);

			app->nowaitcursor = 1;
			pdfapp_updatepage(app);

			if (widget)
			{
				switch (pdf_widget_get_type(widget))
				{
				case PDF_WIDGET_TYPE_TEXT:
					{
						/*
						char *text = pdf_text_widget_text(idoc, widget);
						char *current_text = text;
						int retry = 0;

						do
						{
							current_text = wintextinput(app, current_text, retry);
							retry = 1;
						}
						while (current_text && !pdf_text_widget_set_text(idoc, widget, current_text));

						fz_free(app->ctx, text);
						pdfapp_updatepage(app);
						*/
					}
					break;

				case PDF_WIDGET_TYPE_LISTBOX:
				case PDF_WIDGET_TYPE_COMBOBOX:
					{
						/*
						int nopts;
						int nvals;
						char **opts = NULL;
						char **vals = NULL;

						fz_var(opts);
						fz_var(vals);

						fz_try(ctx)
						{
							nopts = pdf_choice_widget_options(idoc, widget, NULL);
							opts = (char **) fz_malloc(ctx, nopts * sizeof(*opts));
							(void)pdf_choice_widget_options(idoc, widget, opts);

							nvals = pdf_choice_widget_value(idoc, widget, NULL);
							vals = fz_malloc(ctx, MAX(nvals,nopts) * sizeof(*vals));
							(void)pdf_choice_widget_value(idoc, widget, vals);

							if (winchoiceinput(app, nopts, opts, &nvals, vals))
							{
								pdf_choice_widget_set_value(idoc, widget, nvals, vals);
								pdfapp_updatepage(app);
							}
						}
						fz_always(ctx)
						{
							fz_free(ctx, opts);
							fz_free(ctx, vals);
						}
						fz_catch(ctx)
						{
							//pdfapp_warn(app, "setting of choice failed");
						}
						*/
					}
					break;

				case PDF_WIDGET_TYPE_SIGNATURE:
					{
						char ebuf[256];

						ebuf[0] = 0;
						if (pdf_check_signature(idoc, widget, app->docpath, ebuf, sizeof(ebuf)))
						{
							//winwarn(app, "Signature is valid");
						}
						else
						{
							/*
							if (ebuf[0] == 0)
								winwarn(app, "Signature check failed for unknown reason");
							else
								winwarn(app, ebuf);
							*/
						}
					}
					break;
				}
			}

			app->nowaitcursor = 0;
			processed = 1;
		}
	}

	for (link = app->page_links; link; link = link->next)
	{
		if (p.x >= link->rect.x0 && p.x <= link->rect.x1)
			if (p.y >= link->rect.y0 && p.y <= link->rect.y1)
				break;
	}

	if (link)
	{
		wincursor(app, HAND);
		if (btn == 1 && state == 1 && !processed)
		{
			if (link->dest.kind == FZ_LINK_URI)
				pdfapp_gotouri(app, link->dest.ld.uri.uri);
			else if (link->dest.kind == FZ_LINK_GOTO)
				pdfapp_gotopage(app, link->dest.ld.gotor.page);
			return;
		}
	}
	else
	{
		fz_annot *annot;
		for (annot = fz_first_annot(app->doc, app->page); annot; annot = fz_next_annot(app->doc, annot))
		{
			fz_rect rect;
			fz_bound_annot(app->doc, annot, &rect);
			if (x >= rect.x0 && x < rect.x1)
				if (y >= rect.y0 && y < rect.y1)
					break;
		}
		if (annot)
			wincursor(app, CARET);
		else
			wincursor(app, ARROW);
	}

	if (state == 1 && !processed)
	{
		if (btn == 1 && !app->iscopying)
		{
			app->ispanning = 1;
			app->selx = x;
			app->sely = y;
			app->beyondy = 0;
		}
		if (btn == 3 && !app->ispanning)
		{
			app->iscopying = 1;
			app->selx = x;
			app->sely = y;
			app->selr.x0 = x;
			app->selr.x1 = x;
			app->selr.y0 = y;
			app->selr.y1 = y;
		}
		if (btn == 4 || btn == 5) /* scroll wheel */
		{
			int dir = btn == 4 ? 1 : -1;
			app->ispanning = app->iscopying = 0;
			
			if (modifiers & (1<<2))
			{
				/* zoom in/out if ctrl is pressed */
				if (dir > 0)
					app->resolution *= ZOOMSTEP;
				else
					app->resolution /= ZOOMSTEP;

				if (app->resolution > MAXRES)
					app->resolution = MAXRES;
				if (app->resolution < MINRES)
					app->resolution = MINRES;
				pdfapp_showpage(app, 0, 1, 1, 0);
			}
			else
			{
				/* scroll up/down, or left/right if
				shift is pressed */
				int isx = (modifiers & (1<<0));
				int xstep = isx ? 20 * dir : 0;
				int ystep = !isx ? 20 * dir : 0;
				pdfapp_panview(app, app->panx + xstep, app->pany + ystep);
			}
		}
	}

	else if (state == -1)
	{
		if (app->iscopying)
		{
			app->iscopying = 0;
			app->selr.x0 = fz_mini(app->selx, x) - app->panx + irect.x0;
			app->selr.x1 = fz_maxi(app->selx, x) - app->panx + irect.x0;
			app->selr.y0 = fz_mini(app->sely, y) - app->pany + irect.y0;
			app->selr.y1 = fz_maxi(app->sely, y) - app->pany + irect.y0;
			InvalidateRect(NULL);
			if (app->selr.x0 < app->selr.x1 && app->selr.y0 < app->selr.y1)
				windocopy(app);
		}
		app->ispanning = 0;
	}

	else if (app->ispanning)
	{
		int newx = app->panx + x - app->selx;
		int newy = app->pany + y - app->sely;
		/* Scrolling beyond limits implies flipping pages */
		/* Are we requested to scroll beyond limits? */
		if (newy + fz_pixmap_height(app->ctx, app->image) < app->winh || newy > 0)
		{
			/* Yes. We can assume that deltay != 0 */
			int deltay = y - app->sely;
			/* Check whether the panning has occurred in the
			 * direction that we are already crossing the
			 * limit it. If not, we can conclude that we
			 * have switched ends of the page and will thus
			 * start over counting.
			 */
			if( app->beyondy == 0 || (app->beyondy ^ deltay) >= 0 )
			{
				/* Updating how far we are beyond and
				 * flipping pages if beyond threshold
				 */
				app->beyondy += deltay;
				if (app->beyondy > BEYOND_THRESHHOLD)
				{
					if( app->pageno > 1 )
					{
						app->pageno--;
						pdfapp_showpage(app, 1, 1, 1, 0);
						newy = -fz_pixmap_height(app->ctx, app->image);
					}
					app->beyondy = 0;
				}
				else if (app->beyondy < -BEYOND_THRESHHOLD)
				{
					if( app->pageno < app->pagecount )
					{
						app->pageno++;
						pdfapp_showpage(app, 1, 1, 1, 0);
						newy = 0;
					}
					app->beyondy = 0;
				}
			}
			else
				app->beyondy = 0;
		}
		/* Although at this point we've already determined that
		 * or that no scrolling will be performed in
		 * y-direction, the x-direction has not yet been taken
		 * care off. Therefore
		 */
		pdfapp_panview(app, newx, newy);

		app->selx = x;
		app->sely = y;
	}

	else if (app->iscopying)
	{
		app->selr.x0 = fz_mini(app->selx, x) - app->panx + irect.x0;
		app->selr.x1 = fz_maxi(app->selx, x) - app->panx + irect.x0;
		app->selr.y0 = fz_mini(app->sely, y) - app->pany + irect.y0;
		app->selr.y1 = fz_maxi(app->sely, y) - app->pany + irect.y0;
		InvalidateRect(NULL);
	}
}

void CPdfViewer::pdfapp_close(pdfapp_t *app)
{
	if (app == NULL) 
		return;

	fz_drop_display_list(app->ctx, app->page_list);
	app->page_list = NULL;

	fz_drop_display_list(app->ctx, app->annotations_list);
	app->annotations_list = NULL;

	fz_free_text_page(app->ctx, app->page_text);
	app->page_text = NULL;

	fz_free_text_sheet(app->ctx, app->page_sheet);
	app->page_sheet = NULL;

	fz_drop_link(app->ctx, app->page_links);
	app->page_links = NULL;

	fz_free(app->ctx, app->doctitle);
	app->doctitle = NULL;

	fz_free(app->ctx, app->docpath);
	app->docpath = NULL;

	fz_drop_pixmap(app->ctx, app->image);
	app->image = NULL;

	fz_drop_pixmap(app->ctx, app->new_image);
	app->new_image = NULL;

	fz_drop_pixmap(app->ctx, app->old_image);
	app->old_image = NULL;

	fz_free_outline(app->ctx, app->outline);
	app->outline = NULL;

    if(app->m_bFromStream && app->m_pSourceStream) {
		fz_close(app->m_pSourceStream);
        app->m_bFromStream = false;
        app->m_pSourceStream = NULL;
    }

	fz_free_page(app->doc, app->page);
	app->page = NULL;
	
	fz_close_document(app->doc);
	app->doc = NULL;


	fz_flush_warnings(app->ctx);
}


void CPdfViewer::wincursor(pdfapp_t *app, int curs)
{
	if (curs == ARROW)
		SetCursor(m_hArrowcurs);
	if (curs == HAND)
		SetCursor(m_hHandcurs);
	if (curs == WAIT)
		SetCursor(m_hWaitcurs);
	if (curs == CARET)
		SetCursor(m_hCaretcurs);
}

/*
BOOL CPdfViewer::GetImageInfo(int nPageNo, int &nImageWidth, int &nImageHeight, int &nComponents, unsigned char **lpBits)
{
	pdfapp_t *app = &m_pdfapp;
	unsigned char *samples;
	int nOldPage;

	if (nPageNo < 1 || nPageNo > app->pagecount)
		return FALSE;

	nOldPage = app->pageno;
	app->pageno = nPageNo;

	pdfapp_showpage(app, 1, 0, 0, 0);
	
	nImageWidth = fz_pixmap_width(app->ctx, app->image);
	nImageHeight = fz_pixmap_height(app->ctx, app->image);
	nComponents = fz_pixmap_components(app->ctx, app->image);
	unsigned char *samples = fz_pixmap_samples(app->ctx, app->image);
	
	if (app->image)
	{
		pDibInfo = (BITMAPINFO *) malloc(sizeof(BITMAPINFO) + 12);

		pDibInfo->bmiHeader.biSize = sizeof(pDibInfo->bmiHeader);
		pDibInfo->bmiHeader.biPlanes = 1;
		pDibInfo->bmiHeader.biBitCount = 32;
		pDibInfo->bmiHeader.biCompression = BI_RGB;
		pDibInfo->bmiHeader.biXPelsPerMeter = 2834;
		pDibInfo->bmiHeader.biYPelsPerMeter = 2834;
		pDibInfo->bmiHeader.biClrUsed = 0;
		pDibInfo->bmiHeader.biClrImportant = 0;
		pDibInfo->bmiHeader.biClrUsed = 0;
		pDibInfo->bmiHeader.biWidth = image_w;
		pDibInfo->bmiHeader.biHeight = -image_h;
		pDibInfo->bmiHeader.biSizeImage = image_h * 4;


		if (image_n == 2)
		{
			int i = image_w * image_h;
			unsigned char *color = (unsigned char *) malloc(i*4);
			unsigned char *s = samples;
			unsigned char *d = color;
			for (; i > 0 ; i--) {
				d[2] = d[1] = d[0] = *s++;
				d[3] = *s++;
				d += 4;
			}

			SetDIBitsToDevice(pDC->m_hDC, 0, 0, nWidth, nHeight, 0, 0, 0, nHeight, color, pDibInfo, DIB_RGB_COLORS);

			free(color);
		}

		if (image_n == 4){

			::StretchDIBits(pDC->GetSafeHdc(), 0, 0,  nWidth, nHeight, 0, 0, image_w, image_h, samples, pDibInfo,DIB_RGB_COLORS,SRCCOPY);
		
			//::StretchDIBits(pDC->GetSafeHdc(), 0, 0,  nWidth, nHeight, 0, 0, image_w, image_h, samples, pDibInfo,DIB_RGB_COLORS,SRCCOPY);
		}
		free(pDibInfo);
	
		CString ss;
		ss.Format(L"Width = %d, Height = %d", nWidth, nHeight);
		pDC->TextOut(60,0, ss);
	}

	app->pageno = nOldPage;
	pdfapp_showpage(app, 1, 0, 0, 0);
}

*/

void CPdfViewer::winblit(pdfapp_t *app,HDC hdc)
{
	int image_w = fz_pixmap_width(app->ctx, app->image);
	int image_h = fz_pixmap_height(app->ctx, app->image);
	int image_n = fz_pixmap_components(app->ctx, app->image);
	unsigned char *samples = fz_pixmap_samples(app->ctx, app->image);
	int x0 = app->panx;
	int y0 = app->pany;
	int x1 = app->panx + image_w;
	int y1 = app->pany + image_h;
	RECT r;

	if (app == NULL) 
		return;

	if (app->image)
	{
		if (app->iscopying || justcopied)
		{
			pdfapp_invert(app, &app->selr);
			justcopied = 1;
		}

		pdfapp_inverthit(app);

		m_pDibInfo->bmiHeader.biWidth = image_w;
		m_pDibInfo->bmiHeader.biHeight = -image_h;
		m_pDibInfo->bmiHeader.biSizeImage = image_h * 4;

		if (image_n == 2)
		{
			int i = image_w * image_h;
			unsigned char *color = (unsigned char *) malloc(i*4);
			unsigned char *s = samples;
			unsigned char *d = color;
			for (; i > 0 ; i--)
			{
				d[2] = d[1] = d[0] = *s++;
				d[3] = *s++;
				d += 4;
			}
			SetDIBitsToDevice(hdc,
				app->panx, app->pany, image_w, image_h,
				0, 0, 0, image_h, color,
				m_pDibInfo, DIB_RGB_COLORS);
			free(color);
		}
		if (image_n == 4)
		{
			SetDIBitsToDevice(hdc,
				app->panx, app->pany, image_w, image_h,
				0, 0, 0, image_h, samples,
				m_pDibInfo, DIB_RGB_COLORS);
		}

		pdfapp_inverthit(app);

		if (app->iscopying || justcopied)
		{
			pdfapp_invert(app, &app->selr);
			justcopied = 1;
		}
	}

	/* Grey background */
	r.top = 0; r.bottom = app->winh;
	r.left = 0; r.right = x0;
	FillRect(hdc, &r, m_hBgbrush);
	r.left = x1; r.right = app->winw;
	FillRect(hdc, &r, m_hBgbrush);
	r.left = 0; r.right = app->winw;
	r.top = 0; r.bottom = y0;
	FillRect(hdc, &r, m_hBgbrush);
	r.top = y1; r.bottom = app->winh;
	FillRect(hdc, &r, m_hBgbrush);

	/* Drop shadow */
	r.left = x0 + 2;
	r.right = x1 + 2;
	r.top = y1;
	r.bottom = y1 + 2;
	FillRect(hdc, &r, m_hShbrush);
	r.left = x1;
	r.right = x1 + 2;
	r.top = y0 + 2;
	r.bottom = y1;
	FillRect(hdc, &r, m_hShbrush);

	///winblitsearch();
}

void CPdfViewer::windocopy(pdfapp_t *app)
{
	HGLOBAL handle;
	unsigned short *ucsbuf;

	if (!OpenClipboard())
		return;
	EmptyClipboard();

	handle = GlobalAlloc(GMEM_MOVEABLE, 4096 * sizeof(unsigned short));
	if (!handle)
	{
		CloseClipboard();
		return;
	}

	ucsbuf = (unsigned short *) GlobalLock(handle);
	pdfapp_oncopy(app, ucsbuf, 4096);
	GlobalUnlock(handle);

	SetClipboardData(CF_UNICODETEXT, handle);
	CloseClipboard();

	justcopied = 1;	/* keep inversion around for a while... */
}


#define OUR_TIMER_ID 1

void CPdfViewer::winadvancetimer(pdfapp_t *app, float delay)
{
	timer_pending = 1;
	SetTimer(OUR_TIMER_ID, (unsigned int)(1000*delay), NULL);
}

void CPdfViewer::handlekey(int c)
{
	if (timer_pending)
		timer_pending = 0;

	if (GetCapture() == this)
		return;

	if (justcopied)
	{
		justcopied = 0;
		InvalidateRect(NULL);
	}

	/* translate VK into ASCII equivalents */
	if (c > 256)
	{
		switch (c - 256)
		{
		case VK_F1: c = '?'; break;
		case VK_ESCAPE: c = '\033'; break;
		case VK_DOWN: c = 'j'; break;
		case VK_UP: c = 'k'; break;
		case VK_LEFT: c = 'b'; break;
		case VK_RIGHT: c = ' '; break;
		case VK_PRIOR: c = ','; break;
		case VK_NEXT: c = '.'; break;
		}
	}

	pdfapp_onkey(&m_pdfapp, c);
	InvalidateRect(NULL);
}


void CPdfViewer::handlemouse(int x, int y, int btn, int state)
{
	if (state != 0 && timer_pending)
		timer_pending = 0;

	if (state != 0 && justcopied)
	{
		justcopied = 0;
		InvalidateRect(NULL);
	}

	if (state == 1)
		SetCapture();
	if (state == -1)
		ReleaseCapture();

	pdfapp_onmouse(&m_pdfapp, x, y, btn, 0, state);
}

int CPdfViewer::GetPdfPageCount()
{
	return (m_pdfapp.pagecount);
}

BOOL CPdfViewer::IsZoominEnabled()
{
	return (m_pdfapp.resolution < MAXRES);
}

BOOL CPdfViewer::IsZoomoutEnabled()
{
	return (m_pdfapp.resolution > MINRES);
}

void CPdfViewer::Zoomin()
{
	pdfapp_t *app = &m_pdfapp;

	app->ispanning = app->iscopying = 0;
			
	app->resolution *= ZOOMSTEP;

	if (app->resolution > MAXRES)
		app->resolution = MAXRES;

	pdfapp_showpage(app, 0, 1, 1, 0);
}

void CPdfViewer::Zoomout()
{
	pdfapp_t *app = &m_pdfapp;

	app->ispanning = app->iscopying = 0;
			
	app->resolution /= ZOOMSTEP;

	if (app->resolution < MINRES)
		app->resolution = MINRES;

	pdfapp_showpage(app, 0, 1, 1, 0);
}


BOOL CPdfViewer::IsPrevPageEnabled()
{
	pdfapp_t *app = &m_pdfapp;

	return (app->pageno > 1);
}

BOOL CPdfViewer::IsNextPageEnabled()
{
	pdfapp_t *app = &m_pdfapp;

	return (app->pageno < app->pagecount);
}

void CPdfViewer::PrevPage()
{
	pdfapp_t *app = &m_pdfapp;

	app->pageno --;
	if (app->pageno < 1)
		app->pageno = 1;

	pdfapp_showpage(app, 1, 1, 1, 0);
}

void CPdfViewer::NextPage()
{
	pdfapp_t *app = &m_pdfapp;

	app->pageno ++;

	if (app->pageno > app->pagecount)
		app->pageno = app->pagecount;

	pdfapp_showpage(app, 1, 1, 1, 0);
}



BOOL CPdfViewer::Create(CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle)
{
	return CWnd::Create(PDFVIEWER_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}

BOOL CPdfViewer::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CPdfViewer::OnPaint()
{
	CPaintDC dc(this); 
	
	winblit(&m_pdfapp, dc.m_hDC);
	pdfapp_postblit(&m_pdfapp);
}

int CPdfViewer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!m_ctx) 
		return -1;

	m_hArrowcurs = LoadCursor(NULL, IDC_ARROW);
	m_hHandcurs = LoadCursor(NULL, IDC_HAND);
	m_hWaitcurs = LoadCursor(NULL, IDC_WAIT);
	m_hCaretcurs = LoadCursor(NULL, IDC_IBEAM);

	/* And a background color */
	m_hBgbrush = CreateSolidBrush(RGB(0x70,0x70,0x70));
	m_hShbrush = CreateSolidBrush(RGB(0x40,0x40,0x40));

	m_pDibInfo = (BITMAPINFO *) malloc(sizeof(BITMAPINFO) + 12);
	assert(m_pDibInfo);

	m_pDibInfo->bmiHeader.biSize = sizeof(m_pDibInfo->bmiHeader);
	m_pDibInfo->bmiHeader.biPlanes = 1;
	m_pDibInfo->bmiHeader.biBitCount = 32;
	m_pDibInfo->bmiHeader.biCompression = BI_RGB;
	m_pDibInfo->bmiHeader.biXPelsPerMeter = 2834;
	m_pDibInfo->bmiHeader.biYPelsPerMeter = 2834;
	m_pDibInfo->bmiHeader.biClrUsed = 0;
	m_pDibInfo->bmiHeader.biClrImportant = 0;
	m_pDibInfo->bmiHeader.biClrUsed = 0;

	pdfapp_init(m_ctx, &m_pdfapp);

	return 0;
}


void CPdfViewer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
		return;
	if (nType == SIZE_MAXIMIZED)
		m_pdfapp.shrinkwrap = 0;
	
	pdfapp_onresize(&m_pdfapp, cx, cy);
}


void CPdfViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 1, 1);
}


void CPdfViewer::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 2, 1);
}


void CPdfViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 3, 1);
}


void CPdfViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 1, -1);
}


void CPdfViewer::OnMButtonUp(UINT nFlags, CPoint point)
{
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 2, -1);
}


void CPdfViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 3, -1);
}


void CPdfViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	oldx = point.x; 
	oldy = point.y;
	handlemouse(point.x, point.y, 0, 0);
}


BOOL CPdfViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
		handlekey(nFlags & MK_SHIFT ? '+' : 'k');
	else
		handlekey(nFlags & MK_SHIFT ? '-' : 'j');
	return 0;
}



void CPdfViewer::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == OUR_TIMER_ID && timer_pending && m_pdfapp.presentation_mode) {
		timer_pending = 0;
		handlekey(VK_RIGHT + 256);
		handlemouse(oldx, oldy, 0, 0); /* update cursor */
		return;
	}

	//CWnd::OnTimer(nIDEvent);
}


void CPdfViewer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/* only handle special keys */
	switch (nChar) {
	case VK_F1:
	case VK_LEFT:
	case VK_UP:
	case VK_PRIOR:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_NEXT:
	case VK_ESCAPE:
		handlekey(nChar + 256);
		handlemouse(oldx, oldy, 0, 0);	/* update cursor */
		return ;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


