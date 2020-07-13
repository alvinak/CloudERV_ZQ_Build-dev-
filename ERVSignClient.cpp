#include "stdafx.h"
#include "ERVSignClient.h"

CERVSignClient::CERVSignClient()
{
	m_hERVSignDll = NULL;
	Initialize_Dll = NULL;
	Login_Dll = NULL;
	Logout_Dll = NULL;

	GetUserId_Dll = NULL;
	GetUserName_Dll = NULL;
	GetPassword_Dll = NULL;
	GetPermission_Dll = NULL;
}

CERVSignClient::~CERVSignClient()
{
	if (m_hERVSignDll) {
		FreeLibrary(m_hERVSignDll);
		m_hERVSignDll = NULL;
	}
}

void CERVSignClient::LoadClient(const char *szFileName)
{
	char szFullFileName[MAX_PATH + 1], *pch;


	if (m_hERVSignDll != NULL) return;

	memset(szFullFileName, 0, MAX_PATH + 1);
	GetModuleFileNameA(NULL, szFullFileName, MAX_PATH);

	pch = (char *) strrchr(szFullFileName, '\\');
	*(pch + 1) = '\0';

	if (szFileName == NULL || strlen(szFileName) == 0 || _stricmp(szFileName, "Auto") == 0 || _stricmp(szFileName, "$Auto") == 0) {
#ifdef _WIN64
		strcat_s(szFullFileName, MAX_PATH, "ERVSign_x64.dll");
#else 
		strcat_s(szFullFileName, MAX_PATH, "ERVSign_x86.dll");
#endif
	}
	else {
		strcpy_s(szFullFileName, MAX_PATH, szFileName);
	}

	m_hERVSignDll = LoadLibraryA(szFullFileName);

	if (m_hERVSignDll) {
		Initialize_Dll = (void ( *)(const char *))GetProcAddress(m_hERVSignDll, "Initialize");
		Login_Dll = (bool( *)(HWND)) GetProcAddress(m_hERVSignDll, "Login");
		Logout_Dll = (void( *)()) GetProcAddress(m_hERVSignDll, "Logout");

		GetUserId_Dll = (const char * ( *)(void))GetProcAddress(m_hERVSignDll, "GetLoginedUserId");
		GetUserName_Dll = (const char * ( *)(void))GetProcAddress(m_hERVSignDll, "GetLoginedUserName");
		GetPassword_Dll = (const char * ( *)(void))GetProcAddress(m_hERVSignDll, "GetLoginedPassword");
		GetPermission_Dll = (unsigned long ( *)(void))GetProcAddress(m_hERVSignDll, "GetPermission");
	}

}

void CERVSignClient::Initialize(const char *szParamXml)
{
	if (Initialize_Dll)
		Initialize_Dll(szParamXml);
}

bool CERVSignClient::Login(HWND hParentWnd)
{
	if (Login_Dll)
		return Login_Dll(hParentWnd);

	return false;
}

void CERVSignClient::Logout()
{
	if (Logout_Dll)
		Logout_Dll();

	return;
}


const char * CERVSignClient::GetUserId(void)
{
	if (GetUserId_Dll)
		return (GetUserId_Dll());
	return "";
}

const char * CERVSignClient::GetUserName(void)
{
	if (GetUserName_Dll)
		return (GetUserName_Dll());
	return "";
}

const char * CERVSignClient::GetPassword(void)
{
	if (GetPassword_Dll)
		return (GetPassword_Dll());
	return "";
}

unsigned long CERVSignClient::GetPermission(void)
{
	if (GetPermission_Dll)
		return (GetPermission_Dll());
	return 0L;
}

