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
tagPayKeyItems	CTWSDNetPay_DllApp::m_sCurrentDebitInfo							= {0};
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

CTWSDNetPay_DllApp::CTWSDNetPay_DllApp()
{

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
	COleDateTime t1 = (time_t)0;
	COleDateTime t2 = dTime;
	tSpan = t2 - t1;
	return time_t(tSpan.GetTotalSeconds());
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
time_t CTWSDNetPay_DllApp::CString2time_t(LPCTSTR lpstime)
{
//	CString strTemp(_T("2017-03-31 16:26:15"));

	CString strTemp(lpstime);
	COleDateTime oleGetTime;
	oleGetTime.ParseDateTime(strTemp);
	return OleTimeToTime(oleGetTime.m_dt);
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
	return TRUE;
}

int CTWSDNetPay_DllApp::ExitInstance()
{
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
		m_sTranstParam.LaneConfigInfo.LaneID = _ttoi(szLoaclLaneID);

		strTemp.Format(_T("%s"),szAreaInfo);
		if(-1 == strTemp.Find(_T("#")))
			throw -9;
		m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));
		CString strRoadID = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

		strTemp.Format(_T("%s"),szLoaclStation);
		if(-1 == strTemp.Find(_T("#")))
			throw -10;
		m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));

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
						strSTID.Format(_T("%04d%03d%02d"),m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID);
						m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strSTID);
					}
					break;
				}
			}
			break;
		}

		CString strSTName = strTemp.Right(strTemp.GetLength()-strTemp.Find(_T("#"))-1);
		_tcscpy_s(m_sTranstParam.LaneConfigInfo.StationTab,_countof(m_sTranstParam.LaneConfigInfo.StationTab),strSTName.GetBuffer(0));
		strSTName.ReleaseBuffer();
		_tcscpy_s(m_sTranstParam.PathCommDir,_countof(m_sTranstParam.PathCommDir),m_strWorkDir.GetBuffer(0));
		m_strWorkDir.ReleaseBuffer();

		//获取内网反射服务器IP地址
		char szTempIP[256]={0};
		CString strReflectWebFilePath;
		strReflectWebFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		::GetPrivateProfileString(_T("ReflectWeb"),_T("IP"),_T("101.200.214.203"),szTempIP,_countof(szTempIP),strReflectWebFilePath);
		_tcscpy_s(m_sTranstParam.LaneConfigInfo.WebServerIP,_countof(m_sTranstParam.LaneConfigInfo.WebServerIP),szTempIP);

		//判断是否已经初始化
		if( m_bIsInitEnvironment )
			throw -11;

		RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%d)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));

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
		if(NULL != m_pThreadManage && !(m_pThreadManage->m_bExit))
		{
			//设置该线程推出标记为真
			m_pThreadManage->m_bExit = TRUE;
			//沉默50毫秒
			::Sleep(50);

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
// 功能描述：获取用户信息 -- 扣款结果信息
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


		if(strDataString.IsEmpty())
			throw -7;
		RecordLog(FmtStr(_T("[主进程响应],获取用户信息接口(IF_GetAccountInfo)输出数据:%s"),strDataString));
		int tmpLen = strDataString.GetLength();
		iParamSize = tmpLen>=iParamSize?(iParamSize-1):tmpLen;

		memcpy(szParamContext,strDataString.GetBuffer(0),iParamSize);
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
	CString strOperationMode;
	CString strDataString;
	CString strAnalyError;
	int nFType;

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
		nFType = iFormatType; //底层使用json格式传输 （1.xml 2.json）

		if(1 == nFType)
		{
			CDVXMLDebitParamContext xml;

			if(!xml.Init(strDataString))
			{
				strAnalyError = xml.m_strLastErrorDesc;
				throw -4;
			}

			//复制数据
			m_sCurrentDebitInfo.OverTime			= xml.OverTime;
			m_sCurrentDebitInfo.OperationMode		= xml.OperationMode;
			m_sCurrentDebitInfo.PayCertificateCode	= xml.PayCertificateCode;
			m_sCurrentDebitInfo.PayCertificateType	= xml.PayCertificateType;
			m_sCurrentDebitInfo.PayIdentifier		= xml.PayIdentifier;
			m_sCurrentDebitInfo.Money				= xml.Money;
			m_sCurrentDebitInfo.SubTime				= xml.SubTime;
			m_sCurrentDebitInfo.DebitMainType		= xml.DebitMainType;
			m_sCurrentDebitInfo.VehicleType			= xml.VehicleType;
			m_sCurrentDebitInfo.VehicleClass		= xml.VehicleClass;
			m_sCurrentDebitInfo.VLColor				= xml.VLColor;
			m_sCurrentDebitInfo.VehicleLicense		= xml.VehicleLicense;
			m_sCurrentDebitInfo.AxisCount			= xml.AxisCount;
			m_sCurrentDebitInfo.TotalWeight			= xml.Weight;
			m_sCurrentDebitInfo.LimitWeight			= xml.LimitWeight;
			m_sCurrentDebitInfo.PassCertificateType = xml.PassCertificateType;
			m_sCurrentDebitInfo.CardID				= xml.CardID;
			m_sCurrentDebitInfo.TollDate			= xml.TollDate;
			m_sCurrentDebitInfo.ShiftID				= xml.ShiftID;
			m_sCurrentDebitInfo.EntryStationID		= xml.EntryStationID;
			m_sCurrentDebitInfo.EntryStationName	= xml.EntryStationName;
			m_sCurrentDebitInfo.EntryTime			= xml.EntryTime;
			m_sCurrentDebitInfo.EntryLane			= xml.EntryLane;
			m_sCurrentDebitInfo.EntryVehicleLicense = xml.EntryVehicleLicense;
			m_sCurrentDebitInfo.EntryVLColor		= xml.EntryVLColor;
			m_sCurrentDebitInfo.EntryVehicleType	= xml.EntryVehicleType;
			m_sCurrentDebitInfo.EntryVehicleClass	= xml.EntryVehicleClass;
			m_sCurrentDebitInfo.EntryRoadID			= xml.EntryRoadID;
			m_sCurrentDebitInfo.EntryAreaID			= xml.EntryAreaID;
			m_sCurrentDebitInfo.TicketNo			= xml.TicketNo;
			m_sCurrentDebitInfo.Valid				= TRUE;
		}
		else
		{
			CDVJsonParamDebitMoney* pDebitMoney = NULL;
			CJsonParamOperate jsonParamOperation;

			pDebitMoney = (CDVJsonParamDebitMoney*)jsonParamOperation.FormatResponseResult2Class(strDataString,JSON_DT_PARAM_DEBIT_MONEY);
			if(NULL == pDebitMoney)
			{
				strAnalyError.Format(_T(""));
				throw -5;
			}
			//复制数据
			m_sCurrentDebitInfo.OverTime			= pDebitMoney->OverTime;
			m_sCurrentDebitInfo.OperationMode		= pDebitMoney->OperationMode;
			m_sCurrentDebitInfo.PayCertificateCode	= pDebitMoney->PayCertificateCode;
			m_sCurrentDebitInfo.PayCertificateType	= pDebitMoney->PayCertificateType;
			m_sCurrentDebitInfo.PayIdentifier		= pDebitMoney->PayIdentifier;
			m_sCurrentDebitInfo.Money				= pDebitMoney->Money;
			m_sCurrentDebitInfo.SubTime				= pDebitMoney->SubTime;
			m_sCurrentDebitInfo.DebitMainType		= pDebitMoney->DebitMainType;
			m_sCurrentDebitInfo.VehicleType			= pDebitMoney->VehicleType;
			m_sCurrentDebitInfo.VehicleClass		= pDebitMoney->VehicleClass;
			m_sCurrentDebitInfo.VLColor				= pDebitMoney->VLColor;
			m_sCurrentDebitInfo.VehicleLicense		= pDebitMoney->VehicleLicense;
			m_sCurrentDebitInfo.AxisCount			= pDebitMoney->AxisCount;
			m_sCurrentDebitInfo.TotalWeight			= pDebitMoney->Weight;
			m_sCurrentDebitInfo.LimitWeight			= pDebitMoney->LimitWeight;
			m_sCurrentDebitInfo.PassCertificateType = pDebitMoney->PassCertificateType;
			m_sCurrentDebitInfo.CardID				= pDebitMoney->CardID;
			m_sCurrentDebitInfo.TollDate			= pDebitMoney->TollDate;
			m_sCurrentDebitInfo.ShiftID				= pDebitMoney->ShiftID;
			m_sCurrentDebitInfo.EntryStationID		= pDebitMoney->EntryStationID;
			m_sCurrentDebitInfo.EntryStationName	= pDebitMoney->EntryStationName;
			m_sCurrentDebitInfo.EntryTime			= pDebitMoney->EntryTime;
			m_sCurrentDebitInfo.EntryLane			= pDebitMoney->EntryLane;
			m_sCurrentDebitInfo.EntryVehicleLicense = pDebitMoney->EntryVehicleLicense;
			m_sCurrentDebitInfo.EntryVLColor		= pDebitMoney->EntryVLColor;
			m_sCurrentDebitInfo.EntryVehicleType	= pDebitMoney->EntryVehicleType;
			m_sCurrentDebitInfo.EntryVehicleClass	= pDebitMoney->EntryVehicleClass;
			m_sCurrentDebitInfo.EntryRoadID			= pDebitMoney->EntryRoadID;
			m_sCurrentDebitInfo.EntryAreaID			= pDebitMoney->EntryAreaID;
			m_sCurrentDebitInfo.TicketNo			= pDebitMoney->TicketNo;
			m_sCurrentDebitInfo.Valid				= TRUE;

			delete pDebitMoney;
			pDebitMoney = NULL;
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
						strSTID.Format(_T("%04d%03d%02d"),m_sCurrentDebitInfo.EntryAreaID,m_sCurrentDebitInfo.EntryRoadID,m_sCurrentDebitInfo.EntryStationID);
						m_sCurrentDebitInfo.EntryStationID = _ttoi(strSTID);
					}
					break;
				}
			}
			break;
		}

		//检测业务管理线程是否正常
		if(NULL == m_pThreadManage)
			throw -6;

		//判断上次交易是否完成
		if(m_bIsTrading)
			throw -7;

		//标记当前交易正在进行
		m_bIsTrading = TRUE;

		/*****************************响应操作***********************************/

		strOperationMode.Format(_T("整合模式"));
		m_bCurrentDebitMode = TRUE;


		//进行资源申请
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -8;

		//重构扣款JSON/xml
		nFType = 2;
		pstrString->Format(_T("%s"),CostructDebitInfo( nFType, m_sCurrentDebitInfo));

		//关闭上次的定时器
		m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
		//启动扣款超时定时器
		m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,m_sCurrentDebitInfo.OverTime);
		//开始计时
		m_dwTradeBeginOvertime = GetTickCount();
		//启动扣款流程
		m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_START_DEBIT_FLOW,(WPARAM)pstrString,(LPARAM)nFType);//把数据解析类型抛送过去

		//添加描述
		m_strLastErrorDesc.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(%s)"),strOperationMode);
		
		//记录日志
		RecordLog(m_strLastErrorDesc);

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
		//预处理失败，回馈调用者结果
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(m_sTranstParam.CallerWnd))
		{
			::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}
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
			strPayIdentifier.Format(_T("%s"), pDebitCancel->PayIdentifier);

			delete pDebitCancel;
			pDebitCancel = NULL;
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
		//	pstrString->Format(_T("%s"),ConstructCancleInfo(iFormatType,strPayIdentifier));

		m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_DEBIT_CANCEL,(WPARAM)pstrString,(LPARAM)iFormatType);//把解析类型抛送过去

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
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(m_sTranstParam.CallerWnd))
		{
			::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}
		//保存日志
		RecordLog(FmtStr(_T("[主进程响应],撤单操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"),byFunCode,byExecuteState));
	}
	RecordLog(FmtStr(_T("[主进程响应],执行撤单操作接口(IF_DebitCancel)调用结束")));
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_GetDebitResult
//     
// 功能描述：获取扣款
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
// 备注说明：
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
		//判断参数是否异常
		if((iFormatType>2) || (iFormatType<1) || (NULL == szParamContext))
			throw -2;

		//检测业务管理线程是否正常
		if(NULL == m_pThreadManage)
			throw -3;
		if(NULL == m_pThreadManage->m_pThreadDevice)
			throw -4;

		CString strResult;
		WORD wCmdType = MAKEWORD(0x01,0x03);
		m_pThreadManage->GetHKResult(wCmdType,strResult);
		if(strResult.IsEmpty())
			throw -5;

		if(strResult.GetLength()>(iParamSize+1))
			throw -6;

		//把数据转成需要的格式
		ConverToFormatData(iFormatType,strResult);


		strResult.TrimRight();
		RecordLog(FmtStr(_T("[主进程响应],获取扣款结果接口(IF_GetDebitResult)输出结果如下:(%s)%s "),(0x01 == iFormatType)?_T("数据格式:XML"):_T("数据格式:JSON"),strResult));

		_tcscpy_s(szParamContext,strResult.GetLength()+1,strResult.GetBuffer(0));
		strResult.ReleaseBuffer();

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

	int nFormateType = iFormatType;
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


		BOOL bIsClearCmd;
		ConstructMMIInfo(iFormatType,strParamString,bIsClearCmd);

		if(strParamString.IsEmpty())
			throw -9;

		if(bIsClearCmd)
			nFormateType = 0x01;

		//获取数据
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -10;
		pstrString->Format(_T("%s"),strParamString);

		m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO,(WPARAM)pstrString,(LPARAM)nFormateType);

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
		case -9:
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
		//终端设备状态/网络状态
		CString strState;
		if(m_pThreadManage->GetHKResult(1,strState))
		{
			dwState = _ttoi(strState);
		}

		////支付子平台网络状态
		//if(m_bNetErrorHttpServerInTollNetNotExist)
		//{
		//	dwState |= DEVICE_STATE_PAY_WEB;
		//}
		////支付子平台服务器状态
		//if(m_bNetErrorHttpServerTrawePlatformNotExist)
		//{
		//	dwState |= DEVICE_STATE_PAY_SERVER;
		//}
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



//test c0 frame count
bool WINAPI CTWSDNetPay_DllApp::IF_GetC0SuccFaileCount(OUT int& nSucce, OUT int& nFailed, OUT int& nReSendCount, OUT char* strA2Frame, OUT char* strC0Frame)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	CString tmpstrA2Frame;
	CString tmpstrC0Frame;
	//	RecordLog(FmtStr(_T("[主进程响应],C0帧统计接口(IF_GetC0SuccFaileCount)调用开始"));
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

		bReturn = m_pThreadManage->m_pThreadDevice->GetC0SuccFailedCount(nSucce, nFailed,nReSendCount,tmpstrA2Frame,tmpstrC0Frame);

		if(!tmpstrA2Frame.IsEmpty())
		{
			strcpy(strA2Frame,tmpstrA2Frame.GetBuffer(0));
			tmpstrA2Frame.ReleaseBuffer();
		}
		if(!tmpstrC0Frame.IsEmpty())
		{
			strcpy(strC0Frame,tmpstrC0Frame.GetBuffer(0));
			tmpstrC0Frame.ReleaseBuffer();
		}

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[主进程响应],C0帧统计(IF_GetC0SuccFaileCount)出错,组件未成功初始化"));
			break;
		case -2:
			strLog.Format(_T("[主进程响应],C0帧统计(IF_GetC0SuccFaileCount)出错,组件未被授权"));
			break;
		case -3:
		case -4:
			strLog.Format(_T("[主进程响应],C0帧统计(IF_GetC0SuccFaileCount)出错,组件内部资源异常"));
			break;
		}
		//保存错误描述
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
		}
	}
	//	RecordLog(FmtStr(_T("[主进程响应],检测组件状态接口(IF_GetC0SuccFaileCount)调用结束")));

	return bReturn;
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
	m_dwThreadCheckSpanTime = ::GetPrivateProfileIntA(_T("Setup"),_T("ThreadCheckSpanTime"),300,strConfigFilePath);
	//获取虚拟卡前缀
	::GetPrivateProfileString(_T("Setup"),_T("VirtualCardPrefix"),_T("4412"),szTemp,256,strConfigFilePath);
	m_strVirtualCardPrefix.Format(_T("%s"),szTemp);
	//获取往智能硬件下发时间的间隔
	m_dwUpdateDevieTimeSpanTime = (DWORD)::GetPrivateProfileIntA(_T("Setup"),_T("UpdateDeviceTimeSpanTime"),30*60,strConfigFilePath);
	m_dwUpdateDevieTimeSpanTime *= 1000;
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
				if(::PeekMessage(&mMsg,NULL,0,0,PM_NOREMOVE))
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

CString CTWSDNetPay_DllApp::CostructDebitInfo(int nFormatType, tagPayKeyItems& pkItems)
{
	CString strReturn;
	if(1 == nFormatType) //xml
	{

		CMarkup xmlDebit;
		xmlDebit.SetDoc(_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"));
		xmlDebit.AddElem(_T("DebitParamContext")); //root
		xmlDebit.IntoElem();

		xmlDebit.AddElem( _T("Version"),	_T("1.0") );

		xmlDebit.AddElem( _T("Data") );
		xmlDebit.IntoElem();
		xmlDebit.AddElem( _T("OverTime"), pkItems.OverTime );
		xmlDebit.AddElem( _T("OperationMode"), pkItems.OperationMode );

		xmlDebit.AddElem( _T("Debit") );
		xmlDebit.IntoElem();
		xmlDebit.AddElem( _T("PayIdentifier"), pkItems.PayIdentifier );
		xmlDebit.AddElem( _T("Money"), pkItems.Money );
		xmlDebit.AddElem( _T("SubTime"), pkItems.SubTime.Format(_T("%Y-%m-%d %H:%M:%S")) );
		xmlDebit.AddElem( _T("Type"), pkItems.DebitMainType );
		xmlDebit.AddElem( _T("PayCertificateCode"), pkItems.PayCertificateCode );
		xmlDebit.AddElem( _T("PayCertificateType"), pkItems.PayCertificateType );
		xmlDebit.OutOfElem();

		xmlDebit.AddElem( _T("Vehicle") );
		xmlDebit.IntoElem();
		xmlDebit.AddElem( _T("Type") , pkItems.VehicleType);
		xmlDebit.AddElem( _T("Class") , pkItems.VehicleClass);
		xmlDebit.AddElem( _T("License") , pkItems.VehicleLicense);
		xmlDebit.AddElem( _T("VLColor") , pkItems.VLColor);
		xmlDebit.AddElem( _T("AxisCount") ,pkItems.AxisCount);
		xmlDebit.AddElem( _T("Weight") ,pkItems.TotalWeight);
		xmlDebit.AddElem( _T("LimitWeight") ,pkItems.LimitWeight);
		xmlDebit.OutOfElem();

		xmlDebit.AddElem( _T("Operation") );
		xmlDebit.IntoElem();
		xmlDebit.AddElem( _T("PassCertificateType") ,pkItems.PassCertificateType);
		xmlDebit.AddElem( _T("CardID") ,pkItems.CardID);
		xmlDebit.AddElem( _T("TollDate") ,pkItems.TollDate);
		xmlDebit.AddElem( _T("ShiftID") ,pkItems.ShiftID);
		xmlDebit.AddElem( _T("TicketNo") ,pkItems.TicketNo);
		xmlDebit.OutOfElem();

		xmlDebit.AddElem( _T("Entry") );
		xmlDebit.IntoElem();
		xmlDebit.AddElem( _T("StationID") ,pkItems.EntryStationID);
		xmlDebit.AddElem( _T("StationName") ,pkItems.EntryStationName);
		xmlDebit.AddElem( _T("Time") ,pkItems.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		xmlDebit.AddElem( _T("LaneID") ,pkItems.EntryLane);
		xmlDebit.AddElem( _T("License") ,pkItems.EntryVehicleLicense);
		xmlDebit.AddElem( _T("VColor") ,pkItems.EntryVLColor);
		xmlDebit.AddElem( _T("VClass") ,pkItems.EntryVehicleClass);
		xmlDebit.AddElem( _T("VType") ,pkItems.EntryVehicleType);
		xmlDebit.AddElem( _T("RoadID") ,pkItems.EntryRoadID);
		xmlDebit.AddElem( _T("AreaID") ,pkItems.EntryAreaID);
		xmlDebit.OutOfElem();

		xmlDebit.OutOfElem();
		xmlDebit.OutOfElem();
		strReturn = xmlDebit.GetDoc();

	}
	else   //重构扣款json
	{

		//表示整个json对象
		Json::Value 	root;

		Json::Value		DebitParamContext;

		Json::Value		Data;
		Json::Value		Version;

		Json::Value		Debit;
		Json::Value		Entry;
		Json::Value		Operation;
		Json::Value		OperationMode;
		Json::Value		OverTime;
		Json::Value		Vehicle;

		/***********************************************************构造JSAO数据结构***********************************************************/	
		Debit["Money"] =					Json::Value(pkItems.Money);
		Debit["PayCertificateCode"] =		Json::Value(pkItems.PayCertificateCode) ;
		Debit["PayCertificateType"] =		Json::Value(pkItems.PayCertificateType) ;
		Debit["PayIdentifier"] =			Json::Value(pkItems.PayIdentifier) ;
		Debit["SubTime"] =					Json::Value(pkItems.SubTime.Format(_T("%Y-%m-%d %H:%M:%S"))) ;//Format(_T("%Y-%m-%d %H:%M:%S"))
		Debit["Type"] =						1;

		Entry["AreaID"] =					Json::Value(pkItems.EntryAreaID) ;
		Entry["LaneID"] =					Json::Value(pkItems.EntryLane) ;
		Entry["License"] =					Json::Value(pkItems.EntryVehicleLicense) ;
		Entry["RoadID"] =					Json::Value(pkItems.EntryRoadID) ;
		Entry["StationID"] =				Json::Value(pkItems.EntryStationID) ;
		Entry["StationName"] =				Json::Value(pkItems.EntryStationName) ;
		Entry["Time"] =						Json::Value(pkItems.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S"))) ;
		Entry["VClass"] =					Json::Value(pkItems.EntryVehicleClass) ;
		Entry["VColor"] =					Json::Value(pkItems.EntryVLColor );
		Entry["VType"] =					Json::Value(pkItems.EntryVehicleType) ;

		Operation["CardID"] =				Json::Value(pkItems.CardID) ;
		Operation["PassCertificateType"] =  Json::Value(pkItems.PassCertificateType) ;
		Operation["ShiftID"] =				Json::Value(pkItems.ShiftID );
		Operation["TollDate"] =				Json::Value(pkItems.TollDate );

		Vehicle["AxisCount"] =				Json::Value(pkItems.AxisCount) ;
		Vehicle["Class"] =					Json::Value(pkItems.VehicleClass) ;
		Vehicle["License"] =				Json::Value(pkItems.VehicleLicense );
		Vehicle["Type"] =					Json::Value(pkItems.VehicleType );
		Vehicle["VLColor"] =				Json::Value(pkItems.VLColor );
		Vehicle["Weight"] =					Json::Value(pkItems.TotalWeight) ;


		Data["Debit"] = Debit;
		Data["Entry"] = Entry;
		Data["Operation"] = Operation;
		Data["OperationMode"] =				Json::Value(pkItems.OperationMode);
		Data["OverTime"] =					Json::Value(pkItems.OverTime);
		Data["Vehicle"] = Vehicle;

		DebitParamContext["Version"] =		Json::Value(_T("1.0"));
		DebitParamContext["Data"] = Data;
		root["DebitParamContext"] = DebitParamContext;

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());

	}

	return strReturn;
}

CString CTWSDNetPay_DllApp::ConstructCancleInfo(int nFormatType,CString strPayID)
{
	CString strReturn;

	if(1 == nFormatType)
	{
		CMarkup xml;
		xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"GB2312\" ?> \r\n"));
		xml.AddElem(_T("DebitCancelParamContext")); 
		xml.IntoElem();//root
		xml.AddElem(_T("Version"), _T("1.0") );
		xml.AddElem(_T("Data"));
		xml.IntoElem();//Data
		xml.AddElem(_T("PayIdentifier"),strPayID);
		xml.OutOfElem();//Data
		xml.OutOfElem();//root

		strReturn = xml.GetDoc();

	}
	else
	{
		Json::Value root;
		Json::Value DebitCancelParamContext;
		Json::Value Data;

		Data["PayIdentifier"] = Json::Value(strPayID);

		DebitCancelParamContext["Data"] = Data;
		DebitCancelParamContext["Version"] = Json::Value(_T("1.0"));

		root["DebitCancelParamContext"] = DebitCancelParamContext;

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());

	}

	return strReturn;
}

void CTWSDNetPay_DllApp::ConverToFormatData(int iFormatType,IN OUT CString& strResult)
{
	CString strDataString;
	CDVJsonParamDebitResult* jsonDebitResult = NULL;
	CJsonParamOperate jsonParamOperation;

	try{

		if(strResult.IsEmpty())
			throw -1;

		if(iFormatType>2 || iFormatType<1)
			throw -2;

		jsonDebitResult = (CDVJsonParamDebitResult*)jsonParamOperation.FormatResponseResult2Class(strResult,JSON_DT_PARAM_DEBIT_RESULT);//需要释放资源
		if(NULL == jsonDebitResult)
			throw -3;

		switch(iFormatType)
		{
		case 1:
			{
				CDVXMLDebitResult xml;
				xml.ResultType		= jsonDebitResult->ResultType;
				xml.ExecuteCode		= jsonDebitResult->ExecuteCode;
				xml.PayPlatformType = jsonDebitResult->PayPlatformType;
				xml.DebitTime		= jsonDebitResult->DebitTime;
				xml.ConsumeTime		= jsonDebitResult->ConsumeTime;
				xml.ExecuteDesc.Format(_T("%s"),jsonDebitResult->ExecuteDesc);
				xml.TradeSecurityCode.Format(_T("%s"),jsonDebitResult->TradeSecurityCode);
				xml.PayIdentifier.Format(_T("%s"),jsonDebitResult->PayIdentifier);
				xml.DebitOrder.Format(_T("%s"),jsonDebitResult->DebitOrder);

				strDataString = xml.GetXMLString();

				strDataString.Replace(_T("\r"),_T(""));
				strDataString.Replace(_T("\n"),_T(""));
			}
			break;
		case 2:
			{
				strDataString = jsonParamOperation.FormatParamData2Json(jsonDebitResult);
				if(strDataString.IsEmpty())
				{
					throw -4;
				}
			}
			break;
		default:
			break;
		}


		strResult.Format(_T("%s"),strDataString);

		delete jsonDebitResult;
		jsonDebitResult = NULL;


	}
	catch(int& nError)
	{
		switch(nError)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		case -4:
			break;
		}
	}
}


BOOL CTWSDNetPay_DllApp::ConstructMMIInfo(int nFormateType, IN OUT CString& strResult,OUT BOOL& bClearCmd)
{
	bClearCmd = FALSE;
	CString strParamString(strResult);
	if(0x01 == nFormateType)
	{
		//XML格式
		CDVXMLMMI xml;
		if(!xml.Init(strParamString))
		{
			strResult.Empty();
			return FALSE;
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
		if(xml.ClearMMI > 0)
		{
			bClearCmd = TRUE;
			int iMMIType = 0x04;
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
			for(int i = 0;i<xml.Text.GetSize();++i)
			{
				int iTemp = 0x00;
				switch(xml.Text.GetAt(i).Sub)
				{
				case 1://车型   Vtype 0客 1货   //车种 Vclass  01~FF
					{
						iTemp = _ttoi(xml.Text.GetAt(i).Context);

						if(iTemp>10)
						{
							iVType = 0x00;
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
		}

		xmlShow.OutOfElem();
		xmlShow.OutOfElem();

		strResult = xmlShow.GetDoc();

	}
	else 
	{
		//JSON格式
		CDVJsonParamMMI* pJsonMMI = NULL;
		CJsonParamOperate jsonOperation;

		pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString,JSON_DT_PARAM_MMI); //需要释放内存
		if(NULL == pJsonMMI)
		{
			strResult.Empty();
			return FALSE;
		}

		Json::Value root;
		Json::Value MMIParamContext;
		Json::Value Data;


		MMIParamContext["Version"]  = Json::Value(pJsonMMI->Version);
		Data["ClearMMI"]	= Json::Value(pJsonMMI->ClearMMI);


		if(pJsonMMI->ClearMMI > 0)
		{
			bClearCmd = TRUE;


			//重构为XML数据
			CMarkup xmlShow;
			xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
			xmlShow.AddElem( _T("DisplayContext") );//root
			xmlShow.IntoElem();

			xmlShow.AddElem( _T("Version"),		_T("1.0") );
			xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
			xmlShow.AddElem( _T("VerifyCode"),	0 );
			xmlShow.AddElem( _T("Data") );

			xmlShow.IntoElem();

			int iMMIType = 0x04;
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


			xmlShow.OutOfElem();
			xmlShow.OutOfElem();

			strResult = xmlShow.GetDoc();



			/***************************************************/

			/*int iMMIType = 0x04;
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

			Data["Type"] = Json::Value(iMMIType);*/

		}
		else
		{
			Data["Type"] = Json::Value(0x01);

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
				case 1://车型   Vtype 0客 1货   //车种 VClass  01~FF
					{
						iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

						if(iTemp>10)
						{
							iVType = 0x00;
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


			Data["VType"]       = Json::Value(iVType);
			Data["VClass"]      = Json::Value(iVClass);
			Data["Money"]       = Json::Value(iMoney);
			Data["Weight"]      = Json::Value(iWeight);
			Data["AxisCount"]   = Json::Value(iAxis);
			Data["LimitWeight"] = Json::Value(iLimit);
			Data["Balance"]     = Json::Value(iBalance);
			Data["StationName"] = Json::Value(strEntrySTName);



			MMIParamContext["Data"] = Data;
			root["MMIParamContext"] = MMIParamContext;

			//转化为字符串
			Json::FastWriter fast_writer;
			strResult.Format(_T("%s"),fast_writer.write(root).c_str());


		}

		delete pJsonMMI;
		pJsonMMI = NULL;

	}


	return TRUE;



}