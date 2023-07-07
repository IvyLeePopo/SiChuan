// ScanMachine.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ScanMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool	bInitDev = false;		//是否初始化动态库
CString	csErrorMsg = _T("");	//错误信息描述
CString	csConfigFile = _T("");	//配置文件路径
int		nProvinceID = 0;		//省份编码

int		nDevType = 0;			//扫码器设备类型（用于区分不同厂家不同协议）
int		nCommPort = 0;			//扫码器设备端口号
int		nBaudRate = 9600;		//扫码器设备波特率

HWND	hMainWnd = NULL;
DWORD	dwMainThreadID = 0;
DWORD	dwMainMessageID = 0;

int		iSerialNum = 0;			//序列号，用于显示命令的区分


//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CScanMachineApp

BEGIN_MESSAGE_MAP(CScanMachineApp, CWinApp)
END_MESSAGE_MAP()


std::string TmpGetDumpDir()
{
    char modePath[300] = {0};
    memset(modePath, 0, 260);

    GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);

    std::string pathStr = modePath;
    size_t pos = pathStr.find_last_of('\\');
    if (pos == std::string::npos)
    {
        return "";
    }
    pathStr = pathStr.substr(0, pos) + "\\TWSDNetPayLog";
    return pathStr;
}


// CScanMachineApp construction

CScanMachineApp::CScanMachineApp()
{
	m_pThreadSMManager = NULL;
	m_iLogFileMaxSize = 0x00;
}


// The one and only CScanMachineApp object

CScanMachineApp theApp;


// CScanMachineApp initialization

BOOL CScanMachineApp::InitInstance()
{
	CWinApp::InitInstance();

	//获取动态库自身路径，只能在动态库已经实例化后进行
	csAppDir.Format( _T("%s"), GetCurrentDirNew() );
	m_strLogDirPath.Format(_T("%s\\TWSDNetPayLog"),		csAppDir);
	m_strLogFileName.Format(_T("HTS%s.log"), CTime::GetCurrentTime().Format(_T("%Y%m%d")));
	m_strLogFilePath.Format( _T("%s\\%s"), m_strLogDirPath, m_strLogFileName);
	csCfgFile.Format( _T("%s\\%s"),					csAppDir, FILE_NAME_DEVICE_SM_CONFIG);

	//是否记录日志标记
	iWriteLog	= ::GetPrivateProfileInt(_T("SMControl"),	_T("WriteLog"),	0,	csCfgFile);
	//是否允许下发车辆到达/离开（0x59,0x60指令）
	m_iNeedNotifyLoopState = ::GetPrivateProfileInt(_T("SMControl"),	_T("NotifyLoop"),	0,	csCfgFile);
	//日志文件最大值
	m_iLogFileMaxSize = ::GetPrivateProfileInt(_T("SMControl"),	_T("LogMaxSize"), FILE_SIZE_LOG_MAX ,	csCfgFile);

	theApp.RecordMsgTemp(_T("/**********************************硬件控制动态库 -- 程序启动*******************************************/"));


	/*
	std::string tmpStr = TmpGetDumpDir();
    CString strDumpFilePath;
    strDumpFilePath.Format(_T("%s\\DumpFile.dmp"), tmpStr.c_str());
    m_pCatchDump = new CCatchDump(1);

    if(NULL!=m_pCatchDump)
    {
        m_pCatchDump->InitCatchExecption(strDumpFilePath,FALSE,_T(""),2);
    }
	*/

	return TRUE;
}

int CScanMachineApp::ExitInstance()
{
	RecordMsgTemp(_T("扫码管理线程，程序退出..."), TRUE);
	if ( NULL != m_pThreadSMManager )
	{
		m_pThreadSMManager->m_bExit = TRUE;
		::Sleep(100);

		int itry = 0;
		while (::WaitForSingleObject(m_pThreadSMManager->m_hThread, 200) != WAIT_OBJECT_0)
		{
			int nErrCode = ::GetLastError();
			if (6 == nErrCode) break;

			::Sleep(100);
			itry++;
			if (itry > 9) break;
		}
		m_pThreadSMManager = NULL;
	}

	return CWinApp::ExitInstance();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString	FmtStr
//
// 功能描述：
//
// 输入参数：const char * lpzFmt
//         ：...
// 编写人员：ROCY
// 编写时间：星期二 2009年9月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CScanMachineApp::FmtStr(const TCHAR* lpsFmt, ...)
{
	CString	str;
	va_list	lpzArg = NULL;

	va_start(lpzArg, lpsFmt);
	str.FormatV(lpsFmt, lpzArg);

	va_end(lpzArg);

	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:fnHexToBin
////函数功能:将16进制的字符串转换成2进制的数字
////输入参数:IN PBYTE	pHex		-- 需要转换的16进制字符串
////		 OUT PBYTE	pbin		-- 输出的2进制数字数组
////		 IN DWORD	dwHex_len	-- 需要转换的16进制数据长度,必须为2的倍数
////输出参数:无
////编写时间:2008-11-08
////编写人员:ROCY
////修改时间:
////修改人员:
////函数版本:1.0.0.1
////备注说明:所输入的16进制字符串长度必须是2的倍数
//////////////////////////////////////////////////////////////////////////////////////////
void CScanMachineApp::Hex2Bin(IN LPTSTR lpHex, OUT PBYTE pBin, IN DWORD dwHex_len)
{
	if (dwHex_len % 2)
		return;

	CString strTemp(lpHex);
	strTemp.MakeUpper();

	USES_CONVERSION;
	for (int i = 0; i < strTemp.GetLength() / 2; i++)
	{
		BYTE a = (BYTE)(strTemp.GetAt(i * 2) >= 'A') ? (BYTE)(strTemp.GetAt(i * 2) - 'A' + 10) : (BYTE)(strTemp.GetAt(i * 2) - '0');
		BYTE b = (BYTE)(strTemp.GetAt(i * 2 + 1) >= 'A') ? (BYTE)(strTemp.GetAt(i * 2 + 1) - 'A' + 10) : (BYTE)(strTemp.GetAt(i * 2 + 1) - '0');
		pBin[i] = (BYTE)(a * 0x10 + b);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:BinToHex
////函数功能:将2进制的数字转换成16进制的字符串
////输入参数:IN PBYTE	pBin		-- 需要转换的2进制数字数组
////		 IN DWORD	dwHex_len	-- 需要转换的2进制数字数组长度
////输出参数:CString	非空 -- 转换成功后输出的16进制字符串
////		 CString	NULL -- 转换失败
////编写时间:2008-11-08
////编写人员:ROCY
////修改时间:
////修改人员:
////函数版本:1.0.0.1
////备注说明:如果调用成功后,不需要调用者释放内存
//////////////////////////////////////////////////////////////////////////////////////////
CString CScanMachineApp::Bin2Hex(IN PBYTE pBin, IN DWORD dwBin_Len)
{
	CString strHex;
	CString strTemp;
	if (NULL != pBin)
	{
		for ( DWORD i = 0; i < dwBin_Len; ++i )
		{
			strTemp.Format(_T("%02X"), pBin[i] );
			strHex += strTemp;
		}
	}
	return strHex;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	作	  者 : ROCY
//	时	  间 : 2008-06-01
//  版	  本 : 1.0.0.1
//	函数名称 : CRC16
//	函数功能 : 根据给定的数据，返回其CRC校验码
//	输入参数 : LPBYTE	--	lpData	需要进行校验的字节数据流
//			   WORD		--	wLength	需要进行校验的字节长度
//	输出参数 : WORD		--	返回的CRC校验码，双字节
//  备注说明 : 当给定的校验参数指针为空，则返回 0
///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD CScanMachineApp::CRC16(LPBYTE lpData, WORD wLength)
{
	BYTE	CRC16Hi = 0xFF;
	BYTE	CRC16Lo = 0xFF;
	BYTE	CH = 0xA0;
	BYTE	CL = 0x01;
	BYTE	SaveHi = 0;
	BYTE	SaveLo = 0;

	if (NULL == lpData)
		return 0;

	for (int i = 0; i < wLength; i++)
	{
		CRC16Lo = CRC16Lo ^ lpData[i];

		for (int Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi / 2; //高位右移一位
			CRC16Lo = CRC16Lo / 2; //低位右移一位
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}//for(int Flag = 0;Flag < 8;Flag++)
	}//for(int i = 0;i < nLenght;i++)
	return (WORD)(CRC16Hi * 0x100 + CRC16Lo);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CScanMachineApp::GetCurrentDirNew
//
// 功能描述：获取当前程序运行的所在目录（由调用者决定）
//
// 输入参数：无

// 输出参数：CString		--	获取到的程序运行目录
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CScanMachineApp::GetCurrentDirNew()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR szfile[255] = _T("");
	CString csDir = _T("");

	//对于应用程序（调用者），此方式正确。对于动态库（被调用者），此方式获取的是程序路径
	//	DWORD dwLen = GetModuleFileName(NULL, szfile, 255);

	//调整为动态库自身的路径
	HINSTANCE hInsApp;
	hInsApp = AfxGetInstanceHandle();
	DWORD dwLen = GetModuleFileName(hInsApp, szfile, 255);

	csDir.Format( _T("%s"), szfile);
	csDir = csDir.Left( csDir.ReverseFind(_T('\\')) );
	return csDir;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CScanMachineApp::CreateDir
//
// 功能描述：创建指定路径的目录
//
// 输入参数：IN LPCTSTR lpzPath				--	需要创建的目录路径
//
// 输出参数：BOOL
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CScanMachineApp::CreateDir(LPCTSTR lpzPath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL bNetPath = FALSE;
	CStringArray csyDir;
	CString csPath = lpzPath;

	if (csPath.Find(_T("\\\\")) == 0) bNetPath = TRUE;

	errno_t err = 0;
	while ((err = _access_s( csPath, 0)) != 0 )
	{
		if (csPath.Find(_T('\\')) == -1) return FALSE;
		CString csTemp = csPath.Mid(csPath.ReverseFind(_T('\\')));
		csyDir.InsertAt(0, csTemp);
		csPath = csPath.Left(csPath.ReverseFind(_T('\\')));
		if (bNetPath && csPath.GetLength() < 2) return FALSE;
	}

	while (csyDir.GetSize() != 0)
	{
		csPath += csyDir[0];
		csyDir.RemoveAt(0);
		if (!::CreateDirectory(csPath, NULL))
			return FALSE;
	}
	return TRUE;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CScanMachineApp::RecordMsgTemp
//
// 功能描述：记录信息到临时文件（日志）
//
// 输入参数：IN LPCTSTR strMsg			--	需要记录的信息串
//
// 输出参数：void
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CScanMachineApp::RecordMsgTemp(IN LPCTSTR strMsg, IN const BOOL& bForce)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (  ( 0 == iWriteLog ) || (0 == _tcslen(strMsg) ) )
	{
		return;
	}

	CFile m_file;

	static CString strBufferLog = _T("");
	//获得时间
	SYSTEMTIME sysGetTime = {0};
	::GetLocalTime(&sysGetTime);
	CTime m_curtime = CTime::GetCurrentTime();
	CString sTemp = _T("");
	CString strCurrentDateFile;
	strCurrentDateFile.Format(_T("HTS%s.log"), CTime::GetCurrentTime().Format(_T("%Y%m%d")));
	//日志文件名不一致
	if (strCurrentDateFile != m_strLogFileName)
	{
		if (CFile::hFileNull != m_file.m_hFile)
		{
			m_file.Close();
			Sleep(10);
		}

		m_strLogFileName = strCurrentDateFile;
		m_strLogFilePath.Format(_T("%s\\%s"), m_strLogDirPath, m_strLogFileName);
	}

	if (m_file.Open(m_strLogFilePath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate))
	{
		sTemp.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d -- "), sysGetTime.wYear, sysGetTime.wMonth, sysGetTime.wDay, sysGetTime.wHour, sysGetTime.wMinute, sysGetTime.wSecond, sysGetTime.wMilliseconds); // = m_curtime.Format( _T("%Y-%m-%d %H:%M:%S -- "));
		sTemp += strMsg;
		sTemp += _T("\r\n\r\n");

		strBufferLog += sTemp;
		//大于1K才进行保存
		if ( (strBufferLog.GetLength() > 1024) || bForce )
		{
			m_file.SeekToEnd();
			m_file.Write((LPBYTE)strBufferLog.GetBuffer(0), strBufferLog.GetLength());
			strBufferLog.ReleaseBuffer();
			m_file.Close();
			strBufferLog.Empty();
		}
	}
}






//////////////////////////////////////////////////////////////////////////////////////////////////////
//函数名:	OleTimeToTime
//功  能:	将 DATE格式时间转化为 time_t格式时间
//参  数:	DATE dTime		待转化的 DATE型格式时间
//
//返回值:	转化后的 time_t型格式时间
//////////////////////////////////////////////////////////////////////////////////////////////////////
time_t CScanMachineApp::OleTimeToTime(DATE dTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COleDateTimeSpan tSpan;
	COleDateTime t1 = (time_t)0;
	COleDateTime t2 = dTime;
	COleDateTime t3(2999, 12, 31, 23, 59, 59);
	if ( t2 > t3 )
	{
		t2 = t3;
	}
	tSpan = t2 - t1;
	return time_t( tSpan.m_span * 86400 + 0.5 );
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//函数名:	CString2time_t
//功  能:	将字符串型格式时间转化为 time_t型格式时间
//参  数:	LPCTSTR lpztime		待转化的字符串型格式时间
//
//返回值:	转化后的 time_t型格式时间
//////////////////////////////////////////////////////////////////////////////////////////////////////
time_t CScanMachineApp::CString2time_t(LPCTSTR lpztime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString csTemp = _T("");
	COleDateTime oleGetTime;
	csTemp.Format( _T("%s"), lpztime);
	if ( csTemp.GetLength() > 0 )
	{
		if ( csTemp.GetLength() > 19 ) //兼容类似格式 _T("2014-11-23 12:34:56.000");
		{
			csTemp = csTemp.Left(19);
		}
		oleGetTime.ParseDateTime(csTemp);
		return OleTimeToTime(oleGetTime.m_dt);
	}
	else return 0;
}












////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_InitDev
//
// 功能描述：初始化硬件环境，必须首先调用；如调用失败，则后续接口不提供服务
//
// 输入参数：	IN const UINT& dwThreadID				--	调用者线程ID
//				IN const HWND& hWnd						--	调用者窗体句柄
//				IN const UINT& dwNotifyMsgID			--	扫码器主动通知调用者所使用的消息ID,通过 PostMessage(hWnd有效)或   PostThreadMessage(dwThread有效),与调用者进行通讯该参数数值必须大于0
//				IN const int& iProvinceID				--	省份编码
//				IN const int& iOverTime					--	通讯超时设置，单位：毫秒
//
// 输出参数：	BOOL	初始化成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_InitDev(IN const UINT& dwThreadID, IN const HWND& hWnd, IN const UINT& dwNotifyMsgID, IN const int& iProvinceID, IN const int& iOverTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	nProvinceID = iProvinceID;

	if ( 0 == dwNotifyMsgID )
	{
		csErrorMsg.Format( _T("IF_InitDev:	初始化失败！（扫码响应消息为0）") );
		theApp.RecordMsgTemp( csErrorMsg );
		bInitDev = false;
		return FALSE;
	}

	if ( NULL == hWnd && 0 == dwThreadID )
	{
		csErrorMsg.Format( _T("IF_InitDev:	初始化失败！（窗体句柄为空或者线程ID无效）") );
		theApp.RecordMsgTemp( csErrorMsg );
		bInitDev = false;
		return FALSE;
	}

	hMainWnd = hWnd;
	dwMainThreadID = dwThreadID;
	dwMainMessageID = dwNotifyMsgID;

	errno_t err = 0;
	if ((err = _access_s( theApp.csCfgFile, 0)) != 0 )
	{
		csErrorMsg.Format( _T("IF_InitDev:	扫码器配置文件(%s)不存在!"), theApp.csCfgFile );
		theApp.RecordMsgTemp( csErrorMsg );
		bInitDev = false;
		return FALSE;
	}
	else
	{
		nDevType	= ::GetPrivateProfileInt(_T("Device"),	_T("DeviceType"),	1,		theApp.csCfgFile);

		nCommPort	= ::GetPrivateProfileInt(_T("SerialParam"),	_T("Port"),		3,		theApp.csCfgFile);
		nBaudRate	= ::GetPrivateProfileInt(_T("SerialParam"),	_T("BaudRate"),	9600,	theApp.csCfgFile);
	}

	theApp.m_pThreadSMManager = (CThreadSMManager2*)::AfxBeginThread(RUNTIME_CLASS(CThreadSMManager2), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

//	theApp.m_pThreadSMManager = (CThreadSMManager*)::AfxBeginThread(RUNTIME_CLASS(CThreadSMManager), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if ( NULL == theApp.m_pThreadSMManager )
	{
		csErrorMsg.Format( _T("IF_InitDev:	实例化扫码器控制线程异常!") );
		theApp.RecordMsgTemp( csErrorMsg );
		bInitDev = false;
		return FALSE;
	}

	tagTranstParam sParam = {0};
	theApp.m_pThreadSMManager->SetParam(&sParam, hMainWnd, dwMainThreadID);
	theApp.m_pThreadSMManager->SetMessageID( dwMainMessageID );
	theApp.m_pThreadSMManager->ResumeThread();

	::Sleep(10);
	bInitDev = true;
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_Destroy
//
// 功能描述：清除申请的资源，软件退出前必须调用
//
// 输入参数：	无
//
// 输出参数：	BOOL	退出成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_Destroy(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if ( NULL != theApp.m_pThreadSMManager )
		{
			theApp.m_pThreadSMManager->m_bExit = TRUE;
			::Sleep(100);

			int itry = 0;
			while (::WaitForSingleObject(theApp.m_pThreadSMManager->m_hThread, 200) != WAIT_OBJECT_0)
			{
				int nErrCode = ::GetLastError();
				if (6 == nErrCode) break;

				::Sleep(100);
				itry++;
				if (itry > 9) break;
			}
			theApp.m_pThreadSMManager = NULL;
		}

		csErrorMsg.Format( _T("IF_Destroy:	扫码器底层控制线程已退出！") );
		return TRUE;
	}
	else
	{
		csErrorMsg.Format( _T("IF_Destroy:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}





////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetLastErrorDesc
//
// 功能描述：获取动态库上次最新的错误信息
//
// 输入参数：	无
//
// 输出参数：	CString	最后一次的错误信息
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString WINAPI IF_GetLastErrorDesc(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
	}
	else
	{
		csErrorMsg.Format( _T("IF_GetLastErrorDesc:	扫码器控制动态库未成功初始化！") );
	}
	return csErrorMsg;
}





////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_StartGetData
//
// 功能描述：控制硬件开始获取数据的操作，当硬件获取到数据，向调用者发送消息，消息ID为初始化函数的参数4 dwNotifyMsgID
//
// 输入参数：	IN const int& iDelayTime		--	扫码的延迟启动时间
//
// 输出参数：	BOOL	开始扫码成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_StartGetData(IN const int& iDelayTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			//记录日志
			//	strLog = FmtStr(_T("界面响应,任务流程编号%d,启动扫码(%s)，发送扫码命令"),GetGlobalApp()->m_nTask,(1 == iStartType)?_T("自动"):_T("手动"));
			::Sleep(100);

			//对于内部调用来说，只关心启动扫码以及超时时间；开始时间和消耗时间等参数，主程序可在外部获取
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_START_SCAN, (WPARAM)hMainWnd, (LPARAM)(iDelayTime >= 0) ? iDelayTime : 0);

			csErrorMsg.Format( _T("IF_StartGetData:	开始扫码成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_StartGetData:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_StartGetData:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_StopGetData
//
// 功能描述：控制硬件停止获取数据的操作
//
// 输入参数：	无
//
// 输出参数：	BOOL	停止扫码成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_StopGetData( IN const int& iChangeMMI )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			//记录日志
			//	strLog = FmtStr(_T("界面响应,任务流程编号%d,强制停止扫码操作，发送停止扫码命令"),GetGlobalApp()->m_nTask);
			::Sleep(100);

			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_STOP_SCAN, (WPARAM)hMainWnd, (LPARAM)iChangeMMI);
			csErrorMsg.Format( _T("IF_StopGetData:	停止扫码成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_StopGetData:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_StopGetData:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_CheckDeviceStatus
//
// 功能描述：检测硬件的状态
//
// 输入参数：IN OUT DWORD& dwState -- 状态描述，按位进行标识，均为0无1有，二进制数据位定义(1位,串口打开异常;2位,串口通讯故障-无心跳)
//
// 输出参数：	BOOL	检测状态成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_CheckDeviceStatus(IN OUT DWORD& dwState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			dwState = theApp.m_pThreadSMManager->GetSMState();

			csErrorMsg.Format( _T("IF_CheckDeviceStatus:	获取硬件状态成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_CheckDeviceStatus:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_CheckDeviceStatus:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}
BOOL GetData_With_Transmission(IN OUT CString& strData, IN OUT int& iDataType, IN OUT int& iSrcType)
{
	if (!bInitDev)
	{
		return FALSE;
	}
	if (theApp.m_pThreadSMManager == NULL)
	{
		return FALSE;
	}

	tagDeviceFeedback	sTemp;
	::memset(&sTemp, 0, sizeof(tagTransmissionFeedback));
	theApp.m_pThreadSMManager->GetDeviceResponse( sTemp );

	if (sTemp.feed_type == 1)
	{
		if ( 0 == sTemp.inner_dev.ScanResult )
		{
			if ( 0 == sTemp.inner_dev.StateCode)
			{
				iDataType = MAKELONG(0x01, sTemp.inner_dev.DeviceID);
				iSrcType  = 0x01;
				strData.Format( _T("%s"), sTemp.inner_dev.Code );		strData.Trim();
			}
			else
			{
				csErrorMsg.Format( _T("IF_GetData:	获取扫码数据结果[未能检索到有效数据]") );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
		}
	}
	else if (sTemp.feed_type == 2)
	{
		iDataType = MAKELONG(0x02, sTemp.trans_feedback.DeviceID);
		iSrcType  = 0x02;
		strData.Format( _T("%s"), sTemp.trans_feedback.package );
		strData.Trim();
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;




}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetData
//
// 功能描述：当调用者接收到硬件主动发送的消息，通知数据已经准备好，调用该接口获取硬件回传信息
//
// 输入参数：	IN OUT CString& strData		--	回传的数据内容
//				IN OUT int& iDataType		--	回传的数据类型	1-QR支付码（纯数字）	其它-未定义
//				IN OUT int& iSrcType		--	回传数据所属内挂外设类型	1-扫码器	其它-未定义
//
// 输出参数：	BOOL	获取信息成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_GetData(IN OUT CString& strData, IN OUT int& iDataType, IN OUT int& iSrcType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return GetData_With_Transmission(strData, iDataType, iSrcType);

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			tagSMInnerDeviceState	sTemp;
			::memset(&sTemp, 0, sizeof(tagSMInnerDeviceState));
			theApp.m_pThreadSMManager->GetSMResult( sTemp );

			if ( 0 == sTemp.ScanResult )
			{
				if ( 0 == sTemp.StateCode)
				{
					iDataType = MAKELONG(0x01, sTemp.DeviceID);
					iSrcType  = 0x01;
					strData.Format( _T("%s"), sTemp.Code );		strData.Trim();
				}
				else
				{
					csErrorMsg.Format( _T("IF_GetData:	获取扫码数据结果[未能检索到有效数据]") );
					theApp.RecordMsgTemp( csErrorMsg );
					return FALSE;
				}
			}
			else
			{
				csErrorMsg.Format( _T("IF_GetData: 获取扫码数据结果[扫码数据解析有异常]") );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}

			csErrorMsg.Format( _T("IF_GetData:	获取扫码数据结果[成功]") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_GetData:	获取扫码数据结果[控制线程无效] ") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_GetData:	获取扫码数据结果[控制动态库未成功初始化] ") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_Authority
//
// 功能描述：权限认证，用于判断调用者有没有权限使用
//
// 输入参数：	IN const CString& strAuthKey	--	调用者传入的认证信息字符串
//
// 输出参数：	BOOL	认证成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_Authority(IN const CString& strAuthKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		return TRUE;
	}
	else
	{
		csErrorMsg.Format( _T("IF_Authority:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetDeviceFirewareVer
//
// 功能描述：获取硬件固件参数相关,暂不实现
//
// 输入参数：	IN OUT CString& strXML	--	硬件固件信息集合
//
// 输出参数：	BOOL	认证成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_GetDeviceFirewareVer(IN OUT CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			if (theApp.m_strDeviceSerialNo.IsEmpty())
			{
				//如果终端序列号为空，则每隔30秒读取一次硬件终端序号
				static time_t tLastGetSerialNoTime = 0;
				if (    ((time(NULL) - tLastGetSerialNoTime) > 30)
				        && theApp.m_pThreadSMManager->m_bOpen)
				{
					theApp.m_pThreadSMManager->fnSendGetDeviceSerialNoCMD();
					::Sleep(100);
					tLastGetSerialNoTime = time(NULL);
				}
			}
			else
			{
				strXML = theApp.m_strDeviceSerialNo;
			}
		}
		return TRUE;
	}
	else
	{
		csErrorMsg.Format( _T("IF_GetDeviceFirewareVer:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetDisplayContext
//
// 功能描述：获取智能硬件显示部分的内容,暂不实现
//
// 输入参数：	IN OUT int& iID			--	显示设备具体ID
//				IN OUT CString& strXML	--	返回来的显示内容描述
//
// 输出参数：	BOOL	获取显示内容成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_GetDisplayContext(IN OUT int& iID, IN OUT CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		csErrorMsg.Format( _T("IF_GetDisplayContext:	获取扫码器显示内容失败,暂不支持！") );
		return FALSE;

		if (NULL != theApp.m_pThreadSMManager)
		{
			csErrorMsg.Format( _T("IF_GetDisplayContext:	获取扫码器显示内容成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_GetDisplayContext:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_GetDisplayContext:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_SetDisplayContext
//
// 功能描述：设置智能硬件显示部分的内容
//
// 输入参数：	IN const int& iID			--	显示设备具体ID
//				IN const CString& strXML	--	显示内容描述
//
// 输出参数：	BOOL	设置智能硬件显示部分成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_SetDisplayContext(IN const int& iID, IN const CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			tagScanDisplay	sScanTemp;
			::memset(&sScanTemp, 0, sizeof(tagScanDisplay));

			/*
			<?xml version="1.0" encoding="UTF-8"?>
			<DisplayContext>
			<Version>1.0</Version>
			<ProvinceID>63</ProvinceID>
			<VerifyCode>1</VerifyCode>
			<Data>
			<Type>1</Type>
			<Money>122</Money>
			<Weight>122</Weight>
			<VehicleType>1</VehicleType>
			<VehicleClass>1</VehicleClass>
			<CurrentTime>2016-07-22 12:23:11</CurrentTime>
			</Data>
			</DisplayContext>
			*/

			CString	csDisplay = _T("");
			csDisplay.Format( _T("%s"), strXML );		csDisplay.Trim();

			bool bLoad = false;
			CMarkup xmlLoad;
			bLoad = xmlLoad.SetDoc( csDisplay );
			if ( bLoad )
			{
				CString csValue = _T("");
				xmlLoad.ResetMainPos();
				xmlLoad.FindElem();	//root "DisplayContext"
				xmlLoad.IntoElem();


				CString	csVersion = _T("");
				xmlLoad.ResetMainPos();
				if (xmlLoad.FindElem( _T("Version") ))
				{
					csVersion.Format( _T("%s"), xmlLoad.GetData() );
				}
				else
				{
					csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Version!"), csDisplay );
					return FALSE;
				}

				int iProvince = 0;
				xmlLoad.ResetMainPos();
				if (xmlLoad.FindElem( _T("ProvinceID") ))
				{
					iProvince = _tstoi( xmlLoad.GetData() );
				}
				else
				{
					csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据ProvinceID!"), csDisplay );
					return FALSE;
				}

				CString	csVerifyCode = _T("");
				xmlLoad.ResetMainPos();
				if (xmlLoad.FindElem( _T("VerifyCode") ))
				{
					csVerifyCode.Format( _T("%s"), xmlLoad.GetData() );
				}
				else
				{
					csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据VerifyCode!"), csDisplay );
					return FALSE;
				}

				int iDisplayCount = 0;
				int iDisplayType = 0;
				xmlLoad.ResetMainPos();
				while ( true )
				{
					if (xmlLoad.FindElem( _T("Data") ))
					{
						iDisplayCount += 1;
						xmlLoad.IntoElem();

						xmlLoad.ResetMainPos();
						if (xmlLoad.FindElem( _T("Type") ))		{	iDisplayType = _tstoi( xmlLoad.GetData() );	}
						else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Type!"), csDisplay );	} //	return FALSE;	}

						if ( iDisplayType < 100 ) //原有解析照旧
						{
							sScanTemp.DealInfoUse = 1;
							sScanTemp.Type = iDisplayType;

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Money") ))		{	sScanTemp.Money = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Money!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Weight") ))	{	sScanTemp.Weight = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Weight!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("VehicleType") )) {	sScanTemp.VehicleType = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据VehicleType!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("VehicleClass") )) {	sScanTemp.VehicleClass = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据VehicleClass!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("CurrentTime") )) {	sScanTemp.tShowtime = theApp.CString2time_t(xmlLoad.GetData());	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据CurrentTime!"), csDisplay );	} //	return FALSE;	}

							//限重
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("LimitWeight") )) {	sScanTemp.LimitWeight = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据LimitWeight!"), csDisplay );	} //	return FALSE;	}

							//轴数
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("AixsCount") )) {	sScanTemp.AxisCount = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据AxisCount!"), csDisplay );	} //	return FALSE;	}

							//余额
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Balance") )) {	sScanTemp.Balance = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Balance!"), csDisplay );	} //	return FALSE;	}

							//入口站名
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("EntrySTName") ))
							{
								CString strTemp(xmlLoad.GetData());
								memcpy(&sScanTemp.EntrySTName[0], strTemp.GetBuffer(0), strTemp.GetLength() > _countof(sScanTemp.EntrySTName) ? _countof(sScanTemp.EntrySTName) - 1 : strTemp.GetLength());
								strTemp.ReleaseBuffer();
							}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据EntrySTName!"), csDisplay );	} //	return FALSE;	}
						}
						else
						{
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Context") ))
							{
								csValue.Format( _T("%s"), xmlLoad.GetData() );
							}
							else
							{
								csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Context!"), csDisplay );
								return FALSE;
							}

							if ( 102 == iDisplayType )
							{
								sScanTemp.GGMsgUse = 1;
								::memcpy(sScanTemp.GGMsg, csValue.GetBuffer(0), (csValue.GetLength() < 64) ? csValue.GetLength() : 64 );
								csValue.ReleaseBuffer();
							}

							if ( 103 == iDisplayType )
							{
								sScanTemp.GGVoiceUse = 1;
								::memcpy(sScanTemp.GGVoice, csValue.GetBuffer(0), (csValue.GetLength() < 64) ? csValue.GetLength() : 64 );
								csValue.ReleaseBuffer();
							}

							if ( 104 == iDisplayType )
							{
								sScanTemp.GGWelSetUse = 1;
								::memcpy(sScanTemp.GGWelSet, csValue.GetBuffer(0), (csValue.GetLength() < 8) ? csValue.GetLength() : 8 );
								csValue.ReleaseBuffer();
							}
						}

						xmlLoad.OutOfElem();
					}
					else
					{
						break;	//版本1018调整后，此处不再返回错误
					}
				}

				if ( 0 == iDisplayCount )
				{
					csErrorMsg.Format( _T("IF_SetDisplayContext异常：(%s)未找到有效的xml数据Data!"), csDisplay );
					return FALSE;
				}
			}
			else
			{
				csErrorMsg.Format( _T("IF_SetDisplayContext:	设置显示失败（XML数据无效: %s ）！"), csDisplay );
				return FALSE;
			}

			sScanTemp.iSerial = iSerialNum;	//设置当前序列号
			theApp.m_pThreadSMManager->SetDisplay( sScanTemp );
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_SET_DISPLAY, (WPARAM)iSerialNum, (LPARAM)0);

			iSerialNum += 1;//抛送消息后，序列号加1

			csErrorMsg.Format( _T("IF_SetDisplayContext:	设置智能硬件显示成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_SetDisplayContext:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_SetDisplayContext:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_IssuedData
//
// 功能描述：往智能硬件下发数据
//
// 输入参数：	IN const CString& strXML	--	下发内容
//
// 输出参数：	BOOL	往智能硬件下发数据成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_IssuedData(IN const CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		// csErrorMsg.Format( _T("IF_IssuedData:	往智能硬件下发数据失败，暂不支持！") );
		// return FALSE;
		/**
		 * modify 2017-09 zhuyabing
		 * NFC data transmission
		 */

		if (NULL != theApp.m_pThreadSMManager)
		{
			tagTransmissionData trans_data;
			CString	tmp_xml_str = _T("");
			tmp_xml_str.Format( _T("%s"), strXML );
			tmp_xml_str.Trim();

			CMarkup xml_parser;
			if (! xml_parser.SetDoc(tmp_xml_str))
			{
				csErrorMsg.Format( _T("IF_IssuedData:参数无效(%s)！"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			/*
			xml_parser.ResetMainPos();
			*/
			xml_parser.FindElem();
			xml_parser.IntoElem();

			/*
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("IssueData")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:IssueData无效(%s)！"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			*/

			xml_parser.ResetMainPos();
			//xml_parser.IntoElem();
			if (!xml_parser.FindElem(_T("Data")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Data无效(%s)！"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			xml_parser.IntoElem();
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("Module")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Module无效(%s)！"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			trans_data.module = _tstoi( xml_parser.GetData() );
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("Type")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Type无效(%s)！"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			trans_data.type = _tstoi(xml_parser.GetData());
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("Context")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Context无效(%s)！"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			CString str_context(xml_parser.GetData());
			if (trans_data.module != 1) /*support only NFC*/
			{
				return FALSE;
			}
			if (trans_data.type < 1 || trans_data.type > 5)//签到，消费，冲正，停止，测试
			{
				//return FALSE; 201804 去掉限制
			}
			int context_len = (str_context.GetLength() ) / 2;
			if (context_len > (sizeof(trans_data.data) - 1) )
			{
				theApp.RecordMsgTemp(_T("data too long"));
				return FALSE;
			}
			switch (trans_data.type)
			{
			case 1:
				trans_data.data[0] = 1;
				break;
			case 2:
			case 3:
				trans_data.data[0] = 2;
				break;
			case 4:
				trans_data.data[0] = 3;
				break;
			case 5:
				trans_data.data[0] = 4;
				break;
			}

			trans_data.data_len = context_len + 1;
			theApp.Hex2Bin(str_context.GetBuffer(0), trans_data.data + 1, context_len * 2);
			trans_data.iSerial = iSerialNum;
			theApp.m_pThreadSMManager->SetTransData( trans_data );
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_SET_TRANSMISSION, (WPARAM)iSerialNum, (LPARAM)0);
			iSerialNum += 1;
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_IssuedData:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_IssuedData:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_SetParam
//
// 功能描述：设置智能硬件相关参数,暂不实现
//
// 输入参数：	IN const CString& strXML	--	设置参数内容
//
// 输出参数：	BOOL	设置智能硬件相关参数成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_SetParam(IN const CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		csErrorMsg.Format( _T("IF_SetParam:	设置智能硬件相关参数失败，暂不支持！") );
		return FALSE;

		if (NULL != theApp.m_pThreadSMManager)
		{
			csErrorMsg.Format( _T("IF_SetParam:	设置智能硬件相关参数成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_SetParam:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_SetParam:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_SetIOState
//
// 功能描述：设置智能硬件IO端状态(仅仅1.0版本硬件使用)
//
// 输入参数：	IN const int& iIOState	--	定义IO的开关状态;	0-扫码成功;		1-提示扫码;		2-扫码失败;		3-熄灭全部;
//
// 输出参数：	BOOL	设置智能硬件IO状态成功或失败
// 编写人员：XGS
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_SetIOState(IN const int& iIOState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_SET_LIGHT_MODE, (WPARAM)30, (LPARAM)iIOState);

			csErrorMsg.Format( _T("IF_SetIOState:	设置智能硬件IO状态成功！") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_SetIOState:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_SetIOState:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

bool ParseSoundPara(const std::string& soundXML, std::string& soundMsg);

BOOL WINAPI IF_PlaySound(IN const int& iID, IN const CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	std::string soundMsg;
	std::string tmpXML((LPCTSTR)strXML);
	if(!ParseSoundPara(tmpXML,soundMsg))
	{
		theApp.RecordMsgTemp( "IF_PlaySound:解析参数失败" );
		return false;
	}
	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			theApp.m_pThreadSMManager->SetTTSMessage(soundMsg);
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_TTS, 0, 0);
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_PlaySound:	扫码器控制线程无效！") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_PlaySound:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}

}

bool ParseSoundPara(const std::string& soundXML, std::string& soundMsg)
{
	CMarkup xml;
	xml.SetDoc( soundXML.c_str() );

	xml.ResetMainPos();

	if (!xml.FindElem("VoicePara"))
		return false;
	xml.IntoElem();

	xml.ResetChildPos();
	if (!xml.FindChildElem(_T("Version")))
		return false;

	xml.ResetChildPos();
	if (!xml.FindChildElem(_T("PlayCode")))
		return false;
	std::string codestr((LPCTSTR) xml.GetChildData());
	int playcode =0;
	sscanf(codestr.c_str(),"%d",&playcode);
	if (playcode != 1)
	{
		return false;
	}

	xml.ResetChildPos();
	if (!xml.FindChildElem(_T("Content")))
		return false;

	soundMsg = std::string( (LPCTSTR)xml.GetChildData());

	return true;
}

BOOL WINAPI IF_IsUpgrading()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_pThreadSMManager->IsUpgrading();
}

BOOL WINAPI IF_UpgradeFirware( IN const CString& strJson )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(strJson.GetLength()==0)
	{
		return false;
	}
	std::string insteadStr((LPCTSTR)strJson);
	theApp.m_pThreadSMManager->FirmwareUpgradeInfo(insteadStr);
	return true;
}


BOOL WINAPI IF_RealDeviceFirmwareVer(IN OUT CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			if (theApp.m_strFirmwareVer.IsEmpty())
			{
				
				static time_t tLastGetFirmwareVerTime = 0;
				if (    ((time(NULL) - tLastGetFirmwareVerTime) > 30)
				        && theApp.m_pThreadSMManager->m_bOpen)
				{
					theApp.m_pThreadSMManager->fnSendFirmwareVerCMD();
					::Sleep(100);
					tLastGetFirmwareVerTime = time(NULL);
				}
			}
			else
			{
				strXML = theApp.m_strFirmwareVer;
				theApp.RecordMsgTemp("Get new Firmware version:"+strXML);
			}
		}
		return TRUE;
	}
	else
	{
		csErrorMsg.Format( _T("IF_RealDeviceFirmwareVer:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}
BOOL WINAPI IF_RealSlaveDeviceFirmwareVer(IN OUT CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			if (theApp.m_strSlaveFirmwareVer.IsEmpty())
			{
				static time_t tLastGetSlaveFirmwareVerTime = 0;
				if (    ((time(NULL) - tLastGetSlaveFirmwareVerTime) > 30)
				        && theApp.m_pThreadSMManager->m_bOpen)
				{
					theApp.m_pThreadSMManager->fnSendSlaveFirmwareVerCMD();
					::Sleep(100);
					tLastGetSlaveFirmwareVerTime = time(NULL);
				}
			}
			else
			{
				strXML = theApp.m_strSlaveFirmwareVer;
				theApp.RecordMsgTemp("Get new SlaveFirmware version:"+strXML);
			}
		}
		return TRUE;
	}
	else
	{
		csErrorMsg.Format( _T("IF_RealSlaveDeviceFirmwareVer:	扫码器控制动态库未成功初始化！") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

bool WINAPI IF_TerminalSystemVersionStatus(int& status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInitDev)
	{
		theApp.RecordMsgTemp( "IF_TerminalSystemVersionStatus:	扫码器控制动态库未成功初始化！" );
		return false;
	}
	if (theApp.m_pThreadSMManager == NULL)
	{
		theApp.RecordMsgTemp( "IF_TerminalSystemVersionStatus:	控制线程无效" );
		return false;
	}
	return theApp.m_pThreadSMManager->QueryTerminalSystemStatus(status);
	// return true;
}
bool WINAPI IF_ExchangeTerminalSystem(const int& targetSystemVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInitDev)
	{
		theApp.RecordMsgTemp( "IF_ExchangeTerminalSystem:	扫码器控制动态库未成功初始化！" );
		return false;
	}
	if (theApp.m_pThreadSMManager == NULL)
	{
		theApp.RecordMsgTemp( "IF_ExchangeTerminalSystem:	控制线程无效" );
		return false;
	}
	return theApp.m_pThreadSMManager->ExcuteSystemSwitch(targetSystemVersion);
}