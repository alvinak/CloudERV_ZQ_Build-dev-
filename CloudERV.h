// CloudERV.h : main header file for the CloudERV application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <vector>

#include "SystemConfiguration.h"
#include "ERVSignClient.h"

// CCloudERVApp:
// See CloudERV.cpp for the implementation of this class
//

class CSeriesObject;
class CMedicalResultObject; 

typedef struct 
{
	HWND hWnd;
	int  nPort;
	bool bExit;
	bool bClearAll;
	int  nSDT;	// 影像浏览器模式
	int	 nIVM;	// 影像资料显示模式
	TCHAR szHost[21];
	TCHAR szKey[33];
	TCHAR szExp[201];
	TCHAR szOrderNo[31];
	TCHAR szInpatientNo[21];
	TCHAR szOutpatientNo[21];
	TCHAR szPatientID[31];
	TCHAR szDocID[21];
	TCHAR szAccessionNo[31];
	TCHAR szStudyUID[65];
	TCHAR szStudyDate[21];
	TCHAR szStudyGUID[39];	// RIS的StudyGUID, 影像浏览器模式用
	TCHAR szSystem[11];
	TCHAR szHospitalID[11];
	TCHAR szModality[21];
	TCHAR szFileName[251];
} SharedDataStruct;


class CCmdLineInfo : public CCommandLineInfo 
{
// Attributes
public:
   BOOL    m_bClose;        // /END /CLOSE
   CString m_strParameter;
   BOOL    m_bSuccess;     // all switches ok
   CString sPatientId,StudyDate,sModality;

   int	   m_nFtpPort;

   CString m_sStartupFileName;
   BOOL	   m_bUpgradingStartupFile;
   CString m_sPlatform;

// Construction
public:
   CCmdLineInfo(void);

// Overrides
public:
   virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};


class CMoveParam
{
public:
	CMoveParam() 
		{ 
			m_sStudyGUID = "";
			m_sPatientId = "";
			m_sSeriesUID = "";
			m_sStudyUID = "";
			m_sPatientName = "";
			m_sStudyDate = "";
			m_sAccessionNo = "";
			m_sRemoteHost = "";
			m_sRemoteAETitle = "";
			m_nRemotePort = 0;
			m_sDestinationAET = "";
			m_pMainForm = NULL;
		}

	CStringA m_sStudyGUID;
	CStringA m_sPatientId;
	CStringA m_sStudyUID;
	CStringA m_sPatientName;
	CStringA m_sSeriesUID;
	CStringA m_sAccessionNo;
	CStringA m_sStudyDate;
	CStringA m_sRemoteHost;
	CStringA m_sRemoteAETitle;
	int m_nRemotePort;
	CStringA m_sLocalAETitle;
	CStringA m_sDestinationAET; //TODO
	int m_nLocalPort;
	BOOL m_bAutoShow;
	DWORD m_dwThreadId;
	DWORD m_dwTickCount;
	CObList *m_pSeriesObjectList;
	void *m_pMainForm;
	HWND m_hWnd;
};


class CCloudERVApp : public CWinApp
{
public:
	CCloudERVApp();

public:
	const HINSTANCE GetAppInstance() { return m_hInstance;}

public:

	CMultiDocTemplate* m_pPdfReportViewDocTemplate;
	CMultiDocTemplate* m_pDcmImageViewDocTemplate;
	CMultiDocTemplate* m_pBmpReportViewDocTemplate;
	CMultiDocTemplate* m_pExtBmpImageViewDocTemplate;
	CMultiDocTemplate* m_pExtDcmImageViewDocTemplate;
	CMultiDocTemplate* m_pQueryPublishedReportDocTemplate;
	CMultiDocTemplate* m_pWebViewerDocTemplate;

	CMultiDocTemplate* m_pDcmSeriesCompareDocTemplate;

	CMultiDocTemplate* m_pBookingOfRISDocTemplate;
	CMultiDocTemplate* m_pBookingOfUISDocTemplate;
	CMultiDocTemplate* m_pBookingOfEISDocTemplate;

	CSystemConfiguration *GetSystemConfiguration() { return &m_SystemConfiguration; };

	int	GetValidServerInfoCount() { return m_SystemConfiguration.GetValidServerInfoCount(); }

	BOOL PositiveFlagValidForEIS() { return m_bPositiveFlagValidForEIS; }
	BOOL IsZQPETSystemValid() { return m_bZQPETSystemValid; }

private:
	HANDLE	m_hMutex;

	CERVSignClient m_oERVSignClient;

	CSystemConfiguration m_SystemConfiguration;

	int		m_nAutoLoadImagesForSingleResultOfCmdLineMode;
	
	CString m_sCompanyName;

	// 本地ImageViewer的AET & PortNo
	CString m_sImageViewerAET;
	int		m_nImageViewerPort;

	// 优化老系统影像数据的调用
	BOOL	m_bOptimizeLegacyLoading;



	// 锁定送检科室， 如果锁定送检科室，则在用送检科室查询时，只能使用设定的送检科室
	CString m_sLockedRefFromDept;

	// 从HCS查找记录
	// 1 是， 0 否
	int		m_nDefaultSearchFromHCS;
	BOOL	m_bLockSearchMode;
	int		m_nMaxImageViewerWindowsNum;

	BOOL	m_bExportImagesEnabled;		// 允许导出图像

	BOOL	m_bIgnoreStudyDateWhileSearchingByIdentity; 

	BOOL	m_bPositiveFlagValidForEIS; // 内镜系统阴性阳性有效

	BOOL	m_bZQPETSystemValid; // ZQ pet 有效

	CString	m_sPatientNameLabel;

	int		m_nDefaultCMoveParamMode;	//默认影像提取模式，取值为1-3， 1: StudyUID, 2: AccessionNo, 3: PatientID + StudyDate 
	int		m_nValidDaysOfCmdLineQuery; // 命令行查询的有效天数（对于InpatientNo, OutpatientNo, ...)

	BOOL	m_bAutoClosePriorWorkspace; // 自动关闭ImageViewer的其他窗口

	CString	m_sHISProgramFileName;		// HIS程序文件名

	CString	m_sHospitalID;					// 站点的HospitalID
	BOOL	m_bSearchOnlyWithinHospital;	//只查找本站点所在医院的记录 

	int		m_nReportResultGridColumnWidthSaveMode; // 报告结果列表列宽保存模式， 0 -- 手工， 1 -- 自动

	///////////////////////////////////
	// 报告查询界面的送检医生,送检科室列表, 保存在注册表中, 只记录最近使用的20个
	std::vector <TCHAR *> m_lstRecentRefDoctorName;
	std::vector <TCHAR *> m_lstRecentRefDeptName;
	std::vector <TCHAR *> m_lstRecentPatientName;
	std::vector <TCHAR *> m_lstRecentInpatientNo;
	std::vector <TCHAR *> m_lstRecentDocID;

	BOOL m_bRecentRefDeptAndRefDoctorListChanged;
	BOOL m_bRecentRefDeptAndRefDoctorListInited;

	void ReadRecentRefDoctorAndRefDeptList();
	void WriteRecentRefDoctorAndRefDeptList();
	///////////////////////////////////
	
	void OpenDocTemplate(CMultiDocTemplate *pDocTemplate);

	void UpgradeStartupFile(const TCHAR *szLocalFileName,const TCHAR *szPlatform,const TCHAR *szLicHost, int nPort);

	const TCHAR *GetInifileStr(const TCHAR *szSectionName, const TCHAR *szKeyName, const TCHAR *szDefault, const TCHAR *szIniFileName);
	int	  GetInifileInt(const TCHAR *szSectionName, const TCHAR *szKeyName, int nDefault, const TCHAR *szIniFileName);

	BOOL IsPdfReportOpened(const TCHAR *szReportGUID);
	void BringPdfReportViewToFront(const TCHAR *szReportGUID);

	BOOL IsBmpReportOpened(const TCHAR *szReportGUID);
	void BringBmpReportViewToFront(const TCHAR *szReportGUID);

	BOOL SearchReportFileUrls(const TCHAR *szStudyGUID, const TCHAR *szStudyUID, CString &sReportFileUrls, CString &sPDFPassword);

public:

	const TCHAR * GetPatientNameLabel();

	void MakeMenuOfRecentRefDeptOrRefDoctorList(int nTag, UINT nCommandID, CMenu *pMenu);
	void InsertNewItemToRecentDeptOrRefDoctorList(int nTag, const TCHAR *szItem); 
	
	void TextToClipboard(HWND hwnd, const char *szText);

	void LaunchQueryPublishedReport();
	void ViewMedicalReport(const TCHAR *szSystem, const TCHAR *szOrderNo, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szPatientID, const TCHAR *szDocID);
	void ViewPDFReport(const TCHAR *szReportGUID, const TCHAR *szTitle, unsigned char *pBuffer, long nBufferSize, const char *szPdfPassword, BOOL bExternalFile);
	void ViewBmpReport(const TCHAR *szStudyGUID, const TCHAR *szTitle, const TCHAR *szBmpFileName, unsigned char *lpData, long nDataSize, bool bClearFirst);

	void ViewDcmImages(const TCHAR *szStudyGUID, CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear);
	void ViewExtDcmImages(CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear);
	void ViewExtBmpImages(const TCHAR *szBmpFileName, unsigned char *lpJpgData, long nJpgDataSize, bool bClearFirst);

	void ViewOriginalReport(CMedicalResultObject *pObj);

	void DcmSeriesCompare(CSeriesObject *pSeriesObject, bool bFirstPosition, int &nSeriesCount);

	void BookingOfRIS();
	void BookingOfUIS();
	void BookingOfEIS();
	
	void LoadImagesOfStudy(bool bLegacy, int nTagOfIVM, const TCHAR *szStudyGUID, const TCHAR *szStudyUID, const TCHAR *szAccession, const TCHAR *szDocId, const TCHAR *szStudyDate);

	void ViewImageMode1(int nTag, const TCHAR *szStudyGUID, const TCHAR *szSystemCode, const TCHAR *szAccessionNo, const TCHAR *szStudyUID, const TCHAR *szPatientName, const TCHAR *szDocId, const TCHAR *szSex, const TCHAR *szStudyDate, bool bLegacy);
	void ViewImageMode2(const TCHAR *szStudyUID);
	void ViewImageMode3(int nTag, const TCHAR *szStudyUID, const TCHAR *szAccessionNo, const TCHAR *szDocId, const TCHAR *szStudyDate);

	const TCHAR * GetLockedRefFromDept() { return m_sLockedRefFromDept; }

	BOOL IsAutoLoadImagesForSingleResultOfCmdLineMode() { return (m_nAutoLoadImagesForSingleResultOfCmdLineMode != 0); }
	BOOL IsDefaultSearchFromHCS() { return (m_nDefaultSearchFromHCS != 0); }
	BOOL IsLockSearchMode() { return m_bLockSearchMode; }


	BOOL IsIgnoreStudyDateWhileSearchingByIdentity() { return m_bIgnoreStudyDateWhileSearchingByIdentity; }

	int GetDefaultCMoveMode() { return m_nDefaultCMoveParamMode; } 
	int GetValidDaysOfCmdLineQuery() { return m_nValidDaysOfCmdLineQuery; }
	BOOL IsAutoClosePriorWorkspace() { return m_bAutoClosePriorWorkspace; }

	BOOL IsExportImagesEnabled() { return m_bExportImagesEnabled; }

	BOOL GetLocalAET(CString &sLocalAET);

	BOOL GetImageViewerAET(CString &sImageViewerAET);

	int  GetReportResultGridColumnWidthSaveMode() { return m_nReportResultGridColumnWidthSaveMode; }

	const TCHAR *GetCompanyName() { return m_sCompanyName; }

	const TCHAR *GetHospitalID() { return m_sHospitalID; }
	BOOL IsSearchOnlyWithinHospital() { return m_bSearchOnlyWithinHospital; }

	BOOL IsPulishedReportViewOpened();
	BOOL IsHISProgramIsRunning();
	void ReturnToHISDesktop();

	void LocateExamReport(const TCHAR *szStudyGUID);
	// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

	

	virtual int ExitInstance();
};

extern CCloudERVApp theApp;