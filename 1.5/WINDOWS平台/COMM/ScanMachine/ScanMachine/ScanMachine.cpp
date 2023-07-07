// ScanMachine.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ScanMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool	bInitDev = false;		//�Ƿ��ʼ����̬��
CString	csErrorMsg = _T("");	//������Ϣ����
CString	csConfigFile = _T("");	//�����ļ�·��
int		nProvinceID = 0;		//ʡ�ݱ���

int		nDevType = 0;			//ɨ�����豸���ͣ��������ֲ�ͬ���Ҳ�ͬЭ�飩
int		nCommPort = 0;			//ɨ�����豸�˿ں�
int		nBaudRate = 9600;		//ɨ�����豸������

HWND	hMainWnd = NULL;
DWORD	dwMainThreadID = 0;
DWORD	dwMainMessageID = 0;

int		iSerialNum = 0;			//���кţ�������ʾ���������


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

	//��ȡ��̬������·����ֻ���ڶ�̬���Ѿ�ʵ���������
	csAppDir.Format( _T("%s"), GetCurrentDirNew() );
	m_strLogDirPath.Format(_T("%s\\TWSDNetPayLog"),		csAppDir);
	m_strLogFileName.Format(_T("HTS%s.log"), CTime::GetCurrentTime().Format(_T("%Y%m%d")));
	m_strLogFilePath.Format( _T("%s\\%s"), m_strLogDirPath, m_strLogFileName);
	csCfgFile.Format( _T("%s\\%s"),					csAppDir, FILE_NAME_DEVICE_SM_CONFIG);

	//�Ƿ��¼��־���
	iWriteLog	= ::GetPrivateProfileInt(_T("SMControl"),	_T("WriteLog"),	0,	csCfgFile);
	//�Ƿ������·���������/�뿪��0x59,0x60ָ�
	m_iNeedNotifyLoopState = ::GetPrivateProfileInt(_T("SMControl"),	_T("NotifyLoop"),	0,	csCfgFile);
	//��־�ļ����ֵ
	m_iLogFileMaxSize = ::GetPrivateProfileInt(_T("SMControl"),	_T("LogMaxSize"), FILE_SIZE_LOG_MAX ,	csCfgFile);

	theApp.RecordMsgTemp(_T("/**********************************Ӳ�����ƶ�̬�� -- ��������*******************************************/"));


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
	RecordMsgTemp(_T("ɨ������̣߳������˳�..."), TRUE);
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
////��������:fnHexToBin
////��������:��16���Ƶ��ַ���ת����2���Ƶ�����
////�������:IN PBYTE	pHex		-- ��Ҫת����16�����ַ���
////		 OUT PBYTE	pbin		-- �����2������������
////		 IN DWORD	dwHex_len	-- ��Ҫת����16�������ݳ���,����Ϊ2�ı���
////�������:��
////��дʱ��:2008-11-08
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////�����汾:1.0.0.1
////��ע˵��:�������16�����ַ������ȱ�����2�ı���
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
////��������:BinToHex
////��������:��2���Ƶ�����ת����16���Ƶ��ַ���
////�������:IN PBYTE	pBin		-- ��Ҫת����2������������
////		 IN DWORD	dwHex_len	-- ��Ҫת����2�����������鳤��
////�������:CString	�ǿ� -- ת���ɹ��������16�����ַ���
////		 CString	NULL -- ת��ʧ��
////��дʱ��:2008-11-08
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////�����汾:1.0.0.1
////��ע˵��:������óɹ���,����Ҫ�������ͷ��ڴ�
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
//	��	  �� : ROCY
//	ʱ	  �� : 2008-06-01
//  ��	  �� : 1.0.0.1
//	�������� : CRC16
//	�������� : ���ݸ��������ݣ�������CRCУ����
//	������� : LPBYTE	--	lpData	��Ҫ����У����ֽ�������
//			   WORD		--	wLength	��Ҫ����У����ֽڳ���
//	������� : WORD		--	���ص�CRCУ���룬˫�ֽ�
//  ��ע˵�� : ��������У�����ָ��Ϊ�գ��򷵻� 0
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
			CRC16Hi = CRC16Hi / 2; //��λ����һλ
			CRC16Lo = CRC16Lo / 2; //��λ����һλ
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
// �������ƣ�CScanMachineApp::GetCurrentDirNew
//
// ������������ȡ��ǰ�������е�����Ŀ¼���ɵ����߾�����
//
// �����������

// ���������CString		--	��ȡ���ĳ�������Ŀ¼
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CScanMachineApp::GetCurrentDirNew()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	TCHAR szfile[255] = _T("");
	CString csDir = _T("");

	//����Ӧ�ó��򣨵����ߣ����˷�ʽ��ȷ�����ڶ�̬�⣨�������ߣ����˷�ʽ��ȡ���ǳ���·��
	//	DWORD dwLen = GetModuleFileName(NULL, szfile, 255);

	//����Ϊ��̬�������·��
	HINSTANCE hInsApp;
	hInsApp = AfxGetInstanceHandle();
	DWORD dwLen = GetModuleFileName(hInsApp, szfile, 255);

	csDir.Format( _T("%s"), szfile);
	csDir = csDir.Left( csDir.ReverseFind(_T('\\')) );
	return csDir;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CScanMachineApp::CreateDir
//
// ��������������ָ��·����Ŀ¼
//
// ���������IN LPCTSTR lpzPath				--	��Ҫ������Ŀ¼·��
//
// ���������BOOL
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CScanMachineApp::RecordMsgTemp
//
// ������������¼��Ϣ����ʱ�ļ�����־��
//
// ���������IN LPCTSTR strMsg			--	��Ҫ��¼����Ϣ��
//
// ���������void
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
	//���ʱ��
	SYSTEMTIME sysGetTime = {0};
	::GetLocalTime(&sysGetTime);
	CTime m_curtime = CTime::GetCurrentTime();
	CString sTemp = _T("");
	CString strCurrentDateFile;
	strCurrentDateFile.Format(_T("HTS%s.log"), CTime::GetCurrentTime().Format(_T("%Y%m%d")));
	//��־�ļ�����һ��
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
		//����1K�Ž��б���
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
//������:	OleTimeToTime
//��  ��:	�� DATE��ʽʱ��ת��Ϊ time_t��ʽʱ��
//��  ��:	DATE dTime		��ת���� DATE�͸�ʽʱ��
//
//����ֵ:	ת����� time_t�͸�ʽʱ��
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
//������:	CString2time_t
//��  ��:	���ַ����͸�ʽʱ��ת��Ϊ time_t�͸�ʽʱ��
//��  ��:	LPCTSTR lpztime		��ת�����ַ����͸�ʽʱ��
//
//����ֵ:	ת����� time_t�͸�ʽʱ��
//////////////////////////////////////////////////////////////////////////////////////////////////////
time_t CScanMachineApp::CString2time_t(LPCTSTR lpztime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString csTemp = _T("");
	COleDateTime oleGetTime;
	csTemp.Format( _T("%s"), lpztime);
	if ( csTemp.GetLength() > 0 )
	{
		if ( csTemp.GetLength() > 19 ) //�������Ƹ�ʽ _T("2014-11-23 12:34:56.000");
		{
			csTemp = csTemp.Left(19);
		}
		oleGetTime.ParseDateTime(csTemp);
		return OleTimeToTime(oleGetTime.m_dt);
	}
	else return 0;
}












////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_InitDev
//
// ������������ʼ��Ӳ���������������ȵ��ã������ʧ�ܣ�������ӿڲ��ṩ����
//
// ���������	IN const UINT& dwThreadID				--	�������߳�ID
//				IN const HWND& hWnd						--	�����ߴ�����
//				IN const UINT& dwNotifyMsgID			--	ɨ��������֪ͨ��������ʹ�õ���ϢID,ͨ�� PostMessage(hWnd��Ч)��   PostThreadMessage(dwThread��Ч),������߽���ͨѶ�ò�����ֵ�������0
//				IN const int& iProvinceID				--	ʡ�ݱ���
//				IN const int& iOverTime					--	ͨѶ��ʱ���ã���λ������
//
// ���������	BOOL	��ʼ���ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_InitDev(IN const UINT& dwThreadID, IN const HWND& hWnd, IN const UINT& dwNotifyMsgID, IN const int& iProvinceID, IN const int& iOverTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	nProvinceID = iProvinceID;

	if ( 0 == dwNotifyMsgID )
	{
		csErrorMsg.Format( _T("IF_InitDev:	��ʼ��ʧ�ܣ���ɨ����Ӧ��ϢΪ0��") );
		theApp.RecordMsgTemp( csErrorMsg );
		bInitDev = false;
		return FALSE;
	}

	if ( NULL == hWnd && 0 == dwThreadID )
	{
		csErrorMsg.Format( _T("IF_InitDev:	��ʼ��ʧ�ܣ���������Ϊ�ջ����߳�ID��Ч��") );
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
		csErrorMsg.Format( _T("IF_InitDev:	ɨ���������ļ�(%s)������!"), theApp.csCfgFile );
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
		csErrorMsg.Format( _T("IF_InitDev:	ʵ����ɨ���������߳��쳣!") );
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
// �������ƣ�IF_Destroy
//
// ��������������������Դ������˳�ǰ�������
//
// ���������	��
//
// ���������	BOOL	�˳��ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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

		csErrorMsg.Format( _T("IF_Destroy:	ɨ�����ײ�����߳����˳���") );
		return TRUE;
	}
	else
	{
		csErrorMsg.Format( _T("IF_Destroy:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}





////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetLastErrorDesc
//
// ������������ȡ��̬���ϴ����µĴ�����Ϣ
//
// ���������	��
//
// ���������	CString	���һ�εĴ�����Ϣ
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		csErrorMsg.Format( _T("IF_GetLastErrorDesc:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
	}
	return csErrorMsg;
}





////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_StartGetData
//
// ��������������Ӳ����ʼ��ȡ���ݵĲ�������Ӳ����ȡ�����ݣ�������߷�����Ϣ����ϢIDΪ��ʼ�������Ĳ���4 dwNotifyMsgID
//
// ���������	IN const int& iDelayTime		--	ɨ����ӳ�����ʱ��
//
// ���������	BOOL	��ʼɨ��ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_StartGetData(IN const int& iDelayTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			//��¼��־
			//	strLog = FmtStr(_T("������Ӧ,�������̱��%d,����ɨ��(%s)������ɨ������"),GetGlobalApp()->m_nTask,(1 == iStartType)?_T("�Զ�"):_T("�ֶ�"));
			::Sleep(100);

			//�����ڲ�������˵��ֻ��������ɨ���Լ���ʱʱ�䣻��ʼʱ�������ʱ��Ȳ���������������ⲿ��ȡ
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_START_SCAN, (WPARAM)hMainWnd, (LPARAM)(iDelayTime >= 0) ? iDelayTime : 0);

			csErrorMsg.Format( _T("IF_StartGetData:	��ʼɨ��ɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_StartGetData:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_StartGetData:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_StopGetData
//
// ��������������Ӳ��ֹͣ��ȡ���ݵĲ���
//
// ���������	��
//
// ���������	BOOL	ֹͣɨ��ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_StopGetData( IN const int& iChangeMMI )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		if (NULL != theApp.m_pThreadSMManager)
		{
			//��¼��־
			//	strLog = FmtStr(_T("������Ӧ,�������̱��%d,ǿ��ֹͣɨ�����������ֹͣɨ������"),GetGlobalApp()->m_nTask);
			::Sleep(100);

			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_STOP_SCAN, (WPARAM)hMainWnd, (LPARAM)iChangeMMI);
			csErrorMsg.Format( _T("IF_StopGetData:	ֹͣɨ��ɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_StopGetData:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_StopGetData:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_CheckDeviceStatus
//
// �������������Ӳ����״̬
//
// ���������IN OUT DWORD& dwState -- ״̬��������λ���б�ʶ����Ϊ0��1�У�����������λ����(1λ,���ڴ��쳣;2λ,����ͨѶ����-������)
//
// ���������	BOOL	���״̬�ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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

			csErrorMsg.Format( _T("IF_CheckDeviceStatus:	��ȡӲ��״̬�ɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_CheckDeviceStatus:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_CheckDeviceStatus:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
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
				csErrorMsg.Format( _T("IF_GetData:	��ȡɨ�����ݽ��[δ�ܼ�������Ч����]") );
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
// �������ƣ�IF_GetData
//
// �����������������߽��յ�Ӳ���������͵���Ϣ��֪ͨ�����Ѿ�׼���ã����øýӿڻ�ȡӲ���ش���Ϣ
//
// ���������	IN OUT CString& strData		--	�ش�����������
//				IN OUT int& iDataType		--	�ش�����������	1-QR֧���루�����֣�	����-δ����
//				IN OUT int& iSrcType		--	�ش����������ڹ���������	1-ɨ����	����-δ����
//
// ���������	BOOL	��ȡ��Ϣ�ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
					csErrorMsg.Format( _T("IF_GetData:	��ȡɨ�����ݽ��[δ�ܼ�������Ч����]") );
					theApp.RecordMsgTemp( csErrorMsg );
					return FALSE;
				}
			}
			else
			{
				csErrorMsg.Format( _T("IF_GetData: ��ȡɨ�����ݽ��[ɨ�����ݽ������쳣]") );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}

			csErrorMsg.Format( _T("IF_GetData:	��ȡɨ�����ݽ��[�ɹ�]") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_GetData:	��ȡɨ�����ݽ��[�����߳���Ч] ") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_GetData:	��ȡɨ�����ݽ��[���ƶ�̬��δ�ɹ���ʼ��] ") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_Authority
//
// ����������Ȩ����֤�������жϵ�������û��Ȩ��ʹ��
//
// ���������	IN const CString& strAuthKey	--	�����ߴ������֤��Ϣ�ַ���
//
// ���������	BOOL	��֤�ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		csErrorMsg.Format( _T("IF_Authority:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetDeviceFirewareVer
//
// ������������ȡӲ���̼��������,�ݲ�ʵ��
//
// ���������	IN OUT CString& strXML	--	Ӳ���̼���Ϣ����
//
// ���������	BOOL	��֤�ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
				//����ն����к�Ϊ�գ���ÿ��30���ȡһ��Ӳ���ն����
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
		csErrorMsg.Format( _T("IF_GetDeviceFirewareVer:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_GetDisplayContext
//
// ������������ȡ����Ӳ����ʾ���ֵ�����,�ݲ�ʵ��
//
// ���������	IN OUT int& iID			--	��ʾ�豸����ID
//				IN OUT CString& strXML	--	����������ʾ��������
//
// ���������	BOOL	��ȡ��ʾ���ݳɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_GetDisplayContext(IN OUT int& iID, IN OUT CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		csErrorMsg.Format( _T("IF_GetDisplayContext:	��ȡɨ������ʾ����ʧ��,�ݲ�֧�֣�") );
		return FALSE;

		if (NULL != theApp.m_pThreadSMManager)
		{
			csErrorMsg.Format( _T("IF_GetDisplayContext:	��ȡɨ������ʾ���ݳɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_GetDisplayContext:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_GetDisplayContext:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}







////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_SetDisplayContext
//
// ������������������Ӳ����ʾ���ֵ�����
//
// ���������	IN const int& iID			--	��ʾ�豸����ID
//				IN const CString& strXML	--	��ʾ��������
//
// ���������	BOOL	��������Ӳ����ʾ���ֳɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
					csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Version!"), csDisplay );
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
					csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����ProvinceID!"), csDisplay );
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
					csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����VerifyCode!"), csDisplay );
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
						else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Type!"), csDisplay );	} //	return FALSE;	}

						if ( iDisplayType < 100 ) //ԭ�н����վ�
						{
							sScanTemp.DealInfoUse = 1;
							sScanTemp.Type = iDisplayType;

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Money") ))		{	sScanTemp.Money = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Money!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Weight") ))	{	sScanTemp.Weight = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Weight!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("VehicleType") )) {	sScanTemp.VehicleType = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����VehicleType!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("VehicleClass") )) {	sScanTemp.VehicleClass = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����VehicleClass!"), csDisplay );	} //	return FALSE;	}

							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("CurrentTime") )) {	sScanTemp.tShowtime = theApp.CString2time_t(xmlLoad.GetData());	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����CurrentTime!"), csDisplay );	} //	return FALSE;	}

							//����
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("LimitWeight") )) {	sScanTemp.LimitWeight = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����LimitWeight!"), csDisplay );	} //	return FALSE;	}

							//����
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("AixsCount") )) {	sScanTemp.AxisCount = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����AxisCount!"), csDisplay );	} //	return FALSE;	}

							//���
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("Balance") )) {	sScanTemp.Balance = _tstoi( xmlLoad.GetData() );	}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Balance!"), csDisplay );	} //	return FALSE;	}

							//���վ��
							xmlLoad.ResetMainPos();
							if (xmlLoad.FindElem( _T("EntrySTName") ))
							{
								CString strTemp(xmlLoad.GetData());
								memcpy(&sScanTemp.EntrySTName[0], strTemp.GetBuffer(0), strTemp.GetLength() > _countof(sScanTemp.EntrySTName) ? _countof(sScanTemp.EntrySTName) - 1 : strTemp.GetLength());
								strTemp.ReleaseBuffer();
							}
							else {	csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����EntrySTName!"), csDisplay );	} //	return FALSE;	}
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
								csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Context!"), csDisplay );
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
						break;	//�汾1018�����󣬴˴����ٷ��ش���
					}
				}

				if ( 0 == iDisplayCount )
				{
					csErrorMsg.Format( _T("IF_SetDisplayContext�쳣��(%s)δ�ҵ���Ч��xml����Data!"), csDisplay );
					return FALSE;
				}
			}
			else
			{
				csErrorMsg.Format( _T("IF_SetDisplayContext:	������ʾʧ�ܣ�XML������Ч: %s ����"), csDisplay );
				return FALSE;
			}

			sScanTemp.iSerial = iSerialNum;	//���õ�ǰ���к�
			theApp.m_pThreadSMManager->SetDisplay( sScanTemp );
			theApp.m_pThreadSMManager->PostThreadMessage(WM_THREAD_SM_SET_DISPLAY, (WPARAM)iSerialNum, (LPARAM)0);

			iSerialNum += 1;//������Ϣ�����кż�1

			csErrorMsg.Format( _T("IF_SetDisplayContext:	��������Ӳ����ʾ�ɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_SetDisplayContext:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_SetDisplayContext:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_IssuedData
//
// ����������������Ӳ���·�����
//
// ���������	IN const CString& strXML	--	�·�����
//
// ���������	BOOL	������Ӳ���·����ݳɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_IssuedData(IN const CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		// csErrorMsg.Format( _T("IF_IssuedData:	������Ӳ���·�����ʧ�ܣ��ݲ�֧�֣�") );
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
				csErrorMsg.Format( _T("IF_IssuedData:������Ч(%s)��"), tmp_xml_str );
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
				csErrorMsg.Format( _T("IF_IssuedData:IssueData��Ч(%s)��"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			*/

			xml_parser.ResetMainPos();
			//xml_parser.IntoElem();
			if (!xml_parser.FindElem(_T("Data")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Data��Ч(%s)��"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			xml_parser.IntoElem();
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("Module")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Module��Ч(%s)��"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			trans_data.module = _tstoi( xml_parser.GetData() );
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("Type")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Type��Ч(%s)��"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			trans_data.type = _tstoi(xml_parser.GetData());
			xml_parser.ResetMainPos();
			if (!xml_parser.FindElem(_T("Context")))
			{
				csErrorMsg.Format( _T("IF_IssuedData:Context��Ч(%s)��"), tmp_xml_str );
				theApp.RecordMsgTemp( csErrorMsg );
				return FALSE;
			}
			CString str_context(xml_parser.GetData());
			if (trans_data.module != 1) /*support only NFC*/
			{
				return FALSE;
			}
			if (trans_data.type < 1 || trans_data.type > 5)//ǩ�������ѣ�������ֹͣ������
			{
				//return FALSE; 201804 ȥ������
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
			csErrorMsg.Format( _T("IF_IssuedData:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_IssuedData:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_SetParam
//
// ������������������Ӳ����ز���,�ݲ�ʵ��
//
// ���������	IN const CString& strXML	--	���ò�������
//
// ���������	BOOL	��������Ӳ����ز����ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI IF_SetParam(IN const CString& strXML)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( bInitDev )
	{
		csErrorMsg.Format( _T("IF_SetParam:	��������Ӳ����ز���ʧ�ܣ��ݲ�֧�֣�") );
		return FALSE;

		if (NULL != theApp.m_pThreadSMManager)
		{
			csErrorMsg.Format( _T("IF_SetParam:	��������Ӳ����ز����ɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_SetParam:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_SetParam:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�IF_SetIOState
//
// ������������������Ӳ��IO��״̬(����1.0�汾Ӳ��ʹ��)
//
// ���������	IN const int& iIOState	--	����IO�Ŀ���״̬;	0-ɨ��ɹ�;		1-��ʾɨ��;		2-ɨ��ʧ��;		3-Ϩ��ȫ��;
//
// ���������	BOOL	��������Ӳ��IO״̬�ɹ���ʧ��
// ��д��Ա��XGS
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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

			csErrorMsg.Format( _T("IF_SetIOState:	��������Ӳ��IO״̬�ɹ���") );
			return TRUE;
		}
		else
		{
			csErrorMsg.Format( _T("IF_SetIOState:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_SetIOState:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
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
		theApp.RecordMsgTemp( "IF_PlaySound:��������ʧ��" );
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
			csErrorMsg.Format( _T("IF_PlaySound:	ɨ���������߳���Ч��") );
			theApp.RecordMsgTemp( csErrorMsg );
			return FALSE;
		}
	}
	else
	{
		csErrorMsg.Format( _T("IF_PlaySound:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
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
		csErrorMsg.Format( _T("IF_RealDeviceFirmwareVer:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
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
		csErrorMsg.Format( _T("IF_RealSlaveDeviceFirmwareVer:	ɨ�������ƶ�̬��δ�ɹ���ʼ����") );
		theApp.RecordMsgTemp( csErrorMsg );
		return FALSE;
	}
}

bool WINAPI IF_TerminalSystemVersionStatus(int& status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!bInitDev)
	{
		theApp.RecordMsgTemp( "IF_TerminalSystemVersionStatus:	ɨ�������ƶ�̬��δ�ɹ���ʼ����" );
		return false;
	}
	if (theApp.m_pThreadSMManager == NULL)
	{
		theApp.RecordMsgTemp( "IF_TerminalSystemVersionStatus:	�����߳���Ч" );
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
		theApp.RecordMsgTemp( "IF_ExchangeTerminalSystem:	ɨ�������ƶ�̬��δ�ɹ���ʼ����" );
		return false;
	}
	if (theApp.m_pThreadSMManager == NULL)
	{
		theApp.RecordMsgTemp( "IF_ExchangeTerminalSystem:	�����߳���Ч" );
		return false;
	}
	return theApp.m_pThreadSMManager->ExcuteSystemSwitch(targetSystemVersion);
}