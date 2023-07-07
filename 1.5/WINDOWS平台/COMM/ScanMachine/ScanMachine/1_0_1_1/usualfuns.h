#ifndef _USUALFUNS_H
#define _USUALFUNS_H

//ȷ��ͷ�ļ�ֻ������һ��
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _DEBUG
	#pragma comment(lib,"UsualFunsD.lib")
#else
	#pragma comment(lib,"UsualFunsR.lib")
#endif

/*NTȨ��˵�� 
#define SE_CREATE_TOKEN_NAME					TEXT("SeCreateTokenPrivilege")
#define SE_ASSIGNPRIMARYTOKEN_NAME				TEXT("SeAssignPrimaryTokenPrivilege")
#define SE_LOCK_MEMORY_NAME						TEXT("SeLockMemoryPrivilege")
#define SE_INCREASE_QUOTA_NAME					TEXT("SeIncreaseQuotaPrivilege")
#define SE_UNSOLICITED_INPUT_NAME				TEXT("SeUnsolicitedInputPrivilege")
#define SE_MACHINE_ACCOUNT_NAME					TEXT("SeMachineAccountPrivilege")
#define SE_TCB_NAME								TEXT("SeTcbPrivilege")
#define SE_SECURITY_NAME						TEXT("SeSecurityPrivilege")
#define SE_TAKE_OWNERSHIP_NAME					TEXT("SeTakeOwnershipPrivilege")
#define SE_LOAD_DRIVER_NAME						TEXT("SeLoadDriverPrivilege")
#define SE_SYSTEM_PROFILE_NAME					TEXT("SeSystemProfilePrivilege")
#define SE_SYSTEMTIME_NAME						TEXT("SeSystemtimePrivilege")
#define SE_PROF_SINGLE_PROCESS_NAME				TEXT("SeProfileSingleProcessPrivilege")
#define SE_INC_BASE_PRIORITY_NAME				TEXT("SeIncreaseBasePriorityPrivilege")
#define SE_CREATE_PAGEFILE_NAME					TEXT("SeCreatePagefilePrivilege")
#define SE_CREATE_PERMANENT_NAME				TEXT("SeCreatePermanentPrivilege")
#define SE_BACKUP_NAME							TEXT("SeBackupPrivilege")
#define SE_RESTORE_NAME							TEXT("SeRestorePrivilege")
#define SE_SHUTDOWN_NAME						TEXT("SeShutdownPrivilege")
#define SE_DEBUG_NAME							TEXT("SeDebugPrivilege")
#define SE_AUDIT_NAME							TEXT("SeAuditPrivilege")
#define SE_SYSTEM_ENVIRONMENT_NAME				TEXT("SeSystemEnvironmentPrivilege")
#define SE_CHANGE_NOTIFY_NAME					TEXT("SeChangeNotifyPrivilege")
#define SE_REMOTE_SHUTDOWN_NAME					TEXT("SeRemoteShutdownPrivilege")
#define SE_INTERACTIVE_LOGON_NAME				TEXT("SeInteractiveLogonRight")				--	�����ص�¼
#define SE_NETWORK_LOGON_NAME					TEXT("SeNetworkLogonRight")					--	�����������ʴ˼����
#define SE_BATCH_LOGON_NAME						TEXT("SeBatchLogonRight")					--	��Ϊ���������õ�¼
#define SE_SERVICE_LOGON_NAME					TEXT("SeServiceLogonRight")					--	��Ϊ�����¼
#define SE_DENY_INTERACTIVE_LOGON_NAME			TEXT("SeDenyInteractiveLogonRight")			--	�ܾ����ص�¼
#define SE_DENY_NETWORK_LOGON_NAME				TEXT("SeDenyNetworkLogonRight")				--	�ܾ���������ʴ˼����
#define SE_DENY_BATCH_LOGON_NAME				TEXT("SeDenyBatchLogonRight")				--	�ܾ���Ϊ��������ҵ��¼
#define SE_DENY_SERVICE_LOGON_NAME				TEXT("SeDenyServiceLogonRight")				--	�ܾ���Ϊ�����¼ 
#define SE_REMOTE_INTERACTIVE_LOGON_NAME		TEXT("SeRemoteInteractiveLogonRight")		--	�����Զ�˵�¼
#define SE_DENY_REMOTE_INTERACTIVE_LOGON_NAME	TEXT("SeDenyRemoteInteractiveLogonRight")	--	�ܾ���Զ�˵�¼
*/

//��ʽ������Window������Ϣ
LPCTSTR DisplayWinError(DWORD dwErrorCode,int nType=0,BOOL bPrompt=FALSE);
//ϵͳ��ȫ�Բ����ຯ��
BOOL SetPrivilege(LPCTSTR sNewPrivilege);		//���ݸ�����ֵ������ϵͳ����Ȩ��	
//����Ӳ����������
DWORD GetCPUInfo(CString &strCPUInfo);			//��ȡ���ؼ������CPU��Ϣ
CString GetMACAddress();						//��ȡ���ؼ������MAC��ַ
DWORD GetLocalIPAddress(CString &strIPVal);		//��ȡ���ؼ������IP��ַ
DWORD GetLocalIPMark(CString &strIPMarkVal);	//��ȡ���ؼ����IP��ַ����������
DWORD GetMemoryUsePer();						//��ȡ�ڴ�ʹ�����(%)
//int GetSysMemoryStatus(IN OUT DWORD& dwTotalPhys,IN OUT DWORD& dwAvailPhys,IN OUT DWORD& dwTotalPageFile,IN OUT DWORD& dwAvailPageFile,IN OUT DWORD& dwTotalVirtual,IN OUT DWORD& dwAvailVirtual);  //��ȡϵͳ�ڴ����,��λΪM
int GetProcessMemoryStatus(IN HANDLE hProcess,IN OUT DWORD& dwCurrentUsePhys,IN OUT DWORD& dwPeakUsePhys,IN OUT DWORD& dwCurrentPageFile,IN OUT DWORD& dwPeakPageFile);	//��ȡָ�����̵��ڴ�ʹ��״̬

//����ע�����
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, DWORD &dwVal);							//��ָ���ļ����ע����ڶ�ȡһ��DWORDֵ	
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, CString &strVal);						//��ָ���ļ����ע����ڶ�ȡһ���ַ���(Unicode)
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, TCHAR* szVal,DWORD& dwSize);			//��ָ���ļ����ע����ڶ�ȡһ���ַ���(Unicode,�������з��������ַ�)
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, BYTE *lpzVal, DWORD &dwSize);			//��ָ���ļ����ע����ڶ�ȡһ��2�������ݽṹ

BOOL WriteKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, DWORD dwData);							//��ָ���ļ����ע�����д��һ��DWORDֵ		
BOOL WriteKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, LPCTSTR lpVal,int iValueType=0);		//��ָ���ļ����ע�����д��һ���ַ���(Unicode)	���ַ���ֵ(������Ϊ0)������ַ���ֵ(������Ϊ1)
BOOL WriteKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, BYTE *lpVal,DWORD dwSize);				//��ָ���ļ����ע�����д��һ��2�������ݽṹ

BOOL DeleteKey(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey);											//��ָ���ļ����ע����ڸ���ָ��·��ɾ��һ����
BOOL DeleteKeyValue(LPCTSTR lpHostName,HKEY hKeyRoot,LPCTSTR lpPath,LPCTSTR lpValue);										//��ָ���ļ����ע����ڸ����ض�����ɾ��һ����ֵ

BOOL GetEnumKeys(HKEY hKeyRoot, LPCTSTR lpsPath, CStringArray &strArray);													//ö��ָ������·�������еļ���
BOOL GetEnumValues(HKEY hKeyRoot, LPCTSTR pszPath, CStringArray &strArray);													//ö��ָ������·�������еļ�ֵ
BOOL GetRemoteEnumKeys(LPCTSTR lpsComputer, HKEY hKeyRoot, LPCTSTR lpsPath, CStringArray &strArray);						//ö��ָ�������ָ��·���µ����м���
BOOL GetRemoteEnumValues(LPCTSTR lpsComputer, HKEY hKeyRoot, LPCTSTR lpsPath, CStringArray &strArray);						//ö��ָ�������ָ��·���µ����м�ֵ

CString GetSysEnvVariable(IN LPCTSTR lpszName);																				//��ȡָ������������ֵ
BOOL SetSysEnvVariable(IN LPCTSTR lpsName,IN LPCTSTR lpsVal);																//����һ����������

UINT	GetRandNumber(IN const UINT& nMin=0,IN const UINT& nMax=999999);													//��ȡָ����Χ�������
//����ת������
UINT Hex2Uint(IN LPTSTR lpsHex,IN const BOOL& bOrder);
void Hex2Bin(IN LPTSTR pHex,OUT PBYTE pBin,IN DWORD dwHex_len);
CString Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len);
void Int2Byte(IN const int& iData,IN OUT PBYTE pData,BOOL bOrder=FALSE);
BYTE CountXor(IN const LPBYTE pBuff,IN const BYTE& byLen);
const CString Hex2BinString(IN const CString& strHexData);
//���ܽ��ܺ���
CString EncryptData(LPCSTR lpzData,LPCTSTR lpContainerKey=NULL);
BOOL EncryptData(LPSTR lpzData,DWORD dwStrLen,LPCTSTR lpContainerKey=NULL);
BOOL EncryptData(LPSTR lpzData,DWORD dwStrLen,DWORD dwPwd,LPCTSTR lpContainerKey=NULL);

CString DecryptData(LPSTR lpzData,LPCTSTR lpContainerKey=NULL);
BOOL DecryptData(LPSTR lpzData,DWORD dwStrLen,LPCTSTR lpContainerKey=NULL);
BOOL DecryptData(LPSTR lpzData,DWORD dwStrLen,DWORD dwPwd,LPCTSTR lpContainerKey=NULL);

DWORD EncryptVerifyCode(LPSTR lpzDate,DWORD dwStrLen,LPCTSTR lpContainerKey=NULL);
WORD  VerifyICCard(IN LPBYTE ptr,IN const WORD& count);

//������ƺ���
int		GetServiceState(LPCTSTR lpsHostName, LPCTSTR lpsService, CString &strStatus);
int		GetServiceInfor(LPCTSTR lpsHostName, LPCTSTR lpsService, CString &strInfo);
BOOL	GetServiceInfor(LPCTSTR lpsHostName, LPCTSTR lpsService, int& iStartMode,int& iRunState,int& iInteract,CString& strDisplayName,CString& strBindAccount,CString& strFilePath,CString& strDescribe,CString& strFileVersion,long& lFileModifyTime,DWORD& dwFileSize,CString &strError);
BOOL	RegisterServices(IN LPCTSTR lpsHostName,IN LPCTSTR lpsService,IN LPCTSTR lpsDisplyName,IN LPCTSTR lpsServicePath,IN LPCTSTR lpsAccount,IN LPCTSTR lpsPassword,IN const BOOL& bAlternat,IN const BOOL& bAutoStat,IN OUT CString &strError);
BOOL	UnRegisterServices(LPCTSTR lpsHostName, LPCTSTR lpsService,CString &strError);
BOOL	StartServices(LPCTSTR lpsHostName, LPCTSTR lpsService,CString &strError);
BOOL	StopService(LPCTSTR lpsHostName, LPCTSTR lpsService,CString &strError);
BOOL	EnumServices(LPCTSTR lpsHostName,int iServiceType,CStringArray& aryServicesInfo,CString &strError,int iGetType=0);

//�ļ���չ��������
BOOL CompareFileTime(LPCTSTR lpsSrcFile,LPCTSTR lpsDesFile);																		//�ļ�ʱ��ȽϺ���
BOOL ChangeFileTime(IN LPCTSTR lpsFilePath,IN const time_t& tTime);																	//�ı��ļ���ʱ��
BOOL ReleaseFileFromRes(LPCTSTR lpsType, LPCTSTR lpsResName, LPCTSTR lpsDesFile);													//��2��������������Դ�ͷų��ļ�
BOOL FileReadAddedMsg(IN LPCTSTR lpsFileName,IN LPBYTE lpMsg,IN const DWORD& dwSize);												//��Ŀ���ļ�ĩ��ȡָ����С�ĸ�������
BOOL FileWriteAddedMsg(IN LPCTSTR lpsFileName,IN LPBYTE lpMsg,IN const DWORD& dwSize);												//��Ŀ���ļ�ĩд�븽��2��������
BOOL ReadSafeFile(IN LPCTSTR lpsFileName,IN OUT LPBYTE lpData,IN const DWORD& dwSize);												//���ļ�ĩβ�����ȡָ���ֽڵ�����
BOOL WriteSafeFile(IN LPCTSTR lpsFileName,IN const LPBYTE lpData,IN const DWORD& dwSize);											//���ļ�ĩβ׷��ָ����С������
BOOL DeleteAllFileInDir(IN const CString& strDirName,IN const BOOL bIncludeDir=TRUE,IN const BOOL& bDelSelf=FALSE);					//ɾ��ָ��Ŀ¼�µ������ļ�(�����ļ���),Ĭ�ϲ�ɾ������Ŀ¼
BOOL GetFoldersInPath(IN LPCTSTR lpsPath,IN OUT CStringArray &strArray,IN const BOOL& bChanged=TRUE);								//��ȡָ��·���µ������ļ�
BOOL GetFilesInPath(IN LPCTSTR lpsPath,IN OUT CStringArray &strArray,IN const BOOL& bOnlyName=FALSE,IN const BOOL& bChanged=TRUE);	//��ȡָ��·���µ�����Ŀ¼
BOOL CheckFileIntact(IN LPCTSTR lpsFile,IN const DWORD& dwSize);																	//���Ŀ���ļ��Ƿ�Ϊ�̶��ṹ�洢�������ļ�
BOOL UniteFile(IN LPCTSTR lpsSrcFile1,IN LPCTSTR lpsSrcFile2,IN LPCTSTR lpsDesFile=NULL);											//�������ļ��ϲ���һ���ļ���Ĭ�Ͻ���һ���ļ�����ڶ����ļ���ĩβ

int DeleteSelfExe();																												//���ڳ������н�����ɾ������
CString GetSysDir();																												//��ȡϵͳĿ¼·��
CString GetVersionOfFile(IN LPCTSTR lpsPath);																						//��ȡ�ļ��汾��
DWORD	GetSizeOfFile(IN LPCTSTR lpsPath);																							//��ȡ�ļ���С(��Χ�ֽ���)
BOOL	GetTimeOfFile(IN LPCTSTR lpsPath,IN OUT long& lFileCreateTime,IN OUT long& lFileModifyTime,IN OUT long& lFileAccessTime);	//��ȡ�ļ��������޸ģ�����ʱ��(UNIX��ʽ)
CString GetFilePathName(IN LPCTSTR lpsPath);																						//��ȡָ���ļ����ľ���·��
CString GetFolderPathName(IN LPTSTR lpsRoot=NULL);																					//��ȡָ���ļ��еľ���·��
CString GetModulePathName(IN const DWORD& dwProcessID);																				//���ݸ�������ID��ȡ��ģ��ľ���·��
//�ַ���չ��������
CString	FmtStr(const TCHAR* lpsFmt,...);																							//�ַ�����ʽ������
LPCTSTR DelSubFromString(IN LPCTSTR lpsStr,IN LPCTSTR lpsSub,IN LPCTSTR lpsSeps=NULL);
LPCTSTR UnExpandEnvString(IN LPCTSTR lpsName,IN LPCTSTR lpsVal);
CString VerifyStr(IN LPTSTR lpsStr);																								//�����ַ����еķǷ��ַ� -- У�����ַ���(\'��\"�ÿո����,ȥ����β�İ�������ַ�)
BOOL StrInc(IN LPCTSTR lpsStr,IN LPCTSTR lpsSub,IN const BOOL& bNoCase=FALSE);														//���Ŀ���ַ������Ƿ�����������ִ�
CString TrimString(IN LPTSTR lpsStr);																								//ȥ�������ַ��������ҿո�
//Windows NT �ʻ���������
BOOL WNetLogon(LPCTSTR lpsHostName, LPCTSTR lpsUser, LPCTSTR lpsPassword,CString &strError);										//��¼ָ���������IPC ��ʽ��
void WNetLogoff(LPCTSTR lpsHostName);																								//ע����¼ָ�������
BOOL WNetGetAutoLogonStatus(CString &strAutoUser);																					//��ȡ�����Զ���¼�ʻ�
CString	WNetGetCurrentUser();																										//��ȡ���ص�ǰ��¼�ʻ�
BOOL WNetAddUser(LPCTSTR lpsComputer,LPCTSTR lpsUser,LPCTSTR lpsPassword,LPCTSTR lpsGroup,LPCTSTR lpsComment,CString &strError);	//��ָ�������������һ���ʻ�
BOOL WNetDelUser(LPCTSTR lpsComputer,LPCTSTR lpsUser,CString &strError);															//��ָ���������ɾ��һ���ʻ�
BOOL WNetModifyUserPwd(LPCTSTR lpsComputer,LPCTSTR lpsUser,LPCTSTR lpsOldPassword,LPCTSTR lpsNewPassword,CString &strErr);			//��ָ���ļ�������޸�ĳ���ʻ�������
BOOL WNetDelShare(LPCTSTR lpsHostName,LPCTSTR lpsShare,CString &strErr);															//��ָ���ļ������ɾ������

//���ú���
BOOL RegisterOCX(IN LPCTSTR lpsOcxFileName);																						//ע��OCX�ؼ�
BOOL CheckOCXRegister(LPWSTR lpsClsid,IN CString &strProgID);																		//���OCX�ؼ��Ƿ�ע��
DWORD	ConvertIPAddress(LPCTSTR lpszIP);																							//IP��ַת�����ַ���ת����DWORD��
LPCTSTR ConvertIPAddress(DWORD dwIP);																								//IP��ַת����DWORDת�����ַ�����
time_t	OleTimeToTime(DATE dTime);																									//OLEʱ��(DATE)ת����time_tʱ��
const CString GetTimeString(IN const SYSTEMTIME& sSysTime,IN const BOOL& bType=FALSE);												//���ݸ�����SYSTEMTIME�ṹת���ɱ�׼ʱ���ʽ
DWORD	GetServerTime(LPCTSTR lpServerName);																						//��ȡָ��������ʱ��
BOOL	SynchronizeTime(LPCTSTR lpServerName);																						//����ָ����������ʱ��ͬ�����ؼ����ʱ��
WORD	CRC16(LPBYTE lpData, WORD wLength);																							//CRCУ�������ɺ���
BOOL	Ping(LPCTSTR pszHostName=NULL,UCHAR nTTL = 10, DWORD dwTimeout = 500, UCHAR nPacketSize = 32);								//����Ŀ�������Ƿ����(����ECHO����)
CString GetHostNameByIPAddr(LPCTSTR lpzIPAddr=NULL);																				//���ݸ�����IP��ȡĿ������������
void SetComputerName(LPCTSTR lpComputerName,LPCTSTR lpTargetIP=NULL,LPCTSTR lpLoginName=NULL,LPCTSTR lpLoginPassword=NULL);			//�޸�ָ���ļ����������
BOOL ReStartComputerEx(LPCTSTR lpHostName,UINT uFlags);																				//�������������
BOOL SetAccountRights(LPCTSTR lpsAccountName,LPCTSTR lpsHostName,LPCTSTR lpsPrivilegeName,BOOL bEnable);							//�����û�����Ȩ��
BOOL MakeDirectory(CString strCreateDir);																							//�������Ŀ¼
void  HandOnMessage(HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);												//ʹ��Ϣ���´���
BOOL ChangeDisplayyMode(DWORD dwBitsPerPel,DWORD dwWidth,DWORD dwHeight);															//������ʾ����ʾģʽ
CString time_t2CString(time_t vtime);																								//����ֵ��ʱ���޸�Ϊ�ַ���ʱ��
time_t CString2time_t(LPCTSTR lpstime);																								//���ַ�����ʱ���޸�Ϊ��ֵ��ʱ��
BOOL CheckSQLExist(IN LPCTSTR lpzServerIP,IN OUT CString &strError);																//�����������Ƿ����SQL���ݿ�
const CString GetGuidString(IN const int& iType = 0);																				//��ȡGUID��
int GetMonthDayCount(int iYear,int iMonth);																							//��ȡָ�����µ���ӵ�е�����
void KeepUSKeyboardLayout();																										//�������뷨ΪӢ��
void DisablePowerManage();																											//��ֹϵͳ����
HWND GetWindowHandleByPID(IN const DWORD& dwProcessID);																				//���ݸ����Ľ���ID��ȡ�䴰�ھ��
const CString GetCurrentTimeString(IN const int& iTimeType);																		//��ȡ��ǰʱ��ֵ���ַ���(0,�������룻1�������룩
//IP�������
BOOL GetNetCardID(IN OUT CStringArray &aryNetCardID);																				//��ȡȫ����Ч���������
BOOL GetNetCardID(IN const int& iID,IN OUT LPTSTR lpzNetCardID);																	//��ȡָ��˳��Ŷ�Ӧ���������к�
BOOL GetNetCardIPs(IN LPCTSTR lpzNetCardID,IN OUT CStringArray &aryGetIP,IN OUT CStringArray &aryGetMask);							//��ȡָ�������ϵ�����IP
BOOL SetNetCardIPs(IN LPCTSTR lpzNetCardID,IN OUT CStringArray &arySetIP,IN OUT CStringArray &arySetMask);							//����ָ���������кŵ�����IP������������
//ϵͳδ����API��װ
BOOL NotifyIPChange(LPCTSTR lpAdapterName,int nIndex,LPCTSTR lpIPAddress,LPCTSTR lpNetMask);										//֪ͨϵͳIP���ģ��������������������IP���ĳɹ�������ԭ�ͼ�����
#endif//#ifndef _USUALFUNS_H