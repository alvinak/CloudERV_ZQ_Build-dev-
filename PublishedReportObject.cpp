
#include "stdafx.h"
#include "PublishedReportObject.h"


CPublishedReportObject::CPublishedReportObject()
{
	Clear();
}

CPublishedReportObject::~CPublishedReportObject()
{
}

void CPublishedReportObject::Clear()
{
	m_sStudyGUID		= _T("");
	m_sPublishDate		= _T("");
	m_sPublishDateTime	= _T("");
	m_sPatientId		= _T("");
	m_sSystemCode		= _T("");
	m_sDocId			= _T("");
	m_sAccessionNo		= _T("");
	m_sPatientName		= _T("");
	m_sPatientSex		= _T("");
	m_sPatientAge		= _T("");
	m_sOrderType		= _T("");
	m_sExamType			= _T("");
	m_sStudyDate		= _T("");
	m_sStudyDetails		= _T("");
	m_sWarnContent      = _T("");
	m_sStudyUID			= _T("");
	m_sTechician		= _T("");
	m_sReportDate		= _T("");
	m_sReportDateTime	= _T("");
	m_sReportByEmpNo	= _T("");
	m_sReportByName		= _T("");
	m_sReviewDate		= _T("");
	m_sReviewDateTime	= _T("");
	m_sReviewByEmpNo	= _T("");
	m_sReviewByName		= _T("");
	m_sFindings			= _T("");
	m_sConclusions		= _T("");
	m_nCriticalFlag		= 0;
	m_nInfectiousFlag	= 0;
	m_nPositiveFlag		= 0;
	m_nEnhancedFlag		= 0;
}

void CPublishedReportObject::ReadFromQuery(SACommand &cmd)
{
	m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	m_sPublishDate		= cmd.Field("PublishedDate").asString();
	m_sPublishDateTime	= cmd.Field("PublishedDateTime").asString();
	m_sPatientId		= cmd.Field("PatientId").asString();
	m_sSystemCode		= cmd.Field("SystemCode").asString();
	m_sDocId			= cmd.Field("DocId").asString();
	m_sAccessionNo		= cmd.Field("AcccessionNo").asString();
	m_sPatientName		= cmd.Field("PatientName").asString();
	m_sPatientSex		= cmd.Field("PatientSex").asString();
	m_sPatientAge		= cmd.Field("PatientAge").asString();
	m_sOrderType		= cmd.Field("OrderType").asString();
	m_sExamType			= cmd.Field("ExamType").asString();
	m_sStudyDate		= cmd.Field("StudyDate").asString();
	m_sStudyDetails		= cmd.Field("StudyDetails").asString();
	m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sTechician		= cmd.Field("Technician").asString();
	m_sReportDate		= cmd.Field("ReportDate").asString();
	m_sReportDateTime	= cmd.Field("ReportDateTime").asString();
	m_sReportByEmpNo	= cmd.Field("ReportByEmpNo").asString();
	m_sReportByName		= cmd.Field("ReportByName").asString();
	m_sReviewDate		= cmd.Field("ReviewDate").asString();
	m_sReviewDateTime	= cmd.Field("ReviewDateTime").asString();
	m_sReviewByEmpNo	= cmd.Field("ReviewByEmpNo").asString();
	m_sReviewByName		= cmd.Field("ReviewByName").asString();
	m_sFindings			= cmd.Field("Findings").asString();
	m_sConclusions		= cmd.Field("Conclusions").asString();
	m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	m_nInfectiousFlag	= cmd.Field("InfectiousFlag").asShort();
	m_nPositiveFlag		= cmd.Field("PositiveFlag").asShort();
	m_nEnhancedFlag		= cmd.Field("EnhancedFlag").asShort();

	if (m_sStudyUID.IsEmpty())
		m_sStudyUID = m_sSystemCode + m_sAccessionNo;
}