// DummyDoc.cpp : implementation of the CDummyDoc class
//

#include "stdafx.h"
#include "CloudERV.h"

#include "DummyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDummyDoc

IMPLEMENT_DYNCREATE(CDummyDoc, CDocument)

BEGIN_MESSAGE_MAP(CDummyDoc, CDocument)
END_MESSAGE_MAP()


// CDummyDoc construction/destruction

CDummyDoc::CDummyDoc()
{
	// TODO: add one-time construction code here

}

CDummyDoc::~CDummyDoc()
{
}

BOOL CDummyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CDummyDoc serialization

void CDummyDoc::Serialize(CArchive& ar)
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


// CDummyDoc diagnostics

#ifdef _DEBUG
void CDummyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDummyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDummyDoc commands
