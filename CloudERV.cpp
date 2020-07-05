// CloudERV.cpp : Defines the class behaviors for the application.
//

///////////////////////////////////////////////////////////////////////////////
// CloudERV 可以工作在Imageviewer模式， 作为影像浏览器供CloudRIS调用
// 有2个控制参数SDT和IVM：
//  -SDT
//    = 0  无效， 当-SDT=0(默认), 则忽略-IVM参数
//	  = 1  老系统影像资料
//    = 2  新系统影像资料
//
//  -IVM
//    = 0  自动采用CloudERV的参数配置
//	  = 1  采用简易浏览器显示 （内嵌的影像浏览器）
//	  = 2  采用专业浏览器显示 （新版的ImageViewer)
//	  = 3  老资料用简易浏览器， 新资料用专业浏览器
//
// 时则工作在影像浏览器状态
// 支持下列参数：
//

// -accno=xxxxxxx (选项)
// -docid=xxxxxxx  (选项)
// -studyuid=xxxxxx (选项)
// -studydate=xxxxxx (选项)


// CloudRIS 典型的调用例子
// CloudERV.exe -ivm=1 -cmr=1 -accno=XR20101023001
// 表示调用老的影响资料，存取号为XR20101023001, 在简易浏览器中显示提取到的图像
//

// CloudERV.exe -ivm=1 -cmr=2 -accno=XR20101023001
// 表示调用老的影响资料，存取号为XR20101023001, 在专业浏览器中显示提取到的图像
// 

// CloudERV.exe -ivm=1 -cmr=3 -accno=XR20101023001
// 表示调用老的影响资料，存取号为XR20101023001, 在简易浏览器中显示提取到的图像
// 如果是新的影像资料(-ivm=2),则在专业浏览器中显示

// CloudERV.exe -ivm=1 -cmr=0 -accno=XR20101023001
// 表示调用老的影响资料，存取号为XR20101023001, 在哪里显示取决于CloudERV中的一个参数配置
// 

// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxinet.h"

#include "CloudERV.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "DummyDoc.h"

#include "MedicalResultObject.h"
#include "PublishedReportDoc.h"
#include "PublishedReportFrm.h"
#include "PublishedReportView.h"

#include "PdfReportDoc.h"
#include "PdfReportFrm.h"
#include "PdfReportView.h"

#include "DcmImageDoc.h"
#include "DcmImageFrm.h"
#include "DcmImageView.h"

#include "DcmSeriesCompareDoc.h"
#include "DcmSeriesCompareFrm.h"
#include "DcmSeriesCompareView.h"

#include "BitmapViewerDoc.h"
#include "BitmapViewerFrm.h"
#include "BitmapViewerView.h"

#include "BookingOfRISDoc.h"
#include "BookingOfRISFrm.h"
#include "BookingOfRISView.h"

#include "WebBrowserDoc.h"
#include "WebBrowserFrm.h"
#include "WebBrowserView.h"

#include "dcmtk/dcmnet/dcasccfg.h"      /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmnet/dcasccff.h"      /* for class DcmAssociationConfigurationFile */

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmjpeg/djencode.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmdata/dcrledrg.h"  
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilefo.h"

#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djdecabs.h"
#include "dcmtk/dcmjpeg/djdijg8.h"
#include "dcmtk/dcmdata/dcofsetl.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcpixseq.h"

#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcostrmb.h"

#include "dcmtk/dcmjpeg/djrplol.h"   /* for DJ_RPLossless */
#include "dcmtk/dcmjpeg/djrploss.h"  /* for DJ_RPLossy */

#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrlo.h"

#include "dcmtk/dcmdata/dcwcache.h"


#include "FTPClient/FTPClient.h"

#include "JPEGIJG.H"
#include "Aescrypt.h"

#include "curl\curl.h"


//
// 新老ImageViewer的接口不同
// 使用老的ImageViewer接口时， 复制老的ViewerAutomation到.\ViewerAutomation
// 增加编译预定义USEOLDIMAGEVIEWER
//

#include ".\ViewerAutomation\ViewerAutomation.nsmap"
#include ".\ViewerAutomation\soapViewerAutomationProxy.h"

#include ".\ViewerAutomation\soapStudyLocatorProxy.h"

#include <tlhelp32.h>
#include "CWebBrowser2.h"

#define IDR_DUMMY 11

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


SharedDataStruct SharedData ={0,0,0,0,0,0,_T(""),_T(""),_T(""),_T(""),_T(""),_T(""), _T(""),_T(""),_T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""),_T("")};

const TCHAR *g_lpszClassName = _T("_ClassName_CloudERV_");

///////////////////////////////////////////////////////////////////////////////
static CString g_sImageViewerFileNameWithPath("");
static CString g_sImageViewerPath("");
static CString g_sImageViewerFileName("");
static CString g_sWebViewerUrl("");
static CString g_sWebViewerInitParams("");
static CString g_sWebViewerUserId("");
static CString g_sWebViewerPassword("");

static HANDLE g_hOpenStudyThread = NULL;
static DWORD  g_dwOpenStudyThreadId = 0;
static DWORD  g_dwImageViewerProcessId = 0;

static CString g_sPrimaryDicomServerHost("");
static CString g_sPrimaryDicomServerAET("");
static int	   g_nPrimaryDicomServerPort = 0;

static CString g_sSecondaryDicomServerHost("");
static CString g_sSecondaryDicomServerAET("");
static int	   g_nSecondaryDicomServerPort = 0;

static CString g_sLegacyPrimaryDicomServerHost("");
static CString g_sLegacyPrimaryDicomServerAET("");
static int	   g_nLegacyPrimaryDicomServerPort = 0;

static CString g_sLegacySecondaryDicomServerHost("");
static CString g_sLegacySecondaryDicomServerAET("");
static int	   g_nLegacySecondaryDicomServerPort = 0;

static CString g_sLocalDicomServerAET("");
static int g_nLocalDicomServerPort = 0;
static BOOL g_bOptimizeLegacyLoading = FALSE;

static int g_nDicomMaxPDUSize = 0;

static CString g_sImageViewerAET("");
static int g_nImageViewerPort = 0;

static DcmAssociationConfiguration g_Asccfg;
static char g_lpszConfigFileName[260],g_lpszProfileName[100];

static HANDLE g_hCMoveThread = NULL;
static DWORD  g_dwCMoveThreadId = 0;


//---------------------------------------------------------------------------

int TokenizeStr(const std::string& i_source,
										const std::string& i_seperators,
										bool i_discard_empty_tokens,
										std::vector<std::string> & o_tokens)
{
	unsigned prev_pos = 0;
	unsigned pos = 0;
	unsigned number_of_tokens = 0;

	o_tokens.clear();

	pos = i_source.find_first_of(i_seperators, pos);

	while (pos != std::string::npos)
	{
		std::string token = i_source.substr(prev_pos, pos - prev_pos);
		if (!i_discard_empty_tokens || token != "")
		{
				o_tokens.push_back(i_source.substr(prev_pos, pos - prev_pos));
				number_of_tokens++;
		}

		pos++;
		prev_pos = pos;
		pos = i_source.find_first_of(i_seperators, pos);
	}

	if (prev_pos < i_source.length())
	{
		o_tokens.push_back(i_source.substr(prev_pos));
		number_of_tokens++;
	}

	return number_of_tokens;
}

//---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////

struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};

typedef struct 
{
    T_ASC_Association *assoc;
    T_ASC_PresentationContextID presId;
	std::vector <std::string *> *pStudyUids;
} CallbackStudyUidsOfCFind;

void FindStudyUidsCallback(void *callbackData,T_DIMSE_C_FindRQ *request,int responseCount,T_DIMSE_C_FindRSP *rsp,DcmDataset *responseIdentifiers)
    /*
     * This function.is used to indicate progress when findscu receives search results over the
     * network. This function will simply cause some information to be dumped to stdout.
     *
     * Parameters:
     *   callbackData        - [in] data for this callback function
     *   request             - [in] The original find request message.
     *   responseCount       - [in] Specifies how many C-FIND-RSP were received including the current one.
     *   rsp                 - [in] the C-FIND-RSP message which was received shortly before the call to
     *                              this function.
     *   responseIdentifiers - [in] Contains the record which was received. This record matches the search
     *                              mask of the C-FIND-RQ which was sent.
     */
{
    CallbackStudyUidsOfCFind *pCallbackData;
	const char *szValue = NULL;

	pCallbackData = OFstatic_cast(CallbackStudyUidsOfCFind *, callbackData);

	if (pCallbackData != NULL && pCallbackData->pStudyUids != NULL) {
		responseIdentifiers->findAndGetString(DCM_StudyInstanceUID,szValue);
		if (szValue != NULL) {
			pCallbackData->pStudyUids->push_back(new std::string(szValue));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

BOOL IsImageViewerAvailable()
{
	return !g_sImageViewerFileName.IsEmpty();
}

BOOL IsWebViewerAvailable()
{
	return !g_sWebViewerUrl.IsEmpty() && !g_sWebViewerInitParams.IsEmpty();
}

BOOL IsDicomRetrieveAvailable()
{
	return !g_sPrimaryDicomServerAET.IsEmpty();
}

///////////////////////////////////////////////////////////////////////////////

class CMoveThreadParams
{
public:
	CMoveThreadParams(const TCHAR *szStudyGUID, const TCHAR *szStudyUID, const TCHAR *szPatientName, const TCHAR *szAccessionNo, const TCHAR *szPatientId, const TCHAR *szStudyDate, bool bLegacy,  HWND hMainWnd, HANDLE *pThreadHandle, DWORD *pThreadId) {
		m_sStudyGUID	= szStudyGUID;
		m_sStudyUID		= szStudyUID;
		m_sPatientName	= szPatientName;
		m_sAccessionNo	= szAccessionNo;
		m_sPatientId	= szPatientId;
		m_sStudyDate	= szStudyDate;

		m_bLegacy		= bLegacy;

		m_hMainWnd		= hMainWnd;
		m_pThreadHandle = pThreadHandle;
		m_pThreadId		= pThreadId;
	};

	~CMoveThreadParams() { 
		
		if (m_pThreadHandle != NULL) {
			CloseHandle(*m_pThreadHandle);
			*m_pThreadHandle = NULL;
			*m_pThreadId = 0;
		}	
	};

public:
	DWORD		*m_pThreadId;
	HANDLE		*m_pThreadHandle;
	CStringA	m_sStudyGUID;
	CStringA	m_sStudyUID;
	CStringA	m_sPatientName;
	
	CStringA	m_sAccessionNo;
	CStringA	m_sPatientId;
	CStringA	m_sStudyDate;

	bool		m_bLegacy;

	HWND		m_hMainWnd;
};


typedef enum 
{
     QMPatientRoot = 0,
     QMStudyRoot = 1,
} QueryModel;

typedef struct 
{
    const char *findSyntax;
    const char *moveSyntax;
} QuerySyntax;


QuerySyntax querySyntax[2] = 
{
    { UID_FINDPatientRootQueryRetrieveInformationModel,
      UID_MOVEPatientRootQueryRetrieveInformationModel },

    { UID_FINDStudyRootQueryRetrieveInformationModel,
      UID_MOVEStudyRootQueryRetrieveInformationModel }
};


OFCondition CMove_AddPresentationContext(T_ASC_Parameters *params,
                        T_ASC_PresentationContextID pid,
                        const char* abstractSyntax)
{
    /*
    ** We prefer to use Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organise the proposed transfer syntaxes to take advantage
    ** of such behaviour.
    **
    ** The presentation contexts proposed here are only used for
    ** C-FIND and C-MOVE, so there is no need to support compressed
    ** transmission.
    */

	E_TransferSyntax  opt_out_networkTransferSyntax = EXS_Unknown;

  
    const char* transferSyntaxes[] = { NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (opt_out_networkTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
    case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    default:
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext(params, pid, abstractSyntax, transferSyntaxes, numTransferSyntaxes);

}


OFCondition CMove_AcceptSubAssoc(T_ASC_Network * aNet, T_ASC_Association ** assoc)
{
	BOOL bCfgFileOk;

	E_TransferSyntax  opt_in_networkTransferSyntax = EXS_JPEGProcess14SV1TransferSyntax;//EXS_Unknown;
	//E_TransferSyntax  opt_in_networkTransferSyntax = EXS_JPEGProcess14TransferSyntax;//EXS_Unknown;

    const char* knownAbstractSyntaxes[] = 
	{
        UID_VerificationSOPClass
    };
    
	const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
    
	int numTransferSyntaxes;
	int	nDicomMaxPDUSize = 0;
    OFCondition cond; 
	
	if (g_nDicomMaxPDUSize > 0)
		nDicomMaxPDUSize = g_nDicomMaxPDUSize;
	else
		nDicomMaxPDUSize = ASC_DEFAULTMAXPDU;

	cond= ASC_receiveAssociation(aNet, assoc, nDicomMaxPDUSize); //ASC_DEFAULTMAXPDU);
    
	
	if (cond.good())
    {
		bCfgFileOk = FALSE;

		if (g_Asccfg.isKnownProfile(g_lpszProfileName) && g_Asccfg.isValidSCPProfile(g_lpszProfileName))
		{
			cond = g_Asccfg.evaluateAssociationParameters(g_lpszProfileName, **assoc);
			bCfgFileOk = cond.good();
		}

		if (!bCfgFileOk)
		{
			switch (opt_in_networkTransferSyntax)
			{
			case EXS_LittleEndianImplicit:
				// we only support Little Endian Implicit 
				transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 1;
				break;
			case EXS_LittleEndianExplicit:
				// we prefer Little Endian Explicit 
				transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 3;
				break;
			case EXS_BigEndianExplicit:
				// we prefer Big Endian Explicit 
				transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 3;
				break;
			case EXS_JPEGProcess14TransferSyntax:
				transferSyntaxes[0] = UID_JPEGProcess14TransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
 
				break;
			case EXS_JPEGProcess14SV1TransferSyntax:
				// we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) 
				transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
				break;
			case EXS_JPEGProcess1TransferSyntax:
				// we prefer JPEGBaseline (default lossy for 8 bit images) 
				transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
				break;
			case EXS_JPEGProcess2_4TransferSyntax:
				// we prefer JPEGExtended (default lossy for 12 bit images) 
				transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
				break;
			case EXS_JPEG2000:
				// we prefer JPEG2000 Lossy 
				transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
				break;
			case EXS_JPEG2000LosslessOnly:
				// we prefer JPEG2000 Lossless 
				transferSyntaxes[0] = UID_JPEG2000LosslessOnlyTransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
				break;
			case EXS_RLELossless:
				// we prefer RLE Lossless 
				transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
				transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
				transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 4;
				break;
			default:

				// We prefer explicit transfer syntaxes.
				// If we are running on a Little Endian machine we prefer
				// LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
           
				if (gLocalByteOrder == EBO_LittleEndian)  // defined in dcxfer.h 
				{
					transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
					transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
				} 
				else 
				{
					transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
					transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
				}
					
				transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
				numTransferSyntaxes = 3;
				break;

			}


		
			// accept the Verification SOP Class if presented 
			cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
				(*assoc)->params,
				knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
				transferSyntaxes, numTransferSyntaxes);

			if (cond.good())
			{
				// the array of Storage SOP Class UIDs comes from dcuid.h 
				cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
					(*assoc)->params,
					dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,
					transferSyntaxes, numTransferSyntaxes);
			}
		}
    }
    

	if (cond.good()) cond = ASC_acknowledgeAssociation(*assoc);
    
	if (cond.bad()) 
	{
        ASC_dropAssociation(*assoc);
        ASC_destroyAssociation(assoc);
    }
    return cond;

}


OFCondition CMove_StoreSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID,DWORD dwThreadId,DWORD dwTickCount,CObList *pSeriesObjectList) //,CSeriesObject **pCurSeriesObj)
{
	POSITION pos;
	DcmDataset *pds;    
	OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
	BOOL bSeriesFound,bImageFound,bCanUnite;
	CSeriesObject *pSeriesObject,*pCurrentSeriesObject = NULL;
	CDicomObject *pDicomObject;
	const char *lpszValue = NULL;
	CString sPatientId,sPatientName,sStudyId,sSeriesNo,sSeriesUID,sInstanceUID,sModality,sStudyDate,sStudyUID;
	CString sSequenceName(""), sSeriesDescription("");

    req = &msg->msg.CStoreRQ;
    
	pds = new DcmDataset;
   
	if (pds == NULL) {
		return EC_Normal;
	}

    cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, TRUE,
        &pds, NULL, NULL, DIMSE_BLOCKING, 0);


	lpszValue = NULL;
	pds->findAndGetString(DCM_PatientID,lpszValue);
	sPatientId = lpszValue;

	lpszValue = NULL;
	pds->findAndGetString(DCM_PatientName,lpszValue);
	sPatientName = lpszValue;

	lpszValue = NULL;
	pds->findAndGetString(DCM_StudyID,lpszValue);
	sStudyId = lpszValue;

	lpszValue = NULL;
	pds->findAndGetString(DCM_StudyInstanceUID,lpszValue);
	sStudyUID = lpszValue;

	lpszValue = NULL;
	pds->findAndGetString(DCM_SeriesNumber,lpszValue);
	sSeriesNo = lpszValue;

	lpszValue = NULL;
	pds->findAndGetString(DCM_SeriesInstanceUID,lpszValue);
	sSeriesUID = lpszValue;

	lpszValue = NULL;
	pds->findAndGetString(DCM_SOPInstanceUID,lpszValue);
	sInstanceUID = lpszValue;
	
	lpszValue = NULL;
	pds->findAndGetString(DCM_Modality,lpszValue);
	sModality = lpszValue;

	if (sModality.Compare(_T("MR")) == 0  || sModality.Compare(_T("CT")) == 0)
	{
		lpszValue = NULL;
		pds->findAndGetString(DCM_SequenceName,lpszValue);
		sSequenceName = lpszValue;

		lpszValue = NULL;
		pds->findAndGetString(DCM_SeriesDescription, lpszValue);
		sSeriesDescription = lpszValue;
	}

	lpszValue = NULL;
	pds->findAndGetString(DCM_StudyDate,lpszValue);
	sStudyDate = lpszValue;


	bSeriesFound = FALSE;
	bImageFound = FALSE;

	bCanUnite = FALSE;

	pos = pSeriesObjectList->GetHeadPosition();

	while (pos != NULL && bSeriesFound == FALSE)
	{
		pSeriesObject = (CSeriesObject *) pSeriesObjectList->GetNext(pos);
			
		if (pSeriesObject->m_sSeriesUID == sSeriesUID)
		{
			pCurrentSeriesObject = pSeriesObject;
			bSeriesFound = TRUE;
		}
	}

	if (bSeriesFound)
	{
		pos = pSeriesObject->m_DicomObjectList.GetHeadPosition();

		while (pos != NULL && bImageFound == FALSE)
		{
			pDicomObject = (CDicomObject *) pSeriesObject->m_DicomObjectList.GetNext(pos);

			if (sInstanceUID == pDicomObject->GetInstanceUID())
			{
				bImageFound = TRUE;
			}
		}
	}
	else 
	{
		bCanUnite = FALSE;


		pos = pSeriesObjectList->GetHeadPosition();

		while (pos != NULL && bCanUnite == FALSE)
		{
			pSeriesObject = (CSeriesObject *) pSeriesObjectList->GetNext(pos);
			
			if (pSeriesObject->CanUnite(sStudyUID,sModality,sPatientId,sPatientName,sStudyDate))
			{
				pCurrentSeriesObject = pSeriesObject;
				bCanUnite = TRUE;
			}
		}
	}

	if (pCurrentSeriesObject == NULL)
	{
		
		pCurrentSeriesObject = new CSeriesObject;
	
		if (pCurrentSeriesObject != NULL) {
			pCurrentSeriesObject->m_sPatientId = sPatientId;
			pCurrentSeriesObject->m_sPatientName = sPatientName;
			pCurrentSeriesObject->m_sStudyId = sStudyId;
			pCurrentSeriesObject->m_sSeriesNo = sSeriesNo;
			pCurrentSeriesObject->m_sSeriesUID = sSeriesUID;
			pCurrentSeriesObject->m_sStudyUID = sStudyUID;
			pCurrentSeriesObject->m_sStudyDate = sStudyDate;
			pCurrentSeriesObject->m_sModality = sModality;
		
			if (sModality.Compare(_T("MR")) == 0 || sModality.Compare(_T("CT")) == 0) {
				pCurrentSeriesObject->m_sSequenceName = sSequenceName;
				pCurrentSeriesObject->m_sSeriesDescription = sSeriesDescription;
			}

			pCurrentSeriesObject->m_nCurrentIndex = 0;
			pCurrentSeriesObject->m_dwThreadId = dwThreadId;
			pCurrentSeriesObject->m_dwTickCount = dwTickCount;
			pCurrentSeriesObject->m_bFromRIS = FALSE;

			pSeriesObjectList->AddTail(pCurrentSeriesObject);
		}
	}

	if (!bImageFound)
	{
		if (pCurrentSeriesObject != NULL) {
			try {

				pDicomObject = new CDicomObject(pds,TRUE);
				if (pDicomObject != NULL) 
					pCurrentSeriesObject->m_DicomObjectList.AddTail(pDicomObject);
				else
					delete pds;
			}
			catch(...) {
				if (pDicomObject != NULL) {
					delete pDicomObject;
				}
				else {
					delete pds;
				}
			}
	///		*pCurSeriesObj = pCurrentSeriesObject;
		}
		else {
			delete pds;
		}

	}
	else
	{
		delete pds;
	}
	
    return cond;
}


OFCondition CMove_SubOpSCP(T_ASC_Association **subAssoc,DWORD dwThreadId,DWORD dwTickCount,CObList *pSeriesObjectList) //,CSeriesObject **pCurrentObj)
{
    T_DIMSE_Message   msg;
    T_ASC_PresentationContextID presID;

    if (!ASC_dataWaiting(*subAssoc, 0)) /* just in case */
	{
	   return DIMSE_NODATAAVAILABLE;
	}


    OFCondition cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);

    if (cond == EC_Normal) 
	{
        switch (msg.CommandField) 
		{
        case DIMSE_C_STORE_RQ:
            cond = CMove_StoreSCP(*subAssoc, &msg, presID,dwThreadId,dwTickCount,pSeriesObjectList);//,pCurrentObj);
            break;
        default:
            cond = DIMSE_BADCOMMANDTYPE;
            break;
        }
    }

    /* clean up on association termination */
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        cond = ASC_acknowledgeRelease(*subAssoc);
        ASC_dropSCPAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
        return cond;
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
    }
    else if (cond != EC_Normal)
    {
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation(*subAssoc);
    }

    if (cond != EC_Normal)
    {
        ASC_dropAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
    }
  
	return cond;
}



void CMove_SubOpCallback(void * subOpCallbackData ,T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{
	CMoveParam *pMoveParam;

	pMoveParam = (CMoveParam *) subOpCallbackData;

    if (aNet == NULL) return;   /* help no net ! */

    if (*subAssoc == NULL) 
	{
        /* negotiate association */
        CMove_AcceptSubAssoc(aNet, subAssoc);
    } 
	else 
	{
       /* be a service class provider */
        CMove_SubOpSCP(subAssoc,pMoveParam->m_dwThreadId,pMoveParam->m_dwTickCount,pMoveParam->m_pSeriesObjectList);
    }
}


void CMoveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request,
    int responseCount, T_DIMSE_C_MoveRSP *response)
{
	int nRemaining,nCompleted,wParam;
	CMoveParam *pMoveParam;

	pMoveParam = (CMoveParam *) callbackData;
		
    OFCondition cond = EC_Normal;

	nRemaining = response->NumberOfRemainingSubOperations;
	nCompleted = response->NumberOfCompletedSubOperations;

	wParam = (nRemaining << 16) + nCompleted;

	PostMessage(pMoveParam->m_hWnd,WM_SHOWMESSAGE,100,0);

	PostMessage(pMoveParam->m_hWnd,WM_SHOWPROGRESS,nRemaining,nCompleted);

	Sleep(1);
}


///////////////////////////////////////////////////////////////////////////////
// 
// CMoveThreadOfAcceptOrRequestor
// 必须提供一个StudyUID
// 1.如果存在AccessionNumber 则先按AccessionNumber找出StudyUIDs
// 2.否则如果存在影像号，则用影像号+检查日期找出StudyUIDs
// 3.用提供的StudyUID+找出的StudyUIDs向服务器提取影像资料
//
///////////////////////////////////////////////////////////////////////////////

DWORD WINAPI CMoveThreadOfAcceptOrRequestor(LPVOID lpParam)
{
	CMoveParam info;
	T_ASC_Network *net = NULL;
	T_ASC_Parameters *params = NULL;
    T_ASC_Association *assoc = NULL;
	DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
	OFCondition cond;

	bool bCFindSkipped = false;
    T_ASC_PresentationContextID presIdOfFind;
    T_DIMSE_C_FindRQ    reqFind;
    T_DIMSE_C_FindRSP   rspFind;
    DcmDataset          *rspIdsOfFind = NULL;
    const char          *sopClassOfFind;
    DcmDataset          *statusDetailOfFind = NULL;
	DcmDataset			finddset;

    T_ASC_PresentationContextID presIdOfMove;
    T_DIMSE_C_MoveRQ    reqMove;
    T_DIMSE_C_MoveRSP   rspMove;
    DcmDataset          *rspIdsOfMove = NULL;
    const char          *sopClassOfMove;
    DcmDataset          *statusDetailOfMove = NULL;
	DcmDataset			movedset;

	DcmElement *elem;
	CStringA sMsg;
	POSITION pos;
	CSeriesObject *pSeriesObject;
	CStringA sPatientId,sDate,sPatientId1,sPatientId2;
	CStringA sKey(""),sValue(""),sTmp;
	int iRand = 0;
	int nKeyLen,nPos1,nPos2,nPos;
	CStringA sRemoteHost(""),sRemoteAET(""),sLocalAET("");
	int nRemotePort = 0,nLocalPort = 1111;
	int nDicomMaxPDUSize = 0;
	CMoveThreadParams *param;	
	char sId[1024],sSeriesUID[1024],sAccessionNo[21],sStudyUID[1024],sStudyDate[21],sModality[21];
	CObList oNewSeriesObjectList,*pSeriesObjectList;
	int nSeriesMax;
	int nYear,nMonth,nDay;
	COleDateTime dtStudyDate1,dtStudyDate2;
	CallbackStudyUidsOfCFind callbackDataOfFind;
	std::vector <std::string *> lstStudyUids;
	std::vector <std::string *> ::iterator itStudyUid;
	std::string studyUids("");
	std::string studyDate("");

	/* initialize random seed: */
	srand (time(NULL));

	/* generate number between 0 and 2 */
	iRand = rand() % 3;

	param = (CMoveThreadParams *) lpParam;
	
	if (param == NULL) {
		return 0;
	}

	if (param->m_sStudyUID.IsEmpty()) {
		delete param;
		return 0;
	}

	oNewSeriesObjectList.RemoveAll();
	
	if (param->m_bLegacy) {

		if (iRand == 1) {
			sRemoteHost = g_sLegacySecondaryDicomServerHost;
			sRemoteAET  = g_sLegacySecondaryDicomServerAET;
			nRemotePort = g_nLegacySecondaryDicomServerPort;
		}
		else {
			sRemoteHost = g_sLegacyPrimaryDicomServerHost;
			sRemoteAET  = g_sLegacyPrimaryDicomServerAET;
			nRemotePort = g_nLegacyPrimaryDicomServerPort;
		}

		// 容错， 如果从服务器设置为空， 则仍然采用主服务器
		if (sRemoteHost.IsEmpty() || sRemoteAET.IsEmpty()) {
			sRemoteHost = g_sLegacyPrimaryDicomServerHost;
			sRemoteAET  = g_sLegacyPrimaryDicomServerAET;
			nRemotePort = g_nLegacyPrimaryDicomServerPort;
		}
	}
	else {

		if (iRand == 1) {
			sRemoteHost = g_sSecondaryDicomServerHost;
			sRemoteAET  = g_sSecondaryDicomServerAET;
			nRemotePort = g_nSecondaryDicomServerPort;
		}
		else {
			sRemoteHost = g_sPrimaryDicomServerHost;
			sRemoteAET  = g_sPrimaryDicomServerAET;
			nRemotePort = g_nPrimaryDicomServerPort;
		}

		if (sRemoteHost.IsEmpty() || sRemoteAET.IsEmpty()) {
			sRemoteHost = g_sPrimaryDicomServerHost;
			sRemoteAET  = g_sPrimaryDicomServerAET;
			nRemotePort = g_nPrimaryDicomServerPort;
		}
	}

	if (sRemoteHost == "" || sRemoteAET == "" || nRemotePort == 0)
	{
		delete param;
		return 0;
	}
	
	sLocalAET = g_sLocalDicomServerAET;
	nLocalPort = g_nLocalDicomServerPort;

	gethostname(localHost, sizeof(localHost) - 1);
	
	if (nLocalPort == 0) 
		nLocalPort = 104;

	sLocalAET.Trim();

	if (sLocalAET.GetLength() == 0)
	{
		sLocalAET = localHost;
	}

	info.m_hWnd = param->m_hMainWnd;
	info.m_nLocalPort = nLocalPort;
	info.m_sLocalAETitle = sLocalAET;

	info.m_sRemoteAETitle = sRemoteAET;
	info.m_sRemoteHost = sRemoteHost;
	info.m_nRemotePort = nRemotePort;
	info.m_sStudyGUID = param->m_sStudyGUID;
	info.m_sStudyUID = param->m_sStudyUID;
	info.m_sPatientName = param->m_sPatientName;

	info.m_sAccessionNo = param->m_sAccessionNo;
	info.m_sPatientId = param->m_sPatientId;
	info.m_sStudyDate = param->m_sStudyDate;
	
	info.m_bAutoShow = TRUE;
	info.m_pSeriesObjectList = &oNewSeriesObjectList;

	PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,101,0);
	
    /* network for move request and responses */
    cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, (int)info.m_nLocalPort, 1000, &net);

	if (cond.bad())
	{
		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,102,0);
		delete param;
		return 0;
	}
	
	if (g_nDicomMaxPDUSize > 0)
		nDicomMaxPDUSize = g_nDicomMaxPDUSize;
	else
		nDicomMaxPDUSize = ASC_DEFAULTMAXPDU;

    /* set up main association */
    cond = ASC_createAssociationParameters(&params, nDicomMaxPDUSize); //ASC_DEFAULTMAXPDU );
   
	if (cond.bad()) 
	{
		cond = ASC_dropNetwork(&net);

		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,103,0);

		delete param;
		return 0;
    }
	    

	ASC_setAPTitles(params,  info.m_sLocalAETitle, info.m_sRemoteAETitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
 
    sprintf(peerHost, "%s:%d", info.m_sRemoteHost, info.m_nRemotePort);

	ASC_setPresentationAddresses(params, localHost, peerHost);

    /*
     * We also add a presentation context for the corresponding
     * find sop class.
     */

    CMove_AddPresentationContext(params, 1,querySyntax[QMPatientRoot].findSyntax);
    CMove_AddPresentationContext(params, 3,querySyntax[QMPatientRoot].moveSyntax);

    CMove_AddPresentationContext(params, 1,querySyntax[QMStudyRoot].findSyntax);
    CMove_AddPresentationContext(params, 3,querySyntax[QMStudyRoot].moveSyntax);
  
    /* create association */

	cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) 
	{	

	    cond = ASC_destroyAssociation(&assoc);   
		cond = ASC_dropNetwork(&net);
		
		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,104,0);

		delete param;

		return 0 ;
	}



    /* what has been accepted/refused ? */
    
	if (ASC_countAcceptedPresentationContexts(params) == 0) 
	{

	    cond = ASC_destroyAssociation(&assoc);   
		cond = ASC_dropNetwork(&net);

		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,105,0);

		delete param;

		return 0;
    }

	bCFindSkipped = false;

	// C-Find First
	elem = newDicomElement(DcmTagKey(DCM_QueryRetrieveLevel));
	elem->putString("STUDY");
	finddset.insert(elem, OFTrue);

	elem = newDicomElement(DcmTagKey(DCM_StudyInstanceUID));
	elem->putString("");
	finddset.insert(elem, OFTrue);

	elem = newDicomElement(DcmTagKey(DCM_PatientID));
	elem->putString("");
	finddset.insert(elem, OFTrue);

	elem = newDicomElement(DcmTagKey(DCM_StudyDate));
	elem->putString("");
	finddset.insert(elem, OFTrue);

	elem = newDicomElement(DcmTagKey(DCM_AccessionNumber));
	elem->putString("");
	finddset.insert(elem, OFTrue);

	if (!info.m_sAccessionNo.IsEmpty()) {
		elem = newDicomElement(DcmTagKey(DCM_AccessionNumber));
		elem->putString(info.m_sAccessionNo);
		finddset.insert(elem, OFTrue);
		bCFindSkipped = false;
	}
	else if (!info.m_sPatientId.IsEmpty()) {		
		elem = newDicomElement(DcmTagKey(DCM_PatientID));
		elem->putString(info.m_sPatientId);
		finddset.insert(elem, OFTrue);

		elem = newDicomElement(DcmTagKey(DCM_StudyDate));

		studyDate = info.m_sStudyDate;
		
		studyDate.erase(std::remove(studyDate.begin(), studyDate.end(), '-'), studyDate.end());
		
		elem->putString(studyDate.c_str());
		finddset.insert(elem, OFTrue);

		bCFindSkipped = false;
	}
	else {
		bCFindSkipped = true;
	}

	if (!bCFindSkipped) {
		/* prepare the transmission of data */
		bzero((char*)&reqFind, sizeof(reqFind));
		sopClassOfFind = querySyntax[QMPatientRoot].findSyntax;
		/* which presentation context should be used */
		presIdOfFind = ASC_findAcceptedPresentationContextID(assoc, sopClassOfFind);
		if (presIdOfFind == 0) 
		{
			cond = ASC_destroyAssociation(&assoc);   
			cond = ASC_dropNetwork(&net);

			PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,106,0);
		
			delete param;

			return 0;
		}

		reqFind.MessageID = assoc->nextMsgID;
		strcpy(reqFind.AffectedSOPClassUID, sopClassOfFind);
		reqFind.Priority = DIMSE_PRIORITY_MEDIUM;
		reqFind.DataSetType = DIMSE_DATASET_PRESENT;

		/* prepare the callback data */
		callbackDataOfFind.assoc = assoc;
		callbackDataOfFind.presId = presIdOfFind;
		callbackDataOfFind.pStudyUids = &lstStudyUids;

	    /* finally conduct transmission of data */
		cond = DIMSE_findUser(assoc, presIdOfFind, &reqFind, &finddset,
                          FindStudyUidsCallback, &callbackDataOfFind,
                          DIMSE_BLOCKING, 0,
                          &rspFind, &statusDetailOfFind);


		/* dump status detail information if there is some */
		if (statusDetailOfFind != NULL)  {
			delete statusDetailOfFind;
			statusDetailOfFind = NULL;
		}

		if (rspIdsOfFind != NULL) { 
			delete rspIdsOfFind;
			rspIdsOfFind = NULL;
		}
	}

	/////////////////////////////////////////////////////////
	// C-Move

	elem = newDicomElement(DcmTagKey(DCM_QueryRetrieveLevel));
	elem->putString("STUDY");
	movedset.insert(elem, OFTrue);

	elem = newDicomElement(DcmTagKey(DCM_StudyInstanceUID));
	studyUids = info.m_sStudyUID;
	for (itStudyUid = lstStudyUids.begin(); itStudyUid != lstStudyUids.end(); itStudyUid ++) {
		if (strcmp(info.m_sStudyUID, (*itStudyUid)->c_str()) != 0) { 
			if (!studyUids.empty())
				studyUids += "\\";

			studyUids += *(*itStudyUid);
		}
	}

	elem->putString(studyUids.c_str());
	movedset.insert(elem, OFTrue);

	for (itStudyUid = lstStudyUids.begin(); itStudyUid != lstStudyUids.end(); itStudyUid ++) {
		delete (*itStudyUid);
	}

	lstStudyUids.clear();
	
	// C-Move

	sopClassOfMove = querySyntax[QMPatientRoot].moveSyntax;
	
    /* which presentation context should be used */
    presIdOfMove = ASC_findAcceptedPresentationContextID(assoc, sopClassOfMove);
    
	if (presIdOfMove == 0) 
	{
	    cond = ASC_destroyAssociation(&assoc);   
		cond = ASC_dropNetwork(&net);

		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,106,0);
		
		delete param;

		return 0;
	}

	reqMove.MessageID = assoc->nextMsgID;
    strcpy(reqMove.AffectedSOPClassUID, sopClassOfMove);
    reqMove.Priority = DIMSE_PRIORITY_MEDIUM;
    reqMove.DataSetType = DIMSE_DATASET_PRESENT;

    /* set the destination to be me */
    ASC_getAPTitles(assoc->params, reqMove.MoveDestination, NULL, NULL);
    
	cond = DIMSE_moveUser(assoc, presIdOfMove, &reqMove, &movedset,
        CMoveCallback, &info, DIMSE_BLOCKING, 0,
        net, CMove_SubOpCallback, &info,
        &rspMove, &statusDetailOfMove, &rspIdsOfMove);


	if (rspMove.NumberOfCompletedSubOperations == 0) {
		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,107,0);
	}


	if (statusDetailOfMove != NULL)  {
        delete statusDetailOfMove;
		statusDetailOfMove = NULL;
    }

    if (rspIdsOfMove != NULL) { 
		delete rspIdsOfMove;
		rspIdsOfMove = NULL;
	}

	pos = oNewSeriesObjectList.GetHeadPosition();

	while (pos != NULL)
	{
		pSeriesObject = (CSeriesObject *) oNewSeriesObjectList.GetNext(pos);
		pSeriesObject->m_sPatientName_CN = info.m_sPatientName;
		pSeriesObject->Sort();
	}

	PostMessage(param->m_hMainWnd,WM_SHOWPROGRESS,0,0);

    cond = ASC_abortAssociation(assoc);
    cond = ASC_destroyAssociation(&assoc);   
	cond = ASC_dropNetwork(&net);

	if (info.m_pSeriesObjectList->GetCount() > 0) {
		SendMessage(param->m_hMainWnd,WM_IMAGEISCOMING,(WPARAM) &(info),0);
		PostMessage(param->m_hMainWnd,WM_SHOWMESSAGE,100,0);
	}
	
	info.m_pSeriesObjectList->RemoveAll();

	delete param;

	return 1;
}


///////////////////////////////////////////////////////////////////////////////

class COpenStudyParams {
public:
	COpenStudyParams(CStringList *pStudyUIDList, const TCHAR *szAccessionNo, const TCHAR *szDocId, const TCHAR *szStudyDate, HANDLE hMainForm, HANDLE *pThreadHandle, DWORD *pThreadId, bool bCloseOtherViewers) {
		m_pStudyUIDList		= pStudyUIDList;
		m_sAccessionNo		= szAccessionNo;
		m_sDocId			= szDocId;
		m_sStudyDate		= szStudyDate;
		m_hMainForm			= hMainForm;
		m_pThreadHandle		= pThreadHandle;
		m_pThreadId			= pThreadId;
		m_bCloseOtherViewers= bCloseOtherViewers;
	}

	~COpenStudyParams() {

		if (m_pStudyUIDList != NULL) {
			m_pStudyUIDList->RemoveAll();
			delete m_pStudyUIDList;
		}

		if (m_pThreadHandle != NULL) {
			CloseHandle(*m_pThreadHandle);
			*m_pThreadHandle = NULL;
			*m_pThreadId = 0;
		}
	}
public:
	DWORD		*m_pThreadId;
	HANDLE		*m_pThreadHandle;
	CStringList *m_pStudyUIDList;
	CStringA	m_sAccessionNo;
	CStringA	m_sDocId;
	CStringA	m_sStudyDate;
	HANDLE		m_hMainForm;
	bool 		m_bCloseOtherViewers;
};

//---------------------------------------------------------------------------
DWORD FindProcessId(const wchar_t * processName)
{
	BOOL bRetValue;
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
    DWORD dwProcessId = 0;

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if ( processesSnapshot == INVALID_HANDLE_VALUE ) {
		return 0;
	}

    dwProcessId = 0;
	bRetValue = Process32First(processesSnapshot, &processInfo);
    while (bRetValue && (dwProcessId == 0)) {
        if (wcsicmp(processName, processInfo.szExeFile) == 0) {
            dwProcessId = processInfo.th32ProcessID;
        }

		bRetValue = Process32Next(processesSnapshot, &processInfo);
    }

    CloseHandle(processesSnapshot);

	return dwProcessId;
}
//---------------------------------------------------------------------------

DWORD LaunchProcess(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait, std::wstring sCurrentDirectory)
{
	size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
	DWORD dwExitCode = 0;
	std::wstring sTempStr = L"";
	DWORD dwProcessId = 0;

	/* - NOTE - You should check here to see if the exe even exists */

	/* Add a space to the beginning of the Parameters */
	if (Parameters.size() != 0)
	{
		if (Parameters[0] != L' ')
		{
			Parameters.insert(0,L" ");
		}
	}

	/* The first parameter needs to be the exe itself */
	sTempStr = FullPathToExe;
	iPos = sTempStr.find_last_of(L"\\");
	sTempStr.erase(0, iPos +1);
	Parameters = sTempStr.append(Parameters);

	 /* CreateProcessW can modify Parameters thus we allocate needed memory */
	wchar_t * pwszParam = new wchar_t[Parameters.size() + 1];
	if (pwszParam == 0)
	{
		return 0;
	}
	const wchar_t* pchrTemp = Parameters.c_str();
	wcscpy_s(pwszParam, Parameters.size() + 1, pchrTemp);

	/* CreateProcess API initialization */
	STARTUPINFOW siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);

	if (CreateProcess(const_cast<LPCWSTR>(FullPathToExe.c_str()),
							pwszParam, 
							0, 
							0, 
							false,
							CREATE_DEFAULT_ERROR_MODE | DETACHED_PROCESS,
							0, 
							sCurrentDirectory.c_str(),
							&siStartupInfo, &piProcessInfo) != false)
	{
		 /* Watch the process. */
		WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000));

		dwProcessId = piProcessInfo.dwProcessId;
	}

	/* Free memory */
	delete[]pwszParam;
	pwszParam = 0;

	/* Release handles */
	CloseHandle(piProcessInfo.hProcess);
	CloseHandle(piProcessInfo.hThread);

	return dwProcessId;
}

//---------------------------------------------------------------------------

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}

HWND find_main_window(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.best_handle;
}

//---------------------------------------------------------------------------

void LaunchImageViewer()
{
	CString sCmdLine;
	std::wstring sExecutePath, sExecuteFile;
	DWORD dwProcessId;
	HWND hMainWnd = NULL;

	sExecutePath = g_sImageViewerPath;
	sExecuteFile = g_sImageViewerFileName;

	sCmdLine.Format(L"%s\\%s", sExecutePath.c_str(), sExecuteFile.c_str());
	dwProcessId = FindProcessId(sExecuteFile.c_str());

	if (dwProcessId == 0) {
		g_dwImageViewerProcessId = LaunchProcess((const wchar_t *) sCmdLine, L"", 10, sExecutePath);
		Sleep(100);
	}
	else {
		if (g_dwImageViewerProcessId != dwProcessId)
			g_dwImageViewerProcessId = 0;

		hMainWnd = find_main_window(dwProcessId);
		if (hMainWnd) {
			if (::IsIconic(hMainWnd))
				::ShowWindow(hMainWnd, SW_RESTORE);
			::BringWindowToTop(hMainWnd);
		}
	}
}


//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// 新的ImageViewer 接口 .Net 4.0 Frameworks
/////////////////////////////////////////////////////////////////////////////
DWORD WINAPI OpenStudyThread(void *lpParams)
{
	COpenStudyParams *pOpenStudyParams = NULL;
	struct soap soap; // the gSOAP runtime context
	ViewerAutomationProxy openStudyProxy;
	StudyLocatorProxy findProxy;
	int nReturnCode = 0;
	

	std::vector<_ns2__Viewer *> lstViewer;
	std::vector<_ns2__Viewer *> ::iterator itViewer;
	_ns2__Viewer	*pViewer;
	_ns2__ArrayOfViewer	*pListViewer;
	_ns2__GetActiveViewers  getActiveViewersInParam;
	_ns2__GetActiveViewersResponse getActiveViewsResponse;

	_ns2__CloseViewer closeViewerInParam;
	_ns2__CloseViewerRequest closeViewerRequest;
	_ns2__CloseViewerResponse closeViewerResponse;

	_ns2__OpenStudyInfo *pInfo;
	_ns2__ArrayOfOpenStudyInfo lstInfo;
	_ns2__OpenStudies inParam;
	_ns2__OpenStudiesRequest  request;
	_ns2__OpenStudiesResponse response;

	//
	std::string sAccessionNo(""), sDocId(""), sStudyDate(""), sNullValue("");
	_ns5__StudyRootStudyIdentifier *pStudyIdentifier;
	_ns5__StudyQuery queryParam;
	_ns5__StudyQueryResponse queryResponse;
	std::vector <_ns5__StudyRootStudyIdentifier *> ::iterator itStudy;
	//

	std::string strStudyUID("");
	std::string strAETitle("");
	bool bActiveIfExisting = true;
	bool bFound = false;
	bool bFindSkipped = true;
	std::string studyuid(""), aet("");
	CStringA sEndPointOfOpen("http://localhost:51124/CloudPACS/ImageViewer/Automation");
	CStringA sEndPointOfFind("http://localhost:51124/CloudPACS/ImageViewer/StudyLocator"); 
	CStringA sItem("");
	POSITION pos;
	int ni;

	pOpenStudyParams = (COpenStudyParams * ) lpParams;
	if (pOpenStudyParams == NULL) {
		return 0;
	}

	if (pOpenStudyParams->m_pStudyUIDList == NULL) {
		delete pOpenStudyParams;
		return 0;
	}

	if (pOpenStudyParams->m_pStudyUIDList->GetCount() == 0) {
		delete pOpenStudyParams;
		return 0;
	}
	
	soap_init(&soap); // initialize the context (only once!)

	if (pOpenStudyParams->m_bCloseOtherViewers) {

		getActiveViewersInParam.soap = &soap;

		// 从ImageViewer获取已经打开的检查列表

		if (openStudyProxy.GetActiveViewers(sEndPointOfOpen.IsEmpty() ? NULL : sEndPointOfOpen, NULL,
			&getActiveViewersInParam, &getActiveViewsResponse) == SOAP_OK) {

			if (getActiveViewsResponse.GetActiveViewersResult != NULL) {

				pListViewer = getActiveViewsResponse.GetActiveViewersResult->ActiveViewers;
				lstViewer = pListViewer->Viewer;

				for (itViewer = lstViewer.begin(); itViewer != lstViewer.end(); itViewer ++) {
					for (ni = 0, bFound = false; ni < pOpenStudyParams->m_pStudyUIDList->GetCount() && !bFound; ni ++) {
						pos = pOpenStudyParams->m_pStudyUIDList->FindIndex(ni);
						if (pos != NULL) {
							sItem = pOpenStudyParams->m_pStudyUIDList->GetAt(pos);

							if ((*itViewer)->PrimaryStudyIdentifier &&((*itViewer)->PrimaryStudyIdentifier->StudyInstanceUid)) {
								if (strcmp(sItem, (*itViewer)->PrimaryStudyIdentifier->StudyInstanceUid->c_str()) == 0)
									bFound = true;
							}
						}
					}

					// 没有找到， 即StudyUID不在将要打开的StudyUIDs中,则关闭图像
					if (!bFound) {

						closeViewerRequest.soap = &soap;
						closeViewerRequest.Viewer = (*itViewer);
						closeViewerInParam.soap = &soap;
						closeViewerInParam.request = &closeViewerRequest;
						openStudyProxy.CloseViewer(sEndPointOfOpen.IsEmpty() ? NULL : sEndPointOfOpen, NULL, &closeViewerInParam, &closeViewerResponse);
					}
				}
			}
		}
	}


	// 如果AccessionNo 不为空,则按AccessionNo查找StudyUIDs
	// 否则如果DocId不为空,则按PatientId查找StudyUIDs
	sAccessionNo	= pOpenStudyParams->m_sAccessionNo;
	sDocId			= pOpenStudyParams->m_sDocId;
	sStudyDate		= pOpenStudyParams->m_sStudyDate;

	sStudyDate.erase(std::remove(sStudyDate.begin(), sStudyDate.end(), '-'), sStudyDate.end());

	pStudyIdentifier = soap_new_ns5__StudyRootStudyIdentifier(&soap);
	pStudyIdentifier->soap = &soap;
	pStudyIdentifier->StudyInstanceUid = new std::string("");


	bFindSkipped = true;
	if (!sAccessionNo.empty()) {
		pStudyIdentifier->AccessionNumber = new std::string(sAccessionNo.c_str());
		pStudyIdentifier->PatientId = new std::string("");
		pStudyIdentifier->StudyDate = new std::string("");
		
		bFindSkipped = false;
	}
	else if (!sDocId.empty()) {
		pStudyIdentifier->AccessionNumber = new std::string("");
		pStudyIdentifier->PatientId = new std::string(sDocId.c_str());
		pStudyIdentifier->StudyDate = new std::string(sStudyDate.c_str());
		bFindSkipped = false;
	}

	if (!bFindSkipped) {
	
		queryParam.soap = &soap;
		queryParam.queryCriteria = pStudyIdentifier;
	
		queryResponse.soap = &soap;

		nReturnCode = findProxy.StudyQuery(sEndPointOfFind.IsEmpty() ? NULL :sEndPointOfFind, NULL, &queryParam, &queryResponse);
		if (nReturnCode == SOAP_OK) {			
			if (queryResponse.StudyQueryResult) {
				for (itStudy = queryResponse.StudyQueryResult->StudyRootStudyIdentifier.begin(); itStudy != queryResponse.StudyQueryResult->StudyRootStudyIdentifier.end(); itStudy ++) {
					studyuid = *(*itStudy)->StudyInstanceUid;

					if (!studyuid.empty()) { 
						if (pOpenStudyParams->m_pStudyUIDList->Find(CString(studyuid.c_str()))  == NULL) {
							pOpenStudyParams->m_pStudyUIDList->AddTail(CString(studyuid.c_str()));
						}
					}
				}
			}
		}
		else {
		MessageBoxA(NULL, findProxy.soap_fault_detail(), "Err", MB_OK);
		}
	}

	soap_delete_ns5__StudyRootStudyIdentifier(&soap, pStudyIdentifier);

	std::string ssss("");

	////////////////////////////////////////////////////////////////////////////
	for (ni = 0; ni < pOpenStudyParams->m_pStudyUIDList->GetCount(); ni ++) {
		pos = pOpenStudyParams->m_pStudyUIDList->FindIndex(ni);
		
		if (pos != NULL) {
			sItem = pOpenStudyParams->m_pStudyUIDList->GetAt(pos);
			pInfo = new _ns2__OpenStudyInfo;
			pInfo->soap = &soap;
			pInfo->SourceAETitle = new std::string("");
			pInfo->StudyInstanceUid = new std::string(sItem);
	
			lstInfo.OpenStudyInfo.push_back(pInfo);
		}
	}

	bool bb = false;
	request.soap_default(&soap);
	request.StudiesToOpen = &lstInfo;
	request.LoadPriors = &bb;
	request.ActivateIfAlreadyOpen = &bActiveIfExisting;

	//request.
	inParam.soap_default(&soap);
	inParam.request = &request;

	if (openStudyProxy.OpenStudies(sEndPointOfOpen.IsEmpty() ? NULL : sEndPointOfOpen, NULL, &inParam, &response) != SOAP_OK) {
		SendMessage((HWND) pOpenStudyParams->m_hMainForm, 0x1010, 0, 0);
	}

	soap_destroy(&soap); // delete deserialized class instances (for C++)
	soap_end(&soap); // remove deserialized data and clean up
	soap_done(&soap); // detach the gSOAP context

	delete pOpenStudyParams;

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
void OpenStudyViaImageViewer(int nTag, const TCHAR *szStudyUID, const TCHAR *szAccessionNo, const TCHAR *szDocId, const TCHAR *szStudyDate, bool bCloseOtherViewers)
{
	COpenStudyParams *pParams;
	HANDLE hOpenStudyThread = NULL;
	DWORD  dwOpenStudyThreadId = 0;
	CStringList *pList;
	CWaitCursor wait;

	int nLeft, nTop;

	if (g_hOpenStudyThread != NULL) {
		SendMessage(AfxGetMainWnd()->m_hWnd, 0x1011, 0, 0);
		return;
	}
	else
		SendMessage(AfxGetMainWnd()->m_hWnd, 0x1011, 1, 0);

	SendMessage(AfxGetMainWnd()->m_hWnd, 0x1010, 1, 0);

	LaunchImageViewer();

	if (szStudyUID != NULL && _tcslen(szStudyUID) > 0) {

		pList = new CStringList;
		pList->AddTail(szStudyUID);

		switch(nTag) {
		case 1:
			pParams = new COpenStudyParams(pList, _T(""), _T(""), _T(""), AfxGetMainWnd()->m_hWnd, &g_hOpenStudyThread, &g_dwOpenStudyThreadId, bCloseOtherViewers);
			break;
		case 2:
			pParams = new COpenStudyParams(pList, szAccessionNo, _T(""), _T(""), AfxGetMainWnd()->m_hWnd, &g_hOpenStudyThread, &g_dwOpenStudyThreadId, bCloseOtherViewers);
			break;
		case 3:
			pParams = new COpenStudyParams(pList, _T(""), szDocId, szStudyDate, AfxGetMainWnd()->m_hWnd, &g_hOpenStudyThread, &g_dwOpenStudyThreadId, bCloseOtherViewers);
			break;
		case 4:
			pParams = new COpenStudyParams(pList, _T(""), szDocId, _T(""), AfxGetMainWnd()->m_hWnd, &g_hOpenStudyThread, &g_dwOpenStudyThreadId, bCloseOtherViewers);
			break;
		default:
			pParams = new COpenStudyParams(pList, _T(""), _T(""), _T(""), AfxGetMainWnd()->m_hWnd, &g_hOpenStudyThread, &g_dwOpenStudyThreadId, bCloseOtherViewers);
		}

		g_hOpenStudyThread = CreateThread(NULL, 0, OpenStudyThread, pParams, 0, &g_dwOpenStudyThreadId);
	}
}

///////////////////////////////////////////////////////////////////////////////
void OpenStudyViaC_Move(const TCHAR *szStudyGUID, const TCHAR *szStudyUID, const TCHAR *szPatientName, const TCHAR *szAccessionNo, const TCHAR *szPatientId, const TCHAR *szStudyDate, bool bLegacy)
{
	CWaitCursor wait;
	CMoveThreadParams *pParams;

	if (g_hCMoveThread != NULL) {		
		SendMessage(AfxGetMainWnd()->m_hWnd, 0x1011, 0, 0);
		return;
	}
	else
		SendMessage(AfxGetMainWnd()->m_hWnd, 0x1011, 1, 0);

	SendMessage(AfxGetMainWnd()->m_hWnd, 0x1010, 1, 0);

	if (szStudyUID != NULL && _tcslen(szStudyUID) > 0) {

		pParams = new CMoveThreadParams(szStudyGUID, szStudyUID, szPatientName, szAccessionNo, szPatientId, szStudyDate, bLegacy,  AfxGetMainWnd()->m_hWnd, &g_hCMoveThread, &g_dwCMoveThreadId);
		g_hCMoveThread = CreateThread(NULL, 0, CMoveThreadOfAcceptOrRequestor, pParams, 0, &g_dwCMoveThreadId);
	}
}

///////////////////////////////////////////////////////////////////////////////

CCmdLineInfo::CCmdLineInfo(void) : 
	CCommandLineInfo()
{
	memset(&SharedData,0,sizeof(SharedDataStruct));
	SharedData.nPort = 6200;

	m_sStartupFileName = "";
	m_bUpgradingStartupFile = FALSE;
	m_sPlatform = "";

	m_nFtpPort = 221;
}


void CCmdLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bSwitch, BOOL bLast)
{
	CString sParam,sParam1,sValue("");
	CString sKey,sKey0,sKey1,sKey2,sKey3,sKey4,sKey5,sKey6,sKey7,sKey8,sKey9,sKeyx, sKeyxi, sKeyxii,sKeyxiii, sKeyxiv, sKeyxv, sKeyxvi;
	CString sKeySDT, sKeyIVM, sKeyStudyGUID, sKeyxx1, sKeyxx2, sKeyxx3;
	int nPos,nPos0,nPos1,nPos2,nPos3,nPos4,nPos5,nPos6,nPos7,nPos8,nPos9,nPosx, nPosxi, nPosxii, nPosxiii, nPosxiv, nPosxv, nPosxvi;
	int nPosSDT, nPosIVM, nPosStudyGUID, nPosxx1, nPosxx2, nPosxx3;
	int nKeyLen,nKey0Len,nKey1Len,nKey2Len,nKey3Len,nKey4Len,nKey5Len,nKey6Len,nKey7Len,nKey8Len,nKey9Len,nKeyxLen,nKeyxiLen, nKeyxiiLen, nKeyxiiiLen, nKeyxivLen, nKeyxvLen, nKeyxviLen;
	int nKeySDTLen, nKeyIVMLen, nKeyStudyGUIDLen, nKeyxx1Len, nKeyxx2Len, nKeyxx3Len;
	DWORD_PTR hWnd;
	BOOL bHandled;

	sParam = lpszParam;
	sParam1 = lpszParam;
	sParam1.MakeLower();

	    
	bHandled = FALSE;

	if (bSwitch)
    {
		if ((_tcsicmp(lpszParam,_T("stop")) == 0) || (_tcsicmp(lpszParam,_T("quit")) == 0) ||
				(_tcsicmp(lpszParam,_T("exit")) == 0))
		{
			SharedData.bExit = TRUE;
			bHandled = TRUE;
		}
	}

	if (!bHandled)
	{

		sKey = "upgrade32=";
		nKeyLen = sKey.GetLength();
		nPos = sParam1.Find(sKey,0);

		if (nPos >= 0)
		{
			sValue = sParam.Mid(nPos + nKeyLen);
			m_bUpgradingStartupFile = TRUE;
			m_sPlatform = "WIN32";
			m_sStartupFileName = sValue;
		}

		sKey = "upgrade64=";
		nKeyLen = sKey.GetLength();
		nPos = sParam1.Find(sKey,0);

		if (nPos >= 0)
		{
			sValue = sParam.Mid(nPos + nKeyLen);
			m_bUpgradingStartupFile = TRUE;
			m_sPlatform = "WIN64";
			m_sStartupFileName = sValue;
		}

		
		sKey = "ftpport=";
		nKeyLen = sKey.GetLength();
		nPos = sParam1.Find(sKey,0);

		if (nPos >= 0)
		{
			sValue = sParam.Mid(nPos + nKeyLen);
			
			m_nFtpPort = _ttoi(sValue);

			if (m_nFtpPort <= 0 || m_nFtpPort > 65535) m_nFtpPort = 221;
		}
		

		sKey0 = "hwnd=";
		nKey0Len = sKey0.GetLength();
		nPos0 = sParam1.Find(sKey0,0);

		sKey1 = "host=";
		nKey1Len = sKey1.GetLength();
		nPos1 = sParam1.Find(sKey1,0);

		sKey2 = "port=";
		nKey2Len = sKey2.GetLength();
		nPos2 = sParam1.Find(sKey2,0);

		sKey3 = "exp=";
		nKey3Len = sKey3.GetLength();
		nPos3 = sParam1.Find(sKey3,0);

		sKey4 = "key=";
		nKey4Len = sKey4.GetLength();
		nPos4 = sParam1.Find(sKey4,0);

		sKey5 = "removestartup=";
		nKey5Len = sKey5.GetLength();
		nPos5 = sParam1.Find(sKey5,0);

		sKey6 = "sys=";
		nKey6Len = sKey6.GetLength();
		nPos6 = sParam1.Find(sKey6,0);

		sKey7 = "filename=";
		nKey7Len = sKey7.GetLength();
		nPos7 = sParam1.Find(sKey7,0);

		sKey8 = "modality=";
		nKey8Len = sKey8.GetLength();
		nPos8 = sParam1.Find(sKey8,0);

		sKey9 = "clearall=";
		nKey9Len = sKey8.GetLength();
		nPos9 = sParam1.Find(sKey9,0);

		sKeyx = "unlock=";
		nKeyxLen = sKeyx.GetLength();
		nPosx = sParam1.Find(sKeyx,0);

		sKeyxi = "orderno=";
		nKeyxiLen = sKeyxi.GetLength();
		nPosxi = sParam1.Find(sKeyxi,0);

		sKeyxii = "inpatientno=";
		nKeyxiiLen = sKeyxii.GetLength();
		nPosxii = sParam1.Find(sKeyxii,0);

		sKeyxiii = "systemcode=";
		nKeyxiiiLen = sKeyxiii.GetLength();
		nPosxiii = sParam1.Find(sKeyxiii,0);

		sKeyxiv = "outpatientno=";
		nKeyxivLen = sKeyxiv.GetLength();
		nPosxiv = sParam1.Find(sKeyxiv, 0);

		sKeyxv = "patientid=";
		nKeyxvLen = sKeyxv.GetLength();
		nPosxv = sParam1.Find(sKeyxv, 0);

		sKeyxvi = "docid=";
		nKeyxviLen = sKeyxvi.GetLength();
		nPosxvi = sParam1.Find(sKeyxvi, 0);

		// 
		sKeySDT = "sdt=";
		nKeySDTLen = sKeySDT.GetLength();
		nPosSDT = sParam1.Find(sKeySDT, 0);

		sKeyIVM = "ivm=";
		nKeyIVMLen = sKeyIVM.GetLength();
		nPosIVM = sParam1.Find(sKeyIVM, 0);

		sKeyStudyGUID = "studyguid=";
		nKeyStudyGUIDLen = sKeyStudyGUID.GetLength();
		nPosStudyGUID = sParam1.Find(sKeyStudyGUID, 0);

		sKeyxx1 = "accno=";
		nKeyxx1Len = sKeyxx1.GetLength();
		nPosxx1 = sParam1.Find(sKeyxx1, 0);

		sKeyxx2 = "studyuid=";
		nKeyxx2Len = sKeyxx2.GetLength();
		nPosxx2 = sParam1.Find(sKeyxx2, 0);

		sKeyxx3 = "studydate=";
		nKeyxx3Len = sKeyxx3.GetLength();
		nPosxx3 = sParam1.Find(sKeyxx3, 0);

		if (nPos0 >= 0)
		{
			sValue = sParam.Mid(nPos0 + nKey0Len);
			SharedData.hWnd = (HWND) (_ttol(sValue));
		}
			
		if (nPos1 >= 0)
		{
			sValue = sParam.Mid(nPos1 + nKey1Len);
			_tcscpy(SharedData.szHost,sValue);
		}
		else if (nPos2 >= 0 && nPos2 < 2 )
		{
			// 区别于 ftpport=
			sValue = sParam.Mid(nPos2 + nKey2Len);
			SharedData.nPort = _ttoi(sValue);
		}
		else if (nPos3 >= 0)
		{
			sValue = sParam.Mid(nPos3 + nKey3Len);
			_tcscpy_s(SharedData.szExp,200,sValue);
		}
		else if (nPos4 >= 0)
		{
			sValue = sParam.Mid(nPos4 + nKey4Len);
			_tcscpy_s(SharedData.szKey,32,sValue);
		}
		else if (nPos5 >= 0)
		{
			sValue = sParam.Mid(nPos5 + nKey5Len);
			DeleteFile(sValue);
		}
		else if (nPos6 >= 0)
		{
			sValue = sParam.Mid(nPos6 + nKey6Len);
			_tcscpy(SharedData.szSystem,sValue);
		}
		else if (nPos7 >= 0)
		{
			sValue = sParam.Mid(nPos7 + nKey7Len);
			_tcscpy_s(SharedData.szFileName,250,sValue);
		}
		else if (nPos8 >= 0)
		{	
			sValue = sParam.Mid(nPos8 + nKey8Len);
			_tcscpy_s(SharedData.szModality,20,sValue);
		}
		else if (nPos9 >= 0)
		{
			sValue = sParam.Mid(nPos9 + nKey9Len);
			if (_tcsicmp(sValue,_T("true")) == 0)
			{
				SharedData.bClearAll = true;
			}
		}
		else if (nPosx >= 0)
		{
			sValue = sParam.Mid(nPosx + nKeyxLen);
			//
		}
		else if (nPosxi >= 0)
		{
			sValue = sParam.Mid(nPosxi + nKeyxiLen);
			_tcscpy_s(SharedData.szOrderNo, 30, sValue);
		}
		else if (nPosxii >= 0)
		{
			sValue = sParam.Mid(nPosxii + nKeyxiiLen);
			_tcscpy_s(SharedData.szInpatientNo, 20, sValue);
		}
		else if (nPosxiii >= 0)
		{
			sValue = sParam.Mid(nPosxiii + nKeyxiiiLen);
			_tcscpy_s(SharedData.szSystem, 10, sValue); 
		}
		else if (nPosxiv >= 0)
		{
			sValue = sParam.Mid(nPosxiv + nKeyxivLen);
			_tcscpy_s(SharedData.szOutpatientNo, 20, sValue);
		}
		else if (nPosxv >= 0)
		{
			sValue = sParam.Mid(nPosxv + nKeyxvLen);
			_tcscpy_s(SharedData.szPatientID, 30, sValue);
		}
		else if (nPosxvi >= 0)
		{
			sValue = sParam.Mid(nPosxvi + nKeyxviLen);
			_tcscpy_s(SharedData.szDocID, 20, sValue);
		}
		else if (nPosSDT >= 0)
		{
			sValue = sParam.Mid(nPosSDT + nKeySDTLen);
			SharedData.nSDT = _ttoi(sValue);
		}
		else if (nPosIVM >= 0)
		{
			sValue = sParam.Mid(nPosIVM + nKeyIVMLen);
			SharedData.nIVM = _ttoi(sValue);
		}
		else if (nPosStudyGUID >= 0)
		{
			sValue = sParam.Mid(nPosStudyGUID + nKeyStudyGUIDLen);
			_tcscpy_s(SharedData.szStudyGUID, 38, sValue);
		}
		else if (nPosxx1 > 0)	// accno
		{
			sValue = sParam.Mid(nPosxx1 + nKeyxx1Len);
			_tcscpy_s(SharedData.szAccessionNo, 30, sValue);
		}
		else if (nPosxx2 > 0)	// studyuid
		{
			sValue = sParam.Mid(nPosxx2 + nKeyxx2Len);
			_tcscpy_s(SharedData.szStudyUID, 64, sValue);
		}
		else if (nPosxx3 > 0)	// studydate
		{
			sValue = sParam.Mid(nPosxx3 + nKeyxx3Len);
			_tcscpy_s(SharedData.szStudyDate, 20, sValue);
		}
	}
}


// CCloudERVApp

BEGIN_MESSAGE_MAP(CCloudERVApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CCloudERVApp::OnAppAbout)
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CCloudERVApp construction

CCloudERVApp::CCloudERVApp()
{
	m_hMutex = NULL;
	m_sHISProgramFileName = _T("");

	// ImageViewer 新的影像浏览器的AETitle
	m_sImageViewerAET = _T("");
	m_nImageViewerPort = 0;

	m_sPatientNameLabel = _T("");

	m_sLockedRefFromDept = _T("");

	m_bRecentRefDeptAndRefDoctorListChanged = FALSE;
	m_bRecentRefDeptAndRefDoctorListInited = FALSE;

	m_bPositiveFlagValidForEIS = TRUE;
}


// The one and only CCloudERVApp object

CCloudERVApp theApp;


const TCHAR * CCloudERVApp::GetPatientNameLabel()
{
	if (m_sPatientNameLabel.IsEmpty())
		return _T("患者姓名");

	return m_sPatientNameLabel;
}

void CCloudERVApp::UpgradeStartupFile(const TCHAR *szLocalFileName,const TCHAR *szPlatform,const TCHAR *szLicHost,int nPort)
{
	//TODO 
	const TCHAR *szClassName = _T("StartupClassNameByQiuzhenLiu");
	TCHAR szSrcFileName[MAX_PATH + 1];
	HWND hWnd;
	nsFTP::CFTPClient ftpClient;
	nsFTP::CLogonInfo logonInfo(szLicHost, nPort,_T("system4lic"),_T("mAsTeRkEy#71217"));
	BOOL bFtpOk;

	hWnd = FindWindow(szClassName,NULL);

	if (hWnd != NULL)
	{
		SendMessage(hWnd,WM_COMMAND,IDOK,0);
	}

	Sleep(50);

	_stprintf(szSrcFileName,_T("//Downloads/%s/Launch/launch.exe"), szPlatform);

	ftpClient.SetResumeMode(false);
	
	bFtpOk = ftpClient.Login(logonInfo);
	
	if (bFtpOk)
	{
		// do file operations
		ftpClient.DownloadFile(szSrcFileName,szLocalFileName);

		ftpClient.Logout();
	}
}


const TCHAR *CCloudERVApp::GetInifileStr(const TCHAR *szSectionName, const TCHAR *szKeyName, const TCHAR *szDefault, const TCHAR *szIniFileName)
{
	static TCHAR szValue[201];
	int nSize = 200;
	
	memset(szValue, 0, nSize * sizeof(TCHAR));

	::GetPrivateProfileString(szSectionName, szKeyName, szDefault, szValue, nSize, szIniFileName);

	return szValue;
}

int	  CCloudERVApp::GetInifileInt(const TCHAR *szSectionName, const TCHAR *szKeyName, int nDefault, const TCHAR *szIniFileName)
{
	int nValue = 0;
	nValue = ::GetPrivateProfileInt(szSectionName, szKeyName, nDefault, szIniFileName);
	return nValue;
}

// CCloudERVApp initialization

BOOL CCloudERVApp::InitInstance()
{
	HWND    hPrevWnd;
	CCmdLineInfo cmdInfo;
	BOOL bSendOk;
	DWORD dwTickCount = 0;
	const TCHAR *szSettingSection = _T("Settings");
	const TCHAR *szCompanyName = _T("AQIDBAUGBwgJCgsMDQ4PELP/xJp1ewwRqwcBRgS8/dQdeNnOgb+4zSJ6TjwdPv5E");
	std::string strBase64, strDecrypted, strAes;
	CString sLicHost(""), sHospitalID(""), sValidSystems(""), sCompanyName, sMapName, sAppTitle("");
	CString sImageViewerFileNameWithPath("");
	CString sWebViewerUrl(""), sWebViewerInitParams(""), sWebViewerUserId(""), sWebViewerPassword("");
	CString sDicomServerHost(""), sDicomServerAET("");
	int nZQPETSystemValid = 0;
	int nDicomServerPort = 0;
	int nLen,nErrNo, nPos = 0;
	int nLicPort = 0;
	TCHAR szIniFile[MAX_PATH + 1], *pch;
	int nSize;
	HANDLE hMapFile;
	void *pBuf;
    WSAData winSockData;
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;	
	FILE *fp;
	HRSRC hRes;
	HGLOBAL hResData;
	CString sLicMode("");
	BOOL bDemoMode = FALSE;


	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

    if (!AfxOleInit())
        return FALSE;
  
	AfxEnableControlContainer();

	CWinApp::InitInstance();
	WSAStartup(winSockVersionNeeded, &winSockData);

	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_bUpgradingStartupFile)
	{
		UpgradeStartupFile(cmdInfo.m_sStartupFileName,cmdInfo.m_sPlatform,SharedData.szHost,cmdInfo.m_nFtpPort);
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	
	SetRegistryKey(_T("CloudERV"));
	
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	
	::GetModuleFileName(NULL, szIniFile, MAX_PATH);

	pch = _tcsrchr(szIniFile, _T('.'));
	*pch = _T('\0');
	_tcscat(szIniFile, _T(".ini"));

	strcpy(g_lpszConfigFileName, CStringA(szIniFile));
	///////////////////////////////////////////////////////////////////////

	m_hMutex = CreateMutex(NULL, FALSE, g_lpszClassName);   // Create mutex

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		m_hMutex = NULL;
		hPrevWnd = FindWindowEx(NULL,NULL,g_lpszClassName,NULL);
		if (hPrevWnd != NULL)
		{	        
			if (SharedData.bExit)
			{
				PostMessage(hPrevWnd,WM_COMMAND,ID_APP_EXIT,0);
			}
			else
			{
				bSendOk = FALSE;

				dwTickCount = ::GetTickCount();
				sMapName.Format(_T("MAP3888%u"),dwTickCount);
				nSize = sizeof(SharedDataStruct);

				hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nSize, sMapName);
				
				if (hMapFile != INVALID_HANDLE_VALUE)
				{

					pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nSize);
					
					memcpy(pBuf,&SharedData,nSize);

					UnmapViewOfFile(pBuf);
					bSendOk = SendMessage(hPrevWnd,0x3888,dwTickCount,0);

		        	CloseHandle(hMapFile);
				}

				if (bSendOk)
				{

					PostMessage(hPrevWnd,WM_REFRESHPARAMS,0,0);

					if (IsIconic(hPrevWnd))
					{
						ShowWindow(hPrevWnd, SW_RESTORE);
					}

					SetForegroundWindow(hPrevWnd);
				}
			}
		}

		return FALSE;
	}
	else
	{
		if (SharedData.bExit) return FALSE;

		// Register the unique window class name so others can find it.
		WNDCLASSEX wndcls; 

		memset(&wndcls, 0, sizeof(WNDCLASSEX));

		wndcls.cbSize = sizeof(WNDCLASSEX);
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = AfxWndProc;
		wndcls.hInstance = AfxGetInstanceHandle();
		wndcls.hIcon = ::LoadIcon( wndcls.hInstance, MAKEINTRESOURCE(IDR_MAINFRAME) );//or AFX_IDI_STD_FRAME; 
		wndcls.hCursor = ::LoadCursor( wndcls.hInstance, IDC_ARROW );
		wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = g_lpszClassName;

		if (!RegisterClassEx(&wndcls))
		{
			AfxMessageBox( _T("Failed to register window class!"), MB_ICONSTOP | MB_OK );
			return FALSE;
		}
	}



	hFind = ::FindFirstFile(szIniFile, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}
	else
	{
		hRes = ::FindResource(NULL,_T("IDR_CONFIGFILE"),_T("CFG"));

		if (hRes != NULL)
		{
			hResData = LoadResource(NULL,hRes);

			if (hResData != NULL)
			{
				pBuf = ::LockResource(hResData);
				nLen = ::SizeofResource(NULL,hRes);

				if (pBuf != NULL && nLen > 0)
				{
					nErrNo = _tfopen_s(&fp, szIniFile, _T("wb"));

					if (nErrNo == 0)
					{
						fwrite(pBuf,nLen,1,fp);
						fclose(fp);
					}
				}
			}

		}

	}


	sLicMode = GetInifileStr(szSettingSection, _T("LicMode"), L"", szIniFile);
	if (sLicMode.CompareNoCase(_T("Demo")) == 0)
		bDemoMode = TRUE;
	else
		bDemoMode = FALSE;

	if (_tcslen(SharedData.szHost) == 0) {
		sLicHost	= GetInifileStr(szSettingSection,  _T("LicHost"), L"", szIniFile);
		nLicPort	= GetInifileInt(szSettingSection, _T("LicPort"), 0, szIniFile);
	}
	else {
		sLicHost	= SharedData.szHost;
		nLicPort	= SharedData.nPort;
	}

	sHospitalID = GetInifileStr(szSettingSection, _T("HospitalID"), L"", szIniFile);
	sValidSystems = GetInifileStr(szSettingSection, _T("ValidSystems"), L"", szIniFile);

	nZQPETSystemValid = GetInifileInt(szSettingSection, _T("ZQPETCTValid"), 1, szIniFile);
	m_bZQPETSystemValid = (nZQPETSystemValid > 0);

	sCompanyName = GetInifileStr(szSettingSection, _T("CompanyName"), L"", szIniFile);
	strBase64 = CStringA(sCompanyName);
	strDecrypted = "";
	try {
		strAes = ::base64_decode(strBase64);
		strDecrypted = aes_decrypt(strAes, "");
	}
	catch(...) {
			
	}

	m_sCompanyName = strDecrypted.c_str();
	if (m_sCompanyName.Left(1) != _T("*")) 
		m_sCompanyName = "";
	else
		m_sCompanyName = m_sCompanyName.Mid(1);


	if (m_sCompanyName.IsEmpty()) {

		strBase64 = CStringA(szCompanyName);
		strDecrypted = "";
		try {
			strAes = ::base64_decode(strBase64);
			strDecrypted = aes_decrypt(strAes, "");
		}
		catch(...) {
			
		}

		m_sCompanyName = strDecrypted.c_str();
	}

	m_sHospitalID = sHospitalID;
	m_bSearchOnlyWithinHospital = (GetInifileInt(szSettingSection, _T("SearchOnlyWithinHospital"), 1, szIniFile) == 1);

	m_sPatientNameLabel = GetInifileStr(szSettingSection, _T("PatientNameLabel"), _T("患者姓名"), szIniFile);

	m_sLockedRefFromDept = GetInifileStr(szSettingSection, _T("LockedRefFromDept"), _T(""), szIniFile);

	m_bIgnoreStudyDateWhileSearchingByIdentity = (GetInifileInt(szSettingSection, _T("IgnoreStudyDateWhileSearchingByIdentity"), 1, szIniFile) == 1);

	m_nAutoLoadImagesForSingleResultOfCmdLineMode = GetInifileInt(szSettingSection, _T("AutoLoadImage"), 0, szIniFile);
	m_nDefaultSearchFromHCS = GetInifileInt(szSettingSection, _T("DefaultSearchFromHCS"), 1, szIniFile);
	m_bLockSearchMode = (GetInifileInt(szSettingSection, _T("LockSearchMode"), 1, szIniFile) == 1);

	m_bExportImagesEnabled = (GetInifileInt(szSettingSection, _T("ExportImagesEnabled"), 0, szIniFile) == 1);

	m_bPositiveFlagValidForEIS = (GetInifileInt(szSettingSection, _T("PositiveFlagValidForEIS"), 1, szIniFile) == 1);

	m_nMaxImageViewerWindowsNum = GetInifileInt(szSettingSection, _T("MaxImageViewerWindowsNum"), 1, szIniFile);

	if (m_nMaxImageViewerWindowsNum < 0)
		m_nMaxImageViewerWindowsNum = 1;
	
	if (m_nMaxImageViewerWindowsNum > 32)
		m_nMaxImageViewerWindowsNum = 32;
		
	m_nReportResultGridColumnWidthSaveMode = GetInifileInt(szSettingSection, _T("ReportResultGridColumnWidthSaveMode"), 0, szIniFile);

	sWebViewerUrl = GetInifileStr(szSettingSection, _T("WebViewerUrl"), _T(""), szIniFile);
	if (!sWebViewerUrl.IsEmpty()) {
		g_sWebViewerUrl = sWebViewerUrl;
	}

	sWebViewerInitParams = GetInifileStr(szSettingSection, _T("WebViewerInitParams"), _T(""), szIniFile);
	if (!sWebViewerInitParams.IsEmpty()) {
		if (sWebViewerInitParams.Find(_T("%s") > 0)) 
			g_sWebViewerInitParams = sWebViewerInitParams;
	}

	sWebViewerUserId = GetInifileStr(szSettingSection, _T("WebViewerUserId"), _T(""), szIniFile);
	sWebViewerPassword = GetInifileStr(szSettingSection, _T("WebViewerPassword"), _T(""), szIniFile);

	if (!sWebViewerUserId.IsEmpty() && !sWebViewerPassword.IsEmpty()) {

		
		strBase64 = CStringA(sWebViewerUserId);
		try {
			strAes = ::base64_decode(strBase64);
			strDecrypted = aes_decrypt(strAes, "");
		}
		catch(...) {
			
		}

		g_sWebViewerUserId = strDecrypted.empty() ? sWebViewerUserId : CString(strDecrypted.c_str());

		strBase64 = CStringA(sWebViewerPassword);
		try {
			strAes = ::base64_decode(strBase64);
			strDecrypted = aes_decrypt(strAes, "");
		}
		catch(...) {

		}

		g_sWebViewerPassword = strDecrypted.empty() ? sWebViewerPassword : CString(strDecrypted.c_str());
	}

	sImageViewerFileNameWithPath = GetInifileStr(szSettingSection, _T("ImageViewerFileName"), _T(""), szIniFile);
	if (!sImageViewerFileNameWithPath.IsEmpty()) {
		nPos = sImageViewerFileNameWithPath.ReverseFind(_T('\\'));
		if (nPos > 0) {
			g_sImageViewerFileName = sImageViewerFileNameWithPath.Mid(nPos + 1);
			g_sImageViewerPath = sImageViewerFileNameWithPath.Mid(0, nPos);
			g_sImageViewerFileNameWithPath = sImageViewerFileNameWithPath;
		}
		else {
			nPos = sImageViewerFileNameWithPath.ReverseFind(_T('/'));
			if (nPos > 0) {
				g_sImageViewerFileName = sImageViewerFileNameWithPath.Mid(nPos + 1);
				g_sImageViewerPath = sImageViewerFileNameWithPath.Mid(0, nPos);
				g_sImageViewerFileNameWithPath = sImageViewerFileNameWithPath;
			}
		}
	}

	nDicomServerPort = GetInifileInt(szSettingSection, _T("RemotePrimaryDicomServerPort"), 0, szIniFile);
	sDicomServerHost = GetInifileStr(szSettingSection, _T("RemotePrimaryDicomServerHost"), _T(""), szIniFile);
	sDicomServerAET  = GetInifileStr(szSettingSection, _T("RemotePrimaryDicomServerAET"), _T(""), szIniFile);

	if (!sDicomServerHost.IsEmpty() && !sDicomServerAET.IsEmpty() && nDicomServerPort > 0) {
		g_sPrimaryDicomServerHost = sDicomServerHost;
		g_sPrimaryDicomServerAET = sDicomServerAET;
		g_nPrimaryDicomServerPort = nDicomServerPort;
	}

	nDicomServerPort = GetInifileInt(szSettingSection, _T("RemoteSecondaryDicomServerPort"), 0, szIniFile);
	sDicomServerHost = GetInifileStr(szSettingSection, _T("RemoteSecondaryDicomServerHost"), _T(""), szIniFile);
	sDicomServerAET  = GetInifileStr(szSettingSection, _T("RemoteSecondaryDicomServerAET"), _T(""), szIniFile);

	if (!sDicomServerHost.IsEmpty() && !sDicomServerAET.IsEmpty() && nDicomServerPort > 0) {
		g_sSecondaryDicomServerHost = sDicomServerHost;
		g_sSecondaryDicomServerAET = sDicomServerAET;
		g_nSecondaryDicomServerPort = nDicomServerPort;
	}

	// Legacy DicomServer
	nDicomServerPort = GetInifileInt(szSettingSection, _T("LegacyRemotePrimaryDicomServerPort"), 0, szIniFile);
	sDicomServerHost = GetInifileStr(szSettingSection, _T("LegacyRemotePrimaryDicomServerHost"), _T(""), szIniFile);
	sDicomServerAET  = GetInifileStr(szSettingSection, _T("LegacyRemotePrimaryDicomServerAET"), _T(""), szIniFile);

	if (!sDicomServerHost.IsEmpty() && !sDicomServerAET.IsEmpty() && nDicomServerPort > 0) {
		g_sLegacyPrimaryDicomServerHost = sDicomServerHost;
		g_sLegacyPrimaryDicomServerAET = sDicomServerAET;
		g_nLegacyPrimaryDicomServerPort = nDicomServerPort;
	}


	nDicomServerPort = GetInifileInt(szSettingSection, _T("LegacyRemoteSecondaryDicomServerPort"), 0, szIniFile);
	sDicomServerHost = GetInifileStr(szSettingSection, _T("LegacyRemoteSecondaryDicomServerHost"), _T(""), szIniFile);
	sDicomServerAET  = GetInifileStr(szSettingSection, _T("LegacyRemoteSecondaryDicomServerAET"), _T(""), szIniFile);

	if (!sDicomServerHost.IsEmpty() && !sDicomServerAET.IsEmpty() && nDicomServerPort > 0) {
		g_sLegacySecondaryDicomServerHost = sDicomServerHost;
		g_sLegacySecondaryDicomServerAET = sDicomServerAET;
		g_nLegacySecondaryDicomServerPort = nDicomServerPort;
	}

	sDicomServerAET	 = GetInifileStr(szSettingSection, _T("LocalDicomServerAET"), _T(""), szIniFile);
	nDicomServerPort = GetInifileInt(szSettingSection, _T("LocalDicomServerPort"), 0, szIniFile);

	if (sDicomServerAET.IsEmpty() || sDicomServerAET.CompareNoCase(_T("Auto")) == 0 || sDicomServerAET.Find(_T("$")) >= 0 )
		GetLocalAET(sDicomServerAET);

	if (nDicomServerPort == 0)
		nDicomServerPort = 104;

	g_sLocalDicomServerAET = sDicomServerAET;
	g_nLocalDicomServerPort = nDicomServerPort;

	g_nDicomMaxPDUSize	= GetInifileInt(szSettingSection, _T("DicomMaxPDUSize"), 0, szIniFile);

	sDicomServerAET		= GetInifileStr(szSettingSection, _T("ImageViewerAET"), _T(""), szIniFile);
	nDicomServerPort	= GetInifileInt(szSettingSection, _T("ImageViewerPort"), 0, szIniFile);
	
	if (sDicomServerAET.IsEmpty() || sDicomServerAET.CompareNoCase(_T("Auto")) == 0 || sDicomServerAET.Find(_T("$")) >= 0)
		GetImageViewerAET(sDicomServerAET);

	if (nDicomServerPort == 0)
		nDicomServerPort = 104;

	g_sImageViewerAET = sDicomServerAET;
	g_nImageViewerPort = nDicomServerPort;

	// 优化老系统数据调取(C-Move)
	m_bOptimizeLegacyLoading = (GetInifileInt(szSettingSection, _T("OptimizeLegacyLoading"), 0, szIniFile) == 1);
	g_bOptimizeLegacyLoading = m_bOptimizeLegacyLoading;

	m_nDefaultCMoveParamMode = GetInifileInt(szSettingSection, _T("DefaultCMoveParamMode"), 1, szIniFile);

	if (m_nDefaultCMoveParamMode < 1 || m_nDefaultCMoveParamMode > 4)
		m_nDefaultCMoveParamMode = 1;


	m_nValidDaysOfCmdLineQuery = GetInifileInt(szSettingSection, _T("ValidDaysOfCmdLineQuery"), 365, szIniFile);
	
	if (m_nValidDaysOfCmdLineQuery == 0)
		m_nValidDaysOfCmdLineQuery = 365;

	if (m_nValidDaysOfCmdLineQuery < 7) 
		m_nValidDaysOfCmdLineQuery = 7;

	if (m_nValidDaysOfCmdLineQuery > 365 * 3)
		m_nValidDaysOfCmdLineQuery = 365 * 3;


	m_bAutoClosePriorWorkspace = (GetInifileInt(szSettingSection, _T("AutoClosePriorWorkspace"), 1, szIniFile) != 0);

	m_sHISProgramFileName = GetInifileStr(szSettingSection, _T("HISProgramFileName"), _T(""), szIniFile);

	if (!m_SystemConfiguration.Initialize(sLicHost, sHospitalID, nLicPort, sValidSystems)) {
		if (!bDemoMode) {
			::MessageBox(NULL, _T("系统连接信息初始化失败!"), _T("错误提示"), MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
	}
	// 在这里加入肇庆ＰＥＴ—CT的数据库连接参数
	if (nZQPETSystemValid > 0) {
		m_SystemConfiguration.AddSystemInfo_HardCode(_T("PET"), _T("PET"), _T("000000"), _T("10.20.20.183"), _T("medexmeMrs"),_T("MSSQL"), _T("dbo"),_T("petct"),_T("petct1234"),_T(""));
	}


	dcmDisableGethostbyaddr.set(OFTrue);
	DcmAssociationConfigurationFile::initialize(g_Asccfg, g_lpszConfigFileName);

	sAppTitle = m_SystemConfiguration.GetSystemName(_T("CloudERV"));
	if (sAppTitle.IsEmpty())
		sAppTitle.LoadString(AFX_IDS_APP_TITLE);

	m_pQueryPublishedReportDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CPublishedReportDoc),
		RUNTIME_CLASS(CPublishedReportFrame),
		RUNTIME_CLASS(CPublishedReportView));

	if (!m_pQueryPublishedReportDocTemplate)
		return FALSE;

	AddDocTemplate(m_pQueryPublishedReportDocTemplate);
	
	m_pPdfReportViewDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CPdfReportDoc),
		RUNTIME_CLASS(CPdfReportFrame), // custom MDI child frame
		RUNTIME_CLASS(CPdfReportView));

	if (!m_pPdfReportViewDocTemplate)
		return FALSE;

	AddDocTemplate(m_pPdfReportViewDocTemplate);

	m_pDcmImageViewDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CDcmImageDoc),
		RUNTIME_CLASS(CDcmImageFrame), // custom MDI child frame
		RUNTIME_CLASS(CDcmImageView));
	
	if (!m_pDcmImageViewDocTemplate)
		return FALSE;
	
	AddDocTemplate(m_pDcmImageViewDocTemplate);


	m_pDcmSeriesCompareDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CDcmSeriesCompareDoc),
		RUNTIME_CLASS(CDcmSeriesCompareFrame),
		RUNTIME_CLASS(CDcmSeriesCompareView));

	if (!m_pDcmSeriesCompareDocTemplate)
		return FALSE;

	AddDocTemplate(m_pDcmSeriesCompareDocTemplate);


	m_pExtDcmImageViewDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CDcmImageDoc),
		RUNTIME_CLASS(CDcmImageFrame), // custom MDI child frame
		RUNTIME_CLASS(CDcmImageView));
	
	if (!m_pExtDcmImageViewDocTemplate)
		return FALSE;
	
	AddDocTemplate(m_pExtDcmImageViewDocTemplate);


	m_pBmpReportViewDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CBitmapViewerDoc),
		RUNTIME_CLASS(CBitmapViewerFrame), // custom MDI child frame
		RUNTIME_CLASS(CBitmapViewerView));
	
	if (!m_pBmpReportViewDocTemplate)
		return FALSE;
	
	AddDocTemplate(m_pBmpReportViewDocTemplate);

	m_pExtBmpImageViewDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CBitmapViewerDoc),
		RUNTIME_CLASS(CBitmapViewerFrame), // custom MDI child frame
		RUNTIME_CLASS(CBitmapViewerView));
	
	if (!m_pExtBmpImageViewDocTemplate)
		return FALSE;
	
	AddDocTemplate(m_pExtBmpImageViewDocTemplate);

	m_pBookingOfRISDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CBookingOfRISDoc),
		RUNTIME_CLASS(CBookingOfRISFrame),
		RUNTIME_CLASS(CBookingOfRISView));

	if (!m_pBookingOfRISDocTemplate)
		return FALSE;

	AddDocTemplate(m_pBookingOfRISDocTemplate);

	m_pWebViewerDocTemplate = new CMultiDocTemplate(IDR_DUMMY,
		RUNTIME_CLASS(CWebBrowserDoc),
		RUNTIME_CLASS(CWebBrowserFrame),
		RUNTIME_CLASS(CWebBrowserView));

	if (!m_pWebViewerDocTemplate)
		return FALSE;

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	
	pMainFrame->SetTitle(sAppTitle);
	pMainFrame->LoadAccelTable(_T("IDR_MAINFRAME"));

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd

	//////////////////////////////////////////////////////
	// Modified at 2007-01-11 
	// disable hostname lookup

    dcmDisableGethostbyaddr.set(OFTrue);               // disable hostname lookup

	//////////////////////////////////////////////////////
	DJEncoderRegistration::registerCodecs(ECC_monochrome);
	DJDecoderRegistration::registerCodecs(); // register JPEG codecs
    // register RLE decompression codec
    DcmRLEDecoderRegistration::registerCodecs(OFFalse /*pCreateSOPInstanceUID*/, OFFalse);

	// Parse command line for standard shell commands, DDE, file open
	ParseCommandLine(cmdInfo);
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->SetTaskPaneAutoHide(TRUE);


	// 检查命令行参数， 打开相应的记录
	PostMessage(pMainFrame->m_hWnd,WM_REFRESHPARAMS,0,0);

	m_pQueryPublishedReportDocTemplate->OpenDocumentFile(NULL);

	return TRUE;
}


int CCloudERVApp::ExitInstance()
{
	DWORD dwProcessId;
	HANDLE hProcess;
	std::vector <TCHAR *> ::iterator itStr;

	if (m_bRecentRefDeptAndRefDoctorListChanged) 
		WriteRecentRefDoctorAndRefDeptList();

	// Clear Recent RefDept & RefDoctor List
	for (itStr = m_lstRecentRefDeptName.begin(); itStr != m_lstRecentRefDeptName.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentRefDeptName.clear();

	for (itStr = m_lstRecentRefDoctorName.begin(); itStr != m_lstRecentRefDoctorName.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentRefDoctorName.clear();


	for (itStr = m_lstRecentPatientName.begin(); itStr != m_lstRecentPatientName.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentPatientName.clear();


	for (itStr = m_lstRecentInpatientNo.begin(); itStr != m_lstRecentInpatientNo.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentInpatientNo.clear();


	for (itStr = m_lstRecentDocID.begin(); itStr != m_lstRecentDocID.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentDocID.clear();

	//

	if (g_dwImageViewerProcessId != 0) {
		dwProcessId = ::FindProcessId(g_sImageViewerFileName);
		if (dwProcessId == g_dwImageViewerProcessId) {
			hProcess = OpenProcess(PROCESS_TERMINATE, false, dwProcessId);
			if (hProcess) {
				if (!TerminateProcess(hProcess, 1))
					CloseHandle(hProcess);
			}
		}
	}

	if (m_hMutex != NULL) {
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	DcmRLEDecoderRegistration::cleanup();
	DJDecoderRegistration::cleanup(); 
    DJEncoderRegistration::cleanup(); 

	WSACleanup();

	return CWinApp::ExitInstance();
}


void CCloudERVApp::ReadRecentRefDoctorAndRefDeptList()
{
	const TCHAR *szRecentRefDeptSection = _T("RecentRefDepts");
	const TCHAR *szRecentRefDoctorSection = _T("RecentRefDoctors");
	const TCHAR *szRecentPatientNameSection = _T("RecentPatientNames");
	const TCHAR *szRecentInpatientNoSection = _T("RecentInpatientNos");
	const TCHAR *szRecentDocIDSection = _T("RecentDocIDs");

	TCHAR szKey[10], *pItem;
	CString sItem;
	std::vector <TCHAR *> ::iterator itStr;
	int ni;

	for (itStr = m_lstRecentRefDeptName.begin(); itStr != m_lstRecentRefDeptName.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentRefDeptName.clear();

	for (itStr = m_lstRecentRefDoctorName.begin(); itStr != m_lstRecentRefDoctorName.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentRefDoctorName.clear();

	for (itStr = m_lstRecentPatientName.begin(); itStr != m_lstRecentPatientName.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentPatientName.clear();

	for (itStr = m_lstRecentInpatientNo.begin(); itStr != m_lstRecentInpatientNo.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentInpatientNo.clear();


	for (itStr = m_lstRecentDocID.begin(); itStr != m_lstRecentDocID.end(); itStr ++) 
		delete []  *(itStr);

	m_lstRecentDocID.clear();



	for (ni = 0; ni < 20; ni ++) {
		_stprintf(szKey, _T("Item%02d"), ni + 1); 
		
		sItem = GetProfileString(szRecentRefDeptSection, szKey, _T(""));
		if (!sItem.IsEmpty()) {
			pItem = new TCHAR[sItem.GetLength() + 1];
			_tcscpy(pItem, sItem);
			m_lstRecentRefDeptName.push_back(pItem);
		}

		sItem = GetProfileString(szRecentRefDoctorSection, szKey, _T(""));
		if (!sItem.IsEmpty()) {
			pItem = new TCHAR[sItem.GetLength() + 1];
			_tcscpy(pItem, sItem);
			m_lstRecentRefDoctorName.push_back(pItem); 
		}

		sItem = GetProfileString(szRecentPatientNameSection, szKey, _T(""));
		if (!sItem.IsEmpty()) {
			pItem = new TCHAR[sItem.GetLength() + 1];
			_tcscpy(pItem, sItem);
			m_lstRecentPatientName.push_back(pItem); 
		}


		sItem = GetProfileString(szRecentInpatientNoSection, szKey, _T(""));
		if (!sItem.IsEmpty()) {
			pItem = new TCHAR[sItem.GetLength() + 1];
			_tcscpy(pItem, sItem);
			m_lstRecentInpatientNo.push_back(pItem); 
		}


		sItem = GetProfileString(szRecentDocIDSection, szKey, _T(""));
		if (!sItem.IsEmpty()) {
			pItem = new TCHAR[sItem.GetLength() + 1];
			_tcscpy(pItem, sItem);
			m_lstRecentDocID.push_back(pItem); 
		}



	}

	m_bRecentRefDeptAndRefDoctorListInited = TRUE;
}


void CCloudERVApp::WriteRecentRefDoctorAndRefDeptList()
{
	const TCHAR *szRecentRefDeptSection = _T("RecentRefDepts");
	const TCHAR *szRecentRefDoctorSection = _T("RecentRefDoctors");
	const TCHAR *szRecentPatientNameSection = _T("RecentPatientNames");
	const TCHAR *szRecentInpatientNoSection = _T("RecentInpatientNos");
	const TCHAR *szRecentDocIDSection = _T("RecentDocIDs");


	TCHAR szKey[10];
	std::vector <TCHAR *> ::iterator itStr;
	int ni;

	for (ni = 0; ni < 20; ni ++) {
		_stprintf(szKey, _T("Item%02d"), ni + 1); 
		
		if (ni < m_lstRecentRefDeptName.size())
			WriteProfileString(szRecentRefDeptSection, szKey, m_lstRecentRefDeptName[ni] );
		else 
			WriteProfileString(szRecentRefDeptSection, szKey, _T(""));
			
		if (ni < m_lstRecentRefDoctorName.size())
			WriteProfileString(szRecentRefDoctorSection, szKey, m_lstRecentRefDoctorName[ni] );
		else
			WriteProfileString(szRecentRefDoctorSection, szKey, _T(""));


		if (ni < m_lstRecentPatientName.size())
			WriteProfileString(szRecentPatientNameSection, szKey, m_lstRecentPatientName[ni] );
		else
			WriteProfileString(szRecentPatientNameSection, szKey, _T(""));

		if (ni < m_lstRecentInpatientNo.size())
			WriteProfileString(szRecentInpatientNoSection, szKey, m_lstRecentInpatientNo[ni] );
		else
			WriteProfileString(szRecentInpatientNoSection, szKey, _T(""));

		if (ni < m_lstRecentDocID.size())
			WriteProfileString(szRecentDocIDSection, szKey, m_lstRecentDocID[ni] );
		else
			WriteProfileString(szRecentDocIDSection, szKey, _T(""));


	}
}

// Tag = 1, RefDept
// Tag = 2, RefDoctor
// Tag = 3, PatientName
// Tag = 4, InpatientNo
// Tag = 5, DocID

void CCloudERVApp::InsertNewItemToRecentDeptOrRefDoctorList(int nTag, const TCHAR *szItem)
{
	std::vector <TCHAR *> *pItemList = NULL;
	std::vector <TCHAR *> ::iterator itStr;
	TCHAR  *pItem;
	BOOL bFound = FALSE;

	if (nTag == 1)
		pItemList = &m_lstRecentRefDeptName;
	else if (nTag == 2)
		pItemList = &m_lstRecentRefDoctorName;
	else if (nTag == 3)
		pItemList = &m_lstRecentPatientName;
	else if (nTag == 4)
		pItemList = &m_lstRecentInpatientNo;
	else if (nTag == 5)
		pItemList = &m_lstRecentDocID;

	if (pItemList == NULL || szItem == NULL || _tcslen(szItem) == 0)
		return;

	bFound = FALSE;
	itStr = pItemList->begin();
	while (itStr != pItemList->end() && !bFound ) {
		if (*(itStr) != NULL && _tcslen(*(itStr)) > 0) {
			if (_tcscmp(szItem, *(itStr)) == 0) {

				if (itStr != pItemList->begin()) 
					m_bRecentRefDeptAndRefDoctorListChanged = TRUE;

				pItemList->erase(itStr);
				bFound = TRUE;
			}
		}

		itStr ++;
	}
	
	if (!bFound) 
		m_bRecentRefDeptAndRefDoctorListChanged = TRUE;

	pItem = new TCHAR[_tcslen(szItem) + 1];
	_tcscpy(pItem, szItem);
	pItemList->insert(pItemList->begin(), pItem);
}


// Tag = 1, RefDept
// Tag = 2, RefDoctor
// Tag = 3, PatientName
// Tag = 4, InpatientNo
// Tag = 5, DocID
void CCloudERVApp::MakeMenuOfRecentRefDeptOrRefDoctorList(int nTag, UINT nCommandID, CMenu *pMenu)
{
	std::vector <TCHAR *> *pItemList = NULL;
	std::vector <TCHAR *> ::iterator itStr;
	CString sItem;
	int nItemIndex = 0;

	if (nTag == 1)
		pItemList = &m_lstRecentRefDeptName;
	else if (nTag == 2)
		pItemList = &m_lstRecentRefDoctorName;
	else if (nTag == 3)
		pItemList = &m_lstRecentPatientName;
	else if (nTag == 4)
		pItemList = &m_lstRecentInpatientNo;
	else if (nTag == 5)
		pItemList = &m_lstRecentDocID;

	if (pMenu == NULL || pItemList == NULL)
		return;

	if (!m_bRecentRefDeptAndRefDoctorListInited)
		ReadRecentRefDoctorAndRefDeptList();


	nItemIndex = 0;
	for (itStr = pItemList->begin(); itStr != pItemList->end(); itStr ++) {
		if (*(itStr) != NULL && _tcslen(*(itStr)) > 0 && nItemIndex < 20) {

			pMenu->AppendMenu(MF_STRING, nCommandID + nItemIndex, *(itStr));
			nItemIndex ++;
		}
	}
}


void CCloudERVApp::OpenDocTemplate(CMultiDocTemplate *pDocTemplate)
{
	CDocument *pDoc;
	CView *pView;
	POSITION pos;

	pos = pDocTemplate->GetFirstDocPosition();

	if (pos == NULL) {
		pDocTemplate->OpenDocumentFile(NULL);
	}
	else {
		pDoc = pDocTemplate->GetNextDoc(pos);
		pos = pDoc->GetFirstViewPosition();
		if (pos) { 
			pView = pDoc->GetNextView(pos);
			pView->GetParentFrame()->ActivateFrame();	
		}
	}

	return;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CCloudERVApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CCloudERVApp::LaunchQueryPublishedReport()
{
	OpenDocTemplate(m_pQueryPublishedReportDocTemplate);
}


void CCloudERVApp::DcmSeriesCompare(CSeriesObject *pObj, bool bHeadPosition, int &nSeriesCount)
{
	CDcmSeriesCompareDoc *pDoc;
	CDcmSeriesCompareView *pDcmSeriesCompareView = NULL;
	CView *pView;
	POSITION pos;
	int nSeriesObjCount = 0;

	pos = m_pDcmSeriesCompareDocTemplate->GetFirstDocPosition();
	if (pos == NULL) 
		m_pDcmSeriesCompareDocTemplate->OpenDocumentFile(NULL);

	pos = m_pDcmSeriesCompareDocTemplate->GetFirstDocPosition();
	if (pos != NULL) {
		pDoc = dynamic_cast <CDcmSeriesCompareDoc *> (m_pDcmSeriesCompareDocTemplate->GetNextDoc(pos));
		if (pDoc) {
			
			if (pObj) {
				pDoc->AppendSeriesObject(pObj, bHeadPosition);
				pDoc->UpdateAllViews(NULL);
			}

			nSeriesObjCount = pDoc->GetSeriesObjectCount();

			pDcmSeriesCompareView = NULL;
			pos = pDoc->GetFirstViewPosition();
			while (pos != NULL && pDcmSeriesCompareView == NULL) {
				pView = pDoc->GetNextView(pos);
				if (pView->IsKindOf(RUNTIME_CLASS(CDcmSeriesCompareView))) 
					pDcmSeriesCompareView = dynamic_cast <CDcmSeriesCompareView *> (pView);
			}

			if (pDcmSeriesCompareView != NULL) {
				pDcmSeriesCompareView->GetParentFrame()->ShowWindow(SW_SHOW);
				if  (nSeriesObjCount >= 2) 	
					pDcmSeriesCompareView->GetParentFrame()->BringWindowToTop();
			}
		}
	}

	nSeriesCount = nSeriesObjCount;

	return;
}


void CCloudERVApp::ViewMedicalReport(const TCHAR *szSystem, const TCHAR *szOrderNo, const TCHAR *szInpatientNo, const TCHAR *szOutpatientNo, const TCHAR *szPatientID, const TCHAR *szDocID)
{
	CPublishedReportDoc *pDoc;
	CPublishedReportView *pReportView = NULL;
	CView *pView;
	POSITION pos;

	pos = m_pQueryPublishedReportDocTemplate->GetFirstDocPosition();

	if (pos == NULL) 
		m_pQueryPublishedReportDocTemplate->OpenDocumentFile(NULL);

	pos = m_pQueryPublishedReportDocTemplate->GetFirstDocPosition();
	if (pos != NULL) {
		pDoc = dynamic_cast <CPublishedReportDoc *> (m_pQueryPublishedReportDocTemplate->GetNextDoc(pos));
		if (pDoc) {
			pReportView = NULL;
			pos = pDoc->GetFirstViewPosition();

			while (pos != NULL && pReportView == NULL) {
				pView = pDoc->GetNextView(pos); 
				if (pView->IsKindOf(RUNTIME_CLASS(CPublishedReportView))) {
					pReportView = dynamic_cast<CPublishedReportView *> (pView);
				}
			}

			if (pReportView != NULL) {
				pReportView->DoQuery(szSystem, szOrderNo, szInpatientNo, szOutpatientNo, szPatientID, szDocID);
			}
		}
	}
}


DcmDataset * BuildDcmDataset(void *jpegData, int jpegLen, const TCHAR *sAccessionNo, const TCHAR *sDocId, const TCHAR *sName,const TCHAR *sSex, const TCHAR *sStudyDate, const TCHAR *sStudyUID, const TCHAR *sInstanceUID)
{
	CStringA sImgNo(""),sPhotometric("");
	DcmDataset *pds = NULL,*pDataset=NULL;
	int ni,nImageWidth = 0,nImageHeight = 0,nBytesPerPixel = 0,nScanLineBytes = 0,nColorMode=0;
    OFCondition status = EC_Normal;
	DcmFileFormat dcmff;
	DcmPixelSequence *pPixelSequence;
	DcmPixelItem *pOffsetTable;
	unsigned short nCompressedBits = 8;
	DcmOffsetList offsetList;	
	offile_off_t  nOffset;
	E_TransferSyntax xferSyntax = EXS_JPEGProcess14SV1TransferSyntax;
	char buf[4096];
	DcmOutputBufferStream output(buf,4095);
	DcmInputBufferStream input;
	void *pBuf;
	BOOL bLast = FALSE,bContinue = TRUE;
    DcmWriteCache wcache;


	//sImgNo = sImgId;
	//sImgNo = sImgNo.Right(3);

	//dcmGenerateUniqueIdentifier(newuid);	
	//sInstanceUID = newuid;
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	
	if (!ReadJpegParams(jpegData,jpegLen,nImageWidth,nImageHeight,nBytesPerPixel,nColorMode))
		return NULL;

	switch(nColorMode)
	{
	case 1:
		sPhotometric = "MONOCHROME2";
		break;
	case 2: 
		sPhotometric = "YBR_FULL";
		break;
	case 3:
		sPhotometric = "RGB";
		break;
	default:
		sPhotometric = "RGB";
		break;
	}

	pds = dcmff.getDataset();

	pds->putAndInsertUint16(DCM_Columns,nImageWidth);
	pds->putAndInsertUint16(DCM_Rows,nImageHeight);
	pds->putAndInsertUint16(DCM_SamplesPerPixel, nBytesPerPixel);
    pds->putAndInsertString(DCM_PhotometricInterpretation, sPhotometric);
	pds->putAndInsertSint32(DCM_NumberOfFrames,1);
    pds->putAndInsertUint16(DCM_BitsAllocated, 8);
	pds->putAndInsertUint16(DCM_BitsStored, 8);
    pds->putAndInsertUint16(DCM_HighBit, 7);
    pds->putAndInsertUint16(DCM_PixelRepresentation, 0);

	pds->putAndInsertString(DCM_ImageType, "DERIVED\\SECONDARY");
	pds->putAndInsertString(DCM_DerivationDescription, "ZhongbangPACS");
	pds->putAndInsertString(DCM_PatientOrientation," ");

	if (1==2) //monochromeMode) 
		delete pds->remove(DCM_PlanarConfiguration);
    else 
		pds->putAndInsertUint16(DCM_PlanarConfiguration, 0);

    
	pds->putAndInsertString(DCM_TransferSyntaxUID,UID_JPEGProcess1TransferSyntax);

 	pds->putAndInsertString(DCM_SOPClassUID,UID_SecondaryCaptureImageStorage);
	pds->putAndInsertString(DCM_SOPInstanceUID,CStringA(sInstanceUID));

	pds->putAndInsertString(DCM_PatientID,CStringA(sDocId));
	pds->putAndInsertString(DCM_Modality,"SC");
	pds->putAndInsertString(DCM_PatientName,CStringA(sName));
	pds->putAndInsertString(DCM_PatientSex,CStringA(sSex));
	pds->putAndInsertString(DCM_StudyInstanceUID, CStringA(sStudyUID));
	pds->putAndInsertString(DCM_SeriesInstanceUID,CStringA(sStudyUID));
	pds->putAndInsertString(DCM_StudyID," ");
	pds->putAndInsertString(DCM_AccessionNumber,CStringA(sAccessionNo));
	
	pds->putAndInsertString(DCM_InstanceNumber," ");//sImgId);

	pds->putAndInsertString(DCM_StudyDate,CStringA(sStudyDate));
	pds->putAndInsertString(DCM_SeriesDate,CStringA(sStudyDate));
	pds->putAndInsertString(DCM_AcquisitionDate,CStringA(sStudyDate));
	pds->putAndInsertString(DCM_InstitutionName," ");


    pPixelSequence = new DcmPixelSequence(DcmTag(DCM_PixelData,EVR_OB));

	// create empty offset table
    pOffsetTable = new DcmPixelItem(DcmTag(DCM_Item,EVR_OB));

    pPixelSequence->insert(pOffsetTable);

	pPixelSequence->storeCompressedFrame(offsetList,(unsigned char *) jpegData, jpegLen, 0);

	pOffsetTable->createOffsetTable(offsetList);

	pds->insert(pPixelSequence);
	
	
	pDataset = new DcmDataset();

	pDataset->clear();
	ni = 0;
	bContinue = TRUE;
	bLast = FALSE;

	pds->transferInit();
	pDataset->transferInit();

	while (bLast == FALSE && bContinue == TRUE)
	{
		status = pds->write(output,xferSyntax,EET_UndefinedLength, &wcache);

		
		if (status == EC_Normal)
		{
			bLast = TRUE;
		}
		else if (status == EC_StreamNotifyClient)
		{

		}
		else
		{
			bContinue = FALSE;
		}

		if (bContinue)
		{
			output.flushBuffer(pBuf,nOffset);

			if (nOffset > 0)
			{
				input.setBuffer(pBuf,nOffset);

				if (status == EC_Normal)
				{
					input.setEos();
				}
				pDataset->read(input, xferSyntax,EGL_withGL);
				input.releaseBuffer();
			}
		}

	}

	pds->transferEnd();
	pDataset->transferEnd();
	
	if (!bContinue) {
		if (pDataset != NULL) {
			delete pDataset;
			pDataset = NULL;
		}
	}

	return pDataset;
}


void CCloudERVApp::TextToClipboard(HWND hwnd, const char *szText)
{
	int nLen;

	if (szText == NULL)
		return;
	
	nLen = strlen(szText);
	if (nLen == 0)
		return;
 
	OpenClipboard(hwnd);
	EmptyClipboard();
	
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE, nLen + 1);
	if (!hg){
		CloseClipboard();
		return;
	}

	memcpy(GlobalLock(hg), szText, nLen + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
}

BOOL CCloudERVApp::GetLocalAET(CString &sLocalAET)
{
	char  szHostName[256];
	int nRectCode;
    struct hostent FAR *lpHostEnt;
	LPSTR lpAddr;
    struct in_addr inAddr;
	BOOL bReturn = FALSE;
	CString sIP1(""), sIP2(""), sIP3(""), sIP4("");

	memset(szHostName, 0, 256);
    nRectCode = gethostname(szHostName,sizeof(szHostName));
 
    if ( nRectCode ==0) {
       lpHostEnt = gethostbyname(szHostName);
       if (lpHostEnt != NULL) {
			lpAddr = lpHostEnt->h_addr_list[0];
			memcpy(&inAddr,lpHostEnt->h_addr_list[0],sizeof(in_addr)); 
			
			sIP1.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b1);
			sIP2.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b2);
			sIP3.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b3);
			sIP4.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b4);

			if (sLocalAET.IsEmpty() || sLocalAET.CompareNoCase(_T("Auto")) == 0) {
				sLocalAET.Format(_T("WS%03d%03d%03d"), inAddr.S_un.S_un_b.s_b2, inAddr.S_un.S_un_b.s_b3, inAddr.S_un.S_un_b.s_b4);
			}
			else {
				sLocalAET.Replace(_T("$IP1"), sIP1);
				sLocalAET.Replace(_T("$IP2"), sIP2);
				sLocalAET.Replace(_T("$IP3"), sIP3);
				sLocalAET.Replace(_T("$IP4"), sIP4);
			}

			bReturn = TRUE;
	   }
	}

	return bReturn;
}


BOOL CCloudERVApp::GetImageViewerAET(CString &sImageViewerAET)
{
	char  szHostName[256];
	int nRectCode;
    struct hostent FAR *lpHostEnt;
	LPSTR lpAddr;
    struct in_addr inAddr;
	BOOL bReturn = FALSE;
	CString sIP1(""), sIP2(""), sIP3(""), sIP4("");

	memset(szHostName, 0, 256);
    nRectCode = gethostname(szHostName,sizeof(szHostName));
 
    if ( nRectCode ==0) {
       lpHostEnt = gethostbyname(szHostName);
       if (lpHostEnt != NULL) {
			lpAddr = lpHostEnt->h_addr_list[0];
			memcpy(&inAddr,lpHostEnt->h_addr_list[0],sizeof(in_addr)); 
			
			sIP1.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b1);
			sIP2.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b2);
			sIP3.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b3);
			sIP4.Format(_T("%03d"), inAddr.S_un.S_un_b.s_b4);

			if (sImageViewerAET.IsEmpty() || sImageViewerAET.CompareNoCase(_T("Auto")) == 0) {
				sImageViewerAET.Format(_T("WS%03d%03d"), inAddr.S_un.S_un_b.s_b3, inAddr.S_un.S_un_b.s_b4);
			}
			else {
				sImageViewerAET.Replace(_T("$IP1"), sIP1);
				sImageViewerAET.Replace(_T("$IP2"), sIP2);
				sImageViewerAET.Replace(_T("$IP3"), sIP3);
				sImageViewerAET.Replace(_T("$IP4"), sIP4);
			}

			bReturn = TRUE;
	   }
	}

	return bReturn;
}


BOOL CCloudERVApp::IsPulishedReportViewOpened()
{
	return (m_pQueryPublishedReportDocTemplate->GetFirstDocPosition() != NULL);
}


void CCloudERVApp::LocateExamReport(const TCHAR *szStudyGUID)
{
	CPublishedReportView *pView;
	CPublishedReportDoc *pDoc;
	POSITION pos1, pos2;

	pos1 = m_pQueryPublishedReportDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = dynamic_cast <CPublishedReportDoc *> (m_pQueryPublishedReportDocTemplate->GetNextDoc(pos1));
		if (pDoc) {
			pDoc->LocateExamReport(szStudyGUID);
			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = dynamic_cast <CPublishedReportView *> (pDoc->GetNextView(pos2));
				if (pView) {
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}


struct MemoryStruct {
	unsigned char *memory;
	size_t size;
};


static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
	mem->memory = (unsigned char *) realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		// out of memory!  
		return 0;
	}
 
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
 
	return realsize;
}


void CCloudERVApp::ViewOriginalReport(CMedicalResultObject *pObj)
{

	CString sTitle, sStudyGUID, sStudyGUIDx, sStudyUID, sReportFileUrls(""), sPDFPassword(""), sFileNames(""), sErrMsg("没有发现上传的报告的Url.");	
	std::string sUrls(""), sUrl(""), sPdfPswd("");
	CURL *curl_handle;
	CURLcode res;
	struct MemoryStruct chunk;
	bool bPdfReport = false, bBmpReport = false, bFirstCalled = true;
	std::vector <std::string> list;
	std::vector <std::string> ::iterator itStr;	
	int nFileIdx = 0;
	CWaitCursor wait;

	if (pObj == NULL) 
		return;

	sStudyGUID = pObj->m_sStudyGUID;
	sStudyUID = pObj->m_sStudyUID;

	if (pObj->m_bFromXIS) {
		if (pObj->m_bReportFileUrlsSearched) {
			if (pObj->m_sReportFileUrls.IsEmpty()) {
				::MessageBox(AfxGetMainWnd()->m_hWnd, sErrMsg, _T("错误信息"), MB_ICONEXCLAMATION | MB_OK);
				return;
			}
		}
		else {
			SearchReportFileUrls(sStudyGUID, sStudyUID, sReportFileUrls, sPDFPassword);
				
			pObj->m_bReportFileUrlsSearched = TRUE;
			pObj->m_sReportFileUrls = sReportFileUrls;
			pObj->m_sPDFPassword = sPDFPassword;

			if (pObj->m_sReportFileUrls.IsEmpty()) {
				::MessageBox(AfxGetMainWnd()->m_hWnd, sErrMsg, _T("错误信息"), MB_ICONEXCLAMATION | MB_OK);
				return;
			}
		}
	}
	else {
		if (pObj->m_sReportFileUrls.IsEmpty()) {
			::MessageBox(AfxGetMainWnd()->m_hWnd, sErrMsg, _T("错误信息"), MB_ICONEXCLAMATION | MB_OK);
			return;
		}
	}	
	
	sReportFileUrls = pObj->m_sReportFileUrls;
	sFileNames = sReportFileUrls;
	sFileNames.MakeLower();

	sPdfPswd = CStringA(pObj->m_sPDFPassword);
	sTitle.Format(_T("原始报告_%s"), pObj->m_sPatientName);

	// 目前只能处理jpg和pdf文件， bmp 文件暂不支持

	if (sFileNames.Find(_T(".jpg")) > 0 || sFileNames.Find(_T(".jpeg")) > 0) {
		bBmpReport = true;
		bPdfReport = false;
	}
	else if (sFileNames.Find(_T(".pdf")) > 0) {
		bPdfReport = true;
		bBmpReport = false;
	}
	else 
		return;
	

	if (bPdfReport && IsPdfReportOpened(sStudyGUID)) {
		BringPdfReportViewToFront(sStudyGUID);
		return;
	}


	if (bBmpReport && IsBmpReportOpened(sStudyGUID)) {
		BringBmpReportViewToFront(sStudyGUID);
		return;
	}


	sUrls = CStringA(sReportFileUrls);

	list.clear();
	TokenizeStr(sUrls.c_str(), std::string("\r\n;"), true, list);
	bFirstCalled = true;
	nFileIdx = 0;

	for (itStr = list.begin(), nFileIdx = 0; itStr != list.end(); itStr ++, nFileIdx ++) {
		sUrl = (*itStr);

		if (bFirstCalled)
			sStudyGUIDx = sStudyGUID;
		else
			sStudyGUIDx.Format(_T("%s_%d"), sStudyGUID, nFileIdx);

		chunk.memory = (unsigned char *) malloc(1);
		chunk.size = 0;    // no data at this point  
 
		curl_global_init(CURL_GLOBAL_ALL);
 
		// init the curl session  
		curl_handle = curl_easy_init();
 
		// specify URL to get  
		curl_easy_setopt(curl_handle, CURLOPT_URL, sUrl.c_str()); 

		// 连接响应的时间 5s
		curl_easy_setopt(curl_handle, CURLOPT_FTP_RESPONSE_TIMEOUT, 5L); 
	
		// 下载时间限制为 30s
		curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L); 

		// send all data to this function   
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
		/// we pass our 'chunk' struct to the callback function 
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
 
		// some servers don't like requests that are made without a user-agent
		// field, so we provide one  
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
		// get it!  
		res = curl_easy_perform(curl_handle); 
		// cleanup curl stuff  
		curl_easy_cleanup(curl_handle);
		curl_global_cleanup();

		if (res != CURLE_OK) {
			if( chunk.memory) {
				free(chunk.memory);
				chunk.memory = NULL;
			}
		}

		if (chunk.memory != NULL) {
		
			if (bPdfReport) {
				if (chunk.size > 128) {
					ViewPDFReport(sStudyGUIDx, sTitle, chunk.memory, chunk.size, sPdfPswd.c_str(), FALSE);
				}
			}
			else {
				if (chunk.size > 1024) {
					ViewBmpReport(sStudyGUID, sTitle, NULL, chunk.memory, chunk.size, bFirstCalled);

					if (bFirstCalled) 
						bFirstCalled = false;
				}
			}

			free(chunk.memory);
			chunk.memory = NULL;
		}
	}


}



BOOL CCloudERVApp::SearchReportFileUrls(const TCHAR *szStudyGUID, const TCHAR *szStudyUID, CString &sReportFileUrls, CString &sPDFPassword)
{
	BOOL bRet = FALSE;
	CSystemConfiguration *pConfig;
	SAConnection con;
	SACommand cmd;
	SAString strCmdText(""), sWhereExp;
	CString sFileUrls(""), sPswd("");
	std::string strAes(""), strBase64(""), strDecrypted("");

	bRet = FALSE;

	pConfig = GetSystemConfiguration();

	if ((szStudyGUID != NULL && wcslen(szStudyGUID) > 0) && (szStudyUID != NULL &&  wcslen(szStudyUID) > 0)) {
		sWhereExp.Format(_T("StudyGUID = '%s' OR StudyUID = '%s' "),  szStudyGUID,  szStudyUID);
	}
	else if (szStudyGUID != NULL && wcslen(szStudyGUID) > 0) {
		sWhereExp.Format(_T("StudyGUID = '%s' "),  szStudyGUID);
	}
	else if (szStudyUID != NULL &&  wcslen(szStudyUID) > 0) {
		sWhereExp.Format(_T("StudyUID = '%s' "),  szStudyUID);
	}
	else 
	{
		sReportFileUrls = sFileUrls;
		sPDFPassword = sPswd;

		return FALSE;
	}

	strCmdText.Format(_T("SELECT ReportFileUrls, PDFPassword FROM %s WHERE %s "), 
					pConfig->GetMedicalResultTableName(),
					sWhereExp);

	try {
	
		con.Connect(pConfig->GetCloudERVDBName(), pConfig->GetCloudERVDBUserId(), pConfig->GetCloudERVDBPassword(), pConfig->GetCloudERVDBType());
		cmd.setConnection(&con);
		cmd.setCommandText(strCmdText);

		cmd.Execute();

		if (cmd.FetchNext()) {
			
			sFileUrls = cmd.Field("ReportFileUrls").asString();
			sPswd = cmd.Field("PDFPassword").asString();

			con.Commit();
			bRet = TRUE;
		}
	}
	catch(SAException &e) {

		try {
			con.Rollback();
		}
		catch(...) {
		}

	}
	
	if (bRet) {

		if (!sPswd.IsEmpty()) {

			strBase64 = CStringA(sPswd);
			strDecrypted = strBase64;
			try {
				strAes = ::base64_decode(strBase64);
				strDecrypted = aes_decrypt(strAes, "");
			}
			catch(...) {
			
			}

			sPswd = strDecrypted.c_str();
		}
	}

	sReportFileUrls = sFileUrls;
	sPDFPassword = sPswd;

	return bRet;
}

// C-Move 提取影像资料 
// 必须提供szStudyGUID
void CCloudERVApp::LoadImagesOfStudy(bool bLegacy, int nTagOfIVM, const TCHAR *szStudyGUID, const TCHAR *szStudyUID, const TCHAR *szAccession, const TCHAR *szDocId, const TCHAR *szStudyDate)
{
	if ((szStudyGUID == NULL || _tcslen(szStudyGUID) == 0) || ((szStudyUID == NULL || _tcslen(szStudyUID) == 0) && (szAccession == NULL || _tcslen(szAccession) == 0) 
			&& (szDocId == NULL || _tcslen(szDocId) == 0) && (szStudyDate == NULL || _tcslen(szStudyDate) == 0))) {

		return;
	}

	OpenStudyViaC_Move(szStudyGUID, szStudyUID, _T(""), szAccession, szDocId, szStudyDate, bLegacy);
}



// 简易浏览器
void CCloudERVApp::ViewImageMode1(int nTag, const TCHAR *szStudyGUID, const TCHAR *szSystemCode, const TCHAR *szAccessionNo, const TCHAR *szStudyUID, const TCHAR* szPatientName, const TCHAR *szDocId, const TCHAR *szSex, const TCHAR *szStudyDate, bool bLegacy)
{
	int nIndex = -1;
	CString sTitle("");
	CString sStudyUIDx;
	SAConnection con;
	SACommand cmd;
	SAString strDBName(""), strDBUserId(""), strDBPassword(""), strCmdText("");
	SAClient_t clientType;
	CObList *pSeriesObjList;
	CSeriesObject *pSeriesObject;
	CDicomObject *pDicomObject;
	DcmDataset *pDcmDataset = NULL;
	SAString sInstanceUID, sImageData;
	void *pData;
	long nImageLen = 0;
	CDcmImageDoc *pDoc, *pDocExisted = NULL;
	CView *pView;
	POSITION pos1, pos2;

	// 首先从已打开的文档中查找，如果已经存在，则转到相关的文档所对应的View

	if (_tcsicmp(szSystemCode, _T("UIS")) == 0 || _tcsicmp(szSystemCode, _T("EIS")) == 0 || _tcsicmp(szSystemCode, _T("PIS")) == 0 ) {
		if (szStudyUID && _tcslen(szStudyUID) > 0)
			sStudyUIDx = szStudyUID;
		else
			sStudyUIDx = CString(szSystemCode) + CString(szAccessionNo);
	}
	else 
		sStudyUIDx = szStudyUID;


	pDocExisted = NULL;
	pos1 = m_pDcmImageViewDocTemplate->GetFirstDocPosition();
	while (pos1 != NULL && pDocExisted == NULL) {
		pDoc = dynamic_cast<CDcmImageDoc *> (m_pDcmImageViewDocTemplate->GetNextDoc(pos1));
		if (pDoc) {
			if (pDoc->IsStudyUIDExisted(sStudyUIDx))
				pDocExisted = pDoc;
		}
	}

	if (pDocExisted) {
		pos2 = pDocExisted->GetFirstViewPosition();
		if (pos2) {
			pView = pDocExisted->GetNextView(pos2);
			if (pView) {
				pView->GetParentFrame()->BringWindowToTop();
				return;
			}
		}
	}

	// 没有找到， 从服务器下载图像

	if (_tcsicmp(szSystemCode, _T("UIS")) == 0 || _tcsicmp(szSystemCode, _T("EIS")) == 0 || _tcsicmp(szSystemCode, _T("PIS")) == 0 ) {
		// 超声、内镜、病理图像直接从UIS、EIS、PIS的数据库中提取

		strDBName = m_SystemConfiguration.GetXISDBName(szSystemCode);
		strDBUserId = m_SystemConfiguration.GetXISDBUserId(szSystemCode);
		strDBPassword = m_SystemConfiguration.GetXISDBPassword(szSystemCode);
		clientType = m_SystemConfiguration.GetXISDBType(szSystemCode);

		if (_tcsicmp(szSystemCode, _T("UIS")) == 0)
			strCmdText.Format(_T("SELECT TOP 100 ImgId as ImageId, Image as ImageData FROM %s WHERE ID = '%s' "), 
				m_SystemConfiguration.GetXISImageTableName(szSystemCode),
				szAccessionNo);
		else if (_tcsicmp(szSystemCode, _T("EIS")) == 0) {
			if (m_SystemConfiguration.XISDBTypeIsOracle(szSystemCode)) {
				strCmdText.Format(_T("SELECT * FROM \
									 (SELECT ImgId as ImageId, Image as ImageData FROM %s WHERE ID = '%s' ) \
									  WHERE RowNum <= 100"), 
					m_SystemConfiguration.GetXISImageTableName(szSystemCode),
					szAccessionNo);
			}
			else {
				strCmdText.Format(_T("SELECT TOP 100 ImgId as ImageId, Image as ImageData FROM %s WHERE ID = '%s' "), 
					m_SystemConfiguration.GetXISImageTableName(szSystemCode),
					szAccessionNo);
			}
		}
		else if (_tcsicmp(szSystemCode, _T("PIS")) == 0)
			strCmdText.Format(_T("SELECT TOP 100 ( MedicalNo + cast( Serialno as varchar(3))) as ImageId, ImgDataset as ImageData From %s WHERE medicalno = '%s' "),
				m_SystemConfiguration.GetXISImageTableName(szSystemCode),
				szAccessionNo);

		pSeriesObject = new CSeriesObject();
		pSeriesObject->m_bFromRIS = TRUE;
		pSeriesObject->m_bFromExtFile = FALSE;
		pSeriesObject->m_sPatientId = szDocId;
		pSeriesObject->m_sPatientName = szPatientName;
		pSeriesObject->m_sPatientName_CN = szPatientName;

		if (szStudyUID && _tcslen(szStudyUID) > 0)
			pSeriesObject->m_sSeriesUID = szStudyUID;
		else
			pSeriesObject->m_sSeriesUID = CString(szSystemCode) + CString(szAccessionNo);

		pSeriesObject->m_sStudyUID = pSeriesObject->m_sSeriesUID;
	


		try {
			con.Connect(strDBName, strDBUserId, strDBPassword, clientType);
			cmd.setConnection(&con);
			cmd.setCommandText(strCmdText);

			cmd.Execute();

			while (cmd.FetchNext()) {

				sImageData = cmd.Field("ImageData").asBLob();
				nImageLen = sImageData.GetBinaryLength();

				sInstanceUID = SAString(szSystemCode) + cmd.Field("ImageId").asString();
	
				pData = sImageData.GetBinaryBuffer(0);

				try {
					pDcmDataset = BuildDcmDataset(pData, nImageLen, szAccessionNo, szDocId, szPatientName, szSex, szStudyDate, szStudyUID, sInstanceUID);
				}
				catch(...)
				{
				}

				sImageData.ReleaseBuffer();

				if (pDcmDataset) {
					pDicomObject = new CDicomObject(pDcmDataset, TRUE);
					pSeriesObject->m_DicomObjectList.AddTail(pDicomObject);
				}
			}

			con.Commit();
		}
		catch(SAException &e) {
			try {
				con.Rollback();
			}
			catch(...) {
			}

			MessageBox(NULL, e.ErrText(), _T("错误信息"), MB_OK | MB_ICONEXCLAMATION);
		}

		if (pSeriesObject->GetDicomObjectCount() == 0) {
			delete pSeriesObject;
			pSeriesObject = NULL;
		}
		else {
			pSeriesObjList = new CObList();
			pSeriesObjList->AddTail(pSeriesObject);

			ViewDcmImages(szStudyGUID, pSeriesObjList, FALSE, TRUE);
		}
	}
	else if ((_tcsicmp(szSystemCode, _T("RIS")) == 0)||(_tcsicmp(szSystemCode, _T("PET")) == 0)) {
		// 放射\PET-CT子系统，　放射图像要从DICOM 服务器提取
		// 但如果配置了ImageViewer, 则ImageViewer优先

		if (IsImageViewerAvailable() == TRUE)
			OpenStudyViaImageViewer(nTag, szStudyUID, szAccessionNo, szDocId, szStudyDate, (m_bAutoClosePriorWorkspace == TRUE));
		else {
			switch(nTag) {
			case 1:		// StudyUID
				OpenStudyViaC_Move(szStudyGUID, szStudyUID, szPatientName, _T(""), _T(""), _T(""), bLegacy);
				break;
			case 2:		// AccessionNo
				OpenStudyViaC_Move(szStudyGUID, szStudyUID, szPatientName, szAccessionNo, _T(""), _T(""), bLegacy);
				break;
			case 3:		// DocID + StudyDate
				OpenStudyViaC_Move(szStudyGUID, szStudyUID, szPatientName, _T(""), szDocId, szStudyDate, bLegacy);
				break;
			case 4:		// DocID
				OpenStudyViaC_Move(szStudyGUID, szStudyUID, szPatientName, _T(""), szDocId, _T(""), bLegacy);
				break;
			}
		}
	}
}

// Web浏览器 
void CCloudERVApp::ViewImageMode2(const TCHAR *szStudyUID)
{
	CWebBrowserDoc *pDoc;
	CView *pView;
	POSITION pos1, pos2;

	pos1 = m_pWebViewerDocTemplate->GetFirstDocPosition();
	if (pos1 == NULL) {
		m_pWebViewerDocTemplate->OpenDocumentFile(NULL);
	}

	pos1 = m_pWebViewerDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = (CWebBrowserDoc *) m_pWebViewerDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			pDoc->SetDocParams(g_sWebViewerUrl, g_sWebViewerUserId, g_sWebViewerPassword, g_sWebViewerInitParams, szStudyUID);
			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = pDoc->GetNextView(pos2);
				if (pView) {
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}

// 专业浏览器
void CCloudERVApp::ViewImageMode3(int nTag, const TCHAR *szStudyUID, const TCHAR *szAccessionNo, const TCHAR *szDocId, const TCHAR *szStudyDate)
{
	OpenStudyViaImageViewer(nTag, szStudyUID, szAccessionNo, szDocId, szStudyDate, (m_bAutoClosePriorWorkspace == TRUE));
}


void CCloudERVApp::ViewPDFReport(const TCHAR *szReportGUID, const TCHAR *szTitle, unsigned char *pBuffer, long nBufferSize, const char *szPdfPassword, BOOL bExternalFile)
{
	CPdfReportDoc *pDoc;
	CView *pView;
	POSITION pos;
	BOOL bNewDoc = FALSE, bFound = FALSE;

	pos = m_pPdfReportViewDocTemplate->GetFirstDocPosition();
	if (pos == NULL) {
		m_pPdfReportViewDocTemplate->OpenDocumentFile(NULL);
		bNewDoc = TRUE;
	}
	else {
		bFound = FALSE;
		while (pos != NULL && !bFound) {
			pDoc = (CPdfReportDoc *) m_pPdfReportViewDocTemplate->GetNextDoc(pos);
			if (pDoc) {
				if (_tcsicmp(pDoc->GetReportPdfGuid(), szReportGUID) == 0) {
					pos = pDoc->GetFirstViewPosition();
					if (pos) { 
						pView = pDoc->GetNextView(pos);
						pView->GetParentFrame()->ActivateFrame();
					}
					bFound = TRUE;
					bNewDoc = FALSE;
				}
			}
		}
		
		if (!bFound) {
			m_pPdfReportViewDocTemplate->OpenDocumentFile(NULL);
			bNewDoc = TRUE;
		}
	}

	if (bNewDoc) {
		pos = m_pPdfReportViewDocTemplate->GetFirstDocPosition();
		if (pos) {
			bFound = FALSE;

			while (pos != NULL && !bFound) {
				pDoc = (CPdfReportDoc *) m_pPdfReportViewDocTemplate->GetNextDoc(pos);
				if (pDoc) {

					if (!pDoc->IsInitialized()) {

						pDoc->SetDocParams(pBuffer, nBufferSize, szTitle, szReportGUID, szPdfPassword, bExternalFile);
						bFound = TRUE;
					}
				}
			}
		}
	}
}


void CCloudERVApp::ViewBmpReport(const TCHAR *szStudyGUID, const TCHAR *szTitle, const TCHAR  *szBmpFileName, unsigned char *lpData, long nDataSize, bool bClearFirst)
{
	CBitmapViewerDoc *pDoc;
	CView *pView;
	POSITION pos1, pos2;
	int nDocCount = 0;

	nDocCount = 0;
	pos1 = m_pBmpReportViewDocTemplate->GetFirstDocPosition();
	while (pos1 != NULL) {
		nDocCount ++;
		m_pBmpReportViewDocTemplate->GetNextDoc(pos1);
	}

	if (nDocCount >= m_nMaxImageViewerWindowsNum) { 
		pos1 = m_pBmpReportViewDocTemplate->GetFirstDocPosition();
		if (pos1) {
			pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->GetNextDoc(pos1);
			if (pDoc) {
				pos2 = pDoc->GetFirstViewPosition();
				if (pos2) {
					pView = pDoc->GetNextView(pos2);
					if (pView) {
						pView->GetParentFrame()->DestroyWindow();
					}
				}
			}
		}	
	}

	pos1 = m_pBmpReportViewDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			if (_tcsicmp(pDoc->GetStudyGUID(), szStudyGUID) != 0) 
				pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->OpenDocumentFile(NULL);
		}
		else {
			pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->OpenDocumentFile(NULL);
		}
	}
	else
		pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->OpenDocumentFile(NULL);
	
	if (pDoc) {
		if (bClearFirst) {
			pDoc->ClearImages();
		}

		if (szBmpFileName != NULL && _tcslen(szBmpFileName) > 0) {
			pDoc->AppendBmpFromFile(szBmpFileName, true);
		}
		else if (nDataSize > 1024 && lpData[0] == 0xFF && lpData[1] == 0xD8) {
			pDoc->AppendJpgFromBuffer(lpData, nDataSize, true);
		}
	
		pDoc->SetDocInfo(szStudyGUID, szTitle);

		pos2 = pDoc->GetFirstViewPosition();
		if (pos2) {
			pView = pDoc->GetNextView(pos2);
			if (pView) {
				pView->GetParentFrame()->Invalidate();
				pView->GetParentFrame()->BringWindowToTop();
			}
		}
	}
}


void CCloudERVApp::ViewDcmImages(const TCHAR *szStudyGUID, CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear)
{
	CDcmImageDoc *pDoc;
	CView *pView;
	POSITION pos1, pos2;
	int nDocCount = 0;

	if (pSeriesObjList == NULL) 
		return;

	if (pSeriesObjList->GetSize() == 0)
		return;

	// 计算DcmImageViewerDocTemplate打开的文档数量

	nDocCount = 0;
	pos1 = m_pDcmImageViewDocTemplate->GetFirstDocPosition();
	while (pos1 != NULL) {
		nDocCount ++;
		m_pDcmImageViewDocTemplate->GetNextDoc(pos1);
	}

	// 如果打开的文档数大于设定的值，则关闭第一个文档
	if (nDocCount >= m_nMaxImageViewerWindowsNum) {

		pos1 = m_pDcmImageViewDocTemplate->GetFirstDocPosition();
		if (pos1) {

			pDoc = (CDcmImageDoc *) m_pDcmImageViewDocTemplate->GetNextDoc(pos1);
			if (pDoc) {
				pos2 = pDoc->GetFirstViewPosition();
				if (pos2) {
					pView = pDoc->GetNextView(pos2);
					if (pView) {
						pView->GetParentFrame()->DestroyWindow();
					}
				}
			}
		}
	}

	pDoc = (CDcmImageDoc *) m_pDcmImageViewDocTemplate->OpenDocumentFile(NULL);
	if (pDoc) {
		pDoc->SetStudyGUID(szStudyGUID);
		pDoc->AppendSeriesObjectList(pSeriesObjList, bSeriesMode, bClear);
		pos2 = pDoc->GetFirstViewPosition();
		if (pos2) {
			pView = pDoc->GetNextView(pos2);
			if (pView) {
				pView->GetParentFrame()->Invalidate();
				pView->GetParentFrame()->BringWindowToTop();
			}
		}
	}
}

void CCloudERVApp::ViewExtDcmImages(CObList *pSeriesObjList, BOOL bSeriesMode, BOOL bClear)
{
	CDcmImageDoc *pDoc;
	CView *pView;
	POSITION pos1, pos2;

	if (pSeriesObjList == NULL) 
		return;

	if (pSeriesObjList->GetSize() == 0)
		return;

	pos1 = m_pExtDcmImageViewDocTemplate->GetFirstDocPosition();
	if (pos1 == NULL) {
		m_pExtDcmImageViewDocTemplate->OpenDocumentFile(NULL);
	}

	pos1 = m_pExtDcmImageViewDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = (CDcmImageDoc *) m_pExtDcmImageViewDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			pDoc->AppendSeriesObjectList(pSeriesObjList, bSeriesMode, bClear);
			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = pDoc->GetNextView(pos2);
				if (pView) {
					pView->GetParentFrame()->Invalidate();
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}


void CCloudERVApp::ViewExtBmpImages(const TCHAR *szBmpFileName, unsigned char *lpJpgData, long nJpgDataSize, bool bClearFirst)
{
	CBitmapViewerDoc *pDoc;
	CView *pView;
	POSITION pos1, pos2;

	pos1 = m_pExtBmpImageViewDocTemplate->GetFirstDocPosition();
	if (pos1 == NULL) {
		m_pExtBmpImageViewDocTemplate->OpenDocumentFile(NULL);
	}

	pos1 = m_pExtBmpImageViewDocTemplate->GetFirstDocPosition();
	if (pos1) {

		pDoc = (CBitmapViewerDoc *) m_pExtBmpImageViewDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			if (bClearFirst) {
				pDoc->ClearImages();
			}
			
			pDoc->SetTitle(_T("外部图像文件"));
			if (szBmpFileName != NULL && _tcslen(szBmpFileName) > 0) {
				pDoc->AppendBmpFromFile(szBmpFileName, true);
			}
			else if (nJpgDataSize > 1024 && lpJpgData[0] == 0xFF && lpJpgData[1] == 0xD8) {
				pDoc->AppendJpgFromBuffer(lpJpgData, nJpgDataSize, true);
			}

			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = pDoc->GetNextView(pos2);
				if (pView) {
					pView->GetParentFrame()->Invalidate();
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}


BOOL CCloudERVApp::IsPdfReportOpened(const TCHAR *szReportGUID)
{
	CPdfReportDoc *pDoc;
	POSITION pos;
	BOOL bFound = FALSE;

	pos = m_pPdfReportViewDocTemplate->GetFirstDocPosition();
	if (pos != NULL) {
		while (pos != NULL && !bFound) {
			pDoc = (CPdfReportDoc *) m_pPdfReportViewDocTemplate->GetNextDoc(pos);
			if (pDoc) {
				if (_tcsicmp(pDoc->GetReportPdfGuid(), szReportGUID) == 0) {
					bFound = TRUE;
				}
			}
		}
	}

	return bFound;
}


void CCloudERVApp::BringPdfReportViewToFront(const TCHAR *szReportGUID)
{
	CPdfReportDoc *pDoc;
	CView *pView;
	POSITION pos;
	BOOL bFound = FALSE;

	pos = m_pPdfReportViewDocTemplate->GetFirstDocPosition();
	if (pos != NULL) {
		bFound = FALSE;
		while (pos != NULL && !bFound) {
			pDoc = (CPdfReportDoc *) m_pPdfReportViewDocTemplate->GetNextDoc(pos);
			if (pDoc) {
				if (_tcsicmp(pDoc->GetReportPdfGuid(), szReportGUID) == 0) {
					bFound = TRUE;
					pos = pDoc->GetFirstViewPosition();
					if (pos) { 
						pView = pDoc->GetNextView(pos);
						pView->GetParentFrame()->BringWindowToTop();
					}
				}
			}
		}
	}
}



BOOL CCloudERVApp::IsBmpReportOpened(const TCHAR *szReportGUID)
{
	CBitmapViewerDoc *pDoc;
	POSITION pos;
	BOOL bFound = FALSE;

	pos = m_pBmpReportViewDocTemplate->GetFirstDocPosition();
	if (pos != NULL) {
		while (pos != NULL && !bFound) {
			pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->GetNextDoc(pos);
			if (pDoc) {
				if (_tcsicmp(pDoc->GetStudyGUID(), szReportGUID) == 0) {
					bFound = TRUE;
				}
			}
		}
	}

	return bFound;
}


void CCloudERVApp::BringBmpReportViewToFront(const TCHAR *szReportGUID)
{
	CBitmapViewerDoc *pDoc;
	CView *pView;
	POSITION pos;
	BOOL bFound = FALSE;

	pos = m_pBmpReportViewDocTemplate->GetFirstDocPosition();
	if (pos != NULL) {
		bFound = FALSE;
		while (pos != NULL && !bFound) {
			pDoc = (CBitmapViewerDoc *) m_pBmpReportViewDocTemplate->GetNextDoc(pos);
			if (pDoc) {
				if (_tcsicmp(pDoc->GetStudyGUID(), szReportGUID) == 0) {
					bFound = TRUE;
					pos = pDoc->GetFirstViewPosition();
					if (pos) { 
						pView = pDoc->GetNextView(pos);
						pView->GetParentFrame()->BringWindowToTop();
					}
				}
			}
		}
	}
}


BOOL CCloudERVApp::IsHISProgramIsRunning()
{
	DWORD dwProcessId = 0;
	CString sFileName;
	int nPos, nLen;

	if (m_sHISProgramFileName.IsEmpty())
		return FALSE;
	nLen = m_sHISProgramFileName.GetLength();
	nPos = m_sHISProgramFileName.ReverseFind(_T('\\'));
	if (nPos < 0)
		nPos = m_sHISProgramFileName.ReverseFind(_T('/'));

	if (nPos > 0)
		sFileName = m_sHISProgramFileName.Mid(nPos + 1, nLen - nPos);
	else
		sFileName = m_sHISProgramFileName;

	dwProcessId = FindProcessId(sFileName);

	return (dwProcessId > 0);
}


void CCloudERVApp::ReturnToHISDesktop()
{
	DWORD dwProcessId = 0;
	CString sFileName;
	int nPos, nLen;
	HWND hWnd = NULL;

	if (m_sHISProgramFileName.IsEmpty())
		return ;

	nLen = m_sHISProgramFileName.GetLength();
	nPos = m_sHISProgramFileName.ReverseFind(_T('\\'));
	if (nPos < 0)
		nPos = m_sHISProgramFileName.ReverseFind(_T('/'));

	if (nPos > 0)
		sFileName = m_sHISProgramFileName.Mid(nPos + 1, nLen - nPos);
	else
		sFileName = m_sHISProgramFileName;

	dwProcessId = FindProcessId(sFileName);

	if (dwProcessId == 0)
		return;

	hWnd = find_main_window(dwProcessId);

	if (hWnd != NULL) {
		if (::IsIconic(hWnd)) 
			::ShowWindow(hWnd, SW_RESTORE);

		::BringWindowToTop(hWnd);
	}
}






void CCloudERVApp::BookingOfRIS()
{
	CDocument *pDoc;
	CView *pView;
	POSITION pos1, pos2;

	pos1 = m_pBookingOfRISDocTemplate->GetFirstDocPosition();
	if (pos1 == NULL) {
		m_pBookingOfRISDocTemplate->OpenDocumentFile(NULL);
	}

	pos1 = m_pBookingOfRISDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = (CDcmImageDoc *) m_pBookingOfRISDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = pDoc->GetNextView(pos2);
				if (pView) {
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}

void CCloudERVApp::BookingOfUIS()
{
	CDocument *pDoc;
	CView *pView;
	POSITION pos1, pos2;

	pos1 = m_pBookingOfUISDocTemplate->GetFirstDocPosition();
	if (pos1 == NULL) {
		m_pBookingOfUISDocTemplate->OpenDocumentFile(NULL);
	}

	pos1 = m_pBookingOfUISDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = m_pBookingOfUISDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = pDoc->GetNextView(pos2);
				if (pView) {
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}


void CCloudERVApp::BookingOfEIS()
{
	CDocument *pDoc;
	CView *pView;
	POSITION pos1, pos2;

	pos1 = m_pBookingOfEISDocTemplate->GetFirstDocPosition();
	if (pos1 == NULL) {
		m_pBookingOfEISDocTemplate->OpenDocumentFile(NULL);
	}

	pos1 = m_pBookingOfEISDocTemplate->GetFirstDocPosition();
	if (pos1) {
		pDoc = m_pBookingOfEISDocTemplate->GetNextDoc(pos1);
		if (pDoc) {
			pos2 = pDoc->GetFirstViewPosition();
			if (pos2) {
				pView = pDoc->GetNextView(pos2);
				if (pView) {
					pView->GetParentFrame()->BringWindowToTop();
				}
			}
		}
	}
}


