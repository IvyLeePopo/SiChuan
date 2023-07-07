// TWSDNetPay_Dll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Ψһȫ�ֱ���
CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp()  { return &theApp; }

BEGIN_MESSAGE_MAP(CTWSDNetPay_DllApp, CWinApp)
END_MESSAGE_MAP()

/***********************������ʼ��*********************************/
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
// �������ƣ�CString	FmtStr
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
time_t CTWSDNetPay_DllApp::CString2time_t(LPCTSTR lpztime)
{
	CString csTemp = _T("");
	COleDateTime oleGetTime;
	csTemp.Format( _T("%s"), lpztime);
	if( csTemp.GetLength() > 0 )
	{
		if( csTemp.GetLength() > 19 )//�������Ƹ�ʽ _T("2014-11-23 12:34:56.000");
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
///��������: MakeDirectory()
///����˵��: ����ָ���Ķ༶Ŀ¼
///����˵��: CString dd,���������Ķ༶Ŀ¼·��
///����˵��: TRUE  -- �����ɹ�
///			 FALSE -- ����ʧ��
///�޸�ʱ��: 2006-04-12
///�޸���Ա: ROCY
///��    ע: ��Ҫ���� windows.h
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
//	��	  �� : ROCY
//	ʱ	  �� : 2008-06-01
//  ��	  �� : 1.0.0.1
//	�������� : GetVersionOfFile
//	�������� : ��ȡָ�����ļ��İ汾��
//	������� : LPCTSTR	--	lpsPath �ļ��ľ���·��
//	������� : DWORD -- ���صı��ؼ����IP�������룬�ֽںϳɸ�ʽ
//  ��ע˵�� : ��
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
// �������ƣ�NotifyCaller
//     
// ����������֪ͨ�ⲿ�����ߣ�ҵ���첽����õ��Ľ��
//     
// ���������IN const int& iOption				-- ֪ͨ���ͣ�1���ۿ2��������
//         ��IN const int& iResult				-- ���������������ĵ�
// �����������
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CTWSDNetPay_DllApp::NotifyCaller(IN const int& iOption,IN const int& iResult)
{
	switch(m_iNotifyMode)
	{
	case 1:
		{
			//֪ͨ�����߷��ؽ������ǰ����ʧ��
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
			//ʹ�ûص�����֪ͨ�ⲿ������
			if(NULL != m_fnCallbackNotifyFun)
			{
				m_fnCallbackNotifyFun(iOption,iResult);
			}
			else
			{
				RecordLog(FmtStr(_T("[��������Ӧ],���֪ͨ�ⲿ�����ߵĻص�����(NotifyCaller)ָ��Ϊ�գ��޷�֪ͨ������")),LOG_LEVEL_ERROR,true);
			}
		}
		break;
	case 3:
		{
			//ʹ�ûص�����֪ͨ�ⲿ������(�����ع�)
			if(NULL != m_fnCallbackNotifyFun45)
			{
				m_fnCallbackNotifyFun45(iOption,iResult,m_pIndPtrFor45);
			}
			else
			{
				RecordLog(FmtStr(_T("[��������Ӧ],���֪ͨ�ⲿ�����ߵĻص�����(NotifyCaller45)ָ��Ϊ�գ��޷�֪ͨ������")),LOG_LEVEL_ERROR,true);
			}
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::CheckIsAlNum
//     
// �������������������ַ������Ƿ�Ϊ���ֺ���ĸ�����ֻ���ĸ������һ��
//     
// ���������IN const char* szData
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��4��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		case 0://�Ƿ����֡���ĸ���
			{
				if(0 == isalnum(szData[i]))
				{
					bReturn = FALSE;
				}
			}
			break;
		case 1://�Ƿ�����
			{
				 if( (szData[i] < '0') || (szData[i] >'9') )
				 {
					 bReturn = FALSE;
				 }
			}
			break;
		case 2://�Ƿ���ĸ
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



//����վ�Ż��߳�����ת�������У�������ڲ��Ƿ�����ĸ
BOOL CTWSDNetPay_DllApp::CheckStringLetter(char * strCheck, int nType)
{
	CString csCheck = _T("");
	csCheck.Format( _T("%s"), strCheck );
	csCheck.MakeUpper();

	if( 1 == nType )//����1��ʾվ�����
	{
		if( csCheck.GetLength() > 9 )//�Դ���9λ���ȵ�վ����룬Ĭ�ϰ����ַ�������
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

	//��ȡ�����ļ�
	ReadConfigInfo();

	//������־Ŀ¼
	strTemp.Format(_T("%s\\TWSDNetPayLog"),m_strWorkDir);
	_tcscpy_s(m_sTranstParam.PathLogDir,_countof(m_sTranstParam.PathLogDir),strTemp.GetBuffer(0));
	strTemp.ReleaseBuffer();
	MakeDirectory(m_sTranstParam.PathLogDir);
	//��ȡ����汾
	CString strDllFilePath(szTempDir);

	m_strNetPayDllVersion = GetVersionOfFile(strDllFilePath);
	
	strDllFilePath.Format(_T("%s\\%s"),strDir,FILE_NAME_DEVICE_DLL);
	m_strDeviceDllVersion = GetVersionOfFile(strDllFilePath);

	//��ʼ����־ϵͳ
	if(m_bSaveLog)
	{
		INIT_LOG(m_sTranstParam.PathLogDir,_T(""));

		RecordLog(FmtStr(_T("\r\n/*************************************��������(ҵ��%s - �豸%s)*************************************/"),m_strNetPayDllVersion,m_strDeviceDllVersion));
	}

	hFileUpdate	= CreateEvent(NULL, FALSE, TRUE,	NULL);	//Ĭ�����ԡ��Զ����á���ʼ��
	if( NULL == hFileUpdate )
	{
		RecordLog( _T("���� �������±��ش洢�ļ� ͬ������ʧ�ܣ�") );
	}

	//����NFC
	m_strNFCSerialNo.Format(_T("53 4E 32 30 31 35 30 31 30 38 30 31 00"));
	m_strNFCTerminal.Format(_T("4C 41 4E 44 49 2D 45 35 58 30 00"));

	m_strNFCSerialNo.Replace(_T(" "),_T(""));
	m_strNFCTerminal.Replace(_T(" "),_T(""));


	return TRUE;
}

int CTWSDNetPay_DllApp::ExitInstance()
{
	//�˳���־ϵͳ
	if(m_bSaveLog)
	{
		RecordLog(FmtStr(_T("\r\n/*************************************����ֹͣ(ҵ��%s - �豸%s)*************************************/"),m_strNetPayDllVersion,m_strDeviceDllVersion));

		QUIT_LOG();
	}

	if( NULL != hFileUpdate )	
	{
		::CloseHandle(hFileUpdate);
	}

	return CWinApp::ExitInstance();
}

/****************************************����ӿں���********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_InitEnvironment
//     
// ������������ʼ���������
//     
// ���������IN const UINT& nThreadID				-- ������ָ�����߳�ID
//         ��IN const HWND& hWnd					-- ������ָ���Ĵ�����
//         ��IN const unsigned int& nNotifyMsgID	-- ������ָ����֪ͨ��ϢID
//		   ��IN const char* szAreaInfo				-- ������Ϣ(�������#·�κ�)
//         ��IN const char* szLoaclStation			-- վ����Ϣ(վ��#վ��)
//         ��IN const char* szLoaclLaneID			-- ��������
//         ��IN const char* szServerInfo			-- ��������Ϣ
//         ��IN const int& iProvinceID				-- ʡ�ݱ���
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment(IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	
	RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ÿ�ʼ")));
	try
	{
		/*******************���������Ƿ�����*******************/
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
		//���������Ϣ
		TCHAR   szTemp[256]={0};
		CString strTemp;
		
		m_sTranstParam.ProvinceID			 = iProvinceID;
		m_sTranstParam.ActiveNotifyMsgID	 = nNotifyMsgID;
		m_sTranstParam.CallerThreadID		 = nThreadID;
		m_sTranstParam.CallerWnd			 = hWnd;

		//��������
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

		//·�Ρ��������
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

		//վ������վ��
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

		//���⴦���������
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//�㶫����ĳ���վ�����⴦��
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

		RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%s)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));


		//�汾1060���� ���д��۶�̬����ش���
		AgencyInitDll(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);

		//���кϷ���У��
		m_bAuthorizedLegal = TRUE;
		//����ҵ������߳�
		CString strConfigFilePath;
		m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadManage)
			throw -12;
		strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadManage->SetParam(&m_sTranstParam,NULL);
		//�����߳����»���
		m_pThreadManage->ResumeThread();

		//��ǳ�ʼ��״̬
		m_bIsInitEnvironment = TRUE;

		//���֪ͨ��ʽ
		m_iNotifyMode = 1;

		//���ؽ��
		bReturn = m_bIsInitEnvironment?true:false;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(m_strLastErrorDesc != strLog)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}

	RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ý���")));

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_InitEnvironment2
//     
// ������������ʼ���������
//     
// ���������IN const UINT& nThreadID				-- ������ָ�����߳�ID
//         ��IN const HWND& hWnd					-- ������ָ���Ĵ�����
//		   ��IN const char* szAreaInfo				-- ������Ϣ(�������#·�κ�)
//         ��IN const char* szLoaclStation			-- վ����Ϣ(վ��#վ��)
//         ��IN const char* szLoaclLaneID			-- ��������
//         ��IN const char* szServerInfo			-- ��������Ϣ
//         ��IN const int& iProvinceID				-- ʡ�ݱ���
//         ��void (*fun)(int option, int result)	-- �ۿ�/�������֪ͨ���ص�����
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2016��12��14��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1-1
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment2 (IN const UINT& nThreadID,IN const HWND& hWnd,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID,void (WINAPI *fun)(int option, int result))
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	
	RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2)���ÿ�ʼ")));
	try
	{
		/*******************���������Ƿ�����*******************/
		if(  (0 == nThreadID) && (NULL == hWnd))
			throw -1;
		if(NULL == fun)
			throw -2;
		//�ص�����ָ�븳ֵ������֪ͨ������
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
		//���������Ϣ
		TCHAR   szTemp[256]={0};
		CString strTemp;
		//������ʼ��������ֵ
		m_sTranstParam.ProvinceID			 = iProvinceID;
		m_sTranstParam.ActiveNotifyMsgID	 = 0;
		m_sTranstParam.CallerThreadID		 = nThreadID;
		m_sTranstParam.CallerWnd			 = hWnd;

		//��������
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

		//·�Ρ��������
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

		//վ������վ��
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

		//���⴦���������
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//�㶫����ĳ���վ�����⴦��
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

		RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment2)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%s)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));

		//�汾1060���� ���д��۶�̬����ش���
		AgencyInitDll(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);

		//���кϷ���У��
		m_bAuthorizedLegal = TRUE;
		//����ҵ������߳�
		CString strConfigFilePath;
		m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadManage)
			throw -12;
		strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadManage->SetParam(&m_sTranstParam,NULL);
		//�����߳����»���
		m_pThreadManage->ResumeThread();

		//��ǳ�ʼ��״̬
		m_bIsInitEnvironment = TRUE;

		//���֪ͨ��ʽ
		m_iNotifyMode = 2;

		//���ؽ��
		bReturn = m_bIsInitEnvironment?true:false;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(m_strLastErrorDesc != strLog)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
// ���������IN const UINT& nThreadID				-- ������ָ�����߳�ID
//         ��IN const HWND& hWnd					-- ������ָ���Ĵ�����
//		   ��IN const char* szAreaInfo				-- ������Ϣ(�������#·�κ�)
//         ��IN const char* szLoaclStation			-- վ����Ϣ(վ��#վ��)
//         ��IN const char* szLoaclLaneID			-- ��������
//         ��IN const char* szServerInfo			-- ��������Ϣ
//         ��IN const int& iProvinceID				-- ʡ�ݱ���
//         ��IN OUT void* pIndPtr					-- ���Ի�ָ��
//         ��void (*fun)(int option, int result,void* pIndPtr)	-- �ۿ�/�������֪ͨ���ص�����
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��03��16��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ӿ�1-2��ר��Ϊ�������Ƶĳ�ʼ����������ʼ��������ʱ�򣬱���һ�����Ի�ָ�룬�ڻص���ʱ�򴫳�ȥ
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_InitEnvironment3 (IN const UINT& nThreadID,IN const HWND& hWnd,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID,IN OUT void* pIndPtr,void (WINAPI *fun)(int option, int result,void* pIndPtr))
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	
	RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3)���ÿ�ʼ,���Ի�ָ�����:%04X"),pIndPtr));
	try
	{
		/*******************���������Ƿ�����*******************/
		if(  (0 == nThreadID) && (NULL == hWnd))
			throw -1;
		if(NULL == fun)
			throw -2;
		//�ص�����ָ�븳ֵ������֪ͨ������
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
		//���������Ϣ
		TCHAR   szTemp[256]={0};
		CString strTemp;
		//������ʼ��������ֵ
		m_sTranstParam.ProvinceID			 = iProvinceID;
		m_sTranstParam.ActiveNotifyMsgID	 = 0;
		m_sTranstParam.CallerThreadID		 = nThreadID;
		m_sTranstParam.CallerWnd			 = hWnd;

		//��������
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

		//·�Ρ��������
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

		//վ������վ��
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

		//���⴦���������
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//�㶫����ĳ���վ�����⴦��
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

		//���Ͳ���������һ�����Ի�ָ��
		m_pIndPtrFor45 = pIndPtr;

		//��¼��־
		RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment3)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%s)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));

		//�汾1060���� ���д��۶�̬����ش���
		AgencyInitDll(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);

		//���кϷ���У��
		m_bAuthorizedLegal = TRUE;
		//����ҵ������߳�
		CString strConfigFilePath;
		m_pThreadManage = (CThreadManage *)::AfxBeginThread(RUNTIME_CLASS(CThreadManage),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadManage)
			throw -12;
		strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadManage->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadManage->SetParam(&m_sTranstParam,NULL);
		//�����߳����»���
		m_pThreadManage->ResumeThread();

		//��ǳ�ʼ��״̬
		m_bIsInitEnvironment = TRUE;

		//���֪ͨ��ʽ
		m_iNotifyMode = 3;

		//���ؽ��
		bReturn = m_bIsInitEnvironment?true:false;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(m_strLastErrorDesc != strLog)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
	try
	{
		// 1052�汾������Ϊ�˷�ֹ�˳����������������ķǷ����ʣ�����ʼ�������Ϊ��
		m_bIsInitEnvironment = FALSE;

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
		DWORD	dwSpanTime = 100;

		//ֹͣҵ������߳�
		if(NULL != m_pThreadManage)
		{
			//���ø��߳��Ƴ����Ϊ��
		//	m_pThreadManage->m_bExit = TRUE;
			m_pThreadManage->ThreadQuit();
			//��Ĭ200����
			::Sleep(200);

			//ÿ�εȴ�1000���룬���Դ���������10��
			while(OP_SUCCESS != fnCloseThread(m_pThreadManage->m_hThread,1000))
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

			strLog.Format(_T("[��������Ӧ],�����Դ����ӿ�(IF_Destroy),%s"),FmtStr(_T("��ҵ������̡߳��Ѿ�ֹͣ(���Դ��� %d)..."),iHaveTryCount));
			//��¼��־
			RecordLog(strLog);
		}

		//�ͷŴ��۶�̬��
		m_AgencyPay.Destroy();
		//�ͷ�һЩָ����Դ
		m_fnCallbackNotifyFun	= NULL;
		m_fnCallbackNotifyFun45 = NULL;
		m_pIndPtrFor45			= NULL;
		memset(&m_sTranstParam,sizeof(tagTranstParam),0);
		memset(&m_sCurrentDebitResult,sizeof(tagDebitResultItems),0);
		//���ñ��λ
		m_bIsInitEnvironment						= FALSE;
		m_bAuthorizedLegal							= FALSE;
		m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
		m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
		//��Ĭ500����
		::Sleep(m_iExitWaitForTime);
		//��¼��־
		strLog.Format(_T("[��������Ӧ],��������˳�..."));
		RecordLog(strLog,0,TRUE);		
		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
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
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�����Ƿ�ɹ���Ȩ
		if(!m_bAuthorizedLegal)
			throw -2;
		//�ж�ҵ�����߳��Ƿ��쳣
		if(NULL == m_pThreadManage)
			throw -3;
		//��ҵ������̷߳�����Ϣ��Ҫ�����Ӳ�����л�ȡ�û���Ϣ����
		m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_START_GET_DATA,(WPARAM)iOvertime,(LPARAM)0x01);
		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		BYTE byFunCode		= 0x03;
		BYTE byExecuteState	= RCC_INNER_ERROR;
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[��������Ӧ],������ȡ�û���Ϣ����(IF_StartGetAccountInfo)����,���δ�ɹ���ʼ��"));
			byExecuteState	= RCC_COM_NOT_INIT;
			break;
		case -2:
			strLog.Format(_T("[��������Ӧ],������ȡ�û���Ϣ����(IF_StartGetAccountInfo)����,���δ����Ȩ"));
			byExecuteState	= RCC_COM_ILLEGAL;
			break;
		case -3:
			strLog.Format(_T("[��������Ӧ],������ȡ�û���Ϣ����(IF_StartGetAccountInfo)����,����ڲ���Դ�쳣"));
			break;
		}
		//�����������
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
		//���������߽��
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(m_sTranstParam.CallerWnd))
		{
			::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}
		//������־
		RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ����Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"),byFunCode,byExecuteState));
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
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�����Ƿ�ɹ���Ȩ
		if(!m_bAuthorizedLegal)
			throw -2;
		//�ж�ҵ�����߳��Ƿ��쳣
		if(NULL == m_pThreadManage)
			throw -3;
		//��ҵ������̷߳�����Ϣ��Ҫ�����Ӳ�����л�ȡ�û���Ϣ����
		m_pThreadManage->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA,(WPARAM)0,(LPARAM)0);
		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}

	}
	RecordLog(FmtStr(_T("[��������Ӧ],ֹͣ��ȡ�û���Ϣ�����ӿ�(IF_StopGetAccountInfo)���ý���")));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_GetAccountInfo
//     
// ������������ȡ�û���Ϣ
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
bool WINAPI CTWSDNetPay_DllApp::IF_GetAccountInfo(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	CString strDataString;
	RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ�ӿ�(IF_GetAccountInfo)���ÿ�ʼ,����1:%04X,����2:%d,����3:%d"),szParamContext,iParamSize,iFormatType));
	try
	{
		//�ж�����Ƿ��Ѿ�����ʼ��
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�����Ƿ�ɹ���Ȩ
		if(!m_bAuthorizedLegal)
			throw -2;
		//�ж�ҵ�����߳��Ƿ��쳣
		if(NULL == m_pThreadManage)
			throw -3;
		//�жϲ����Ƿ��쳣
		if((NULL == szParamContext) || (iParamSize<1024))
			throw -4;
		if( (iFormatType>2) || (iFormatType<1))
			throw -5;
		//�ж������Ƿ��Ѿ���ȡ�ɹ�
		if(!m_pThreadManage->GetAccountDataReadyState())
			throw -6;
		//��ȡ����
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
		RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ�ӿ�(IF_GetAccountInfo)�������:%s"),strDataString));
		memcpy(szParamContext,strDataString.GetBuffer(0),strDataString.GetLength()>=iParamSize?(iParamSize-1):strDataString.GetLength());
		strDataString.ReleaseBuffer();
		//��ǳɹ�
		bReturn = true;
		//�����û�����׼��״̬Ϊ��
		m_pThreadManage->SetAccountDataReadyState(FALSE);
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
bool WINAPI CTWSDNetPay_DllApp::IF_DebitMoney(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[��������Ӧ],ִ�пۿ�����ӿ�(IF_DebitMoney)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"),szParamContext,iParamSize,iFormatType);
	strLog.Replace(_T("\r"),_T(""));
	strLog.Replace(_T("\n"),_T(""));
	RecordLog(strLog);
	CString strOperationMode(_T("δ֪"));
	CString strDataString;
	CString strAnalyError;

	try
	{
		/*****************************У�˹���***********************************/
		//�ж�����Ƿ��ʼ���ɹ�
		 if(!m_bIsInitEnvironment)
			throw -1;
		//�жϺϷ���
		if(!m_bAuthorizedLegal)
			throw -2;
		//�жϲ����Ƿ��쳣
		if((NULL == szParamContext) || (0 == iParamSize) || (iFormatType>2 || iFormatType<1))
			throw -3;
		//�ж��ϴν����Ƿ����
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

			//��������
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
			//���������շ�Ա��Ϣ
			m_pCurrentDebitInfo.OperatorID			= xml.OperatorID;
			m_pCurrentDebitInfo.OperatorName		= xml.OperatorName;

			m_pCurrentDebitInfo.Valid				= TRUE;
		}
		else
		{
			CDVJsonParamDebitMoney* pDebitMoney = NULL;
			CJsonParamOperate jsonParamOperation;
//			strDataString.Format(_T("{\"DebitParamContext\":{\"Data\":{\"OperationMode\":2,\"OverTime\":60000,\"Vehicle\":{\"License\":\"0\",\"Weight\":0,\"AxisCount\":0,\"Type\":1,\"VLColor\":0,\"Class\":1},\"Operation\":{\"ShiftID\":1,\"PassCertificateType\":0,\"CardID\":\"0\",\"TollDate\":\"20160823\"},\"Entry\":{\"RoadID\":30,\"License\":\"\",\"AreaID\":4412,\"StationName\":\"����᯴�\",\"VColor\":0,\"VClass\":9,\"VType\":1,\"Time\":\"2016-08-23 14:34:28\",\"LaneID\":0,\"StationID\":8},\"Debit\":{\"PayCertificateType\":1,\"PayCertificateCode\":\"283487891099961852\",\"Money\":200,\"PayIdentifier\":\"1E09381F4F1B90000000000000000004\",\"SubTime\":\"2016-08-23 14:34:28\",\"Type\":1}},\"Version\":\"1.0\"}}"));
			pDebitMoney = (CDVJsonParamDebitMoney*)jsonParamOperation.FormatResponseResult2Class(strDataString,JSON_DT_PARAM_DEBIT_MONEY);
			if(NULL == pDebitMoney)
			{
				strAnalyError.Format(_T(""));
				throw -5;
			}
			//��������
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
			//���������շ�Ա��Ϣ
			m_pCurrentDebitInfo.OperatorID			= pDebitMoney->OperatorID;
			m_pCurrentDebitInfo.OperatorName		= pDebitMoney->OperatorName;

			m_pCurrentDebitInfo.Valid				= TRUE;
		}
		//���⴦���������
		switch(m_sTranstParam.ProvinceID)
		{
		case 44:
			{
				CString strSTID;
				switch(m_sTranstParam.LaneConfigInfo.RoadID)
				{
				//�㶫��������վ�����⴦��
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

		//���ҵ������߳��Ƿ�����
		if(NULL == m_pThreadManage)
			throw -6;

		//��ǵ�ǰ�������ڽ���
		m_bIsTrading = TRUE;
		/*****************************��Ӧ����***********************************/

		//�汾1060���������Ƚ��д��۴���
		if(!AgencyConstructDebitData())
		{
			//������ɨ������
			switch(m_pCurrentDebitInfo.OperationMode)
			{
			case 1://���ϲ���
				{
					strOperationMode.Format(_T("����ģʽ"));
					m_bCurrentDebitMode = TRUE;
					//�ع�֧������
					CDVXMLDebitFlow xmlDebit;
					xmlDebit.ProvinceID		= m_sTranstParam.ProvinceID;
					xmlDebit.Type			= 0x03;
					xmlDebit.Money			= m_pCurrentDebitInfo.Money;
					xmlDebit.Weight			= m_pCurrentDebitInfo.TotalWeight;
					xmlDebit.VehicleType	= m_pCurrentDebitInfo.VehicleType;
					xmlDebit.VehicleClass	= m_pCurrentDebitInfo.VehicleClass;
					xmlDebit.CurrentTime	= time(NULL);
					CString strXMLString	= xmlDebit.ConstructFormatData();

					//������Դ����
					CString* pstrString = new CString();
					if(NULL == pstrString)
						throw -8;
					pstrString->Format(_T("%s"),strXMLString);

					//�ر��ϴεĶ�ʱ��
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_CHECK);
					//�����ۿʱ��ʱ��
					m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,m_pCurrentDebitInfo.OverTime);
					//��ʼ��ʱ
					m_dwTradeBeginOvertime = GetTickCount();
					//�����ۿ�����
					m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_START_DEBIT_FLOW,(WPARAM)pstrString,(LPARAM)0);
					//�������
					m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney),ҵ�����ڴ���(���ϲ���)"));
				}
				break;
			case 2://�������
				{
					strOperationMode.Format(_T("����ģʽ"));
					m_bCurrentDebitMode = FALSE;
					//��ʼ��ʱ
					m_dwTradeBeginOvertime = GetTickCount();
					//�ر��ϴεĶ�ʱ��
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
					m_pThreadManage->StopTimer(TIMER_ID_DEBIT_CHECK);
					//�����ۿʱ��ʱ��
					m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,m_pCurrentDebitInfo.OverTime);
					//ִ�пۿ����
					m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_ONLY_DEBIT_MONEY,(WPARAM)0,(LPARAM)0);
					//�������
					m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney),ҵ�����ڴ���(�������)"));
				}
				break;
			}
			//��¼��־
			RecordLog(m_strLastErrorDesc);
		}

		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		BYTE byFunCode		= 0x01;
		BYTE byExecuteState	= RCC_INNER_ERROR;
	
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),���δ�ɹ���ʼ��"),strOperationMode);
			byExecuteState	= RCC_COM_NOT_INIT;
			break;
		case -2:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�Ϸ���У��ʧ��"),strOperationMode);
			byExecuteState	= RCC_COM_ILLEGAL;
			break;
		case -3:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),��������쳣"),strOperationMode);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -4:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�������(XML��ʽ)����������ϸ����:%s"),strOperationMode,strAnalyError);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -5:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�������(JSON��ʽ)��������"),strOperationMode);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -6:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�ڲ������쳣:ҵ�����ģ��δ����������"),strOperationMode);
			break;
		case -7:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�ڲ������쳣:�ϴν���δ��ɣ�������Ƶ������"),strOperationMode);
			byExecuteState	= RCC_LAST_TRADE_NOT_END;
			break;
		case -8:
			strLog.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney)����(%s),�ڲ������쳣:��Դ����ʧ��"),strOperationMode);
			break;
		}
		//�����������
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
		//���������߽��
		NotifyCaller(byFunCode,byExecuteState);
		//������־
		RecordLog(FmtStr(_T("[��������Ӧ],�ۿ����Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"),byFunCode,byExecuteState));
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
bool WINAPI CTWSDNetPay_DllApp::IF_DebitCancel(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[��������Ӧ],ִ�г��������ӿ�(IF_DebitCancel)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"),szParamContext,iParamSize,iFormatType);
	strLog.Replace(_T("\r"),_T(""));
	strLog.Replace(_T("\n"),_T(""));
	RecordLog(strLog);
	
	CString strDataString;
	CString strPayIdentifier;
	CDVXMLDebitCancel xml;
	try
	{
		/*****************************У�˹���***********************************/
		//�ж�����Ƿ��ʼ���ɹ�
		if(!m_bIsInitEnvironment)
			throw -1;
		//�жϺϷ���
		if(!m_bAuthorizedLegal)
			throw -2;
		//�жϲ����Ƿ��쳣
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
		//���ҵ������߳��Ƿ�����
		if(NULL == m_pThreadManage)
			throw -6;
		//��⳷�����ױ�ʶ�Ƿ�Ϸ�
		strPayIdentifier.TrimLeft();
		strPayIdentifier.TrimRight();
		if(strPayIdentifier.IsEmpty())
			throw -7;
		//������Դ����
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
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,���δ�ɹ���ʼ��"));
			byExecuteState	= RCC_COM_NOT_INIT;
			break;
		case -2:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�Ϸ���У��ʧ��"));
			byExecuteState	= RCC_COM_ILLEGAL;
			break;
		case -3:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,��������쳣"));
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -4:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�����������������ϸ����:%s"),xml.m_strLastErrorDesc);
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -5:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,���������������(JSON)"));
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -6:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�ڲ������쳣:ҵ�����ģ��δ����������"));
			break;
		case -7:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�����������,��Ҫ�����Ķ�����Ӧ�Ľ���ʶ����Ϊ��"));
			byExecuteState	= RCC_PARAM_ERROR;
			break;
		case -8:
			strLog.Format(_T("[��������Ӧ],ִ�г�������(IF_DebitCancel)����,�ڲ������쳣:��Դ����ʧ��"));
			break;
		}
		//�����������
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
		//���������߽��
		NotifyCaller(byFunCode,byExecuteState);
		//������־
		RecordLog(FmtStr(_T("[��������Ӧ],��������Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"),byFunCode,byExecuteState));
	}
	RecordLog(FmtStr(_T("[��������Ӧ],ִ�г��������ӿ�(IF_DebitCancel)���ý���")));
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetDebitResult
//     
// ������������ȡ�ۿ�/���οۿ���
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
// ��ע˵�����ӿ�9
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_GetDebitResult(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	CString strDataString;
	RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�ۿ����ӿ�(IF_GetDebitResult)���ÿ�ʼ,����1:%04X,����2:%d,����3:%d"),szParamContext,iParamSize,iFormatType));
	try
	{
		//�ж�����Ƿ��ʼ���ɹ�
		if(!m_bIsInitEnvironment)
			throw -1;
		//�жϺϷ���
		if(!m_bAuthorizedLegal)
			throw -2;
		//�жϲ����Ƿ��쳣
		if((iFormatType>2) || (iFormatType<1) || (NULL == szParamContext))
			throw -3;
		//�жϽ���Ƿ��Ѿ�׼����
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
		RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�ۿ����ӿ�(IF_GetDebitResult)����������:(%s)%s "),(0x01 == iFormatType)?_T("���ݸ�ʽ:XML"):_T("���ݸ�ʽ:JSON"),strDataString));

		_tcscpy_s(szParamContext,strDataString.GetLength()+1,strDataString.GetBuffer(0));
		strDataString.ReleaseBuffer();
		//���ý��׽��
		m_sCurrentDebitResult.Valid = FALSE;

		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
bool WINAPI	CTWSDNetPay_DllApp::IF_SetMMI(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[��������Ӧ],���ý�������ӿ�(IF_SetMMI)���ÿ�ʼ,����1:%s,����2:%d,����3:%d"),szParamContext,iParamSize,iFormatType);
	strLog.Replace(_T("\r"),_T(""));
	strLog.Replace(_T("\n"),_T(""));
	RecordLog(strLog);

	CString strParamString(szParamContext);
	CString strDataString;
	CString strSubError;
	try
	{
		//�ж�����Ƿ��Ѿ�����ʼ��
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�����Ƿ�ɹ���Ȩ
		if(!m_bAuthorizedLegal)
			throw -2;
		//�ж�ҵ�����߳��Ƿ��쳣
		if(NULL == m_pThreadManage)
			throw -3;
		if(NULL == m_pThreadManage->m_pThreadDevice)
			throw -4;
		//�жϲ����Ƿ��쳣
		if((NULL == szParamContext) || (0 == iParamSize))
			throw -5;
		//�жϲ�������
		if( (iFormatType>2) || (iFormatType<1))
			throw -6;
		//��ȡӲ��״̬
	//	DWORD dwHDState = m_pThreadManage->m_pThreadDevice->GetDeviceState();

		int iMMIType = 0x00;

		if(0x01 == iFormatType)
		{
			//XML��ʽ
			CDVXMLMMI xml;
			if(!xml.Init(strParamString))
			{
				strSubError = xml.m_strLastErrorDesc;
				throw -7;
			}
			//�ع�XML����
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
					//��ȡ�����ͳ��֡�����������
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
						case 1://����
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
			//JSON��ʽ
			CDVJsonParamMMI* pJsonMMI = NULL;
			CJsonParamOperate jsonOperation;

//			strParamString.Format(_T("{\"MMIParamContext\":{\"Data\":{\"ClearMMI\":0,\"ItemData\":{\"Graph\":null,\"Voice\":null,\"Text\":[{\"Sub\":1,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"1\"},{\"Sub\":2,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"200\"},{\"Sub\":3,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":4,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":5,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":6,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"0\"},{\"Sub\":7,\"Row\":0,\"Type\":1,\"IsScroll\":0,\"Context\":\"����᯴�\"}]},\"ItemCount\":7},\"Version\":\"1.0\"}}"));

			pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString,JSON_DT_PARAM_MMI);
			if(NULL == pJsonMMI)
			{
				strSubError.Format(_T("%s"),jsonOperation.GetLastErrorDesc());
				throw -8;
			}

			//�ع�XML����
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
				//��ȡ�����ͳ��֡�����������
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
					case 1://����
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

		//��ȡ����
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -10;
		pstrString->Format(_T("%s"),strDataString);

		m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO,(WPARAM)pstrString,(LPARAM)iMMIType);

		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		case -10:
			strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,����ڲ���Դ�쳣"));
			break;
		case -7:
			strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,��������(XML��ʽ)������ϸ����:%s"),strSubError);
			break;
		case -8:
			strLog.Format(_T("[��������Ӧ],���ý����������(IF_SetMMI)����,��������(JSON��ʽ)������ϸ����:%s"),strSubError);
			break;
		}
		//����������� 
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}
	RecordLog(FmtStr(_T("[��������Ӧ],���ý�������ӿ�(IF_SetMMI)���ý���")));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�WINAPI	CTWSDNetPay_DllApp::IF_GetParam
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
bool WINAPI	CTWSDNetPay_DllApp::IF_GetParam(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT char* szResultContext,IN OUT int& iResultSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[��������Ӧ],��ȡӲ�������ӿ�(IF_GetParam)�ݲ��ṩ�����"));
	
	if(strLog != m_strLastErrorDesc)
	{
		m_strLastErrorDesc = strLog;
		RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
//	RecordLog(FmtStr(_T("[��������Ӧ],������״̬�ӿ�(IF_GetComponentStatus)���ÿ�ʼ,����1:%d"),uStatus));
	try
	{
		//�ж�����Ƿ��Ѿ�����ʼ��
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�����Ƿ�ɹ���Ȩ
		if(!m_bAuthorizedLegal)
			throw -2;
		//�ж�ҵ�����߳��Ƿ��쳣
		if(NULL == m_pThreadManage)
			throw -3;
		//�ж�Ӳ�������߳��Ƿ�����
		if(NULL == m_pThreadManage->m_pThreadDevice)
			throw -4;

		//֧����ƽ̨����״̬
		if(m_bNetErrorHttpServerInTollNetNotExist)
		{
			dwState |= DEVICE_STATE_PAY_WEB;
		}
		//֧����ƽ̨������״̬
		if(m_bNetErrorHttpServerTrawePlatformNotExist)
		{
			dwState |= DEVICE_STATE_PAY_SERVER;
		}
		//��ȡӲ��״̬
		DWORD dwHDState = m_pThreadManage->m_pThreadDevice->GetDeviceState();
		
		uStatus = (dwState | dwHDState);

		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
		}
	}
//	RecordLog(FmtStr(_T("[��������Ӧ],������״̬�ӿ�(IF_GetComponentStatus)���ý���")));

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
bool WINAPI CTWSDNetPay_DllApp::IF_TranslateData(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = FmtStr(_T("[��������Ӧ],ִ�д���ؼ���Ϣ���ݽӿ�(IF_TranslateData)�ݲ��ṩ�����"));
	if(strLog != m_strLastErrorDesc)
	{
		m_strLastErrorDesc = strLog;
		RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
bool WINAPI CTWSDNetPay_DllApp::IF_Authority(IN const char* szAuthKey,IN const int& iAuthKeySize)
{
	bool bReturn = false;

	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_TransferImage
//     
// ��������������ͼ��
//     
// ���������	IN const char* szImageInfo	ͼ�����ݴ���XML��JSON��
//				IN const int& iSize			ͼ�����ݴ���С
//				IN const int& iFormatType	ͼ�����ݴ���ʽ��XML��JSON��
// ���������bool
// ��д��Ա��WingF
// ��дʱ�䣺2017��8��13��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ڲ��ӿ�
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool WINAPI CTWSDNetPay_DllApp::IF_TransferImage(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog = _T("");
	try
	{
		//�ж�����Ƿ��Ѿ�����ʼ��
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�ҵ�����߳��Ƿ��쳣
		if(NULL == m_pThreadManage)
			throw -2;

		int nImageType = 0;
		CString	csPayID = _T("");
		CString	csImageData = _T("");

		CString	strImage = _T("");
		strImage.Format( _T("%s"), szImageInfo );

		m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ��ͼ�������(IF_TransferImage), ���ڽ���ͼ�������"));
		//��������ȡ�ؼ�����
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

		//���촫��XML����
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
		m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ��ͼ�������(IF_TransferImage), ���ڽ���ͼ���ϴ�����"));
		if( 0 == (iResultCode = m_AgencyPay.AgencyDebitImage( strImageParam.GetBuffer(0), nImageLength, 1)) )
		{
			RecordLog(FmtStr(_T("[��������Ӧ],ִ��ͼ�������(IF_TransferImage)�����óɹ�")));
			bReturn = true;
		}
		else
		{
			RecordLog(FmtStr(_T("[��������Ӧ],ִ��ͼ�������(IF_TransferImage)�쳣, ��������ʧ��(%d)"),iResultCode) );
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,���δ�ɹ���ʼ��"));
			break;
		case -2:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,ҵ���߳��쳣"));
			break;
		case -11:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����XML���������쳣"));
			break;
		case -12:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����XML�������ڵ��쳣"));
			break;
		case -13:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����XML���� ������ Data �쳣"));
			break;
		case -14:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����XML���� ���� Type �쳣"));
			break;
		case -15:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����XML���� ���� PayIdentifier �쳣"));
			break;
		case -16:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����XML���� ���� Image �쳣"));
			break;
		case -21:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����JSON���������쳣"));
			break;
		case -22:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����JSON���� ���� Type �쳣"));
			break;
		case -23:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����JSON���� ���� PayIdentifier �쳣"));
			break;
		case -24:
			strLog.Format(_T("[��������Ӧ],ͼ����(IF_TransferImage)����,����JSON���� ���� Image �쳣"));
			break;
		}
		//�����������
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			RecordLog(m_strLastErrorDesc);
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�WINAPI	CTWSDNetPay_DllApp::IF_AgencyDebitQuery
//     
// �������������۲�ѯ����ѯ�����Ƿ�֧�ִ��ۣ�
//     
// ���������IN const char* szParamContext
//         ��IN const int& iSize
//         ��IN const int& iFormatType
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��10��16��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_AgencyDebitQuery(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT int& iResult)
{
	//�ж��Ƿ����ô��۶�̬��
	if(!m_bUseAgencyDll)
	{
		return false;
	}
	//ִ�д��۲�ѯ
	iResult = m_AgencyPay.AgencyDebitQuery(szParamContext,iParamSize,iFormatType);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�WINAPI	CTWSDNetPay_DllApp::IF_InnerGetDeviceNo
//     
// ������������ȡӲ�����ն����
//     
// ���������IN char* szParamContext
//         ��IN int& iSize
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��10��16��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ڲ��ӿ�
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_InnerGetDeviceNo(IN char* szParamContext,IN int& iSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	DWORD dwState = 0x00;
	CString strLog;
//	RecordLog(FmtStr(_T("[��������Ӧ],�ڲ��ӿ�(IF_InnerGetDeviceNo)���ÿ�ʼ,����1:%d"),szParamContext,iSize));
	try
	{
		//�ж�����Ƿ��Ѿ�����ʼ��
		if(!m_bIsInitEnvironment)
			throw -1;
		//�ж�����Ƿ�ɹ���Ȩ
		if(!m_bAuthorizedLegal)
			throw -2;
		//�жϲ��������Ƿ���Ч
		if((NULL == szParamContext) || (iSize<24))
			throw -3;

		if(!m_strTWProductSerialNo.IsEmpty())
		{
			memset(szParamContext,0,iSize);
			strcpy_s(szParamContext,iSize,m_strTWProductSerialNo.GetBuffer(0));
			m_strTWProductSerialNo.ReleaseBuffer();
		}

		//��ǳɹ�
		bReturn = true;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
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
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
		}
	}
//	RecordLog(FmtStr(_T("[��������Ӧ],�ڲ��ӿ�(IF_InnerGetDeviceNo)���ý���")));

	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_InnerGetSpecialData
//     
// ������������ȡָ������
//     
// ���������IN char* szParamContext
//         ��IN int& iParamSize
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��10��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ڲ��ӿ�
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_InnerGetSpecialData(IN char* szParamContext,IN int& iParamSize)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::IF_InnerSetSpecialData
//     
// ��������������ָ������
//     
// ���������IN const char* szParamContext
//         ��IN const int& iParamSize
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��10��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ڲ��ӿ�
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::IF_InnerSetSpecialData(IN const char* szParamContext,IN const int& iParamSize)
{
	return false;
}

/****************************************�ڲ���������********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::RecordLog
//     
// ������������¼��־
//     
// ���������IN const CString& strLog -- ��־����
//         ��IN const int& iLevel	  -- ��־�ȼ�
//         ��IN const BOOL& bSave	  -- ��־�Ƿ�������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
	TCHAR szTemp[256]={0};
	CString strConfigFilePath;
	strConfigFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
	//�Ƿ񱣴���־
	m_bSaveLog				= (BOOL)::GetPrivateProfileIntA(_T("Log"),_T("Save"),0,strConfigFilePath);
	//��ȡ��־ɾ���޶�ʱ������
	m_iDeleteLogStartHour = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteStartHour"),2,strConfigFilePath);
	//��ȡ��־ɾ���޶�ʱ����յ�
	m_iDeleteLogEndHour = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteEndHour"),2,strConfigFilePath);
	//��־ɾ��������ѯʱ��
	m_iDeleteLogSpanTime = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteSpanTime"),30*60,strConfigFilePath);
	//��־���ر�������ʱ��
	m_iDeleteLogSaveDays = ::GetPrivateProfileIntA(_T("Log"),_T("DeleteSaveDays"),60,strConfigFilePath);
	//����Ƿ��������
	m_bIsTestData			= (BOOL)::GetPrivateProfileIntA(_T("Setup"),_T("TestData"),0,strConfigFilePath);
	//�̴߳����־���ʱ����
	m_dwThreadCheckSpanTime = ::GetPrivateProfileIntA(_T("Setup"),_T("ThreadCheckSpanTime"),0,strConfigFilePath);
	//��ȡ���⿨ǰ׺
	::GetPrivateProfileString(_T("Setup"),_T("VirtualCardPrefix"),_T("4412"),szTemp,256,strConfigFilePath);
	m_strVirtualCardPrefix.Format(_T("%s"),szTemp);
	//��ȡ������Ӳ���·�ʱ��ļ��
	m_dwUpdateDevieTimeSpanTime = (DWORD)::GetPrivateProfileIntA(_T("Setup"),_T("UpdateDeviceTimeSpanTime"),30*60,strConfigFilePath);
	m_dwUpdateDevieTimeSpanTime *= 1000;
	//���������ط����ʱ����
	m_tResendDataSpanTime		= (DWORD)::GetPrivateProfileInt(_T("CancelRecord"),_T("CheckTime"),30,strConfigFilePath);
	//�����������汾��ʱ�ޣ���λ�����ӣ�
	m_iLastDelayTime			= (DWORD)::GetPrivateProfileInt(_T("CancelRecord"),_T("LastDelay"),30,strConfigFilePath);
	//�����˳�ǰ�ĵȴ�ʱ�䣨�ȴ���Դ�����
	m_iExitWaitForTime			= (DWORD)::GetPrivateProfileInt(_T("Setup"),_T("ExitWaitForTime"),500,strConfigFilePath);
	//HTTP�ڲ����
	m_iHTTPVersion				= ::GetPrivateProfileInt(_T("ReflectWeb"),	_T("Version"),	0,	strConfigFilePath);
	//ȷ��ͨѶ��̬����ص��豸��������
	m_iDeviceType				= ::GetPrivateProfileInt(_T("Setup"),		_T("Device"),	0,	strConfigFilePath);
	//�Ƿ��ƶ�֧���ʹ���֧����֧�����������뱣��һ��
	m_bReturnPayChannelCodeUnify	= (bool)::GetPrivateProfileInt(_T("Setup"),		_T("ReturnPayChannelCodeUnify"),0,strConfigFilePath);
	//��ȡ���ۿ۷Ѳ�����ʱʱ��ֵ
	m_iAgencyDebitTimeout		= ::GetPrivateProfileInt(_T("Setup"),_T("AgencyDebitTimeout"),2000,strConfigFilePath);
	//��ȡ���������ڲ���ʱʱ��ֵ
	m_iClearMMIDelayTime		= ::GetPrivateProfileInt(_T("Setup"),_T("ClearMMIDelayTime"),100,strConfigFilePath);
	//�Ƿ����ô��۶�̬��
	m_bUseAgencyDll				= (bool)::GetPrivateProfileInt(_T("Setup"),_T("UseAgencyPayDll"),1,strConfigFilePath);
	//�Ƿ�ʹ����չ�Ľ���ϸ��ģʽ
	m_bIsExternMMIDetail		= (BOOL)::GetPrivateProfileInt(_T("Setup"),_T("UseExternMMI"),1,strConfigFilePath);
	
	//�Ƿ����������ǽ�ģ��
	m_bNFCIsUse							= (bool)::GetPrivateProfileInt(_T("Unipay"),_T("UseUnipay"),0,strConfigFilePath);
	//����ģ��ǩ��״̬���ʱ����
	m_iNFCCheckSignSpanTime				= ::GetPrivateProfileInt(_T("Unipay"),_T("CheckSignSpanTime"),10,strConfigFilePath);
	if(m_iNFCCheckSignSpanTime>1800 || m_iNFCCheckSignSpanTime<10)
	{
		m_iNFCCheckSignSpanTime = 30;
	}
	//����ģ�鶨ʱ��ǩ���ʱ����ʼ��
	m_iNFCDefaultStartReSignHour		= ::GetPrivateProfileInt(_T("Unipay"),_T("AutoStartReSignHour"),0,strConfigFilePath);
	//����ģ�鶨ʱ��ǩ���ʱ���ֹ��
	m_iNFCDefaultEndReSignHour			= ::GetPrivateProfileInt(_T("Unipay"),_T("AutoEndReSignHour"),3,strConfigFilePath);
	//����ģ��ǩ�����ݽ���ʱ��������
	m_iNFCSignDataLimitSpanTime			= ::GetPrivateProfileInt(_T("Unipay"),_T("SignDataLimitSpanTime"),60,strConfigFilePath);
	if(m_iNFCSignDataLimitSpanTime>70 || m_iNFCSignDataLimitSpanTime<30)
	{
		m_iNFCSignDataLimitSpanTime = 60;
	}
	//����֧�����̣��Ƿ�����û�л�ȡ��������Ҳ�����Ϳ۷�����
	m_bNFCIsAllowDebitWithoutCancelData	= (BOOL)::GetPrivateProfileInt(_T("Unipay"),_T("AllowDebitWithoutCancelData"),0,strConfigFilePath);
	//����֧�����̣���ȡ��ˢ�����ݺ����Ƶȴ��������ݵ�ʱ�䣨��λ���룩
	m_iNFCGetCancelDataLimitTime		= ::GetPrivateProfileInt(_T("Unipay"),_T("GetCancelDataLimitTime"),10,strConfigFilePath);
	if(m_iNFCGetCancelDataLimitTime>15 || m_iNFCGetCancelDataLimitTime<5)
	{
		m_iNFCGetCancelDataLimitTime = 10;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::fnCloseThread
//     
// �����������ȴ�һ���������ر��߳������ر�
//     
// ���������IN const HANDLE& hThread			--	��Ҫ���رյ��߳̾��
//         ��IN const DWORD& dwMilliseconds		--	�ȴ�ʱ�䣨��λ�����룩
// ���������int
//		   ��0  -- �߳��Ѿ��������ر�
//         ��-1	-- �߳̾����Ч
//         ��-2	-- �ں˶���δ�������ȴ�ʱ���ѵ�
//         ��-3	-- �ȴ��Ļ�����δ���ͷ�
//         ��-4	-- �ȴ�����ִ��ʧ��
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::fnCloseThread(IN const HANDLE& hThread,IN const DWORD& dwMilliseconds)
{
	int iReturn = 0;
	//���Թر��߳�
	DWORD dwWaitResult = ::WaitForSingleObject(hThread,dwMilliseconds);
	//��ô�����
	DWORD dwErrorCode = ::GetLastError();
	//��Ϣ�ṹ����
	MSG	 mMsg;

	if (6 == dwErrorCode)//�����Ч
	{
		iReturn = 0;
	}
	else
	{
		//�жϵȴ��������ؽ��
		switch(dwWaitResult)
		{
		case WAIT_OBJECT_0:	//�ں˶����Ϊ����״̬
			iReturn = 0;
			break;
		case WAIT_TIMEOUT:	//�ں˶���δ�������ȴ�ʱ���ѵ�
			{
				if(::PeekMessage(&mMsg,NULL,0,0,PM_REMOVE))
				{
					::TranslateMessage(&mMsg);Sleep(10);
					::DispatchMessage(&mMsg);Sleep(10);
				}
			}
			iReturn = -2;
			break;
		case WAIT_ABANDONED://�ȴ��Ļ�����δ���ͷ�
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
//	��	  �� : ROCY
//	ʱ	  �� : 2008-05-31
//  ��	  �� : 1.0.0.1
//	�������� : DisplayWinError
//  �������� : ��ʾϵͳ���µĴ�����Ϣ
//  ������� : DWORD	--	dwErrorCode	������ϵͳ�������,һ���� GetLastError() ��������
//			   int		--	iType		Ĭ��Ϊ0
//			   BOOL		--	bPrompt		�Ƿ���Ҫ�öԻ�����ʾ�ô�����Ϣ��TRUEΪ��Ҫ��FALSEΪ����Ҫ��
//  ������� : LPCTSTR -- ��ϸ�Ĵ���������Ϣ
//  ��ע˵�� : ÿ�ε���API�󶼿��Ե��ô˺����鿴��ϸ�Ĵ�����Ϣ
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
	strTemp.Format(_T("���������:%d�� -- ����ϸ����:%s��"),dwErrorCode,A2T(lpMessageBuffer));
	strTemp.Replace(_T("\r\n"),_T(" "));

	if(bPrompt)
		::MessageBox(NULL,strTemp,_T("ϵͳ��ʾ"),MB_ICONWARNING);

    LocalFree(lpMessageBuffer);
	return strTemp;
}

//��ȡ��ǰ������ʹ�õ��ڴ�
DWORD CTWSDNetPay_DllApp::GetProcessMemory(IN const int& iShowType)
{
	DWORD dDevValue = (1 == iShowType)?(1024*1024):1024;
	//��ȡ��ǰ����ʹ���ڴ���Ϣ
	DWORD dwProcessMemory = 0x00;
	PROCESS_MEMORY_COUNTERS pLocalProcessMemory = {0};
	if(GetProcessMemoryInfo(GetCurrentProcess(),&pLocalProcessMemory,sizeof(pLocalProcessMemory)))
	{
		dwProcessMemory = pLocalProcessMemory.PagefileUsage/dDevValue;
	}

	return dwProcessMemory;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::AgencyInitDll
//     
// ������������ʼ�����۶�̬��
//     
// ���������void
// ���������bool
// ��д��Ա��ROCY
// ��дʱ�䣺2017��8��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::AgencyInitDll(IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID)
{
	bool bInitAgencyDll = false;

	//�ж��Ƿ����ô��۶�̬��
	if(!m_bUseAgencyDll)
	{
		RecordLog( FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)�����۶�̬��δ����!)")) );
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
			RecordLog( FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)�����۶�̬��(%s)��ʼ���ɹ�!)"), strAgencyPayDll) );
		}
		else
		{
			RecordLog( FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�ڲ���ʼ������,�޷���ʼ�����۶�̬��(�������:%d)"),iResultCode) );
		}
	}
	else
	{
		RecordLog( FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment),�ڲ���ʼ������,���۶�̬��(%s)����ʧ��"),strAgencyPayDll) );
	}
	return bInitAgencyDll;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::AgencyConstructDebitData
//     
// ����������
//     
// ���������void
// ���������int
// ��д��Ա��ROCY
// ��дʱ�䣺2017��8��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵���������ؽ��Ϊtrue�����ʾ���ٽ��к�����ɨ��ҵ�����̣������ؽ��Ϊfalse�����ʾ������Ҫִ��ɨ��ҵ������
//     
////////////////////////////////////////////////////////////////////////////////////////////////
bool CTWSDNetPay_DllApp::AgencyConstructDebitData()
{
	//�ж��Ƿ����ô��۶�̬��
	if(!m_bUseAgencyDll)
	{
		return false;
	}

	//�����жϳ�����Ч����������7λ��
	if( m_pCurrentDebitInfo.VehicleLicense.GetLength() < 7 )
		return false;

	BYTE byFunCode		= 0x01;
	BYTE byExecuteState	= RCC_INNER_ERROR;

	m_dwTradeBeginOvertime = ::GetTickCount();	//��ʱ
	m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney), ���ڽ��г������۴���"));

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
			//֪ͨ�ɹ�
			byFunCode = 0x01;
			byExecuteState = RCC_OK;
			//�����ն���ʾ�ɹ�
			m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)0x01,(LPARAM)0);
			//��ȡ�����Ϣ�ɹ���֪ͨ������
			NotifyCaller(byFunCode,byExecuteState);
			//��¼��־
			RecordLog(FmtStr(_T("[��������Ӧ],ִ�пۿ�����ӿ�(IF_DebitMoney)���ý���(%s,����֧��,�ɹ���"),m_pCurrentDebitInfo.VehicleLicense));
			return true;
		}
		else
		{
			iIsCancelAnency = 0x01;
		}
	}
	else
	{
		//�жϷ��ش���ȷ���Ƿ���Ҫ������۳���
		switch(iAnencyResult)
		{
		case 6:
		case 12:
			iIsCancelAnency = 0x02;
			break;
		}
	}
	
	//�ж��Ƿ���Ҫִ�д��۳���
	if(iIsCancelAnency>0)
	{
		//��ʶ�۷��������
		GetGlobalApp()->m_bIsTrading		= FALSE;
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;

		AgencyConstructCancelData(m_pCurrentDebitInfo.PayIdentifier);
		//֪ͨʧ��
		byFunCode = 0x01;
		byExecuteState = RCC_PAY_PLATFORM_ERROR;
		//֪ͨ������
		NotifyCaller(byFunCode,byExecuteState);
		//��¼��־
		RecordLog(FmtStr(_T("[��������Ӧ],����(%s)���ۿ۷ѽ������ʧ��(%s)���Ѿ����𳷵���ת��ɨ�봦��"), m_pCurrentDebitInfo.VehicleLicense,(0x01 == iIsCancelAnency)?_T("��ȡ����쳣"):_T("���糬ʱ")) );
		//���ٽ���ɨ������
		return true;
	}
	else
	{
		RecordLog(FmtStr(_T("[��������Ӧ],����(%s)���ۿ۷ѽ������ʧ�ܣ�����Ҫ������ת��ɨ�봦��"), m_pCurrentDebitInfo.VehicleLicense) );
	}
	//��������ɨ������
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::AgencyConstructCancelData
//     
// ����������������۳������ݽṹ��
//     
// ����������޲���
// ��д��Ա��ROCY
// ��дʱ�䣺2017��8��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CTWSDNetPay_DllApp::AgencyConstructCancelData(IN const CString& strPayIdentifier)
{
	//�ж��Ƿ����ô��۶�̬��
	if(!m_bUseAgencyDll)
	{
		return false;
	}

	//��ȡ�����Ϣʧ�ܣ����г�������ת��ɨ��
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
// �������ƣ�CTWSDNetPay_DllApp::GetAgencyResult
//     
// �����������������۵ķ��ؽ���ַ������γɸ�ʽ�ַ���
//     
// ���������CString strXML
// ���������int
// ��д��Ա��ROCY
// ��дʱ�䣺2017��8��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
			//�ж��Ƿ����֧��
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

			//�ж��Ƿ񽫴���֧���ϲ����ƶ�֧��ԭ�ж��嵱��
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

			//��ȡ��������ʱ��
			m_sCurrentDebitResult.ConsumeTime = dwConsumeTime;

			//��������
			m_sCurrentDebitResult.ConsumeTime	= (::GetTickCount() - m_dwTradeBeginOvertime);
			m_sCurrentDebitResult.Valid			= TRUE;

			//��ʶ�۷��������
			GetGlobalApp()->m_bIsTrading		= FALSE;
			GetGlobalApp()->m_bCurrentDebitMode = FALSE;
			//���ش���
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
			m_strLastErrorDesc.Format(_T("���۽������ʧ�ܣ�����XML���쳣"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("���۽������ʧ�ܣ�δ�ҵ�XMLͷ DebitResultContext"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� Data"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� ResultType"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� ExecuteCode"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� ExecuteDesc"));
			break;
		case -14:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� TradeKeyItem"));
			break;
		case -15:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� TradeSecurityCode"));
			break;
		case -16:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� PayPlatformType"));
			break;
		case -17:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� PayIdentifier"));
			break;
		case -18:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� DebitOrder"));
			break;
		case -19:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ�ҵ�ָ�������� DebitTime"));
			break;

		default:
			m_strLastErrorDesc.Format(_T("���۽������ʧ��, δ֪�쳣"));
			break;
		}
		RecordLog( m_strLastErrorDesc );
	}

	return iReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTWSDNetPay_DllApp::Hex2Bin
//     
// ������������16���Ƶ��ַ���ת����2���Ƶ����� 
//     
// ���������IN LPTSTR lpHex	-- ��Ҫת����16�����ַ���
//         ��OUT PBYTE pBin		-- �����2������������
//         ��IN DWORD dwHex_len	-- ��Ҫת����16�������ݳ���,����Ϊ2�ı���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����������16�����ַ������ȱ�����2�ı���
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
// �������ƣ�CTWSDNetPay_DllApp::Bin2Hex
//     
// ������������2���Ƶ�����ת����16���Ƶ��ַ���
//     
// ���������IN PBYTE pBin			-- ��Ҫת����2������������
//         ��IN DWORD dwBin_Len		-- ��Ҫת����2�����������鳤��
// ���������CString  �ǿ� -- ת���ɹ��������16�����ַ���;NULL -- ת��ʧ��
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CTWSDNetPay_DllApp::HexToBase64
//     
// ����������ת��16�����ַ���Ϊbase64����
//     
// ���������IN const CString& strSrcHex
//         ��OUT CString& strDesBase64
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�����ڲ��ӿ�
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

		//������Դ
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
// �������ƣ�CTWSDNetPay_DllApp::Base64ToHex
//     
// ����������ת��base64����Ϊ16�����ַ�
//     
// ���������IN const CString& strSrcBase64
//         ��OUT CString& strDesHex
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CTWSDNetPay_DllApp::Base64ToHex(IN const CString& strSrcBase64,OUT CString& strDesHex)
{
	BOOL bReturn = FALSE;
	try
	{
		//�жϳ���
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
// �������ƣ�CTWSDNetPay_DllApp::BASE64_Encode
//     
// ��������������ת����������
//     
// ���������const BYTE* inputBuffer
//         ��INT inputCount
//         ��TCHAR* outputBuffer
// ���������INT
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		return -1;  // ��������
	}

	if (outputBuffer != NULL)
	{
		for (i = inputCount; i > 0; i -= 3)
		{
			if (i >= 3)
			{  // ��3�ֽ�����ת����4��ASCII�ַ�
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

		*outputBuffer++ = TEXT('/0');  // ����ַ����������
	}

	return ((inputCount + 2) / 3) * 4;  // ������Ч�ַ�����
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