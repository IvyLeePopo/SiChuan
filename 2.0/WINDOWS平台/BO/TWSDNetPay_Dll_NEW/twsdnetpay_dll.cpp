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

//Ψһȫ�ֱ���
CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp()  { return &theApp; }

BEGIN_MESSAGE_MAP(CTWSDNetPay_DllApp, CWinApp)
END_MESSAGE_MAP()

/***********************������ʼ��*********************************/
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
// �������ƣ�CString   FmtStr
//
// ����������
//
// ���������const char * lpzFmt
//         ��...
// ��д��Ա��ROCY
// ��дʱ�䣺���ڶ� 2009��9��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CString2time_t
//
// ����������ʱ���ʽ�ַ���ת��Ϊ ������ ��ʱ��
//
// ���������LPCTSTR lpstime
// ���������time_t
// ��д��Ա��ROCY
// ��дʱ�䣺���ڶ� 2009��9��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�GetGuidString
//
// ������������ȡ�µ�GUID�ַ���
//
// ����������޲���
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2012��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CTWSDNetPay_DllApp::GetCurrentTimeString
//
// ������������ȡ��ǰ��ʱ��ֵ
//
// ���������IN const int& iTimeType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��19��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
///��������: MakeDirectory()
///����˵��: ����ָ���Ķ༶Ŀ¼
///����˵��: CString dd,���������Ķ༶Ŀ¼·��
///����˵��: TRUE  -- �����ɹ�
///          FALSE -- ����ʧ��
///�޸�ʱ��: 2006-04-12
///�޸���Ա: ROCY
///��    ע: ��Ҫ���� windows.h
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
//  ��   �� : ROCY
//  ʱ   �� : 2008-06-01
//  ��   �� : 1.0.0.1
//  �������� : GetVersionOfFile
//  �������� : ��ȡָ�����ļ��İ汾��
//  ������� : LPCTSTR  --  lpsPath �ļ��ľ���·��
//  ������� : DWORD -- ���صı��ؼ����IP�������룬�ֽںϳɸ�ʽ
//  ��ע˵�� : ��
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

    //��ȡ�����ļ�
    ReadConfigInfo();

    //������־Ŀ¼
    strTemp.Format(_T("%s\\TWSDNetPayLog"), m_strWorkDir);
    _tcscpy_s(m_sTranstParam.PathLogDir, _countof(m_sTranstParam.PathLogDir), strTemp.GetBuffer(0));
    strTemp.ReleaseBuffer();
    MakeDirectory(m_sTranstParam.PathLogDir);
    //��ȡ����汾
    CString strDllFilePath(szTempDir);
    m_strNetPayDllVersion = GetVersionOfFile(strDllFilePath);

    strDllFilePath.Format(_T("%s\\%s"), strDir, FILE_NAME_DEVICE_DLL);
    m_strDeviceDllVersion = GetVersionOfFile(strDllFilePath);

    //��ʼ����־ϵͳ
    if (m_bSaveLog)
    {
        INIT_LOG(m_sTranstParam.PathLogDir, _T(""));

        RecordLog(FmtStr(_T("\r\n/*************************************��������(ҵ��%s - �豸%s)*************************************/"), m_strNetPayDllVersion, m_strDeviceDllVersion));
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




/****************************************����ӿں���********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_InitEnvironment
//
// ������������ʼ���������
//
// ���������IN const UINT& nThreadID              -- ������ָ�����߳�ID
//         ��IN const HWND& hWnd                   -- ������ָ���Ĵ�����
//         ��IN const unsigned int& nNotifyMsgID   -- ������ָ����֪ͨ��ϢID
//         ��IN const char* szAreaInfo             -- ������Ϣ(�������#·�κ�)
//         ��IN const char* szLoaclStation         -- վ����Ϣ(վ��#վ��)
//         ��IN const char* szLoaclLaneID          -- ��������
//         ��IN const char* szServerInfo           -- ��������Ϣ
//         ��IN const int& iProvinceID             -- ʡ�ݱ���
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment(IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());


    bool bReturn = false;
    CString strLog;

    RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ÿ�ʼ")));
    try
    {
        /*******************���������Ƿ�����*******************/
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
        //���������Ϣ
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

        //���⴦���������
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //�㶫����ĳ���վ�����⴦��
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
		

        //��ȡ�ƶ�ͨѶIP��ַ�Լ����Ŷ˿�
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
        //��ȡƽ̨�ڲ�IP������Ϣ
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

        //�ж��Ƿ��Ѿ���ʼ��
        if ( m_bIsInitEnvironment )
            throw - 11;

        RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%s)"), strSTName, m_sTranstParam.ProvinceID, m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, strSTID));

        GenerateRawInitPara(m_sTranstParam);
        //���кϷ���У��
        m_bAuthorizedLegal = TRUE;
        //����ҵ������߳�
        CString strConfigFilePath;
        m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (NULL == m_pThreadManage)
            throw - 12;
        strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam, NULL);
        //�����߳����»���
        m_pThreadManage->ResumeThread();

        //��ǳ�ʼ��״̬
        m_bIsInitEnvironment = TRUE;

        //���֪ͨ��ʽ
        m_iNotifyMode = 1;

        //���ؽ��
        bReturn = m_bIsInitEnvironment ? true : false;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴����߳�ID�봰������Ϊ0"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴���֪ͨ��������ϢIDΪ0"));
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴���������Ϣ�Ƿ�"));
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴���վ�������Ϣ�Ƿ�"));
            break;
        case -5:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴��복��������Ϣ�Ƿ�"));
            break;
        case -6:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴����������Ϣ�Ƿ�"));
            break;
        case -7:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�������������󣬴���ʡ�ݱ���Ϊ0"));
            break;
        case -8:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),��������������,����������߳�ID�ʹ���������ͬʱΪ��"));
            break;
        case -9:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),��������������,�������������ʽ����ȷ"));
            break;
        case -10:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),��������������,����վ����Ϣ������ʽ����ȷ"));
            break;
        case -11:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),����������γ�ʼ��"));
            break;
        case -12:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�ڲ���ʼ������,�޷�����ҵ������߳�"));
            break;
        }
        //��¼��־
        if (m_strLastErrorDesc != strLog)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }

    RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ý���")));

    strLog.Format(_T("��ʼ�������%d"), bReturn);

    RecordLog(strLog);

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_InitEnvironment2
//
// ������������ʼ���������
//
// ���������IN const UINT& nThreadID              -- ������ָ�����߳�ID
//         ��IN const HWND& hWnd                   -- ������ָ���Ĵ�����
//         ��IN const char* szAreaInfo             -- ������Ϣ(�������#·�κ�)
//         ��IN const char* szLoaclStation         -- վ����Ϣ(վ��#վ��)
//         ��IN const char* szLoaclLaneID          -- ��������
//         ��IN const char* szServerInfo           -- ��������Ϣ
//         ��IN const int& iProvinceID             -- ʡ�ݱ���
//         ��void (*fun)(int option, int result)   -- �ۿ�/�������֪ͨ���ص�����
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��04��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1-1
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment2 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, void (*fun)(int option, int result))
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;

    RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2)���ÿ�ʼ")));
    try
    {
        /*******************���������Ƿ�����*******************/
        if (  (0 == nThreadID) && (NULL == hWnd))
            throw - 1;
        if (NULL == fun)
            throw - 2;
        //�ص�����ָ�븳ֵ������֪ͨ������
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
        //���������Ϣ
        TCHAR   szTemp[256] = {0};
        CString strTemp;
        CString strSTID;

        //������ʼ��������ֵ
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

        //���⴦���������
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            CString strSTID;
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //�㶫����ĳ���վ�����⴦��
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

        //��ȡ���������IP��ַ�Լ����Ŷ˿�
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

        RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%s)"), strSTName, m_sTranstParam.ProvinceID, m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, strSTID));

        GenerateRawInitPara(m_sTranstParam);
        //���кϷ���У��
        m_bAuthorizedLegal = TRUE;
        //����ҵ������߳�
        CString strConfigFilePath;
        m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (NULL == m_pThreadManage)
            throw - 12;
        strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam, NULL);
        //�����߳����»���
        m_pThreadManage->ResumeThread();

        //��ǳ�ʼ��״̬
        m_bIsInitEnvironment = TRUE;

        //���֪ͨ��ʽ
        m_iNotifyMode = 2;

        //���ؽ��
        bReturn = m_bIsInitEnvironment ? true : false;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴����߳�ID�봰������Ϊ0"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴���ص�����ָ�����"));
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴���������Ϣ�Ƿ�"));
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴���վ�������Ϣ�Ƿ�"));
            break;
        case -5:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴��복��������Ϣ�Ƿ�"));
            break;
        case -6:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴����������Ϣ�Ƿ�"));
            break;
        case -7:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�������������󣬴���ʡ�ݱ���Ϊ0"));
            break;
        case -8:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),��������������,����������߳�ID�ʹ���������ͬʱΪ��"));
            break;
        case -9:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),��������������,�������������ʽ����ȷ"));
            break;
        case -10:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),��������������,����վ����Ϣ������ʽ����ȷ"));
            break;
        case -11:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),����������γ�ʼ��"));
            break;
        case -12:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2),�ڲ���ʼ������,�޷�����ҵ������߳�"));
            break;
        }
        //��¼��־
        if (m_strLastErrorDesc != strLog)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }

    RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2)���ý���")));

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_InitEnvironment3
//
// ������������ʼ���������
//
// ���������IN const UINT& nThreadID              -- ������ָ�����߳�ID
//         ��IN const HWND& hWnd                   -- ������ָ���Ĵ�����
//         ��IN const char* szAreaInfo             -- ������Ϣ(�������#·�κ�)
//         ��IN const char* szLoaclStation         -- վ����Ϣ(վ��#վ��)
//         ��IN const char* szLoaclLaneID          -- ��������
//         ��IN const char* szServerInfo           -- ��������Ϣ
//         ��IN const int& iProvinceID             -- ʡ�ݱ���
//         ��IN OUT void* pIndPtr                  -- ���Ի�ָ��
//         ��void (*fun)(int option, int result,void* pIndPtr) -- �ۿ�/�������֪ͨ���ص�����
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��03��16��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1-2��ר��Ϊ�������Ƶĳ�ʼ����������ʼ��������ʱ�򣬱���һ�����Ի�ָ�룬�ڻص���ʱ�򴫳�ȥ
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment3 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, IN OUT void* pIndPtr, void (*fun)(int option, int result, void* pIndPtr))
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;

    RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3)���ÿ�ʼ,���Ի�ָ�����:%04X"), pIndPtr));
    try
    {
        /*******************���������Ƿ�����*******************/
        if (  (0 == nThreadID) && (NULL == hWnd))
            throw - 1;
        if (NULL == fun)
            throw - 2;
        //�ص�����ָ�븳ֵ������֪ͨ������
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
        //���������Ϣ
        TCHAR   szTemp[256] = {0};
        CString strTemp;
        CString strSTID;

        //������ʼ��������ֵ
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


        //���⴦���������
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            CString strSTID;
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //�㶫����ĳ���վ�����⴦��
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

        //���Ͳ���������һ�����Ի�ָ��
        m_pIndPtrFor45 = pIndPtr;

        //��¼��־
        RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%s)"), strSTName, m_sTranstParam.ProvinceID, m_sTranstParam.LaneConfigInfo.AreaID, m_sTranstParam.LaneConfigInfo.RoadID, strSTID));

        GenerateRawInitPara(m_sTranstParam);
        //���кϷ���У��
        m_bAuthorizedLegal = TRUE;
        //����ҵ������߳�
        CString strConfigFilePath;
        m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        if (NULL == m_pThreadManage)
            throw - 12;
        strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadManage->SetParam(&m_sTranstParam, NULL);
        //�����߳����»���
        m_pThreadManage->ResumeThread();

        //��ǳ�ʼ��״̬
        m_bIsInitEnvironment = TRUE;

        //���֪ͨ��ʽ
        m_iNotifyMode = 3;

        //���ؽ��
        bReturn = m_bIsInitEnvironment ? true : false;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴����߳�ID�봰������Ϊ0"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴���ص�����ָ�����"));
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴���������Ϣ�Ƿ�"));
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴���վ�������Ϣ�Ƿ�"));
            break;
        case -5:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴��복��������Ϣ�Ƿ�"));
            break;
        case -6:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴����������Ϣ�Ƿ�"));
            break;
        case -7:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�������������󣬴���ʡ�ݱ���Ϊ0"));
            break;
        case -8:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),��������������,����������߳�ID�ʹ���������ͬʱΪ��"));
            break;
        case -9:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),��������������,�������������ʽ����ȷ"));
            break;
        case -10:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),��������������,����վ����Ϣ������ʽ����ȷ"));
            break;
        case -11:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),����������γ�ʼ��"));
            break;
        case -12:
            strLog.Format(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3),�ڲ���ʼ������,�޷�����ҵ������߳�"));
            break;
        }
        //��¼��־
        if (m_strLastErrorDesc != strLog)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }

    RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3)���ý���")));

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_Destroy
//
// ����������������Դ
//
// ���������void
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�2
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
        /**********************************************��������߳���Դ**********************************************/

        strLog.Format(_T("[��������Ӧ],�����Դ����ӿ�(IF_Destroy),���ڿ�ʼ�˳���̨�߳�..."));
        //��¼��־
        RecordLog(strLog);

        //�߳��˳����
        DWORD dwThreadExitFlag = 0x00;
        //�������ԵĴ���
        int iAllowTryCount = 10;
        //�Ѿ����Դ���
        int iHaveTryCount = 0;
        //ÿ�����Ե�ʱ����ʱ�䣨��λ�����룩
        DWORD   dwSpanTime = 100;
        m_bIsInitEnvironment = false;

        //ֹͣҵ������߳�
        if (NULL != m_pThreadManage && !(m_pThreadManage->m_bExit))
        {
            m_pThreadManage->Destroy_Dev();
			m_pThreadManage->ThreadQuit();
            //���ø��߳��Ƴ����Ϊ��
            //m_pThreadManage->m_bExit = TRUE;
            //��Ĭ50����
            ::Sleep(50);
            //ÿ�εȴ�1000���룬���Դ���������10��
            while (OP_SUCCESS != fnCloseThread(m_pThreadManage->m_hThread, 1000))
            {
                Sleep(dwSpanTime);
                ++iHaveTryCount;
                if (iHaveTryCount >= iAllowTryCount)
                {
                    //��Ǹ��߳�δ�������˳�
                    dwThreadExitFlag |= 0x01;
                    //��ֹ����
                    break;
                }
            }

            strLog.Format(_T("[��������Ӧ],�����Դ����ӿ�(IF_Destroy),%s"), FmtStr(_T("��ҵ������̡߳��Ѿ�ֹͣ(���Դ��� %d)..."), iHaveTryCount));
            //��¼��־
            RecordLog(strLog);
        }


        strLog.Format(_T("[��������Ӧ],��������˳�..."));
        RecordLog(strLog, 0, TRUE);
        //��ǳɹ�
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
    }

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetLastErrorDesc
//
// ������������ȡ��һ�δ�����Ϣ
//
// ���������void
// ���������char*
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�3
//
////////////////////////////////////////////////////////////////////////////////////////////////
const char* WINAPI CTWSDNetPay_DllApp::IF_GetLastErrorDesc(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return m_strLastErrorDesc.GetBuffer(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_StartGetAccountInfo
//
// ����������������ȡ�û���Ϣ����
//
// ���������IN const int& iOvertime
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�4
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_StartGetAccountInfo(IN const int& iOvertime)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;
    RecordLog(FmtStr(_T("[��������Ӧ],������ȡ�û���Ϣ�����ӿ�(IF_StartGetAccountInfo)���ÿ�ʼ")));
    try
    {
        //�ж�����Ƿ��Ѿ�����ʼ��
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�ж�����Ƿ�ɹ���Ȩ
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�ж�ҵ�����߳��Ƿ��쳣
        if (NULL == m_pThreadManage)
            throw - 3;
        //��ҵ������̷߳�����Ϣ��Ҫ�����Ӳ�����л�ȡ�û���Ϣ����
        m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_START_GET_DATA, (WPARAM)iOvertime, (LPARAM)0x01);
        //��ǳɹ�
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        BYTE byFunCode      = 0x03;
        BYTE byExecuteState = RCC_INNER_ERROR;
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],������ȡ�û���Ϣ����(IF_StartGetAccountInfo)����,���δ�ɹ���ʼ��"));
            byExecuteState  = RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],������ȡ�û���Ϣ����(IF_StartGetAccountInfo)����,���δ����Ȩ"));
            byExecuteState  = RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],������ȡ�û���Ϣ����(IF_StartGetAccountInfo)����,����ڲ���Դ�쳣"));
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
        //���������߽��
        if (0 != m_sTranstParam.CallerThreadID)
        {
            ::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byFunCode, byExecuteState), MAKEWORD(0, 0)), (LPARAM)0);
        }

        if (::IsWindow(m_sTranstParam.CallerWnd))
        {
            ::PostMessage(m_sTranstParam.CallerWnd, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byFunCode, byExecuteState), MAKEWORD(0, 0)), (LPARAM)0);
        }
        //������־
        RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ����Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"), byFunCode, byExecuteState));
    }
    RecordLog(FmtStr(_T("[��������Ӧ],������ȡ�û���Ϣ�����ӿ�(IF_StartGetAccountInfo)���ý���")));

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_StopGetAccountInfo
//
// ����������ֹͣ��ȡ�û���Ϣ����
//
// ���������void
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�5
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_StopGetAccountInfo(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;
    RecordLog(FmtStr(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ�����ӿ�(IF_StopGetAccountInfo)���ÿ�ʼ")));
    try
    {
        //�ж�����Ƿ��Ѿ�����ʼ��
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�ж�����Ƿ�ɹ���Ȩ
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�ж�ҵ�����߳��Ƿ��쳣
        if (NULL == m_pThreadManage)
            throw - 3;
        //��ҵ������̷߳�����Ϣ��Ҫ�����Ӳ�����л�ȡ�û���Ϣ����
        m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA, (WPARAM)0, (LPARAM)0);
        //��ǳɹ�
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ����(IF_StopGetAccountInfo)����,���δ�ɹ���ʼ��"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ����(IF_StopGetAccountInfo)����,���δ����Ȩ"));
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ����(IF_StopGetAccountInfo)����,����ڲ���Դ�쳣"));
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ����(IF_StopGetAccountInfo)����,Ӳ��δ���ڹ���״̬"));
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }

    }
    RecordLog(FmtStr(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ�����ӿ�(IF_StopGetAccountInfo)���ý���")));
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_GetAccountInfo
//
// ������������ȡ�û���Ϣ -- �ۿ�����Ϣ
//
// ���������IN OUT char* szParamContext
//         ��IN OUT int& iParamSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�6
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetAccountInfo(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;
    CString strDataString;
    RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ�ӿ�(IF_GetAccountInfo)���ÿ�ʼ,����1:%04X,����2:%d,����3:%d"), szParamContext, iParamSize, iFormatType));
    try
    {
        //�ж�����Ƿ��Ѿ�����ʼ��
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�ж�����Ƿ�ɹ���Ȩ
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�ж�ҵ�����߳��Ƿ��쳣
        if (NULL == m_pThreadManage)
            throw - 3;
        //�жϲ����Ƿ��쳣
        if ((NULL == szParamContext) || (iParamSize < 1024))
            throw - 4;
        if ( (iFormatType > 2) || (iFormatType < 1))
            throw - 5;
        //�ж������Ƿ��Ѿ���ȡ�ɹ�
        if (!m_pThreadManage->GetAccountDataReadyState())
            throw - 6;


        if (strDataString.IsEmpty())
            throw - 7;
        RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ�ӿ�(IF_GetAccountInfo)�������:%s"), strDataString));
        int tmpLen = strDataString.GetLength();
        iParamSize = tmpLen >= iParamSize ? (iParamSize - 1) : tmpLen;

        memcpy(szParamContext, strDataString.GetBuffer(0), iParamSize);
        strDataString.ReleaseBuffer();
        //��ǳɹ�
        bReturn = true;
        //�����û�����׼��״̬Ϊ��
        m_pThreadManage->SetAccountDataReadyState(FALSE);
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],��ȡ�û���Ϣ����(IF_GetAccountInfo)����,���δ�ɹ���ʼ��"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],��ȡ�û���Ϣ����(IF_GetAccountInfo)����,���δ����Ȩ"));
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],��ȡ�û���Ϣ����(IF_GetAccountInfo)����,����ڲ���Դ�쳣"));
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],��ȡ�û���Ϣ����(IF_GetAccountInfo)����,�û�����δ�ܻ�ȡ�ɹ�"));
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }
    RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ�ӿ�(IF_GetAccountInfo)���ý���")));
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_DebitMoney
//
// ����������ִ�пۿ����
//
// ���������IN const char* szParamContext
//         ��IN const int& iParamSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�7
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_DebitMoney(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog = FmtStr(_T("[��������Ӧ],ִ�пۿ�����ӿ�(IF_DebitMoney)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);
    CString strOperationMode;
    CString strDataString;
    CString strAnalyError;
    int nFType;

    try
    {
        /*****************************У�˹���***********************************/
        //�ж�����Ƿ��ʼ���ɹ�
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�жϺϷ���
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�жϲ����Ƿ��쳣
        if ((NULL == szParamContext) || (0 == iParamSize) || (iFormatType > 2 || iFormatType < 1))
            throw - 3;

        strDataString.Format(_T("%s"), szParamContext);
        nFType = iFormatType; //�ײ�ʹ��json��ʽ���� ��1.xml 2.json��

        if (1 == nFType)
        {
            CDVXMLDebitParamContext xml;

            if (!xml.Init(strDataString))
            {
                strAnalyError = xml.m_strLastErrorDesc;
                throw - 4;
            }

            //��������
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

            //��������
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
        //add 2018-01 ,����ʱ��ת�����Ƕ�������ת��Ϊ������
		m_sCurrentDebitInfo.SubTime = FixUTC8(m_sCurrentDebitInfo.SubTime);
		m_sCurrentDebitInfo.EntryTime = FixUTC8(m_sCurrentDebitInfo.EntryTime);
        ////���⴦��ʱʱ�䣨��ֹƽ̨Ӧ�ò����м���ĳ�ʱ��ͬ��),Ҫ��ƽ̨Ӧ�ò㳬ʱʱ����м������3��
        //if(m_sCurrentDebitInfo.OverTime > 3000)
        //{
        //  m_sCurrentDebitInfo.OverTime -= 3000;
        //}

        //���⴦���������
        switch (m_sTranstParam.ProvinceID)
        {
        case 44:
        {
            CString strSTID;
            switch (m_sTranstParam.LaneConfigInfo.RoadID)
            {
            //�㶫��������վ�����⴦��
            case 30:
            case 31:
            default:/*add 2017-11 zhuyabing ,�㶫ʡȫ��ʹ��ƴ��*/
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

        //���ҵ������߳��Ƿ�����
        if (NULL == m_pThreadManage)
            throw - 6;

        //�ж��ϴν����Ƿ����
        if (m_bIsTrading)
            throw - 7;

        //��ǵ�ǰ�������ڽ���
        m_bIsTrading = TRUE;

        /*****************************��Ӧ����***********************************/

        strOperationMode.Format(_T("����ģʽ"));
        m_bCurrentDebitMode = TRUE;


        //������Դ����
        CString* pstrString = new CString();
        if (NULL == pstrString)
            throw - 8;

        //�ع��ۿ�JSON/xml
        nFType = 2;
        pstrString->Format(_T("%s"), CostructDebitInfo( nFType, m_sCurrentDebitInfo));

        //�ر��ϴεĶ�ʱ��
        m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
        //�����ۿʱ��ʱ��
        m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY, m_sCurrentDebitInfo.OverTime);
        //��ʼ��ʱ
        m_dwTradeBeginOvertime = GetTickCount();
        //�����ۿ�����
        m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_START_DEBIT_FLOW, (WPARAM)pstrString, (LPARAM)nFType); //�����ݽ����������͹�ȥ

        //�������
        m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney),ҵ�����ڴ���(%s)"), strOperationMode);

        //��¼��־
        RecordLog(m_strLastErrorDesc);

        //��ǳɹ�
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        BYTE byFunCode      = 0x01;
        BYTE byExecuteState = RCC_INNER_ERROR;
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),���δ�ɹ���ʼ��"), strOperationMode);
            byExecuteState  = RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�Ϸ���У��ʧ��"), strOperationMode);
            byExecuteState  = RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),��������쳣"), strOperationMode);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�������(XML��ʽ)����������ϸ����:%s"), strOperationMode, strAnalyError);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -5:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�������(JSON��ʽ)��������"), strOperationMode);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -6:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�ڲ������쳣:ҵ�����ģ��δ����������"), strOperationMode);
            break;
        case -7:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�ڲ������쳣:�ϴν���δ��ɣ�������Ƶ������"), strOperationMode);
            byExecuteState  = RCC_LAST_TRADE_NOT_END;
            break;
        case -8:
            strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�ڲ������쳣:��Դ����ʧ��"), strOperationMode);
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
        //Ԥ����ʧ�ܣ����������߽��
        ::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
        //������־
        RecordLog(FmtStr(_T("[��������Ӧ],�ۿ����Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"), byFunCode, byExecuteState));
    }
    RecordLog(FmtStr(_T("[��������Ӧ],ִ�пۿ�����ӿ�(IF_DebitMoney)���ý���")));

    return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_DebitCancel
//
// ����������ִ�г�������
//
// ���������IN const char* szParamContext
//         ��IN const int& iParamSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�8
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_DebitCancel(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //MessageBox(0,"break","",0);
    bool bReturn = false;
    CString strLog = FmtStr(_T("[��������Ӧ],ִ�г��������ӿ�(IF_DebitCancel)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);

    CString strDataString;
    CString strPayIdentifier;
    CDVXMLDebitCancel xml;
    try
    {
        /*****************************У�˹���***********************************/
        //�ж�����Ƿ��ʼ���ɹ�
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�жϺϷ���
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�жϲ����Ƿ��쳣
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
        //���ҵ������߳��Ƿ�����
        if (NULL == m_pThreadManage)
            throw - 6;
        //��⳷�����ױ�ʶ�Ƿ�Ϸ�
        strPayIdentifier.TrimLeft();
        strPayIdentifier.TrimRight();
        if (strPayIdentifier.IsEmpty())
            throw - 7;
        //������Դ����
        CString* pstrString = new CString();
        if (NULL == pstrString)
            throw - 8;

        pstrString->Format(_T("%s"), strPayIdentifier);


        m_pThreadManage->SendBreakDebit(strPayIdentifier);

        m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_DEBIT_CANCEL, (WPARAM)pstrString, (LPARAM)iFormatType); //�ѽ����������͹�ȥ

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
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,���δ�ɹ���ʼ��"));
            byExecuteState  = RCC_COM_NOT_INIT;
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�Ϸ���У��ʧ��"));
            byExecuteState  = RCC_COM_ILLEGAL;
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,��������쳣"));
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�����������������ϸ����:%s"), xml.m_strLastErrorDesc);
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -5:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,���������������(JSON)"));
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -6:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�ڲ������쳣:ҵ�����ģ��δ����������"));
            break;
        case -7:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�����������,��Ҫ�����Ķ�����Ӧ�Ľ���ʶ����Ϊ��"));
            byExecuteState  = RCC_PARAM_ERROR;
            break;
        case -8:
            strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�ڲ������쳣:��Դ����ʧ��"));
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
        //Ԥ����ʧ�ܣ����������߽��
        ::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
        //������־
        RecordLog(FmtStr(_T("[��������Ӧ],��������Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"), byFunCode, byExecuteState));
    }
    RecordLog(FmtStr(_T("[��������Ӧ],ִ�г��������ӿ�(IF_DebitCancel)���ý���")));
    return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetDebitResult
//
// ������������ȡ�ۿ�
//
// ���������IN OUT char* szParamContext
//         ��IN OUT int& iParamSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetDebitResult(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog;
    CString strDataString;
    RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�ۿ����ӿ�(IF_GetDebitResult)���ÿ�ʼ,����1:%04X,����2:%d,����3:%d"), szParamContext, iParamSize, iFormatType));
    try
    {
        //�ж�����Ƿ��ʼ���ɹ�
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�жϺϷ���
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�жϲ����Ƿ��쳣
        if ((iFormatType > 2) || (iFormatType < 1) || (NULL == szParamContext))
            throw - 3;
        //�жϽ���Ƿ��Ѿ�׼����
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
        RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�ۿ����ӿ�(IF_GetDebitResult)����������:(%s)%s "), (0x01 == iFormatType) ? _T("���ݸ�ʽ:XML") : _T("���ݸ�ʽ:JSON"), strDataString));

        _tcscpy_s(szParamContext, strDataString.GetLength() + 1, strDataString.GetBuffer(0));
        strDataString.ReleaseBuffer();

        //���ý��׽��
        m_sCurrentDebitResult.Valid = FALSE;
        memset(&m_sCurrentDebitResult, 0, sizeof(tagDebitResultItems));

        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],��ȡ�ۿ�������(IF_GetDebitResult)����,���δ�ɹ���ʼ��"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],��ȡ�ۿ�������(IF_GetDebitResult)����,���δ����Ȩ"));
            break;
        case -3:
            strLog.Format(_T("[��������Ӧ],��ȡ�ۿ�������(IF_GetDebitResult)����,��������쳣"));
            break;
        case -4:
            strLog.Format(_T("[��������Ӧ],��ȡ�ۿ�������(IF_GetDebitResult)����,�ۿ���δ׼����"));
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }
    RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�ۿ����ӿ�(IF_GetDebitResult)���ý���")));
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_SetMMI
//
// �������������ý�������
//
// ���������IN const char* szParamContext
//         ��IN const int& iParamSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�10
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_SetMMI(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());


    bool bReturn = false;
    CString strLog = FmtStr(_T("[��������Ӧ],���ý�������ӿ�(IF_SetMMI)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"), szParamContext, iParamSize, iFormatType);
    strLog.Replace(_T("\r"), _T(""));
    strLog.Replace(_T("\n"), _T(""));
    RecordLog(strLog);

    int nFormateType = iFormatType;
    CString strParamString(szParamContext);
    CString strDataString;
    CString strSubError;
    try
    {
        //�ж�����Ƿ��Ѿ�����ʼ��
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�ж�����Ƿ�ɹ���Ȩ
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�ж�ҵ�����߳��Ƿ��쳣
        if (NULL == m_pThreadManage)
            throw - 3;
        if (NULL == m_pThreadManage->m_pThreadDevice)
            throw - 4;
        //�жϲ����Ƿ��쳣
        if ((NULL == szParamContext) || (0 == iParamSize))
            throw - 5;
        //�жϲ�������
        if ( (iFormatType > 2) || (iFormatType < 1))
            throw - 6;

        CString strMMIString = ConstructMMIInfo(iFormatType, strParamString);

        if (strMMIString.IsEmpty())
            throw - 9;

        //��ȡ����
        CString* pstrString = new CString();
        if (NULL == pstrString)
            throw - 10;
        pstrString->Format(_T("%s"), strMMIString);
        //֪ͨӲ�������߳���ʾ����
        m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO, (WPARAM)pstrString, (LPARAM)nFormateType);
        //��ǳɹ�
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,���δ�ɹ���ʼ��"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,���δ����Ȩ"));
            break;
        case -3:
        case -4:
        case -5:
        case -6:
        case -9:
        case -10:
            strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,����ڲ���Դ�쳣"));
            break;
        case -7:
            strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,��������(XML��ʽ)������ϸ����:%s"), strSubError);
            break;
        case -8:
            strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,��������(JSON��ʽ)������ϸ����:%s"), strSubError);
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
        }
    }

    RecordLog(FmtStr(_T("[��������Ӧ],���ý�������ӿ�(IF_SetMMI)���ý���")));
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�WINAPI    CTWSDNetPay_DllApp::IF_GetParam
//
// ������������ȡӲ������
//
// ���������IN const char* szParamContext
//         ��IN const int& iParamSize
//         ��IN const int& iFormatType
//         ��IN OUT char* szResultContext
//         ��IN OUT int& iResultSize
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�11
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetParam(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, IN OUT char* szResultContext, IN OUT int& iResultSize)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog = FmtStr(_T("[��������Ӧ],��ȡӲ�������ӿ�(IF_GetParam)�ݲ��ṩ�����"));

    if (strLog != m_strLastErrorDesc)
    {
        m_strLastErrorDesc = strLog;
        RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
    }
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetComponentStatus
//
// ����������������״̬
//
// ���������IN OUT unsigned int& uStatus
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�11
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetComponentStatus(IN OUT unsigned int& uStatus)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    DWORD dwState = 0x00;
    CString strLog;
    //  RecordLog(FmtStr(_T("[��������Ӧ],������״̬�ӿ�(IF_GetComponentStatus)���ÿ�ʼ,����1:%d"),uStatus));
    try
    {
        //�ж�����Ƿ��Ѿ�����ʼ��
        if (!m_bIsInitEnvironment)
            throw - 1;
        //�ж�����Ƿ�ɹ���Ȩ
        if (!m_bAuthorizedLegal)
            throw - 2;
        //�ж�ҵ�����߳��Ƿ��쳣
        if (NULL == m_pThreadManage)
            throw - 3;
        //�ж�Ӳ�������߳��Ƿ�����
        if (NULL == m_pThreadManage->m_pThreadDevice)
            throw - 4;
        //�ն��豸״̬/����״̬
        CString strState;

        //��ȡӲ��״̬
        DWORD dwHDState = m_pThreadManage->m_pThreadDevice->GetDeviceState();

        uStatus = (dwState | dwHDState);

        //add 2018-01,�����߼����ж��Ƿ������豸״̬�ķ���
        CString strReflectWebFilePath;
        strReflectWebFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
        int hide_option = ::GetPrivateProfileInt(_T("SetUp"), _T("HideDeviceState"), 0, strReflectWebFilePath);
        if (hide_option == 1)
        {
            uStatus = 0;
        }

        //��ǳɹ�
        bReturn = true;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("[��������Ӧ],������״̬(IF_GetComponentStatus)����,���δ�ɹ���ʼ��"));
            break;
        case -2:
            strLog.Format(_T("[��������Ӧ],������״̬(IF_GetComponentStatus)����,���δ����Ȩ"));
            break;
        case -3:
        case -4:
            strLog.Format(_T("[��������Ӧ],������״̬(IF_GetComponentStatus)����,����ڲ���Դ�쳣"));
            break;
        }
        //�����������
        if (strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
        }
    }
    //  RecordLog(FmtStr(_T("[��������Ӧ],������״̬�ӿ�(IF_GetComponentStatus)���ý���")));

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_TranslateData
//
// ����������ִ�д���ؼ���Ϣ���ݲ���
//
// ���������IN const char* szParamContext
//         ��IN const int& iParamSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�12
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_TranslateData(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    bool bReturn = false;
    CString strLog = FmtStr(_T("[��������Ӧ],ִ�д���ؼ���Ϣ���ݽӿ�(IF_TranslateData)�ݲ��ṩ�����"));
    if (strLog != m_strLastErrorDesc)
    {
        m_strLastErrorDesc = strLog;
        RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
    }
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_Authority
//
// ����������Ȩ����֤
//
// ���������IN const char* szAuthKey
//         ��IN const int& iAuthKeySize
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ڲ��ӿ�
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

    //����json�ķ�װ
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
        CString strLog = FmtStr(_T("[��������Ӧ],ͼ����ӿ�(IF_TransferImage)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"), szImageInfo, iSize, iFormatType);
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
        RecordLog("Error Para Info��not json or xml", LOG_LEVEL_ERROR);
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


    //����json�ķ�װ
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

	theApp.RecordLog("IF_AgencyDebitQuery ��ʼ����");
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
        RecordLog("Error Para Info��not json or xml", LOG_LEVEL_ERROR);
        return false;
    }
    if (! m_pThreadManage->m_pThreadDevice->AgencyDebitQuery(paraStr, iFormatType, result))
    {
		theApp.RecordLog("IF_AgencyDebitQuery ���ý�����ʧ��");
        return false;
    }
	theApp.RecordLog("IF_AgencyDebitQuery ���ý������ɹ�");

    return true;
}



/****************************************�ڲ���������********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::RecordLog
//
// ������������¼��־
//
// ���������IN const CString& strLog -- ��־����
//         ��IN const int& iLevel    -- ��־�ȼ�
//         ��IN const BOOL& bSave    -- ��־�Ƿ�������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CTWSDNetPay_DllApp::ReadConfigInfo
//
// ������������ȡ������Ϣ
//
// ���������void
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::ReadConfigInfo(void)
{
    TCHAR szTemp[256] = {0};
    CString strConfigFilePath;
    strConfigFilePath.Format(_T("%s\\%s"), m_strWorkDir, FILE_NAME_APP_CONFIG);
    //�Ƿ񱣴���־
    m_bSaveLog              = (BOOL)::GetPrivateProfileIntA(_T("Log"), _T("Save"), 0, strConfigFilePath);
    //��ȡ��־ɾ���޶�ʱ������
    m_iDeleteLogStartHour = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteStartHour"), 2, strConfigFilePath);
    //��ȡ��־ɾ���޶�ʱ����յ�
    m_iDeleteLogEndHour = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteEndHour"), 2, strConfigFilePath);
    //��־ɾ��������ѯʱ��
    m_iDeleteLogSpanTime = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteSpanTime"), 30 * 60, strConfigFilePath);
    //��־���ر�������ʱ��
    m_iDeleteLogSaveDays = ::GetPrivateProfileIntA(_T("Log"), _T("DeleteSaveDays"), 60, strConfigFilePath);
    //����Ƿ��������
    m_bIsTestData           = (BOOL)::GetPrivateProfileIntA(_T("Setup"), _T("TestData"), 0, strConfigFilePath);
    //�̴߳����־���ʱ����
    m_dwThreadCheckSpanTime = ::GetPrivateProfileIntA(_T("Setup"), _T("ThreadCheckSpanTime"), 300, strConfigFilePath);
    //��ȡ���⿨ǰ׺
    ::GetPrivateProfileString(_T("Setup"), _T("VirtualCardPrefix"), _T("4412"), szTemp, 256, strConfigFilePath);
    m_strVirtualCardPrefix.Format(_T("%s"), szTemp);
    //��ȡ������Ӳ���·�ʱ��ļ��
    m_dwUpdateDevieTimeSpanTime = (DWORD)::GetPrivateProfileIntA(_T("Setup"), _T("UpdateDeviceTimeSpanTime"), 30 * 60, strConfigFilePath);
    m_dwUpdateDevieTimeSpanTime *= 1000;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::fnCloseThread
//
// �����������ȴ�һ���������ر��߳������ر�
//
// ���������IN const HANDLE& hThread          --  ��Ҫ���رյ��߳̾��
//         ��IN const DWORD& dwMilliseconds        --  �ȴ�ʱ�䣨��λ�����룩
// ���������int
//         ��0  -- �߳��Ѿ��������ر�
//         ��-1    -- �߳̾����Ч
//         ��-2    -- �ں˶���δ�������ȴ�ʱ���ѵ�
//         ��-3    -- �ȴ��Ļ�����δ���ͷ�
//         ��-4    -- �ȴ�����ִ��ʧ��
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::fnCloseThread(IN const HANDLE& hThread, IN const DWORD& dwMilliseconds)
{
    int iReturn = 0;
    //���Թر��߳�
    DWORD dwWaitResult = ::WaitForSingleObject(hThread, dwMilliseconds);
    //��ô�����
    DWORD dwErrorCode = ::GetLastError();
    //��Ϣ�ṹ����
    MSG  mMsg;

    if (6 == dwErrorCode)//�����Ч
    {
        iReturn = 0;
    }
    else
    {
        //�жϵȴ��������ؽ��
        switch (dwWaitResult)
        {
        case WAIT_OBJECT_0: //�ں˶����Ϊ����״̬
            iReturn = 0;
            break;
        case WAIT_TIMEOUT:  //�ں˶���δ�������ȴ�ʱ���ѵ�
        {
            if (::PeekMessage(&mMsg, NULL, 0, 0, PM_NOREMOVE))
            {
                ::TranslateMessage(&mMsg); Sleep(10);
                ::DispatchMessage(&mMsg); Sleep(10);
            }
        }
        iReturn = -2;
        break;
        case WAIT_ABANDONED://�ȴ��Ļ�����δ���ͷ�
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
//  ��   �� : ROCY
//  ʱ   �� : 2008-05-31
//  ��   �� : 1.0.0.1
//  �������� : DisplayWinError
//  �������� : ��ʾϵͳ���µĴ�����Ϣ
//  ������� : DWORD    --  dwErrorCode ������ϵͳ�������,һ���� GetLastError() ��������
//             int      --  iType       Ĭ��Ϊ0
//             BOOL     --  bPrompt     �Ƿ���Ҫ�öԻ�����ʾ�ô�����Ϣ��TRUEΪ��Ҫ��FALSEΪ����Ҫ��
//  ������� : LPCTSTR -- ��ϸ�Ĵ���������Ϣ
//  ��ע˵�� : ÿ�ε���API�󶼿��Ե��ô˺����鿴��ϸ�Ĵ�����Ϣ
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
    strTemp.Format(_T("���������:%d�� -- ����ϸ����:%s��"), dwErrorCode, A2T(lpMessageBuffer));
    strTemp.Replace(_T("\r\n"), _T(" "));

    if (bPrompt)
        ::MessageBox(NULL, strTemp, _T("ϵͳ��ʾ"), MB_ICONWARNING);

    LocalFree(lpMessageBuffer);
    return strTemp;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::GetProcessMemory
//
// ������������ȡ��ǰ������ʹ�õ��ڴ�
//
// ���������IN const int& iShowType
// ���������DWORD
// ��д��Ա��ROCY
// ��дʱ�䣺2017��5��7��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CTWSDNetPay_DllApp::GetProcessMemory(IN const int& iShowType)
{
    DWORD dDevValue = (1 == iShowType) ? (1024 * 1024) : 1024;
    //��ȡ��ǰ����ʹ���ڴ���Ϣ
    DWORD dwProcessMemory = 0x00;
    PROCESS_MEMORY_COUNTERS pLocalProcessMemory = {0};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pLocalProcessMemory, sizeof(pLocalProcessMemory)))
    {
        dwProcessMemory = pLocalProcessMemory.PagefileUsage / dDevValue;
    }

    return dwProcessMemory;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::CostructDebitInfo
//
// �������������췢�͵����ڵ����ݸ�ʽ -- �ۿ��
//
// ���������int nFormatType
//         ��tagPayKeyItems& pkItems
// ���������CString
// ��д��Ա���ֻ�Ȫ
// ��дʱ�䣺2017��2��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::CostructDebitInfo(int nFormatType, tagPayKeyItems& pkItems)
{
    /*add 2017-06 zhuyabing �����ʱ������Ϊ�����ʱ��һ�¡� */
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
    else   //�ع��ۿ�json
    {

        //��ʾ����json����
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

        /***********************************************************����JSAO���ݽṹ***********************************************************/
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

        //ת��Ϊ�ַ���
        Json::FastWriter fast_writer;
        strReturn.Format(_T("%s"), fast_writer.write(root).c_str());

    }

    return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::ConstructCancleInfo
//
// �������������췢�͵����ڵ����ݸ�ʽ -- ��������
//
// ���������int nFormatType
//         ��CString strPayID
// ���������CString
// ��д��Ա���ֻ�Ȫ
// ��дʱ�䣺2017��2��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::ConstructCancleInfo(int nFormatType, CString strPayID)
{
    /*2017-05-31 zhuyabing �ָ���KeyItem�ڵ�*/
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

    //ת��Ϊ�ַ���
    Json::FastWriter fast_writer;
    strReturn.Format(_T("%s"), fast_writer.write(root).c_str());

    return strReturn;
}

/**
 * ��Ŀ���Ƶ���ʱת����������������json��ʽ
 * Ŀǰ����֧������
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
        {   //���г����ķ�����ʾ
            Json::Value  rootH, typeH, mmiH, dataH, versionH;
            Json::Value moneyH, vtypeH, vclassH, weightH, axisH, weightLimitH, balanceH, stationNameH;
            for (int i = 0; i < xml.Text.GetSize(); ++i)
            {
                int iTemp = 0x00;
                switch (xml.Text.GetAt(i).Sub)
                {
                case 1://����
                {
                    int tmpVal = _ttoi(xml.Text.GetAt(i).Context);
                    vtypeH = Json::Value( tmpVal / 10);
                    vclassH = Json::Value(tmpVal % 10);
                }
                break;
                case 2://���
                {
                    moneyH =  Json::Value(_ttoi(xml.Text.GetAt(i).Context));
                }
                break;
                case 3://����
                {
                    weightH = Json::Value(_ttoi(xml.Text.GetAt(i).Context));


                }
                break;
                case 4://����
                {
                    axisH = Json::Value(_ttoi(xml.Text.GetAt(i).Context));

                }
                break;
                case 5://����
                {
                    weightLimitH = Json::Value( _ttoi(xml.Text.GetAt(i).Context));
                }
                break;
                case 6://����֧�������
                {
                    balanceH = Json::Value(_ttoi(xml.Text.GetAt(i).Context));

                }
                break;
                case 7://����շ�վ������
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
            case 1://����
            {
                int tmpVal = _ttoi(pJsonMMI->Text.GetAt(i).Context);
                vtypeH = Json::Value(tmpVal / 10);
                vclassH = Json::Value(tmpVal % 10);
            }
            break;
            case 2://���
            {
                moneyH = Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));

            }
            break;
            case 3://����
            {
                weightH =  Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));


            }
            break;
            case 4://����
            {
                axisH = Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));


            }
            break;
            case 5://����
            {
                weightLimitH =  Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));

            }
            break;
            case 6://����֧�������
            {
                balanceH = Json::Value(_ttoi(pJsonMMI->Text.GetAt(i).Context));

            }
            break;
            case 7://����շ�վ������
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
// �������ƣ�CTWSDNetPay_DllApp::ConstructMMIInfo
//
// �������������췢�͵����ڵ����ݸ�ʽ -- ���Դ���
//
// ���������int nFormateType
//         ��IN const CString& strOrgString
//         ��OUT BOOL& bClearCmd
// ���������BOOL
// ��д��Ա���ֻ�Ȫ
// ��дʱ�䣺2017��2��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CTWSDNetPay_DllApp::ConstructMMIInfo(int nFormateType, IN const CString& strOrgString)
{
    return TmpScreenOnlyJson(strOrgString, nFormateType);

    CString strParamString(strOrgString);
    CString strMMIString;
    if (0x01 == nFormateType)
    {
        //XML��ʽ
        CDVXMLMMI xml;
        if (!xml.Init(strParamString))
        {
            strMMIString.Empty();
            return FALSE;
        }

        //�ع�XML����
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
            //��ȡ�����ͳ��֡�����������
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
                case 1://����   Vtype 0�� 1��   //���� Vclass  01~FF
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

                case 2://���
                {
                    iTemp = _ttoi(xml.Text.GetAt(i).Context);
                    iMoney = iTemp;
                }
                break;
                case 3://����
                {
                    iTemp = _ttoi(xml.Text.GetAt(i).Context);
                    iWeight = iTemp;
                }
                break;
                case 4://����
                {
                    iTemp = _ttoi(xml.Text.GetAt(i).Context);
                    iAxis = iTemp;
                }
                break;
                case 5://����
                {
                    iTemp = _ttoi(xml.Text.GetAt(i).Context);
                    iLimit = iTemp;
                }
                break;
                case 6://����֧�������
                {
                    iTemp = _ttoi(xml.Text.GetAt(i).Context);
                    iBalance = iTemp;
                }
                break;
                case 7://����շ�վ������
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
        //JSON��ʽ
        CDVJsonParamMMI* pJsonMMI = NULL;
        CJsonParamOperate jsonOperation;

        pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString, JSON_DT_PARAM_MMI); //��Ҫ�ͷ��ڴ�
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
            //�ع�ΪXML����
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

            //��ȡ�����ͳ��֡�����������
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
                case 1://����   Vtype 0�� 1��   //���� VClass  01~FF
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
                case 2://���
                {
                    iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

                    iMoney = iTemp;
                }
                break;
                case 3://����
                {
                    iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

                    iWeight = iTemp;
                }
                break;
                case 4://����
                {
                    iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

                    iAxis = iTemp;
                }
                break;
                case 5://����
                {
                    iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

                    iLimit = iTemp;
                }
                break;
                case 6://����֧�������
                {
                    iTemp = _ttoi(pJsonMMI->Text.GetAt(i).Context);

                    iBalance = iTemp;
                }
                break;
                case 7://����շ�վ������
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

            //ת��Ϊ�ַ���
            Json::FastWriter fast_writer;
            strMMIString.Format(_T("%s"), fast_writer.write(root).c_str());
        }

        delete pJsonMMI;
        pJsonMMI = NULL;
    }
    return strMMIString;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�NotifyCaller
//
// ����������֪ͨ�ⲿ�����ߣ�ҵ���첽����õ��Ľ��
//
// ���������IN const int& iOption             -- ֪ͨ���ͣ�1���ۿ2��������
//         ��IN const int& iResult             -- ���������������ĵ�
// �����������
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::NotifyCaller(IN const int& iOption, IN const int& iResult)
{
    switch (m_iNotifyMode)
    {
    case 1:
    {
        //֪ͨ�����߷��ؽ������ǰ����ʧ��
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
        //ʹ�ûص�����֪ͨ�ⲿ������
        if (NULL != m_fnCallbackNotifyFun)
        {
            m_fnCallbackNotifyFun(iOption, iResult);
        }
        else
        {
            RecordLog(FmtStr(_T("[��������Ӧ],���֪ͨ�ⲿ�����ߵĻص�����(NotifyCaller)ָ��Ϊ�գ��޷�֪ͨ������")), LOG_LEVEL_ERROR, true);
        }
    }
    break;
    case 3:
    {
        //ʹ�ûص�����֪ͨ�ⲿ������(�����ع�)
        if (NULL != m_fnCallbackNotifyFun45)
        {
            m_fnCallbackNotifyFun45(iOption, iResult, m_pIndPtrFor45);
        }
        else
        {
            RecordLog(FmtStr(_T("[��������Ӧ],���֪ͨ�ⲿ�����ߵĻص�����(NotifyCaller45)ָ��Ϊ�գ��޷�֪ͨ������")), LOG_LEVEL_ERROR, true);
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
        
        if (  pay_diff > 60) //����һ����û���յ���������
        {
            
            return 1;
        }
    }
	else if(srcKey == KEY_DEVICE_TEST)
	{
		 //Ӳ�����Բ�ѯ
        DevcieQueryInterrupt(msg);
	}
    else if(srcKey==24)
    {//��ͣ������
        std::string hexStr = Bin2Hex((unsigned char*)msg,(size_t)len);
		//MessageBox(0,hexStr.c_str(),"",0);
        m_pThreadManage->m_pThreadDevice->ExcuteComCommond(hexStr);
    }

	//std::string sss= "key:" + Int2str(srcKey);
	//MessageBox(0,sss.c_str(),"",0);
    //add 2018-04 ���ӶԽ���ģ��Ĵ���
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

//����������8��
int GetCurrentTimeZone()
{
	static int time_zone = RealCalTimeZone();
	return time_zone;
}



/*ͨѶ���У�Ĭ�ϵ�ǰʱ��ΪUTC8�������ǰʱ��Ϊ��UTC8������ͨѶ�⴦���ʱ�䣨��νUTC8ʱ�䣩����Ӧ�����ӻ��߼���
�ڴ˺����У��Ὣ��Ӱ����е���
�������ã���UTC8ʱ����Ϊ��׼�����ݵ�ǰʱ������ʱ����������������ǰʱ��ΪUTC0����ʵ��ʱ��Ӧ�þ��� ����ǰʱ��-8����Ҳ����UTCʱ����Ӧ��ӵ�е���ȷʱ��
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
/*��UTC8ʱ�����Ϊ��ǰʱ����ʱ�䣬
�����ǰʱ��ΪUTC0����ʵ��ʱ��Ӧ���� ��old_time + 8),��Ϊold_time��Ӧ�ø���UTC8ʱ�����н��ͣ�ȴʵ�ʱ�������UTC0ʱ�����������ٵ�8��Сʱ

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




//add 2018-03���˴�����commonbus�ļ��ݴ���

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

   

    //����json�ķ�װ
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
    CString strLog = FmtStr(_T("[��������Ӧ],ִ��Ʊ�ݽӿ�(IF_CompletionTicketNo)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"), szParamContext, iParamSize, iFormatType);
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
        RecordLog("Error Para Info��not json or xml", LOG_LEVEL_ERROR);
        return false;
    }
    if (! m_pThreadManage->m_pThreadDevice->TransTickNo(paraStr))
    {
        RecordLog("IF_CompletionTicketNo ����ʧ��");
        return false;
    }

    RecordLog("IF_CompletionTicketNo ���óɹ�");
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