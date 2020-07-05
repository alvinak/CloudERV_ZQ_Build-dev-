// BitmapViewerDoc.h : interface of the CBitmapViewerDoc class
//


#pragma once

#include <vector>



class CBitmapViewerDoc : public CDocument
{
protected: // create from serialization only
	CBitmapViewerDoc();
	DECLARE_DYNCREATE(CBitmapViewerDoc)

// Attributes
public:
	int m_nImageIndex;
	std::vector <CBitmap *> m_lstBmpImages;

	CString	m_sTitle;
	CString m_sStudyGUID;

// Operations
public:

	void ClearImages();

	BOOL AppendBmpFromFile(const TCHAR *szFileName, bool bKeepCurrentIndex);
	BOOL AppendDibFromBuffer(unsigned char *lpBits, int nImageWidth, int nImageHeight, int nBitCount, bool bKeepCurrentIndex);
	BOOL AppendJpgFromBuffer(unsigned char *jpg_buffer, long jpg_size, bool bKeepCurrentIndex);

	int GetImageIndex(); 
	int GetImageCount();

	int SetImageIndex(int nIndex);
	void SetDocInfo(const TCHAR *szStudyGUID, const TCHAR *szTitle);

	CBitmap * GetImage();
	BOOL IsValidImage();
	CSize GetImageSize();

	BOOL IsMoveToPrevImageEnabled();
	BOOL IsMoveToNextImageEnabled();

	void MoveToPrevImage();
	void MoveToNextImage();

	const TCHAR *GetTitle() { return m_sTitle; }
	const TCHAR *GetStudyGUID() { return m_sStudyGUID; }

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CBitmapViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
