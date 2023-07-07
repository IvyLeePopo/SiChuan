#ifndef _USUALFUNS_H
#define _USUALFUNS_H

//确保头文件只被包含一次
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _DEBUG
	#pragma comment(lib,"UsualFunsD.lib")
#else
	#pragma comment(lib,"UsualFunsR.lib")
#endif

/*NT权限说明 
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
#define SE_INTERACTIVE_LOGON_NAME				TEXT("SeInteractiveLogonRight")				--	允许本地登录
#define SE_NETWORK_LOGON_NAME					TEXT("SeNetworkLogonRight")					--	允许从网络访问此计算机
#define SE_BATCH_LOGON_NAME						TEXT("SeBatchLogonRight")					--	作为批处理作用登录
#define SE_SERVICE_LOGON_NAME					TEXT("SeServiceLogonRight")					--	作为服务登录
#define SE_DENY_INTERACTIVE_LOGON_NAME			TEXT("SeDenyInteractiveLogonRight")			--	拒绝本地登录
#define SE_DENY_NETWORK_LOGON_NAME				TEXT("SeDenyNetworkLogonRight")				--	拒绝从网络访问此计算机
#define SE_DENY_BATCH_LOGON_NAME				TEXT("SeDenyBatchLogonRight")				--	拒绝作为批处理作业登录
#define SE_DENY_SERVICE_LOGON_NAME				TEXT("SeDenyServiceLogonRight")				--	拒绝作为服务登录 
#define SE_REMOTE_INTERACTIVE_LOGON_NAME		TEXT("SeRemoteInteractiveLogonRight")		--	允许从远端登录
#define SE_DENY_REMOTE_INTERACTIVE_LOGON_NAME	TEXT("SeDenyRemoteInteractiveLogonRight")	--	拒绝从远端登录
*/

//格式化所有Window错误信息
LPCTSTR DisplayWinError(DWORD dwErrorCode,int nType=0,BOOL bPrompt=FALSE);
//系统安全性操作类函数
BOOL SetPrivilege(LPCTSTR sNewPrivilege);		//根据给定的值，设置系统操作权限	
//访问硬件参数函数
DWORD GetCPUInfo(CString &strCPUInfo);			//获取本地计算机的CPU信息
CString GetMACAddress();						//获取本地计算机的MAC地址
DWORD GetLocalIPAddress(CString &strIPVal);		//获取本地计算机的IP地址
DWORD GetLocalIPMark(CString &strIPMarkVal);	//获取本地计算机IP地址的子网掩码
DWORD GetMemoryUsePer();						//获取内存使用情况(%)
//int GetSysMemoryStatus(IN OUT DWORD& dwTotalPhys,IN OUT DWORD& dwAvailPhys,IN OUT DWORD& dwTotalPageFile,IN OUT DWORD& dwAvailPageFile,IN OUT DWORD& dwTotalVirtual,IN OUT DWORD& dwAvailVirtual);  //获取系统内存情况,单位为M
int GetProcessMemoryStatus(IN HANDLE hProcess,IN OUT DWORD& dwCurrentUsePhys,IN OUT DWORD& dwPeakUsePhys,IN OUT DWORD& dwCurrentPageFile,IN OUT DWORD& dwPeakPageFile);	//获取指定进程的内存使用状态

//操作注册表函数
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, DWORD &dwVal);							//在指定的计算机注册表内读取一个DWORD值	
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, CString &strVal);						//在指定的计算机注册表内读取一个字符串(Unicode)
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, TCHAR* szVal,DWORD& dwSize);			//在指定的计算机注册表内读取一个字符串(Unicode,包含换行符等特殊字符)
BOOL ReadKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, BYTE *lpzVal, DWORD &dwSize);			//在指定的计算机注册表内读取一个2进制数据结构

BOOL WriteKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, DWORD dwData);							//在指定的计算机注册表内写入一个DWORD值		
BOOL WriteKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, LPCTSTR lpVal,int iValueType=0);		//在指定的计算机注册表内写入一个字符串(Unicode)	单字符串值(最后参数为0)或多行字符串值(最后参数为1)
BOOL WriteKeyValue(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey, BYTE *lpVal,DWORD dwSize);				//在指定的计算机注册表内写入一个2进制数据结构

BOOL DeleteKey(LPCTSTR lpHostName, HKEY hKeyRoot, LPCTSTR lpPath, LPCTSTR lpKey);											//在指定的计算机注册表内根据指定路径删除一个键
BOOL DeleteKeyValue(LPCTSTR lpHostName,HKEY hKeyRoot,LPCTSTR lpPath,LPCTSTR lpValue);										//在指定的计算机注册表内根据特定内容删除一个键值

BOOL GetEnumKeys(HKEY hKeyRoot, LPCTSTR lpsPath, CStringArray &strArray);													//枚举指定键名路径下所有的键名
BOOL GetEnumValues(HKEY hKeyRoot, LPCTSTR pszPath, CStringArray &strArray);													//枚举指定键名路径下所有的键值
BOOL GetRemoteEnumKeys(LPCTSTR lpsComputer, HKEY hKeyRoot, LPCTSTR lpsPath, CStringArray &strArray);						//枚举指定计算机指定路径下的所有键名
BOOL GetRemoteEnumValues(LPCTSTR lpsComputer, HKEY hKeyRoot, LPCTSTR lpsPath, CStringArray &strArray);						//枚举指定计算机指定路径下的所有键值

CString GetSysEnvVariable(IN LPCTSTR lpszName);																				//获取指定环境变量的值
BOOL SetSysEnvVariable(IN LPCTSTR lpsName,IN LPCTSTR lpsVal);																//设置一个环境变量

UINT	GetRandNumber(IN const UINT& nMin=0,IN const UINT& nMax=999999);													//获取指定范围的随机数
//数制转换函数
UINT Hex2Uint(IN LPTSTR lpsHex,IN const BOOL& bOrder);
void Hex2Bin(IN LPTSTR pHex,OUT PBYTE pBin,IN DWORD dwHex_len);
CString Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len);
void Int2Byte(IN const int& iData,IN OUT PBYTE pData,BOOL bOrder=FALSE);
BYTE CountXor(IN const LPBYTE pBuff,IN const BYTE& byLen);
const CString Hex2BinString(IN const CString& strHexData);
//加密解密函数
CString EncryptData(LPCSTR lpzData,LPCTSTR lpContainerKey=NULL);
BOOL EncryptData(LPSTR lpzData,DWORD dwStrLen,LPCTSTR lpContainerKey=NULL);
BOOL EncryptData(LPSTR lpzData,DWORD dwStrLen,DWORD dwPwd,LPCTSTR lpContainerKey=NULL);

CString DecryptData(LPSTR lpzData,LPCTSTR lpContainerKey=NULL);
BOOL DecryptData(LPSTR lpzData,DWORD dwStrLen,LPCTSTR lpContainerKey=NULL);
BOOL DecryptData(LPSTR lpzData,DWORD dwStrLen,DWORD dwPwd,LPCTSTR lpContainerKey=NULL);

DWORD EncryptVerifyCode(LPSTR lpzDate,DWORD dwStrLen,LPCTSTR lpContainerKey=NULL);
WORD  VerifyICCard(IN LPBYTE ptr,IN const WORD& count);

//服务控制函数
int		GetServiceState(LPCTSTR lpsHostName, LPCTSTR lpsService, CString &strStatus);
int		GetServiceInfor(LPCTSTR lpsHostName, LPCTSTR lpsService, CString &strInfo);
BOOL	GetServiceInfor(LPCTSTR lpsHostName, LPCTSTR lpsService, int& iStartMode,int& iRunState,int& iInteract,CString& strDisplayName,CString& strBindAccount,CString& strFilePath,CString& strDescribe,CString& strFileVersion,long& lFileModifyTime,DWORD& dwFileSize,CString &strError);
BOOL	RegisterServices(IN LPCTSTR lpsHostName,IN LPCTSTR lpsService,IN LPCTSTR lpsDisplyName,IN LPCTSTR lpsServicePath,IN LPCTSTR lpsAccount,IN LPCTSTR lpsPassword,IN const BOOL& bAlternat,IN const BOOL& bAutoStat,IN OUT CString &strError);
BOOL	UnRegisterServices(LPCTSTR lpsHostName, LPCTSTR lpsService,CString &strError);
BOOL	StartServices(LPCTSTR lpsHostName, LPCTSTR lpsService,CString &strError);
BOOL	StopService(LPCTSTR lpsHostName, LPCTSTR lpsService,CString &strError);
BOOL	EnumServices(LPCTSTR lpsHostName,int iServiceType,CStringArray& aryServicesInfo,CString &strError,int iGetType=0);

//文件扩展操作函数
BOOL CompareFileTime(LPCTSTR lpsSrcFile,LPCTSTR lpsDesFile);																		//文件时间比较函数
BOOL ChangeFileTime(IN LPCTSTR lpsFilePath,IN const time_t& tTime);																	//改变文件的时间
BOOL ReleaseFileFromRes(LPCTSTR lpsType, LPCTSTR lpsResName, LPCTSTR lpsDesFile);													//将2进制数据流从资源释放成文件
BOOL FileReadAddedMsg(IN LPCTSTR lpsFileName,IN LPBYTE lpMsg,IN const DWORD& dwSize);												//在目标文件末读取指定大小的附加数据
BOOL FileWriteAddedMsg(IN LPCTSTR lpsFileName,IN LPBYTE lpMsg,IN const DWORD& dwSize);												//在目标文件末写入附加2进制数据
BOOL ReadSafeFile(IN LPCTSTR lpsFileName,IN OUT LPBYTE lpData,IN const DWORD& dwSize);												//从文件末尾反向读取指定字节的数据
BOOL WriteSafeFile(IN LPCTSTR lpsFileName,IN const LPBYTE lpData,IN const DWORD& dwSize);											//从文件末尾追加指定大小的数据
BOOL DeleteAllFileInDir(IN const CString& strDirName,IN const BOOL bIncludeDir=TRUE,IN const BOOL& bDelSelf=FALSE);					//删除指定目录下的所有文件(包括文件夹),默认不删除本身目录
BOOL GetFoldersInPath(IN LPCTSTR lpsPath,IN OUT CStringArray &strArray,IN const BOOL& bChanged=TRUE);								//获取指定路径下的所有文件
BOOL GetFilesInPath(IN LPCTSTR lpsPath,IN OUT CStringArray &strArray,IN const BOOL& bOnlyName=FALSE,IN const BOOL& bChanged=TRUE);	//获取指定路径下的所有目录
BOOL CheckFileIntact(IN LPCTSTR lpsFile,IN const DWORD& dwSize);																	//检查目标文件是否为固定结构存储的完整文件
BOOL UniteFile(IN LPCTSTR lpsSrcFile1,IN LPCTSTR lpsSrcFile2,IN LPCTSTR lpsDesFile=NULL);											//将两个文件合并成一个文件，默认将第一个文件并入第二个文件的末尾

int DeleteSelfExe();																												//用于程序运行结束后删除自身
CString GetSysDir();																												//获取系统目录路径
CString GetVersionOfFile(IN LPCTSTR lpsPath);																						//获取文件版本号
DWORD	GetSizeOfFile(IN LPCTSTR lpsPath);																							//获取文件大小(范围字节数)
BOOL	GetTimeOfFile(IN LPCTSTR lpsPath,IN OUT long& lFileCreateTime,IN OUT long& lFileModifyTime,IN OUT long& lFileAccessTime);	//获取文件创建，修改，访问时间(UNIX格式)
CString GetFilePathName(IN LPCTSTR lpsPath);																						//获取指定文件名的绝对路径
CString GetFolderPathName(IN LPTSTR lpsRoot=NULL);																					//获取指定文件夹的绝对路径
CString GetModulePathName(IN const DWORD& dwProcessID);																				//根据给定进程ID获取其模块的绝对路径
//字符扩展操作函数
CString	FmtStr(const TCHAR* lpsFmt,...);																							//字符串格式化函数
LPCTSTR DelSubFromString(IN LPCTSTR lpsStr,IN LPCTSTR lpsSub,IN LPCTSTR lpsSeps=NULL);
LPCTSTR UnExpandEnvString(IN LPCTSTR lpsName,IN LPCTSTR lpsVal);
CString VerifyStr(IN LPTSTR lpsStr);																								//发现字符串中的非法字符 -- 校验后的字符串(\'或\"用空格替代,去除结尾的半个汉字字符)
BOOL StrInc(IN LPCTSTR lpsStr,IN LPCTSTR lpsSub,IN const BOOL& bNoCase=FALSE);														//检查目标字符串中是否包含给定的字串
CString TrimString(IN LPTSTR lpsStr);																								//去掉给定字符串的左右空格
//Windows NT 帐户操作函数
BOOL WNetLogon(LPCTSTR lpsHostName, LPCTSTR lpsUser, LPCTSTR lpsPassword,CString &strError);										//登录指定计算机（IPC 方式）
void WNetLogoff(LPCTSTR lpsHostName);																								//注销登录指定计算机
BOOL WNetGetAutoLogonStatus(CString &strAutoUser);																					//获取本地自动登录帐户
CString	WNetGetCurrentUser();																										//获取本地当前登录帐户
BOOL WNetAddUser(LPCTSTR lpsComputer,LPCTSTR lpsUser,LPCTSTR lpsPassword,LPCTSTR lpsGroup,LPCTSTR lpsComment,CString &strError);	//在指定计算机上增加一个帐户
BOOL WNetDelUser(LPCTSTR lpsComputer,LPCTSTR lpsUser,CString &strError);															//在指定计算机上删除一个帐户
BOOL WNetModifyUserPwd(LPCTSTR lpsComputer,LPCTSTR lpsUser,LPCTSTR lpsOldPassword,LPCTSTR lpsNewPassword,CString &strErr);			//在指定的计算机上修改某个帐户的密码
BOOL WNetDelShare(LPCTSTR lpsHostName,LPCTSTR lpsShare,CString &strErr);															//在指定的计算机上删除共享

//常用函数
BOOL RegisterOCX(IN LPCTSTR lpsOcxFileName);																						//注册OCX控件
BOOL CheckOCXRegister(LPWSTR lpsClsid,IN CString &strProgID);																		//检查OCX控件是否注册
DWORD	ConvertIPAddress(LPCTSTR lpszIP);																							//IP地址转换（字符串转换成DWORD）
LPCTSTR ConvertIPAddress(DWORD dwIP);																								//IP地址转换（DWORD转换成字符串）
time_t	OleTimeToTime(DATE dTime);																									//OLE时间(DATE)转化成time_t时间
const CString GetTimeString(IN const SYSTEMTIME& sSysTime,IN const BOOL& bType=FALSE);												//根据给定的SYSTEMTIME结构转化成标准时间格式
DWORD	GetServerTime(LPCTSTR lpServerName);																						//获取指定服务器时间
BOOL	SynchronizeTime(LPCTSTR lpServerName);																						//根据指定服务器的时间同步本地计算机时间
WORD	CRC16(LPBYTE lpData, WORD wLength);																							//CRC校验码生成函数
BOOL	Ping(LPCTSTR pszHostName=NULL,UCHAR nTTL = 10, DWORD dwTimeout = 500, UCHAR nPacketSize = 32);								//测试目标计算机是否存在(发送ECHO请求)
CString GetHostNameByIPAddr(LPCTSTR lpzIPAddr=NULL);																				//根据给定的IP获取目标计算机的名字
void SetComputerName(LPCTSTR lpComputerName,LPCTSTR lpTargetIP=NULL,LPCTSTR lpLoginName=NULL,LPCTSTR lpLoginPassword=NULL);			//修改指定的计算机的名字
BOOL ReStartComputerEx(LPCTSTR lpHostName,UINT uFlags);																				//重新启动计算机
BOOL SetAccountRights(LPCTSTR lpsAccountName,LPCTSTR lpsHostName,LPCTSTR lpsPrivilegeName,BOOL bEnable);							//设置用户运行权限
BOOL MakeDirectory(CString strCreateDir);																							//创建多层目录
void  HandOnMessage(HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);												//使消息往下传递
BOOL ChangeDisplayyMode(DWORD dwBitsPerPel,DWORD dwWidth,DWORD dwHeight);															//更改显示器显示模式
CString time_t2CString(time_t vtime);																								//将数值型时间修改为字符串时间
time_t CString2time_t(LPCTSTR lpstime);																								//将字符串型时间修改为数值型时间
BOOL CheckSQLExist(IN LPCTSTR lpzServerIP,IN OUT CString &strError);																//检查服务器上是否存在SQL数据库
const CString GetGuidString(IN const int& iType = 0);																				//获取GUID码
int GetMonthDayCount(int iYear,int iMonth);																							//获取指定的月当中拥有的天数
void KeepUSKeyboardLayout();																										//设置输入法为英文
void DisablePowerManage();																											//禁止系统休眠
HWND GetWindowHandleByPID(IN const DWORD& dwProcessID);																				//根据给定的进程ID获取其窗口句柄
const CString GetCurrentTimeString(IN const int& iTimeType);																		//获取当前时间值的字符串(0,不带毫秒；1，带毫秒）
//IP设置相关
BOOL GetNetCardID(IN OUT CStringArray &aryNetCardID);																				//读取全部有效的网卡序号
BOOL GetNetCardID(IN const int& iID,IN OUT LPTSTR lpzNetCardID);																	//读取指定顺序号对应的网卡序列号
BOOL GetNetCardIPs(IN LPCTSTR lpzNetCardID,IN OUT CStringArray &aryGetIP,IN OUT CStringArray &aryGetMask);							//获取指定网卡上的所有IP
BOOL SetNetCardIPs(IN LPCTSTR lpzNetCardID,IN OUT CStringArray &arySetIP,IN OUT CStringArray &arySetMask);							//设置指定网卡序列号的所有IP，和子网掩码
//系统未公布API封装
BOOL NotifyIPChange(LPCTSTR lpAdapterName,int nIndex,LPCTSTR lpIPAddress,LPCTSTR lpNetMask);										//通知系统IP更改，不需重启计算机即可让IP更改成功，函数原型见声明
#endif//#ifndef _USUALFUNS_H