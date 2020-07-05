#pragma once

#include "afx.h"
#include "SQLAPI.h"

class CPublishedReportObject
{
public:
	CString	m_sStudyGUID;
	CString m_sPublishDate;
	CString m_sPublishDateTime;
	CString m_sPatientId;
	CString m_sSystemCode;
	CString m_sDocId;
	CString m_sAccessionNo;
	CString m_sPatientName;
	CString m_sPatientSex;
	CString m_sPatientAge;
	CString m_sOrderType;
	CString m_sExamType;
	CString m_sStudyDate;
	CString m_sStudyDetails;
	CString m_sWarnContent;
	CString m_sStudyUID;
	CString m_sTechician;
	CString m_sReportDate;
	CString m_sReportDateTime;
	CString m_sReportByEmpNo;
	CString m_sReportByName;
	CString m_sReviewDate;
	CString m_sReviewDateTime;
	CString m_sReviewByEmpNo;
	CString m_sReviewByName;
	CString m_sFindings;
	CString m_sConclusions;
	short	m_nCriticalFlag;
	short	m_nInfectiousFlag;
	short	m_nPositiveFlag;
	short	m_nEnhancedFlag;

	CPublishedReportObject();	
	~CPublishedReportObject();

	void Clear();

	void ReadFromQuery(SACommand &cmd);
};