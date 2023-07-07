// CatchDump.cpp: implementation of the CCatchDump class.
//
//////////////////////////////////////////////////////////////////////

#include "CatchDump.h"
#include  <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//��̬������ʼ��
BOOL	CCatchDump::m_bDebugDumpShowAlert	= TRUE;
CString CCatchDump::m_strDebugDumpTip		= _T("");
CString CCatchDump::m_strDebugDumpFilePath	= _T("");
int		CCatchDump::m_iDumpDataType			= 0;

CCatchDump::CCatchDump(IN const int& iCompilerType)
	:m_iDebugDumpCompilerType(iCompilerType)
{

}

CCatchDump::~CCatchDump()
{

}

// ========================================================================================================================
// 
// ��������: CCatchDump::InitCatchExecption()
// 
// ��������: ��ʼ���������DUMP�ļ��Ļ���
// �������: IN const CString& strDumpFilePath	--	���ɵ�Dump�ļ�·��
//         : IN const BOOL& bShowAlert			--	�ڳ������������Ƿ���Ҫ��ʾ��ʾ��Ϣ(TRUE,��ʾ;FALSE,����ʾ)
//         : IN const CString& strTip			--	�ڳ�������������ʾ����ʾ��Ϣ����(Ϊ������Ĭ�ϵ���ʾ��Ϣ)
//		   : IN const int& iDumpDataType		--	���ɵ�Dump�ļ�����������
//													0,Ĭ����С���ݸ�ʽ,����������С��������,���ɲ鿴������
//												    1,ָ���û������ռ�ģʽ,���Բ鿴ȫ�ֱ���ֵ
//												    2,ָ��ȫ�ڴ�����ģʽ,�������쳣��ʹ���ڴ���Ϣ������dump�ļ�,ռ�ÿռ�ϴ�
// ���ز���: void
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2015-1-23 13:55:00]
// ��ע˵��: ����ӿ�,�ú���������APP�࿿ǰ�ĵط�����
// 
// ======================================================================================================================== 
void CCatchDump::InitCatchExecption(IN const CString& strDumpFilePath,IN const BOOL& bShowAlert,IN const CString& strTip,IN const int& iDumpDataType)
{
	m_bDebugDumpShowAlert	= bShowAlert;
	m_strDebugDumpFilePath	= strDumpFilePath;
	m_strDebugDumpTip		= strTip;
	m_iDumpDataType			= iDumpDataType;
	
	::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	
	//���ڸ���6.0�汾��IDE,��Ҫ���������ӿڽ�ֹϵͳCRT�Դ����쳣�����������
	if(0 != m_iDebugDumpCompilerType)
	{
		DisableSetUnhandledExceptionFilter();
	}
}

// ========================================================================================================================
// 
// ��������: BOOL CCatchDump::CreateDumpFile()
// 
// ��������: ����Dump�ļ�,·��Ϊ����ָ��
// �������: IN const CString& strDumpFilePath		--	ָ����DUMP�ļ�·��
//		   : IN OUT EXCEPTION_POINTERS *pException	--	�������쳣������ָ��
// ���ز���: const BOOL
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2015-1-23 10:03:51]
// ��ע˵��: �ڲ���������
// 
// ======================================================================================================================== 
const BOOL CCatchDump::CreateDumpFile(IN const CString& strDumpFilePath,IN OUT EXCEPTION_POINTERS *pException)
{
	BOOL bReturn = FALSE;
	//�Ѿ����ڸ��ļ�,���Խ���ɾ��
	if(::PathFileExists(strDumpFilePath))
	{
		::SetFileAttributes(strDumpFilePath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(strDumpFilePath);
	}
	//��ȡָ���ļ�Ŀ¼
	CString strDir = strDumpFilePath.Left(strDumpFilePath.ReverseFind('\\'));
	if(CreateDumpFileDir(strDir))
	{
		//��ͼ���Դ���DUMP�ļ�
		HANDLE hDumpFile = CreateFile(strDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		if(INVALID_HANDLE_VALUE != hDumpFile)
		{
			//���ò���Dump�ļ�����Ϣ  
			MINIDUMP_EXCEPTION_INFORMATION dumpInfo;

			dumpInfo.ExceptionPointers	= pException;
			dumpInfo.ThreadId			= GetCurrentThreadId();
			dumpInfo.ClientPointers		= TRUE;  
  
			// д��Dump�ļ�����
			// ������ϸ������URL https://msdn.microsoft.com/en-us/library/ms680519(v=vs.85).aspx
			// �ؼ� ��4������ MINIDUMP_TYPE DumpType
			// MiniDumpNormal							-- Ĭ�ϱ��,��ֵΪ0
			// MiniDumpWithFullMemory					-- ȫ�ڴ��ʶ����ģʽ��dmp�ļ�����ܴ�һ��Ϊ����MB
			// MiniDumpWithPrivateReadWriteMemory		-- �������пɶ��Ϳ�д��˽���ڴ�ҳ�����ݣ����Բ쿴ջ��������TLS�����ݣ�PEB��TEBҲ���������档
			// MiniDumpWithIndirectlyReferencedMemory	-- ���߳�ջ�ڴ��е�ÿһ��ָ�롣��Щָ�����ָ���̵߳�ַ�ռ�������ɶ��ڴ�ҳ��һ������������ָ�룬������ȡָ�븽��1024�ֽڵ����ݴ浽minidump�У�ָ��ǰ��256�ֽں�ָ����768�ֽڣ�
			// MiniDumpWithDataSegs						-- ��������װ�ص����п�ִ��ģ��Ŀ�д���ݶ�,���Բ鿴ȫ�ֱ�������ģʽ�µ�dump�ļ���ȫ�ڴ����µ��ļ�ҪС���
			// MiniDumpWithCodeSegs						-- �������н���װ�صĿ�ִ��ģ��Ĵ����
			// MiniDumpWithHandleData					-- ��������ʱ�̽��̹��ϱ���������о��
			// MiniDumpWithThreadInfo					-- �ռ��������̵߳ĸ�����Ϣ[�߳�ʱ�� (����ʱ��,ִ���û�������ں˴����ʱ��),��ڵ�ַ,�����]
			// MiniDumpWithProcessThreadData			-- ����PEB��TEBռ�ݵ��ڴ�ҳ
			// MiniDumpWithFullMemoryInfo				-- �������������ڴ沼�ֵ�������Ϣ
			// MiniDumpWithoutOptionalData				-- ������С������dump�е��ڴ�����ݡ���ָ�������־�ǣ�ֻ��MiniDumpNormalָ�����ڴ�ᱻ���档
			// MiniDumpFilterMemory						-- ջ�ڴ�����ݻ��ڱ���֮ǰ���й���,ֻ���ؽ�����ջ��Ҫ�����ݲŻᱻ����,�������ݻᱻд��0
			// MiniDumpFilterModulePaths				-- �Ƿ����ģ��·��
			// MiniDumpScanMemory						-- ��Լminidumpռ�õĿռ�,�ѵ��Բ���Ҫ�Ŀ�ִ��ģ��ȥ��
			
			MINIDUMP_TYPE enMinidumpType;
			switch(m_iDumpDataType)
			{
			case 0:
				enMinidumpType = MiniDumpNormal;
				break;
			case 1:
				enMinidumpType = MiniDumpWithDataSegs;
				break;
			case 2:
				enMinidumpType = MiniDumpWithFullMemory;
				break;
			}
			//����dump�ļ�
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, enMinidumpType, &dumpInfo, NULL, NULL);  
			//�رվ��
			CloseHandle(hDumpFile);
			//����ִ�з��سɹ�
			bReturn = TRUE;
		}
	}
	return bReturn;
}

// ========================================================================================================================
// 
// ��������: CALLBACK CCatchDump::ApplicationCrashHandler()
// 
// ��������: ����Unhandled Exception�Ļص�����
// �������: EXCEPTION_POINTERS *pException
// ���ز���: LONG
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2015-1-23 10:05:00]
// ��ע˵��: �ڲ���������
// 
// ======================================================================================================================== 
long CALLBACK CCatchDump::ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	//����Dump�ļ�
	if(m_strDebugDumpFilePath.IsEmpty())
		m_strDebugDumpFilePath.Format(_T("D:\\UnHandleExecption.dmp"));
	CreateDumpFile(m_strDebugDumpFilePath,pException);
	//����һЩҵ����

	//ҵ�������,����һ������Ի����˳�����  
	if(m_bDebugDumpShowAlert)
	{  
		if(m_strDebugDumpTip.IsEmpty())
			m_strDebugDumpTip.Format(_T("*** �����⵽ϵͳ���������쳣,�޷������ṩ���񣬳��򼴽��˳�,������ȷ�������������������***"));
		FatalAppExit(0,m_strDebugDumpTip);  
	}


	//��¼��־
	//remove by bing WRITE_LOG(FmtStr(_T("ϵͳ�쳣������δ��׽���Ĵ���(�������:%d)������쳣��ֹ")),pException->ExceptionRecord->ExceptionCode);
	//ע��ϵͳ
	//remove by bing ReStartComputerEx(NULL,EWX_LOGOFF | EWX_FORCE);
	//ǿ�ƽ�������
//	::ExitProcess(0);
	::ExitProcess(0);

	return EXCEPTION_EXECUTE_HANDLER;
}

// ========================================================================================================================
// 
// ��������: CCatchDump::DisableSetUnhandledExceptionFilter()
// 
// ��������: �°汾��CRTʵ�����쳣������,ǿ��ɾ������Ӧ�ó�����ǰ���õĲ�����,�����ó����޷�����DUMP�ļ�
//		     ����CRT����SetUnhandledExceptionFilter������ʹ֮��Ч
// ���ز���: void
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2015-1-23 10:10:15]
// ��ע˵��: �ú���x86��������Ч
// 
// ======================================================================================================================== 
void CCatchDump::DisableSetUnhandledExceptionFilter()
{
	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),"SetUnhandledExceptionFilter");
	if(NULL != addr)
	{
		BYTE byCode[16] = {0};
		int iSize = 0;

		byCode[iSize++] = 0x33;
		byCode[iSize++] = 0xC0;
		byCode[iSize++] = 0xC2;
		byCode[iSize++] = 0x04;
		byCode[iSize++] = 0x00;

		DWORD dwOldFlag		= 0;
		DWORD dwTempFlag	= 0;
		VirtualProtect(addr, iSize, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, byCode, iSize, NULL);
		VirtualProtect(addr, iSize, dwOldFlag, &dwTempFlag);
	}
}

// ========================================================================================================================
// 
// ��������: BOOL CCatchDump::CreateDumpFileDir()
// 
// ��������: ����ָ����·������Ŀ¼
// �������: IN const CString& strDirPath	--	ָ����Ŀ¼·��
// ���ز���: const BOOL
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2015-1-23 15:04:38]
// ��ע˵��: �ڲ���������
// 
// ======================================================================================================================== 
const BOOL CCatchDump::CreateDumpFileDir(IN const CString& strDirPath)
{
	BOOL bNetPath	= FALSE;
	CStringArray	aDirList;
	CString strPath(strDirPath);

	if(strPath.Find(_T("\\\\"))==0) 
		bNetPath=TRUE;

	while(::_taccess(strPath,0)!=0)
	{
		if(-1 == strPath.Find('\\')	) 
			return FALSE;
		CString strTemp = strPath.Mid(strPath.ReverseFind('\\'));
		aDirList.InsertAt(0,strTemp);
		strPath = strPath.Left(strPath.ReverseFind('\\'));
		if(		bNetPath 
			&&	strPath.GetLength()<2) 
			return FALSE;
	}

	while(aDirList.GetSize()!=0)
	{
		strPath += aDirList[0];
		aDirList.RemoveAt(0);
		if(!::CreateDirectory(strPath,NULL))
			return FALSE;
	}

	return TRUE;
}