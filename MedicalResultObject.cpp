#include "StdAfx.h"
#include "MedicalResultObject.h"
#include "Aescrypt.h"


CMedicalResultObject::CMedicalResultObject(void)
{
	m_sStudyGUID		= _T("");
	m_sHospitalID		= _T("");
	m_sSystemCode		= _T("");
	m_sAccessionNo		= _T("");
	m_sDocID			= _T("");
	m_sOrderNo			= _T("");
	m_sMANo				= _T("");
	m_sPatientID		= _T("");
	m_sMRN				= _T("");
	m_sPatientName		= _T("");
	m_sPatientSex		= _T("");
	m_sPatientAge		= _T("");
	m_sPatientDOB		= _T("");
	m_sOrderType		= _T("");
	m_sOutpatientNo		= _T("");
	m_sInpatientNo		= _T("");
	m_sCheckupNo		= _T("");
	m_sWard				= _T("");
	m_sBedNo			= _T("");
	m_sRefDeptID		= _T("");
	m_sRefDeptName		= _T("");
	m_sRefDoctorID		= _T("");
	m_sRefDoctorName	= _T("");
	m_sStudyDate		= _T("");
	m_sStudyTime		= _T("");
	m_sExamType			= _T("");
	m_sExamMethods		= _T("");
	m_sStudyDetails		= _T("");
	m_sDeviceID			= _T("");
	m_sDeviceName		= _T("");
	m_sStudyUID			= _T("");
	m_sReportDate		= _T("");
	m_sReprotTime		= _T("");
	m_sReviewDate		= _T("");
	m_sReviewTime		= _T("");
	m_sReportDoctorID	= _T("");
	m_sReportDoctorName	= _T("");
	m_sReviewDoctorID	= _T("");
	m_sReviewDoctorName	= _T("");
	m_sReportType		= _T("");
	m_sFindings			= _T("");
	m_sConclusions		= _T("");
	m_sProposal			= _T("");
	m_nClinicPathFlag	= 0;
	m_nCriticalFlag		= 0;
	m_nPositiveFlag		= 0;
	m_nInfectiousFlag	= 0;
	m_nEnhancedFlag		= 0;
	m_sSubmitDate		= _T("");
	m_sSubmitTime		= _T("");
	m_nVersionNo		= 0;

	m_nLegacyFlag		= 0;

	m_sReportFileUrls	= _T("");
	m_sPDFPassword		= _T("");
	m_sWarnContent		= _T("");


	m_bFinalReport		= FALSE;

	m_bFromHCS			= FALSE;
	m_bFromXIS			= FALSE;
	m_bReportFileUrlsSearched	= FALSE;
}


CMedicalResultObject::CMedicalResultObject(const CMedicalResultObject &obj)
{
	m_sStudyGUID		= obj.m_sStudyGUID;
	m_sHospitalID		= obj.m_sHospitalID;
	m_sSystemCode		= obj.m_sSystemCode;
	m_sAccessionNo		= obj.m_sAccessionNo;
	m_sDocID			= obj.m_sDocID;
	m_sOrderNo			= obj.m_sOrderNo;
	m_sMANo				= obj.m_sMANo;
	m_sPatientID		= obj.m_sPatientID;
	m_sMRN				= obj.m_sMRN;
	m_sPatientName		= obj.m_sPatientName;
	m_sPatientSex		= obj.m_sPatientSex;
	m_sPatientAge		= obj.m_sPatientAge;
	m_sPatientDOB		= obj.m_sPatientDOB;
	m_sOrderType		= obj.m_sOrderType;
	m_sOutpatientNo		= obj.m_sOutpatientNo;
	m_sInpatientNo		= obj.m_sInpatientNo;
	m_sCheckupNo		= obj.m_sCheckupNo;
	m_sWard				= obj.m_sWard;
	m_sBedNo			= obj.m_sBedNo;
	m_sRefDeptID		= obj.m_sRefDeptID;
	m_sRefDeptName		= obj.m_sRefDeptName;
	m_sRefDoctorID		= obj.m_sRefDoctorID;
	m_sRefDoctorName	= obj.m_sRefDoctorName;
	m_sStudyDate		= obj.m_sStudyDate;
	m_sStudyTime		= obj.m_sStudyTime;
	m_sExamType			= obj.m_sExamType;
	m_sExamMethods		= obj.m_sExamMethods;
	m_sStudyDetails		= obj.m_sStudyDetails;
	m_sDeviceID			= obj.m_sDeviceID;
	m_sDeviceName		= obj.m_sDeviceName;
	m_sStudyUID			= obj.m_sStudyUID;
	m_sReportDate		= obj.m_sReportDate;
	m_sReprotTime		= obj.m_sReprotTime;
	m_sReviewDate		= obj.m_sReviewDate ;
	m_sReviewTime		= obj.m_sReviewTime;
	m_sReportDoctorID	= obj.m_sReportDoctorID ;
	m_sReportDoctorName	= obj.m_sReportDoctorName;
	m_sReviewDoctorID	= obj.m_sReviewDoctorID ;
	m_sReviewDoctorName	= obj.m_sReviewDoctorName;
	m_sReportType		= obj.m_sReportType;
	m_sFindings			= obj.m_sFindings;
	m_sConclusions		= obj.m_sConclusions;
	m_sProposal			= obj.m_sProposal;
	m_nClinicPathFlag	= obj.m_nClinicPathFlag;
	m_nCriticalFlag		= obj.m_nCriticalFlag;
	m_nPositiveFlag		= obj.m_nPositiveFlag;
	m_nInfectiousFlag	= obj.m_nInfectiousFlag;
	m_nEnhancedFlag		= obj.m_nEnhancedFlag;
	m_sSubmitDate		= obj.m_sSubmitDate;
	m_sSubmitTime		= obj.m_sSubmitTime;
	m_nVersionNo		= obj.m_nVersionNo;
	m_sWarnContent		= obj.m_sWarnContent;

	m_nLegacyFlag		= obj.m_nLegacyFlag;

	m_sReportFileUrls	= obj.m_sReportFileUrls;
	m_sPDFPassword		= obj.m_sPDFPassword;

	m_bFinalReport		= obj.m_bFinalReport;

	m_bFromHCS			= obj.m_bFromHCS;
	m_bFromXIS			= obj.m_bFromXIS;

}

CMedicalResultObject::~CMedicalResultObject(void)
{
}


// 按患者姓名排序函数 
bool CMedicalResultObject::SortByPatientName_Asc(const CMedicalResultObject *lx, const CMedicalResultObject *rx )
{
	if (_tcscmp((*lx).m_sPatientName, (*rx).m_sPatientName) < 0) 
		return true;

	return false;
}


// 按患者姓名排序函数 
bool CMedicalResultObject::SortByPatientName_Desc(const CMedicalResultObject *lx, const CMedicalResultObject *rx )
{
	if (_tcscmp((*lx).m_sPatientName, (*rx).m_sPatientName) > 0) 
		return true;

	return false;
}



// 按检查项目排序函数 
bool CMedicalResultObject::SortByStudyDetail_Asc(const CMedicalResultObject *lx, const CMedicalResultObject *rx )
{
	// SystemCode + StudyDetails
	int iRet;

	iRet = _tcscmp(lx->m_sSystemCode, rx->m_sSystemCode);

	if (iRet < 0) 
		return true;
	
	if (iRet > 0)
		return false;

	iRet = _tcscmp(lx->m_sStudyDetails, rx->m_sStudyDetails);

	if (iRet < 0)
		return true;
	
	if (iRet > 0)
		return false;

	return false;

}


// 按检查项目排序函数 
bool CMedicalResultObject::SortByStudyDetail_Desc(const CMedicalResultObject *lx, const CMedicalResultObject *rx )
{
	// SystemCode + StudyDetails
	int iRet;

	iRet = _tcscmp(rx->m_sSystemCode, lx->m_sSystemCode);

	if (iRet < 0) 
		return true;
	
	if (iRet > 0)
		return false;

	iRet = _tcscmp(rx->m_sStudyDetails, lx->m_sStudyDetails);

	if (iRet < 0)
		return true;
	
	if (iRet > 0)
		return false;

	return false;
}


// 按检查方法排序函数 
bool CMedicalResultObject::SortByExamMethod_Asc(const CMedicalResultObject *lx, const CMedicalResultObject *rx )
{
	// SystemCode + ExamMethod
	int iRet;

	iRet = _tcscmp(lx->m_sSystemCode, rx->m_sSystemCode);

	if (iRet < 0) 
		return true;
	
	if (iRet > 0)
		return false;

	iRet = _tcscmp(lx->m_sExamMethods, rx->m_sExamMethods);

	if (iRet < 0)
		return true;
	
	if (iRet > 0)
		return false;

	return false;
}


// 按检查方法函数 
bool CMedicalResultObject::SortByExamMethod_Desc(const CMedicalResultObject *lx, const CMedicalResultObject *rx )
{
	// SystemCode + ExamMethod

	int iRet;

	iRet = _tcscmp(rx->m_sSystemCode, lx->m_sSystemCode);

	if (iRet < 0) 
		return true;
	
	if (iRet > 0)
		return false;

	iRet = _tcscmp(rx->m_sExamMethods, lx->m_sExamMethods);

	if (iRet < 0)
		return true;
	
	if (iRet > 0)
		return false;

	return false;
}




void CMedicalResultObject::ReadFromHCS(SACommand &cmd)
{
	std::string strAes(""), strBase64(""), strDecrypted("");

	if (!cmd.isOpened())
		return;

	m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= cmd.Field("SystemCode").asString();
	m_sAccessionNo		= cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("DocID").asString();
	m_sOrderNo			= cmd.Field("OrderNo").asString();
	m_sMANo				= cmd.Field("MANo").asString();
	m_sPatientID		= cmd.Field("PatientID").asString();
	m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientName		= cmd.Field("PatientName").asString();
	m_sPatientSex		= cmd.Field("PatientSex").asString();
	m_sPatientAge		= cmd.Field("PatientAge").asString();
	m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("OutpatientNo").asString();
	m_sInpatientNo		= cmd.Field("InpatientNo").asString();
	m_sCheckupNo		= cmd.Field("CheckupNo").asString();
	m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("BedNo").asString();
	m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("RefDeptName").asString();
	m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("RefDoctorName").asString();
	m_sStudyDate		= cmd.Field("StudyDate").asString();
	m_sStudyTime		= cmd.Field("StudyTime").asString();
	m_sExamMethods		= cmd.Field("ExamMethods").asString();
	m_sStudyDetails		= cmd.Field("StudyDetails").asString();
	m_sDeviceID			= cmd.Field("DeviceID").asString();
	m_sDeviceName		= cmd.Field("DeviceName").asString();
	m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= cmd.Field("ReportDate").asString();
	m_sReprotTime		= cmd.Field("ReportTime").asString();
	m_sReviewDate		= cmd.Field("ReviewDate").asString();
	m_sReviewTime		= cmd.Field("ReviewTime").asString();
	m_sReportDoctorID	= cmd.Field("ReportDoctorID").asString();
	m_sReportDoctorName	= cmd.Field("ReportDoctorName").asString();
	m_sReviewDoctorID	= cmd.Field("ReviewDoctorID").asString();
	m_sReviewDoctorName	= cmd.Field("ReviewDoctorName").asString();
	m_sReportType		= cmd.Field("ReportType").asString();
	m_sFindings			= (const wchar_t *) cmd.Field("Findings").asString();
	m_sConclusions		= cmd.Field("Conclusions").asString();
	m_sProposal			= cmd.Field("Proposal").asString();
	m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	m_nPositiveFlag		= cmd.Field("PositiveFlag").asShort();
	m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	m_nVersionNo		= cmd.Field("VersionNo").asShort();

	m_nLegacyFlag		= cmd.Field("LegacyFlag").asShort();

	m_sReportFileUrls	= cmd.Field("ReportFileUrls").asString();
	m_sPDFPassword		= cmd.Field("PDFPassword").asString();

	if (!m_sPDFPassword.IsEmpty()) {

		strBase64 = CStringA(m_sPDFPassword);
		strDecrypted = strBase64;
		try {
			strAes = ::base64_decode(strBase64);
			strDecrypted = aes_decrypt(strAes, "");
		}
		catch(...) {
			
		}

		m_sPDFPassword = strDecrypted.c_str();
	}

	m_bFinalReport		= TRUE;

	m_bFromHCS			= TRUE;
	m_bFromXIS			= FALSE;
}


void CMedicalResultObject::ReadFromUIS(SACommand &cmd)
{
	SADateTime dtExamDate;
	CString sExamDate, sIsFinished("");
	CString sAgeUnit1, sAgeUnit2, sPositiveFlag("");
	int nAge;

	if (!cmd.isOpened())
		return;

	//dtExamDate	= cmd.Field("ExamDate").asDateTime();
	//sExamDate.Format(_T("%d-%02d-%02d"), dtExamDate.GetYear(), dtExamDate.GetMonth(), dtExamDate.GetDay());

	sExamDate		= cmd.Field("ExamDate").asString();
	//dtExamDate	= cmd.Field("ExamFinish").asDateTime();
	//sExamDate.Format(_T("%d-%02d-%02d %d:%02d:%02d"), dtExamDate.GetYear(), dtExamDate.GetMonth(), dtExamDate.GetDay(),dtExamDate.GetHour(),dtExamDate.GetMinute(),dtExamDate.GetSecond());

	//m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "UIS";//cmd.Field("SystemCode").asString();
	m_sAccessionNo		= cmd.Field("ID").asString(); //cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("DocID").asString();
	m_sOrderNo			= cmd.Field("OrderNo").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	//m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientName		= cmd.Field("Name").asString();
	m_sPatientSex		= cmd.Field("Sex").asString();

	nAge				= cmd.Field("Age").asShort();
	sAgeUnit1			= cmd.Field("AgeUnit").asString();
	if (sAgeUnit1.Compare(_T("Y")) == 0)
		sAgeUnit2 = _T("岁");
	else if (sAgeUnit1.Compare(_T("M")) == 0)
		sAgeUnit2 = _T("月");
	else if (sAgeUnit1.Compare(_T("D")) == 0)
		sAgeUnit2 = _T("天");
	else if (sAgeUnit1.Compare(_T("H")) == 0)
		sAgeUnit2 = _T("小时");
	else
		sAgeUnit2 = sAgeUnit1;

	if (nAge > 0)
		m_sPatientAge.Format(_T("%d%s"), nAge, sAgeUnit2);
	else
		m_sPatientAge = _T("");

	//m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	//m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("OutpatientNo").asString();
	m_sInpatientNo		= cmd.Field("InpatientNo").asString();
	//m_sCheckupNo		= cmd.Field("CheckupNo").asString();
	//m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("SickBedNo").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("FromDept").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("FromDoctor").asString();
	//m_sStudyDate		= sExamDate;
	m_sStudyDate		= sExamDate.Mid(0,10);		
	m_sStudyTime		= cmd.Field("ExamTime").asString();
	m_sExamMethods		= cmd.Field("ExamMethod").asString();
	m_sStudyDetails		= cmd.Field("Visceras").asString();
	//m_sDeviceID			= cmd.Field("DeviceID").asString();
	//m_sDeviceName		= cmd.Field("DeviceName").asString();
	//m_sStudyUID			= cmd.Field("StudyUID").asString();
	//m_sReportDate		= sExamDate;
	//m_sReprotTime		= cmd.Field("ExamTime").asString();
	m_sReportDate		= sExamDate.Mid(0,10);
	 m_sReprotTime		= cmd.Field("ExamTime").asString();
	//m_sReviewDate		= cmd.Field("ReviewDate").asString();
	//m_sReviewTime		= cmd.Field("ReviewTime").asString();
	//m_sReportDoctorID	= cmd.Field("ReportDoctorID").asString();
	m_sReportDoctorName	= cmd.Field("DoctorName").asString();
	//m_sReviewDoctorID	= cmd.Field("ReviewDoctorID").asString();
	//m_sReviewDoctorName	= cmd.Field("ReviewDoctorName").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();
	m_sFindings			= cmd.Field("Describes").asString(); //Findings").asString();
	m_sConclusions		= cmd.Field("Diagnosis").asString(); //Conclusions").asString();
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	m_sWarnContent		= cmd.Field("WarnContent").asString();

	sPositiveFlag		= cmd.Field("ResultIsPlus").asString();
	m_nPositiveFlag		= (sPositiveFlag == "T") ? 1 : 0;

	//m_nPositiveFlag	= cmd.Field("PositiveFlag").asShort();
	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	m_nLegacyFlag		= 1;

	sIsFinished			= cmd.Field("ISFinished").asString();
	m_bFinalReport		= (sIsFinished == "T");

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= FALSE;

}


void CMedicalResultObject::ReadFromPIS(SACommand &cmd)
{
	SADateTime dtSjrq, dtBgrq;
	CString sFinding(""), sGjsj(""), sRysj(""), sSjrq(""), sBgrq("");
	int nBgzt;

	if (!cmd.isOpened())
		return;

	dtSjrq	= cmd.Field("Sjrq").asDateTime();
	sSjrq.Format(_T("%d-%02d-%02d"), dtSjrq.GetYear(), dtSjrq.GetMonth(), dtSjrq.GetDay());

	dtBgrq = cmd.Field("Bgrq").asDateTime();
	sBgrq.Format(_T("%d-%02d-%02d"), dtBgrq.GetYear(), dtBgrq.GetMonth(), dtBgrq.GetDay());


	//m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "PIS";//cmd.Field("SystemCode").asString();
	m_sAccessionNo		= cmd.Field("MedicalNo").asString(); //cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("MedicalNo").asString();
	m_sOrderNo			= cmd.Field("HISID").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	//m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientName		= cmd.Field("xm").asString();
	m_sPatientSex		= cmd.Field("xb").asString();
	m_sPatientAge		= cmd.Field("nl").asString();
	//m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	//m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("mzh").asString();
	m_sInpatientNo		= cmd.Field("zyh").asString();
	//m_sCheckupNo		= cmd.Field("CheckupNo").asString();
	//m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("ch").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("sjks").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("sjys").asString();
	m_sStudyDate		= sSjrq;
	//m_sStudyTime		= cmd.Field("ExamTime").asString();
	//m_sExamMethods		= cmd.Field("ExamMethod").asString();
	m_sStudyDetails		= cmd.Field("bbmc").asString();
	//m_sDeviceID			= cmd.Field("DeviceID").asString();
	//m_sDeviceName		= cmd.Field("DeviceName").asString();
	//m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= sBgrq;
	//m_sReprotTime		= cmd.Field("ExamTime").asString();
	m_sReviewDate		= cmd.Field("ReviewDate").asString();
	//m_sReviewTime		= cmd.Field("ReviewTime").asString();
	//m_sReportDoctorID	= cmd.Field("ReportDoctorID").asString();
	m_sReportDoctorName	= cmd.Field("zzys").asString();
	//m_sReviewDoctorID	= cmd.Field("ReviewDoctorID").asString();
	m_sReviewDoctorName	= cmd.Field("shys").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();

	sFinding	= _T("");

	sRysj				= cmd.Field("Rysj").asString();
	sGjsj				= cmd.Field("gjsj").asString();

	if (!sRysj.IsEmpty())
		sFinding = _T("[肉眼所见]:\n") + sRysj + _T("\n");

	if (!sGjsj.IsEmpty())
		sFinding = sFinding + _T("[光镜所见]:\n") + sGjsj; 

	m_sFindings = sFinding;

	m_sConclusions		= cmd.Field("zdyj").asString(); //Conclusions").asString();
	nBgzt				= cmd.Field("bgzt").asShort();
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	//m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	//m_nPositiveFlag	= cmd.Field("PositiveFlag").asShort();
	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	if( (nBgzt & 0x08) <= 0)
	{
		m_sFindings		= _T("报告未审核，内容不可见");
		m_sConclusions	= _T("报告未审核，内容不可见");
	}

	m_nLegacyFlag		= 1;

	// 如果报告医生没有签名， 则认为报告没有完成
	if (m_sReportDoctorName.IsEmpty()) 
		m_bFinalReport		= FALSE;
	else 
		m_bFinalReport		= TRUE;

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= FALSE;

}

void CMedicalResultObject::ReadFromPIS_1(SACommand &cmd)
{
	SADateTime dtSjrq, dtBgrq, dtSHSJ;
	CString sFinding(""), sGjsj(""), sRysj(""), sSjrq(""), sBgrq(""), sSHSJ("");
	int nBgzt;

	if (!cmd.isOpened())
		return;

	dtSjrq	= cmd.Field("BBJSRQ").asDateTime();
	sSjrq.Format(_T("%d-%02d-%02d"), dtSjrq.GetYear(), dtSjrq.GetMonth(), dtSjrq.GetDay());

	dtBgrq = cmd.Field("BGSJ").asDateTime();
	sBgrq.Format(_T("%d-%02d-%02d"), dtBgrq.GetYear(), dtBgrq.GetMonth(), dtBgrq.GetDay());

	dtSHSJ = cmd.Field("SHSJ").asDateTime();
	sSHSJ.Format(_T("%d-%02d-%02d"), dtSHSJ.GetYear(), dtSHSJ.GetMonth(), dtSHSJ.GetDay());


	m_sStudyGUID		= cmd.Field("CASENO").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "PIS";//cmd.Field("SystemCode").asString();
	m_sAccessionNo		= cmd.Field("BLH").asString(); //cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("BLH").asString();
	m_sOrderNo			= cmd.Field("HISID").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	//m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientName		= cmd.Field("PATIENTNAME").asString();
	m_sPatientSex		= cmd.Field("SEX").asString();
	m_sPatientAge		= cmd.Field("NL").asString();
	//m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	m_sOrderType		= cmd.Field("PATIENTTYPE").asString();
	m_sOutpatientNo		= cmd.Field("MZH").asString();
	m_sInpatientNo		= cmd.Field("ZYH").asString();
	m_sCheckupNo		= cmd.Field("TJH").asString();
	//m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("BEDNO").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("REQDEPT").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("REQDOCT").asString();
	m_sStudyDate		= sSjrq;
	//m_sStudyTime		= cmd.Field("ExamTime").asString();
	//m_sExamMethods		= cmd.Field("ExamMethod").asString();
	m_sStudyDetails		= cmd.Field("BBMC").asString();
	//m_sDeviceID			= cmd.Field("DeviceID").asString();
	//m_sDeviceName		= cmd.Field("DeviceName").asString();
	//m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= sBgrq;
	//m_sReprotTime		= cmd.Field("ExamTime").asString();
	m_sReviewDate		= sSHSJ;
	//m_sReviewTime		= cmd.Field("ReviewTime").asString();
	//m_sReportDoctorID	= cmd.Field("ReportDoctorID").asString();
	m_sReportDoctorName	= cmd.Field("BGYS").asString();
	//m_sReviewDoctorID	= cmd.Field("ReviewDoctorID").asString();
	m_sReviewDoctorName	= cmd.Field("SHYS").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();

	sFinding	= _T("");

	sRysj				= cmd.Field("DTSJ").asString();
	sGjsj				= cmd.Field("JXSJ").asString();

	if (!sRysj.IsEmpty())
		sFinding = _T("[肉眼所见]:\n") + sRysj + _T("\n");

	if (!sGjsj.IsEmpty())
		sFinding = sFinding + _T("[光镜所见]:\n") + sGjsj; 

	m_sFindings = sFinding;

	m_sConclusions		= cmd.Field("BLZD").asString(); //Conclusions").asString();
	nBgzt				= cmd.Field("SHFLAG").asShort();
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	//m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	//m_nPositiveFlag	= cmd.Field("PositiveFlag").asShort();
	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	if( nBgzt < 1)
	{
		m_sFindings		= _T("报告未审核，内容不可见");
		m_sConclusions	= _T("报告未审核，内容不可见");
	}

	m_nLegacyFlag		= 1;

	// 如果报告医生没有签名， 则认为报告没有完成
	//if (m_sReportDoctorName.IsEmpty()) 
	if (m_sReviewDoctorName.IsEmpty()) 
		m_bFinalReport		= FALSE;
	else 
		m_bFinalReport		= TRUE;

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= TRUE;

}


void CMedicalResultObject::ReadFromRIS(SACommand &cmd)
{
	SADateTime dtExamDate;
	CString sStudyDate;
	int nPatientAge;
	CString sPatientAgeUnit;
	CString	sServiceGUIDs("");
	TCHAR szBuf[40];

	if (!cmd.isOpened())
		return;

	//dtExamDate	= cmd.Field("ExamDate").asDateTime();
	//sExamDate.Format(_T("%d-%02d-%02d"), dtExamDate.GetYear(), dtExamDate.GetMonth(), dtExamDate.GetDay());

	m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "RIS";
	m_sAccessionNo		= cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("DocID").asString();
	m_sOrderNo			= cmd.Field("OrderNo").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientID		= cmd.Field("MRN").asString();
	
	m_sPatientName		= cmd.Field("PatientName").asString();
	m_sPatientName.Trim();
	m_sPatientSex		= cmd.Field("PatientSex").asString();
	nPatientAge			= cmd.Field("PatientAge").asShort();
	sPatientAgeUnit		= cmd.Field("PatientAgeUnit").asString();
	
	if (nPatientAge > 0) {
		_stprintf(szBuf, _T("%d%s"), nPatientAge, sPatientAgeUnit);
		m_sPatientAge = szBuf;
	}
	else
		m_sPatientAge	= _T("");
	
	m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("OutpatientNo").asString();
	m_sInpatientNo		= cmd.Field("InpatientNo").asString();
	m_sCheckupNo		= cmd.Field("BodyCheckupNo").asString();
	m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("BedNo").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("RefDept").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("RefDoctor").asString();
	m_sStudyDate		= cmd.Field("StudyDate").asString();
	m_sStudyTime		= cmd.Field("StudyTime").asString();
	m_sExamType			= cmd.Field("ExamType").asString();
	m_sExamMethods		= cmd.Field("ExamMethods").asString();
	m_sStudyDetails		= cmd.Field("ServiceNames").asString();
	m_sDeviceID			= cmd.Field("DeviceAET").asString();
	m_sDeviceName		= cmd.Field("DeviceName").asString();
	m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= cmd.Field("ReportDate").asString();
	m_sReprotTime		= cmd.Field("ReportTime").asString();
	m_sReviewDate		= cmd.Field("ReviewDate").asString();
	m_sReviewTime		= cmd.Field("ReviewTime").asString();
	m_sReportDoctorID	= cmd.Field("ReportDoctorEmpNo").asString();
	m_sReportDoctorName	= cmd.Field("ReportDoctorName").asString();
	m_sReviewDoctorID	= cmd.Field("ReviewDoctorEmpNo").asString();
	m_sReviewDoctorName	= cmd.Field("ReviewDoctorName").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();
	m_sFindings			= cmd.Field("Findings").asString(); 
	m_sConclusions		= cmd.Field("Conclusions").asString(); 
	m_sWarnContent		= cmd.Field("WarnContent").asString();
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	m_nPositiveFlag		= cmd.Field("PositiveFlag").asShort();
	m_nInfectiousFlag	= cmd.Field("InfectiousFlag").asShort();
	m_nEnhancedFlag		= cmd.Field("EnhancedFlag").asShort();
	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	sServiceGUIDs		= cmd.Field("ServiceGUIDs").asString();
	if (sServiceGUIDs.CompareNoCase(_T("Legacy")) == 0)
		m_nLegacyFlag	= 1;


	m_bFinalReport		= (cmd.Field("StudyStatus").asShort() >= 9);

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= FALSE;

}

void CMedicalResultObject::ReadFromPET(SACommand &cmd)
{
	SADateTime dtExamDate;
	CString sStudyDate;
	int nPatientAge;
	CString sPatientAgeUnit;
	CString	sServiceGUIDs("");
	TCHAR szBuf[40];

	if (!cmd.isOpened())
		return;

	//dtExamDate	= cmd.Field("ExamDate").asDateTime();
	//sExamDate.Format(_T("%d-%02d-%02d"), dtExamDate.GetYear(), dtExamDate.GetMonth(), dtExamDate.GetDay());

	m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "PET";
	m_sAccessionNo		= cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("DocID").asString();
	m_sOrderNo			= cmd.Field("OrderNo").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientID		= cmd.Field("MRN").asString();
	
	m_sPatientName		= cmd.Field("PatientName").asString();
	m_sPatientName.Trim();
	m_sPatientSex		= cmd.Field("PatientSex").asString();
	nPatientAge			= cmd.Field("PatientAge").asShort();
	sPatientAgeUnit		= cmd.Field("PatientAgeUnit").asString();
	
	if (nPatientAge > 0) {
		_stprintf(szBuf, _T("%d%s"), nPatientAge, sPatientAgeUnit);
		m_sPatientAge = szBuf;
	}
	else
		m_sPatientAge	= _T("");
	
	m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("OutpatientNo").asString();
	m_sInpatientNo		= cmd.Field("InpatientNo").asString();
	m_sCheckupNo		= cmd.Field("BodyCheckupNo").asString();
	m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("BedNo").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("RefDept").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("RefDoctor").asString();
	m_sStudyDate		= cmd.Field("StudyDate").asString();
	m_sStudyTime		= cmd.Field("StudyTime").asString();
	m_sExamType			= cmd.Field("ExamType").asString();
	m_sExamMethods		= cmd.Field("ExamMethods").asString();
	m_sStudyDetails		= cmd.Field("ServiceNames").asString();
	m_sDeviceID			= cmd.Field("DeviceAET").asString();
	m_sDeviceName		= cmd.Field("DeviceName").asString();
	m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= cmd.Field("ReportDate").asString();
	m_sReprotTime		= cmd.Field("ReportTime").asString();
	m_sReviewDate		= cmd.Field("ReviewDate").asString();
	m_sReviewTime		= cmd.Field("ReviewTime").asString();
	m_sReportDoctorID	= cmd.Field("ReportDoctorEmpNo").asString();
	m_sReportDoctorName	= cmd.Field("ReportDoctorName").asString();
	m_sReviewDoctorID	= cmd.Field("ReviewDoctorEmpNo").asString();
	m_sReviewDoctorName	= cmd.Field("ReviewDoctorName").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();
	m_sFindings			= cmd.Field("Findings").asString(); 
	m_sConclusions		= cmd.Field("Conclusions").asString(); 
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	m_nPositiveFlag		= cmd.Field("PositiveFlag").asShort();
	m_nInfectiousFlag	= cmd.Field("InfectiousFlag").asShort();
	m_nEnhancedFlag		= cmd.Field("EnhancedFlag").asShort();
	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	sServiceGUIDs		= cmd.Field("ServiceGUIDs").asString();
	if (sServiceGUIDs.CompareNoCase(_T("Legacy")) == 0)
		m_nLegacyFlag	= 1;


	m_bFinalReport		= (cmd.Field("StudyStatus").asShort() >= 9);

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= FALSE;

}
void CMedicalResultObject::ReadFromEIS(SACommand &cmd)
{
	// 老EIS
	
	SADateTime dtExamDate,dtReportDate;
	CString sAgeUnit1(""), sAgeUnit2(""), sExamDate, sReportDate, sIsFinished(""), sPositiveFlag("");
	int nAge;

	if (!cmd.isOpened())
		return;

	//dtExamDate	= cmd.Field("ExamDate").asDateTime();
	//sExamDate.Format(_T("%d-%02d-%02d"), dtExamDate.GetYear(), dtExamDate.GetMonth(), dtExamDate.GetDay());

	sExamDate		= cmd.Field("ExamDate").asString();
	//dtReportDate	= cmd.Field("ReportDate").asDateTime();
	//sReportDate.Format(_T("%d-%02d-%02d"), dtReportDate.GetYear(), dtReportDate.GetMonth(), dtReportDate.GetDay());
	sReportDate		= cmd.Field("ReportDate").asString();
	//m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "EIS";//cmd.Field("SystemCode").asString();
	m_sAccessionNo		= cmd.Field("ID").asString(); //cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("DocID").asString();
	m_sOrderNo			= cmd.Field("HISID").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	//m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientName		= cmd.Field("Name").asString();
	m_sPatientSex		= cmd.Field("Sex").asString();

	nAge				= cmd.Field("Age").asShort();
	sAgeUnit1			= cmd.Field("AgeUnit").asString();
	if (sAgeUnit1.Compare(_T("Y")) == 0)
		sAgeUnit2 = _T("岁");
	else if (sAgeUnit1.Compare(_T("M")) == 0)
		sAgeUnit2 = _T("月");
	else if (sAgeUnit1.Compare(_T("D")) == 0)
		sAgeUnit2 = _T("天");
	else if (sAgeUnit1.Compare(_T("H")) == 0)
		sAgeUnit2 = _T("小时");
	else
		sAgeUnit2 = sAgeUnit1;

	if (nAge > 0)
		m_sPatientAge.Format(_T("%d%s"), nAge, sAgeUnit2);
	else
		m_sPatientAge = _T("");

	//m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	//m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("OutpatientNo").asString();
	m_sInpatientNo		= cmd.Field("InpatientNo").asString();
	//m_sCheckupNo		= cmd.Field("CheckupNo").asString();
	//m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("SickBedNo").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("FromDept").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("FromDoctor").asString();
	m_sStudyDate		= sExamDate.Mid(0,10);
	m_sStudyTime		= cmd.Field("ExamTime").asString();
	m_sExamMethods		= cmd.Field("Modality").asString();
	m_sStudyDetails		= cmd.Field("Visceras").asString();
	//m_sDeviceID			= cmd.Field("DeviceID").asString();
	//m_sDeviceName		= cmd.Field("DeviceName").asString();
	//m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= sReportDate.Mid(0,10);
	m_sReprotTime		= "";//cmd.Field("ExamTime").asString();
	//m_sReviewDate		= cmd.Field("ReviewDate").asString();
	//m_sReviewTime		= cmd.Field("ReviewTime").asString();
	//m_sReportDoctorID	= cmd.Field("ReportDoctorID").asString();
	m_sReportDoctorName	= cmd.Field("DoctorName").asString();
	//m_sReviewDoctorID	= cmd.Field("ReviewDoctorID").asString();
	//m_sReviewDoctorName	= cmd.Field("ReviewDoctorName").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();
	m_sFindings			= cmd.Field("Describes").asString(); //Findings").asString();
	m_sConclusions		= cmd.Field("Diagnosis").asString(); //Conclusions").asString();
	m_sWarnContent		= cmd.Field("WarnContent").asString();
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	//m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	sPositiveFlag		= cmd.Field("ResultIsPlus").asString();

	m_nPositiveFlag		= ( sPositiveFlag == "T") ? 1 : 0;

	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	m_nLegacyFlag		= 1;

	sIsFinished			= cmd.Field("ISFinished").asString();
	m_bFinalReport		= (sIsFinished == "T");

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= FALSE;

	

	//新EIS
	/*
	SADateTime dtExamDate;
	CString sStudyDate;
	int nPatientAge;
	CString sPatientAgeUnit;
	CString	sServiceGUIDs("");
	TCHAR szBuf[40];

	if (!cmd.isOpened())
		return;

	//dtExamDate	= cmd.Field("ExamDate").asDateTime();
	//sExamDate.Format(_T("%d-%02d-%02d"), dtExamDate.GetYear(), dtExamDate.GetMonth(), dtExamDate.GetDay());

	m_sStudyGUID		= cmd.Field("StudyGUID").asString();
	//m_sHospitalID		= cmd.Field("HospitalID").asString();
	m_sSystemCode		= "EIS";
	m_sAccessionNo		= cmd.Field("AccessionNo").asString();
	m_sDocID			= cmd.Field("DocID").asString();
	m_sOrderNo			= cmd.Field("OrderNo").asString();
	//m_sMANo				= cmd.Field("MANo").asString();
	//m_sPatientID		= cmd.Field("PatientID").asString();
	m_sMRN				= cmd.Field("MRN").asString();
	m_sPatientID		= cmd.Field("MRN").asString();
	
	m_sPatientName		= cmd.Field("PatientName").asString();
	m_sPatientName.Trim();
	m_sPatientSex		= cmd.Field("PatientSex").asString();
	nPatientAge			= cmd.Field("PatientAge").asShort();
	sPatientAgeUnit		= cmd.Field("PatientAgeUnit").asString();
	
	if (nPatientAge > 0) {
		_stprintf(szBuf, _T("%d%s"), nPatientAge, sPatientAgeUnit);
		m_sPatientAge = szBuf;
	}
	else
		m_sPatientAge	= _T("");
	
	m_sPatientDOB		= cmd.Field("PatientDOB").asString();
	m_sOrderType		= cmd.Field("OrderType").asString();
	m_sOutpatientNo		= cmd.Field("OutpatientNo").asString();
	m_sInpatientNo		= cmd.Field("InpatientNo").asString();
	m_sCheckupNo		= cmd.Field("BodyCheckupNo").asString();
	m_sWard				= cmd.Field("Ward").asString();
	m_sBedNo			= cmd.Field("BedNo").asString();
	//m_sRefDeptID		= cmd.Field("RefDeptID").asString();
	m_sRefDeptName		= cmd.Field("RefDept").asString();
	//m_sRefDoctorID		= cmd.Field("RefDoctorID").asString();
	m_sRefDoctorName	= cmd.Field("RefDoctor").asString();
	m_sStudyDate		= cmd.Field("StudyDate").asString();
	m_sStudyTime		= cmd.Field("StudyTime").asString();
	m_sExamType			= cmd.Field("ExamType").asString();
	m_sExamMethods		= cmd.Field("ExamMethods").asString();
	m_sStudyDetails		= cmd.Field("ServiceNames").asString();
	m_sDeviceName		= cmd.Field("DeviceName").asString();
	m_sStudyUID			= cmd.Field("StudyUID").asString();
	m_sReportDate		= cmd.Field("ReportDate").asString();
	m_sReprotTime		= cmd.Field("ReportTime").asString();
	m_sReviewDate		= cmd.Field("ReviewDate").asString();
	m_sReviewTime		= cmd.Field("ReviewTime").asString();
	m_sReportDoctorID	= cmd.Field("ReportDoctorEmpNo").asString();
	m_sReportDoctorName	= cmd.Field("ReportDoctorName").asString();
	m_sReviewDoctorID	= cmd.Field("ReviewDoctorEmpNo").asString();
	m_sReviewDoctorName	= cmd.Field("ReviewDoctorName").asString();
	//m_sReportType		= cmd.Field("ReportType").asString();
	m_sFindings			= cmd.Field("Findings").asString(); 
	m_sConclusions		= cmd.Field("Conclusions").asString(); 
	m_sWarnContent		= cmd.Field("WarnContent").asString();
	//m_sProposal			= cmd.Field("Proposal").asString();
	//m_nClinicPathFlag	= cmd.Field("ClinicPathFlag").asShort();
	m_nCriticalFlag		= cmd.Field("CriticalFlag").asShort();
	m_nPositiveFlag		= cmd.Field("PositiveFlag").asShort();
	//m_sSubmitDate		= cmd.Field("SubmitDate").asString();
	//m_sSubmitTime		= cmd.Field("SubmitTime").asString();
	//m_nVersionNo		= cmd.Field("VersionNo").asShort();

	sServiceGUIDs		= cmd.Field("ServiceGUIDs").asString();
	if (sServiceGUIDs.CompareNoCase(_T("Legacy")) == 0)
		m_nLegacyFlag	= 1;


	m_bFinalReport		= (cmd.Field("StudyStatus").asShort() >= 9);

	m_bFromHCS			= FALSE;
	m_bFromXIS			= TRUE;
	m_bReportFileUrlsSearched	= FALSE;
	*/
}
