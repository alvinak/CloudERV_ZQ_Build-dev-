#pragma once

#include "SQLAPI.h"

#define MAX_SERVERINFO_NUM 40

class CSystemConfiguration
{
public:
	typedef struct
	{
		TCHAR SystemId[21];
		TCHAR SystemName[61];
		TCHAR HospitalID[21];
		TCHAR DBType[21];
		TCHAR DBHost[41];
		TCHAR DBName[41];
		TCHAR DBSchema[41];
		TCHAR DBUserId[41];
		TCHAR DBPassword[41];
		TCHAR DBOther[61];
	} ServerInfoStruct;

public:

	CSystemConfiguration(void);
	~CSystemConfiguration(void);

	BOOL Initialize(const TCHAR *szHost, const TCHAR *szHospitalID, int nPort, const TCHAR *szValidSystems);
	
	int  GetValidServerInfoCount() { return m_nServerInfoCount;}
	
	const TCHAR *GetSystemName(const TCHAR *szSystemId);
	const TCHAR *GetMedicalResultTableName();
	const TCHAR *GetRISQueueGroupsTableName();
	const TCHAR *GetRISStudiesTableName();

	const TCHAR *GetCloudERVDBName();
	const TCHAR *GetCloudERVDBUserId();
	const TCHAR *GetCloudERVDBPassword();
	SAClient_t GetCloudERVDBType();

	const TCHAR *GetXISImageTableName(const TCHAR *szSystemCode);
	const TCHAR *GetXISDBName(const TCHAR *szSystemCode);
	const TCHAR *GetXISDBUserId(const TCHAR *szSystemCode);
	const TCHAR *GetXISDBPassword(const TCHAR *szSystemCode);

	SAClient_t GetXISDBType(const TCHAR *szSystemCode);

	BOOL	XISDBTypeIsOracle(const TCHAR *szSystemCode) { return (GetXISDBType(szSystemCode) == SA_Oracle_Client); }

	void AddSystemInfo_HardCode(const TCHAR *szSystemId, const TCHAR *szSystemName,const TCHAR *szHospitalID ,const TCHAR *szDBHost,const TCHAR *szDBName,const TCHAR *szDBType,const TCHAR *szDBSchema,const TCHAR *szDBUserId,const TCHAR *szDBPassword,const TCHAR *szDBOther);


private:
	ServerInfoStruct m_ServerInfo[MAX_SERVERINFO_NUM];
	CString m_sLicServerHost, m_sHospitalID;
	int m_nLicServerPort;
	int m_nServerInfoCount;
	
	// ¡Ÿ ±±‰¡ø
	CString m_sDBName, m_sDBNameX, m_sTableNameX;
	//

	BOOL GetServerInfoText(const char *szHost, const char *szHospitalID, int nPort, CString &sServerInfoText);

	int GetSystemInfoIndex(const TCHAR *szSystemCode, const TCHAR *szHospitalID);
};

