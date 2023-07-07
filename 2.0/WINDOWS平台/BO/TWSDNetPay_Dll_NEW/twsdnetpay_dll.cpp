// TWSDNetPay_Dll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"

#include "fs_funcs.h"
#include "pystring.h"
#include "string_func.h"
#include "TypeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//唯一全局变量
CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp()  { return &theApp; }

BEGIN_MESSAGE_MAP(CTWSDNetPay_DllApp, CWinApp)
END_MESSAGE_MAP()

/***********************变量初始化*********************************/
CThreadManage*  CTWSDNetPay_DllApp::m_pThreadManage                             = NULL;
BOOL            CTWSDNetPay_DllApp::m_bIsInitEnvironment                        = FALSE;
BOOL            CTWSDNetPay_DllApp:: m_bAuthorizedLegal                         = FALSE;
BOOL            CTWSDNetPay_DllApp::m_bNetErrorHttpServerInTollNetNotExist      = FALSE;
BOOL            CTWSDNetPay_DllApp::m_bNetErrorHttpServerTrawePlatformNotExist  = FALSE;
BOOL            CTWSDNetPay_DllApp::m_bSaveLog                                  = FALSE;
DWORD           CTWSDNetPay_DllApp::m_dwThreadCheckSpanTime                     = 300;
CString         CTWSDNetPay_DllApp::m_strWorkDir                                = _T("");
CString         CTWSDNetPay_DllApp::m_strLastErrorDesc                          = _T("");
CString         CTWSDNetPay_DllApp::m_strVirtualCardPrefix                      = _T("");
tagTranstParam  CTWSDNetPay_DllApp::m_sTranstParam                              = {0};
BOOL            CTWSDNetPay_DllApp::m_bCurrentDebitMode                         = FALSE;
BOOL            CTWSDNetPay_DllApp::m_bIsTrading                                = FALSE;
tagPayKeyItems  CTWSDNetPay_DllApp::m_sCurrentDebitInfo                         = {0};
tagDebitResultItems CTWSDNetPay_DllApp::m_sCurrentDebitResult                   = {0};
DWORD               CTWSDNetPay_DllApp::m_dwTradeBeginOvertime                  = 0;
DWORD               CTWSDNetPay_DllApp::m_dwUpdateDevieTimeSpanTime             = 0;
BOOL                CTWSDNetPay_DllApp::m_bIsTestData                           = FALSE;
CString         CTWSDNetPay_DllApp::m_strNetPayDllVersion                       = _T("");
CString         CTWSDNetPay_DllApp::m_strDeviceDllVersion                       = _T("");
int             CTWSDNetPay_DllApp::m_iDeleteLogStartHour                       = 0;
int             CTWSDNetPay_DllApp::m_iDeleteLogEndHour                         = 0;
int             CTWSDNetPay_DllApp::m_iDeleteLogSpanTime                        = 0;
int             CTWSDNetPay_DllApp::m_iDeleteLogSaveDays                        = 0;
int             CTWSDNetPay_DllApp::m_iDebitTaskNo                              = 0;
int             CTWSDNetPay_DllApp::m_iNotifyMode                               = 1;
NotifyCallerFun CTWSDNetPay_DllApp::m_fnCallbackNotifyFun                       = NULL;
NotifyCallerFun45 CTWSDNetPay_DllApp::m_fnCallbackNotifyFun45                   = NULL;
void*           CTWSDNetPay_DllApp::m_pIndPtrFor45                              = NULL;

time_t CTWSDNetPay_DllApp::m_lastPayTime = 0;

std::string CTWSDNetPay_DllApp::m_CommonInitPara=std::string();

CTWSDNetPay_DllApp::CTWSDNetPay_DllApp()
{
    m_connect_bus_core = NULL;
	m_commonbus_module =NULL;
	m_disconnect_bus_core= NULL;
	m_send_core= NULL;
	m_register_core= NULL;
	m_unRegister_core= NULL;
	m_bushandle_core=0;
//	ConnectBus();
//  this->register2Bus(KEY_BUSINESS);
}
CTWSDNetPay_DllApp::~CTWSDNetPay_DllApp()
{
    this->unRegister2Bus(KEY_BUSINESS);
	DisConnectBus();
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString   FmtStr
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
CString CTWSDNetPay_DllApp::FmtStr(const TCHAR* lpsFmt, ...)
{
    CString str;
    va_list lpzArg = NULL;

    va_start(lpzArg, lpsFmt);
    str.FormatV(lpsFmt, lpzArg);

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
//  CString strTemp(_T("2017-03-31 16:26:15"));

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
    if (S_OK == ::CoCreateGuid(&guid))
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
    if (1 == iType)
    {
        strReturn.Replace(_T("-"), _T(""));
        strReturn.Replace(_T("{"), _T(""));
        strReturn.Replace(_T("}"), _T(""));
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
    if (0 == iTimeType)
    {
        strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    }
    else
    {
        strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
    }
    return strTime;
}

//////////////////////////////////////////////////////////////////////////////////
///函数名称: MakeDirectory()
///功能说明: 创建指定的多级目录
///参数说明: CString dd,用来创建的多级目录路径
///数出说明: TRUE  -- 创建成功
///          FALSE -- 创建失败
///修改时间: 2006-04-12
///修改人员: ROCY
///备    注: 需要包含 windows.h
//////////////////////////////////////////////////////////////////////////////////
BOOL CTWSDNetPay_DllApp::MakeDirectory(CString strCreateDir)
{
    HANDLE  fFile;                      // File Handle
    WIN32_FIND_DATA fileinfo = {0};     // File Information Structure
    CStringArray arr;               // CString Array to hold Directory Structures
    BOOL bSuccess = false;              // BOOL used to test if Create Directory was successful
    int nCount = 0;                     // Counter
    CString strTemp;                    // Temporary CString Object

    fFile = FindFirstFile(strCreateDir, &fileinfo);

    // if the file exists and it is a directory
    if (fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
    {
        //  Directory Exists close file and return
        FindClose(fFile);
        return TRUE;
    }
    arr.RemoveAll();

    for (nCount = 0; nCount < strCreateDir.GetLength(); nCount++ ) // Parse the supplied CString Directory String
    {
        if (strCreateDir.GetAt(nCount) != '\\') // if the Charachter is not a \ 
            strTemp += strCreateDir.GetAt(nCount);  // add the character to the Temp String
        else
        {
            arr.Add(strTemp);   // if the Character is a \ 
            strTemp += _T("\\");   // Now add the \ to the temp string
        }
        if (nCount == strCreateDir.GetLength() - 1) // If we reached the end of the String
            arr.Add(strTemp);
    }

    // Close the file
    FindClose(fFile);

    // Now lets cycle through the String Array and create each directory in turn
    for (nCount = 1; nCount < arr.GetSize(); nCount++)
    {
        strTemp = arr.GetAt(nCount);
        bSuccess = CreateDirectory(strTemp, NULL);

        // If the Directory exists it will return a false
        if (bSuccess)
            SetFileAttributes(strTemp, FILE_ATTRIBUTE_NORMAL);
        // If we were successful we set the attributes to normal
    }
    //  Now lets see if the directory was successfully created
    fFile = FindFirstFile(strCreateDir, &fileinfo);

    arr.RemoveAll();
    if (fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
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
//  作   者 : ROCY
//  时   间 : 2008-06-01
//  版   本 : 1.0.0.1
//  函数名称 : GetVersionOfFile
//  函数功能 : 获取指定的文件的版本号
//  输入参数 : LPCTSTR  --  lpsPath 文件的绝对路径
//  输出参数 : DWORD -- 返回的本地计算机IP子网掩码，字节合成格式
//  备注说明 : 无
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::GetVersionOfFile(LPCTSTR lpsPath)
{
    DWORD   dwLength = 0;
    DWORD   dwNullHandle = 0;
    UINT    uiVerLength;
    LPVOID  lpVersionPtr;
    CString strVer = _T("0.0.0.0");

    dwLength = ::GetFileVersionInfoSize(lpsPath, &dwNullHandle);
    if (dwLength > 0)
    {
        BYTE* pVersionInfo = new BYTE [dwLength];
        if (::GetFileVersionInfo (lpsPath, NULL, dwLength, pVersionInfo))
        {
            if (::VerQueryValue (pVersionInfo, _T ("\\"), &lpVersionPtr, &uiVerLength))
            {
                VS_FIXEDFILEINFO * p = (VS_FIXEDFILEINFO *)lpVersionPtr;
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

    TCHAR szTempDir[260] = {0};
    ::GetModuleFileName(AfxGetInstanceHandle(), szTempDir, sizeof(TCHAR) * 260);
    CString strTemp(szTempDir);
    CString strDir;
    strDir = strTemp.Left(strTemp.ReverseFind('\\'));
    m_strWorkDir = strDir;

    //读取配置文件
    ReadConfigInfo();

    //创建日志目录
    strTemp.Format(_T("%s\\TWSDNetPayLog"), m_strWorkDir);
    _tcscpy_s(m_sTranstParam.PathLogDir, _countof(m_sTranstParam.PathLogDir), strTemp.GetBuffer(0));
    strTemp.ReleaseBuffer();
    MakeDirectory(m_sTranstParam.PathLogDir);
    //获取程序版本
    CString strDllFilePath(szTempDir);
    m_strNetPayDllVersion = GetVersionOfFile(strDllFilePath);

    strDllFilePath.Format(_T("%s\\%s"), strDir, FILE_NAME_DEVICE_DLL);
    m_strDeviceDllVersion = GetVersionOfFile(strDllFilePath);

    //初始化日志系统
    if (m_bSaveLog)
    {
        INIT_LOG(m_sTranstParam.PathLogDir, _T(""));

        RecordLog(FmtStr(_T("\r\n/*************************************程序启动(业务%s - 设备%s)*************************************/"), m_strNetPayDllVersion, m_strDeviceDllVersion));
    }

	ConnectBus();
	this->register2Bus(KEY_BUSINESS);
    return TRUE;
}

int CTWSDNetPay_DllApp::ExitInstance()
{
    this->unRegister2Bus(KEY_BUSINESS);
    QUIT_LOG();
    return CWinApp::ExitInstance();
}




/****************************************对外接口函数********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_InitEnvironment
//
// 功能描述：初始化组件环境
//
// 输入参数：IN const UINT& nThreadID              -- 调用者指定的线程ID
//         ：IN const HWND& hWnd                   -- 调用者指定的窗体句柄
//         ：IN const unsigned int& nNotifyMsgID   -- 调用者指定的通知消息ID
//         ：IN const char* szAreaInfo             -- 区域信息(区域编码#路段号)
//         ：IN const char* szLoaclStation         -- 站点信息(站号#站名)
//         ：IN const char* szLoaclLaneID          -- 车道编码
//         ：IN const char* szServerInfo           -- 服务器信息
//         ：IN const int& iProvinceID             -- 省份编码
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment(IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());


    bool bReturn = false;
    CString strLog;

    RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)调用开始")));
    try
    {
        /*******************检索参数是否正常*******************/
        if (  (0 == nThreadID) && (NULL == hWnd))
            throw - 1;
        if ( 0 == nNotifyMsgID)
            throw - 2;
        if ( (NULL == szAreaInfo) || (0 == _tcslen(szAreaInfo)))
            throw - 3;
        if ( (NULL == szLoaclStation) || (0 == _tcslen(szLoaclStation)))
            throw - 4;
        if ( (NULL == szLoaclLaneID) || (0 == _tcslen(szLoaclLaneID)))
            throw - 5;
        if ( (NULL == szServerInfo) || (0 == _tcslen(szServerInfo)))
            throw - 6;
        if ( 0 == iProvinceID)
            throw - 7;
        if ( (0 == nThreadID) && (!::IsWindow(hWnd)))
            throw - 8;
        //填充配置信息
        TCHAR   szTemp[256] = {0};
        CString strTemp;
        CString strSTID;

        m_sTranstParam.ProvinceID            = iProvinceID;
        m_sTranstParam.ActiveNotifyMsgID     = nNotifyMsgID;
        m_sTranstParam.CallerThreadID        = nThreadID;
        m_sTranstParam.CallerWnd             = hWnd;
        m_sTranstParam.LaneConfigInfo.LaneID = _ttoi(szLoaclLaneID);

        strncpy(m_sTranstParam.LaneConfigInfo.str_LaneID , szLoaclLaneID, 128);

        strTemp.Format(_T("%s"), szAreaInfo);
        if (-1 == strTemp.Find(_T("#")))
            throw - 9;
        m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));
		strncpy(m_sTranstParam.LaneConfigInfo.str_AreaID,strTemp.Left(strTemp.Find(_T("#"))),128);
        CString strRoadID = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("#")) - 1);

        strncpy(m_sTranstParam.LaneConfigInfo.str_RoadID, strRoadID, 128);
        m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

        strTemp.Format(_T("%s"), szLoaclStation);
        if (-1 == strTemp.Find(_T("#")))
            throw - 10;
        m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));

        strncpy(m_sTranstParam.LaneConfigInfo.str_StationID, strTemp.Left(strTemp.Find(_T("#"))).GetBuffer(),128);
        strSTID.Format(_T("%010d"), m_sTranstParam.LaneConfigInfo.StationID);

        //特殊处理代码区域
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的出口站号特殊处理
            case 30:
            case 31:
            default:
            {
                strSTID.Format(_T("%04d%03d%02d"), m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, m_sTranstParam.LaneConfigInfo.StationID);
                m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strSTID);
				
				CString Area_zero_str = m_sTranstParam.LaneConfigInfo.str_AreaID;
				if(Area_zero_str.GetLength() < 4)
				{
					Area_zero_str = CString('0',4-Area_zero_str.GetLength())+Area_zero_str;
				}
				CString Road_zero_str = m_sTranstParam.LaneConfigInfo.str_RoadID;
				if(Road_zero_str.GetLength() < 3)
				{
					Road_zero_str = CString('0',3-Road_zero_str.GetLength())+Road_zero_str;
				}
				CString Station_zero_str = m_sTranstParam.LaneConfigInfo.str_StationID;
				if(Station_zero_str.GetLength() < 2)
				{
					Station_zero_str = CString('0',2-Station_zero_str.GetLength())+Station_zero_str;
				}
				CString long_str;
				long_str.Format(_T("%s%s%s"), Area_zero_str, Road_zero_str, Station_zero_str);
                strncpy(m_sTranstParam.LaneConfigInfo.str_StationID, long_str.GetBuffer(),128);
				
            }
            break;
            }
        }
        break;
        }

	

        CString strSTName = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("#")) - 1);
			
        _tcscpy_s(m_sTranstParam.LaneConfigInfo.StationTab, _countof(m_sTranstParam.LaneConfigInfo.StationTab), strSTName.GetBuffer(0));
		
        strSTName.ReleaseBuffer();
        _tcscpy_s(m_sTranstParam.PathCommDir, _countof(m_sTranstParam.PathCommDir), m_strWorkDir.GetBuffer(0));
        m_strWorkDir.ReleaseBuffer();
		

        //获取云端通讯IP地址以及开放端口
        char szTempIP[256] = {0};
        CString strReflectWebFilePath;
        strReflectWebFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);

        ::GetPrivateProfileString(_T("ReflectWeb"), _T("ExternalIP"), _T("101.200.214.203"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
        _tcscpy_s(m_sTranstParam.LaneConfigInfo.WebServerIP, _countof(m_sTranstParam.LaneConfigInfo.WebServerIP), szTempIP);

		::GetPrivateProfileString(_T("ReflectWeb"), _T("IP"), _T("101.200.214.203"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
		_tcscpy_s(m_sTranstParam.LaneConfigInfo.LocalPCWebServerIP, _countof(m_sTranstParam.LaneConfigInfo.LocalPCWebServerIP), szTempIP);

        m_sTranstParam.LaneConfigInfo.WebServerPort = ::GetPrivateProfileInt(_T("ReflectWeb"), _T("PORT"), 80, strReflectWebFilePath);

        m_sTranstParam.LaneConfigInfo.WebUpdatePort = ::GetPrivateProfileInt(_T("ReflectWeb"), _T("UpdatePORT"), 8090, strReflectWebFilePath);
        m_sTranstParam.LaneConfigInfo.UseAngencyDll = ::GetPrivateProfileInt(_T("Setup"), _T("UseAgencyPayDll"), 0, strReflectWebFilePath);
        m_sTranstParam.LaneConfigInfo.ReturnPayChannelCodeUnify    = ::GetPrivateProfileInt(_T("Setup"),     _T("ReturnPayChannelCodeUnify"), 0, strReflectWebFilePath);


		m_sTranstParam.LaneConfigInfo.ForceUTC8 = ::GetPrivateProfileInt(_T("Setup"), _T("ForceUTC8"), 1, strReflectWebFilePath);
        //获取平台内部IP设置信息
        int iPlaConfigIPCount = ::GetPrivateProfileInt(_T("PlaConfigNetInfo"), _T("IPCount"), 0, strReflectWebFilePath);
        m_sTranstParam.LaneConfigInfo.ConfigLocalIPMode = ::GetPrivateProfileInt(_T("PlaConfigNetInfo"), _T("IPMode"), 0, strReflectWebFilePath);

        if (0 == m_sTranstParam.LaneConfigInfo.ConfigLocalIPMode)
        {
            ::GetPrivateProfileString(_T("PlaConfigNetInfo"), _T("DefGate"), _T("192.168.5.3"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
            _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate), szTempIP);

            for (int i = 0; i < iPlaConfigIPCount; ++i)
            {
                CString strSection;
                CString strItem;
                strSection.Format(_T("NetInfo%02d"), i + 1);

                switch (i + 1)
                {
                case 1:
                {
                    ::GetPrivateProfileString(strSection, _T("IP"), _T("192.168.5.220"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
                    _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0), szTempIP);

                    ::GetPrivateProfileString(strSection, _T("Mask"), _T("255.255.255.0"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
                    _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0), szTempIP);

                    ::GetPrivateProfileString(strSection, _T("Gate"), _T("192.168.5.1"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
                    _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0), szTempIP);
                }
                break;
                case 2:
                {
                    ::GetPrivateProfileString(strSection, _T("IP"), _T("192.168.5.220"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
                    _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1), szTempIP);

                    ::GetPrivateProfileString(strSection, _T("Mask"), _T("255.255.255.0"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
                    _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1), szTempIP);

                    ::GetPrivateProfileString(strSection, _T("Gate"), _T("192.168.5.1"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
                    _tcscpy_s(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1, _countof(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1), szTempIP);
                }   break;
                }
            }
        }

        //判断是否已经初始化
        if ( m_bIsInitEnvironment )
            throw - 11;

        RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%s)"), strSTName, m_sTranstParam.ProvinceID, m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, strSTID));

        GenerateRawInitPara(m_sTranstParam);
        //进行合法性校验
        m_bAuthorizedLegal = TRUE;
        //启动业务管理线程
        CString strConfigFilePath;
        m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (NULL == m_pThreadManage)
            throw - 12;
        strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam, NULL);
        //将该线程重新唤醒
        m_pThreadManage->ResumeThread();

        //标记初始化状态
        m_bIsInitEnvironment = TRUE;

        //标记通知方式
        m_iNotifyMode = 1;

        //返回结果
        bReturn = m_bIsInitEnvironment ? true : false;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (m_strLastErrorDesc != strLog)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }

    RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment)调用结束")));

    strLog.Format(_T("初始化结果：%d"), bReturn);

    RecordLog(strLog);

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：IF_InitEnvironment2
//
// 功能描述：初始化组件环境
//
// 输入参数：IN const UINT& nThreadID              -- 调用者指定的线程ID
//         ：IN const HWND& hWnd                   -- 调用者指定的窗体句柄
//         ：IN const char* szAreaInfo             -- 区域信息(区域编码#路段号)
//         ：IN const char* szLoaclStation         -- 站点信息(站号#站名)
//         ：IN const char* szLoaclLaneID          -- 车道编码
//         ：IN const char* szServerInfo           -- 服务器信息
//         ：IN const int& iProvinceID             -- 省份编码
//         ：void (*fun)(int option, int result)   -- 扣款/撤单结果通知，回调函数
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年04月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1-1
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment2 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, void (*fun)(int option, int result))
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;

    RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2)调用开始")));
    try
    {
        /*******************检索参数是否正常*******************/
        if (  (0 == nThreadID) && (NULL == hWnd))
            throw - 1;
        if (NULL == fun)
            throw - 2;
        //回调函数指针赋值，便于通知调用者
        m_fnCallbackNotifyFun = fun;

        if ( (NULL == szAreaInfo) || (0 == _tcslen(szAreaInfo)))
            throw - 3;
        if ( (NULL == szLoaclStation) || (0 == _tcslen(szLoaclStation)))
            throw - 4;
        if ( (NULL == szLoaclLaneID) || (0 == _tcslen(szLoaclLaneID)))
            throw - 5;
        if ( (NULL == szServerInfo) || (0 == _tcslen(szServerInfo)))
            throw - 6;
        if ( 0 == iProvinceID)
            throw - 7;
        if ( (0 == nThreadID) && (!::IsWindow(hWnd)))
            throw - 8;
        //填充配置信息
        TCHAR   szTemp[256] = {0};
        CString strTemp;
        CString strSTID;

        //其它初始化参数赋值
        m_sTranstParam.ProvinceID            = iProvinceID;
        m_sTranstParam.ActiveNotifyMsgID     = 0;
        m_sTranstParam.CallerThreadID        = nThreadID;
        m_sTranstParam.CallerWnd             = hWnd;
        m_sTranstParam.LaneConfigInfo.LaneID = _ttoi(szLoaclLaneID);

		 strncpy(m_sTranstParam.LaneConfigInfo.str_LaneID , szLoaclLaneID, 128);

        strTemp.Format(_T("%s"), szAreaInfo);
        if (-1 == strTemp.Find(_T("#")))
            throw - 9;
        m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));
		strncpy(m_sTranstParam.LaneConfigInfo.str_AreaID,strTemp.Left(strTemp.Find(_T("#"))),128);
        CString strRoadID = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("#")) - 1);
        strncpy(m_sTranstParam.LaneConfigInfo.str_RoadID, strRoadID, 128);
        m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

        strTemp.Format(_T("%s"), szLoaclStation);
        if (-1 == strTemp.Find(_T("#")))
            throw - 10;
        m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));
        strcpy(m_sTranstParam.LaneConfigInfo.str_StationID , strTemp.Left(strTemp.Find(_T("#"))).GetBuffer());
        strSTID.Format(_T("%010d"), m_sTranstParam.LaneConfigInfo.StationID);

        //特殊处理代码区域
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            CString strSTID;
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的出口站号特殊处理
            case 30:
            case 31:
			default:
            {
                strSTID.Format(_T("%04d%03d%02d"), m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, m_sTranstParam.LaneConfigInfo.StationID);
                m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strSTID);
                strcpy(m_sTranstParam.LaneConfigInfo.str_StationID, strSTID.GetBuffer());

				CString Area_zero_str = m_sTranstParam.LaneConfigInfo.str_AreaID;
				if(Area_zero_str.GetLength() < 4)
				{
					Area_zero_str = CString('0',4-Area_zero_str.GetLength())+Area_zero_str;
				}
				CString Road_zero_str = m_sTranstParam.LaneConfigInfo.str_RoadID;
				if(Road_zero_str.GetLength() < 3)
				{
					Road_zero_str = CString('0',3-Road_zero_str.GetLength())+Road_zero_str;
				}
				CString Station_zero_str = m_sTranstParam.LaneConfigInfo.str_StationID;
				if(Station_zero_str.GetLength() < 2)
				{
					Station_zero_str = CString('0',2-Station_zero_str.GetLength())+Station_zero_str;
				}
				CString long_str;
				long_str.Format(_T("%s%s%s"), Area_zero_str, Road_zero_str, Station_zero_str);
                strncpy(m_sTranstParam.LaneConfigInfo.str_StationID, long_str.GetBuffer(),128);
            }
            break;
            }
        }
        break;
        }

        CString strSTName = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("#")) - 1);
        _tcscpy_s(m_sTranstParam.LaneConfigInfo.StationTab, _countof(m_sTranstParam.LaneConfigInfo.StationTab), strSTName.GetBuffer(0));
        strSTName.ReleaseBuffer();
        _tcscpy_s(m_sTranstParam.PathCommDir, _countof(m_sTranstParam.PathCommDir), m_strWorkDir.GetBuffer(0));
        m_strWorkDir.ReleaseBuffer();

        //获取反射服务器IP地址以及开放端口
        char szTempIP[256] = {0};
        CString strReflectWebFilePath;
        strReflectWebFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
		
        //::GetPrivateProfileString(_T("ReflectWeb"), _T("IP"), _T("101.200.214.203"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
        //_tcscpy_s(m_sTranstParam.LaneConfigInfo.WebServerIP, _countof(m_sTranstParam.LaneConfigInfo.WebServerIP), szTempIP);

		::GetPrivateProfileString(_T("ReflectWeb"), _T("ExternalIP"), _T("101.200.214.203"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
        _tcscpy_s(m_sTranstParam.LaneConfigInfo.WebServerIP, _countof(m_sTranstParam.LaneConfigInfo.WebServerIP), szTempIP);

		::GetPrivateProfileString(_T("ReflectWeb"), _T("IP"), _T("101.200.214.203"), szTempIP, _countof(szTempIP), strReflectWebFilePath);
		_tcscpy_s(m_sTranstParam.LaneConfigInfo.LocalPCWebServerIP, _countof(m_sTranstParam.LaneConfigInfo.LocalPCWebServerIP), szTempIP);


        m_sTranstParam.LaneConfigInfo.WebServerPort = ::GetPrivateProfileInt(_T("ReflectWeb"), _T("PORT"), 80, strReflectWebFilePath);

        m_sTranstParam.LaneConfigInfo.WebUpdatePort = ::GetPrivateProfileInt(_T("ReflectWeb"), _T("UpdatePORT"), 8089, strReflectWebFilePath);
        m_sTranstParam.LaneConfigInfo.UseAngencyDll = ::GetPrivateProfileInt(_T("Setup"), _T("UseAgencyPayDll"), 0, strReflectWebFilePath);
        m_sTranstParam.LaneConfigInfo.ReturnPayChannelCodeUnify    = ::GetPrivateProfileInt(_T("Setup"),     _T("ReturnPayChannelCodeUnify"), 0, strReflectWebFilePath);

		m_sTranstParam.LaneConfigInfo.ForceUTC8 = ::GetPrivateProfileInt(_T("Setup"), _T("ForceUTC8"), 0, strReflectWebFilePath);

        if ( m_bIsInitEnvironment )
            throw - 11;

        RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment2)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%s)"), strSTName, m_sTranstParam.ProvinceID, m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, strSTID));

        GenerateRawInitPara(m_sTranstParam);
        //进行合法性校验
        m_bAuthorizedLegal = TRUE;
        //启动业务管理线程
        CString strConfigFilePath;
        m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (NULL == m_pThreadManage)
            throw - 12;
        strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam, NULL);
        //将该线程重新唤醒
        m_pThreadManage->ResumeThread();

        //标记初始化状态
        m_bIsInitEnvironment = TRUE;

        //标记通知方式
        m_iNotifyMode = 2;

        //返回结果
        bReturn = m_bIsInitEnvironment ? true : false;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (m_strLastErrorDesc != strLog)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
// 输入参数：IN const UINT& nThreadID              -- 调用者指定的线程ID
//         ：IN const HWND& hWnd                   -- 调用者指定的窗体句柄
//         ：IN const char* szAreaInfo             -- 区域信息(区域编码#路段号)
//         ：IN const char* szLoaclStation         -- 站点信息(站号#站名)
//         ：IN const char* szLoaclLaneID          -- 车道编码
//         ：IN const char* szServerInfo           -- 服务器信息
//         ：IN const int& iProvinceID             -- 省份编码
//         ：IN OUT void* pIndPtr                  -- 个性化指针
//         ：void (*fun)(int option, int result,void* pIndPtr) -- 扣款/撤单结果通知，回调函数
// 输出参数：bool
// 编写人员：ROCY
// 编写时间：2017年03月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1-2，专门为广西定制的初始化函数，初始化操作的时候，保存一个个性化指针，在回掉的时候传出去
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment3 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, IN OUT void* pIndPtr, void (*fun)(int option, int result, void* pIndPtr))
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;

    RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3)调用开始,个性化指针参数:%04X"), pIndPtr));
    try
    {
        /*******************检索参数是否正常*******************/
        if (  (0 == nThreadID) && (NULL == hWnd))
            throw - 1;
        if (NULL == fun)
            throw - 2;
        //回调函数指针赋值，便于通知调用者
        m_fnCallbackNotifyFun45 = fun;

        if ( (NULL == szAreaInfo) || (0 == _tcslen(szAreaInfo)))
            throw - 3;
        if ( (NULL == szLoaclStation) || (0 == _tcslen(szLoaclStation)))
            throw - 4;
        if ( (NULL == szLoaclLaneID) || (0 == _tcslen(szLoaclLaneID)))
            throw - 5;
        if ( (NULL == szServerInfo) || (0 == _tcslen(szServerInfo)))
            throw - 6;
        if ( 0 == iProvinceID)
            throw - 7;
        if ( (0 == nThreadID) && (!::IsWindow(hWnd)))
            throw - 8;
        //填充配置信息
        TCHAR   szTemp[256] = {0};
        CString strTemp;
        CString strSTID;

        //其它初始化参数赋值
        m_sTranstParam.ProvinceID            = iProvinceID;
        m_sTranstParam.ActiveNotifyMsgID     = 0;
        m_sTranstParam.CallerThreadID        = nThreadID;
        m_sTranstParam.CallerWnd             = hWnd;
        m_sTranstParam.LaneConfigInfo.LaneID = _ttoi(szLoaclLaneID);

		strncpy(m_sTranstParam.LaneConfigInfo.str_LaneID , szLoaclLaneID, 128);

        strTemp.Format(_T("%s"), szAreaInfo);
        if (-1 == strTemp.Find(_T("#")))
            throw - 9;
        m_sTranstParam.LaneConfigInfo.AreaID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));
		strncpy(m_sTranstParam.LaneConfigInfo.str_AreaID,strTemp.Left(strTemp.Find(_T("#"))),128);
        CString strRoadID = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("#")) - 1);
        strncpy(m_sTranstParam.LaneConfigInfo.str_RoadID, strRoadID, 128);
        m_sTranstParam.LaneConfigInfo.RoadID = _ttoi(strRoadID);

        strTemp.Format(_T("%s"), szLoaclStation);
        if (-1 == strTemp.Find(_T("#")))
            throw - 10;
        m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strTemp.Left(strTemp.Find(_T("#"))));
        strcpy(m_sTranstParam.LaneConfigInfo.str_StationID, strTemp.Left(strTemp.Find(_T("#"))).GetBuffer());
        strSTID.Format(_T("%010d"), m_sTranstParam.LaneConfigInfo.StationID);


        //特殊处理代码区域
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            CString strSTID;
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的出口站号特殊处理
            case 30:
            case 31:
			default:
            {
                strSTID.Format(_T("%04d%03d%02d"), m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, m_sTranstParam.LaneConfigInfo.StationID);
                m_sTranstParam.LaneConfigInfo.StationID = _ttoi(strSTID);
                strcpy(m_sTranstParam.LaneConfigInfo.str_StationID, strSTID.GetBuffer());

				CString Area_zero_str = m_sTranstParam.LaneConfigInfo.str_AreaID;
				if(Area_zero_str.GetLength() < 4)
				{
					Area_zero_str = CString('0',4-Area_zero_str.GetLength())+Area_zero_str;
				}
				CString Road_zero_str = m_sTranstParam.LaneConfigInfo.str_RoadID;
				if(Road_zero_str.GetLength() < 3)
				{
					Road_zero_str = CString('0',3-Road_zero_str.GetLength())+Road_zero_str;
				}
				CString Station_zero_str = m_sTranstParam.LaneConfigInfo.str_StationID;
				if(Station_zero_str.GetLength() < 2)
				{
					Station_zero_str = CString('0',2-Station_zero_str.GetLength())+Station_zero_str;
				}
				CString long_str;
				long_str.Format(_T("%s%s%s"), Area_zero_str, Road_zero_str, Station_zero_str);
                strncpy(m_sTranstParam.LaneConfigInfo.str_StationID, long_str.GetBuffer(),128);
            }
            break;
            }
        }
        break;
        }

        CString strSTName = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("#")) - 1);
        _tcscpy_s(m_sTranstParam.LaneConfigInfo.StationTab, _countof(m_sTranstParam.LaneConfigInfo.StationTab), strSTName.GetBuffer(0));
        strSTName.ReleaseBuffer();
        _tcscpy_s(m_sTranstParam.PathCommDir, _countof(m_sTranstParam.PathCommDir), m_strWorkDir.GetBuffer(0));
        m_strWorkDir.ReleaseBuffer();

        if ( m_bIsInitEnvironment )
            throw - 11;

        //新型操作，保存一个个性化指针
        m_pIndPtrFor45 = pIndPtr;

        //记录日志
        RecordLog(FmtStr(_T("[主进程响应],组件初始化接口(IF_InitEnvironment3)，内部参数配置:%s(省份编码:%d,区域编码:%d,路段编码:%d,站点编码:%s)"), strSTName, m_sTranstParam.ProvinceID, m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, strSTID));

        GenerateRawInitPara(m_sTranstParam);
        //进行合法性校验
        m_bAuthorizedLegal = TRUE;
        //启动业务管理线程
        CString strConfigFilePath;
        m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (NULL == m_pThreadManage)
            throw - 12;
        strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam, NULL);
        //将该线程重新唤醒
        m_pThreadManage->ResumeThread();

        //标记初始化状态
        m_bIsInitEnvironment = TRUE;

        //标记通知方式
        m_iNotifyMode = 3;

        //返回结果
        bReturn = m_bIsInitEnvironment ? true : false;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (m_strLastErrorDesc != strLog)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
    m_fnCallbackNotifyFun45 = NULL;
    m_pIndPtrFor45 = NULL;
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
        DWORD   dwSpanTime = 100;
        m_bIsInitEnvironment = false;

        //停止业务管理线程
        if (NULL != m_pThreadManage && !(m_pThreadManage->m_bExit))
        {
            m_pThreadManage->Destroy_Dev();
			m_pThreadManage->ThreadQuit();
            //设置该线程推出标记为真
            //m_pThreadManage->m_bExit = TRUE;
            //沉默50毫秒
            ::Sleep(50);
            //每次等待1000毫秒，重试次数不超过10次
            while (OP_SUCCESS != fnCloseThread(m_pThreadManage->m_hThread, 1000))
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

            strLog.Format(_T("[主进程响应],组件资源清理接口(IF_Destroy),%s"), FmtStr(_T("【业务管理线程】已经停止(重试次数 %d)..."), iHaveTryCount));
            //记录日志
            RecordLog(strLog);
        }


        strLog.Format(_T("[主进程响应],组件即将退出..."));
        RecordLog(strLog, 0, TRUE);
        //标记成功
        bReturn = true;
    }
    catch (int& iErrorCode)
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
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断组件是否成功授权
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断业务处理线程是否异常
        if (NULL == m_pThreadManage)
            throw - 3;
        //给业务管理线程发送消息，要求控制硬件进行获取用户信息操作
        m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_START_GET_DATA, (WPARAM)iOvertime, (LPARAM)0x01);
        //标记成功
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        BYTE byFunCode      = 0x03;
        BYTE byExecuteState = RCC_INNER_ERROR;
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件未成功初始化"));
            byExecuteState  = RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog.Format(_T("[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件未被授权"));
            byExecuteState  = RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog.Format(_T("[主进程响应],启动获取用户信息操作(IF_StartGetAccountInfo)出错,组件内部资源异常"));
            break;
        }
        //保存错误描述
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
        //回馈调用者结果
        if (0 != m_sTranstParam.CallerThreadID)
        {
            ::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byFunCode, byExecuteState), MAKEWORD(0, 0)), (LPARAM)0);
        }

        if (::IsWindow(m_sTranstParam.CallerWnd))
        {
            ::PostMessage(m_sTranstParam.CallerWnd, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byFunCode, byExecuteState), MAKEWORD(0, 0)), (LPARAM)0);
        }
        //保存日志
        RecordLog(FmtStr(_T("[主进程响应],获取用户信息操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"), byFunCode, byExecuteState));
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
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断组件是否成功授权
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断业务处理线程是否异常
        if (NULL == m_pThreadManage)
            throw - 3;
        //给业务管理线程发送消息，要求控制硬件进行获取用户信息操作
        m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA, (WPARAM)0, (LPARAM)0);
        //标记成功
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
bool WINAPI CTWSDNetPay_DllApp::IF_GetAccountInfo(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;
    CString strDataString;
    RecordLog(FmtStr(_T("[主进程响应],获取用户信息接口(IF_GetAccountInfo)调用开始,参数1:%04X,参数2:%d,参数3:%d"), szParamContext, iParamSize, iFormatType));
    try
    {
        //判断组件是否已经被初始化
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断组件是否成功授权
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断业务处理线程是否异常
        if (NULL == m_pThreadManage)
            throw - 3;
        //判断参数是否异常
        if ((NULL == szParamContext) || (iParamSize < 1024))
            throw - 4;
        if ( (iFormatType > 2) || (iFormatType < 1))
            throw - 5;
        //判断数据是否已经获取成功
        if (!m_pThreadManage->GetAccountDataReadyState())
            throw - 6;


        if (strDataString.IsEmpty())
            throw - 7;
        RecordLog(FmtStr(_T("[主进程响应],获取用户信息接口(IF_GetAccountInfo)输出数据:%s"), strDataString));
        int tmpLen = strDataString.GetLength();
        iParamSize = tmpLen >= iParamSize ? (iParamSize - 1) : tmpLen;

        memcpy(szParamContext, strDataString.GetBuffer(0), iParamSize);
        strDataString.ReleaseBuffer();
        //标记成功
        bReturn = true;
        //设置用户数据准备状态为假
        m_pThreadManage->SetAccountDataReadyState(FALSE);
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
bool WINAPI CTWSDNetPay_DllApp::IF_DebitMoney(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog = FmtStr(_T("[主进程响应],执行扣款操作接口(IF_DebitMoney)调用开始,参数1:%s,参数2:%d,参数3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);
    CString strOperationMode;
    CString strDataString;
    CString strAnalyError;
    int nFType;

    try
    {
        /*****************************校核过程***********************************/
        //判断组件是否初始化成功
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断合法性
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断参数是否异常
        if ((NULL == szParamContext) || (0 == iParamSize) || (iFormatType > 2 || iFormatType < 1))
            throw - 3;

        strDataString.Format(_T("%s"), szParamContext);
        nFType = iFormatType; //底层使用json格式传输 （1.xml 2.json）

        if (1 == nFType)
        {
            CDVXMLDebitParamContext xml;

            if (!xml.Init(strDataString))
            {
                strAnalyError = xml.m_strLastErrorDesc;
                throw - 4;
            }

            //复制数据
            m_sCurrentDebitInfo.OverTime            = xml.OverTime;
            m_sCurrentDebitInfo.OperationMode       = xml.OperationMode;

		    //add 2018-05
			m_sCurrentDebitInfo.PayModel            = xml.PayModel;
			
            m_sCurrentDebitInfo.PayCertificateCode  = xml.PayCertificateCode;
            m_sCurrentDebitInfo.PayCertificateType  = xml.PayCertificateType;
            m_sCurrentDebitInfo.PayIdentifier       = xml.PayIdentifier;
            m_sCurrentDebitInfo.Money               = xml.Money;
            m_sCurrentDebitInfo.SubTime             = xml.SubTime;
            m_sCurrentDebitInfo.DebitMainType       = xml.DebitMainType;
            m_sCurrentDebitInfo.VehicleType         = xml.VehicleType;
            m_sCurrentDebitInfo.VehicleClass        = xml.VehicleClass;
            m_sCurrentDebitInfo.VLColor             = xml.VLColor;
            m_sCurrentDebitInfo.VehicleLicense      = xml.VehicleLicense;
            m_sCurrentDebitInfo.AxisCount           = xml.AxisCount;
            m_sCurrentDebitInfo.TotalWeight         = xml.Weight;
            m_sCurrentDebitInfo.LimitWeight         = xml.LimitWeight;
            m_sCurrentDebitInfo.PassCertificateType = xml.PassCertificateType;
            m_sCurrentDebitInfo.CardID              = xml.CardID;
            m_sCurrentDebitInfo.TollDate            = xml.TollDate;
            m_sCurrentDebitInfo.ShiftID             = xml.ShiftID;
            m_sCurrentDebitInfo.EntryStationID      = xml.EntryStationID;
			//add 2018-05
			m_sCurrentDebitInfo.AutoPlate = xml.AutoPlate;
			m_sCurrentDebitInfo.AutoPlateColor = xml.AutoPlateColor;

            //strcpy(m_sCurrentDebitInfo.str_EntryStationID,xml.str_EntryStationID.GetBuffer());
            m_sCurrentDebitInfo.str_EntryStationID = xml.str_EntryStationID;

            m_sCurrentDebitInfo.EntryStationName    = xml.EntryStationName;
            m_sCurrentDebitInfo.EntryTime           = xml.EntryTime;
            m_sCurrentDebitInfo.EntryLane           = xml.EntryLane;
			m_sCurrentDebitInfo.str_EntryLane       = xml.str_EntryLane;
            m_sCurrentDebitInfo.EntryVehicleLicense = xml.EntryVehicleLicense;
            m_sCurrentDebitInfo.EntryVLColor        = xml.EntryVLColor;
            m_sCurrentDebitInfo.EntryVehicleType    = xml.EntryVehicleType;
            m_sCurrentDebitInfo.EntryVehicleClass   = xml.EntryVehicleClass;
            m_sCurrentDebitInfo.EntryRoadID         = xml.EntryRoadID;

            m_sCurrentDebitInfo.str_EntryRoadID         = xml.str_EntryRoadID;//add 2018-01


            m_sCurrentDebitInfo.EntryAreaID         = xml.EntryAreaID;
			m_sCurrentDebitInfo.str_EntryAreaID = xml.str_EntryAreaID;
	
			
            m_sCurrentDebitInfo.TicketNo            = xml.TicketNo;
			

            m_sCurrentDebitInfo.OperatorID = xml.OperatorID;
            m_sCurrentDebitInfo.OperatorName = xml.OperatorName;

            m_sCurrentDebitInfo.Valid               = TRUE;
        }
        else
        {
            CDVJsonParamDebitMoney* pDebitMoney = NULL;
            CJsonParamOperate jsonParamOperation;

            pDebitMoney = (CDVJsonParamDebitMoney*)jsonParamOperation.FormatResponseResult2Class(strDataString, JSON_DT_PARAM_DEBIT_MONEY);
            if (NULL == pDebitMoney)
            {
                strAnalyError.Format(_T(""));
                throw - 5;
            }

            //复制数据
            m_sCurrentDebitInfo.OverTime            = pDebitMoney->OverTime;
            m_sCurrentDebitInfo.OperationMode       = pDebitMoney->OperationMode;
			//add 2018-05
			m_sCurrentDebitInfo.PayModel            = pDebitMoney->PayModel;

            m_sCurrentDebitInfo.PayCertificateCode  = pDebitMoney->PayCertificateCode;
            m_sCurrentDebitInfo.PayCertificateType  = pDebitMoney->PayCertificateType;
            m_sCurrentDebitInfo.PayIdentifier       = pDebitMoney->PayIdentifier;
            m_sCurrentDebitInfo.Money               = pDebitMoney->Money;
            m_sCurrentDebitInfo.SubTime             = pDebitMoney->SubTime;
            m_sCurrentDebitInfo.DebitMainType       = pDebitMoney->DebitMainType;
            m_sCurrentDebitInfo.VehicleType         = pDebitMoney->VehicleType;
            m_sCurrentDebitInfo.VehicleClass        = pDebitMoney->VehicleClass;
            m_sCurrentDebitInfo.VLColor             = pDebitMoney->VLColor;
            m_sCurrentDebitInfo.VehicleLicense      = pDebitMoney->VehicleLicense;
            m_sCurrentDebitInfo.AxisCount           = pDebitMoney->AxisCount;
            m_sCurrentDebitInfo.TotalWeight         = pDebitMoney->Weight;
            m_sCurrentDebitInfo.LimitWeight         = pDebitMoney->LimitWeight;
            m_sCurrentDebitInfo.PassCertificateType = pDebitMoney->PassCertificateType;
            m_sCurrentDebitInfo.CardID              = pDebitMoney->CardID;
            m_sCurrentDebitInfo.TollDate            = pDebitMoney->TollDate;
            m_sCurrentDebitInfo.ShiftID             = pDebitMoney->ShiftID;
            m_sCurrentDebitInfo.EntryStationID      = pDebitMoney->EntryStationID;
			//add 2018-05
			m_sCurrentDebitInfo.AutoPlate =				pDebitMoney->AutoPlate;
			m_sCurrentDebitInfo.AutoPlateColor =         pDebitMoney->AutoPlateColor;

            m_sCurrentDebitInfo.str_EntryStationID = pDebitMoney->str_EntryStationID;
            //strcpy(m_sCurrentDebitInfo.str_EntryStationID,pDebitMoney->str_EntryStationID.GetBuffer());
            m_sCurrentDebitInfo.EntryStationName    = pDebitMoney->EntryStationName;
            m_sCurrentDebitInfo.EntryTime           = pDebitMoney->EntryTime;
            m_sCurrentDebitInfo.EntryLane           = pDebitMoney->EntryLane;
			m_sCurrentDebitInfo.str_EntryLane       = pDebitMoney->str_EntryLane;
            m_sCurrentDebitInfo.EntryVehicleLicense = pDebitMoney->EntryVehicleLicense;
            m_sCurrentDebitInfo.EntryVLColor        = pDebitMoney->EntryVLColor;
            m_sCurrentDebitInfo.EntryVehicleType    = pDebitMoney->EntryVehicleType;
            m_sCurrentDebitInfo.EntryVehicleClass   = pDebitMoney->EntryVehicleClass;
            m_sCurrentDebitInfo.EntryRoadID         = pDebitMoney->EntryRoadID;

            m_sCurrentDebitInfo.str_EntryRoadID         = pDebitMoney->str_EntryRoadID;//add 2018-01
            m_sCurrentDebitInfo.EntryAreaID         = pDebitMoney->EntryAreaID;
			m_sCurrentDebitInfo.str_EntryAreaID     = pDebitMoney->str_EntryAreaID;//add 2018-02
            m_sCurrentDebitInfo.TicketNo            = pDebitMoney->TicketNo;

            m_sCurrentDebitInfo.OperatorID = pDebitMoney->OperatorID;
            m_sCurrentDebitInfo.OperatorName = pDebitMoney->OperatorName;

            m_sCurrentDebitInfo.Valid               = TRUE;

            delete pDebitMoney;
            pDebitMoney = NULL;
        }
        //add 2018-01 ,增加时间转换，非东八区，转换为东八区
		m_sCurrentDebitInfo.SubTime = FixUTC8(m_sCurrentDebitInfo.SubTime);
		m_sCurrentDebitInfo.EntryTime = FixUTC8(m_sCurrentDebitInfo.EntryTime);
        ////特殊处理超时时间（防止平台应用层与中间件的超时不同步),要求平台应用层超时时间比中间件减少3秒
        //if(m_sCurrentDebitInfo.OverTime > 3000)
        //{
        //  m_sCurrentDebitInfo.OverTime -= 3000;
        //}

        //特殊处理代码区域
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            CString strSTID;
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //广东华快的入口站号特殊处理
            case 30:
            case 31:
            default:/*add 2017-11 zhuyabing ,广东省全部使用拼接*/
            {
                strSTID.Format(_T("%04d%03d%02d"), m_sCurrentDebitInfo.EntryAreaID, m_sCurrentDebitInfo.EntryRoadID, m_sCurrentDebitInfo.EntryStationID);
                m_sCurrentDebitInfo.EntryStationID = _ttoi(strSTID);

                m_sCurrentDebitInfo.str_EntryStationID = strSTID;
                //strcpy(m_sCurrentDebitInfo.str_EntryStationID,strSTID.GetBuffer());
            }
            break;
            }
        }
        break;
        }

        //检测业务管理线程是否正常
        if (NULL == m_pThreadManage)
            throw - 6;

        //判断上次交易是否完成
        if (m_bIsTrading)
            throw - 7;

        //标记当前交易正在进行
        m_bIsTrading = TRUE;

        /*****************************响应操作***********************************/

        strOperationMode.Format(_T("整合模式"));
        m_bCurrentDebitMode = TRUE;


        //进行资源申请
        CString* pstrString = new CString();
        if (NULL == pstrString)
            throw - 8;

        //重构扣款JSON/xml
        nFType = 2;
        pstrString->Format(_T("%s"), CostructDebitInfo( nFType, m_sCurrentDebitInfo));

        //关闭上次的定时器
        m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
        //启动扣款超时定时器
        m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY, m_sCurrentDebitInfo.OverTime);
        //开始计时
        m_dwTradeBeginOvertime = GetTickCount();
        //启动扣款流程
        m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_START_DEBIT_FLOW, (WPARAM)pstrString, (LPARAM)nFType); //把数据解析类型抛送过去

        //添加描述
        m_strLastErrorDesc.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney),业务正在处理(%s)"), strOperationMode);

        //记录日志
        RecordLog(m_strLastErrorDesc);

        //标记成功
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        BYTE byFunCode      = 0x01;
        BYTE byExecuteState = RCC_INNER_ERROR;
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),组件未成功初始化"), strOperationMode);
            byExecuteState  = RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),合法性校验失败"), strOperationMode);
            byExecuteState  = RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),传入参数异常"), strOperationMode);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -4:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),传入参数(XML格式)解析出错，详细描述:%s"), strOperationMode, strAnalyError);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -5:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),传入参数(JSON格式)解析出错"), strOperationMode);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -6:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),内部处理异常:业务管理模块未能正常工作"), strOperationMode);
            break;
        case -7:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),内部处理异常:上次交易未完成，不允许频繁调用"), strOperationMode);
            byExecuteState  = RCC_LAST_TRADE_NOT_END;
            break;
        case -8:
            strLog.Format(_T("[主进程响应],执行扣款操作(IF_DebitMoney)出错(%s),内部处理异常:资源申请失败"), strOperationMode);
            break;
        }
        //保存错误描述
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
        //预处理失败，回馈调用者结果
        ::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
        //保存日志
        RecordLog(FmtStr(_T("[主进程响应],扣款操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"), byFunCode, byExecuteState));
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
bool WINAPI CTWSDNetPay_DllApp::IF_DebitCancel(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //MessageBox(0,"break","",0);
    bool bReturn = false;
    CString strLog = FmtStr(_T("[主进程响应],执行撤单操作接口(IF_DebitCancel)调用开始,参数1:%s,参数2:%d,参数3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);

    CString strDataString;
    CString strPayIdentifier;
    CDVXMLDebitCancel xml;
    try
    {
        /*****************************校核过程***********************************/
        //判断组件是否初始化成功
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断合法性
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断参数是否异常
        if ((NULL == szParamContext) || (0 == iParamSize) || (iFormatType > 2 || iFormatType < 1))
            throw - 3;

        strDataString.Format(_T("%s"), szParamContext);
        if (1 == iFormatType)
        {
            if (!xml.Init(strDataString))
                throw - 4;
            strPayIdentifier.Format(_T("%s"), xml.PayIdentifier);
        }
        else
        {
            CDVJsonParamDebitCancel* pDebitCancel = NULL;
            CJsonParamOperate jsonParamOperation;
            pDebitCancel = (CDVJsonParamDebitCancel*)jsonParamOperation.FormatResponseResult2Class(strDataString, JSON_DT_PARAM_DEBIT_CANCLE);
            if (NULL == pDebitCancel)
            {
                throw - 5;
            }
            strPayIdentifier.Format(_T("%s"), pDebitCancel->PayIdentifier);

            delete pDebitCancel;
            pDebitCancel = NULL;
        }
        //检测业务管理线程是否正常
        if (NULL == m_pThreadManage)
            throw - 6;
        //检测撤单交易标识是否合法
        strPayIdentifier.TrimLeft();
        strPayIdentifier.TrimRight();
        if (strPayIdentifier.IsEmpty())
            throw - 7;
        //进行资源申请
        CString* pstrString = new CString();
        if (NULL == pstrString)
            throw - 8;

        pstrString->Format(_T("%s"), strPayIdentifier);


        m_pThreadManage->SendBreakDebit(strPayIdentifier);

        m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_DEBIT_CANCEL, (WPARAM)pstrString, (LPARAM)iFormatType); //把解析类型抛送过去

        //Sleep(1000);//
        //m_pThreadManage->SendBreakDebit(strPayIdentifier);

        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        BYTE byFunCode      = 0x02;
        BYTE byExecuteState = RCC_INNER_ERROR;

        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,组件未成功初始化"));
            byExecuteState  = RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,合法性校验失败"));
            byExecuteState  = RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数异常"));
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -4:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数解析出错，详细描述:%s"), xml.m_strLastErrorDesc);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -5:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数解析出错(JSON)"));
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -6:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,内部处理异常:业务管理模块未能正常工作"));
            break;
        case -7:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,传入参数有误,需要撤销的订单对应的交易识别码为空"));
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -8:
            strLog.Format(_T("[主进程响应],执行撤单操作(IF_DebitCancel)出错,内部处理异常:资源申请失败"));
            break;
        }
        //保存错误描述
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
        //预处理失败，回馈调用者结果
        ::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
        //保存日志
        RecordLog(FmtStr(_T("[主进程响应],撤单操作预处理失败，将结果通知调用者(功能代码:%d,执行状况:%d)"), byFunCode, byExecuteState));
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
bool WINAPI CTWSDNetPay_DllApp::IF_GetDebitResult(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;
    CString strDataString;
    RecordLog(FmtStr(_T("[主进程响应],获取扣款结果接口(IF_GetDebitResult)调用开始,参数1:%04X,参数2:%d,参数3:%d"), szParamContext, iParamSize, iFormatType));
    try
    {
        //判断组件是否初始化成功
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断合法性
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断参数是否异常
        if ((iFormatType > 2) || (iFormatType < 1) || (NULL == szParamContext))
            throw - 3;
        //判断结果是否已经准备好
        if (!m_sCurrentDebitResult.Valid)
            throw - 4;

        switch (iFormatType)
        {
        case 1:
        {
            CDVXMLDebitResult xml;
            xml.ResultType      = m_sCurrentDebitResult.ResultType;
            xml.ExecuteCode     = m_sCurrentDebitResult.ExecuteCode;
            xml.PayPlatformType = m_sCurrentDebitResult.PayPlatformType;
            xml.DebitTime       = m_sCurrentDebitResult.DebitTime;
            xml.ConsumeTime     = m_sCurrentDebitResult.ConsumeTime;
            xml.ExecuteDesc.Format(_T("%s"), m_sCurrentDebitResult.ExecuteDesc);
            xml.TradeSecurityCode.Format(_T("%s"), m_sCurrentDebitResult.TradeSecurityCode);
            xml.PayIdentifier.Format(_T("%s"), m_sCurrentDebitResult.PayIdentifier);
            xml.DebitOrder.Format(_T("%s"), m_sCurrentDebitResult.DebitOrder);

            strDataString = xml.GetXMLString();

            strDataString.Replace(_T("\r"), _T(""));
            strDataString.Replace(_T("\n"), _T(""));
        }
        break;
        case 2:
        {
            CDVJsonParamDebitResult jsonDebitResult;
            jsonDebitResult.ResultType      = m_sCurrentDebitResult.ResultType;
            jsonDebitResult.ExecuteCode     = m_sCurrentDebitResult.ExecuteCode;
            jsonDebitResult.PayPlatformType = m_sCurrentDebitResult.PayPlatformType;
            jsonDebitResult.DebitTime       = m_sCurrentDebitResult.DebitTime;
            jsonDebitResult.ConsumeTime     = m_sCurrentDebitResult.ConsumeTime;
            jsonDebitResult.ExecuteDesc.Format(_T("%s"), m_sCurrentDebitResult.ExecuteDesc);
            jsonDebitResult.TradeSecurityCode.Format(_T("%s"), m_sCurrentDebitResult.TradeSecurityCode);
            jsonDebitResult.PayIdentifier.Format(_T("%s"), m_sCurrentDebitResult.PayIdentifier);
            jsonDebitResult.DebitOrder.Format(_T("%s"), m_sCurrentDebitResult.DebitOrder);

            CJsonParamOperate jsonParamOperation;
            strDataString = jsonParamOperation.FormatParamData2Json(&jsonDebitResult);
            if (strDataString.IsEmpty())
            {
                throw - 5;
            }
        }
        break;
        default:
            break;
        }

        if (strDataString.GetLength() > (iParamSize + 1))
            throw - 6;

        strDataString.TrimRight();
        RecordLog(FmtStr(_T("[主进程响应],获取扣款结果接口(IF_GetDebitResult)输出结果如下:(%s)%s "), (0x01 == iFormatType) ? _T("数据格式:XML") : _T("数据格式:JSON"), strDataString));

        _tcscpy_s(szParamContext, strDataString.GetLength() + 1, strDataString.GetBuffer(0));
        strDataString.ReleaseBuffer();

        //重置交易结果
        m_sCurrentDebitResult.Valid = FALSE;
        memset(&m_sCurrentDebitResult, 0, sizeof(tagDebitResultItems));

        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
bool WINAPI CTWSDNetPay_DllApp::IF_SetMMI(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());


    bool bReturn = false;
    CString strLog = FmtStr(_T("[主进程响应],设置交互界面接口(IF_SetMMI)调用开始,参数1:%s,参数2:%d,参数3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);

    int nFormateType = iFormatType;
    CString strParamString(szParamContext);
    CString strDataString;
    CString strSubError;
    try
    {
        //判断组件是否已经被初始化
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断组件是否成功授权
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断业务处理线程是否异常
        if (NULL == m_pThreadManage)
            throw - 3;
        if (NULL == m_pThreadManage->m_pThreadDevice)
            throw - 4;
        //判断参数是否异常
        if ((NULL == szParamContext) || (0 == iParamSize))
            throw - 5;
        //判断参数类型
        if ( (iFormatType > 2) || (iFormatType < 1))
            throw - 6;

        CString strMMIString = ConstructMMIInfo(iFormatType, strParamString);

        if (strMMIString.IsEmpty())
            throw - 9;

        //获取数据
        CString* pstrString = new CString();
        if (NULL == pstrString)
            throw - 10;
        pstrString->Format(_T("%s"), strMMIString);
        //通知硬件控制线程显示界面
        m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO, (WPARAM)pstrString, (LPARAM)nFormateType);
        //标记成功
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
            strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(XML格式)有误，详细描述:%s"), strSubError);
            break;
        case -8:
            strLog.Format(_T("[主进程响应],设置交互界面操作(IF_SetMMI)出错,解析参数(JSON格式)有误，详细描述:%s"), strSubError);
            break;
        }
        //保存错误描述
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }

    RecordLog(FmtStr(_T("[主进程响应],设置交互界面接口(IF_SetMMI)调用结束")));
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：WINAPI    CTWSDNetPay_DllApp::IF_GetParam
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
bool WINAPI CTWSDNetPay_DllApp::IF_GetParam(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, IN OUT char* szResultContext, IN OUT int& iResultSize)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog = FmtStr(_T("[主进程响应],获取硬件参数接口(IF_GetParam)暂不提供此项功能"));

    if (strLog != m_strLastErrorDesc)
    {
        m_strLastErrorDesc = strLog;
        RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
    //  RecordLog(FmtStr(_T("[主进程响应],检测组件状态接口(IF_GetComponentStatus)调用开始,参数1:%d"),uStatus));
    try
    {
        //判断组件是否已经被初始化
        if (!m_bIsInitEnvironment)
            throw - 1;
        //判断组件是否成功授权
        if (!m_bAuthorizedLegal)
            throw - 2;
        //判断业务处理线程是否异常
        if (NULL == m_pThreadManage)
            throw - 3;
        //判断硬件控制线程是否正常
        if (NULL == m_pThreadManage->m_pThreadDevice)
            throw - 4;
        //终端设备状态/网络状态
        CString strState;

        //获取硬件状态
        DWORD dwHDState = m_pThreadManage->m_pThreadDevice->GetDeviceState();

        uStatus = (dwState | dwHDState);

        //add 2018-01,增加逻辑，判断是否屏蔽设备状态的返回
        CString strReflectWebFilePath;
        strReflectWebFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        int hide_option = ::GetPrivateProfileInt(_T("SetUp"), _T("HideDeviceState"), 0, strReflectWebFilePath);
        if (hide_option == 1)
        {
            uStatus = 0;
        }

        //标记成功
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
        }
    }
    //  RecordLog(FmtStr(_T("[主进程响应],检测组件状态接口(IF_GetComponentStatus)调用结束")));

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
bool WINAPI CTWSDNetPay_DllApp::IF_TranslateData(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog = FmtStr(_T("[主进程响应],执行传输关键信息数据接口(IF_TranslateData)暂不提供此项功能"));
    if (strLog != m_strLastErrorDesc)
    {
        m_strLastErrorDesc = strLog;
        RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
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
bool WINAPI CTWSDNetPay_DllApp::IF_Authority(IN const char* szAuthKey, IN const int& iAuthKeySize)
{
    bool bReturn = false;

    return bReturn;
}


bool AgencyImageXML2Json(const std::string& xml_str, std::string& json_str)
{
    CMarkup xml;
    xml.SetDoc( xml_str.c_str() );
    xml.ResetMainPos();

    xml.ResetMainPos();
    if (!xml.FindElem(_T("DebitImage")))
    {
        theApp.RecordLog("DebitImage not found");
        return false;
    }

    xml.IntoElem();

    xml.ResetMainPos();
    if (!xml.FindElem(_T("Data")))
    {
        theApp.RecordLog("Data not found");
        return false;
    }

    xml.IntoElem();

	
    int Type,dataFormat;
    std::string PayIdentifier , Image ;
    xml.ResetMainPos();

    if (!xml.FindElem(_T("Type")))
    {
        theApp.RecordLog("Type not found");
        return false;
    }
	
    Type = atoi(xml.GetData());



    xml.ResetMainPos();
    if (!xml.FindElem(_T("PayIdentifier")))
    {
        theApp.RecordLog("PayIdentifier not found");
        return false;
    }

    PayIdentifier = std::string((LPCTSTR)xml.GetData());

    xml.ResetMainPos();
    if (!xml.FindElem(_T("Image")))
    {
        theApp.RecordLog("Image not found");
        return false;
    }

    Image = std::string((LPCTSTR)xml.GetData());

	xml.ResetMainPos();
	if (!xml.FindElem(_T("DataFormat")))
    {
		if(!xml.FindElem(_T("Class")))
		{
			 theApp.RecordLog("Class and DataFormat not found");
			 return false;
		}
       
    }
	dataFormat = atoi(xml.GetData());

    //进行json的封装
    Json::Value rootN, contextN, dataN;
   

    dataN["Type"] = Json::Value(Type);
	dataN["DataFormat"] = Json::Value(dataFormat);
    dataN["PayIdentifier"] = Json::Value(PayIdentifier);
    dataN["Image"] = Json::Value(Image);
    contextN["Data"] = dataN;
    rootN["DebitImage"] = contextN;
    rootN["Version"] = Json::Value("1.0");
    Json::FastWriter JFW;

    json_str = JFW.write(rootN);

    return true;
}

std::string RepairAgentImageJson(const std::string& json_str)
{
	Json::Value rootN,debitN,dataN;
	Json::Reader jsonReader;
	if(!jsonReader.parse(json_str,rootN))
	{
		return json_str;
	}
	debitN=rootN["DebitImage"];
	dataN=debitN["Data"];
	
	if(dataN["DataFormat"] == Json::nullValue)
	{
		dataN["DataFormat"]=dataN["Class"];
	}
	
	dataN.removeMember("Class");
	debitN["Data"]=dataN;
	rootN["DebitImage"]=debitN;
	
	Json::FastWriter jfw;
	return jfw.write(rootN);
}

bool WINAPI CTWSDNetPay_DllApp::IF_TransferImage(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strReflectWebFilePath;
    strReflectWebFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
    int image_print_option = ::GetPrivateProfileInt(_T("SetUp"), _T("PrintImageData"), 0, strReflectWebFilePath);
    if (image_print_option == 1)
    {
        CString strLog = FmtStr(_T("[主进程响应],图像传输接口(IF_TransferImage)调用开始,参数1:%s,参数2:%d,参数3:%d"), szImageInfo, iSize, iFormatType);
        strLog.Replace(_T("\r"), _T(""));
        strLog.Replace(_T("\n"), _T(""));
        RecordLog(strLog);
    }
	
    std::string paraStr;

    if (iFormatType == 1) //xml
    {
        if (!AgencyImageXML2Json(szImageInfo, paraStr))
        {
            RecordLog(("Error Para Info" + std::string(szImageInfo)).c_str(), LOG_LEVEL_ERROR);
            return false;
        }
    }
    else if (iFormatType == 2) //json
    {
        paraStr = RepairAgentImageJson(szImageInfo);
    }
    else
    {
        RecordLog("Error Para Info：not json or xml", LOG_LEVEL_ERROR);
        return false;
    }

    if (! m_pThreadManage->m_pThreadDevice->TransferImage(paraStr))
    {
        return false;
    }

    return true;
}


bool AgencyXML2Json(const std::string& xml_str, std::string& json_str)
{
    CMarkup xml;
    xml.SetDoc( xml_str.c_str() );
    xml.ResetMainPos();

    xml.ResetChildPos();
    if (!xml.FindElem(_T("DebitQueryParamContext")))
    {
        theApp.RecordLog("DebitQueryParamContext not found");
        return false;
    }

    xml.IntoElem();


    xml.ResetChildPos();
    if (!xml.FindElem(_T("Data")))
    {
        theApp.RecordLog("Data not found");
        return false;
    }

    xml.IntoElem();


    xml.ResetChildPos();
    if (!xml.FindElem(_T("Vehicle")))
    {
        theApp.RecordLog("Vehicle not found");
        return false;
    }


    int AxisCount , Class  , LimitWeight , Type , VLColor , Weight;
    std::string License;

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("AxisCount")))
    {
        theApp.RecordLog("AxisCount not found");
        return false;
    }

    AxisCount = atoi(xml.GetChildData());

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("Class")))
    {
        theApp.RecordLog("Class not found");
        return false;
    }

    Class = atoi(xml.GetChildData());

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("License")))
    {
        theApp.RecordLog("License not found");
        return false;
    }

    License = std::string((LPCTSTR)xml.GetChildData());

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("LimitWeight")))
    {
        theApp.RecordLog("LimitWeight not found");
        return false;
    }
    LimitWeight = atoi(xml.GetChildData());

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("Type")))
    {
        theApp.RecordLog("Type not found");
        return false;
    }

    Type = atoi(xml.GetChildData());

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("VLColor")))
    {
        theApp.RecordLog("VLColor not found");
        return false;
    }

    VLColor = atoi(xml.GetChildData());

    xml.ResetChildPos();
    if (!xml.FindChildElem(_T("Weight")))
    {
        theApp.RecordLog("Weight not found");
        return false;
    }


    Weight = atoi(xml.GetChildData());


    //进行json的封装
    Json::Value rootN, contextN, dataN, VehicleN;
    VehicleN["AxisCount"] = Json::Value(AxisCount);
    VehicleN["Class"] = Json::Value(Class);
    VehicleN["License"] = Json::Value(License);
    VehicleN["LimitWeight"] = Json::Value(LimitWeight);
    VehicleN["Type"] = Json::Value(Type);
    VehicleN["VLColor"] = Json::Value(VLColor);
    VehicleN["Weight"] = Json::Value(Weight);

    dataN["Vehicle"] = VehicleN;
    contextN["Data"] = dataN;
    rootN["DebitQueryParamContext"] = contextN;
    rootN["Version"] = Json::Value("1.0");
    Json::FastWriter JFW;

    json_str = JFW.write(rootN);

    return true;
}

bool CTWSDNetPay_DllApp::IF_AgencyDebitQuery(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, int& result)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.RecordLog("IF_AgencyDebitQuery 开始调用");
    std::string paraStr;
    if (iFormatType == 1) //xml
    {
        if (!AgencyXML2Json(szParamContext, paraStr))
        {
            RecordLog(("Error Para Info" + std::string(szParamContext)).c_str(), LOG_LEVEL_ERROR);
            return false;
        }
    }
    else if (iFormatType == 2) //json
    {
        paraStr = std::string(szParamContext);
    }
    else
    {
        RecordLog("Error Para Info：not json or xml", LOG_LEVEL_ERROR);
        return false;
    }
    if (! m_pThreadManage->m_pThreadDevice->AgencyDebitQuery(paraStr, iFormatType, result))
    {
		theApp.RecordLog("IF_AgencyDebitQuery 调用结束：失败");
        return false;
    }
	theApp.RecordLog("IF_AgencyDebitQuery 调用结束：成功");

    return true;
}



/****************************************内部辅助函数********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::RecordLog
//
// 功能描述：记录日志
//
// 输入参数：IN const CString& strLog -- 日志内容
//         ：IN const int& iLevel    -- 日志等级
//         ：IN const BOOL& bSave    -- 日志是否立马保存
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::RecordLog(IN const CString& strLog, IN const int& iLevel, IN const BOOL& bSave)
{
    if (m_bSaveLog)
    {
        if (strLog.GetLength() < 4)
        {
            return;
        }
        WRITE_LOG(strLog, iLevel, bSave);
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
    TCHAR szTemp[256] = {0};
    CString strConfigFilePath;
    strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
    //是否保存日志
    m_bSaveLog              = (BOOL)::GetPrivateProfileIntA(_T("Log"), _T("Save"), 0, strConfigFilePath);
    //获取日志删除限定时间段起点
    m_iDeleteLogStartHour = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteStartHour"), 2, strConfigFilePath);
    //获取日志删除限定时间段终点
    m_iDeleteLogEndHour = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteEndHour"), 2, strConfigFilePath);
    //日志删除操作轮询时间
    m_iDeleteLogSpanTime = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteSpanTime"), 30 * 60, strConfigFilePath);
    //日志本地保存天数时间
    m_iDeleteLogSaveDays = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteSaveDays"), 60, strConfigFilePath);
    //标记是否测试数据
    m_bIsTestData           = (BOOL)::GetPrivateProfileIntA(_T("Setup"), _T("TestData"), 0, strConfigFilePath);
    //线程存活日志检测时间间隔
    m_dwThreadCheckSpanTime = ::GetPrivateProfileIntA(_T("Setup"), _T("ThreadCheckSpanTime"), 300, strConfigFilePath);
    //获取虚拟卡前缀
    ::GetPrivateProfileString(_T("Setup"), _T("VirtualCardPrefix"), _T("4412"), szTemp, 256, strConfigFilePath);
    m_strVirtualCardPrefix.Format(_T("%s"), szTemp);
    //获取往智能硬件下发时间的间隔
    m_dwUpdateDevieTimeSpanTime = (DWORD)::GetPrivateProfileIntA(_T("Setup"), _T("UpdateDeviceTimeSpanTime"), 30 * 60, strConfigFilePath);
    m_dwUpdateDevieTimeSpanTime *= 1000;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::fnCloseThread
//
// 功能描述：等待一个即将被关闭线程正常关闭
//
// 输入参数：IN const HANDLE& hThread          --  需要被关闭的线程句柄
//         ：IN const DWORD& dwMilliseconds        --  等待时间（单位：毫秒）
// 输出参数：int
//         ：0  -- 线程已经被正常关闭
//         ：-1    -- 线程句柄无效
//         ：-2    -- 内核对象未激发而等待时间已到
//         ：-3    -- 等待的互斥器未被释放
//         ：-4    -- 等待函数执行失败
// 编写人员：ROCY
// 编写时间：2015年11月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::fnCloseThread(IN const HANDLE& hThread, IN const DWORD& dwMilliseconds)
{
    int iReturn = 0;
    //尝试关闭线程
    DWORD dwWaitResult = ::WaitForSingleObject(hThread, dwMilliseconds);
    //获得错误码
    DWORD dwErrorCode = ::GetLastError();
    //消息结构体提
    MSG  mMsg;

    if (6 == dwErrorCode)//句柄无效
    {
        iReturn = 0;
    }
    else
    {
        //判断等待函数返回结果
        switch (dwWaitResult)
        {
        case WAIT_OBJECT_0: //内核对象变为激发状态
            iReturn = 0;
            break;
        case WAIT_TIMEOUT:  //内核对象未激发而等待时间已到
        {
            if (::PeekMessage(&mMsg, NULL, 0, 0, PM_NOREMOVE))
            {
                ::TranslateMessage(&mMsg); Sleep(10);
                ::DispatchMessage(&mMsg); Sleep(10);
            }
        }
        iReturn = -2;
        break;
        case WAIT_ABANDONED://等待的互斥器未被释放
        {
            if (::PeekMessage(&mMsg, NULL, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&mMsg); Sleep(10);
                ::DispatchMessage(&mMsg); Sleep(10);
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
//  作   者 : ROCY
//  时   间 : 2008-05-31
//  版   本 : 1.0.0.1
//  函数名称 : DisplayWinError
//  函数功能 : 显示系统最新的错误信息
//  输入参数 : DWORD    --  dwErrorCode 给定的系统错误代码,一般由 GetLastError() 函数捕获
//             int      --  iType       默认为0
//             BOOL     --  bPrompt     是否需要用对话框显示该错误信息（TRUE为需要，FALSE为不需要）
//  输出参数 : LPCTSTR -- 详细的错误描述信息
//  备注说明 : 每次调用API后都可以调用此函数查看详细的错误信息
///////////////////////////////////////////////////////////////////////////////////////////////////////////
LPCTSTR CTWSDNetPay_DllApp::DisplayWinError(DWORD dwErrorCode, int nType, BOOL bPrompt)
{
    static CString  strTemp = _T("");
    LPSTR lpMessageBuffer = NULL;
    USES_CONVERSION;
    ::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,// | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      (nType == 0) ? dwErrorCode : LsaNtStatusToWinError(dwErrorCode),
                      0,
                      (LPSTR) &lpMessageBuffer,
                      0,
                      NULL);
    strTemp.Format(_T("【错误代码:%d】 -- 【详细描述:%s】"), dwErrorCode, A2T(lpMessageBuffer));
    strTemp.Replace(_T("\r\n"), _T(" "));

    if (bPrompt)
        ::MessageBox(NULL, strTemp, _T("系统提示"), MB_ICONWARNING);

    LocalFree(lpMessageBuffer);
    return strTemp;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::GetProcessMemory
//
// 功能描述：获取当前进程所使用的内存
//
// 输入参数：IN const int& iShowType
// 输出参数：DWORD
// 编写人员：ROCY
// 编写时间：2017年5月7日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CTWSDNetPay_DllApp::GetProcessMemory(IN const int& iShowType)
{
    DWORD dDevValue = (1 == iShowType) ? (1024 * 1024) : 1024;
    //获取当前进程使用内存信息
    DWORD dwProcessMemory = 0x00;
    PROCESS_MEMORY_COUNTERS pLocalProcessMemory = {0};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pLocalProcessMemory, sizeof(pLocalProcessMemory)))
    {
        dwProcessMemory = pLocalProcessMemory.PagefileUsage / dDevValue;
    }

    return dwProcessMemory;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::CostructDebitInfo
//
// 功能描述：构造发送到串口的数据格式 -- 扣款处理
//
// 输入参数：int nFormatType
//         ：tagPayKeyItems& pkItems
// 输出参数：CString
// 编写人员：林伙泉
// 编写时间：2017年2月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::CostructDebitInfo(int nFormatType, tagPayKeyItems& pkItems)
{
    /*add 2017-06 zhuyabing 将入口时间修正为与过车时间一致。 */
    //pkItems.EntryTime = pkItems.SubTime;

    CString strReturn;
    if (1 == nFormatType) //xml
    {
        CMarkup xmlDebit;
        xmlDebit.SetDoc(_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"));
        xmlDebit.AddElem(_T("DebitParamContext")); //root
        xmlDebit.IntoElem();

        xmlDebit.AddElem( _T("Version"),    _T("1.0") );

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
        xmlDebit.AddElem( _T("AxisCount") , pkItems.AxisCount);
        xmlDebit.AddElem( _T("Weight") , pkItems.TotalWeight);
        xmlDebit.AddElem( _T("LimitWeight") , pkItems.LimitWeight);
        xmlDebit.OutOfElem();

        xmlDebit.AddElem( _T("Operation") );
        xmlDebit.IntoElem();
        xmlDebit.AddElem( _T("PassCertificateType") , pkItems.PassCertificateType);
        xmlDebit.AddElem( _T("CardID") , pkItems.CardID);
        xmlDebit.AddElem( _T("TollDate") , pkItems.TollDate);
        xmlDebit.AddElem( _T("ShiftID") , pkItems.ShiftID);
        xmlDebit.AddElem( _T("TicketNo") , pkItems.TicketNo);
        xmlDebit.AddElem( _T("OperatorName") , pkItems.OperatorName);
        xmlDebit.AddElem( _T("OperatorID") , pkItems.OperatorID);

        xmlDebit.OutOfElem();

        xmlDebit.AddElem( _T("Entry") );
        xmlDebit.IntoElem();
        //xmlDebit.AddElem( _T("StationID") , pkItems.EntryStationID);
        xmlDebit.AddElem( _T("StationID") , pkItems.str_EntryStationID);
        xmlDebit.AddElem( _T("StationName") , pkItems.EntryStationName);
        xmlDebit.AddElem( _T("Time") , pkItems.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		xmlDebit.AddElem( _T("LaneID") , pkItems.str_EntryLane);
        xmlDebit.AddElem( _T("License") , pkItems.EntryVehicleLicense);
        xmlDebit.AddElem( _T("VColor") , pkItems.EntryVLColor);
        xmlDebit.AddElem( _T("VClass") , pkItems.EntryVehicleClass);
        xmlDebit.AddElem( _T("VType") , pkItems.EntryVehicleType);
        //xmlDebit.AddElem( _T("RoadID") , pkItems.EntryRoadID);
        xmlDebit.AddElem( _T("RoadID") , pkItems.str_EntryRoadID);
		xmlDebit.AddElem( _T("AreaID") , pkItems.str_EntryAreaID);
        xmlDebit.OutOfElem();

        xmlDebit.OutOfElem();
        xmlDebit.OutOfElem();
        strReturn = xmlDebit.GetDoc();

    }
    else   //重构扣款json
    {

        //表示整个json对象
        Json::Value     root;

        Json::Value     DebitParamContext;

        Json::Value     Data;
        Json::Value     Version;

        Json::Value     Debit;
        Json::Value     Entry;
        Json::Value     Operation;
        Json::Value     OperationMode;
        Json::Value     OverTime;
        Json::Value     Vehicle;

        /***********************************************************构造JSAO数据结构***********************************************************/
        Debit["Money"] =                    Json::Value(pkItems.Money);
        Debit["PayCertificateCode"] =       Json::Value(pkItems.PayCertificateCode) ;
        Debit["PayCertificateType"] =       Json::Value(pkItems.PayCertificateType) ;
        Debit["PayIdentifier"] =            Json::Value(pkItems.PayIdentifier) ;
        Debit["SubTime"] =                  Json::Value(pkItems.SubTime.Format(_T("%Y-%m-%d %H:%M:%S"))) ;//Format(_T("%Y-%m-%d %H:%M:%S"))
        Debit["Type"] =                     1;

		Entry["AreaID"] =                   Json::Value(pkItems.str_EntryAreaID) ;
		Entry["LaneID"] =                   Json::Value(pkItems.str_EntryLane) ;
        Entry["License"] =                  Json::Value(pkItems.EntryVehicleLicense) ;
        Entry["RoadID"] =                   Json::Value(pkItems.str_EntryRoadID);//modify 2018-01//Json::Value(pkItems.EntryRoadID) ;
        Entry["StationID"] =                Json::Value(pkItems.str_EntryStationID);//Json::Value(pkItems.EntryStationID) ;
        Entry["StationName"] =              Json::Value(pkItems.EntryStationName) ;
        Entry["Time"] =                     Json::Value(pkItems.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S"))) ;
        Entry["VClass"] =                   Json::Value(pkItems.EntryVehicleClass) ;
        Entry["VColor"] =                   Json::Value(pkItems.EntryVLColor );
        Entry["VType"] =                    Json::Value(pkItems.EntryVehicleType) ;

        Operation["CardID"] =               Json::Value(pkItems.CardID) ;
        Operation["PassCertificateType"] =  Json::Value(pkItems.PassCertificateType) ;
        Operation["ShiftID"] =              Json::Value(pkItems.ShiftID );
        Operation["TollDate"] =             Json::Value(pkItems.TollDate );
        Operation["OperatorID"] =               Json::Value(pkItems.OperatorID );
        Operation["OperatorName"] =             Json::Value(pkItems.OperatorName );
		Operation["TickNo"]		 =				Json::Value(pkItems.TicketNo );
		


        Vehicle["AxisCount"] =              Json::Value(pkItems.AxisCount) ;
        Vehicle["Class"] =                  Json::Value(pkItems.VehicleClass) ;
        Vehicle["License"] =                Json::Value(pkItems.VehicleLicense );
        Vehicle["Type"] =                   Json::Value(pkItems.VehicleType );
        Vehicle["VLColor"] =                Json::Value(pkItems.VLColor );
        Vehicle["Weight"] =                 Json::Value(pkItems.TotalWeight) ;
		Json::Value autoPlateN;

		autoPlateN["AutoPlate"]		 =				Json::Value(pkItems.AutoPlate );
		autoPlateN["AutoPlateColor"]	 =				Json::Value(pkItems.AutoPlateColor );
		Vehicle["AutoPlate"] = autoPlateN;



        Data["Debit"] = Debit;
        Data["Entry"] = Entry;
        Data["Operation"] = Operation;
        Data["OperationMode"] =             Json::Value(pkItems.OperationMode);
        Data["OverTime"] =                  Json::Value(pkItems.OverTime);
        Data["Vehicle"] = Vehicle;
		Data["PayModel"] = pkItems.PayModel;

        DebitParamContext["Version"] =      Json::Value(_T("1.0"));
        DebitParamContext["Data"] = Data;
        root["DebitParamContext"] = DebitParamContext;

        //转化为字符串
        Json::FastWriter fast_writer;
        strReturn.Format(_T("%s"), fast_writer.write(root).c_str());

    }

    return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::ConstructCancleInfo
//
// 功能描述：构造发送到串口的数据格式 -- 撤单处理
//
// 输入参数：int nFormatType
//         ：CString strPayID
// 输出参数：CString
// 编写人员：林伙泉
// 编写时间：2017年2月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::ConstructCancleInfo(int nFormatType, CString strPayID)
{
    /*2017-05-31 zhuyabing 恢复了KeyItem节点*/
    CString strReturn;

    Json::Value root;
    Json::Value DebitCancelParamContext;
    Json::Value Data;
    Json::Value keyItem;


    keyItem["PayIdentifier"] = Json::Value(strPayID);
    keyItem["Type"] = Json::Value(1);
    Data["TradeKeyItem"] = keyItem;

    DebitCancelParamContext["Data"]     = Data;
    DebitCancelParamContext["Version"]  = Json::Value(_T("1.0"));

    root["DebitCancelParamContext"]     = DebitCancelParamContext;

    //转化为字符串
    Json::FastWriter fast_writer;
    strReturn.Format(_T("%s"), fast_writer.write(root).c_str());

    return strReturn;
}

/**
 * 节目控制的临时转换函数，用于生成json格式
 * 目前仅仅支持清屏
 * @param  strOrgString
 * @return
 * 2017-05-19
 */
CString TmpScreenOnlyJson(const CString& strOrgString, int type)
{
    CString strParamString(strOrgString);
    CString result;

    if (type == 1)
    {
        CDVXMLMMI xml;
        if (!xml.Init(strParamString))
        {
            return result;
        }
        if (xml.ClearMMI == 1)
        {
            return "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
        }
        else if (xml.ClearMMI == 0)
        {   //进行车辆的费显显示
            Json::Value  rootH, typeH, mmiH, dataH, versionH;
            Json::Value moneyH, vtypeH, vclassH, weightH, axisH, weightLimitH, balanceH, stationNameH;
            for (int i = 0; i < xml.Text.GetSize(); ++i)
            {
                int iTemp = 0x00;
                switch (xml.Text.GetAt(i).Sub)
                {
                case 1://车型
                {
                    int tmpVal = _ttoi(xml.Text.GetAt(i).Context);
                    vtypeH = Json::Value( tmpVal / 10);
                    vclassH = Json::Value(tmpVal % 10);
                }
                break;
                case 2://金额
                {
                    moneyH =  Json::Value(_ttoi(xml.Text.GetAt(i).Context));
                }
                break;
                case 3://载重
                {
                    weightH = Json::Value(_ttoi(xml.Text.GetAt(i).Context));


                }
                break;
                case 4://轴数
                {
                    axisH = Json::Value(_ttoi(xml.Text.GetAt(i).Context));

                }
                break;
                case 5://限重
                {
                    weightLimitH = Json::Value( _ttoi(xml.Text.GetAt(i).Context));
                }
                break;
                case 6://电子支付卡余额
                {
                    balanceH = Json::Value(_ttoi(xml.Text.GetAt(i).Context));

                }
                break;
                case 7://入口收费站点名称
                {
                    stationNameH = Json::Value((xml.Text.GetAt(i).Context));
                }
                break;
                }
            }
            dataH["Money"] = moneyH;
            dataH["VType"] = vtypeH;
            dataH["VClass"] = vclassH;
            dataH["Balance"] = balanceH;
            dataH["AxisCount"] = axisH;

            dataH["Weight"] = weightH;
            dataH["LimitWeight"] = weightLimitH;
            dataH["StationName"] = stationNameH;


            versionH = Json::Value("1.0");
            mmiH["Data"] = dataH;
            mmiH["version"] = versionH;

            rootH["Type"] = Json::Value("203");
            rootH["MMIParamContext"] = mmiH;

            Json::FastWriter writerJ;
            result.Format(_T("%s"), writerJ.write(rootH).c_str());
            return result;


        }
        else
        {
            return result;
        }
    }
    else if (type == 2)
    {
        CDVJsonParamMMI* pJsonMMI = NULL;
        CJsonParamOperate jsonOperation;


        pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString, JSON_DT_PARAM_MMI);
        if (NULL == pJsonMMI)
        {
            return result;
        }
        if (pJsonMMI->ClearMMI == 1)
        {
            return "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
        }
        if (pJsonMMI->ClearMMI != 0)
        {
            return result;
        }
        Json::Value  rootH, typeH, mmiH, dataH, versionH;
        Json::Value moneyH, vtypeH, vclassH, weightH, axisH, weightLimitH, balanceH, stationNameH;
        for (int i = 0; i < pJsonMMI->Text.GetSize(); ++i)
        {
            switch (pJsonMMI->Text.GetAt(i).Sub)
            {
            case 1://车型
            {
                int tmpVal = _ttoi(pJsonMMI->Text.GetAt(i).Context);
                vtypeH = Json::Value(tmpVal / 10);
                vclassH = Json::Value(tmpVal % 10);
            }
            break;
            case 2://金额
            {
                moneyH = Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));

            }
            break;
            case 3://载重
            {
                weightH =  Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));


            }
            break;
            case 4://轴数
            {
                axisH = Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));


            }
            break;
            case 5://限重
            {
                weightLimitH =  Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));

            }
            break;
            case 6://电子支付卡余额
            {
                balanceH = Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));

            }
            break;
            case 7://入口收费站点名称
            {
                stationNameH = Json::Value(pJsonMMI->Text.GetAt(i).Context);
            }
            break;
            }
        }
        dataH["Money"] = moneyH;
        dataH["VType"] = vtypeH;
        dataH["VClass"] = vclassH;
        dataH["Balance"] = balanceH;
        dataH["AxisCount"] = axisH;

        dataH["Weight"] = weightH;
        dataH["LimitWeight"] = weightLimitH;
        dataH["StationName"] = stationNameH;


        versionH = Json::Value("1.0");
        mmiH["Data"] = dataH;
        mmiH["version"] = versionH;

        rootH["Type"] = Json::Value("203");
        rootH["MMIParamContext"] = mmiH;

        Json::FastWriter writerJ;
        result.Format(_T("%s"), writerJ.write(rootH).c_str());
        return result;
    }
    else
    {
        return result;
    }
    result = "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
    return result;

}

CString CTWSDNetPay_DllApp::GetClearScreenJson()
{
    return "{\"Type\":\"204\",\"MMIParamContext\":{\"Data\":{\"Value\":\"\"},\"Version\":\"1.0\"}}";
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTWSDNetPay_DllApp::ConstructMMIInfo
//
// 功能描述：构造发送到串口的数据格式 -- 费显处理
//
// 输入参数：int nFormateType
//         ：IN const CString& strOrgString
//         ：OUT BOOL& bClearCmd
// 输出参数：BOOL
// 编写人员：林伙泉
// 编写时间：2017年2月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::ConstructMMIInfo(int nFormateType, IN const CString& strOrgString)
{
    return TmpScreenOnlyJson(strOrgString, nFormateType);

    CString strParamString(strOrgString);
    CString strMMIString;
    if (0x01 == nFormateType)
    {
        //XML格式
        CDVXMLMMI xml;
        if (!xml.Init(strParamString))
        {
            strMMIString.Empty();
            return FALSE;
        }

        //重构XML数据
        CMarkup xmlShow;
        xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
        xmlShow.AddElem( _T("DisplayContext") );//root
        xmlShow.IntoElem();

        xmlShow.AddElem( _T("Version"),     _T("1.0") );
        xmlShow.AddElem( _T("ProvinceID"),  m_sTranstParam.ProvinceID );
        xmlShow.AddElem( _T("VerifyCode"),  0 );
        xmlShow.AddElem( _T("Data") );

        xmlShow.IntoElem();
        if (xml.ClearMMI > 0)
        {
            int iMMIType = 0x04;
            switch (xml.ClearMMI)
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
            xmlShow.AddElem( _T("Type"),        iMMIType );
        }
        else
        {
            xmlShow.AddElem( _T("Type"),        0x01 );
            //获取金额、车型车种、重量、限重
            int iMoney  = 0x00;
            int iWeight = 0x00;
            int iVType  = 0x00;
            int iVClass = 0x00;
            int iLimit  = 0x00;
            int iAxis   = 0x00;
            int iBalance = 0x00;
            CString strEntrySTName;
            for (int i = 0; i < xml.Text.GetSize(); ++i)
            {
                int iTemp = 0x00;
                switch (xml.Text.GetAt(i).Sub)
                {
                case 1://车型   Vtype 0客 1货   //车种 Vclass  01~FF
                {
                    iTemp = _ttoi(xml.Text.GetAt(i).Context);

                    if (iTemp > 10)
                    {
                        iVType = 0x00;
                    }
                    else
                    {
                        iVType = 0x01;
                    }
                    iVClass = iTemp % 10;

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

            xmlShow.AddElem( _T("Money"),           iMoney );
            xmlShow.AddElem( _T("Weight"),          iWeight );
            xmlShow.AddElem( _T("VehicleType"),     iVType );
            xmlShow.AddElem( _T("VehicleClass"),    iVClass );
            xmlShow.AddElem( _T("LimitWeight"),     iLimit );
            xmlShow.AddElem( _T("AixsCount"),       iAxis );
            xmlShow.AddElem( _T("Balance"),         iBalance );
            xmlShow.AddElem( _T("EntrySTName"),     strEntrySTName );
            xmlShow.AddElem( _T("CurrentTime"),     CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") );
        }
        xmlShow.OutOfElem();
        xmlShow.OutOfElem();

        strMMIString = xmlShow.GetDoc();
    }
    else
    {
        //JSON格式
        CDVJsonParamMMI* pJsonMMI = NULL;
        CJsonParamOperate jsonOperation;

        pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString, JSON_DT_PARAM_MMI); //需要释放内存
        if (NULL == pJsonMMI)
        {
            strMMIString.Empty();
            return FALSE;
        }

        Json::Value root;
        Json::Value MMIParamContext;
        Json::Value Data;

        MMIParamContext["Version"]  = Json::Value(pJsonMMI->Version);
        MMIParamContext["ClearMMI"] = Json::Value(pJsonMMI->ClearMMI);

        if (pJsonMMI->ClearMMI > 0)
        {
            //重构为XML数据
            CMarkup xmlShow;
            xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
            xmlShow.AddElem( _T("DisplayContext") );//root
            xmlShow.IntoElem();

            xmlShow.AddElem( _T("Version"),     _T("1.0") );
            xmlShow.AddElem( _T("ProvinceID"),  m_sTranstParam.ProvinceID );
            xmlShow.AddElem( _T("VerifyCode"),  0 );
            xmlShow.AddElem( _T("Data") );

            xmlShow.IntoElem();

            int iMMIType = 0x04;
            switch (pJsonMMI->ClearMMI)
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
            xmlShow.AddElem( _T("Type"),        iMMIType );

            xmlShow.OutOfElem();
            xmlShow.OutOfElem();

            strMMIString = xmlShow.GetDoc();
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
            int iBalance = 0x00;
            CString strEntrySTName;
            for (int i = 0; i < pJsonMMI->Text.GetSize(); ++i)
            {
                int iTemp = 0x00;
                switch (pJsonMMI->Text.GetAt(i).Sub)
                {
                case 1://车型   Vtype 0客 1货   //车种 VClass  01~FF
                {
                    iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

                    if (iTemp > 10)
                    {
                        iVType = 0x00;
                    }
                    else
                    {
                        iVType = 0x01;
                    }
                    iVClass = iTemp % 10;

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
            strMMIString.Format(_T("%s"), fast_writer.write(root).c_str());
        }

        delete pJsonMMI;
        pJsonMMI = NULL;
    }
    return strMMIString;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：NotifyCaller
//
// 功能描述：通知外部调用者，业务被异步处理得到的结果
//
// 输入参数：IN const int& iOption             -- 通知类型（1，扣款；2，撤单）
//         ：IN const int& iResult             -- 处理结果，见开发文档
// 输出参数：无
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：接口1
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::NotifyCaller(IN const int& iOption, IN const int& iResult)
{
    switch (m_iNotifyMode)
    {
    case 1:
    {
        //通知调用者返回结果，当前操作失败
        if (0 != m_sTranstParam.CallerThreadID)
        {
            ::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(iOption, iResult), MAKEWORD(0, 0)), (LPARAM)0);
        }

        if (::IsWindow(m_sTranstParam.CallerWnd))
        {
            ::PostMessage(m_sTranstParam.CallerWnd, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(iOption, iResult), MAKEWORD(0, 0)), (LPARAM)0);
        }
    }
    break;
    case 2:
    {
        //使用回调函数通知外部调用者
        if (NULL != m_fnCallbackNotifyFun)
        {
            m_fnCallbackNotifyFun(iOption, iResult);
        }
        else
        {
            RecordLog(FmtStr(_T("[主进程响应],组件通知外部调用者的回调函数(NotifyCaller)指针为空，无法通知调用者")), LOG_LEVEL_ERROR, true);
        }
    }
    break;
    case 3:
    {
        //使用回调函数通知外部调用者(广西特供)
        if (NULL != m_fnCallbackNotifyFun45)
        {
            m_fnCallbackNotifyFun45(iOption, iResult, m_pIndPtrFor45);
        }
        else
        {
            RecordLog(FmtStr(_T("[主进程响应],组件通知外部调用者的回调函数(NotifyCaller45)指针为空，无法通知调用者")), LOG_LEVEL_ERROR, true);
        }
    }
    break;
    default:
        break;
    }
}

int CTWSDNetPay_DllApp::DevcieQueryInterrupt(const char* para)
{
	std::string unusePara;
	std::string testResult;
	GetGlobalApp()->m_pThreadManage->m_pThreadDevice->HardwareTestGreenChannel(1,unusePara,testResult);
	char chsResult[1000]={0};
	strncpy(chsResult,testResult.c_str(),800);
	send2Bus(KEY_DEVICE_TEST,chsResult,strlen(chsResult),KEY_BUSINESS);
	return 0;
}


int CTWSDNetPay_DllApp::recvFromBus(int destKey, char *msg, int len, int srcKey )
{
    if (srcKey == KEY_COMMON_FRAME)
    {
        int pay_diff = time(NULL) - m_lastPayTime;
        char chs_debug[10] = {0};
        sprintf(chs_debug, "%d", pay_diff);
        
        if (  pay_diff > 60) //超过一分钟没有收到交易请求
        {
            
            return 1;
        }
    }
	else if(srcKey == KEY_DEVICE_TEST)
	{
		 //硬件测试查询
        DevcieQueryInterrupt(msg);
	}
    else if(srcKey==24)
    {//不停车接入
        std::string hexStr = Bin2Hex((unsigned char*)msg,(size_t)len);
		//MessageBox(0,hexStr.c_str(),"",0);
        m_pThreadManage->m_pThreadDevice->ExcuteComCommond(hexStr);
    }

	//std::string sss= "key:" + Int2str(srcKey);
	//MessageBox(0,sss.c_str(),"",0);
    //add 2018-04 增加对接入模块的处理
    if(KEY_COMMONBUS_ACCESS == srcKey)
    {
        char * para_msg=  new char[1000];
        memset(para_msg,0,1000);
        strncpy(para_msg,m_CommonInitPara.c_str(),200);
        send2Bus(srcKey,para_msg,strlen(para_msg),destKey)   ;
        delete para_msg;
        para_msg=NULL;
    }
    return 0;
}


// CommonBus*  WINAPI CTWSDNetPay_DllApp::getInstance()
// {
//     return static_cast<CommonBus*>(GetGlobalApp());
// }
// CommonBus*  WINAPI CTWSDNetPay_DllApp::deleteInstance()
// {
//     return NULL;/*do nothing*/
//     // return static_cast<CommonBus*>GetGlobalApp();
// }


int RealCalTimeZone()
{
	struct tm time_local, time_utc;
	time_t t;
	t = time(NULL);
	time_local = *localtime(&t);
	time_utc = *gmtime(&t);
	return  ((time_local.tm_mday-time_utc.tm_mday)*24 +  time_local.tm_hour - time_utc.tm_hour);
}

//东八区返回8，
int GetCurrentTimeZone()
{
	static int time_zone = RealCalTimeZone();
	return time_zone;
}



/*通讯库中，默认当前时区为UTC8，如果当前时区为非UTC8，则在通讯库处理后，时间（所谓UTC8时间）会相应的增加或者减少
在此函数中，会将此影响进行抵消
函数作用：以UTC8时区作为标准，根据当前时区调整时间参数，比如如果当前时区为UTC0，则实际时间应该就是 （当前时间-8），也就是UTC时区所应该拥有的正确时间
*/
CTime CTWSDNetPay_DllApp::FixUTC8(const CTime& old_time)
{
	if(m_sTranstParam.LaneConfigInfo.ForceUTC8==0)
	{
		return old_time;
	}
	int time_diff_hour = 8 - GetCurrentTimeZone();
	CTime new_time ; 
	if(time_diff_hour >=0)
	{
		new_time = old_time - CTimeSpan(0,time_diff_hour,0,0);
	}
	else
	{
		new_time = old_time + CTimeSpan(0,-time_diff_hour,0,0);
	}
	
	
	return new_time;
}
/*将UTC8时间调整为当前时区的时间，
如果当前时区为UTC0，则实际时间应该是 （old_time + 8),因为old_time本应该根据UTC8时区进行解释，却实际被当成了UTC0时区，这样会少掉8个小时

*/
time_t CTWSDNetPay_DllApp::FixUTC(const time_t& old_time)
{
	if(m_sTranstParam.LaneConfigInfo.ForceUTC8==0)
	{
		return old_time;
	}
	time_t new_time = old_time + (8-GetCurrentTimeZone())*60*60;
	return new_time;
}




//add 2018-03，此处进行commonbus的兼容处理

int recvFromBus_X(int destKey, char *msg, int len, int srcKey)
{
    return GetGlobalApp()->recvFromBus(destKey,msg,len,srcKey);
}




void CTWSDNetPay_DllApp::ConnectBus()
{
    std::string base_dir = fs_thisModulePath();
	
    std::string commonBusPath = pystring::os::path::join( base_dir,"CommonBus.dll");
    HMODULE tmp_module = ::LoadLibrary(commonBusPath.c_str());
    if(tmp_module == NULL)
    {
		RecordLog("Connect CommonBus fail");
        return ; //no such dll
    }
	RecordLog("Connect CommonBus Success ");
    
    m_connect_bus_core = (NewBusConnector)::GetProcAddress(tmp_module,"NewBusConnector");
    m_disconnect_bus_core = (RemoveBusConnector)::GetProcAddress(tmp_module,"RemoveBusConnector");
    m_send_core = (send2Bus_Target)::GetProcAddress(tmp_module,"send2Bus_Target");
    m_register_core = (register2Bus_Target)::GetProcAddress(tmp_module,"register2Bus_Target");
    m_unRegister_core= (unRegister2Bus_Target)::GetProcAddress(tmp_module,"unRegister2Bus_Target");
    if(m_connect_bus_core!=NULL)
    {
        m_bushandle_core = m_connect_bus_core(recvFromBus_X);
    }
    if(m_send_core ==NULL
        || m_register_core==NULL
        ||m_unRegister_core == NULL)
    {
        ;
    }
    m_commonbus_module=tmp_module;
    

}

void CTWSDNetPay_DllApp::DisConnectBus()
{
    if(m_disconnect_bus_core!=NULL)
    {
        m_disconnect_bus_core(m_bushandle_core);
    }
}
void CTWSDNetPay_DllApp::register2Bus(int key)
{
    if(m_register_core!=NULL)
    {
        m_register_core(m_bushandle_core,key);
    }
}
void CTWSDNetPay_DllApp::unRegister2Bus(int key)
{
    if(m_unRegister_core!=NULL)
    {
        m_unRegister_core(m_bushandle_core,key);
    }

}
int CTWSDNetPay_DllApp::send2Bus(int destKey,char *msg,int len,int srcKey)
{
    if(m_send_core!=NULL)
    {
        return m_send_core(m_bushandle_core,destKey,msg,len,srcKey);
    }
    return -1;
}


bool TicketNoXML2Json(const std::string& xml_str, std::string& json_str)
{
    CMarkup xml;
    xml.SetDoc( xml_str.c_str() );
    xml.ResetMainPos();

    xml.ResetMainPos();
    if (!xml.FindElem(_T("CompletionTicketNo")))
    {
        theApp.RecordLog("CompletionTicketNo not found");
        return false;
    }

    xml.IntoElem();

    xml.ResetMainPos();
    if (!xml.FindElem(_T("Data")))
    {
        theApp.RecordLog("Data not found");
        return false;
    }

    xml.IntoElem();

    xml.ResetMainPos();
    if (!xml.FindElem(_T("TicketInfo")))
    {
        theApp.RecordLog("TicketInfo not found");
        return false;
    }

    xml.IntoElem();

    
    std::string pay_id,tick_no;
    

    xml.ResetMainPos();

    if (!xml.FindElem(_T("PayIdentifier")))
    {
        theApp.RecordLog("PayIdentifier not found");
        return false;
    }
    
    pay_id = std::string((LPCTSTR)xml.GetData());



    xml.ResetMainPos();
    if (!xml.FindElem(_T("No")))
    {
        theApp.RecordLog("No not found");
        return false;
    }

    tick_no = std::string((LPCTSTR)xml.GetData());

   

    //进行json的封装
    Json::Value rootN, contextN, dataN;

    dataN["PayIdentifier"] = Json::Value(pay_id);
    dataN["No"] = Json::Value(tick_no);
    contextN["Data"] = dataN;
    contextN["Version"] = Json::Value("1.0");
    rootN["CompletionTicketNo"] = contextN;
    Json::FastWriter JFW;

    json_str = JFW.write(rootN);

    return true;
}

bool WINAPI CTWSDNetPay_DllApp::IF_CompletionTicketNo(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strLog = FmtStr(_T("[主进程响应],执行票据接口(IF_CompletionTicketNo)调用开始,参数1:%s,参数2:%d,参数3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);
    std::string paraStr;
    if (iFormatType == 1) //xml
    {
        if (!TicketNoXML2Json(szParamContext, paraStr))
        {
            RecordLog(("Error Para Info" + std::string(szParamContext)).c_str(), LOG_LEVEL_ERROR);
            return false;
        }
    }
    else if (iFormatType == 2) //json
    {
        paraStr = szParamContext;
    }
    else
    {
        RecordLog("Error Para Info：not json or xml", LOG_LEVEL_ERROR);
        return false;
    }
    if (! m_pThreadManage->m_pThreadDevice->TransTickNo(paraStr))
    {
        RecordLog("IF_CompletionTicketNo 调用失败");
        return false;
    }

    RecordLog("IF_CompletionTicketNo 调用成功");
    return true;

}

void CTWSDNetPay_DllApp::GenerateRawInitPara(const tagTranstParam& para)
{
    Json::Value rootN;
	rootN["AreaID"] = Json::Value(para.LaneConfigInfo.str_AreaID);
	rootN["RoadID"] = Json::Value(para.LaneConfigInfo.str_RoadID);
	rootN["StationID"] = Json::Value(para.LaneConfigInfo.str_StationID);
	rootN["StationName"] = Json::Value(para.LaneConfigInfo.StationTab);
	rootN["szLocalLaneID"] = Json::Value(para.LaneConfigInfo.str_LaneID);
	rootN["ServerIP"] = Json::Value(para.LaneConfigInfo.WebServerIP);
	rootN["ServerPort"] = Json::Value(para.LaneConfigInfo.WebServerPort);
	rootN["ProvinceID"] = Json::Value(Int2str(para.ProvinceID));
    
    Json::FastWriter jsonWriter;

    m_CommonInitPara = jsonWriter.write(rootN);
}