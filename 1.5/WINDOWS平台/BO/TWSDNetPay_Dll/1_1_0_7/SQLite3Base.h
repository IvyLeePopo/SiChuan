/*
 * Copyright(C) 2014,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 *
 * �ļ����ƣ�SQLite3Base.h
 * ժ    Ҫ������SQLite3�Ļ�������
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2014��8��6��
 */
#pragma once
#include "sqlite3.h"
//#pragma comment(lib, "sqlite3.lib")

class CSQLite3
{
public:
	CSQLite3(void);
	~CSQLite3(void);
//Interfaces
public:
	//��ȡ���µĴ�����Ϣ
	inline const CString GetLastErrorDesc() {return m_strLastErrorDesc;}
	inline const CString GetLastErrorDesc(IN OUT DWORD& dwErrorCode) {dwErrorCode = m_dwLastErrorCode; return m_strLastErrorDesc;}

	//�������ݿ��ļ��ľ���·��
	inline const BOOL SetDatabaseFilePath(IN const CString& strFilePath) {m_strDatabaseFilePath = strFilePath;}

	//��ȡ��һ�β���������ʱ��
	inline const DWORD GetLastOperationConsumeTime(IN const int& iType = 1) {return (1 == iType) ? m_dwExecuteSQLConsumeTime : m_dwExecuteTransactionTime;}

	/***************************��׼SQLִ��*************************************/

	//ִ��SQL���
	const BOOL ExecuteSQL(IN const CString& strSQL, IN const BOOL& bIgnorePrimaryKey = FALSE);
	//ִ��SQL�������ú���
	const BOOL ExecuteSQL(IN const CString& strSQL, int (*callback)(void*, int, char**, char**), IN const BOOL& bIgnorePrimaryKey = FALSE);
	//��������
	const BOOL BeginTransaction(IN const DWORD& dwSingleSubmitCount = 0);
	//�ݽ�����
	const BOOL CommitTransaction(void);
	//�ع�����
	const BOOL RollbackTransaction(void);

	/***************************ϵͳ����*************************************/

	//���ô����ݿ�����Ա��
	void SetOpenConnectFlag(IN const BOOL& bMulThread = FALSE);
	//�����ݿ�
	const BOOL OpenConnection(IN const BOOL& bAutoShrinkageDatabase = FALSE);
	const BOOL OpenConnection(IN const CString& strDatabaseFilePath, IN const BOOL& bAutoShrinkageDatabase = FALSE);
	//�ر����ݿ�
	const BOOL CloseConnection(void);
	//�򿪼�¼��
	const BOOL OpenRecordSet(void);
	//�رռ�¼��
	const BOOL CloseRecordSet(void);
	//�ж������Ƿ��
	inline const BOOL IsOpenConnection(void) const {return NULL != m_pSQLite3DB;}
	//�жϼ�¼���Ƿ��
	inline const BOOL IsOpenRecordSet(void) const {return NULL != m_pSQLite3STMT;}


	//�����������Ƿ����
	const BOOL CheckTableExists(IN const CString& strTableName);
	//ɾ����
	const BOOL DropTable(IN const CString& strTableName);

	//���������ݱ�Ӱ��
	const int EffectRows(IN const CString& strSQL = _T(""));
	//��ȡ��ǰ����ϵͳ�ڴ�ʹ��״̬
	const sqlite3_int64 GetUseMemory(void);
	//��ȡ�����������ļ�ʵʱ��С
	const DWORD GetDatabaseFileSize(void);

	/***************************��¼�����ݻ�ȡ*************************************/

	//�жϵ�ǰ���ؼ�¼���Ƿ��Ѿ��α굽���
	const BOOL IsEOF(void);
	//����������ȡ��ֵ(�ַ���)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT CString& strValue);
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT TCHAR* szValue);
	//����������ȡ��ֵ(�з���4�ֽ�����)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT int& iValue);
	//����������ȡ��ֵ(�޷���4�ֽ�����)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT UINT& nValue);
	//����������ȡ��ֵ(�з���8�ֽ�����)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT INT64& llValue);
	//����������ȡ��ֵ(������)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT double& dValue);
	//����������ȡ��ֵ(������)
	const BOOL GetColumnValue(IN const CString& strColumnName, IN OUT CTime& cTime);
private:
	/**
	* add zhuyabing 2017-04
	* ��VS���Ե�����£���������� throw��䣬�����Ƶ��������ʾ��Ϣ��Ӱ�����
	* �˴��Կ���Ӱ��Ĳ��֣�ͳһ����û���쳣�İ汾
	*/
	const BOOL CloseRecordSet_noexcept(void);
	const BOOL CloseConnection_noexcept(void);
private:
	//���ô����ݿ�����Ա��
	void SetOpenConnectFlag(IN const DWORD& dwOpenFlag) { m_dwOpenFlag = dwOpenFlag;}
	//ASCII�ַ���ת��ΪUnicode�ַ���
	const CString AsciiToUnicode(IN const char* szString);
	//Unicode�ַ���ת��ΪASCII�ַ���
	char* UnicodeToAscii(IN const CString& strString);
	//�������ݿ�
	const BOOL AutoShrinkageDatabase(void);
//Attributes
private:
	//������ݿ����Ƿ��Ѿ�����ʼ��
	BOOL	m_bInitDatabase;
	//�Ƿ����������ͻ
	BOOL	m_bIgnorePrimaryKey;
	//���µĴ������
	DWORD	m_dwLastErrorCode;
	//���µĴ���������Ϣ
	CString m_strLastErrorDesc;
	//�������ݿ��ļ�����·��
	CString	m_strDatabaseFilePath;
	//���ݾ��
	sqlite3* m_pSQLite3DB;
	//���ݼ����
	sqlite3_stmt* m_pSQLite3STMT;
	//�����ݿ�ı��--Ĭ�ϴ�����ֻ������
	DWORD	m_dwOpenFlag;
	//ִ��SQLӰ��ļ�¼��
	DWORD	m_dwEffectRecordCount;
	//��ȡ��¼����������
	DWORD	m_dwIndexOfRecordSet;
	//���ִ��SQL���Ƿ��м�¼��
	BOOL	m_bConfirmHasRecordSet;
	//�����������ʱ��ִ�в������ʱ���۵�ָ����¼������еݽ�
	DWORD	m_dwCommitInsertRecordCount;
	//��ʼִ������ĳ�ʼʱ��
	DWORD	m_dwStartTransactionTime;
	//ִ�е���SQL����������ʱ��
	DWORD	m_dwExecuteSQLConsumeTime;
	//ִ������������ĵ�ʱ��
	DWORD	m_dwExecuteTransactionTime;


    
};
