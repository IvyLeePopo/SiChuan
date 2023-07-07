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
//	CString strTemp(_T("2017-03-31 16:26:15"));

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
	return TRUE;
}

int CTWSDNetPay_DllApp::ExitInstance()
{
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

		//��ȡ�������������IP��ַ
		char szTempIP[256]={0};
		CString strReflectWebFilePath;
		strReflectWebFilePath.Format(_T("%s\\%s"),m_strWorkDir,FILE_NAME_APP_CONFIG);
		::GetPrivateProfileString(_T("ReflectWeb"),_T("IP"),_T("101.200.214.203"),szTempIP,_countof(szTempIP),strReflectWebFilePath);
		_tcscpy_s(m_sTranstParam.LaneConfigInfo.WebServerIP,_countof(m_sTranstParam.LaneConfigInfo.WebServerIP),szTempIP);

		//�ж��Ƿ��Ѿ���ʼ��
		if( m_bIsInitEnvironment )
			throw -11;

		RecordLog(FmtStr(_T("[��������Ӧ],�����ʼ���ӿ�(IF_InitEnvironment)���ڲ���������:%s(ʡ�ݱ���:%d,�������:%d,·�α���:%d,վ�����:%d)"),strSTName,m_sTranstParam.ProvinceID,m_sTranstParam.LaneConfigInfo.AreaID,m_sTranstParam.LaneConfigInfo.RoadID,m_sTranstParam.LaneConfigInfo.StationID));

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
		if(NULL != m_pThreadManage && !(m_pThreadManage->m_bExit))
		{
			//���ø��߳��Ƴ����Ϊ��
			m_pThreadManage->m_bExit = TRUE;
			//��Ĭ50����
			::Sleep(50);

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


		if(strDataString.IsEmpty())
			throw -7;
		RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�û���Ϣ�ӿ�(IF_GetAccountInfo)�������:%s"),strDataString));
		int tmpLen = strDataString.GetLength();
		iParamSize = tmpLen>=iParamSize?(iParamSize-1):tmpLen;

		memcpy(szParamContext,strDataString.GetBuffer(0),iParamSize);
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
	CString strOperationMode;
	CString strDataString;
	CString strAnalyError;
	int nFType;

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
		nFType = iFormatType; //�ײ�ʹ��json��ʽ���� ��1.xml 2.json��

		if(1 == nFType)
		{
			CDVXMLDebitParamContext xml;

			if(!xml.Init(strDataString))
			{
				strAnalyError = xml.m_strLastErrorDesc;
				throw -4;
			}

			//��������
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
			//��������
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
						strSTID.Format(_T("%04d%03d%02d"),m_sCurrentDebitInfo.EntryAreaID,m_sCurrentDebitInfo.EntryRoadID,m_sCurrentDebitInfo.EntryStationID);
						m_sCurrentDebitInfo.EntryStationID = _ttoi(strSTID);
					}
					break;
				}
			}
			break;
		}

		//���ҵ������߳��Ƿ�����
		if(NULL == m_pThreadManage)
			throw -6;

		//�ж��ϴν����Ƿ����
		if(m_bIsTrading)
			throw -7;

		//��ǵ�ǰ�������ڽ���
		m_bIsTrading = TRUE;

		/*****************************��Ӧ����***********************************/

		strOperationMode.Format(_T("����ģʽ"));
		m_bCurrentDebitMode = TRUE;


		//������Դ����
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -8;

		//�ع��ۿ�JSON/xml
		nFType = 2;
		pstrString->Format(_T("%s"),CostructDebitInfo( nFType, m_sCurrentDebitInfo));

		//�ر��ϴεĶ�ʱ��
		m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
		//�����ۿʱ��ʱ��
		m_pThreadManage->StartTimer(TIMER_ID_DEBIT_MONEY,m_sCurrentDebitInfo.OverTime);
		//��ʼ��ʱ
		m_dwTradeBeginOvertime = GetTickCount();
		//�����ۿ�����
		m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_START_DEBIT_FLOW,(WPARAM)pstrString,(LPARAM)nFType);//�����ݽ����������͹�ȥ

		//�������
		m_strLastErrorDesc.Format(_T("[��������Ӧ],ִ�пۿ����(IF_DebitMoney),ҵ�����ڴ���(%s)"),strOperationMode);
		
		//��¼��־
		RecordLog(m_strLastErrorDesc);

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
		//Ԥ����ʧ�ܣ����������߽��
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(m_sTranstParam.CallerWnd))
		{
			::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}
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
			strPayIdentifier.Format(_T("%s"), pDebitCancel->PayIdentifier);

			delete pDebitCancel;
			pDebitCancel = NULL;
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
		//	pstrString->Format(_T("%s"),ConstructCancleInfo(iFormatType,strPayIdentifier));

		m_pThreadManage->PostThreadMessage(WM_THREAD_MANAGE_DEBIT_CANCEL,(WPARAM)pstrString,(LPARAM)iFormatType);//�ѽ����������͹�ȥ

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
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(m_sTranstParam.CallerWnd))
		{
			::PostMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0,0)),(LPARAM)0);
		}
		//������־
		RecordLog(FmtStr(_T("[��������Ӧ],��������Ԥ����ʧ�ܣ������֪ͨ������(���ܴ���:%d,ִ��״��:%d)"),byFunCode,byExecuteState));
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
		//�жϲ����Ƿ��쳣
		if((iFormatType>2) || (iFormatType<1) || (NULL == szParamContext))
			throw -2;

		//���ҵ������߳��Ƿ�����
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

		//������ת����Ҫ�ĸ�ʽ
		ConverToFormatData(iFormatType,strResult);


		strResult.TrimRight();
		RecordLog(FmtStr(_T("[��������Ӧ],��ȡ�ۿ����ӿ�(IF_GetDebitResult)����������:(%s)%s "),(0x01 == iFormatType)?_T("���ݸ�ʽ:XML"):_T("���ݸ�ʽ:JSON"),strResult));

		_tcscpy_s(szParamContext,strResult.GetLength()+1,strResult.GetBuffer(0));
		strResult.ReleaseBuffer();

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

	int nFormateType = iFormatType;
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


		BOOL bIsClearCmd;
		ConstructMMIInfo(iFormatType,strParamString,bIsClearCmd);

		if(strParamString.IsEmpty())
			throw -9;

		if(bIsClearCmd)
			nFormateType = 0x01;

		//��ȡ����
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -10;
		pstrString->Format(_T("%s"),strParamString);

		m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO,(WPARAM)pstrString,(LPARAM)nFormateType);

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
		case -9:
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
		//�ն��豸״̬/����״̬
		CString strState;
		if(m_pThreadManage->GetHKResult(1,strState))
		{
			dwState = _ttoi(strState);
		}

		////֧����ƽ̨����״̬
		//if(m_bNetErrorHttpServerInTollNetNotExist)
		//{
		//	dwState |= DEVICE_STATE_PAY_WEB;
		//}
		////֧����ƽ̨������״̬
		//if(m_bNetErrorHttpServerTrawePlatformNotExist)
		//{
		//	dwState |= DEVICE_STATE_PAY_SERVER;
		//}
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



//test c0 frame count
bool WINAPI CTWSDNetPay_DllApp::IF_GetC0SuccFaileCount(OUT int& nSucce, OUT int& nFailed, OUT int& nReSendCount, OUT char* strA2Frame, OUT char* strC0Frame)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool bReturn = false;
	CString strLog;
	CString tmpstrA2Frame;
	CString tmpstrC0Frame;
	//	RecordLog(FmtStr(_T("[��������Ӧ],C0֡ͳ�ƽӿ�(IF_GetC0SuccFaileCount)���ÿ�ʼ"));
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
			strLog.Format(_T("[��������Ӧ],C0֡ͳ��(IF_GetC0SuccFaileCount)����,���δ�ɹ���ʼ��"));
			break;
		case -2:
			strLog.Format(_T("[��������Ӧ],C0֡ͳ��(IF_GetC0SuccFaileCount)����,���δ����Ȩ"));
			break;
		case -3:
		case -4:
			strLog.Format(_T("[��������Ӧ],C0֡ͳ��(IF_GetC0SuccFaileCount)����,����ڲ���Դ�쳣"));
			break;
		}
		//�����������
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
		}
	}
	//	RecordLog(FmtStr(_T("[��������Ӧ],������״̬�ӿ�(IF_GetC0SuccFaileCount)���ý���")));

	return bReturn;
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
	m_dwThreadCheckSpanTime = ::GetPrivateProfileIntA(_T("Setup"),_T("ThreadCheckSpanTime"),300,strConfigFilePath);
	//��ȡ���⿨ǰ׺
	::GetPrivateProfileString(_T("Setup"),_T("VirtualCardPrefix"),_T("4412"),szTemp,256,strConfigFilePath);
	m_strVirtualCardPrefix.Format(_T("%s"),szTemp);
	//��ȡ������Ӳ���·�ʱ��ļ��
	m_dwUpdateDevieTimeSpanTime = (DWORD)::GetPrivateProfileIntA(_T("Setup"),_T("UpdateDeviceTimeSpanTime"),30*60,strConfigFilePath);
	m_dwUpdateDevieTimeSpanTime *= 1000;
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
				if(::PeekMessage(&mMsg,NULL,0,0,PM_NOREMOVE))
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
	else   //�ع��ۿ�json
	{

		//��ʾ����json����
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

		/***********************************************************����JSAO���ݽṹ***********************************************************/	
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

		//ת��Ϊ�ַ���
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

		//ת��Ϊ�ַ���
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

		jsonDebitResult = (CDVJsonParamDebitResult*)jsonParamOperation.FormatResponseResult2Class(strResult,JSON_DT_PARAM_DEBIT_RESULT);//��Ҫ�ͷ���Դ
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
		//XML��ʽ
		CDVXMLMMI xml;
		if(!xml.Init(strParamString))
		{
			strResult.Empty();
			return FALSE;
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
				case 1://����   Vtype 0�� 1��   //���� Vclass  01~FF
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
		}

		xmlShow.OutOfElem();
		xmlShow.OutOfElem();

		strResult = xmlShow.GetDoc();

	}
	else 
	{
		//JSON��ʽ
		CDVJsonParamMMI* pJsonMMI = NULL;
		CJsonParamOperate jsonOperation;

		pJsonMMI = (CDVJsonParamMMI*)jsonOperation.FormatResponseResult2Class(strParamString,JSON_DT_PARAM_MMI); //��Ҫ�ͷ��ڴ�
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


			//�ع�ΪXML����
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
				case 1://����   Vtype 0�� 1��   //���� VClass  01~FF
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
			strResult.Format(_T("%s"),fast_writer.write(root).c_str());


		}

		delete pJsonMMI;
		pJsonMMI = NULL;

	}


	return TRUE;



}