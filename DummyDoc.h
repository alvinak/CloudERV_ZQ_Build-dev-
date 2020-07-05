// DummyDoc.h : interface of the CDummyDoc class
//


#pragma once


class CDummyDoc : public CDocument
{
protected: // create from serialization only
	CDummyDoc();
	DECLARE_DYNCREATE(CDummyDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CDummyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
