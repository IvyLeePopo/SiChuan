// TWSDNetPay_Dll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//唯一全局变量
CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp()  { return &theApp; }

BEGIN_MESSAGE_MAP(CTWSDNetPay_DllApp, CWinApp)
END_MESSAGE_MAP()

/***********************变量初始化*********************************/
CThreadManage*	CTWSDNetPay_DllApp::m_pThreadManage								= NULL;
BOOL			CTWSDNetPay_DllApp::m_bIsInitEnvironment						= FALSE;
BOOL			CTWSDNetPay_DllApp:: m_bAuthorizedLegal							= FALSE;
BOOL			CTWSDNetPay_DllApp::m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
BOOL			CTWSDNetPay_DllApp::m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
BOOL			CTWSDNetPay_DllApp::m_bSaveLog									= FALSE;
DWORD			CTWSDNetPay_DllApp::m_dwThreadCheckSpanTime						= 300;
CString			CTWSDNetPay_DllApp::m_strWorkDir								= _T("");
CString			CTWSDNetPay_DllApp::m_strLastErrorDesc							= _T("");
CString			CTWSDNetPay_DllApp::m_strVirtualCardPrefix						= _T("");
tagTranstParam	CTWSDNetPay_DllApp::m_sTranstParam								= {0};
BOOL			CTWSDNetPay_DllApp::m_bCurrentDebitMode							= FALSE;
BOOL			CTWSDNetPay_DllApp::m_bIsTrading								= FALSE;
//CPayKeyItems*	CTWSDNetPay_DllApp::m_pCurrentDebitInfo							= NULL;
CPayKeyItems	CTWSDNetPay_DllApp::m_pCurrentDebitInfo;
tagDebitResultItems	CTWSDNetPay_DllApp::m_sCurrentDebitResult					= {0};
DWORD				CTWSDNetPay_DllApp::m_dwTradeBeginOvertime					= 0;
DWORD				CTWSDNetPay_DllApp::m_dwUpdateDevieTimeSpanTime				= 0;
BOOL				CTWSDNetPay_DllApp::m_bIsTestData							= FALSE;
CString			CTWSDNetPay_DllApp::m_strNetPayDllVersion						= _T("");
CString			CTWSDNetPay_DllApp::m_strDeviceDllVersion						= _T("");
int				CTWSDNetPay_DllApp::m_iDeleteLogStartHour						= 0;
int				CTWSDNetPay_DllApp::m_iDeleteLogEndHour							= 0;
int				CTWSDNetPay_DllApp::m_iDeleteLogSpanTime						= 0;
int				CTWSDNetPay_DllApp::m_iDeleteLogSaveDays						= 0;
int				CTWSDNetPay_DllApp::m_iDebitTaskNo								= 0;
int				CTWSDNetPay_DllApp::m_iNotifyMode								= 1;
int				CTWSDNetPay_DllApp::m_iLastDelayTime							= 20;
time_t		    CTWSDNetPay_DllApp::m_tResendDataSpanTime						= 30;
NotifyCallerFun CTWSDNetPay_DllApp::m_fnCallbackNotifyFun						= NULL;
NotifyCallerFun45 CTWSDNetPay_DllApp::m_fnCallbackNotifyFun45					= NULL;
void*			CTWSDNetPay_DllApp::m_pIndPtrFor45								= NULL;
int				CTWSDNetPay_DllApp::m_iExitWaitForTime							= 500;
int				CTWSDNetPay_DllApp::m_iHTTPVersion								= 0;
int				CTWSDNetPay_DllApp::m_iDeviceType								= 0;
CAgencyPay		CTWSDNetPay_DllApp::m_AgencyPay;
bool			CTWSDNetPay_DllApp::m_bReturnPayChannelCodeUnify				= false;
int				CTWSDNetPay_DllApp::m_iAgencyDebitTimeout						= 0;
int				CTWSDNetPay_DllApp::m_iClearMMIDelayTime						= 0;
bool			CTWSDNetPay_DllApp::m_bUseAgencyDll								= true;
BOOL			CTWSDNetPay_DllApp::m_bIsExternMMIDetail						= FALSE;
CString			CTWSDNetPay_DllApp::m_strTWProductSerialNo						= _T("");

int				CTWSDNetPay_DllApp::m_iNFCModuleState							= 0;
bool			CTWSDNetPay_DllApp::m_bNFCSignInSuccess							= false;
bool			CTWSDNetPay_DllApp::m_bNFCIsUse  								= false;
int				CTWSDNetPay_DllApp::m_iNFCCheckSignSpanTime						= 0;
BOOL			CTWSDNetPay_DllApp::m_bNFCPayFail  								= FALSE;
CString			CTWSDNetPay_DllApp::m_strNFCTerminal							= _T("");
CString			CTWSDNetPay_DllApp::m_strNFCSerialNo							= _T("");
int				CTWSDNetPay_DllApp::m_iNFCSignFlowDataCount						= 0;
int				CTWSDNetPay_DllApp::m_iNFCDefaultStartReSignHour				= 0;
int				CTWSDNetPay_DllApp::m_iNFCDefaultEndReSignHour					= 0;
BOOL			CTWSDNetPay_DllApp::m_bNFCAutoSignIsExecute						= FALSE;
int  			CTWSDNetPay_DllApp::m_iNFCSignDataLimitSpanTime					= 0;
time_t   		CTWSDNetPay_DllApp::m_tNFCLastSignDataTime						= 0;
BOOL			CTWSDNetPay_DllApp::m_bNFCIsAllowDebitWithoutCancelData			= FALSE;
int				CTWSDNetPay_DllApp::m_iNFCGetCancelDataLimitTime				= 0;
time_t			CTWSDNetPay_DllApp::m_tNFCStartSignInTime						= 0;
int				CTWSDNetPay_DllApp::m_iNFCPayFailCount							= 0;

CTWSDNetPay_DllApp::CTWSDNetPay_DllApp() 
{
	hFileUpdate = NULL;
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
CString	CTWSDNetPay_DllApp::FmtStr(const TCHAR* lpsFmt,...)
{
	CString	str;
	va_list	lpzArg=NULL;

	va_start(lpzArg, lpsFmt);
	str.FormatV(lpsFmt,lpzArg);
	
	va_end(lpzArg);

	return str;
}

time_t OleTimeToTime(DATE dTime)
{
	COleDateTimeSpan tSpan;
	COleDateTime t1=(time_t)0;
	COleDateTime t2=dTime;
	COleDateTime t3(2999, 12, 31, 23, 59, 59);
	if( t2 > t3 )
	{
		t2 = t3;
	}
	tSpan=t2-t1;
	return time_t( tSpan.m_span * 86400 + 0.5 );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString2time_t
//     
// 功能描述：时间格式字符串转化为 整型数 的时间
//     
// 输入参数：LPCTSTR lpstime
// 输出参数：time_t
// 编写人员：ROCY
// 编写时间：星期二 2009年9月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
time_t CTWSDNetPay_DllApp::CString2time_t(LPCTSTR lpztime)
{
	CString csTemp = _T("");
	COleDateTime oleGetTime;
	csTemp.Format( _T("%s"), lpztime);
	if( csTemp.GetLength() > 0 )
	{
		if( csTemp.GetLength() > 19 )//兼容类似格式 _T("2014-11-23 12:34:56.000");
		{
			csTemp = csTemp.Left(19);
		}

		if( oleGetTime.ParseDateTime(csTemp) )
		{
			return OleTimeToTime(oleGetTime.m_dt);
		}
		else
		{
			return time_t( DEFAULT_TIME );
		}
	}
	else 
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：GetGuidString
//     
// 功能描述：获取新的GUID字符窜
//     
// 输入参数：无参数
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2012年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CTWSDNetPay_DllApp::GetGuidString(IN const int& iType)
{
	CString strReturn;
	static char buf[64] = {0};  
	GUID guid;  
	if(S_OK == ::CoCreateGuid(&guid))  
	{  
		strReturn.Format(_T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}")
		, guid.Data1  
		, guid.Data2  
		, guid.Data3  
		, guid.Data4[0], guid.Data4[1]  
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]  
		, guid.Data4[6], guid.Data4[7]  
		);  
	} 
	if(1 == iType)
	{
		strReturn.Replace(_T("-"),_T(""));
		strReturn.Replace(_T("{"),_T(""));
		strReturn.Replace(_T("}"),_T(""));
	}
	return strReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::GetCurrentTimeString
//     
// 功能描述：获取当前的时间值
//     
// 输入参数：IN const int& iTimeType
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月19日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CTWSDNetPay_DllApp::GetCurrentTimeString(IN const int& iTimeType)
{
	SYSTEMTIME sysTime = {0};
	::GetLocalTime(&sysTime);
	CString strTime;
	if(0 == iTimeType)
	{
		strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
	}
	else
	{
		strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
	}
	return strTime;
}

//////////////////////////////////////////////////////////////////////////////////
///函数名称: MakeDirectory()
///功能说明: 创建指定的多级目录
///参数说明: CString dd,用来创建的多级目录路径
///数出说明: TRUE  -- 创建成功
///			 FALSE -- 创建失败
///修改时间: 2006-04-12
///修改人员: ROCY
///备    注: 需要包含 windows.h
//////////////////////////////////////////////////////////////////////////////////
BOOL CTWSDNetPay_DllApp::MakeDirectory(CString strCreateDir)
{
	 HANDLE  fFile;  					// File Handle
	 WIN32_FIND_DATA fileinfo = {0}; 	// File Information Structure
	 CStringArray arr;  				// CString Array to hold Directory Structures
	 BOOL bSuccess = false;   			// BOOL used to test if Create Directory was successful
	 int nCount = 0;   					// Counter
	 CString strTemp;  					// Temporary CString Object

	 fFile = FindFirstFile(strCreateDir,&fileinfo);

	 // if the file exists and it is a directory
	 if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	 {
		  //  Directory Exists close file and return
		  FindClose(fFile);
		  return TRUE;
	 }
	 arr.RemoveAll();

	 for(nCount = 0; nCount < strCreateDir.GetLength(); nCount++ )  // Parse the supplied CString Directory String
	 {         
		if(strCreateDir.GetAt(nCount) != '\\')  // if the Charachter is not a \ 
			strTemp += strCreateDir.GetAt(nCount);  // add the character to the Temp String
		else
		{
			arr.Add(strTemp);   // if the Character is a \ 
			strTemp += _T("\\");   // Now add the \ to the temp string
		}
		if(nCount == strCreateDir.GetLength()-1)  // If we reached the end of the String
			arr.Add(strTemp);
	 }

	 // Close the file
	 FindClose(fFile);

	 // Now lets cycle through the String Array and create each directory in turn
	 for(nCount = 1; nCount < arr.GetSize(); nCount++)
	 {
		strTemp = arr.GetAt(nCount);
		bSuccess = CreateDirectory(strTemp,NULL);

		// If the Directory exists it will return a false
		if(bSuccess)
			SetFileAttributes(strTemp,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	 }
	 //  Now lets see if the directory was successfully created
	 fFile = FindFirstFile(strCreateDir,&fileinfo);

	 arr.RemoveAll();
	 if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	 {
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	 }
	 else
	 {
		// For Some reason the Function Failed  Return FALSE
		FindClose(fFile);
		return FALSE;
	 }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	作	  者 : ROCY
//	时	  间 : 2008-06-01
//  版	  本 : 1.0.0.1
//	函数名称 : GetVersionOfFile
//	函数功能 : 获取指定的文件的版本号
//	输入参数 : LPCTSTR	--	lpsPath 文件的绝对路径
//	输出参数 : DWORD -- 返回的本地计算机IP子网掩码，字节合成格式
//  备注说明 : 无
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::GetVersionOfFile(LPCTSTR lpsPath)
{
	DWORD	dwLength=0;
	DWORD	dwNullHandle=0;
	UINT	uiVerLength;
	LPVOID	lpVersionPtr;
	CString strVer=_T("0.0.0.0");
	
	dwLength = ::GetFileVersionInfoSize(lpsPath, &dwNullHandle);
	if (dwLength >0)
	{
		BYTE* pVersionInfo = new BYTE [dwLength];
		if (::GetFileVersionInfo (lpsPath, NULL, dwLength, pVersionInfo)) 
		{
			if (::VerQueryValue (pVersionInfo, _T ("\\"), &lpVersionPtr, &uiVerLength)) 
			{
				VS_FIXEDFILEINFO * p=(VS_FIXEDFILEINFO *)lpVersionPtr;
				strVer.Format(_T("%d.%d.%d.%d"),
					HIWORD(p->dwFileVersionMS),
					LOWORD(p->dwFileVersionMS),
					HIWORD(p->dwFileVersionLS),
					LOWORD(p->dwFileVersionLS));
			}
		}
		delete []pVersionInfo;
	}
	return strVer;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：NotifyCaller
//     
// 功能描述：通知外部调用者，业务被异步处理得到的结果
//     
// 输入参数：IN const int& iOption				-- 通知类型（1，扣款；2，撤单）
//         ：IN const int& iResult				-- 处理结果，见开发文档
// 输出参数：无
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::NotifyCaller(IN const int& iOption,IN const int& iResult)
{
	switch(m_iNotifyMode)
	{
	case 1:
		{
			//通知调用者返回结果，当前操作失败
			if(0 != m_sTranstParam.CallerThreadID)
			{
				::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(iOption,iResult),MAKEWORD(0,0)),(LPARAM)0);
			}

			if(::IsWindow(m_sTranstParam.CallerWnd))
			{
				::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(iOption,iResult),MAKEWORD(0,0)),(LPARAM)0);
			}
		}
		break;
	case 2:
		{
			//使用回调函数通知外部调用者
			if(NULL != m_fnCallbackNotifyFun)
			{
				m_fnCallbackNotifyFun(iOption,iResult);
			}
			else
			{
				RecordLog(FmtStr(_T("[主进程响应],组件通知外部调用者的回调函数(NotifyCaller)指针为空，无法通知调用者")),LOG_LEVEL_ERROR,true);
			}
		}
		break;
	case 3:
		{
			//使用回调函数通知外部调用者(广西特供)
			if(NULL != m_fnCallbackNotifyFun45)
			{
				m_fnCallbackNotifyFun45(iOption,iResult,m_pIndPtrFor45);
			}
			else
			{
				RecordLog(FmtStr(_T("[主进程响应],组件通知外部调用者的回调函数(NotifyCaller45)指针为空，无法通知调用者")),LOG_LEVEL_ERROR,true);
			}
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::CheckIsAlNum
//     
// 功能描述：检测给定的字符串，是否为数字和字母或数字或字母的其中一种
//     
// 输入参数：IN const char* szData
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年4月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTWSDNetPay_DllApp::CheckIsAlNum(IN const char* szData,IN const int& iType)
{
	BOOL bReturn = TRUE;

	if(NULL == szData)
		return bReturn;

	int iLen = _tcslen(szData);
	for(int i = 0;i<iLen;++i)
	{
		switch(iType)
		{
		case 0://是否数字、字母混合
			{
				if(0 == isalnum(szData[i]))
				{
					bReturn = FALSE;
				}
			}
			break;
		case 1://是否数字
			{
				 if( (szData[i] < '0') || (szData[i] >'9') )
				 {
					 bReturn = FALSE;
				 }
			}
			break;
		case 2://是否字母
			{
				if(0 == isalpha(szData[i]))
				{
					bReturn = FALSE;
				}
			}
			break;
		}
		if(!bReturn)
		{
			break;
		}
	}

	return bReturn;
}



//用于站号或者车道号转换过程中，检测其内部是否含有字母
BOOL CTWSDNetPay_DllApp::CheckStringLetter(char * strCheck, int nType)
{
	CString csCheck = _T("");
	csCheck.Format( _T("%s"), strCheck );
	csCheck.MakeUpper();

	if( 1 == nType )//类型1表示站点编码
	{
		if( csCheck.GetLength() > 9 )//对大于9位长度的站点编码，默认按照字符串处理
		{
			return TRUE;
		}
	}

	BOOL bCheck = FALSE;
	for(int i=0; i<csCheck.GetLength(); ++i)
	{
		char	tc = csCheck.GetAt( i );
		if( tc>=_T('A') && tc<=_T('Z') )
		{
			bCheck = TRUE;
			break;
		}
	}
	return bCheck;
}






BOOL CTWSDNetPay_DllApp::InitInstance()
{
	CWinApp::InitInstance();

	TCHAR szTempDir[260]={0};
	::GetModuleFileName(AfxGetInstanceHandle(),szTempDir,sizeof(TCHAR)*260);
	CString strTemp(szTempDir);
	CString strDir;
	strDir = strTemp.Left(strTemp.ReverseFind('\\'));
	m_strWorkDir = strDir;

	//读取配置文件
	ReadConfigInfo();

	//创建日志目录
	strTemp.Format(_T("%s\\TWSDNetPayLog"),m_strWorkDir);
	_tcscpy_s(m_sTranstParam.PathLogDir,_countof(m_sTranstParam.PathLogDir),strTemp.GetBuffer(0));
	strTemp.ReleaseBuffer();
	MakeDirectory(m_sTranstParam.PathLogDir);
	//获取程序版本
	CString strDllFilePath(szTempDir);

	m_strNetPayDllVersion = GetVersionOfFile(strDllFilePath);
	
	strDllFilePath.Format(_T("%s\\%s"),strDir,FILE_NAME_DEVICE_DLL);
	m_strDeviceDllVersion = GetVersionOfFile(strDllFilePath);

	//初始化日志系统
	if(m_bSaveLog)
	{
		INIT_LOG(m_sTranstParam.PathLogDir,_T(""));

		RecordLog(FmtStr(_T("\r\n/*************************************程序启动(业务%s - 设备%s)*************************************/"),m_strNetPayDllVersion,m_strDeviceDllVersion));
	}

	hFileUpdate	= CreateEvent(NULL, FALSE, TRUE,	NULL);	//默认属性、自动重置、初始化
	if( NULL == hFileUpdate )
	{
		RecordLog( _T("创建 播报更新本地存储文件 同步变量失败！") );
	}

	//测试NFC
	m_strNFCSerialNo.Format(_T("53 4E 32 30 31 35 30 31 30 38 30 31 00"));
	m_strNFCTerminal.Format(_T("4C 41 4E 44 49 2D 45 35 58 30 00"));

	m_strNFCSerialNo.Replace(_T(" "),_T(""));
	m_strNFCTerminal.Replace(_T(" "),_T(""));


	return TRUE;
}

int CTWSDNetPay_DllApp::ExitInstance()
{
	//退出日志系统
	if(m_bSaveLog)
	{
		RecordLog(FmtStr(_T("\r\n/*************************************程序停止(业务%s - 设备%s)*************************************/"),m_strNetPayDllVersion,m_strDeviceDllVersion));

		QUIT_LOG();
	}

	if( NULL != hFileUpdate )	
	{
		::CloseHandle(hFileUpdate);
	}

	return CWinApp::ExitInstance();
}

/****************************************对外接口函数********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_InitEnvironment
//     
// 功能描述：初始化组件环境
//     
// 输入参数：IN const UINT& nThreadID				-- 调用者指定的线程ID
//         ：IN const HWND& hWnd					-- 调用者指定的窗体句柄
//         ：IN const unsigned int& nNotifyMsgID	-- 调用者指定的通知消息ID
//		   ：IN const char* szAreaInfo				-- 区域信息(区域编码#路段号)
//         ：IN const char* szLoaclStation			-- 站点信息(站号#站名)
//         ：IN const char* szLoaclLaneID			-- 车道编码
//         ：IN const char* szServerInfo			-- 服务器信息
//         ：IN const int& iProvinceID				-- 省份编码
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment(IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	
	RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)调用开始")));
	try
	{
		/*******************检索参数是否正常*******************/
		if(  (0 == nThreadID) && (NULL == hWnd))
			throw -1;
		if( 0 == nNotifyMsgID)
			throw -2;
		if( (NULL == szAreaInfo) || (0 == _tcslen(szAreaInfo)))
			throw -3;
		if( (NULL == szLoaclStation) || (0 == _tcslen(szLoaclStation)))
			throw -4;
		if( (NULL == szLoaclLaneID) || (0 == _tcslen(szLoaclLaneID)))
			throw -5;
		if( (NULL == szServerInfo) || (0 == _tcslen(szServerInfo)))
			throw -6;
		if( 0 == iProvinceID)
			throw -7;
		if( (0 == nThreadID) && (!::IsWindow(hWnd)))
			throw -8;
		//填充配置信息
		TCHAR   szTemp[256]={0};
		CString strTemp;
		
		m_sTranstParam.ProvinceID			 = iProvinceID;
		m_sTranstParam.ActiveNotifyMsgID	 = nNotifyMsgID;
		m_sTranstParam.CallerThreadID		 = nThreadID;
		m_sTranstParam.CallerWnd			 = hWnd;

		//车道编码
		if( CheckStringLetter( (char *)szLoaclLaneID ) )
		{
			_tcsncpy_s(m_sTranstParam.LaneConfigInfo.LaneID,_countof(m_sTranstParam.LaneConfigInfo.LaneID),szLoaclLaneID,_countof(m_sTranstParam.LaneConfigInfo.LaneID)-1);
		}
		else
		{
			UINT nLaneID = _ttoi( szLoaclLaneID ) % 100000000;
			::_stprintf(m_sTranstParam.LaneConfigInfo.LaneID, "%u", nLaneID );
		}
		if(!CheckIsAlNum(m_sTranstParam.LaneConfigInfo.LaneID))
			throw -5;

		//路段、区域编码
		strTemp.Format(_T("%s"),szAreaInfo);
		if(-1 == strTemp.Find(_T("#")))
			throw -9;
		char szTempArea[20]={0};
		_tcsncpy_s(szTempArea,_countof(szTempArea),strTemp.Left(strTemp.Find(_T("#"))).GetBuffer(0),_countof(szTempArea)-1);
		strTemp.Left(strTemp.Find(_T("#"))).ReleaseBuffer();
		if(!CheckIsAlNum(szTempArea,0x01))
			throw -3;
		m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(szTempArea);

		char szTempRaod[20]={0};
		CString strRoadID = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcsncpy_s(szTempRaod,_countof(szTempRaod),strRoadID.GetBuffer(0),_countof(szTempRaod)-1);
		strRoadID.ReleaseBuffer();
		if(!CheckIsAlNum(szTempRaod,0x01))
			throw -3;
		m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

		//站点编码和站名
		strTemp.Format(_T("%s"),szLoaclStation);
		if(-1 == strTemp.Find(_T("#")))
			throw -10;
		CString strSTTemp = strTemp.Left(strTemp.Find(_T("#")));

		BOOL bCheck = CheckStringLetter( strSTTemp.GetBuffer(0), 1 );
		strSTTemp.ReleaseBuffer();
		if( bCheck )
		{
			_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationID,_countof(m_sTranstParam.LaneConfigInfo.StationID),strSTTemp.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationID)-1);strSTTemp.ReleaseBuffer();
		}
		else
		{
			UINT nStationID = _ttoi( strSTTemp );
			::_stprintf(m_sTranstParam.LaneConfigInfo.StationID, "%u", nStationID );
		}

		//特殊处理代码区域
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//广东华快的出口站号特殊处理
				case 30:
				case 31:
					{
						strSTID.Format(_T("%04d%03d%02d"),m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,_ttoi(m_sTranstParam.LaneConfigInfo.StationID));
						_tcscpy_s(m_sTranstParam.LaneConfigInfo.StationID,_countof(m_sTranstParam.LaneConfigInfo.StationID),strSTID.GetBuffer(0));strSTID.ReleaseBuffer();
						//m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strSTID);
					}
					break;
				}
			}
			break;
		}
		if(!CheckIsAlNum(m_sTranstParam.LaneConfigInfo.StationID))
			throw -4;

		CString strSTName = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationTab,_countof(m_sTranstParam.LaneConfigInfo.StationTab),strSTName.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationTab)-1);strSTName.ReleaseBuffer();
		
		_tcsncpy_s(m_sTranstParam.PathCommDir,_countof(m_sTranstParam.PathCommDir),m_strWorkDir.GetBuffer(0),_countof(m_sTranstParam.PathCommDir)-1);m_strWorkDir.ReleaseBuffer();
		
		if( m_bIsInitEnvironment )
			throw -11;

		RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%s)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));


		//版本1060调整 进行代扣动态库加载处理
		AgencyInitDll(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);

		//进行合法性校验
		m_bAuthorizedLegal = TRUE;
		//启动业务管理线程
		CString strConfigFilePath;
		m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadManage)
			throw -12;
		strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadManage->SetParam(&m_sTranstParam,NULL);
		//将该线程重新唤醒
		m_pThreadManage->ResumeThread();

		//标记初始化状态
		m_bIsInitEnvironment = TRUE;

		//标记通知方式
		m_iNotifyMode = 1;

		//返回结果
		bReturn = m_bIsInitEnvironment?true:false;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入线程ID与窗体句柄均为0"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入通知调用者消息ID为0"));
			break;
		case -3:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入区域信息非法"));
			break;
		case -4:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入站点编码信息非法"));
			break;
		case -5:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入车道编码信息非法"));
			break;
		case -6:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入服务器信息非法"));
			break;
		case -7:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误，传入省份编码为0"));
			break;
		case -8:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误,传入调用者线程ID和窗体句柄不能同时为空"));
			break;
		case -9:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误,传入区域参数格式不正确"));
			break;
		case -10:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),传入参数检测有误,传入站点信息参数格式不正确"));
			break;
		case -11:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),组件不允许多次初始化"));
			break;
		case -12:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),内部初始化错误,无法启动业务管理线程"));
			break;
		}
		//记录日志
		if(m_strLastErrorDesc != strLog)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}

	RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)调用结束")));

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_InitEnvironment2
//     
// 功能描述：初始化组件环境
//     
// 输入参数：IN const UINT& nThreadID				-- 调用者指定的线程ID
//         ：IN const HWND& hWnd					-- 调用者指定的窗体句柄
//		   ：IN const char* szAreaInfo				-- 区域信息(区域编码#路段号)
//         ：IN const char* szLoaclStation			-- 站点信息(站号#站名)
//         ：IN const char* szLoaclLaneID			-- 车道编码
//         ：IN const char* szServerInfo			-- 服务器信息
//         ：IN const int& iProvinceID				-- 省份编码
//         ：void (*fun)(int option, int result)	-- 扣款/撤单结果通知，回调函数
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年12月14日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1-1
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment2 (IN const UINT& nThreadID,IN const HWND& hWnd,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID,void (WINAPI *fun)(int option, int result))
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	
	RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2)调用开始")));
	try
	{
		/*******************检索参数是否正常*******************/
		if(  (0 == nThreadID) && (NULL == hWnd))
			throw -1;
		if(NULL == fun)
			throw -2;
		//回调函数指针赋值，便于通知调用者
		m_fnCallbackNotifyFun = fun;

		if( (NULL == szAreaInfo) || (0 == _tcslen(szAreaInfo)))
			throw -3;
		if( (NULL == szLoaclStation) || (0 == _tcslen(szLoaclStation)))
			throw -4;
		if( (NULL == szLoaclLaneID) || (0 == _tcslen(szLoaclLaneID)))
			throw -5;
		if( (NULL == szServerInfo) || (0 == _tcslen(szServerInfo)))
			throw -6;
		if( 0 == iProvinceID)
			throw -7;
		if( (0 == nThreadID) && (!::IsWindow(hWnd)))
			throw -8;
		//填充配置信息
		TCHAR   szTemp[256]={0};
		CString strTemp;
		//其它初始化参数赋值
		m_sTranstParam.ProvinceID			 = iProvinceID;
		m_sTranstParam.ActiveNotifyMsgID	 = 0;
		m_sTranstParam.CallerThreadID		 = nThreadID;
		m_sTranstParam.CallerWnd			 = hWnd;

		//车道编码
		if( CheckStringLetter( (char *)szLoaclLaneID ) )
		{
			_tcsncpy_s(m_sTranstParam.LaneConfigInfo.LaneID,_countof(m_sTranstParam.LaneConfigInfo.LaneID),szLoaclLaneID,_countof(m_sTranstParam.LaneConfigInfo.LaneID)-1);
		}
		else
		{
			UINT nLaneID = _ttoi( szLoaclLaneID ) % 100000000;
			::_stprintf(m_sTranstParam.LaneConfigInfo.LaneID, "%u", nLaneID );
		}
		if(!CheckIsAlNum(m_sTranstParam.LaneConfigInfo.LaneID))
			throw -5;

		//路段、区域编码
		strTemp.Format(_T("%s"),szAreaInfo);
		if(-1 == strTemp.Find(_T("#")))
			throw -9;
		char szTempArea[20]={0};
		_tcsncpy_s(szTempArea,_countof(szTempArea),strTemp.Left(strTemp.Find(_T("#"))).GetBuffer(0),_countof(szTempArea)-1);
		strTemp.Left(strTemp.Find(_T("#"))).ReleaseBuffer();
		if(!CheckIsAlNum(szTempArea,0x01))
			throw -3;
		m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(szTempArea);

		char szTempRaod[20]={0};
		CString strRoadID = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcsncpy_s(szTempRaod,_countof(szTempRaod),strRoadID.GetBuffer(0),_countof(szTempRaod)-1);
		strRoadID.ReleaseBuffer();
		if(!CheckIsAlNum(szTempRaod,0x01))
			throw -3;
		m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

		//站点编码和站名
		strTemp.Format(_T("%s"),szLoaclStation);
		if(-1 == strTemp.Find(_T("#")))
			throw -10;
		CString strSTTemp = strTemp.Left(strTemp.Find(_T("#")));

		BOOL bCheck = CheckStringLetter( strSTTemp.GetBuffer(0), 1 );
		strSTTemp.ReleaseBuffer();
		if( bCheck )
		{
			_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationID,_countof(m_sTranstParam.LaneConfigInfo.StationID),strSTTemp.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationID)-1);strSTTemp.ReleaseBuffer();
		}
		else
		{
			UINT nStationID = _ttoi( strSTTemp );
			::_stprintf(m_sTranstParam.LaneConfigInfo.StationID, "%u", nStationID );
		}

		//特殊处理代码区域
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//广东华快的出口站号特殊处理
				case 30:
				case 31:
					{
						strSTID.Format(_T("%04d%03d%02d"),m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,_ttoi(m_sTranstParam.LaneConfigInfo.StationID));
						_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationID,_countof(m_sTranstParam.LaneConfigInfo.StationID),strSTID.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationID)-1);strSTID.ReleaseBuffer();
					}
					break;
				}
			}
			break;
		}
		if(!CheckIsAlNum(m_sTranstParam.LaneConfigInfo.StationID))
			throw -4;

		CString strSTName = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationTab,_countof(m_sTranstParam.LaneConfigInfo.StationTab),strSTName.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationTab)-1);strSTName.ReleaseBuffer();
		
		_tcsncpy_s(m_sTranstParam.PathCommDir,_countof(m_sTranstParam.PathCommDir),m_strWorkDir.GetBuffer(0),_countof(m_sTranstParam.PathCommDir)-1);m_strWorkDir.ReleaseBuffer();
		
		if( m_bIsInitEnvironment )
			throw -11;

		RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%s)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));

		//版本1060调整 进行代扣动态库加载处理
		AgencyInitDll(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);

		//进行合法性校验
		m_bAuthorizedLegal = TRUE;
		//启动业务管理线程
		CString strConfigFilePath;
		m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadManage)
			throw -12;
		strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadManage->SetParam(&m_sTranstParam,NULL);
		//将该线程重新唤醒
		m_pThreadManage->ResumeThread();

		//标记初始化状态
		m_bIsInitEnvironment = TRUE;

		//标记通知方式
		m_iNotifyMode = 2;

		//返回结果
		bReturn = m_bIsInitEnvironment?true:false;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入线程ID与窗体句柄均为0"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入回调函数指针错误"));
			break;
		case -3:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入区域信息非法"));
			break;
		case -4:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入站点编码信息非法"));
			break;
		case -5:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入车道编码信息非法"));
			break;
		case -6:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入服务器信息非法"));
			break;
		case -7:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误，传入省份编码为0"));
			break;
		case -8:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误,传入调用者线程ID和窗体句柄不能同时为空"));
			break;
		case -9:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误,传入区域参数格式不正确"));
			break;
		case -10:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),传入参数检测有误,传入站点信息参数格式不正确"));
			break;
		case -11:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),组件不允许多次初始化"));
			break;
		case -12:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2),内部初始化错误,无法启动业务管理线程"));
			break;
		}
		//记录日志
		if(m_strLastErrorDesc != strLog)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}

	RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2)调用结束")));

	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_InitEnvironment3
//     
// 功能描述：初始化组件环境
//     
// 输入参数：IN const UINT& nThreadID				-- 调用者指定的线程ID
//         ：IN const HWND& hWnd					-- 调用者指定的窗体句柄
//		   ：IN const char* szAreaInfo				-- 区域信息(区域编码#路段号)
//         ：IN const char* szLoaclStation			-- 站点信息(站号#站名)
//         ：IN const char* szLoaclLaneID			-- 车道编码
//         ：IN const char* szServerInfo			-- 服务器信息
//         ：IN const int& iProvinceID				-- 省份编码
//         ：IN OUT void* pIndPtr					-- 个性化指针
//         ：void (*fun)(int option, int result,void* pIndPtr)	-- 扣款/撤单结果通知，回调函数
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年03月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1-2，专门为广西定制的初始化函数，初始化操作的时候，保存一个个性化指针，在回掉的时候传出去
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment3 (IN const UINT& nThreadID,IN const HWND& hWnd,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID,IN OUT void* pIndPtr,void (WINAPI *fun)(int option, int result,void* pIndPtr))
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	
	RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3)调用开始,个性化指针参数:%04X"),pIndPtr));
	try
	{
		/*******************检索参数是否正常*******************/
		if(  (0 == nThreadID) && (NULL == hWnd))
			throw -1;
		if(NULL == fun)
			throw -2;
		//回调函数指针赋值，便于通知调用者
		m_fnCallbackNotifyFun45 = fun;

		if( (NULL == szAreaInfo) || (0 == _tcslen(szAreaInfo)))
			throw -3;
		if( (NULL == szLoaclStation) || (0 == _tcslen(szLoaclStation)))
			throw -4;
		if( (NULL == szLoaclLaneID) || (0 == _tcslen(szLoaclLaneID)))
			throw -5;
		if( (NULL == szServerInfo) || (0 == _tcslen(szServerInfo)))
			throw -6;
		if( 0 == iProvinceID)
			throw -7;
		if( (0 == nThreadID) && (!::IsWindow(hWnd)))
			throw -8;
		//填充配置信息
		TCHAR   szTemp[256]={0};
		CString strTemp;
		//其它初始化参数赋值
		m_sTranstParam.ProvinceID			 = iProvinceID;
		m_sTranstParam.ActiveNotifyMsgID	 = 0;
		m_sTranstParam.CallerThreadID		 = nThreadID;
		m_sTranstParam.CallerWnd			 = hWnd;

		//车道编码
		if( CheckStringLetter( (char *)szLoaclLaneID ) )
		{
			_tcsncpy_s(m_sTranstParam.LaneConfigInfo.LaneID,_countof(m_sTranstParam.LaneConfigInfo.LaneID),szLoaclLaneID,_countof(m_sTranstParam.LaneConfigInfo.LaneID)-1);
		}
		else
		{
			UINT nLaneID = _ttoi( szLoaclLaneID ) % 100000000;
			::_stprintf(m_sTranstParam.LaneConfigInfo.LaneID, "%u", nLaneID );
		}
		if(!CheckIsAlNum(m_sTranstParam.LaneConfigInfo.LaneID))
			throw -5;

		//路段、区域编码
		strTemp.Format(_T("%s"),szAreaInfo);
		if(-1 == strTemp.Find(_T("#")))
			throw -9;
		char szTempArea[20]={0};
		_tcsncpy_s(szTempArea,_countof(szTempArea),strTemp.Left(strTemp.Find(_T("#"))).GetBuffer(0),_countof(szTempArea)-1);
		strTemp.Left(strTemp.Find(_T("#"))).ReleaseBuffer();
		if(!CheckIsAlNum(szTempArea,0x01))
			throw -3;
		m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(szTempArea);

		char szTempRaod[20]={0};
		CString strRoadID = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcsncpy_s(szTempRaod,_countof(szTempRaod),strRoadID.GetBuffer(0),_countof(szTempRaod)-1);
		strRoadID.ReleaseBuffer();
		if(!CheckIsAlNum(szTempRaod,0x01))
			throw -3;
		m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

		//站点编码和站名
		strTemp.Format(_T("%s"),szLoaclStation);
		if(-1 == strTemp.Find(_T("#")))
			throw -10;
		CString strSTTemp = strTemp.Left(strTemp.Find(_T("#")));

		BOOL bCheck = CheckStringLetter( strSTTemp.GetBuffer(0), 1 );
		strSTTemp.ReleaseBuffer();
		if( bCheck )
		{
			_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationID,_countof(m_sTranstParam.LaneConfigInfo.StationID),strSTTemp.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationID)-1);strSTTemp.ReleaseBuffer();
		}
		else
		{
			UINT nStationID = _ttoi( strSTTemp );
			::_stprintf(m_sTranstParam.LaneConfigInfo.StationID, "%u", nStationID );
		}

		//特殊处理代码区域
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//广东华快的出口站号特殊处理
				case 30:
				case 31:
					{
						strSTID.Format(_T("%04d%03d%02d"),m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,_ttoi(m_sTranstParam.LaneConfigInfo.StationID));
						_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationID,_countof(m_sTranstParam.LaneConfigInfo.StationID),strSTID.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationID)-1);strSTID.ReleaseBuffer();
					}
					break;
				}
			}
			break;
		}

		CString strSTName = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcsncpy_s(m_sTranstParam.LaneConfigInfo.StationTab,_countof(m_sTranstParam.LaneConfigInfo.StationTab),strSTName.GetBuffer(0),_countof(m_sTranstParam.LaneConfigInfo.StationTab)-1);strSTName.ReleaseBuffer();
		
		_tcsncpy_s(m_sTranstParam.PathCommDir,_countof(m_sTranstParam.PathCommDir),m_strWorkDir.GetBuffer(0),_countof(m_sTranstParam.PathCommDir)-1);m_strWorkDir.ReleaseBuffer();
		
		if( m_bIsInitEnvironment )
			throw -11;

		//新型操作，保存一个个性化指针
		m_pIndPtrFor45 = pIndPtr;

		//记录日志
		RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%s)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));

		//版本1060调整 进行代扣动态库加载处理
		AgencyInitDll(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);

		//进行合法性校验
		m_bAuthorizedLegal = TRUE;
		//启动业务管理线程
		CString strConfigFilePath;
		m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadManage)
			throw -12;
		strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadManage->SetParam(&m_sTranstParam,NULL);
		//将该线程重新唤醒
		m_pThreadManage->ResumeThread();

		//标记初始化状态
		m_bIsInitEnvironment = TRUE;

		//标记通知方式
		m_iNotifyMode = 3;

		//返回结果
		bReturn = m_bIsInitEnvironment?true:false;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入线程ID与窗体句柄均为0"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入回调函数指针错误"));
			break;
		case -3:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入区域信息非法"));
			break;
		case -4:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入站点编码信息非法"));
			break;
		case -5:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入车道编码信息非法"));
			break;
		case -6:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入服务器信息非法"));
			break;
		case -7:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误，传入省份编码为0"));
			break;
		case -8:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误,传入调用者线程ID和窗体句柄不能同时为空"));
			break;
		case -9:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误,传入区域参数格式不正确"));
			break;
		case -10:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),传入参数检测有误,传入站点信息参数格式不正确"));
			break;
		case -11:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),组件不允许多次初始化"));
			break;
		case -12:
			strLog.Format(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3),内部初始化错误,无法启动业务管理线程"));
			break;
		}
		//记录日志
		if(m_strLastErrorDesc != strLog)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}

	RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3)调用结束")));

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_Destroy
//     
// 功能描述：清理资源
//     
// 输入参数：void
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口2
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_Destroy(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	try
	{
		// 1052版本调整，为了防止退出过程中其他函数的非法访问，将初始化标记置为假
		m_bIsInitEnvironment = FALSE;

		DWORD dwSleepTime = 0;

		CString strLog;
		/**********************************************清理各种线程资源**********************************************/

		strLog.Format(_T("[主进程响应],组件资源清理接口(IF_Destroy),正在开始退出后台线程..."));
		//记录日志
		RecordLog(strLog);

		//线程退出标记
		DWORD dwThreadExitFlag = 0x00;
		//允许重试的次数
		int iAllowTryCount = 10;
		//已经重试次数
		int iHaveTryCount = 0;
		//每次重试的时候间隔时间（单位：毫秒）
		DWORD	dwSpanTime = 100;

		//停止业务管理线程
		if(NULL != m_pThreadManage)
		{
			//设置该线程推出标记为真
		//	m_pThreadManage->m_bExit = TRUE;
			m_pThreadManage->ThreadQuit();
			//沉默200毫秒
			::Sleep(200);

			//每次等待1000毫秒，重试次数不超过10次
			while(OP_SUCCESS != fnCloseThread(m_pThreadManage->m_hThread,1000))
			{
				Sleep(dwSpanTime);
				++iHaveTryCount;
				if (iHaveTryCount >= iAllowTryCount) 	
				{
					//标记该线程未能正常退出
					dwThreadExitFlag |= 0x01;
					//终止尝试
					break;
				}
			}

			strLog.Format(_T("[主进程响应],组件资源清理接口(IF_Destroy),%s"),FmtStr(_T("【业务管理线程】已经停止(重试次数 %d)..."),iHaveTryCount));
			//记录日志
			RecordLog(strLog);
		}

		//释放代扣动态库
		m_AgencyPay.Destroy();
		//释放一些指针资源
		m_fnCallbackNotifyFun	= NULL;
		m_fnCallbackNotifyFun45 = NULL;
		m_pIndPtrFor45			= NULL;
		memset(&m_sTranstParam,sizeof(tagTranstParam),0);
		memset(&m_sCurrentDebitResult,sizeof(tagDebitResultItems),0);
		//重置标记位
		m_bIsInitEnvironment						= FALSE;
		m_bAuthorizedLegal							= FALSE;
		m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
		m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
		//沉默500毫秒
		::Sleep(m_iExitWaitForTime);
		//记录日志
		strLog.Format(_T("[主进程响应],组件即将退出..."));
		RecordLog(strLog,0,TRUE);		
		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetLastErrorDesc
//     
// 功能描述：获取上一次错误信息
//     
// 输入参数：void
// 输出参数：char*
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口3
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const char* WINAPI CTWSDNetPay_DllApp::IF_GetLastErrorDesc(void)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_strLastErrorDesc.GetBuffer(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::IF_StartGetAccountInfo
//     
// 功能描述：启动获取用户信息操作
//     
// 输入参数：IN const int& iOvertime
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口4
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_StartGetAccountInfo(IN const int& iOvertime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	RecordLog(FmtStr(_T("[主进程响应],启动获取用户信息操作接口(IF_StartGetAccountInfo)调用开始")));
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断组件是否成功授权
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断业务处理线程是否异常
		if(NULL == m_pThreadManage)
			throw -3;
		//给业务管理线程发送消息，要求控制硬件进行获取用户信息操作
		m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_START_GET_DATA,(WPARAM)iOvertime,(LPARAM)0x01);
		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		BYTE byFunCode		= 0x03;
		BYTE byExecuteState	= RCC_INNER_ERROR;
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件未成功初始化"));
			byExecuteState	= RCC_COM_NOT_INIT;
			break;
		case -2:
			strLog.Format(_T("[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件未被授权"));
			byExecuteState	= RCC_COM_ILLEGAL;
			break;
		case -3:
			strLog.Format(_T("[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件内部资源异常"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
		//回馈调用者结果
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(m_sTranstParam.CallerWnd))
		{
			::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}
		//保存日志
		RecordLog(FmtStr(_T("[主进程响应],获取用户信息操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"),byFunCode,byExecuteState));
	}
	RecordLog(FmtStr(_T("[主进程响应],启动获取用户信息操作接口(IF_StartGetAccountInfo)调用结束")));

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::IF_StopGetAccountInfo
//     
// 功能描述：停止获取用户信息操作
//     
// 输入参数：void
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口5
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_StopGetAccountInfo(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	RecordLog(FmtStr(_T("[主进程响应],停止获取用户信息操作接口(IF_StopGetAccountInfo)调用开始")));
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断组件是否成功授权
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断业务处理线程是否异常
		if(NULL == m_pThreadManage)
			throw -3;
		//给业务管理线程发送消息，要求控制硬件进行获取用户信息操作
		m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA,(WPARAM)0,(LPARAM)0);
		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,组件未被授权"));
			break;
		case -3:
			strLog.Format(_T("[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,组件内部资源异常"));
			break;
		case -4:
			strLog.Format(_T("[主进程响应],停止获取用户信息操作(IF_StopGetAccountInfo)出错,硬件未处于工作状态"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}

	}
	RecordLog(FmtStr(_T("[主进程响应],停止获取用户信息操作接口(IF_StopGetAccountInfo)调用结束")));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::IF_GetAccountInfo
//     
// 功能描述：获取用户信息
//     
// 输入参数：IN OUT char* szParamContext
//         ：IN OUT int& iParamSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口6
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetAccountInfo(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	CString strDataString;
	RecordLog(FmtStr(_T("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用开始,参数1:%04X,参数2:%d,参数3:%d"),szParamContext,iParamSize,iFormatType));
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断组件是否成功授权
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断业务处理线程是否异常
		if(NULL == m_pThreadManage)
			throw -3;
		//判断参数是否异常
		if((NULL == szParamContext) || (iParamSize<1024))
			throw -4;
		if( (iFormatType>2) || (iFormatType<1))
			throw -5;
		//判断数据是否已经获取成功
		if(!m_pThreadManage->GetAccountDataReadyState())
			throw -6;
		//获取数据
		tagAccountInfo sAccountInfo = m_pThreadManage->GetLastAccountInfo();
		if(1 == iFormatType)
		{
			CDVXMLAccountContext xmlAccount;
			strDataString = xmlAccount.GetXMLString(sAccountInfo.DataContext,sAccountInfo.DataType,sAccountInfo.SrcType);
			strDataString.Replace(_T("\r"),_T(""));
			strDataString.Replace(_T("\n"),_T(""));
		}
		else
		{
			CDVJsonParamAccount jsonAccount;
			jsonAccount.DataType	= sAccountInfo.DataType;
			jsonAccount.DataSrc		= sAccountInfo.SrcType;
			jsonAccount.DataContext.Format(_T("%s"),sAccountInfo.DataContext);

			CJsonParamOperate jsonParamOperation;
			strDataString = jsonParamOperation.FormatParamData2Json(&jsonAccount);
			strDataString.TrimRight();
		}
		if(strDataString.IsEmpty())
			throw -7;
		RecordLog(FmtStr(_T("[主进程响应],获取用户信息接口(IF_GetAccountInfo)输出数据:%s"),strDataString));
		memcpy(szParamContext,strDataString.GetBuffer(0),strDataString.GetLength()>=iParamSize?(iParamSize-1):strDataString.GetLength());
		strDataString.ReleaseBuffer();
		//标记成功
		bReturn = true;
		//设置用户数据准备状态为假
		m_pThreadManage->SetAccountDataReadyState(FALSE);
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,组件未被授权"));
			break;
		case -3:
			strLog.Format(_T("[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,组件内部资源异常"));
			break;
		case -4:
			strLog.Format(_T("[主进程响应],获取用户信息操作(IF_GetAccountInfo)出错,用户数据未能获取成功"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}
	RecordLog(FmtStr(_T("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用结束")));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_DebitMoney
//     
// 功能描述：执行扣款操作
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iParamSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口7
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_DebitMoney(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[主进程响应],执行扣款操作接口(IF_DebitMoney)调用开始,参数1:%s,参数2:%d,参数3:%d"),szParamContext,iParamSize,iFormatType);
	strLog.Replace(_T("\r"),_T(""));
	strLog.Replace(_T("\n"),_T(""));
	RecordLog(strLog);
	CString strOperationMode(_T("未知"));
	CString strDataString;
	CString strAnalyError;

	try
	{
		/*****************************校核过程***********************************/
		//判断组件是否初始化成功
		 if(!m_bIsInitEnvironment)
			throw -1;
		//判断合法性
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断参数是否异常
		if((NULL == szParamContext) || (0 == iParamSize) || (iFormatType>2 || iFormatType<1))
			throw -3;
		//判断上次交易是否完成
		if(m_bIsTrading)
			throw -7;

		strDataString.Format(_T("%s"),szParamContext);

		if(1 == iFormatType)
		{
			CDVXMLDebitParamContext xml;
			if(!xml.Init(strDataString))
			{
				strAnalyError = xml.m_strLastErrorDesc;
				throw -4;
			}

			//复制数据
			m_pCurrentDebitInfo.OverTime			= xml.OverTime;
			m_pCurrentDebitInfo.OperationMode		= xml.OperationMode;
			m_pCurrentDebitInfo.PayCertificateCode	= xml.PayCertificateCode;
			m_pCurrentDebitInfo.PayCertificateType	= xml.PayCertificateType;
			m_pCurrentDebitInfo.PayIdentifier		= xml.PayIdentifier;
			m_pCurrentDebitInfo.Money				= xml.Money;
			m_pCurrentDebitInfo.SubTime				= xml.SubTime;
			m_pCurrentDebitInfo.DebitMainType		= xml.DebitMainType;
			m_pCurrentDebitInfo.VehicleType			= xml.VehicleType;
			m_pCurrentDebitInfo.VehicleClass		= xml.VehicleClass;
			m_pCurrentDebitInfo.VLColor				= xml.VLColor;
			m_pCurrentDebitInfo.VehicleLicense		= xml.VehicleLicense;
			m_pCurrentDebitInfo.AxisCount			= xml.AxisCount;
			m_pCurrentDebitInfo.TotalWeight			= xml.Weight;
			m_pCurrentDebitInfo.LimitWeight			= xml.LimitWeight;
			m_pCurrentDebitInfo.PassCertificateType = xml.PassCertificateType;
			m_pCurrentDebitInfo.CardID				= xml.CardID;
			m_pCurrentDebitInfo.TollDate			= xml.TollDate;
			m_pCurrentDebitInfo.ShiftID				= xml.ShiftID;
			m_pCurrentDebitInfo.EntryStationID		= xml.EntryStationID;
			m_pCurrentDebitInfo.EntryStationName	= xml.EntryStationName;
			m_pCurrentDebitInfo.EntryTime			= xml.EntryTime;
			m_pCurrentDebitInfo.EntryLane			= xml.EntryLane;
			m_pCurrentDebitInfo.EntryVehicleLicense = xml.EntryVehicleLicense;
			m_pCurrentDebitInfo.EntryVLColor		= xml.EntryVLColor;
			m_pCurrentDebitInfo.EntryVehicleType	= xml.EntryVehicleType;
			m_pCurrentDebitInfo.EntryVehicleClass	= xml.EntryVehicleClass;
			m_pCurrentDebitInfo.EntryRoadID			= xml.EntryRoadID;
			m_pCurrentDebitInfo.EntryAreaID			= xml.EntryAreaID;
			m_pCurrentDebitInfo.TicketNo			= xml.TicketNo;
			//新增出口收费员信息
			m_pCurrentDebitInfo.OperatorID			= xml.OperatorID;
			m_pCurrentDebitInfo.OperatorName		= xml.OperatorName;

			m_pCurrentDebitInfo.Valid				= TRUE;
		}
		else
		{
			CDVJsonParamDebitMoney* pDebitMoney = NULL;
			CJsonParamOperate jsonParamOperation;
//			strDataString.Format(_T("{\"DebitParamContext\":{\"Data\":{\"OperationMode\":2,\"OverTime\":60000,\"Vehicle\":{\"License\":\"0\",\"Weight\":0,\"AxisCount\":0,\"Type\":1,\"VLColor\":0,\"Class\":1},\"Operation\":{\"ShiftID\":1,\"PassCertificateType\":0,\"CardID\":\"0\",\"TollDate\":\"20160823\"},\"Entry\":{\"RoadID\":30,\"License\":\"\",\"AreaID\":4412,\"StationName\":\"华快岑村\",\"VColor\":0,\"VClass\":9,\"VType\":1,\"Time\":\"2016-08-23 14:34:28\",\"LaneID\":0,\"StationID\":8},\"Debit\":{\"PayCertificateType\":1,\"PayCertificateCode\":\"283487891099961852\",\"Money\":200,\"PayIdentifier\":\"1E09381F4F1B90000000000000000004\",\"SubTime\":\"2016-08-23 14:34:28\",\"Type\":1}},\"Version\":\"1.0\"}}"));
			pDebitMoney = (CDVJsonParamDebitMoney*)jsonParamOperation.FormatResponseResult2Class(strDataString,JSON_DT_PARAM_DEBIT_MONEY);
			if(NULL == pDebitMoney)
			{
				strAnalyError.Format(_T(""));
				throw -5;
			}
			//复制数据
			m_pCurrentDebitInfo.OverTime			= pDebitMoney->OverTime;
			m_pCurrentDebitInfo.OperationMode		= pDebitMoney->OperationMode;
			m_pCurrentDebitInfo.PayCertificateCode	= pDebitMoney->PayCertificateCode;
			m_pCurrentDebitInfo.PayCertificateType	= pDebitMoney->PayCertificateType;
			m_pCurrentDebitInfo.PayIdentifier		= pDebitMoney->PayIdentifier;
			m_pCurrentDebitInfo.Money				= pDebitMoney->Money;
			m_pCurrentDebitInfo.SubTime				= pDebitMoney->SubTime;
			m_pCurrentDebitInfo.DebitMainType		= pDebitMoney->DebitMainType;
			m_pCurrentDebitInfo.VehicleType			= pDebitMoney->VehicleType;
			m_pCurrentDebitInfo.VehicleClass		= pDebitMoney->VehicleClass;
			m_pCurrentDebitInfo.VLColor				= pDebitMoney->VLColor;
			m_pCurrentDebitInfo.VehicleLicense		= pDebitMoney->VehicleLicense;
			m_pCurrentDebitInfo.AxisCount			= pDebitMoney->AxisCount;
			m_pCurrentDebitInfo.TotalWeight			= pDebitMoney->Weight;
			m_pCurrentDebitInfo.LimitWeight			= pDebitMoney->LimitWeight;
			m_pCurrentDebitInfo.PassCertificateType = pDebitMoney->PassCertificateType;
			m_pCurrentDebitInfo.CardID				= pDebitMoney->CardID;
			m_pCurrentDebitInfo.TollDate			= pDebitMoney->TollDate;
			m_pCurrentDebitInfo.ShiftID				= pDebitMoney->ShiftID;
			m_pCurrentDebitInfo.EntryStationID		= pDebitMoney->EntryStationID;
			m_pCurrentDebitInfo.EntryStationName	= pDebitMoney->EntryStationName;
			m_pCurrentDebitInfo.EntryTime			= pDebitMoney->EntryTime;
			m_pCurrentDebitInfo.EntryLane			= pDebitMoney->EntryLane;
			m_pCurrentDebitInfo.EntryVehicleLicense = pDebitMoney->EntryVehicleLicense;
			m_pCurrentDebitInfo.EntryVLColor		= pDebitMoney->EntryVLColor;
			m_pCurrentDebitInfo.EntryVehicleType	= pDebitMoney->EntryVehicleType;
			m_pCurrentDebitInfo.EntryVehicleClass	= pDebitMoney->EntryVehicleClass;
			m_pCurrentDebitInfo.EntryRoadID			= pDebitMoney->EntryRoadID;
			m_pCurrentDebitInfo.EntryAreaID			= pDebitMoney->EntryAreaID;
			m_pCurrentDebitInfo.TicketNo			= pDebitMoney->TicketNo;
			//新增出口收费员信息
			m_pCurrentDebitInfo.OperatorID			= pDebitMoney->OperatorID;
			m_pCurrentDebitInfo.OperatorName		= pDebitMoney->OperatorName;

			m_pCurrentDebitInfo.Valid				= TRUE;
		}
		//特殊处理代码区域
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//广东华快的入口站号特殊处理
				case 30:
				case 31:
					{
						strSTID.Format(_T("%04d%03d%02d"),m_pCurrentDebitInfo.EntryAreaID,m_pCurrentDebitInfo.EntryRoadID,_ttoi(m_pCurrentDebitInfo.EntryStationID.GetBuffer(0)));m_pCurrentDebitInfo.EntryStationID.ReleaseBuffer();
						m_pCurrentDebitInfo.EntryStationID = strSTID;
						//m_pCurrentDebitInfo.EntryStationID = _ttoi(strSTID);
					}
					break;
				}
			}
			break;
		}

		//检测业务管理线程是否正常
		if(NULL == m_pThreadManage)
			throw -6;

		//标记当前交易正在进行
		m_bIsTrading = TRUE;
		/*****************************响应操作***********************************/

		//版本1060调整，首先进行代扣处理
		if(!AgencyConstructDebitData())
		{
			//正常的扫码流程
			switch(m_pCurrentDebitInfo.OperationMode)
			{
			case 1://整合操作
				{
					strOperationMode.Format(_T("整合模式"));
					m_bCurrentDebitMode = TRUE;
					//重构支付参数
					CDVXMLDebitFlow xmlDebit;
					xmlDebit.ProvinceID		= m_sTranstParam.ProvinceID;
					xmlDebit.Type			= 0x03;
					xmlDebit.Money			= m_pCurrentDebitInfo.Money;
					xmlDebit.Weight			= m_pCurrentDebitInfo.TotalWeight;
					xmlDebit.VehicleType	= m_pCurrentDebitInfo.VehicleType;
					xmlDebit.VehicleClass	= m_pCurrentDebitInfo.VehicleClass;
					xmlDebit.CurrentTime	= time(NULL);
					CString strXMLString	= xmlDebit.ConstructFormatData();

					//进行资源申请
					CString* pstrString = new CString();
					if(NULL == pstrString)
						throw -8;
					pstrString->Format(_T("%s"),strXMLString);

					//关闭上次的定时器
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_CHECK);
					//启动扣款超时定时器
					m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,m_pCurrentDebitInfo.OverTime);
					//开始计时
					m_dwTradeBeginOvertime = GetTickCount();
					//启动扣款流程
					m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_START_DEBIT_FLOW,(WPARAM)pstrString,(LPARAM)0);
					//添加描述
					m_strLastErrorDesc.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(整合操作)"));
				}
				break;
			case 2://单体操作
				{
					strOperationMode.Format(_T("单体模式"));
					m_bCurrentDebitMode = FALSE;
					//开始计时
					m_dwTradeBeginOvertime = GetTickCount();
					//关闭上次的定时器
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_CHECK);
					//启动扣款超时定时器
					m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,m_pCurrentDebitInfo.OverTime);
					//执行扣款操作
					m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_ONLY_DEBIT_MONEY,(WPARAM)0,(LPARAM)0);
					//添加描述
					m_strLastErrorDesc.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(单体操作)"));
				}
				break;
			}
			//记录日志
			RecordLog(m_strLastErrorDesc);
		}

		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		BYTE byFunCode		= 0x01;
		BYTE byExecuteState	= RCC_INNER_ERROR;
	
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),组件未成功初始化"),strOperationMode);
			byExecuteState	= RCC_COM_NOT_INIT;
			break;
		case -2:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),合法性校验失败"),strOperationMode);
			byExecuteState	= RCC_COM_ILLEGAL;
			break;
		case -3:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),传入参数异常"),strOperationMode);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -4:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),传入参数(XML格式)解析出错，详细描述:%s"),strOperationMode,strAnalyError);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -5:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),传入参数(JSON格式)解析出错"),strOperationMode);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -6:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),内部处理异常:业务管理模块未能正常工作"),strOperationMode);
			break;
		case -7:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),内部处理异常:上次交易未完成，不允许频繁调用"),strOperationMode);
			byExecuteState	= RCC_LAST_TRADE_NOT_END;
			break;
		case -8:
			strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),内部处理异常:资源申请失败"),strOperationMode);
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
		//回馈调用者结果
		NotifyCaller(byFunCode,byExecuteState);
		//保存日志
		RecordLog(FmtStr(_T("[主进程响应],扣款操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"),byFunCode,byExecuteState));
	}
	RecordLog(FmtStr(_T("[主进程响应],执行扣款操作接口(IF_DebitMoney)调用结束")));

	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_DebitCancel
//     
// 功能描述：执行撤单操作
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iParamSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口8
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_DebitCancel(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[主进程响应],执行撤单操作接口(IF_DebitCancel)调用开始,参数1:%s,参数2:%d,参数3:%d"),szParamContext,iParamSize,iFormatType);
	strLog.Replace(_T("\r"),_T(""));
	strLog.Replace(_T("\n"),_T(""));
	RecordLog(strLog);
	
	CString strDataString;
	CString strPayIdentifier;
	CDVXMLDebitCancel xml;
	try
	{
		/*****************************校核过程***********************************/
		//判断组件是否初始化成功
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断合法性
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断参数是否异常
		if((NULL == szParamContext) || (0 == iParamSize) || (iFormatType>2 || iFormatType<1))
			throw -3;

		strDataString.Format(_T("%s"),szParamContext);
		if(1 == iFormatType)
		{
			if(!xml.Init(strDataString))
				throw -4;
			strPayIdentifier.Format(_T("%s"),xml.PayIdentifier);
		}
		else
		{
			CDVJsonParamDebitCancel* pDebitCancel = NULL;
			CJsonParamOperate jsonParamOperation;
			pDebitCancel = (CDVJsonParamDebitCancel*)jsonParamOperation.FormatResponseResult2Class(strDataString,JSON_DT_PARAM_DEBIT_CANCLE);
			if(NULL == pDebitCancel)
			{
				throw -5;
			}
			strPayIdentifier = pDebitCancel->PayIdentifier;
		}
		//检测业务管理线程是否正常
		if(NULL == m_pThreadManage)
			throw -6;
		//检测撤单交易标识是否合法
		strPayIdentifier.TrimLeft();
		strPayIdentifier.TrimRight();
		if(strPayIdentifier.IsEmpty())
			throw -7;
		//进行资源申请
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -8;
		pstrString->Format(_T("%s"),strPayIdentifier);

		m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_DEBIT_CANCEL,(WPARAM)pstrString,(LPARAM)0);

		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		BYTE byFunCode		= 0x02;
		BYTE byExecuteState	= RCC_INNER_ERROR;

		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,组件未成功初始化"));
			byExecuteState	= RCC_COM_NOT_INIT;
			break;
		case -2:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,合法性校验失败"));
			byExecuteState	= RCC_COM_ILLEGAL;
			break;
		case -3:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数异常"));
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -4:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数解析出错，详细描述:%s"),xml.m_strLastErrorDesc);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -5:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数解析出错(JSON)"));
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -6:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,内部处理异常:业务管理模块未能正常工作"));
			break;
		case -7:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数有误,需要撤销的订单对应的交易识别码为空"));
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -8:
			strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,内部处理异常:资源申请失败"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
		//回馈调用者结果
		NotifyCaller(byFunCode,byExecuteState);
		//保存日志
		RecordLog(FmtStr(_T("[主进程响应],撤单操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"),byFunCode,byExecuteState));
	}
	RecordLog(FmtStr(_T("[主进程响应],执行撤单操作接口(IF_DebitCancel)调用结束")));
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetDebitResult
//     
// 功能描述：获取扣款/二次扣款结果
//     
// 输入参数：IN OUT char* szParamContext
//         ：IN OUT int& iParamSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口9
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetDebitResult(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	CString strDataString;
	RecordLog(FmtStr(_T("[主进程响应],获取扣款结果接口(IF_GetDebitResult)调用开始,参数1:%04X,参数2:%d,参数3:%d"),szParamContext,iParamSize,iFormatType));
	try
	{
		//判断组件是否初始化成功
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断合法性
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断参数是否异常
		if((iFormatType>2) || (iFormatType<1) || (NULL == szParamContext))
			throw -3;
		//判断结果是否已经准备好
		if(!m_sCurrentDebitResult.Valid)
			throw -4;

		switch(iFormatType)
		{
		case 1:
			{
				CDVXMLDebitResult xml;
				xml.ResultType		= m_sCurrentDebitResult.ResultType;
				xml.ExecuteCode		= m_sCurrentDebitResult.ExecuteCode;
				xml.PayPlatformType = m_sCurrentDebitResult.PayPlatformType;
				xml.DebitTime		= m_sCurrentDebitResult.DebitTime;
				xml.ConsumeTime		= m_sCurrentDebitResult.ConsumeTime;
				xml.ExecuteDesc.Format(_T("%s"),m_sCurrentDebitResult.ExecuteDesc);
				xml.TradeSecurityCode.Format(_T("%s"),m_sCurrentDebitResult.TradeSecurityCode);
				xml.PayIdentifier.Format(_T("%s"),m_sCurrentDebitResult.PayIdentifier);
				xml.DebitOrder.Format(_T("%s"),m_sCurrentDebitResult.DebitOrder);

				strDataString = xml.GetXMLString();

				strDataString.Replace(_T("\r"),_T(""));
				strDataString.Replace(_T("\n"),_T(""));
			}
			break;
		case 2:
			{
				CDVJsonParamDebitResult jsonDebitResult;
				jsonDebitResult.ResultType		= m_sCurrentDebitResult.ResultType;
				jsonDebitResult.ExecuteCode		= m_sCurrentDebitResult.ExecuteCode;
				jsonDebitResult.PayPlatformType = m_sCurrentDebitResult.PayPlatformType;
				jsonDebitResult.DebitTime		= m_sCurrentDebitResult.DebitTime;
				jsonDebitResult.ConsumeTime		= m_sCurrentDebitResult.ConsumeTime;
				jsonDebitResult.ExecuteDesc.Format(_T("%s"),m_sCurrentDebitResult.ExecuteDesc);
				jsonDebitResult.TradeSecurityCode.Format(_T("%s"),m_sCurrentDebitResult.TradeSecurityCode);
				jsonDebitResult.PayIdentifier.Format(_T("%s"),m_sCurrentDebitResult.PayIdentifier);
				jsonDebitResult.DebitOrder.Format(_T("%s"),m_sCurrentDebitResult.DebitOrder);

				CJsonParamOperate jsonParamOperation;
				strDataString = jsonParamOperation.FormatParamData2Json(&jsonDebitResult);
				if(strDataString.IsEmpty())
				{
					throw -5;
				}
			}
			break;
		default:
			break;
		}

		if(strDataString.GetLength()>(iParamSize+1))
			throw -6;

		strDataString.TrimRight();
		RecordLog(FmtStr(_T("[主进程响应],获取扣款结果接口(IF_GetDebitResult)输出结果如下:(%s)%s "),(0x01 == iFormatType)?_T("数据格式:XML"):_T("数据格式:JSON"),strDataString));

		_tcscpy_s(szParamContext,strDataString.GetLength()+1,strDataString.GetBuffer(0));
		strDataString.ReleaseBuffer();
		//重置交易结果
		m_sCurrentDebitResult.Valid = FALSE;

		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,组件未被授权"));
			break;
		case -3:
			strLog.Format(_T("[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,传入参数异常"));
			break;
		case -4:
			strLog.Format(_T("[主进程响应],获取扣款结果操作(IF_GetDebitResult)出错,扣款结果未准备好"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}
	RecordLog(FmtStr(_T("[主进程响应],获取扣款结果接口(IF_GetDebitResult)调用结束")));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::IF_SetMMI
//     
// 功能描述：设置交互界面
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iParamSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口10
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI	CTWSDNetPay_DllApp::IF_SetMMI(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[主进程响应],设置交互界面接口(IF_SetMMI)调用开始,参数1:%s,参数2:%d,参数3:%d"),szParamContext,iParamSize,iFormatType);
	strLog.Replace(_T("\r"),_T(""));
	strLog.Replace(_T("\n"),_T(""));
	RecordLog(strLog);

	CString strParamString(szParamContext);
	CString strDataString;
	CString strSubError;
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断组件是否成功授权
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断业务处理线程是否异常
		if(NULL == m_pThreadManage)
			throw -3;
		if(NULL == m_pThreadManage->m_pThreadDevice)
			throw -4;
		//判断参数是否异常
		if((NULL == szParamContext) || (0 == iParamSize))
			throw -5;
		//判断参数类型
		if( (iFormatType>2) || (iFormatType<1))
			throw -6;
		//获取硬件状态
	//	DWORD dwHDState = m_pThreadManage->m_pThreadDevice->GetDeviceState();

		int iMMIType = 0x00;

		if(0x01 == iFormatType)
		{
			//XML格式
			CDVXMLMMI xml;
			if(!xml.Init(strParamString))
			{
				strSubError = xml.m_strLastErrorDesc;
				throw -7;
			}
			//重构XML数据
			CMarkup xmlShow;
			xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
			xmlShow.AddElem( _T("DisplayContext") );//root
			xmlShow.IntoElem();

			xmlShow.AddElem( _T("Version"),		_T("1.0") );
			xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
			xmlShow.AddElem( _T("VerifyCode"),	0 );

			if(xml.ClearMMI > 0)
			{
				iMMIType = 0x04;
				switch(xml.ClearMMI)
				{
				case 1:
					iMMIType = 0x04;
					break;
				case 2:
					iMMIType = 0x05;
					break;
				case 3:
					iMMIType = 0x06;
					break;
				}

				xmlShow.AddElem( _T("Data") );
				xmlShow.IntoElem();
				xmlShow.AddElem( _T("Type"),		iMMIType );
				xmlShow.OutOfElem();
			}
			else
			{
				if( xml.Text.GetSize() > 0 )
				{
					xmlShow.AddElem( _T("Data") );
					xmlShow.IntoElem();
					xmlShow.AddElem( _T("Type"),		0x01 );
					//获取金额、车型车种、重量、限重
					int iMoney  = 0x00;
					int iWeight = 0x00;
					int iVType  = 0x00;
					int iVClass = 0x00;
					int iLimit  = 0x00;
					int iAxis   = 0x00;
					int iBalance= 0x00;
					CString strEntrySTName;
					for(int i = 0;i<xml.Text.GetSize();++i)
					{
						int iTemp = 0x00;
						switch(xml.Text.GetAt(i).Sub)
						{
						case 1://车型
							{
								iTemp = _ttoi(xml.Text.GetAt(i).Context);
								if(iTemp>10)
								{
									iVType = 0x02;
								}
								else
								{
									iVType = 0x01;
								}
								iVClass = iTemp%10;
							}
							break;
						case 2://金额
							{
								iTemp = _ttoi(xml.Text.GetAt(i).Context);
								iMoney = iTemp;
							}
							break;
						case 3://载重
							{
								iTemp = _ttoi(xml.Text.GetAt(i).Context);
								iWeight = iTemp;
							}
							break;
						case 4://轴数
							{
								iTemp = _ttoi(xml.Text.GetAt(i).Context);
								iAxis = iTemp;
							}
							break;
						case 5://限重
							{
								iTemp = _ttoi(xml.Text.GetAt(i).Context);
								iLimit = iTemp;
							}
							break;
						case 6://电子支付卡余额
							{
								iTemp = _ttoi(xml.Text.GetAt(i).Context);
								iBalance = iTemp;
							}
							break;
						case 7://入口收费站点名称
							{
								strEntrySTName = xml.Text.GetAt(i).Context;
							}
							break;
						}
					}

					xmlShow.AddElem( _T("Money"),			iMoney );
					xmlShow.AddElem( _T("Weight"),			iWeight );
					xmlShow.AddElem( _T("VehicleType"),		iVType );
					xmlShow.AddElem( _T("VehicleClass"),	iVClass );
					xmlShow.AddElem( _T("LimitWeight"),		iLimit );
					xmlShow.AddElem( _T("AixsCount"),		iAxis );
					xmlShow.AddElem( _T("Balance"),			iBalance );
					xmlShow.AddElem( _T("EntrySTName"),		strEntrySTName );
					xmlShow.AddElem( _T("CurrentTime"),		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") );
					xmlShow.OutOfElem();
				}

				if( xml.Media.GetSize() > 0 )
				{
					CString strTemp = _T("");
					for(int j=0; j<xml.Media.GetSize(); ++j)
					{
						xmlShow.AddElem( _T("Data") );
						xmlShow.IntoElem();
						xmlShow.AddElem( _T("Type"),		xml.Media.GetAt(j).Type );

						strTemp.Format( _T("%s"),			xml.Media.GetAt(j).Content );
						xmlShow.AddElem( _T("Context"),		strTemp );
						xmlShow.OutOfElem();
					}
				}
			}

			xmlShow.OutOfElem();
			strDataString = xmlShow.GetDoc();
		}
		else 
		{
			//JSON格式
			CDVJsonParamMMI* pJsonMMI = NULL;
			CJsonParamOperate jsonOperation;

//			strParamString.Format(_T("{\"MMIParamContext\":{\"Data\":{\"ClearMMI\":0,\"ItemData\":{\"Graph\":null,\"Voice\":null,\"Text\":[{\"Sub\":1,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"1\"},{\"Sub\":2,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"200\"},{\"Sub\":3,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":4,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":5,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":6,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":7,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"华快岑村\"}]},\"ItemCount\":7},\"Version\":\"1.0\"}}"));

			pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString,JSON_DT_PARAM_MMI);
			if(NULL == pJsonMMI)
			{
				strSubError.Format(_T("%s"),jsonOperation.GetLastErrorDesc());
				throw -8;
			}

			//重构XML数据
			CMarkup xmlShow;
			xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
			xmlShow.AddElem( _T("DisplayContext") );//root
			xmlShow.IntoElem();

			xmlShow.AddElem( _T("Version"),		_T("1.0") );
			xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
			xmlShow.AddElem( _T("VerifyCode"),	0 );
			xmlShow.AddElem( _T("Data") );

			xmlShow.IntoElem();

			if(pJsonMMI->ClearMMI > 0)
			{
				iMMIType = 0x04;
				switch(pJsonMMI->ClearMMI)
				{
				case 1:
					iMMIType = 0x04;
					break;
				case 2:
					iMMIType = 0x05;
					break;
				case 3:
					iMMIType = 0x06;
					break;
				}
				xmlShow.AddElem( _T("Type"),		iMMIType );
			}
			else
			{
				xmlShow.AddElem( _T("Type"),		0x01 );
				//获取金额、车型车种、重量、限重
				int iMoney  = 0x00;
				int iWeight = 0x00;
				int iVType  = 0x00;
				int iVClass = 0x00;
				int iLimit  = 0x00;
				int iAxis   = 0x00;
				int iBalance= 0x00;
				CString strEntrySTName;
				for(int i = 0;i<pJsonMMI->Text.GetSize();++i)
				{
					int iTemp = 0x00;
					switch(pJsonMMI->Text.GetAt(i).Sub)
					{
					case 1://车型
						{
							iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);
							if(iTemp>10)
							{
								iVType = 0x02;
							}
							else
							{
								iVType = 0x01;
							}
							iVClass = iTemp%10;
						}
						break;
					case 2://金额
						{
							iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

							iMoney = iTemp;
						}
						break;
					case 3://载重
						{
							iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

							iWeight = iTemp;
						}
						break;
					case 4://轴数
						{
							iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

							iAxis = iTemp;
						}
						break;
					case 5://限重
						{
							iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

							iLimit = iTemp;
						}
						break;
					case 6://电子支付卡余额
						{
							iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

							iBalance = iTemp;
						}
						break;
					case 7://入口收费站点名称
						{
							strEntrySTName = pJsonMMI->Text.GetAt(i).Context;
						}
						break;
					}
				}

				xmlShow.AddElem( _T("Money"),			iMoney );
				xmlShow.AddElem( _T("Weight"),			iWeight );
				xmlShow.AddElem( _T("VehicleType"),		iVType );
				xmlShow.AddElem( _T("VehicleClass"),	iVClass );
				xmlShow.AddElem( _T("LimitWeihgt"),		iLimit );
				xmlShow.AddElem( _T("AixsCount"),		iAxis );
				xmlShow.AddElem( _T("Balance"),			iBalance );
				xmlShow.AddElem( _T("EntrySTName"),		strEntrySTName );
				xmlShow.AddElem( _T("CurrentTime"),		CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") );
			}

			xmlShow.OutOfElem();
			xmlShow.OutOfElem();

			strDataString = xmlShow.GetDoc();
		}

		if(strDataString.IsEmpty())
			throw -9;

		//获取数据
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -10;
		pstrString->Format(_T("%s"),strDataString);

		m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO,(WPARAM)pstrString,(LPARAM)iMMIType);

		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,组件未被授权"));
			break;
		case -3:
		case -4:
		case -5:
		case -6:
		case -10:
			strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,组件内部资源异常"));
			break;
		case -7:
			strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(XML格式)有误，详细描述:%s"),strSubError);
			break;
		case -8:
			strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(JSON格式)有误，详细描述:%s"),strSubError);
			break;
		}
		//保存错误描述 
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}
	RecordLog(FmtStr(_T("[主进程响应],设置交互界面接口(IF_SetMMI)调用结束")));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：WINAPI	CTWSDNetPay_DllApp::IF_GetParam
//     
// 功能描述：获取硬件参数
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iParamSize
//         ：IN const int& iFormatType
//         ：IN OUT char* szResultContext
//         ：IN OUT int& iResultSize
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口11
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI	CTWSDNetPay_DllApp::IF_GetParam(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT char* szResultContext,IN OUT int& iResultSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[主进程响应],获取硬件参数接口(IF_GetParam)暂不提供此项功能"));
	
	if(strLog != m_strLastErrorDesc)
	{
		m_strLastErrorDesc = strLog;
		RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetComponentStatus
//     
// 功能描述：检测组件状态
//     
// 输入参数：IN OUT unsigned int& uStatus
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口11
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetComponentStatus(IN OUT unsigned int& uStatus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	DWORD dwState = 0x00;
	CString strLog;
//	RecordLog(FmtStr(_T("[主进程响应],检测组件状态接口(IF_GetComponentStatus)调用开始,参数1:%d"),uStatus));
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断组件是否成功授权
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断业务处理线程是否异常
		if(NULL == m_pThreadManage)
			throw -3;
		//判断硬件控制线程是否正常
		if(NULL == m_pThreadManage->m_pThreadDevice)
			throw -4;

		//支付子平台网络状态
		if(m_bNetErrorHttpServerInTollNetNotExist)
		{
			dwState |= DEVICE_STATE_PAY_WEB;
		}
		//支付子平台服务器状态
		if(m_bNetErrorHttpServerTrawePlatformNotExist)
		{
			dwState |= DEVICE_STATE_PAY_SERVER;
		}
		//获取硬件状态
		DWORD dwHDState = m_pThreadManage->m_pThreadDevice->GetDeviceState();
		
		uStatus = (dwState | dwHDState);

		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件未被授权"));
			break;
		case -3:
		case -4:
			strLog.Format(_T("[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件内部资源异常"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
		}
	}
//	RecordLog(FmtStr(_T("[主进程响应],检测组件状态接口(IF_GetComponentStatus)调用结束")));

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_TranslateData
//     
// 功能描述：执行传输关键信息数据操作
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iParamSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口12
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_TranslateData(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[主进程响应],执行传输关键信息数据接口(IF_TranslateData)暂不提供此项功能"));
	if(strLog != m_strLastErrorDesc)
	{
		m_strLastErrorDesc = strLog;
		RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_Authority
//     
// 功能描述：权限验证
//     
// 输入参数：IN const char* szAuthKey
//         ：IN const int& iAuthKeySize
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：内部接口
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_Authority(IN const char* szAuthKey,IN const int& iAuthKeySize)
{
	bool bReturn = false;

	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_TransferImage
//     
// 功能描述：传输图像
//     
// 输入参数：	IN const char* szImageInfo	图像内容串（XML，JSON）
//				IN const int& iSize			图像内容串大小
//				IN const int& iFormatType	图像内容串格式（XML，JSON）
// 输出参数：bool
// 编写人员：WingF
// 编写时间：2017年8月13日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：内部接口
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_TransferImage(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = _T("");
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断业务处理线程是否异常
		if(NULL == m_pThreadManage)
			throw -2;

		int nImageType = 0;
		CString	csPayID = _T("");
		CString	csImageData = _T("");

		CString	strImage = _T("");
		strImage.Format( _T("%s"), szImageInfo );

		m_strLastErrorDesc.Format(_T("[主进程响应],执行图像传输操作(IF_TransferImage), 正在解析图像传入参数"));
		//解析并获取关键数据
		if( 1 == iFormatType )// XML
		{
			CMarkup xml;
			if( xml.SetDoc( strImage ) )
			{
				if( !xml.FindElem( ) )
				{
					throw -12;
				}
				xml.IntoElem();
				xml.ResetMainPos();

				if( !xml.FindElem(_T("Data")) )
				{
					throw -13;
				}
				xml.IntoElem();

				xml.ResetMainPos();
				if(!xml.FindElem(_T("Type")))
					throw -14;
				nImageType = _ttoi(xml.GetData());

				xml.ResetMainPos();
				if(!xml.FindElem(_T("PayIdentifier")))
					throw -15;
				csPayID.Format( _T("%s"), xml.GetData() );

				xml.ResetMainPos();
				if(!xml.FindElem(_T("Image")))
					throw -16;
				csImageData.Format( _T("%s"), xml.GetData() );
			}
			else 
			{
				throw -11;
			}
		}
		else// JSON
		{
			Json::Reader reader;
			Json::Value  root;
			if( reader.parse( strImage.GetBuffer(0), root) )
			{
				strImage.ReleaseBuffer();

				Json::Value  infos;
				infos = root["DebitImage"];
				Json::Value  datas;
				datas = infos["Data"];

				//Type
				if(Json::intValue == datas["Type"].type())
				{
					nImageType = datas["Type"].asInt();
				}
				else throw -22;

				//PayIdentifier
				if(Json::stringValue == datas["PayIdentifier"].type())
				{
					csPayID.Format( _T("%s"), datas["PayIdentifier"].asCString() );
				}
				else throw -23;

				//Image
				if(Json::stringValue == datas["Image"].type())
				{
					csImageData.Format( _T("%s"), datas["Image"].asCString() );
				}
				else throw -24;
			}
			else 
			{
				throw -21;
			}
		}

		//构造传入XML参数
		CMarkup xmlImage;
		xmlImage.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
		xmlImage.AddElem( _T("AgencyImageContext") );//root
		xmlImage.IntoElem();

		xmlImage.AddElem( _T("Version"),		_T("1.0") );
		xmlImage.AddElem( _T("TimeOut"),		5000 );

		xmlImage.AddElem( _T("Data") );
		xmlImage.IntoElem();
		xmlImage.AddElem( _T("PayIdentifier"),	csPayID );
		xmlImage.AddElem( _T("ImageType"),		nImageType );

		xmlImage.AddElem( _T("ItemData") );
		xmlImage.IntoElem();
		xmlImage.AddElem( _T("Item") );
		xmlImage.IntoElem();

		xmlImage.AddElem( _T("Ordinal"),		1 );

		CString strImgType(_T("Hex"));
		switch(nImageType)
		{
		case 1:
			strImgType.Format(_T("Hex"));
			break;
		case 2:
			strImgType.Format(_T("Base64"));
			break;
		}
		xmlImage.AddElem( _T("Type"),			strImgType );
		xmlImage.AddElem( _T("Image"),			csImageData );
		xmlImage.AddElem( _T("MD5"),			_T("No MD5") );

		xmlImage.OutOfElem();
		xmlImage.OutOfElem();
		xmlImage.OutOfElem();

		CString strImageParam = xmlImage.GetDoc();
		int		nImageLength = strImageParam.GetLength();
		int iResultCode = 0x00;
		m_strLastErrorDesc.Format(_T("[主进程响应],执行图像传输操作(IF_TransferImage), 正在进行图像上传处理"));
		if( 0 == (iResultCode = m_AgencyPay.AgencyDebitImage( strImageParam.GetBuffer(0), nImageLength, 1)) )
		{
			RecordLog(FmtStr(_T("[主进程响应],执行图像传输操作(IF_TransferImage)，调用成功")));
			bReturn = true;
		}
		else
		{
			RecordLog(FmtStr(_T("[主进程响应],执行图像传输操作(IF_TransferImage)异常, 函数返回失败(%d)"),iResultCode) );
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,业务线程异常"));
			break;
		case -11:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入XML参数加载异常"));
			break;
		case -12:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入XML参数根节点异常"));
			break;
		case -13:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入XML参数 数据区 Data 异常"));
			break;
		case -14:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入XML参数 数据 Type 异常"));
			break;
		case -15:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入XML参数 数据 PayIdentifier 异常"));
			break;
		case -16:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入XML参数 数据 Image 异常"));
			break;
		case -21:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入JSON参数加载异常"));
			break;
		case -22:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入JSON参数 数据 Type 异常"));
			break;
		case -23:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入JSON参数 数据 PayIdentifier 异常"));
			break;
		case -24:
			strLog.Format(_T("[主进程响应],图像传输(IF_TransferImage)出错,传入JSON参数 数据 Image 异常"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc);
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：WINAPI	CTWSDNetPay_DllApp::IF_AgencyDebitQuery
//     
// 功能描述：代扣查询（查询车辆是否支持代扣）
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iSize
//         ：IN const int& iFormatType
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年10月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_AgencyDebitQuery(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT int& iResult)
{
	//判断是否启用代扣动态库
	if(!m_bUseAgencyDll)
	{
		return false;
	}
	//执行代扣查询
	iResult = m_AgencyPay.AgencyDebitQuery(szParamContext,iParamSize,iFormatType);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：WINAPI	CTWSDNetPay_DllApp::IF_InnerGetDeviceNo
//     
// 功能描述：获取硬件的终端序号
//     
// 输入参数：IN char* szParamContext
//         ：IN int& iSize
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年10月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：内部接口
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_InnerGetDeviceNo(IN char* szParamContext,IN int& iSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	DWORD dwState = 0x00;
	CString strLog;
//	RecordLog(FmtStr(_T("[主进程响应],内部接口(IF_InnerGetDeviceNo)调用开始,参数1:%d"),szParamContext,iSize));
	try
	{
		//判断组件是否已经被初始化
		if(!m_bIsInitEnvironment)
			throw -1;
		//判断组件是否成功授权
		if(!m_bAuthorizedLegal)
			throw -2;
		//判断参数输入是否有效
		if((NULL == szParamContext) || (iSize<24))
			throw -3;

		if(!m_strTWProductSerialNo.IsEmpty())
		{
			memset(szParamContext,0,iSize);
			strcpy_s(szParamContext,iSize,m_strTWProductSerialNo.GetBuffer(0));
			m_strTWProductSerialNo.ReleaseBuffer();
		}

		//标记成功
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件未被授权"));
			break;
		case -3:
		case -4:
			strLog.Format(_T("[主进程响应],检测组件状态(IF_GetComponentStatus)出错,组件内部资源异常"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
		}
	}
//	RecordLog(FmtStr(_T("[主进程响应],内部接口(IF_InnerGetDeviceNo)调用结束")));

	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::IF_InnerGetSpecialData
//     
// 功能描述：获取指定数据
//     
// 输入参数：IN char* szParamContext
//         ：IN int& iParamSize
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年10月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：内部接口
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_InnerGetSpecialData(IN char* szParamContext,IN int& iParamSize)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::IF_InnerSetSpecialData
//     
// 功能描述：传输指定参数
//     
// 输入参数：IN const char* szParamContext
//         ：IN const int& iParamSize
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年10月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：内部接口
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_InnerSetSpecialData(IN const char* szParamContext,IN const int& iParamSize)
{
	return false;
}

/****************************************内部辅助函数********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::RecordLog
//     
// 功能描述：记录日志
//     
// 输入参数：IN const CString& strLog -- 日志内容
//         ：IN const int& iLevel	  -- 日志等级
//         ：IN const BOOL& bSave	  -- 日志是否立马保存
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::RecordLog(IN const CString& strLog,IN const int& iLevel,IN const BOOL& bSave)
{
	if(m_bSaveLog)
	{
		if(strLog.GetLength()<4)
		{
			return;
		}
		WRITE_LOG(strLog,iLevel,bSave);
		//::OutputDebugString(strLog);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::ReadConfigInfo
//     
// 功能描述：读取配置信息
//     
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::ReadConfigInfo(void)
{
	TCHAR szTemp[256]={0};
	CString strConfigFilePath;
	strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
	//是否保存日志
	m_bSaveLog				= (BOOL)::GetPrivateProfileIntA(_T("Log"),_T("Save"),0,strConfigFilePath);
	//获取日志删除限定时间段起点
	m_iDeleteLogStartHour = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteStartHour"),2,strConfigFilePath);
	//获取日志删除限定时间段终点
	m_iDeleteLogEndHour = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteEndHour"),2,strConfigFilePath);
	//日志删除操作轮询时间
	m_iDeleteLogSpanTime = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteSpanTime"),30*60,strConfigFilePath);
	//日志本地保存天数时间
	m_iDeleteLogSaveDays = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteSaveDays"),60,strConfigFilePath);
	//标记是否测试数据
	m_bIsTestData			= (BOOL)::GetPrivateProfileIntA(_T("Setup"),_T("TestData"),0,strConfigFilePath);
	//线程存活日志检测时间间隔
	m_dwThreadCheckSpanTime = ::GetPrivateProfileIntA(_T("Setup"),_T("ThreadCheckSpanTime"),0,strConfigFilePath);
	//获取虚拟卡前缀
	::GetPrivateProfileString(_T("Setup"),_T("VirtualCardPrefix"),_T("4412"),szTemp,256,strConfigFilePath);
	m_strVirtualCardPrefix.Format(_T("%s"),szTemp);
	//获取往智能硬件下发时间的间隔
	m_dwUpdateDevieTimeSpanTime = (DWORD)::GetPrivateProfileIntA(_T("Setup"),_T("UpdateDeviceTimeSpanTime"),30*60,strConfigFilePath);
	m_dwUpdateDevieTimeSpanTime *= 1000;
	//撤单数据重发检测时间间隔
	m_tResendDataSpanTime		= (DWORD)::GetPrivateProfileInt(_T("CancelRecord"),_T("CheckTime"),30,strConfigFilePath);
	//撤单数据留存本地时限（单位：分钟）
	m_iLastDelayTime			= (DWORD)::GetPrivateProfileInt(_T("CancelRecord"),_T("LastDelay"),30,strConfigFilePath);
	//程序退出前的等待时间（等待资源清除）
	m_iExitWaitForTime			= (DWORD)::GetPrivateProfileInt(_T("Setup"),_T("ExitWaitForTime"),500,strConfigFilePath);
	//HTTP内部版号
	m_iHTTPVersion				= ::GetPrivateProfileInt(_T("ReflectWeb"),	_T("Version"),	0,	strConfigFilePath);
	//确定通讯动态库加载的设备厂家类型
	m_iDeviceType				= ::GetPrivateProfileInt(_T("Setup"),		_T("Device"),	0,	strConfigFilePath);
	//是否将移动支付和代扣支付的支付渠道返回码保持一致
	m_bReturnPayChannelCodeUnify	= (bool)::GetPrivateProfileInt(_T("Setup"),		_T("ReturnPayChannelCodeUnify"),0,strConfigFilePath);
	//获取代扣扣费操作超时时间值
	m_iAgencyDebitTimeout		= ::GetPrivateProfileInt(_T("Setup"),_T("AgencyDebitTimeout"),2000,strConfigFilePath);
	//获取清屏操作内部延时时间值
	m_iClearMMIDelayTime		= ::GetPrivateProfileInt(_T("Setup"),_T("ClearMMIDelayTime"),100,strConfigFilePath);
	//是否启用代扣动态库
	m_bUseAgencyDll				= (bool)::GetPrivateProfileInt(_T("Setup"),_T("UseAgencyPayDll"),1,strConfigFilePath);
	//是否使用扩展的交互细节模式
	m_bIsExternMMIDetail		= (BOOL)::GetPrivateProfileInt(_T("Setup"),_T("UseExternMMI"),1,strConfigFilePath);
	
	//是否启用银联非接模块
	m_bNFCIsUse							= (bool)::GetPrivateProfileInt(_T("Unipay"),_T("UseUnipay"),0,strConfigFilePath);
	//银联模块签到状态监测时间间隔
	m_iNFCCheckSignSpanTime				= ::GetPrivateProfileInt(_T("Unipay"),_T("CheckSignSpanTime"),10,strConfigFilePath);
	if(m_iNFCCheckSignSpanTime>1800 || m_iNFCCheckSignSpanTime<10)
	{
		m_iNFCCheckSignSpanTime = 30;
	}
	//银联模块定时重签检测时间起始点
	m_iNFCDefaultStartReSignHour		= ::GetPrivateProfileInt(_T("Unipay"),_T("AutoStartReSignHour"),0,strConfigFilePath);
	//银联模块定时重签检测时间截止点
	m_iNFCDefaultEndReSignHour			= ::GetPrivateProfileInt(_T("Unipay"),_T("AutoEndReSignHour"),3,strConfigFilePath);
	//银联模块签到数据交互时间间隔限制
	m_iNFCSignDataLimitSpanTime			= ::GetPrivateProfileInt(_T("Unipay"),_T("SignDataLimitSpanTime"),60,strConfigFilePath);
	if(m_iNFCSignDataLimitSpanTime>70 || m_iNFCSignDataLimitSpanTime<30)
	{
		m_iNFCSignDataLimitSpanTime = 60;
	}
	//银联支付流程，是否允许没有获取冲正报文也允许发送扣费请求
	m_bNFCIsAllowDebitWithoutCancelData	= (BOOL)::GetPrivateProfileInt(_T("Unipay"),_T("AllowDebitWithoutCancelData"),0,strConfigFilePath);
	//银联支付流程，获取到刷卡数据后，限制等待冲正数据的时间（单位：秒）
	m_iNFCGetCancelDataLimitTime		= ::GetPrivateProfileInt(_T("Unipay"),_T("GetCancelDataLimitTime"),10,strConfigFilePath);
	if(m_iNFCGetCancelDataLimitTime>15 || m_iNFCGetCancelDataLimitTime<5)
	{
		m_iNFCGetCancelDataLimitTime = 10;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::fnCloseThread
//     
// 功能描述：等待一个即将被关闭线程正常关闭
//     
// 输入参数：IN const HANDLE& hThread			--	需要被关闭的线程句柄
//         ：IN const DWORD& dwMilliseconds		--	等待时间（单位：毫秒）
// 输出参数：int
//		   ：0  -- 线程已经被正常关闭
//         ：-1	-- 线程句柄无效
//         ：-2	-- 内核对象未激发而等待时间已到
//         ：-3	-- 等待的互斥器未被释放
//         ：-4	-- 等待函数执行失败
// 编写人员：ROCY
// 编写时间：2015年11月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::fnCloseThread(IN const HANDLE& hThread,IN const DWORD& dwMilliseconds)
{
	int iReturn = 0;
	//尝试关闭线程
	DWORD dwWaitResult = ::WaitForSingleObject(hThread,dwMilliseconds);
	//获得错误码
	DWORD dwErrorCode = ::GetLastError();
	//消息结构体提
	MSG	 mMsg;

	if (6 == dwErrorCode)//句柄无效
	{
		iReturn = 0;
	}
	else
	{
		//判断等待函数返回结果
		switch(dwWaitResult)
		{
		case WAIT_OBJECT_0:	//内核对象变为激发状态
			iReturn = 0;
			break;
		case WAIT_TIMEOUT:	//内核对象未激发而等待时间已到
			{
				if(::PeekMessage(&mMsg,NULL,0,0,PM_REMOVE))
				{
					::TranslateMessage(&mMsg);Sleep(10);
					::DispatchMessage(&mMsg);Sleep(10);
				}
			}
			iReturn = -2;
			break;
		case WAIT_ABANDONED://等待的互斥器未被释放
			{
				if(::PeekMessage(&mMsg,NULL,0,0,PM_REMOVE))
				{
					::TranslateMessage(&mMsg);Sleep(10);
					::DispatchMessage(&mMsg);Sleep(10);
				}
			}
			iReturn = -3;
			break;
		default:
			iReturn = -4;
			break;
		}
		
	}
	return iReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	作	  者 : ROCY
//	时	  间 : 2008-05-31
//  版	  本 : 1.0.0.1
//	函数名称 : DisplayWinError
//  函数功能 : 显示系统最新的错误信息
//  输入参数 : DWORD	--	dwErrorCode	给定的系统错误代码,一般由 GetLastError() 函数捕获
//			   int		--	iType		默认为0
//			   BOOL		--	bPrompt		是否需要用对话框显示该错误信息（TRUE为需要，FALSE为不需要）
//  输出参数 : LPCTSTR -- 详细的错误描述信息
//  备注说明 : 每次调用API后都可以调用此函数查看详细的错误信息
///////////////////////////////////////////////////////////////////////////////////////////////////////////
LPCTSTR CTWSDNetPay_DllApp::DisplayWinError(DWORD dwErrorCode,int nType,BOOL bPrompt)
{
	static CString  strTemp = _T("");
    LPSTR lpMessageBuffer = NULL;
	USES_CONVERSION;
    ::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,// | FORMAT_MESSAGE_IGNORE_INSERTS,
					  NULL,
					  (nType==0)?dwErrorCode:LsaNtStatusToWinError(dwErrorCode),
					  0,
					  (LPSTR) &lpMessageBuffer,
					  0,
					  NULL);
	strTemp.Format(_T("【错误代码:%d】 -- 【详细描述:%s】"),dwErrorCode,A2T(lpMessageBuffer));
	strTemp.Replace(_T("\r\n"),_T(" "));

	if(bPrompt)
		::MessageBox(NULL,strTemp,_T("系统提示"),MB_ICONWARNING);

    LocalFree(lpMessageBuffer);
	return strTemp;
}

//获取当前进程所使用的内存
DWORD CTWSDNetPay_DllApp::GetProcessMemory(IN const int& iShowType)
{
	DWORD dDevValue = (1 == iShowType)?(1024*1024):1024;
	//获取当前进程使用内存信息
	DWORD dwProcessMemory = 0x00;
	PROCESS_MEMORY_COUNTERS pLocalProcessMemory = {0};
	if(GetProcessMemoryInfo(GetCurrentProcess(),&pLocalProcessMemory,sizeof(pLocalProcessMemory)))
	{
		dwProcessMemory = pLocalProcessMemory.PagefileUsage/dDevValue;
	}

	return dwProcessMemory;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::AgencyInitDll
//     
// 功能描述：初始化代扣动态库
//     
// 输入参数：void
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年8月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::AgencyInitDll(IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID)
{
	bool bInitAgencyDll = false;

	//判断是否启用代扣动态库
	if(!m_bUseAgencyDll)
	{
		RecordLog( FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)，代扣动态库未启用!)")) );
		return bInitAgencyDll;
	}

	int iResultCode = 0x00;
	CString strAgencyPayDll;
	strAgencyPayDll.Format( _T("%s\\TWITC_AGENCY.dll"), m_strWorkDir );
	if( m_AgencyPay.Init( strAgencyPayDll ))
	{
		if(0 == (iResultCode = m_AgencyPay.InitEnvironment(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID)) )
		{
			bInitAgencyDll = true;
			RecordLog( FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)，代扣动态库(%s)初始化成功!)"), strAgencyPayDll) );
		}
		else
		{
			RecordLog( FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),内部初始化错误,无法初始化代扣动态库(错误代码:%d)"),iResultCode) );
		}
	}
	else
	{
		RecordLog( FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment),内部初始化错误,代扣动态库(%s)加载失败"),strAgencyPayDll) );
	}
	return bInitAgencyDll;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::AgencyConstructDebitData
//     
// 功能描述：
//     
// 输入参数：void
// 输出参数：int
// 编写人员：ROCY
// 编写时间：2017年8月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：当返回结果为true，则表示不再进行后续的扫码业务流程；当返回结果为false，则表示继续需要执行扫码业务流程
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::AgencyConstructDebitData()
{
	//判断是否启用代扣动态库
	if(!m_bUseAgencyDll)
	{
		return false;
	}

	//首先判断车牌有效（长度最少7位）
	if( m_pCurrentDebitInfo.VehicleLicense.GetLength() < 7 )
		return false;

	BYTE byFunCode		= 0x01;
	BYTE byExecuteState	= RCC_INNER_ERROR;

	m_dwTradeBeginOvertime = ::GetTickCount();	//计时
	m_strLastErrorDesc.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney), 正在进行车辆代扣处理"));

	CMarkup xmlAgencyDebit;
	xmlAgencyDebit.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
	xmlAgencyDebit.AddElem( _T("DebitParamContext") );//root
	xmlAgencyDebit.IntoElem();

	xmlAgencyDebit.AddElem( _T("Version"),		_T("1.0") );
	xmlAgencyDebit.AddElem( _T("TimeOut"),		m_iAgencyDebitTimeout );

	xmlAgencyDebit.AddElem( _T("Data") );
	xmlAgencyDebit.IntoElem();
	xmlAgencyDebit.AddElem( _T("OverTime"),		m_pCurrentDebitInfo.OverTime );

	xmlAgencyDebit.AddElem( _T("Debit") );
	xmlAgencyDebit.IntoElem();
	xmlAgencyDebit.AddElem( _T("PayIdentifier"),	m_pCurrentDebitInfo.PayIdentifier );
	xmlAgencyDebit.AddElem( _T("Money"),			m_pCurrentDebitInfo.Money );
	xmlAgencyDebit.AddElem( _T("SubTime"),			m_pCurrentDebitInfo.SubTime.Format(_T("%Y-%m-%d %H:%M:%S")) );
	xmlAgencyDebit.AddElem( _T("Type"),				1 );
	xmlAgencyDebit.AddElem( _T("PayOperatorId"),	m_pCurrentDebitInfo.OperatorID );
	xmlAgencyDebit.AddElem( _T("PayOperatorName"),	m_pCurrentDebitInfo.OperatorName );
	xmlAgencyDebit.OutOfElem();

	xmlAgencyDebit.AddElem( _T("Vehicle") );
	xmlAgencyDebit.IntoElem();
	xmlAgencyDebit.AddElem( _T("Type"),			m_pCurrentDebitInfo.VehicleType );
	xmlAgencyDebit.AddElem( _T("Class"),		m_pCurrentDebitInfo.VehicleClass );
	xmlAgencyDebit.AddElem( _T("License"),		m_pCurrentDebitInfo.VehicleLicense );
	xmlAgencyDebit.AddElem( _T("VLColor"),		m_pCurrentDebitInfo.VLColor );
	xmlAgencyDebit.AddElem( _T("AxisCount"),	m_pCurrentDebitInfo.AxisCount );
	xmlAgencyDebit.AddElem( _T("Weight"),		m_pCurrentDebitInfo.TotalWeight );
	xmlAgencyDebit.AddElem( _T("LimitWeight"),	m_pCurrentDebitInfo.LimitWeight );
	xmlAgencyDebit.OutOfElem();

	xmlAgencyDebit.AddElem( _T("Operation") );
	xmlAgencyDebit.IntoElem();
	xmlAgencyDebit.AddElem( _T("PassCertificateType"),	m_pCurrentDebitInfo.PassCertificateType );
	xmlAgencyDebit.AddElem( _T("CardID"),		m_pCurrentDebitInfo.CardID );
	xmlAgencyDebit.AddElem( _T("TollDate"),		m_pCurrentDebitInfo.TollDate );
	xmlAgencyDebit.AddElem( _T("ShiftID"),		m_pCurrentDebitInfo.ShiftID );
	xmlAgencyDebit.AddElem( _T("TicketNo"),		m_pCurrentDebitInfo.TicketNo );
	xmlAgencyDebit.OutOfElem();

	xmlAgencyDebit.AddElem( _T("Entry") );
	xmlAgencyDebit.IntoElem();
	xmlAgencyDebit.AddElem( _T("ProvinceId"),	m_sTranstParam.ProvinceID );
	xmlAgencyDebit.AddElem( _T("AreaID"),		m_pCurrentDebitInfo.EntryAreaID );
	xmlAgencyDebit.AddElem( _T("RoadID"),		m_pCurrentDebitInfo.EntryRoadID );
	xmlAgencyDebit.AddElem( _T("StationID"),	m_pCurrentDebitInfo.EntryStationID );
	xmlAgencyDebit.AddElem( _T("StationName"),	m_pCurrentDebitInfo.EntryStationName );
	xmlAgencyDebit.AddElem( _T("Time"),			m_pCurrentDebitInfo.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")) );

	xmlAgencyDebit.AddElem( _T("LaneID"),		m_pCurrentDebitInfo.EntryLane );
	xmlAgencyDebit.AddElem( _T("License"),		m_pCurrentDebitInfo.EntryVehicleLicense );
	xmlAgencyDebit.AddElem( _T("VColor"),		m_pCurrentDebitInfo.EntryVLColor );
	xmlAgencyDebit.AddElem( _T("VClass"),		m_pCurrentDebitInfo.EntryVehicleClass );
	xmlAgencyDebit.AddElem( _T("VType"),		m_pCurrentDebitInfo.EntryVehicleType );
	xmlAgencyDebit.OutOfElem();

	xmlAgencyDebit.OutOfElem();	//xmlAgencyDebit.AddElem( _T("Data") );
	CString strAgencyDebit = xmlAgencyDebit.GetDoc();
	int		nAgencyDebitLength = strAgencyDebit.GetLength();
	char	szAgencyResult[1024] = {0};

	int iIsCancelAnency = 0x00;
	int iAnencyResult = 0x00;
	DWORD dwStartDebitTime = ::GetTickCount();
	iAnencyResult = m_AgencyPay.AgencyDebitMoney( strAgencyDebit.GetBuffer(0), szAgencyResult, nAgencyDebitLength, 1);
	DWORD dwConsumeTimeCount = ::GetTickCount() - dwStartDebitTime;

	if( 0 == iAnencyResult )
	{
		CString	strAgencyResult = _T("");
		strAgencyResult.Format( _T("%s"), szAgencyResult );
		if( 0 == GetAgencyResult( strAgencyResult,dwConsumeTimeCount ) )
		{
			//通知成功
			byFunCode = 0x01;
			byExecuteState = RCC_OK;
			//控制终端提示成功
			m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)0x01,(LPARAM)0);
			//获取结果信息成功，通知调用者
			NotifyCaller(byFunCode,byExecuteState);
			//记录日志
			RecordLog(FmtStr(_T("[主进程响应],执行扣款操作接口(IF_DebitMoney)调用结束(%s,代扣支付,成功）"),m_pCurrentDebitInfo.VehicleLicense));
			return true;
		}
		else
		{
			iIsCancelAnency = 0x01;
		}
	}
	else
	{
		//判断返回错误，确定是否需要发起代扣撤单
		switch(iAnencyResult)
		{
		case 6:
		case 12:
			iIsCancelAnency = 0x02;
			break;
		}
	}
	
	//判断是否需要执行代扣撤单
	if(iIsCancelAnency>0)
	{
		//标识扣费流程完成
		GetGlobalApp()->m_bIsTrading		= FALSE;
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;

		AgencyConstructCancelData(m_pCurrentDebitInfo.PayIdentifier);
		//通知失败
		byFunCode = 0x01;
		byExecuteState = RCC_PAY_PLATFORM_ERROR;
		//通知调用者
		NotifyCaller(byFunCode,byExecuteState);
		//记录日志
		RecordLog(FmtStr(_T("[主进程响应],车辆(%s)代扣扣费结果处理失败(%s)，已经发起撤单，转入扫码处理"), m_pCurrentDebitInfo.VehicleLicense,(0x01 == iIsCancelAnency)?_T("获取结果异常"):_T("网络超时")) );
		//不再进行扫码流程
		return true;
	}
	else
	{
		RecordLog(FmtStr(_T("[主进程响应],车辆(%s)代扣扣费结果处理失败，不需要撤单，转入扫码处理"), m_pCurrentDebitInfo.VehicleLicense) );
	}
	//继续进行扫码流程
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::AgencyConstructCancelData
//     
// 功能描述：构造代扣撤单数据结构体
//     
// 输入参数：无参数
// 编写人员：ROCY
// 编写时间：2017年8月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::AgencyConstructCancelData(IN const CString& strPayIdentifier)
{
	//判断是否启用代扣动态库
	if(!m_bUseAgencyDll)
	{
		return false;
	}

	//获取结果信息失败，进行撤单处理，转入扫码
	CMarkup xmlCancel;
	xmlCancel.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
	xmlCancel.AddElem( _T("DebitCancelParamContext") );//root
	xmlCancel.IntoElem();

	xmlCancel.AddElem( _T("Version"),		_T("1.0") );
	xmlCancel.AddElem( _T("TimeOut"),		300 );

	xmlCancel.AddElem( _T("Data") );
	xmlCancel.IntoElem();
	xmlCancel.AddElem( _T("TradeKeyItem") );
	xmlCancel.IntoElem();

	xmlCancel.AddElem( _T("Type"),			1 );
	xmlCancel.AddElem( _T("PayIdentifier"),strPayIdentifier);

	xmlCancel.OutOfElem();
	xmlCancel.OutOfElem();

	CString strCancel = xmlCancel.GetDoc();
	int		nCancelLength = strCancel.GetLength();
	return m_AgencyPay.AgencyDebitCancel( strCancel.GetBuffer(0), nCancelLength, 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::GetAgencyResult
//     
// 功能描述：解析代扣的返回结果字符串，形成格式字符串
//     
// 输入参数：CString strXML
// 输出参数：int
// 编写人员：ROCY
// 编写时间：2017年8月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::GetAgencyResult(CString strXML,DWORD dwConsumeTime)
{
	int iReturn = -99;
	CString strTemp = _T("");
	CMarkup xml;
	try
	{
		if( xml.SetDoc( strXML ) )
		{
			if( !xml.FindElem(_T("DebitResultContext")) )
			{
				throw -2;
			}
			xml.IntoElem();
			xml.ResetMainPos();

			if( !xml.FindElem(_T("Data")) )
			{
				throw -3;
			}
			xml.IntoElem();

			xml.ResetMainPos();
			if(!xml.FindElem(_T("ResultType")))
				throw -11;
			m_sCurrentDebitResult.ResultType = 0;

			xml.ResetMainPos();
			if(!xml.FindElem(_T("ExecuteCode")))
				throw -12;
			m_sCurrentDebitResult.ExecuteCode = 0;

			xml.ResetMainPos();
			if(!xml.FindElem(_T("ExecuteDesc")))
				throw -13;
			::_stprintf(m_sCurrentDebitResult.ExecuteDesc, _T("%s"), xml.GetData() );

			if( !xml.FindElem(_T("TradeKeyItem")) )
			{
				throw -14;
			}
			xml.IntoElem();

			xml.ResetMainPos();
			if(!xml.FindElem(_T("TradeSecurityCode")))
				throw -15;
			::_stprintf(m_sCurrentDebitResult.TradeSecurityCode, _T("%s"), xml.GetData() );

			xml.ResetMainPos();
			if(!xml.FindElem(_T("PayPlatformType")))
				throw -16;
			int iPayChennelType = _ttoi(xml.GetData());
			//判断是否代扣支付
			switch(iPayChennelType)
			{
			case 0x07:
				m_sCurrentDebitResult.PayPlatformType = PCTC_ALIIPAY_AGENCY;
				break;
			case 0x08:
				m_sCurrentDebitResult.PayPlatformType = PCTC_WEIXIN_AGENCY;
				break;
			case 0x09:
				m_sCurrentDebitResult.PayPlatformType = PCTC_BAIDU_AGENCY;
				break;
			case 0x0A:
				m_sCurrentDebitResult.PayPlatformType = PCTC_JD_AGENCY;
				break;
			case 0x0B:
				m_sCurrentDebitResult.PayPlatformType = PCTC_UNIPAY_AGENCY;
				break;
			}

			//判断是否将代扣支付合并到移动支付原有定义当中
			if(m_bReturnPayChannelCodeUnify)
			{
				switch(m_sCurrentDebitResult.PayPlatformType)
				{
				case PCTC_WEIXIN_AGENCY:
					m_sCurrentDebitResult.PayPlatformType = PCTC_WENXIN;
					break;
				case PCTC_ALIIPAY_AGENCY:
					m_sCurrentDebitResult.PayPlatformType = PCTC_ALIIPAY;
					break;
				case PCTC_BAIDU_AGENCY:
					m_sCurrentDebitResult.PayPlatformType = PCTC_BAIDU;
					break;
				case PCTC_JD_AGENCY:
					m_sCurrentDebitResult.PayPlatformType = PCTC_JD;
					break;
				case PCTC_UNIPAY_AGENCY:
					m_sCurrentDebitResult.PayPlatformType = PCTC_UNIPAY_QR;
					break;
				}
			}

			xml.ResetMainPos();
			if(!xml.FindElem(_T("PayIdentifier")))
				throw -17;
			::_stprintf(m_sCurrentDebitResult.PayIdentifier, _T("%s"), xml.GetData() );

			xml.ResetMainPos();
			if(!xml.FindElem(_T("DebitOrder")))
				throw -18;
			::_stprintf(m_sCurrentDebitResult.DebitOrder, _T("%s"), xml.GetData() );

			xml.ResetMainPos();
			if(!xml.FindElem(_T("DebitTime")))
				throw -19;
			m_sCurrentDebitResult.DebitTime = CString2time_t( xml.GetData() );

			//获取代扣消耗时间
			m_sCurrentDebitResult.ConsumeTime = dwConsumeTime;

			//其它数据
			m_sCurrentDebitResult.ConsumeTime	= (::GetTickCount() - m_dwTradeBeginOvertime);
			m_sCurrentDebitResult.Valid			= TRUE;

			//标识扣费流程完成
			GetGlobalApp()->m_bIsTrading		= FALSE;
			GetGlobalApp()->m_bCurrentDebitMode = FALSE;
			//返回代码
			iReturn = 0;
		}
		else
		{
			throw -1;
		}
	}
	catch(int& iErrorCode)
	{
		iReturn = iErrorCode;
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败，加载XML串异常"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败，未找到XML头 DebitResultContext"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据区 Data"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 ResultType"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 ExecuteCode"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 ExecuteDesc"));
			break;
		case -14:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据区 TradeKeyItem"));
			break;
		case -15:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 TradeSecurityCode"));
			break;
		case -16:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 PayPlatformType"));
			break;
		case -17:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 PayIdentifier"));
			break;
		case -18:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 DebitOrder"));
			break;
		case -19:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未找到指定数据项 DebitTime"));
			break;

		default:
			m_strLastErrorDesc.Format(_T("代扣结果解析失败, 未知异常"));
			break;
		}
		RecordLog( m_strLastErrorDesc );
	}

	return iReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::Hex2Bin
//     
// 功能描述：将16进制的字符串转换成2进制的数字 
//     
// 输入参数：IN LPTSTR lpHex	-- 需要转换的16进制字符串
//         ：OUT PBYTE pBin		-- 输出的2进制数字数组
//         ：IN DWORD dwHex_len	-- 需要转换的16进制数据长度,必须为2的倍数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：所输入的16进制字符串长度必须是2的倍数
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::Hex2Bin(IN LPTSTR lpHex,OUT PBYTE pBin,IN DWORD dwHex_len)
{
	if(dwHex_len%2)
		return;

	CString strTemp(lpHex);	
	strTemp.MakeUpper();

	USES_CONVERSION;
	for(int i=0;i<strTemp.GetLength()/2;i++)
	{
		BYTE a = (BYTE)(strTemp.GetAt(i*2)>='A')?(BYTE)(strTemp.GetAt(i*2)-'A'+10):(BYTE)(strTemp.GetAt(i*2)-'0');
		BYTE b = (BYTE)(strTemp.GetAt(i*2+1)>='A')?(BYTE)(strTemp.GetAt(i*2+1)-'A'+10):(BYTE)(strTemp.GetAt(i*2+1)-'0');
		pBin[i] = (BYTE)(a * 0x10 + b);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::Bin2Hex
//     
// 功能描述：将2进制的数字转换成16进制的字符串
//     
// 输入参数：IN PBYTE pBin			-- 需要转换的2进制数字数组
//         ：IN DWORD dwBin_Len		-- 需要转换的2进制数字数组长度
// 输出参数：CString  非空 -- 转换成功后输出的16进制字符串;NULL -- 转换失败
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len)
{
	CString strHex;
	CString strTemp;
	for ( DWORD i = 0; i < dwBin_Len; ++i )
	{
		strTemp.Format(_T("%02X"), pBin[i] );
		strHex += strTemp;
	}
	return strHex;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::HexToBase64
//     
// 功能描述：转换16进制字符串为base64编码
//     
// 输入参数：IN const CString& strSrcHex
//         ：OUT CString& strDesBase64
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：内部接口
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTWSDNetPay_DllApp::HexToBase64(IN const CString& strSrcHex,OUT CString& strDesBase64)
{
	BOOL bReturn = FALSE;
	try
	{
		if(0 == strSrcHex.GetLength())
			throw -1;

		if(0  != (strSrcHex.GetLength() % 2))
			throw -1;

		unsigned int uSize = strSrcHex.GetLength();
		BYTE *pTemp = new BYTE[uSize/2 + 1];
		memset( pTemp, 0, (uSize/2 + 1)*sizeof(BYTE) );
		for(unsigned int i = 0; i < uSize; i+=2)
		{
			unsigned char tempC = 0,bcd = 0;
			tempC = strSrcHex.GetAt(i);
			if(tempC >= 'a' && tempC <= 'f') tempC = tempC -'a'+'A';
			bcd = ( tempC >= 'A' ? tempC - 'A' + 0x0a:tempC - '0') << 4;
			tempC = strSrcHex.GetAt(i+1);
			if(tempC >= 'a' && tempC <= 'f') tempC = tempC -'a'+'A';
			bcd += tempC >= 'A' ? tempC - 'A' + 0x0a:tempC - '0';
			pTemp[i/2] = bcd;
		}
		char *pOutBuffer = new char[uSize];
		int  iOutDataLen = 0x00;
		memset(pOutBuffer,0, uSize);

		iOutDataLen = BASE64_Encode(pTemp, uSize/2, pOutBuffer);

		pOutBuffer[iOutDataLen] = 0;

		strDesBase64.Format("%s",pOutBuffer);

		//清理资源
		if(NULL != pOutBuffer) 
			delete []pOutBuffer;
		pOutBuffer = NULL; 

		if(NULL != pTemp) 
			delete []pTemp;
		pTemp = NULL;

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::Base64ToHex
//     
// 功能描述：转换base64编码为16进制字符
//     
// 输入参数：IN const CString& strSrcBase64
//         ：OUT CString& strDesHex
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年9月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTWSDNetPay_DllApp::Base64ToHex(IN const CString& strSrcBase64,OUT CString& strDesHex)
{
	BOOL bReturn = FALSE;
	try
	{
		//判断长度
		if( 0 != (strSrcBase64.GetLength()%2))
			throw -1;
		if(0 == strSrcBase64.GetLength())
			throw -2;

		char szSrc[5120] = {0};
		char szDes[5120] = {0};
		CString strTemp(strSrcBase64);
		strcpy_s(szSrc,_countof(szSrc),strTemp.GetBuffer(0));strTemp.ReleaseBuffer();

		BYTE input[4] = {0};
		int iSrcLen = strSrcBase64.GetLength();
		int i = 0;
		int index = 0;
		for(i = 0; i < iSrcLen; i += 4)
		{
			//byte[0]
			input[0] = Decode_GetByte(szSrc[i]);
			input[1] = Decode_GetByte(szSrc[i + 1]);
			szDes[index++] = (input[0] << 2) + (input[1] >> 4);
	        
			//byte[1]
			if(szSrc[i + 2] != '=')
			{
				input[2] = Decode_GetByte(szSrc[i + 2]);
				szDes[index++] = ((input[1] & 0x0f) << 4) + (input[2] >> 2);
			}

			//byte[2]
			if(szSrc[i + 3] != '=')
			{
				input[3] = Decode_GetByte(szSrc[i + 3]);
				szDes[index++] = ((input[2] & 0x03) << 6) + (input[3]);
			}            
		}

		//null-terminator
		szDes[index] = 0;

		strDesHex = Bin2Hex((BYTE*)szDes,index);

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
	}
    return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::BASE64_Encode
//     
// 功能描述：数据转换辅助函数
//     
// 输入参数：const BYTE* inputBuffer
//         ：INT inputCount
//         ：TCHAR* outputBuffer
// 输出参数：INT
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
INT CTWSDNetPay_DllApp::BASE64_Encode(const BYTE* inputBuffer, INT inputCount, TCHAR* outputBuffer)
{
	INT  i  = 0x00;
	BYTE b0 = 0x00;
	BYTE b1 = 0x00;
	BYTE b2 = 0x00;

	CHAR* DATA_BIN2ASCII = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	if ((inputBuffer == NULL) || (inputCount < 0))
	{
		return -1;  // 参数错误
	}

	if (outputBuffer != NULL)
	{
		for (i = inputCount; i > 0; i -= 3)
		{
			if (i >= 3)
			{  // 将3字节数据转换成4个ASCII字符
				b0 = *inputBuffer++;
				b1 = *inputBuffer++;
				b2 = *inputBuffer++;

				*outputBuffer++ = DATA_BIN2ASCII[b0 >> 2];
				*outputBuffer++ = DATA_BIN2ASCII[((b0 << 4) | (b1 >> 4)) & 0x3F];
				*outputBuffer++ = DATA_BIN2ASCII[((b1 << 2) | (b2 >> 6)) & 0x3F];
				*outputBuffer++ = DATA_BIN2ASCII[b2 & 0x3F];
			}
			else
			{
				b0 = *inputBuffer++;
				if (i == 2)b1 = *inputBuffer++; else b1 = 0;

				*outputBuffer++ = DATA_BIN2ASCII[b0 >> 2];
				*outputBuffer++ = DATA_BIN2ASCII[((b0 << 4) | (b1 >> 4)) & 0x3F];
				*outputBuffer++ = (i == 1) ? TEXT('=') : DATA_BIN2ASCII[(b1 << 2) & 0x3F];
				*outputBuffer++ = TEXT('=');
			}
		} // End for i

		*outputBuffer++ = TEXT('/0');  // 添加字符串结束标记
	}

	return ((inputCount + 2) / 3) * 4;  // 返回有效字符个数
}

BYTE CTWSDNetPay_DllApp::Decode_GetByte(char c)
{
    if(c == '+')
        return 62;
    else if(c == '/')
        return 63;
    else if(c <= '9')
        return (BYTE)(c - '0' + 52);
    else if(c == '=')
        return 64;
    else if(c <= 'Z')
        return (BYTE)(c - 'A');
    else if(c <= 'z')
        return (BYTE)(c - 'a' + 26);
    return 64;
}