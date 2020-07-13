#include "StdAfx.h"
#include "SystemConfiguration.h"


CSystemConfiguration::CSystemConfiguration(void)
{
	m_sLicServerHost = "localhost";
	m_sHospitalID = "";
	m_nLicServerPort = 6200;

	for (int ni = 0; ni < MAX_SERVERINFO_NUM; ni ++) 
		memset(m_ServerInfo, 0, sizeof(ServerInfoStruct));

	m_nServerInfoCount = 0;
}

CSystemConfiguration::~CSystemConfiguration(void)
{

}


BOOL CSystemConfiguration::GetServerInfoText(const char *szLicHost, const char *szHospitalID, int nPort, CString &sServerInfoText)
{
	CStringA sLicHostIP,sMsg,sText("");
    struct sockaddr_in sa;
    struct hostent *host;
    int nTimeout = 30000;
    int size,len,n,nLen;
    char szHostName[40],buf[8192];
    SOCKET sock;
	BOOL bRet = FALSE;

	sServerInfoText = _T("");

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,(const char *) &nTimeout, sizeof(nTimeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(const char *) &nTimeout, sizeof(nTimeout));

	if (sock == -1)
        return FALSE;

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(nPort);

	if (strchr(szLicHost,'.') != NULL)
	{
        sLicHostIP = szLicHost;
    }
    else
    {
		if (_stricmp(szLicHost,"localhost") == 0)
		{
			nLen = gethostname(szHostName,40);
			szHostName[nLen] = '\0';
			host = gethostbyname(szHostName);
		}
		else
		{
			host = gethostbyname(szLicHost);
		}
		
		if (host != NULL)
		{
			sLicHostIP = inet_ntoa(*((in_addr *)host->h_addr));
		}
		else
		{
			sLicHostIP = "127.0.0.1";
		}
    }

    sa.sin_addr.s_addr = inet_addr(sLicHostIP);

    connect(sock, (sockaddr *)&sa, sizeof(sa));

	sMsg.Format("@GETSERVERLIST@");

    len = sMsg.GetLength();

    sText = "";

    size = send(sock, sMsg, len, 0);
	
    if (size == len)
    {
		memset(buf,0,8192);
        len = recv(sock, buf, 8192, 0);

        if (len >= 2)
        {
            for (n = 1; n < len ; n ++) buf[n] = buf[n] ^ buf[0];

            buf[0] = ' ';
		}

		sText = buf;

		sServerInfoText = sText;
	}

    closesocket(sock);

	return TRUE;
}


void CSystemConfiguration::AddSystemInfo_HardCode(const TCHAR *szSystemId,const TCHAR *szSystemName,const TCHAR *szHospitalID ,const TCHAR *szDBHost,const TCHAR *szDBName,const TCHAR *szDBType,const TCHAR *szDBSchema,const TCHAR *szDBUserId,const TCHAR *szDBPassword,const TCHAR *szDBOther)
{


	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].SystemId, 10, szSystemId);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].SystemName, 60, szSystemName);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].HospitalID, 10, szHospitalID);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBHost,40 , szDBHost);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBName, 40 ,szDBName);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBType, 20, szDBType);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBSchema, 20, szDBSchema);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBUserId, 20, szDBUserId);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBPassword,40, szDBPassword);
	_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBOther, 60, szDBOther);

	m_nServerInfoCount ++;
}



BOOL CSystemConfiguration::Initialize(const TCHAR *szHost, const TCHAR *szHospitalID, int nPort, const TCHAR *szValidSystems)
{
	CString sServerInfoText("");
	CString sKey, sValue, sTmp;
	CString sSystemId, sSystemName, sHospitalID, sDBHost, sDBType, sDBName, sDBSchema, sDBUserId, sDBPassword, sDBOther;
	CString sSystemIdx;
	int nKeyLen, nPos, nCount, ni;

	if (szHost && _tcslen(szHost) > 0)
		m_sLicServerHost = szHost;

	if (szHospitalID && _tcslen(szHost) > 0)
		m_sHospitalID = szHospitalID;

	if (nPort > 0)
		m_nLicServerPort = nPort;

	GetServerInfoText(CStringA(m_sLicServerHost), CStringA(m_sHospitalID), m_nLicServerPort, sServerInfoText);

	sKey = "SYSTEMCOUNT=";

	nKeyLen = sKey.GetLength();
	nPos = sServerInfoText.Find(sKey);

	sValue = "";

	if (nPos > 0) {
		sTmp = sServerInfoText.Mid(nPos);

		nPos = sTmp.Find(_T(";"));

		if (nPos > nKeyLen)
			sValue = sTmp.Mid(nKeyLen,nPos - nKeyLen);

		nCount = _ttoi(sValue);

		if (nCount > 10) nCount = 10;
	
		m_nServerInfoCount = 0;


		ni = 1;

		while (ni <= nCount) {

			sSystemId = _T("");
			sSystemName = _T("");
			sHospitalID = _T("");
			sDBHost = _T("");
			sDBType = _T("");
			sDBName = _T("");
			sDBSchema = _T("");
			sDBUserId = _T("");
			sDBPassword = _T("");
			sDBOther = _T("");

			sKey.Format(_T("SYSTEMID%d="), ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen) {
					sSystemId = sTmp.Mid(nKeyLen,nPos - nKeyLen);

					if (_tcslen(szValidSystems) > 0 ) {

						if (_tcsstr(szValidSystems, sSystemId) == NULL  && _tcsicmp(sSystemId, _T("CloudERV")) != 0) {

							sSystemIdx = sSystemId;
							sSystemIdx = sSystemIdx.MakeUpper();
							if (sSystemIdx.GetLength() > 5 && sSystemIdx.Left(5) == _T("CLOUD")) {
								sSystemIdx = sSystemIdx.Mid(5);

								if (_tcsstr(szValidSystems, sSystemIdx) == NULL && _tcsicmp(sSystemIdx, _T("ERV")) != 0) 
									sSystemId = _T("");
							}
							else 
								sSystemId = _T("");
						}
					}
				}
			}

			sKey.Format(_T("SYSTEMNAME%d="), ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sSystemName = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("HOSPITALID%d="), ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sHospitalID = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBTYPE%d="),ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBType = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBHOST%d="),ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBHost = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBNAME%d="), ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBName = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBSCHEMA%d="),ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBSchema = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBUSERID%d="), ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBUserId = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBPSWD%d="), ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBPassword = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			sKey.Format(_T("DBOTHER%d="),ni);
			nKeyLen = sKey.GetLength();
			nPos = sServerInfoText.Find(sKey);

			if (nPos > 0) {
				sTmp = sServerInfoText.Mid(nPos);

				nPos = sTmp.Find(_T(";"));

				if (nPos > nKeyLen)
					sDBOther = sTmp.Mid(nKeyLen,nPos - nKeyLen);
			}

			if (sDBHost != "" && sDBName != "" && sDBUserId != "" && sDBType != "") {

				if (sSystemId.CompareNoCase(_T("UIS")) == 0)
					sSystemId = "CloudUIS";
				else if (sSystemId.CompareNoCase(_T("EIS")) == 0)
					sSystemId = "CloudEIS";
				else if (sSystemId.CompareNoCase(_T("RIS")) == 0)
					sSystemId = "CloudRIS";
				else if (sSystemId.CompareNoCase(_T("PIS")) == 0)
					sSystemId = "CloudPIS";
				else if (sSystemId.CompareNoCase(_T("ERV")) == 0)
					sSystemId = "CloudERV";

				if (!sSystemId.IsEmpty()) {

					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].SystemId, 10, sSystemId);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].SystemName, 60, sSystemName);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].HospitalID, 10, sHospitalID);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBHost,40 , sDBHost);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBName, 40 ,sDBName);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBType, 20, sDBType);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBSchema, 20, sDBSchema);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBUserId, 20, sDBUserId);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBPassword,40, sDBPassword);
					_tcscpy_s(m_ServerInfo[m_nServerInfoCount].DBOther, 60, sDBOther);

					m_nServerInfoCount ++;
				}
			}

			ni ++;
		}
	}

	return (m_nServerInfoCount > 0);
}

const TCHAR * CSystemConfiguration::GetSystemName(const TCHAR *szSystemId)
{
	int ni, nIdx = -1;

	for (ni = 0; ni < m_nServerInfoCount && nIdx == -1; ni ++) {
		if (_tcsicmp(m_ServerInfo[ni].SystemId, szSystemId) == 0)
			nIdx = ni;
	}

	return (nIdx == -1 ? _T("") : m_ServerInfo[nIdx].SystemName);
}


int CSystemConfiguration::GetSystemInfoIndex(const TCHAR *szSystemCode, const TCHAR *szHospitalID)
{
	int ni,nIndex = -1;
	CString sSystemId("");

	if (_tcsicmp(szSystemCode, _T("UIS")) == 0)
		sSystemId = _T("CLOUDUIS");
	else if (_tcsicmp(szSystemCode, _T("EIS")) == 0)
		sSystemId = _T("CLOUDEIS");
	else if (_tcsicmp(szSystemCode, _T("RIS")) == 0)
		sSystemId = _T("CLOUDRIS");
	else if (_tcsicmp(szSystemCode, _T("PIS")) == 0)
		sSystemId = _T("CLOUDPIS");
	else if (_tcsicmp(szSystemCode, _T("ERV")) == 0)
		sSystemId = _T("CLOUDERV");
	else
		sSystemId = szSystemCode;


	for (ni = 0; ni < m_nServerInfoCount && nIndex == -1; ni ++) {


		if (szHospitalID == NULL || _tcslen(szHospitalID) == 0) {
/*
CString aa;
aa.Format(L"ni=%d, hospitalid1=%s, hospitaid2=%s,systemid1=%s,systemid2=%s",ni,szHospitalID,m_ServerInfo[ni].HospitalID,szSystemCode,  m_ServerInfo[ni].SystemId);
	::MessageBox(NULL,aa,L"MSGaa",MB_OK);
*/

			if (_tcsicmp(m_ServerInfo[ni].SystemId, sSystemId) == 0 && (_tcslen(m_ServerInfo[ni].HospitalID) == 0  || _tcsicmp(m_ServerInfo[ni].HospitalID, _T("000000")) == 0))
				nIndex = ni;
		}
		else {
			if (_tcsicmp(m_ServerInfo[ni].SystemId, sSystemId) == 0 && _tcsicmp(m_ServerInfo[ni].HospitalID, szHospitalID) == 0)
				nIndex = ni;
		}
	}

	return nIndex;
}


const TCHAR * CSystemConfiguration::GetMedicalResultTableName()
{
	int nIndex = -1;
	CString sDBSchema("");

	nIndex = GetSystemInfoIndex(_T("CloudERV"), _T(""));
	if (nIndex >= 0)
		sDBSchema = m_ServerInfo[nIndex].DBSchema;

	if (sDBSchema.IsEmpty())
		m_sTableNameX = _T("MedicalResults");
	else
		m_sTableNameX.Format(_T("%s.%s"), sDBSchema, _T("MedicalResults"));

	return m_sTableNameX;
}


const TCHAR * CSystemConfiguration::GetRISQueueGroupsTableName()
{
	int nIndex = -1;
	CString sDBSchema("");

	nIndex = GetSystemInfoIndex(_T("CloudRIS"), _T(""));
	if (nIndex >= 0)
		sDBSchema = m_ServerInfo[nIndex].DBSchema;

	if (sDBSchema.IsEmpty())
		m_sTableNameX = _T("QueueGroups");
	else
		m_sTableNameX.Format(_T("%s.%s"), sDBSchema, _T("QueueGroups"));

	return m_sTableNameX;
}


const TCHAR * CSystemConfiguration::GetRISStudiesTableName()
{
	int nIndex = -1;
	CString sDBSchema("");

	nIndex = GetSystemInfoIndex(_T("CloudRIS"), _T(""));
	if (nIndex >= 0)
		sDBSchema = m_ServerInfo[nIndex].DBSchema;

	if (sDBSchema.IsEmpty())
		m_sTableNameX = _T("Studies");
	else
		m_sTableNameX.Format(_T("%s.%s"), sDBSchema, _T("Studies"));

	return m_sTableNameX;
}


const TCHAR * CSystemConfiguration::GetCloudERVDBName()
{
	int nIndex = -1;
	CString sDBName(""), sDBHost(""), sDBType("");

	nIndex = GetSystemInfoIndex(_T("CloudERV"), _T(""));
	if (nIndex >= 0) {
		sDBName = m_ServerInfo[nIndex].DBName;
		sDBHost = m_ServerInfo[nIndex].DBHost;
		sDBType = m_ServerInfo[nIndex].DBType;

		if (sDBType.CompareNoCase(_T("Oracle")) == 0 || sDBType.CompareNoCase(_T("ORA")) == 0 || sDBType.CompareNoCase(_T("RAC")) == 0) {
			if (sDBHost.IsEmpty() || sDBHost.CompareNoCase(_T("TAF")) == 0 || sDBHost.CompareNoCase(_T("RAC")) == 0 )
				m_sDBNameX = sDBName;
			else 
				m_sDBNameX.Format(_T("//%s/%s"), sDBHost, sDBName);
		}
		else 
			m_sDBNameX.Format(_T("%s@%s"), sDBHost, sDBName);
	}

	return m_sDBNameX;
}

const TCHAR * CSystemConfiguration::GetCloudERVDBUserId()
{
	int nIndex = -1;
	nIndex = GetSystemInfoIndex(_T("CloudERV"), _T(""));
	if (nIndex >= 0)
		return (const TCHAR *) m_ServerInfo[nIndex].DBUserId;

	return _T("");
}

const TCHAR * CSystemConfiguration::GetCloudERVDBPassword()
{
	int nIndex = -1;
	nIndex = GetSystemInfoIndex(_T("CloudERV"), _T(""));
	if (nIndex >= 0)
		return (const TCHAR *)  m_ServerInfo[nIndex].DBPassword;

	return _T("");
}

SAClient_t CSystemConfiguration::GetCloudERVDBType()
{
	int nIndex = -1;
	CString sDBType("");
	SAClient_t clientType = SA_SQLServer_Client;

	nIndex = GetSystemInfoIndex(_T("CloudERV"), _T(""));
	if (nIndex >= 0)
		sDBType = m_ServerInfo[nIndex].DBType;

	if (sDBType.CompareNoCase(_T("Oracle")) == 0 || sDBType.CompareNoCase(_T("ORA")) == 0 || sDBType.CompareNoCase(_T("RAC")) == 0)
		clientType = SA_Oracle_Client;
	else if (sDBType.CompareNoCase(_T("SQLite")) == 0)
		clientType = SA_SQLite_Client;
	else if (sDBType.CompareNoCase(_T("MySQL")) == 0)
		clientType = SA_MySQL_Client;
	else if (sDBType.CompareNoCase(_T("DB2")) == 0 || sDBType.CompareNoCase(_T("IBMDB2")) == 0 || sDBType.CompareNoCase(_T("IBMUDB")) == 0)
		clientType = SA_DB2_Client;
	else if (sDBType.CompareNoCase(_T("Sybase")) == 0)
		clientType = SA_Sybase_Client;
	else
		clientType = SA_SQLServer_Client;

	return clientType;
}


const TCHAR * CSystemConfiguration::GetXISImageTableName(const TCHAR *szSystemCode)
{
	int nIndex = -1;
	CString sTableName(""), sDBSchema("");

	nIndex = GetSystemInfoIndex(szSystemCode, _T(""));
	if (nIndex >= 0)
		sDBSchema = m_ServerInfo[nIndex].DBSchema;

	if (_tcsicmp(szSystemCode, _T("UIS")) == 0 ) 
		sTableName = _T("Image");
	else if (_tcsicmp(szSystemCode, _T("EIS")) == 0)
		//ÐÂEIS
		//sTableName = _T("DocImages");
		sTableName = _T("Image");
	else if (_tcsicmp(szSystemCode, _T("RIS")) == 0)
		sTableName = _T("DocImages");
	else if (_tcsicmp(szSystemCode, _T("PIS")) == 0)
		sTableName = _T("cmipsmain_image");

	if (sDBSchema.IsEmpty())
		m_sTableNameX = sTableName;
	else
		m_sTableNameX.Format(_T("%s.%s"), sDBSchema, sTableName);

	return m_sTableNameX;
}


const TCHAR * CSystemConfiguration::GetXISDBName(const TCHAR *szSystemCode)
{
	int nIndex = -1;
	CString sDBName(""), sDBHost(""), sDBType("");

	nIndex = GetSystemInfoIndex(szSystemCode, _T(""));
	

	if (nIndex >= 0) {
		sDBName = m_ServerInfo[nIndex].DBName;
		sDBHost = m_ServerInfo[nIndex].DBHost;
		sDBType = m_ServerInfo[nIndex].DBType;


		if (sDBType.CompareNoCase(_T("Oracle")) == 0 || sDBType.CompareNoCase(_T("ORA")) == 0 || sDBType.CompareNoCase(_T("RAC")) == 0) {
			if (sDBHost.IsEmpty() || sDBHost.CompareNoCase(_T("TAF")) == 0 || sDBHost.CompareNoCase(_T("RAC")) == 0 )
				m_sDBNameX = sDBName;
			else 
				m_sDBNameX.Format(_T("//%s/%s"), sDBHost, sDBName);
		}
		else 
			m_sDBNameX.Format(_T("%s@%s"), sDBHost, sDBName);
	}

	return m_sDBNameX;
}


const TCHAR * CSystemConfiguration::GetXISDBUserId(const TCHAR *szSystemCode)
{	int nIndex = -1;

	nIndex = GetSystemInfoIndex(szSystemCode, _T(""));
	if (nIndex >= 0)
		return (const TCHAR *) m_ServerInfo[nIndex].DBUserId;

	return _T("");
}

const TCHAR * CSystemConfiguration::GetXISDBPassword(const TCHAR *szSystemCode)
{
	int nIndex = -1;

	nIndex = GetSystemInfoIndex(szSystemCode, _T(""));
	if (nIndex >= 0)
		return (const TCHAR  *) m_ServerInfo[nIndex].DBPassword;

	return _T("");
}


SAClient_t CSystemConfiguration::GetXISDBType(const TCHAR *szSystemCode)
{
	int nIndex = -1;
	CString sDBType("");
	SAClient_t clientType = SA_SQLServer_Client;

	nIndex = GetSystemInfoIndex(szSystemCode, _T(""));
	if (nIndex >= 0)
		sDBType = m_ServerInfo[nIndex].DBType;

	if (sDBType.CompareNoCase(_T("Oracle")) == 0 || sDBType.CompareNoCase(_T("ORA")) == 0 || sDBType.CompareNoCase(_T("RAC")) == 0)
		clientType = SA_Oracle_Client;
	else if (sDBType.CompareNoCase(_T("SQLite")) == 0)
		clientType = SA_SQLite_Client;
	else if (sDBType.CompareNoCase(_T("MySQL")) == 0)
		clientType = SA_MySQL_Client;
	else if (sDBType.CompareNoCase(_T("DB2")) == 0 || sDBType.CompareNoCase(_T("IBMDB2")) == 0 || sDBType.CompareNoCase(_T("IBMUDB")) == 0)
		clientType = SA_DB2_Client;
	else if (sDBType.CompareNoCase(_T("Sybase")) == 0)
		clientType = SA_Sybase_Client;
	else
		clientType = SA_SQLServer_Client;

	return clientType;
}