
#if !defined(AFX_CATCHDUMP_H__INCLUDED_)
#define AFX_CATCHDUMP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************˵��********************************************************
		
1.VC6.0��������,�����ɲ���Ĭ�Ϲ��캯��(Ĭ�ϲ���)
2.VC2005�������ϰ汾�������ɲ���Ĭ�Ϲ��캯��,���캯����������Ϊ1
3.������Release�汾���У����뽫������Ϣ������(���������أ���ѡ ��ͨ������Ϣ ����MAP�ļ�)
4.ʹ�÷�ʽ -- �ڳ���ʼ�׶�,���ýӿ� InitCatchExecption ����
5.VS�����ֳ��ָ���ѡ������Ӧ��IDE��dmp�ļ�,Ȼ�󵥲����е��Լ��ɿ������ⷢ�����������

ע��:a VC6.0 Ĭ��֧��SHE�쳣,����ͨ�� try{} catch(...){} �������е��쳣������C++�쳣��CPU�쳣��
     b VS2005���ϵ�IDEĬ�ϲ�֧�ֲ�׽CPU�쳣��ֻ�ܲ�׽C++�쳣����Ҫ��C++����ѡ��������:/Eha
	 c WinDbg���������ݿ�:    SRV*C:\Symbols*http://msdl.microsoft.com/download/symbols
	   ��Ҫ��ָ�������ļ�·��������

******************************************����********************************************************/

#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")  

//����dump�ļ���������Ķ���
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

class CCatchDump  
{
public:
	CCatchDump(IN const int& iCompilerType = 0);
	virtual ~CCatchDump();
//Impl
public:
	//��ʼ������dump�ļ��Ļ���
	void InitCatchExecption(IN const CString& strDumpFilePath,IN const BOOL& bShowAlert = TRUE,IN const CString& strTip = _T(""),IN const int& iDumpDataType = 0);
private:
	//����Dump�ļ�,����Ϊ���ļ��ľ���·��
	static const BOOL CreateDumpFile(IN const CString& strDumpFilePath,IN OUT EXCEPTION_POINTERS *pException);
	//����Unhandled Exception�Ļص�����
	static long CALLBACK ApplicationCrashHandler(EXCEPTION_POINTERS *pException);
	//����ָ��·���µ�Ŀ¼
	static const BOOL CreateDumpFileDir(IN const CString& strDirPath);

	//��VC2005�汾,��Ҫ���������ӿ�,��ֹϵͳ�Զ������˹��趨DUMP����
	void DisableSetUnhandledExceptionFilter();
//Attributes
private:
	//�������汾
	//0,Ĭ��ΪVC6�汾��IDE
	//1,VS2005���ϰ汾��IDE
	int		m_iDebugDumpCompilerType;

	//����������Ƿ��ṩ������ʾ
	static BOOL	m_bDebugDumpShowAlert;
	//������������ʾ��Ϣ
	static CString	m_strDebugDumpTip;
	//Dump�ļ�����·��
	static CString	m_strDebugDumpFilePath;
	//���ɵ�dump�ļ����ݰ�������
	static int m_iDumpDataType;
};

#endif // !defined(AFX_CATCHDUMP_H__INCLUDED_)
