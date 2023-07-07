/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�SuperLog.h
 * ժ    Ҫ����־ϵͳ�������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��1��28��
 */
#pragma once
#include <process.h>

//������ã�ֱ���Ժ궨�巽ʽ����
#define   WRITE_LOG           CSuperLog::WriteLog
#define	  INIT_LOG			  CSuperLog::Init
#define	  CREATE_NEW_LOG_FILE CSuperLog::ChangeFileName
#define	  QUIT_LOG			  CSuperLog::Quit



#define   LOG_LEVEL_DEBUG     CSuperLog::ENUM_LOG_LEVEL_DEBUG
#define   LOG_LEVEL_RUN       CSuperLog::ENUM_LOG_LEVEL_RUN
#define   LOG_LEVEL_ERROR     CSuperLog::ENUM_LOG_LEVEL_ERROR

#define	  MAX_LOG_FILE_SIZE	  10*1024*1024

class CSuperLog
{
public:
	CSuperLog(void);
	~CSuperLog(void);
//�ڲ��ṹ����
public:
// ��־����
    enum enLogInfoLevel
    {
        ENUM_LOG_LEVEL_DEBUG = 0, 
        ENUM_LOG_LEVEL_RUN, 
        ENUM_LOG_LEVEL_ERROR, 
    };   
private:
    // ��־д������״̬
    enum enLogStatus
    {
        ENUM_LOG_INIT = 0,	//�ļ����ڳ�ʼ��״̬
        ENUM_LOG_RUN,		//�ļ���������״̬
        ENUM_LOG_EXITING,	//�ļ������˳�����״̬
        ENUM_LOG_EXITED,	//�ļ��Ѿ��ر�
        ENUM_LOG_INVALID,	//�ļ��Ƿ�
    };

//�ⲿ�ӿ�����
public:
	// ��־ϵͳ��ʼ��
	static BOOL Init(IN const CString& strLogDir,IN const CString& strFileName,IN const CString& m_strShareMemName=_T("SuperLogShareMemData"));
	static void Quit();

	// ������־���µ��ļ�
	static void ChangeFileName(IN const CString& strFileName,IN const CString& strDir=_T(""));
    // ���ز�ͬ������Ϣ��д��־����
#ifdef _DEBUG
    static BOOL WriteLog(IN const CString &strLog,IN const int& iLevel = ENUM_LOG_LEVEL_DEBUG,IN const bool& bForce = false);
#else
    static BOOL WriteLog(IN const CString &strLog,IN const int& iLevel = ENUM_LOG_LEVEL_RUN,IN const bool& bForce = false);
#endif
//�ڲ���������
private:
	// �����߳�ʵ�ֺ���
	static UINT  WINAPI LogProcStart(LPVOID lpParam);
	// ��ȡ��ǰ��ʱ��ֵ�ַ����������룩
	static const CString GetCurTimeStr(void);	
	// �ӹ����ڴ��ж����ò����õ�ǰ����־����
	static void CheckLogLevel(void);
	// ȡ�õ�ǰ����־����
	static BOOL GetLogLevelShareMem(void);				
	// ���õ�ǰ����־����
    static BOOL SetLogLevelShareMem(IN const int& iLevel);			
	// �����־��Ϣ�־û���Ӳ���ļ��Ĳ���
    static const BOOL	WriteLogToFile(void);
	// ��ͼ����־�ļ�
	static enLogStatus  OpenLogFile(void);
	// �ѵ�ǰ����д�������ļ���
    static BOOL OperateConfigInfo(IN const BOOL& bSave);
	// ��ȡ��־�ļ�����·��
	static CString GetLogFilePath(void);
    
//����
private:
	//��־ϵͳ�Ƿ��Ѿ���ʼ��
	static BOOL m_bInit;
	// ��ǰlogд�����Ƿ����,��̬����
	static bool m_bRun;	
	// ��ǰlogд������״̬,��̬����
	static enLogStatus m_enStatus;		
	// ��־�����ַ�������
	static TCHAR* m_aLevelDescList[];
	// ��־��¼�ļ���
	static int	m_iLogLevel;
	// д��־���ٽ����
	static CRITICAL_SECTION m_csWriteLog;	
	// ��־��Ϣ����
	static CString	m_strWriteStrInfo;			
	// ��־���θ��»�������ݳ���
	static DWORD	m_dwSingleWriteDataSize;
	// �������ʱ���Զ�����־����־û�(��λ����),Ĭ��10��,���ֵ 600����Сֵ 5
	static DWORD	m_dwAutoSaveLogSpanTime;

	static HANDLE	m_hMapLogFile;					// �����ڴ���
    static LPTSTR	m_psMapAddr;					// �����ڴ��ַ
	static CString	m_strShareMemName;				// �����ڴ��ں�����
	static DWORD	m_dwShareMemSize;				// �����ڴ���������С

	static CStdioFile*  m_pFile;					// ��־�ļ�ָ��
	static int          m_iMaxLogFileLen;			// ��־�ļ�����
	static CString		m_strFileName;				// ��־�ļ���
 	static CString		m_strLogDir;				// ��־�洢Ŀ¼����·��
	static CString		m_strPathConfigFile;		// ��־�����ļ�����·��

	static HANDLE m_hThread;						// �������߳̾��
};
