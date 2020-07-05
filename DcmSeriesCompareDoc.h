// DcmSeriesCompareDoc.h : interface of the CDcmSeriesCompareDoc class
//#pragma once

#include "DicomObject.h"

class CDcmSeriesCompareDoc : public CDocument
{
protected: // create from serialization only
	CDcmSeriesCompareDoc();
	DECLARE_DYNCREATE(CDcmSeriesCompareDoc)

// Attributes
public:
	BOOL	m_bInitialized;
	
	CStringList m_lstPatientName;
	
	CString m_sStudyGUID;

	CString	m_sTitle;
	CObList	m_SeriesObjectList;
	CObList	*GetSeriesObjectList() { return &m_SeriesObjectList;}
	CSeriesObject *m_pCurrentSeriesObject;
	CSeriesObject * GetCurrrentSeriesObject() { return m_pCurrentSeriesObject; }
	void SetCurrentSeriesObject(CSeriesObject *pObj) { m_pCurrentSeriesObject = pObj; }

	BOOL IsStudyUIDExisted(const TCHAR *szStudyUID);

	void SetStudyGUID(const TCHAR *szStudyGUID) { m_sStudyGUID = szStudyGUID; }
	const TCHAR *GetStudyGUID() { return m_sStudyGUID; }

// Operations
public:

	void AppendSeriesObject(CSeriesObject *pObj, bool bHeadPosition);

	int GetSeriesObjectCount() { return m_SeriesObjectList.GetCount(); }


	void AppendSeriesObjectList(CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear);
	BOOL IsInitialized() { return m_bInitialized; }

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

private:
	void Cleanup();
// Implementation
public:
	virtual ~CDcmSeriesCompareDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
};
