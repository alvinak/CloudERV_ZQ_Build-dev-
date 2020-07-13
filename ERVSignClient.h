#pragma once
#include <Windows.h>

class CERVSignClient
{
public:
	CERVSignClient();
	~CERVSignClient();
public:
	bool Connected() { return m_hERVSignDll != NULL;  }
	void LoadClient(const char *szFileName);
	void Initialize(const char *szParamXml);
	bool Login(HWND hParentWnd);
	void Logout();

	const char * GetUserId(void);
	const char * GetUserName(void);
	const char * GetPassword(void);
	unsigned long GetPermission(void);

private:
	HMODULE m_hERVSignDll;
	void (*Initialize_Dll)(const char *szParamXml);
	bool ( *Login_Dll)(HWND hParentWnd);
	void ( *Logout_Dll)(void);

	const char * ( *GetUserId_Dll)(void);
	const char * ( *GetUserName_Dll)(void);
	const char * ( *GetPassword_Dll)(void);
	unsigned long ( *GetPermission_Dll)(void);

};