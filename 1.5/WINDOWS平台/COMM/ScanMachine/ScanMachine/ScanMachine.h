// ScanMachine.h : main header file for the ScanMachine DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "ThreadSMManager2.h"
#include "ThreadSMManager.h"


//#include "unexcept\CatchDump.h"

// CScanMachineApp
// See ScanMachine.cpp for the implementation of this class
//

class CScanMachineApp : public CWinApp
{
private:
	//CCatchDump* m_pCatchDump;
public:
	CScanMachineApp();

	CString	csAppDir;				//记录程序运行目录
	CString	m_strLogDirPath;		//记录程序运行日志目录路径
	CString	m_strLogFilePath;		//记录程序运行日志完整路径
	CString m_strLogFileName;		//记录程序运行日志完整路径
	CString	csDllFile;				//加载的底层动态库路径
	CString	csCfgFile;				//读取配置文件路径失败后，启用的默认配置文件路径（与动态库同目录）
	int		iWriteLog;				//是否写日志标记
	int		m_iLogFileMaxSize;		//日志文件最大值
	int		m_iNeedNotifyLoopState;	//是否需要下发车辆到达/离开指令
	CString m_strDeviceSerialNo;	//固件序列号

	CString m_strFirmwareVer;
	CString m_strSlaveFirmwareVer;

	CThreadSMManager2* m_pThreadSMManager;
//	CThreadSMManager* m_pThreadSMManager;
public:
	CString GetCurrentDirNew();
	BOOL	CreateDir(LPCTSTR lpzPath);
	void	RecordMsgTemp(IN LPCTSTR strMsg,IN const BOOL& bForce=FALSE);
	time_t	OleTimeToTime(DATE dTime);
	time_t	CString2time_t(LPCTSTR lpztime);
	//实现类是CString类的Format功能
	CString FmtStr(const TCHAR* lpsFmt,...);
	void Hex2Bin(IN LPTSTR lpHex,OUT PBYTE pBin,IN DWORD dwHex_len);
	CString Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len);
	WORD CRC16(LPBYTE lpData, WORD wLength);
// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};



BOOL WINAPI IF_InitDev(IN const UINT& dwThreadID, IN const HWND& hWnd,IN const UINT& dwNotifyMsgID, IN const int& iProvinceID, IN const int& iOverTime);
BOOL WINAPI IF_Destroy(void);
CString WINAPI IF_GetLastErrorDesc(void);
BOOL WINAPI IF_StartGetData(IN const int& iDelayTime);
BOOL WINAPI IF_StopGetData( void );
BOOL WINAPI IF_CheckDeviceStatus(IN OUT DWORD& dwState);
BOOL WINAPI IF_GetData(IN OUT CString& strData, IN OUT int& iDataType, IN OUT int& iSrcType);
BOOL WINAPI IF_Authority(IN const CString& strAuthKey);
BOOL WINAPI IF_GetDeviceFirewareVer(IN OUT CString& strXML);
BOOL WINAPI IF_GetDisplayContext(IN OUT int& iID, IN OUT CString& strXML);
BOOL WINAPI IF_SetDisplayContext(IN const int& iID, IN const CString& strXML);
BOOL WINAPI IF_IssuedData(IN const CString& strXML);
BOOL WINAPI IF_SetParam(IN const CString& strXML);
BOOL WINAPI IF_SetIOState(IN const int& iIOState);




/*add 2017-12 获取实际的固件版本和从机固件版本号信息，在此之前有接口叫类似的名字，但是实际是获取的固件序列号，此处是实际的版本号*/
BOOL WINAPI IF_RealDeviceFirmwareVer(IN OUT CString& strXML);
BOOL WINAPI IF_RealSlaveDeviceFirmwareVer(IN OUT CString& strXML);

/*add 2017-11 zhuyabing,自由语音播放*/
BOOL WINAPI IF_PlaySound(IN const int& iID, IN const CString& strXML);


/*add 2017-12 zhuyabing for upgrade*/
BOOL WINAPI IF_IsUpgrading();//系统升级中，不做业务处理
BOOL WINAPI IF_UpgradeFirware(IN const CString&strJson);//升级固件(包含样式和文件)

bool WINAPI IF_TerminalSystemVersionStatus(int& status);
bool WINAPI IF_ExchangeTerminalSystem(const int& targetSystemVersion);