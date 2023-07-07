/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�SuperLog.cpp
 * ժ    Ҫ����־ϵͳ��ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��1��28��
 */
#include "StdAfx.h"
#include "SuperLog.h"
#include "BaseDefine.h"

//��̬������ʼ��
HANDLE			       CSuperLog::m_hThread					= INVALID_HANDLE_VALUE;
CSuperLog::enLogStatus CSuperLog::m_enStatus				= CSuperLog::ENUM_LOG_INIT;
BOOL                   CSuperLog::m_bInit					= FALSE;
bool                   CSuperLog::m_bRun					= true;
TCHAR*  			   CSuperLog::m_aLevelDescList[]		= {_T("0"), _T("1"), _T("2"), _T("3")};
CRITICAL_SECTION       CSuperLog::m_csWriteLog;
CString				   CSuperLog::m_strWriteStrInfo			= _T("");
DWORD				   CSuperLog::m_dwSingleWriteDataSize	= 0;
DWORD				   CSuperLog::m_dwAutoSaveLogSpanTime   = 10;
#ifdef _DEBUG
	int       CSuperLog::m_iLogLevel  = CSuperLog::ENUM_LOG_LEVEL_DEBUG;
#else
	int       CSuperLog::m_iLogLevel  = CSuperLog::ENUM_LOG_LEVEL_RUN;
#endif

HANDLE	CSuperLog::m_hMapLogFile			= NULL;
LPTSTR	CSuperLog::m_psMapAddr				= _T("");
CString	CSuperLog::m_strShareMemName		= _T("");
DWORD	CSuperLog::m_dwShareMemSize			= 1024;

CStdioFile*	CSuperLog::m_pFile				= NULL;
int	CSuperLog::m_iMaxLogFileLen				= 0;
CString		CSuperLog::m_strFileName		= _T("");
CString		CSuperLog::m_strLogDir			= _T("");
CString		CSuperLog::m_strPathConfigFile	= _T("");

CSuperLog::CSuperLog(void)
{
	 
}

CSuperLog::~CSuperLog(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::Init
//     
// ������������־ϵͳ��ʼ��
//     
// ���������IN const CString& strLogDir
//         ��IN const CString& strFileName
//         ��IN const CString& m_strShareMemName
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::Init(IN const CString& strLogDir,IN const CString& strFileName,IN const CString& strShareMemName)
{
	BOOL bReturn = FALSE;

	// ��ʼ���ٽ�������
	InitializeCriticalSection(&m_csWriteLog);

	//��ȡ����ģ�����·��
	TCHAR szPath[MAX_PATH] = {0};
	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	{
		return bReturn;
	}

	CString strConfigFilePath(szPath);
	int nPos = strConfigFilePath.ReverseFind(_T('\\'));
	if (nPos == -1)
	{
		return bReturn;
	}
	//������־��Ϣ�ļ���
	CString strCurrentDir	= strConfigFilePath.Left(nPos + 1);
	strConfigFilePath		= strCurrentDir;
	strConfigFilePath		+= FILE_NAME_LOG_CONFIG;

	m_strPathConfigFile = strConfigFilePath;

	//���ù����ڴ��ں˶�����
	if(strShareMemName.IsEmpty())
	{
		m_strShareMemName.Format(_T("SuperLogShareMemData"));
	}
	else
	{
		m_strShareMemName = strShareMemName;
	}

	//������־�ļ�Ŀ¼
	if(strLogDir.IsEmpty())
	{
		m_strLogDir.Format(_T("%s"),strCurrentDir);
		//������Ŀ¼
	}
	else
	{
		m_strLogDir = strLogDir;
	}

	//������־�ļ��ļ���
	if(strFileName.IsEmpty())
	{
		m_strFileName.Format(_T("Log%s.log"),CTime::GetCurrentTime().Format(_T("%Y%m%d")));
	}
	else
	{
		m_strFileName = strFileName;
	}

	if( ENUM_LOG_RUN == OpenLogFile() )																	// ����־�ļ�
	{
		m_bInit = TRUE;
		// ������Ϣ
		WriteLog(_T("\r\n\r\n\r\n\r\n��־ϵͳ�ڲ���Ϣ����־ϵͳ����"), ENUM_LOG_LEVEL_RUN, true);
	}
	if(   (OperateConfigInfo(FALSE))																	// �������ļ��ж�ȡ��־����
//		&&(SetLogLevelShareMem(m_iLogLevel))															// ͬ���������ڴ���
		)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL,0,&LogProcStart,NULL,0,NULL);// �����������߳�
		bReturn = TRUE;
	}
	return m_bInit = bReturn;
}






void CSuperLog::Quit()
{
	//�˳������߳�
	m_bRun = false;
//	::Sleep(100);
   
	if( INVALID_HANDLE_VALUE != m_hThread)
	{
		::WaitForSingleObject(m_hThread,1000);
	}
	m_enStatus = ENUM_LOG_EXITING;

    //����ٽ������Դ
    DeleteCriticalSection(&m_csWriteLog);
   
	//����ڴ湲����Դ
	if(m_hMapLogFile!=NULL) ::CloseHandle(m_hMapLogFile);

	//ɾ���ļ�
	if(NULL != m_pFile)
	{
		if( CFile::hFileNull != m_pFile->m_hFile )
		{
			m_pFile->Close();
		}
		delete m_pFile;
		m_pFile = NULL;
	}
}











////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::ChangeFileName
//     
// ������������־�ļ�����������Ҫ����һ�����ļ��洢��־��ʱ�򣬵��øýӿ�
//     
// ���������IN const CString& strFileName
//         ��IN const CString& strDir
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CSuperLog::ChangeFileName(IN const CString& strFileName,IN const CString& strDir)
{
	if(!strFileName.IsEmpty())
	{
		m_strFileName = strFileName;
	}

	if(!strDir.IsEmpty())
	{
		m_strLogDir = strDir;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::WriteLog
//     
// ���������������ӿڣ���Ҫ��¼��־��ʱ����е���
//     
// ���������IN const CString &strLog
//         ��IN const enLogInfoLevel& enLevel/* = ENUM_LOG_LEVEL_RUN*/
//         ��IN const bool& bForce /*= false*/
// ���������int
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::WriteLog(IN const CString &strLog,IN const int& iLevel/* = ENUM_LOG_LEVEL_RUN*/,IN const bool& bForce /*= false*/)
{
	BOOL bReturn = FALSE;
	if( !m_bInit )			//δ��ʼ��
	{
		return bReturn;
	}

	//�б���־�ȼ��Ƿ��ڼ�¼��Χ��
    if (iLevel >= m_iLogLevel)
    {
        //��ʼ����־���µ�����
		EnterCriticalSection(&m_csWriteLog); 
   
		//��ȡ��־����ǰ���\r\n
		CString strPreLogSpace(_T(""));
		if(strLog.GetLength()>2)
		{
			for(int i = 0;i<strLog.GetLength()-1;++i)
			{
				if(   ('\r' == strLog.GetAt(i))
					&&('\n' == strLog.GetAt(i+1))
					)
				{
					strPreLogSpace += _T("\r\n");
				}
			}
		}
		CString strRealLog(strLog);
		strRealLog.TrimLeft();
		//�����־�����Ƿ�Ϊ��
		if (!strRealLog.IsEmpty())
		{
			//�����Ҫ��ʱ����Ϣ�ʹ�����Ŀ��ʾ
			CString strRealContext;
			if (ENUM_LOG_LEVEL_ERROR == iLevel)
			{
			   strRealContext.Format(_T("%s%s:����������%s\r\n"),strPreLogSpace,GetCurTimeStr(),strRealLog);
			}
			else
			{
				strRealContext.Format(_T("%s%s:%s\r\n"),strPreLogSpace,GetCurTimeStr(),strRealLog);
			}
			
			m_strWriteStrInfo += strRealContext;
		}
		//�ж��Ƿ���Ҫ����־����־û�
		if (   ((bForce || (ENUM_LOG_LEVEL_ERROR == iLevel)) && !m_strWriteStrInfo.IsEmpty())	//ǿ������д��Ӳ��(����1������������ǿ��д��/������������2����ǰ��־���ݷǿ�)
			|| (m_strWriteStrInfo.GetLength() > m_dwSingleWriteDataSize)						//�ȴ�������ֱ��д��
			)
		{
			BOOL bWrate = TRUE;
			//��ȡ��ǰ�ļ���
			CString strCurrentFileName;
			strCurrentFileName.Format(_T("Log%s.log"),CTime::GetCurrentTime().Format(_T("%Y%m%d")));

			if(strCurrentFileName != m_strFileName)
			{
				if ( NULL != m_pFile )
				{
					m_pFile->Close();
					delete m_pFile;
					m_pFile = NULL;
				}

				m_strFileName = strCurrentFileName;

				if(ENUM_LOG_RUN != OpenLogFile())
					bWrate = FALSE;
			}
			if(bWrate)
			{
				WriteLogToFile();
			}
		}

		LeaveCriticalSection(&m_csWriteLog);

		bReturn = TRUE;
    }
    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::WriteLogToFile
//     
// �������������������ݳ־û���Ӳ���ļ�
//     
// ����������޲���
// ���������int
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSuperLog::WriteLogToFile()
{
	BOOL bReturn = FALSE;
	//��һ���ļ�
	m_enStatus = OpenLogFile();
	//�ж��Ƿ��������
    if (   (m_pFile == NULL)					//�ļ����Ϊ��
        || (m_strWriteStrInfo.IsEmpty())		//��־��ϢΪ��
        || (m_enStatus == ENUM_LOG_INIT)		//�ļ����ڳ�ʼ��״̬
        || (m_enStatus == ENUM_LOG_EXITED)		//�ļ��Ѿ��ر�
        || (m_enStatus == ENUM_LOG_INVALID)		//�ļ��Ƿ�
        )
    {
        return bReturn;
    }

    try
    {
        m_pFile->WriteString(m_strWriteStrInfo); 
        m_pFile->Flush();
		bReturn = TRUE;
    }
    catch (...)
    {
        m_enStatus = ENUM_LOG_INVALID;
    }

    m_strWriteStrInfo.Empty();

    return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::OpenLogFile
//     
// ��������������־�ļ�
//     
// ���������void
// ���������CSuperLog::enLogStatus
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CSuperLog::enLogStatus CSuperLog::OpenLogFile(void)
{
    //д������г������Ҫ�ر��ļ�
    if (ENUM_LOG_INVALID == m_enStatus)
    {
        if ( NULL != m_pFile )
        {
            m_pFile->Close();
            delete m_pFile;
            m_pFile = NULL;
        }
    }

	if( NULL == m_pFile )
    {
        m_pFile = new CStdioFile;
        if (NULL == m_pFile)
        {
            return m_enStatus = ENUM_LOG_INVALID;
        }

		if(!m_pFile->Open(GetLogFilePath(),CFile::modeCreate|CFile::modeReadWrite|CFile::typeText|CFile::shareDenyNone|CFile::modeNoTruncate))
        {
            delete m_pFile;
            m_pFile = NULL;
            return m_enStatus = ENUM_LOG_INVALID;
        }
    }
	//�ļ�ָ���ƶ���ĩβ
    m_pFile->SeekToEnd();
    return m_enStatus = ENUM_LOG_RUN;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::OperateConfigInfo
//     
// ����������������־����������Ϣ
//     
// ���������IN const BOOL& bSave
// ���������int
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::OperateConfigInfo(IN const BOOL& bSave)
{
    if (bSave)	//��������	
    {
		//���浱ǰ��־��¼��С�ȼ�
        WritePrivateProfileString(
            _T("SuperLog"), 
            _T("LogLevel"), 
            m_aLevelDescList[m_iLogLevel],
            m_strPathConfigFile);

		//���浱ǰ��־�ļ���󳤶�
        CString temp;
        temp.Format(_T("%d"), m_iMaxLogFileLen);
        WritePrivateProfileString(
            _T("SuperLog"), 
            _T("MaxLogFileLen"), 
            (LPCTSTR)temp,
            m_strPathConfigFile);
    } 
    else		//��ȡ����
    {
		//��ȡ��־�Զ����浽Ӳ�̵�ʱ��
		m_dwAutoSaveLogSpanTime = (DWORD)GetPrivateProfileInt(
            _T("SuperLog"), 
            _T("AutSaveSpanTime"), 
            10,
            m_strPathConfigFile);

        if (m_dwAutoSaveLogSpanTime > 600 || m_dwAutoSaveLogSpanTime < 5)
        {
			WriteLog(_T("��־ϵͳ�ڲ���Ϣ���Զ�����ʱ�����÷Ƿ���"),LOG_LEVEL_ERROR);
            m_dwAutoSaveLogSpanTime = 10;
        }

		//��ȡ��־�Զ����浽Ӳ�̵ĵ���д������С
		m_dwSingleWriteDataSize = (DWORD)GetPrivateProfileInt(
            _T("SuperLog"), 
            _T("AutSaveDataSize"), 
            32768,
            m_strPathConfigFile);

        if (m_dwSingleWriteDataSize > 102400 || m_dwSingleWriteDataSize < 64)
        {
			WriteLog(_T("��־ϵͳ�ڲ���Ϣ���Զ��������ݴ�С���÷Ƿ���"),LOG_LEVEL_ERROR);
            m_dwSingleWriteDataSize = 32768;
        }

#ifdef _DEBUG
		m_iLogLevel = GetPrivateProfileInt( _T("SuperLog"), _T("LogLevel"), ENUM_LOG_LEVEL_DEBUG, m_strPathConfigFile);
#else
		m_iLogLevel = GetPrivateProfileInt( _T("SuperLog"), _T("LogLevel"), ENUM_LOG_LEVEL_RUN, m_strPathConfigFile);
#endif


        if (m_iLogLevel > ENUM_LOG_LEVEL_ERROR || m_iLogLevel < ENUM_LOG_LEVEL_DEBUG)
        {
			WriteLog(_T("��־ϵͳ�ڲ���Ϣ�����漶�����÷Ƿ���"),LOG_LEVEL_ERROR);
            m_iLogLevel = ENUM_LOG_LEVEL_RUN;
        }
		
		//��ȡ��־�ļ���󳤶�
        m_iMaxLogFileLen  = GetPrivateProfileInt(
            _T("SuperLog"), 
            _T("MaxLogFileLen"), 
            MAX_LOG_FILE_SIZE,
            m_strPathConfigFile);

        if (m_iMaxLogFileLen > MAX_LOG_FILE_SIZE)
        {
			WriteLog(_T("��־ϵͳ�ڲ���Ϣ���ļ���󳤶����÷Ƿ���"),LOG_LEVEL_ERROR);
            m_iMaxLogFileLen = MAX_LOG_FILE_SIZE;
        }
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::GetLogFilePath
//     
// ������������ȡ��־�ļ�����·��
//     
// ���������void
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CSuperLog::GetLogFilePath(void)
{
	CString strFilePath;
	if(m_strLogDir.GetLength()>0)
	{
		if('\\' == m_strLogDir.GetAt(m_strLogDir.GetLength() - 1))
		{
			strFilePath.Format(_T("%s%s"),m_strLogDir,m_strFileName);
		}
		else
		{
			strFilePath.Format(_T("%s\\%s"),m_strLogDir,m_strFileName);
		}
	}
	return strFilePath;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::GetCurTimeStr
//     
// ������������ȡʱ���ַ���(������)
//     
// ����������޲���
// ���������CString&
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSuperLog::GetCurTimeStr(void)
{
	SYSTEMTIME sysTime={0};
	::GetLocalTime(&sysTime);
    CString strTime;
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds);
    return strTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::LogProcStart
//     
// ������������־�������̣߳�������־������Ϣ�־û�
//     
// ���������LPVOID lpParam
// ���������UINT
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
UINT  WINAPI CSuperLog::LogProcStart(LPVOID lpParam)
{
    INT64 llCount = 1;
    CString strTemp;
    WriteLog(_T("��־ϵͳ�ڲ���Ϣ�������߳�����."), ENUM_LOG_LEVEL_DEBUG, true);

	DWORD dwSpanTime = DWORD(m_dwAutoSaveLogSpanTime*10);
    // �߳̿�ʼ
    do 
    {
        Sleep(100);
        if ( 0 == (++llCount % dwSpanTime) )
        {
			// ÿ���̶�ʱ��дһ����־(Ĭ��10��)
            WriteLog(strTemp,ENUM_LOG_LEVEL_ERROR,true);
			// �����־�ǼǼ���
            //CheckLogLevel();
        }
    } while (m_bRun);

    // �߳̽���
    WriteLog(_T("��־ϵͳ�ڲ���Ϣ�������߳̽���."), ENUM_LOG_LEVEL_RUN, true);

    m_enStatus = ENUM_LOG_EXITED;

    _endthreadex(0);

    return 0;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::CheckLogLevel
//     
// ���������������־��Ϣ��¼����С�ȼ�
//     
// ���������void
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CSuperLog::CheckLogLevel(void)
{
    int iLevel = GetLogLevelShareMem();
    if (iLevel != m_iLogLevel)
    {
    //    SetLogLevelShareMem(iLevel);
        m_iLogLevel = iLevel;
        OperateConfigInfo(TRUE);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::GetLogLevelShareMem
//     
// �����������ӹ����ڴ浱�л�ȡ��ǰ��¼��־����С�ȼ�
//     
// ���������void
// ���������int -- ��־�ǼǼ���
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CSuperLog::GetLogLevelShareMem(void)
{
    //�򿪹�����ļ�����
    if (NULL == m_hMapLogFile)
    {
        m_hMapLogFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,m_strShareMemName);
    }

    if (NULL != m_hMapLogFile)
    {
        //��ʾ������ļ����ݡ�
        if (m_psMapAddr == NULL)
        {
            m_psMapAddr = (LPTSTR)MapViewOfFile(m_hMapLogFile, FILE_MAP_ALL_ACCESS,0,0,0);
            WriteLog(m_psMapAddr, ENUM_LOG_LEVEL_DEBUG);
        }
    }
    else
    {
        //���������ļ���
        m_hMapLogFile = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,m_dwShareMemSize,m_strShareMemName);
        if (NULL != m_hMapLogFile)
        {
            //�������ݵ������ļ��
            m_psMapAddr = (LPTSTR)MapViewOfFile(m_hMapLogFile,FILE_MAP_ALL_ACCESS, 0,0,0);
            if (m_psMapAddr != NULL)
            {
                _tcscpy_s(m_psMapAddr, m_dwShareMemSize, m_aLevelDescList[m_iLogLevel]);                  
                FlushViewOfFile(m_psMapAddr, _tcslen(m_aLevelDescList[m_iLogLevel]));
                WriteLog(_T("��־ϵͳ�ڲ���Ϣ������Ĭ����־���𵽹����ڴ��гɹ���"), ENUM_LOG_LEVEL_RUN);

           }
        }
        else
        {
            WriteLog(_T("��־ϵͳ�ڲ���Ϣ�����������ڴ�ʧ�ܡ�"), ENUM_LOG_LEVEL_ERROR);
        }
    }

    if (m_psMapAddr != NULL)
    {
        if (_tcscmp(m_psMapAddr, m_aLevelDescList[ENUM_LOG_LEVEL_RUN]) == 0)
        {
            return ENUM_LOG_LEVEL_RUN;
        }
        else if (_tcscmp(m_psMapAddr, m_aLevelDescList[ENUM_LOG_LEVEL_DEBUG]) == 0)
        {
            return ENUM_LOG_LEVEL_DEBUG;
        }
        else if (_tcscmp(m_psMapAddr, m_aLevelDescList[ENUM_LOG_LEVEL_ERROR]) == 0)
        {
            return ENUM_LOG_LEVEL_ERROR;
        }
        else
        {
            return ENUM_LOG_LEVEL_RUN;
        }

		//�汾1056�����ڴ�ӳ�䴦��
		::UnmapViewOfFile(m_psMapAddr);
		m_psMapAddr = NULL;
    }

    return m_iLogLevel;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSuperLog::SetLogLevelShareMem
//     
// �������������õ�ǰ����־�ǼǼ��𵽹����ڴ浱��
//     
// ���������IN const int& iLevel
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::SetLogLevelShareMem(IN const int& iLevel)
{
	BOOL bReturn = FALSE;
    if (    (m_hMapLogFile == NULL)
		 || (m_psMapAddr == NULL)
		 )
    {
        GetLogLevelShareMem();
    }

    if (   (m_hMapLogFile != NULL)
		&& (m_psMapAddr != NULL)
		)
    {
        _tcscpy_s(m_psMapAddr,m_dwShareMemSize,m_aLevelDescList[iLevel]);                  
        FlushViewOfFile(m_psMapAddr, _tcslen(m_aLevelDescList[iLevel]));
		WriteLog(_T("��־ϵͳ�ڲ���Ϣ��������־����ɹ���"), ENUM_LOG_LEVEL_RUN);
		bReturn = TRUE;
    }
    return bReturn;
}