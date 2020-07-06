#pragma once
#include "SQLAPI.h"


class CMedicalResultObject
{
public:
	CMedicalResultObject(void);
	CMedicalResultObject(const CMedicalResultObject &obj);
	~CMedicalResultObject(void);

	static bool SortByPatientName_Asc(const CMedicalResultObject *lx, const CMedicalResultObject *rx );
	static bool SortByPatientName_Desc(const CMedicalResultObject *lx, const CMedicalResultObject *rx );

	static bool SortByStudyDetail_Asc(const CMedicalResultObject *lx, const CMedicalResultObject *rx );
	static bool SortByStudyDetail_Desc(const CMedicalResultObject *lx, const CMedicalResultObject *rx );

	static bool SortByExamMethod_Asc(const CMedicalResultObject *lx, const CMedicalResultObject *rx );
	static bool SortByExamMethod_Desc(const CMedicalResultObject *lx, const CMedicalResultObject *rx );

	void ReadFromHCS(SACommand &cmd);
	void ReadFromUIS(SACommand &cmd);
	void ReadFromPIS(SACommand &cmd);
	void ReadFromPIS_1(SACommand &cmd);
	void ReadFromRIS(SACommand &cmd);
	void ReadFromPET(SACommand &cmd);
	void ReadFromEIS(SACommand &cmd);

public:
	BOOL			m_bDataIsValid;
	BOOL			m_bNewRecord;
	BOOL			m_bFinalReport;

	BOOL			m_bFromHCS;
	BOOL			m_bFromXIS;
	BOOL			m_bReportFileUrlsSearched;

public:
	CString m_sStudyGUID;
	CString m_sHospitalID;
	CString m_sSystemCode;
	CString m_sAccessionNo;
	CString m_sDocID;
	CString m_sOrderNo;
	CString m_sMANo;
	CString m_sPatientID;
	CString m_sMRN;
	CString m_sPatientName;
	CString m_sPatientSex;
	CString	m_sPatientAge;
	CString m_sPatientDOB;
	CString m_sOrderType;
	CString m_sOutpatientNo;
	CString m_sInpatientNo;
	CString m_sCheckupNo;
	CString m_sWard;
	CString m_sBedNo;
	CString m_sRefDeptID;
	CString m_sRefDeptName;
	CString m_sRefDoctorID;
	CString m_sRefDoctorName;
	CString m_sStudyDate;
	CString m_sStudyTime;
	CString m_sExamType;
	CString m_sExamMethods;
	CString m_sStudyDetails;
	CString m_sWarnContent;
	CString m_sDeviceID;
	CString m_sDeviceName;
	CString m_sStudyUID;
	CString m_sReportDate;
	CString m_sReprotTime;
	CString m_sReviewDate;
	CString m_sReviewTime;
	CString m_sReportDoctorID;
	CString m_sReportDoctorName;
	CString m_sReviewDoctorID;
	CString m_sReviewDoctorName;
	CString m_sReportType;
	CString m_sFindings;
	CString m_sConclusions;
	CString m_sProposal;
	int		m_nClinicPathFlag;
	int		m_nCriticalFlag;
	int		m_nPositiveFlag;
	int		m_nInfectiousFlag;
	int		m_nEnhancedFlag;
	CString	m_sSubmitDate;
	CString m_sSubmitTime;
	int		m_nVersionNo;
	
	int		m_nLegacyFlag;

	CString m_sReportFileUrls;
	CString m_sPDFPassword;

	CString m_sStudyID;

};

