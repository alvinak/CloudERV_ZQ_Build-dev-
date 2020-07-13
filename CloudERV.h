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
	int  nSDT;	// Ӱ�������ģʽ
	int	 nIVM;	// Ӱ��������ʾģʽ
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
	TCHAR szStudyGUID[39];	// RIS��StudyGUID, Ӱ�������ģʽ��
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

	// ����ImageViewer��AET & PortNo
	CString m_sImageViewerAET;
	int		m_nImageViewerPort;

	// �Ż���ϵͳӰ�����ݵĵ���
	BOOL	m_bOptimizeLegacyLoading;



	// �����ͼ���ң� ��������ͼ���ң��������ͼ���Ҳ�ѯʱ��ֻ��ʹ���趨���ͼ����
	CString m_sLockedRefFromDept;

	// ��HCS���Ҽ�¼
	// 1 �ǣ� 0 ��
	int		m_nDefaultSearchFromHCS;
	BOOL	m_bLockSearchMode;
	int		m_nMaxImageViewerWindowsNum;

	BOOL	m_bExportImagesEnabled;		// ������ͼ��

	BOOL	m_bIgnoreStudyDateWhileSearchingByIdentity; 

	BOOL	m_bPositiveFlagValidForEIS; // �ھ�ϵͳ����������Ч

	BOOL	m_bZQPETSystemValid; // ZQ pet ��Ч

	CString	m_sPatientNameLabel;

	int		m_nDefaultCMoveParamMode;	//Ĭ��Ӱ����ȡģʽ��ȡֵΪ1-3�� 1: StudyUID, 2: AccessionNo, 3: PatientID + StudyDate 
	int		m_nValidDaysOfCmdLineQuery; // �����в�ѯ����Ч����������InpatientNo, OutpatientNo, ...)

	BOOL	m_bAutoClosePriorWorkspace; // �Զ��ر�ImageViewer����������

	CString	m_sHISProgramFileName;		// HIS�����ļ���

	CString	m_sHospitalID;					// վ���HospitalID
	BOOL	m_bSearchOnlyWithinHospital;	//ֻ���ұ�վ������ҽԺ�ļ�¼ 

	int		m_nReportResultGridColumnWidthSaveMode; // �������б��п���ģʽ�� 0 -- �ֹ��� 1 -- �Զ�

	///////////////////////////////////
	// �����ѯ������ͼ�ҽ��,�ͼ�����б�, ������ע�����, ֻ��¼���ʹ�õ�20��
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