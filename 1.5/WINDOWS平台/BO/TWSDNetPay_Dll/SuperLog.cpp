/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：SuperLog.cpp
 * 摘    要：日志系统，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年1月28日
 */
#include "StdAfx.h"
#include "SuperLog.h"
#include "BaseDefine.h"

//静态变量初始化
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
// 函数名称：CSuperLog::Init
//     
// 功能描述：日志系统初始化
//     
// 输入参数：IN const CString& strLogDir
//         ：IN const CString& strFileName
//         ：IN const CString& m_strShareMemName
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::Init(IN const CString& strLogDir,IN const CString& strFileName,IN const CString& strShareMemName)
{
	BOOL bReturn = FALSE;

	// 初始化临界区变量
	InitializeCriticalSection(&m_csWriteLog);

	//获取运行模块绝对路径
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
	//构造日志信息文件夹
	CString strCurrentDir	= strConfigFilePath.Left(nPos + 1);
	strConfigFilePath		= strCurrentDir;
	strConfigFilePath		+= FILE_NAME_LOG_CONFIG;

	m_strPathConfigFile = strConfigFilePath;

	//设置共享内存内核对象名
	if(strShareMemName.IsEmpty())
	{
		m_strShareMemName.Format(_T("SuperLogShareMemData"));
	}
	else
	{
		m_strShareMemName = strShareMemName;
	}

	//设置日志文件目录
	if(strLogDir.IsEmpty())
	{
		m_strLogDir.Format(_T("%s"),strCurrentDir);
		//创建该目录
	}
	else
	{
		m_strLogDir = strLogDir;
	}

	//设置日志文件文件名
	if(strFileName.IsEmpty())
	{
		m_strFileName.Format(_T("Log%s.log"),CTime::GetCurrentTime().Format(_T("%Y%m%d")));
	}
	else
	{
		m_strFileName = strFileName;
	}

	if( ENUM_LOG_RUN == OpenLogFile() )																	// 打开日志文件
	{
		m_bInit = TRUE;
		// 启动信息
		WriteLog(_T("\r\n\r\n\r\n\r\n日志系统内部信息：日志系统启动"), ENUM_LOG_LEVEL_RUN, true);
	}
	if(   (OperateConfigInfo(FALSE))																	// 从配置文件中读取日志级别
//		&&(SetLogLevelShareMem(m_iLogLevel))															// 同步到共享内存中
		)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL,0,&LogProcStart,NULL,0,NULL);// 创建工作子线程
		bReturn = TRUE;
	}
	return m_bInit = bReturn;
}






void CSuperLog::Quit()
{
	//退出工作线程
	m_bRun = false;
//	::Sleep(100);
   
	if( INVALID_HANDLE_VALUE != m_hThread)
	{
		::WaitForSingleObject(m_hThread,1000);
	}
	m_enStatus = ENUM_LOG_EXITING;

    //清除临界变量资源
    DeleteCriticalSection(&m_csWriteLog);
   
	//清除内存共享资源
	if(m_hMapLogFile!=NULL) ::CloseHandle(m_hMapLogFile);

	//删除文件
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
// 函数名称：CSuperLog::ChangeFileName
//     
// 功能描述：日志文件更名，当需要更换一个新文件存储日志的时候，调用该接口
//     
// 输入参数：IN const CString& strFileName
//         ：IN const CString& strDir
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
// 函数名称：CSuperLog::WriteLog
//     
// 功能描述：公共接口，需要记录日志的时候进行调用
//     
// 输入参数：IN const CString &strLog
//         ：IN const enLogInfoLevel& enLevel/* = ENUM_LOG_LEVEL_RUN*/
//         ：IN const bool& bForce /*= false*/
// 输出参数：int
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::WriteLog(IN const CString &strLog,IN const int& iLevel/* = ENUM_LOG_LEVEL_RUN*/,IN const bool& bForce /*= false*/)
{
	BOOL bReturn = FALSE;
	if( !m_bInit )			//未初始化
	{
		return bReturn;
	}

	//判别日志等级是否在记录范围内
    if (iLevel >= m_iLogLevel)
    {
        //开始将日志更新到缓存
		EnterCriticalSection(&m_csWriteLog); 
   
		//获取日志内容前面的\r\n
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
		//检测日志内容是否为空
		if (!strRealLog.IsEmpty())
		{
			//增添必要的时间信息和错误醒目提示
			CString strRealContext;
			if (ENUM_LOG_LEVEL_ERROR == iLevel)
			{
			   strRealContext.Format(_T("%s%s:【产生错误】%s\r\n"),strPreLogSpace,GetCurTimeStr(),strRealLog);
			}
			else
			{
				strRealContext.Format(_T("%s%s:%s\r\n"),strPreLogSpace,GetCurTimeStr(),strRealLog);
			}
			
			m_strWriteStrInfo += strRealContext;
		}
		//判断是否需要将日志缓存持久化
		if (   ((bForce || (ENUM_LOG_LEVEL_ERROR == iLevel)) && !m_strWriteStrInfo.IsEmpty())	//强制马上写入硬盘(条件1：调用者设置强制写入/产生错误；条件2：当前日志内容非空)
			|| (m_strWriteStrInfo.GetLength() > m_dwSingleWriteDataSize)						//等待缓存满直接写入
			)
		{
			BOOL bWrate = TRUE;
			//获取当前文件名
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
// 函数名称：CSuperLog::WriteLogToFile
//     
// 功能描述：将缓存数据持久化到硬盘文件
//     
// 输入参数：无参数
// 输出参数：int
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSuperLog::WriteLogToFile()
{
	BOOL bReturn = FALSE;
	//打开一次文件
	m_enStatus = OpenLogFile();
	//判断是否继续操作
    if (   (m_pFile == NULL)					//文件句柄为空
        || (m_strWriteStrInfo.IsEmpty())		//日志信息为空
        || (m_enStatus == ENUM_LOG_INIT)		//文件处于初始化状态
        || (m_enStatus == ENUM_LOG_EXITED)		//文件已经关闭
        || (m_enStatus == ENUM_LOG_INVALID)		//文件非法
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
// 函数名称：CSuperLog::OpenLogFile
//     
// 功能描述：打开日志文件
//     
// 输入参数：void
// 输出参数：CSuperLog::enLogStatus
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CSuperLog::enLogStatus CSuperLog::OpenLogFile(void)
{
    //写入过程中出错后需要关闭文件
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
	//文件指针移动到末尾
    m_pFile->SeekToEnd();
    return m_enStatus = ENUM_LOG_RUN;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSuperLog::OperateConfigInfo
//     
// 功能描述：操作日志基本配置信息
//     
// 输入参数：IN const BOOL& bSave
// 输出参数：int
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSuperLog::OperateConfigInfo(IN const BOOL& bSave)
{
    if (bSave)	//保存设置	
    {
		//保存当前日志记录最小等级
        WritePrivateProfileString(
            _T("SuperLog"), 
            _T("LogLevel"), 
            m_aLevelDescList[m_iLogLevel],
            m_strPathConfigFile);

		//保存当前日志文件最大长度
        CString temp;
        temp.Format(_T("%d"), m_iMaxLogFileLen);
        WritePrivateProfileString(
            _T("SuperLog"), 
            _T("MaxLogFileLen"), 
            (LPCTSTR)temp,
            m_strPathConfigFile);
    } 
    else		//读取设置
    {
		//读取日志自动保存到硬盘的时间
		m_dwAutoSaveLogSpanTime = (DWORD)GetPrivateProfileInt(
            _T("SuperLog"), 
            _T("AutSaveSpanTime"), 
            10,
            m_strPathConfigFile);

        if (m_dwAutoSaveLogSpanTime > 600 || m_dwAutoSaveLogSpanTime < 5)
        {
			WriteLog(_T("日志系统内部信息：自动保存时间配置非法。"),LOG_LEVEL_ERROR);
            m_dwAutoSaveLogSpanTime = 10;
        }

		//读取日志自动保存到硬盘的单次写入量大小
		m_dwSingleWriteDataSize = (DWORD)GetPrivateProfileInt(
            _T("SuperLog"), 
            _T("AutSaveDataSize"), 
            32768,
            m_strPathConfigFile);

        if (m_dwSingleWriteDataSize > 102400 || m_dwSingleWriteDataSize < 64)
        {
			WriteLog(_T("日志系统内部信息：自动保存数据大小配置非法。"),LOG_LEVEL_ERROR);
            m_dwSingleWriteDataSize = 32768;
        }

#ifdef _DEBUG
		m_iLogLevel = GetPrivateProfileInt( _T("SuperLog"), _T("LogLevel"), ENUM_LOG_LEVEL_DEBUG, m_strPathConfigFile);
#else
		m_iLogLevel = GetPrivateProfileInt( _T("SuperLog"), _T("LogLevel"), ENUM_LOG_LEVEL_RUN, m_strPathConfigFile);
#endif


        if (m_iLogLevel > ENUM_LOG_LEVEL_ERROR || m_iLogLevel < ENUM_LOG_LEVEL_DEBUG)
        {
			WriteLog(_T("日志系统内部信息：保存级别配置非法。"),LOG_LEVEL_ERROR);
            m_iLogLevel = ENUM_LOG_LEVEL_RUN;
        }
		
		//读取日志文件最大长度
        m_iMaxLogFileLen  = GetPrivateProfileInt(
            _T("SuperLog"), 
            _T("MaxLogFileLen"), 
            MAX_LOG_FILE_SIZE,
            m_strPathConfigFile);

        if (m_iMaxLogFileLen > MAX_LOG_FILE_SIZE)
        {
			WriteLog(_T("日志系统内部信息：文件最大长度配置非法。"),LOG_LEVEL_ERROR);
            m_iMaxLogFileLen = MAX_LOG_FILE_SIZE;
        }
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSuperLog::GetLogFilePath
//     
// 功能描述：获取日志文件绝对路径
//     
// 输入参数：void
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
// 函数名称：CSuperLog::GetCurTimeStr
//     
// 功能描述：获取时间字符串(带毫秒)
//     
// 输入参数：无参数
// 输出参数：CString&
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
// 函数名称：CSuperLog::LogProcStart
//     
// 功能描述：日志工作子线程，负责将日志缓存信息持久化
//     
// 输入参数：LPVOID lpParam
// 输出参数：UINT
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
UINT  WINAPI CSuperLog::LogProcStart(LPVOID lpParam)
{
    INT64 llCount = 1;
    CString strTemp;
    WriteLog(_T("日志系统内部信息：工作线程启动."), ENUM_LOG_LEVEL_DEBUG, true);

	DWORD dwSpanTime = DWORD(m_dwAutoSaveLogSpanTime*10);
    // 线程开始
    do 
    {
        Sleep(100);
        if ( 0 == (++llCount % dwSpanTime) )
        {
			// 每隔固定时间写一次日志(默认10秒)
            WriteLog(strTemp,ENUM_LOG_LEVEL_ERROR,true);
			// 检查日志登记级别
            //CheckLogLevel();
        }
    } while (m_bRun);

    // 线程结束
    WriteLog(_T("日志系统内部信息：工作线程结束."), ENUM_LOG_LEVEL_RUN, true);

    m_enStatus = ENUM_LOG_EXITED;

    _endthreadex(0);

    return 0;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSuperLog::CheckLogLevel
//     
// 功能描述：检测日志信息记录的最小等级
//     
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
// 函数名称：CSuperLog::GetLogLevelShareMem
//     
// 功能描述：从共享内存当中获取当前记录日志的最小等级
//     
// 输入参数：void
// 输出参数：int -- 日志登记级别
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
int CSuperLog::GetLogLevelShareMem(void)
{
    //打开共享的文件对象。
    if (NULL == m_hMapLogFile)
    {
        m_hMapLogFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,m_strShareMemName);
    }

    if (NULL != m_hMapLogFile)
    {
        //显示共享的文件数据。
        if (m_psMapAddr == NULL)
        {
            m_psMapAddr = (LPTSTR)MapViewOfFile(m_hMapLogFile, FILE_MAP_ALL_ACCESS,0,0,0);
            WriteLog(m_psMapAddr, ENUM_LOG_LEVEL_DEBUG);
        }
    }
    else
    {
        //创建共享文件。
        m_hMapLogFile = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,m_dwShareMemSize,m_strShareMemName);
        if (NULL != m_hMapLogFile)
        {
            //拷贝数据到共享文件里。
            m_psMapAddr = (LPTSTR)MapViewOfFile(m_hMapLogFile,FILE_MAP_ALL_ACCESS, 0,0,0);
            if (m_psMapAddr != NULL)
            {
                _tcscpy_s(m_psMapAddr, m_dwShareMemSize, m_aLevelDescList[m_iLogLevel]);                  
                FlushViewOfFile(m_psMapAddr, _tcslen(m_aLevelDescList[m_iLogLevel]));
                WriteLog(_T("日志系统内部信息：设置默认日志级别到共享内存中成功。"), ENUM_LOG_LEVEL_RUN);

           }
        }
        else
        {
            WriteLog(_T("日志系统内部信息：创建共享内存失败。"), ENUM_LOG_LEVEL_ERROR);
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

		//版本1056调整内存映射处理
		::UnmapViewOfFile(m_psMapAddr);
		m_psMapAddr = NULL;
    }

    return m_iLogLevel;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSuperLog::SetLogLevelShareMem
//     
// 功能描述：设置当前的日志登记级别到共享内存当中
//     
// 输入参数：IN const int& iLevel
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年1月28日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
		WriteLog(_T("日志系统内部信息：设置日志级别成功。"), ENUM_LOG_LEVEL_RUN);
		bReturn = TRUE;
    }
    return bReturn;
}