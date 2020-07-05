// PublishedReportDoc.h : interface of the CPublishedReportDoc class
//


#pragma once

#include <vector>
#include "MedicalResultObject.h"


class CPublishedReportDoc : public CDocument
{
protected: // create from serialization only
	CPublishedReportDoc();
	DECLARE_DYNCREATE(CPublishedReportDoc)

// Attributes
public:
	std::vector <CMedicalResultObject *> * GetMedicalResultObjectList() { return &m_lstMedicalResultObject; }
	int GetMedicalResultObjectListSize() { return m_lstMedicalResultObject.size(); }
	int GetIndexByStudyGUID(const TCHAR *szStudyGUID);
	CMedicalResultObject * GetCurrentMedicalResultObject() { return m_pCurrentMedicalResultObject; }
	void SetCurrentMedicalResultObject(CMedicalResultObject *pObj) { m_pCurrentMedicalResultObject = pObj; }
	void LocateExamReport(const TCHAR *szStudyGUID);

	// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CPublishedReportDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	std::vector <CMedicalResultObject *> m_lstMedicalResultObject;
	CMedicalResultObject *m_pCurrentMedicalResultObject;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
