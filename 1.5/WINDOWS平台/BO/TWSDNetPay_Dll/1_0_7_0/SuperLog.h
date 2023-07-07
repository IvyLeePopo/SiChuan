/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：SuperLog.h
 * 摘    要：日志系统，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年1月28日
 */
#pragma once
#include <process.h>

//方便调用，直接以宏定义方式函数
#define   WRITE_LOG           CSuperLog::WriteLog
#define	  INIT_LOG			  CSuperLog::Init
#define	  QUIT_LOG			  CSuperLog::Quit
#define	  CREATE_NEW_LOG_FILE CSuperLog::ChangeFileName

#define   LOG_LEVEL_DEBUG     CSuperLog::ENUM_LOG_LEVEL_DEBUG
#define   LOG_LEVEL_RUN       CSuperLog::ENUM_LOG_LEVEL_RUN
#define   LOG_LEVEL_ERROR     CSuperLog::ENUM_LOG_LEVEL_ERROR

#define	  MAX_LOG_FILE_SIZE	  10*1024*1024

class CSuperLog
{
public:
	CSuperLog(void);
	~CSuperLog(void);
//内部结构声明
public:
// 日志级别
    enum enLogInfoLevel
    {
        ENUM_LOG_LEVEL_DEBUG = 0, 
        ENUM_LOG_LEVEL_RUN, 
        ENUM_LOG_LEVEL_ERROR, 
    };   
private:
    // 日志写入器的状态
    enum enLogStatus
    {
        ENUM_LOG_INIT = 0,	//文件处于初始化状态
        ENUM_LOG_RUN,		//文件处于运行状态
        ENUM_LOG_EXITING,	//文件处于退出过程状态
        ENUM_LOG_EXITED,	//文件已经关闭
        ENUM_LOG_INVALID,	//文件非法
    };

//外部接口声明
public:
	// 日志系统初始化
	static BOOL Init(IN const CString& strLogDir,IN const CString& strFileName,IN const CString& m_strShareMemName=_T("SuperLogShareMemData"));
	static void Quit();

	// 保存日志到新的文件
	static void ChangeFileName(IN const CString& strFileName,IN const CString& strDir=_T(""));
    // 重载不同类型消息的写日志函数
#ifdef _DEBUG
    static BOOL WriteLog(IN const CString &strLog,IN const int& iLevel = ENUM_LOG_LEVEL_DEBUG,IN const bool& bForce = false);
#else
    static BOOL WriteLog(IN const CString &strLog,IN const int& iLevel = ENUM_LOG_LEVEL_RUN,IN const bool& bForce = false);
#endif
//内部辅助函数
private:
	// 工作线程实现函数
	static UINT  WINAPI LogProcStart(LPVOID lpParam);
	// 获取当前的时间值字符串（带毫秒）
	static const CString GetCurTimeStr(void);	
	// 从共享内存中读配置并设置当前的日志级别
	static void CheckLogLevel(void);
	// 取得当前的日志级别
	static BOOL GetLogLevelShareMem(void);				
	// 设置当前的日志级别
    static BOOL SetLogLevelShareMem(IN const int& iLevel);			
	// 完成日志信息持久化到硬盘文件的操作
    static const BOOL	WriteLogToFile(void);
	// 试图打开日志文件
	static enLogStatus  OpenLogFile(void);
	// 把当前级别写到配置文件中
    static BOOL OperateConfigInfo(IN const BOOL& bSave);
	// 获取日志文件绝对路径
	static CString GetLogFilePath(void);
    
//属性
private:
	//日志系统是否已经初始化
	static BOOL m_bInit;
	// 当前log写入器是否结束,静态变量
	static bool m_bRun;	
	// 当前log写入器的状态,静态变量
	static enLogStatus m_enStatus;		
	// 日志级别字符型描述
	static TCHAR* m_aLevelDescList[];
	// 日志记录的级别
	static int	m_iLogLevel;
	// 写日志的临界变量
	static CRITICAL_SECTION m_csWriteLog;	
	// 日志信息缓存
	static CString	m_strWriteStrInfo;			
	// 日志单次更新缓存的数据长度
	static DWORD	m_dwSingleWriteDataSize;
	// 间隔多少时间自动将日志缓存持久化(单位：秒),默认10秒,最大值 600，最小值 5
	static DWORD	m_dwAutoSaveLogSpanTime;

	static HANDLE	m_hMapLogFile;					// 共享内存句柄
    static LPTSTR	m_psMapAddr;					// 共享内存地址
	static CString	m_strShareMemName;				// 共享内存内核名称
	static DWORD	m_dwShareMemSize;				// 共享内存数据区大小

	static CStdioFile*  m_pFile;					// 日志文件指针
	static int          m_iMaxLogFileLen;			// 日志文件长度
	static CString		m_strFileName;				// 日志文件名
 	static CString		m_strLogDir;				// 日志存储目录绝对路径
	static CString		m_strPathConfigFile;		// 日志配置文件绝对路径

	static HANDLE m_hThread;						// 工作子线程句柄
};
