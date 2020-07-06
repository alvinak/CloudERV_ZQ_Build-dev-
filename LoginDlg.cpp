// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CloudERV.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "SQLAPI.h"


// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

	CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_txtUserId(_T(""))
	, m_txtPassword(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UserID, m_txtUserId);
	DDX_Text(pDX, IDC_EDIT_Password, m_txtPassword);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CLoginDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CLoginDlg::OnBnClickedBtnCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLoginDlg message handlers


void CLoginDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	BOOL bSucceed = FALSE; 
	int m_nFailedTimes;

	CString sUserId,sPassword;

	//sUserId=GetWindowText(m_txtUserId);
	//GetDlgItemText(IDC_EDIT_UserID)->GetWindowText(sUserId); 
	sUserId = m_txtUserId;
	sUserId = sUserId.Trim ();
	sUserId = sUserId.MakeUpper ();
	 
	//sPassword = GetWindowText(m_txtPassword);
	sPassword = m_txtPassword;
	sPassword = sPassword.Trim ();
	sPassword = sPassword.MakeUpper ();

	if(m_txtUserId.IsEmpty () )
	{
		MessageBox(TEXT("请输入用户名"));
		return ;
	}
	
	if( m_txtPassword.IsEmpty ())
	{
		MessageBox(TEXT("请输入密码"));
		return ;
	}

	//if ((m_txtUserId =="developer" && m_txtPassword == "HelloCloudERV!")
	//	|| (m_txtUserId =="System" && m_txtPassword == "HelloCloudERV!")) 
	//{			
	//		//关闭当前对话框
	//		CDialog::OnCancel ();	
	//		return ;
	//}

	/*　char a[100];
	 　　CString str("aaaaaa");
	   　　strncpy(a,(LPCTSTR)str,sizeof(a));*/

	//const TCHAR *szUserId = sUserId;

	CString szSystem("CloudERV");
	bSucceed = DoLogin(szSystem,sUserId, sPassword);

	if (!bSucceed) 	
	{		
		MessageBox(TEXT("用户名或密码不正确"));
		return ;
	}	
	else
	{	//关闭当前对话框
		CDialog::OnCancel ();	
	}
	
		
}


void CLoginDlg::OnBnClickedBtnCancel()
{
	// TODO: Add your control notification handler code here
	exit(0);
}


void CLoginDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnOK();
}


void CLoginDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	//CDialogEx::OnClose();

	//退出程序
	exit(0);
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	//默认登录信息
	CString sUserId,sPassword;
	sUserId = "System";
	sPassword = "HelloCloudERV";

	m_txtUserId = sUserId;
	m_txtPassword = sPassword;

	//同步到控件中
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
bool CLoginDlg::DoLogin(const TCHAR *szSystem, const TCHAR *szUserID, const TCHAR *szPassword)
{
	BOOL bInteractived = FALSE;
	CCloudERVApp *pApp = (CCloudERVApp *) AfxGetApp();
	CSystemConfiguration *pConfig = pApp->GetSystemConfiguration();

	SAConnection con;
	SACommand cmd;
	SAString strCmdText("");
	CString sUserId(""), sPassword(""),sErrorMessage(""),sErrMsg("");
	CString sCurrentUser(""),sUserGUID(""),sEmpNo(""),sDeptName(""),sRoleGUIDs("");
	CString sMsg,sText(""), sTableName, sWhere(""), sCommandText("");
	CString sPasswordMD5("");
	int nValid,nFailedTimes ;
	unsigned long nPermissions;
	std::string md5str;
	//CString sUserId,sPassword; 

	md5str = md5::MD5String(CStringA(szPassword));
	sPasswordMD5 = md5str.c_str();

	//CString str=_T("Hello,world");  
	//char szStr[256] = {};  
	//wcstombs(szStr, str, str.GetLength());//将宽字符转换成多字符  
	//const char* pBuf = szStr;  	

	sCommandText.Format(_T("SELECT * FROM  %s WHERE UserId = '%s' AND Password ='%s' "), 
		pConfig->GetUsersTableName(),
		(char *)(LPCTSTR)szUserID,	
		sPasswordMD5);
	
	try {
		if (con.isConnected())
			con.Disconnect();

			/*con.Connect(pConfig->GetCloudERVDBName(), pConfig->GetCloudERVDBUserId(), pConfig->GetCloudERVDBPassword(), pConfig->GetCloudERVDBType());*/

		con.Connect("CloudERV","sa", "masterkey", SA_SQLServer_Client);

		cmd.setConnection(&con);
		cmd.setCommandText(SAString(sCommandText));
		cmd.Execute();

		if(cmd.isResultSet() == FALSE)
		{
			sErrorMessage ="用户名或密码错误。";
			return FALSE;
		}

		while (cmd.FetchNext()) {
			nValid = cmd.Field("Valid").asShort();

			if (nValid == 0) {
					nFailedTimes = 0;
					sErrorMessage = "此用户没有启用或被管理员置为无效用户，请和系统管理员联系。";
					return FALSE;
				}
			else {	
				sCurrentUser = cmd.Field("UserId").asString();
				sUserGUID = cmd.Field("UserGUID").asString ();
				sEmpNo = cmd.Field("EmpNo").asString();
				sDeptName = cmd.Field("DeptName").asString();
				sRoleGUIDs = cmd.Field("RoleGUIDs").asString();

				nPermissions = 0;		

				//if (LoadPermissionData(*sRoleGUIDs, nPermissions, sErrMsg)) {
				//	//user.SetPermission(Permissions);
			    return TRUE;			
				}	
			}
		con.Commit();
	}
catch (SAException &e) {

	try {
		con.Rollback();
	}
	catch(...)
	{
	}

	::MessageBox(GetParentFrame()->m_hWnd, e.ErrText(), _T("错误信息"), MB_OK | MB_ICONEXCLAMATION); 
	   }
	
}
//
//bool CLoginDlg::LoadPermissionData( CString *szRoleGUIDs, unsigned long  &nPermission, CString &sErrMsg)
//{
//	std::vector <std::wstring> list;
//	std::vector <std::wstring> ::iterator itStr;
//	UnicodeString sRoleGUIDsList(""), sCmdText;
//	unsigned long nPrivilege = 0, nPrivileges = 0;
//	//TFDQuery *pQuery = NULL;
//	bool bRet = false;
//
//	//list.clear();
//	//TokenizeStrW(szRoleGUIDs, std::wstring(L"\r\n;|"), true, list);
//
//	//sRoleGUIDsList = L"";
//	//for (itStr = list.begin(); itStr != list.end(); itStr ++) {
//	//	if (!sRoleGUIDsList.IsEmpty())
//	//		sRoleGUIDsList = sRoleGUIDsList + L",";
//
//	//	sRoleGUIDsList = sRoleGUIDsList + L"'" + UnicodeString((*itStr).c_str()) + L"'";
//	//}
//
//	//if (!sRoleGUIDsList.IsEmpty()) {
//	//	sCmdText.sprintf(L"SELECT PERMISSIONS FROM %s  WHERE (ROLEGUID IN (%s) AND VALID = 1) ",
//	//		GetTableName_UIS(TBL_ROLES).c_str(),
//	//		sRoleGUIDsList.c_str());
//
//	//}
//	//else {
//	//	nPermission = 0;
//	//	return true;
//	//}
//
//	//if (!Connect_UIS(sErrMsg))
//	//	return false;
//
//	//pQuery = new TFDQuery(NULL);
//	//pQuery->Connection = m_pConnection_UIS;
//	//pQuery->SQL->Text = sCmdText;
//
//	//try {
//	//	pQuery->Open() ;
//	//	pQuery->FetchAll();
//	//	pQuery->First();
//
//	//	while (!pQuery->Eof) {
//	//		nPrivilege = pQuery->FieldByName("PERMISSIONS")->AsLargeInt;
//	//		nPrivileges = (nPrivileges | nPrivilege);
//	//		pQuery->Next();
//	//	}
//
//	//	delete pQuery;
//	//	pQuery = NULL;
//	//	bRet = true;
//	//}
//	//catch(Exception &e) {
//	//	sErrMsg = e.Message;
//	//	if (pQuery)
//	//		delete pQuery;
//	//}
//
//	//nPermission = nPrivileges;
//
//	return bRet;
//}
////---------------