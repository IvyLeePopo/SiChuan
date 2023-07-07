/*
 * Copyright(C) 2014,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 *
 * �ļ����ƣ�SQLite3Base.cpp
 * ժ    Ҫ������SQLite3�����������ݿ����������ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2014��8��6��
 */
#include "StdAfx.h"
#include "SQLite3Base.h"
#include "Shlwapi.h"

#include "sqlite3_agent.h"

CSQLite3::CSQLite3(void)
	: m_bInitDatabase(FALSE)
	, m_bIgnorePrimaryKey(FALSE)
	, m_dwLastErrorCode(0)
	, m_strLastErrorDesc(_T(""))
	, m_strDatabaseFilePath(_T(""))
	, m_pSQLite3DB(NULL)
	, m_pSQLite3STMT(NULL)
	, m_dwOpenFlag(SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE)
	, m_dwEffectRecordCount(0)
	, m_dwIndexOfRecordSet(0)
	, m_bConfirmHasRecordSet(FALSE)
	, m_dwCommitInsertRecordCount(0)
	, m_dwStartTransactionTime(0)
	, m_dwExecuteSQLConsumeTime(0)
	, m_dwExecuteTransactionTime(0)
{
	
}

CSQLite3::~CSQLite3(void)
{
	if (IsOpenRecordSet())
	{
		CloseRecordSet();
	}
	if (IsOpenConnection())
	{
		CloseConnection();
	}
	
}

// ========================================================================================================================
//
// ��������: CSQLite3::SetOpenConnectFlag()
//
// ��������: ���ô����ݿ�ı��
// �������: IN const BOOL& bMulThread -- �Ƿ�������߳�
// ���ز���: void
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2015-1-9 15:28:48]
// ��ע˵��: �����ڵ���OpenConnection֮ǰ���÷���Ч
//
// ========================================================================================================================
void CSQLite3::SetOpenConnectFlag(IN const BOOL& bMulThread)
{
	DWORD dwOpenFlag = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
	if (bMulThread)
		dwOpenFlag |= SQLITE_OPEN_FULLMUTEX;
	SetOpenConnectFlag(dwOpenFlag);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::ExecuteSQL
//
// ����������ִ��SQL���
//
// ���������IN const CString& strSQL		   -- �Ƿ����������ͻ
//		   ��IN const BOOL& bIgnorePrimaryKey  -- �Ƿ����������ͻ
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::ExecuteSQL(IN const CString& strSQL, IN const BOOL& bIgnorePrimaryKey)
{
	BOOL bResult = FALSE;
	char szThrowDBError[512] = {0};
	CString strRealSQL(strSQL);
	strRealSQL.TrimLeft();
	strRealSQL.TrimRight();
	try
	{
		//�ж����ݿ��Ƿ���Ч
		if (!IsOpenConnection())
			throw - 1;
		//�ж���ִ��ʲô���
		BOOL bHasRecordSet = FALSE;
		BOOL bContinueExecuteSQL = TRUE;
		int iDBStatus = -1;
		if (0 == strSQL.Left(6).CompareNoCase(_T("SELECT")))
		{
			if (NULL != m_pSQLite3STMT)
			{
				iDBStatus = sqlite3_agent::sqlite3_finalize_fun(m_pSQLite3STMT);
				m_pSQLite3STMT = NULL;
			}
			//���Զ��ִ��ָ��SQL
			while (bContinueExecuteSQL)
			{
				iDBStatus = sqlite3_agent::sqlite3_prepare_v2_fun(m_pSQLite3DB, strRealSQL.GetBuffer(0), strRealSQL.GetLength(), &m_pSQLite3STMT, NULL);
				switch (iDBStatus)
				{
				case SQLITE_BUSY:
					::Sleep(50);
					break;
				case SQLITE_OK:
					bContinueExecuteSQL = FALSE;
					break;
				default:
					bContinueExecuteSQL = FALSE;
					break;
				}
				Sleep(50);
			}
			//����м�¼������
			m_bConfirmHasRecordSet = TRUE;
		}
		else
		{
			m_bConfirmHasRecordSet = FALSE;

			while (bContinueExecuteSQL)
			{
				iDBStatus = sqlite3_agent::sqlite3_exec_fun(m_pSQLite3DB, strRealSQL.GetBuffer(0), NULL, NULL, NULL);
				switch (iDBStatus)
				{
				case SQLITE_BUSY:
					::Sleep(50);
					break;
				case SQLITE_OK:
					bContinueExecuteSQL = FALSE;
					break;
				default:
					bContinueExecuteSQL = FALSE;
					break;
				}
				Sleep(50);
			}
		}

		//�ж��쳣 --  �������Ҫ����������ͻ�����׳��쳣
		if (19 == iDBStatus)
		{
			if (!bIgnorePrimaryKey)
			{
				if (SQLITE_OK != iDBStatus)
				{
					throw - 3;
				}
			}
		}
		else if (SQLITE_OK != iDBStatus)
			throw - 3;

		//���Ӱ���¼��--�����ɾ��
		m_dwEffectRecordCount = EffectRows(m_bConfirmHasRecordSet ? strSQL : _T(""));
		//��������м�¼�����򵥶�����
		if (m_bConfirmHasRecordSet)
		{
			m_dwIndexOfRecordSet = 0;
		}
		bResult = TRUE;
	}
	catch (int& iErrorCode)
	{
		//���ü�¼�����Ϊ��
		m_bConfirmHasRecordSet = FALSE;
		//������ݿⷴ���Ĵ��������
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("������SQLite3���ݿ�����Ч!"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�ַ���ת��ʧ��!"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("SQLִ��ʧ��,�������:%d,��������:%s"), m_dwLastErrorCode, szThrowDBError);
			break;
		}
	}
	return bResult;
}

const BOOL CSQLite3::ExecuteSQL(IN const CString& strSQL, int (*callback)(void*, int, char**, char**), IN const BOOL& bIgnorePrimaryKey)
{
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::BeginTransaction
//
// ������������������
//
// ���������IN const DWORD& dwSingleSubmitCount=0
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::BeginTransaction(IN const DWORD& dwSingleSubmitCount)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("BEGIN;"));
	if (bResult)
	{
		m_dwCommitInsertRecordCount = dwSingleSubmitCount;
		m_dwStartTransactionTime = ::GetTickCount();
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::CommitTransaction
//
// �����������ݽ�����
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CommitTransaction(void)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("COMMIT;"));
	if (bResult)
	{
		m_dwCommitInsertRecordCount = -1;
		m_dwExecuteTransactionTime = ::GetTickCount() - m_dwStartTransactionTime;
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::RollbackTransaction
//
// �����������ع�����
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::RollbackTransaction(void)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("ROLLBACK;"));
	if (bResult)
	{
		m_dwCommitInsertRecordCount = -1;
		m_dwExecuteTransactionTime = ::GetTickCount() - m_dwStartTransactionTime;
	}
	return bResult;
}

	

char* TMP_MbcsToUtf8(const char *file)
{
    CString str;
    WCHAR   *pwchar=0;
    CHAR    *pchar=0;
    int len=0;
    int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
    len=MultiByteToWideChar(codepage, 0, file, -1, NULL,0);
    pwchar=new WCHAR[len];
    if(pwchar!=0)
    {
        len = MultiByteToWideChar(codepage, 0, file, -1, pwchar, len);
        if( len!=0 ) 
        {
            len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, 0, 0, 0, 0);
            pchar=new CHAR[len];
            if(pchar!=0)
            {
                len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, pchar, len,0, 0);
                if(len!=0)                 
                {
                    str=pchar;                    
                }
                delete pchar;
            }
            delete pwchar; 
        }
    }

	int iLen = str.GetLength();
	char* pszData = NULL;
	if (iLen > 0)
	{
		pszData = new char[iLen + 1];
		if (NULL != pszData)
		{
			memset(pszData, 0, iLen + 1);
			strcpy(pszData, str.GetBuffer(0));
		}
	}
	return pszData;
	

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::OpenConnection
//
// ������������һ�����ݿ⣬�������򴴽�
//
// ���������IN const BOOL& bAutoShrinkageDatabase	--	�Ƿ��������ݿ��Զ�����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::OpenConnection(IN const BOOL& bAutoShrinkageDatabase)
{
	BOOL bResult = FALSE;
	char szThrowDBError[512] = {0};
	try
	{
		//����ڲ��ļ�����·���Ƿ�Ϊ��
		if (m_strDatabaseFilePath.IsEmpty())
			throw - 1;
		//������ݿ����Ƿ��Ѿ���Ч(��Ч��ȥ��)
		int iDBStatus = -1;
		if (!CloseConnection())
			throw - 2;
		//�ַ���ת��
		//modify 
		char* pszString = TMP_MbcsToUtf8((LPSTR)(LPCTSTR)m_strDatabaseFilePath);// = UnicodeToAscii(m_strDatabaseFilePath);
		if (NULL == pszString)
			throw - 3;
		//ִ�д������SQLite3���ݿ�ָ��
		iDBStatus = sqlite3_agent::sqlite3_open_v2_fun(pszString, &m_pSQLite3DB, m_dwOpenFlag, NULL);
		if (SQLITE_OK != iDBStatus)
			throw - 4;
		if (bAutoShrinkageDatabase)
		{
			if (!AutoShrinkageDatabase())
			{
				throw - 5;
			}
		}
		//�����Դ
		delete []pszString;
		pszString = NULL;

		bResult = TRUE;
	}
	catch (int& iErrorCode)
	{
		//������ݿⷴ���Ĵ��������
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ڲ�����:��ָ�������ݿ��ļ�����·���ַ�ֵΪ��!"));
			break;
		case -2:
			break;//�ڲ��Ѿ�����
		case -3:
			m_strLastErrorDesc.Format(_T("�ڲ�����:�ַ���ת��(ASCIIתUnicode)ʧ��!"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("�ڲ�����:�����ݿ��ļ�ʧ��,�������:%d,��������:%s"), m_dwLastErrorCode, szThrowDBError);
			break;
		case -5:
			//�ڲ��Ѿ�����
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::OpenConnection
//
// ������������һ�����ݿ⣬�������򴴽�
//
// ���������IN const CString& strDatabaseFilePath	--	�򿪵����ݿ��ļ�����·��
//		   ��IN const BOOL& bAutoShrinkageDatabase	--	�Ƿ��������ݿ��Զ�����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::OpenConnection(IN const CString& strDatabaseFilePath, IN const BOOL& bAutoShrinkageDatabase)
{
	m_strDatabaseFilePath = strDatabaseFilePath;
	return OpenConnection(bAutoShrinkageDatabase);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::CloseConnection
//
// �����������ر����ݿ�����
//
// ���������void
// ���������const BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CloseConnection(void)
{
	return CloseConnection_noexcept();
	BOOL bResult = TRUE;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	try
	{
		if (NULL != m_pSQLite3DB)
		{
			iDBStatus = sqlite3_agent::sqlite3_close_v2_fun(m_pSQLite3DB);
			if (SQLITE_OK != iDBStatus)
				throw - 1;
			m_pSQLite3DB = NULL;
		}
	}
	catch (int& iErrorCode)
	{
		//������ݿⷴ���Ĵ��������
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ڲ�����:�ر��Ѿ����ڵ�SQLite3���ݿ���ʧ��!"));
			break;
		}
		//�����Դ���,���ر�ʧ�ܵ�ʱ��,ֱ�ӽ������ݿ��ļ��������
		if (SQLITE_ERROR == iDBStatus)
		{
			m_pSQLite3DB = NULL;
		}
		//����ʧ��
		bResult = FALSE;
	}
	return bResult;
}
const BOOL CSQLite3::CloseConnection_noexcept(void)
{
	BOOL bResult = TRUE;
	int preThrowNo = 0;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	while (true)
	{
		if (NULL != m_pSQLite3DB)
		{
			iDBStatus = sqlite3_agent::sqlite3_close_v2_fun(m_pSQLite3DB);
			if (SQLITE_OK != iDBStatus)
			{
				preThrowNo = - 1;
				break;
			}
			m_pSQLite3DB = NULL;
		}
		break;
	}
	if (preThrowNo != 0)
	{
		//������ݿⷴ���Ĵ��������
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (preThrowNo)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ڲ�����:�ر��Ѿ����ڵ�SQLite3���ݿ���ʧ��!"));
			break;
		}
		//�����Դ���,���ر�ʧ�ܵ�ʱ��,ֱ�ӽ������ݿ��ļ��������
		if (SQLITE_ERROR == iDBStatus)
		{
			m_pSQLite3DB = NULL;
		}
		//����ʧ��
		bResult = FALSE;
	}


	return bResult;

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::OpenRecordSet
//
// ��������������һ��ִ��SQL��䷵�صĽ����¼��
//
// ���������void
// ���������const BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::OpenRecordSet(void)
{
	if (    (m_bConfirmHasRecordSet)
	        && (NULL != m_pSQLite3STMT)
	   )
	{
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::CloseRecordSet
//
// �����������رռ�¼��
//
// ���������void
// ���������const BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CloseRecordSet(void)
{
	return CloseRecordSet_noexcept();
	BOOL bResult = TRUE;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	try
	{
		if (!IsOpenConnection())
			throw - 1;
		if (NULL != m_pSQLite3STMT)
		{
			iDBStatus = sqlite3_agent::sqlite3_finalize_fun(m_pSQLite3STMT);
			if (SQLITE_OK != iDBStatus)
				throw - 2;
			m_pSQLite3STMT = NULL;
		}
	}
	catch (int& iErrorCode)
	{
		//������ݿⷴ���Ĵ��������
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ڲ�����:��¼��������SQLite3���ݿ�����Ч!"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�ڲ�����:�ر��Ѿ����ڵ�SQLite3��¼�����ʧ��!"));
			break;
		}
		bResult = FALSE;
	}
	return bResult;
}
const BOOL CSQLite3::CloseRecordSet_noexcept(void)
{
	BOOL bResult = TRUE;
	int preThrowNo = 0;
	int iDBStatus = -1;
	char szThrowDBError[512] = {0};
	while (true)
	{
		if (!IsOpenConnection())
		{
			preThrowNo = -1;
			break;
		}
		if (NULL != m_pSQLite3STMT)
		{
			iDBStatus = sqlite3_agent::sqlite3_finalize_fun(m_pSQLite3STMT);
			if (SQLITE_OK != iDBStatus)
			{
				preThrowNo = -2;
				break;
			}
			m_pSQLite3STMT = NULL;
		}
		break;
	}
	if (preThrowNo != 0)
	{
		//������ݿⷴ���Ĵ��������
		m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(m_pSQLite3DB);
		strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(m_pSQLite3DB));
		switch (preThrowNo)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ڲ�����:��¼��������SQLite3���ݿ�����Ч!"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�ڲ�����:�ر��Ѿ����ڵ�SQLite3��¼�����ʧ��!"));
			break;
		}
		bResult = FALSE;
	}
	return bResult;

}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::CheckTableExists
//
// �����������������ı��Ƿ����
//
// ���������IN const CString& strTableName -- ����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::CheckTableExists(IN const CString& strTableName)
{
	BOOL bRsult = FALSE;
	try
	{
		if (!IsOpenConnection())
			throw - 1;
		//ִ�в�ѯSQL
		CString strSQL;
		strSQL.Format(_T("SELECT tbl_name FROM sqlite_master WHERE type='table' "));
		//�ر��Ѿ����ڵļ�¼��
		CloseRecordSet();
		//ִ�в�ѯ���
		if (!ExecuteSQL(strSQL))
			throw - 2;
		if (NULL == m_pSQLite3STMT)
			throw - 3;
		while (!IsEOF())
		{
			CString strValue;
			if (!GetColumnValue(_T("tbl_name"), strValue))
				throw - 4;
			if (strTableName == strValue)
			{
				bRsult = TRUE;
				break;
			}
			Sleep(1);
		}
		//�ر����ݼ�
		CloseRecordSet();
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ڲ�����:SQLite3���ݿ����Ӿ����Ч!"));
			break;
		case -2://�Ѿ��ڲ�����
		case -3://�Ѿ��ڲ�����
			break;
		case -4:
			break;
		case -5://���������ݳ���
			break;
		}
	}
	return bRsult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::DropTable
//
// ����������ɾ��һ���Ѿ����ڵ��û���
//
// ���������IN const CString& strTableName -- ����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::DropTable(IN const CString& strTableName)
{
	BOOL bResult = TRUE;
	try
	{
		if (!CheckTableExists(strTableName))
			throw - 1;
		CString strSQL;
		strSQL.Format(_T("DROP TABLE %s"), strTableName);
		if (!ExecuteSQL(strSQL))
			throw - 2;
		bResult = TRUE;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
		case -2:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::EffectRows
//
// ������������ȡ�����м�¼��Ӱ��
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const int CSQLite3::EffectRows(IN const CString& strExecuteSQL)
{
	int iEffectRows = -1;
	int iDBStatus = -1;
	CString strSQL(strExecuteSQL);
	if (IsOpenConnection())
	{
		if (strExecuteSQL.IsEmpty())
		{
			iEffectRows = sqlite3_agent::sqlite3_changes_fun(m_pSQLite3DB);
		}
		else
		{
			CString strTemp(strExecuteSQL);
			strTemp.MakeUpper();

			CString strWhere = strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("FROM")));

			CString strColumnName(_T("RecordCount"));
			strSQL.Format(_T("SELECT COUNT(1) AS %s %s "), strColumnName, strWhere);

			sqlite3_stmt * pSQLite3STMT = NULL;

			if (SQLITE_OK == (iDBStatus = sqlite3_agent::sqlite3_prepare_v2_fun(m_pSQLite3DB, strSQL.GetBuffer(0), strSQL.GetLength(), &pSQLite3STMT, NULL)))
			{
				if (SQLITE_ROW == (iDBStatus = sqlite3_agent::sqlite3_step_fun(pSQLite3STMT)))
				{
					int iRecordCount = -1;
					//��ȡָ������ֶ���Ŀ
					int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(pSQLite3STMT);
					//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
					for (int i = 0; i < iFieldCount; ++i)
					{
						char szFieldName[128] = {0};
						strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(pSQLite3STMT, i));
						if (0 == strColumnName.CompareNoCase(szFieldName))
						{
							int iDataType = sqlite3_agent::sqlite3_column_type_fun(pSQLite3STMT, i);
							if (SQLITE_INTEGER == iDataType)
							{
								iRecordCount = sqlite3_agent::sqlite3_column_int_fun(pSQLite3STMT, i);
								iEffectRows = iRecordCount;
							}
							break;
						}
					}
				}
			}
			//�ͷ���Դ
			strSQL.ReleaseBuffer();
			//�رռ�¼��
			if (NULL != pSQLite3STMT)
				iDBStatus = sqlite3_agent::sqlite3_finalize_fun(pSQLite3STMT);
			pSQLite3STMT = NULL;
		}
	}
	return iEffectRows;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetUseMemory
//
// ������������ȡ��ǰSQLite3���ݿ�ʹ���ڴ�
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const sqlite3_int64 CSQLite3::GetUseMemory(void)
{
	return sqlite3_agent::sqlite3_memory_used_fun();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetDatabaseFileSize
//
// ������������ȡ���ݿ��ļ���С
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const DWORD CSQLite3::GetDatabaseFileSize(void)
{
	DWORD dwSize = 0;
	CFileStatus status;
	if (::PathFileExists(m_strDatabaseFilePath))
	{
		CFile::GetStatus(m_strDatabaseFilePath, status);
		dwSize = (DWORD)status.m_size;
	}
	return dwSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::IsEOF
//
// �����������жϼ�¼���Ƿ��Ѿ�����
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::IsEOF(void)
{
	int iDBStatus = -1;
	if (SQLITE_ROW == (iDBStatus = sqlite3_agent::sqlite3_step_fun(m_pSQLite3STMT)))
	{
		if (m_dwIndexOfRecordSet < m_dwEffectRecordCount)
			++m_dwIndexOfRecordSet;
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetColumnValue
//
// ������������ȡ��ֵ - �ַ���
//
// ���������IN const CString& strColumnName -- ����
//         ��IN OUT CString& strValue		 --	����ֵ
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT CString& strValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//��ȡָ������ֶ���Ŀ
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_TEXT == iDataType)
				{
					bResult = TRUE;
					strValue.Format(_T("%s"), sqlite3_agent::sqlite3_column_text_fun(m_pSQLite3STMT, i));
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

// ========================================================================================================================
//
// ��������: BOOL CSQLite3::GetColumnValue()
//
// ������������ȡ��ֵ - �ַ���
//
// ���������IN const CString& strColumnName	-- ����
//         ��IN OUT TCHAR* szValue				--	����ֵ
// ���ز���: const
// �����汾: 1.0.0.1
// ��д��Ա: [ROCY]
// ��дʱ��: [2014-12-4 8:44:51]
// ��ע˵��:
//
// ========================================================================================================================
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT TCHAR* szValue)
{
	CString strValue;
	BOOL bReturn = GetColumnValue(strColumnName, strValue);
	if (bReturn)
		strcpy(szValue, strValue.GetBuffer(0));
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetColumnValue
//
// ������������ȡ��ֵ - ����
//
// ���������IN const CString& strColumnName	--  ����
//         ��IN OUT int& iValue					--	����ֵ
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT int& iValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//��ȡָ������ֶ���Ŀ
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_INTEGER == iDataType)
				{
					iValue = sqlite3_agent::sqlite3_column_int_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetColumnValue
//
// ��������������������ȡ��ֵ(�޷���4�ֽ�����)
//
// ���������IN const CString& strColumnName	--  ����
//         ��IN OUT UINT& nValue				--  4�ֽ��޷�������
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT UINT& nValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//��ȡָ������ֶ���Ŀ
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_INTEGER == iDataType)
				{
					nValue = (UINT)sqlite3_agent::sqlite3_column_int_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetColumnValue
//
// ��������������������ȡ��ֵ(�з���8�ֽ�����)
//
// ���������IN const CString& strColumnName	--  ����
//         ��IN OUT INT64& llValue				--  8�ֽ��з�������
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT INT64& llValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//��ȡָ������ֶ���Ŀ
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_INTEGER == iDataType)
				{
					llValue = sqlite3_agent::sqlite3_column_int64_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetColumnValue
//
// ������������ȡ��ֵ - ����
//
// ���������IN const CString& strColumnName	- ����
//         ��IN OUT double& dValue				- ����ֵ
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT double& dValue)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//��ȡָ������ֶ���Ŀ
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_FLOAT == iDataType)
				{
					dValue = sqlite3_agent::sqlite3_column_double_fun(m_pSQLite3STMT, i);

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::GetColumnValue
//
// ������������ȡ��ֵ - ʱ��ֵ
//
// ���������IN const CString& strColumnName	- ����
//         ��IN OUT CTime& cTime				- ����ֵ
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::GetColumnValue(IN const CString& strColumnName, IN OUT CTime& cTime)
{
	BOOL bResult = FALSE;
	try
	{
		if (NULL == m_pSQLite3STMT)
			throw - 1;
		//��ȡָ������ֶ���Ŀ
		int iFieldCount = sqlite3_agent::sqlite3_column_count_fun(m_pSQLite3STMT);
		//�ֶ���Ŀ����0,��ʾ���ݴ���,��ʼ������ȡ
		if (0 == iFieldCount)
			throw - 2;
		for (int i = 0; i < iFieldCount; ++i)
		{
			char szFieldName[128] = {0};
			strcpy_s(szFieldName, _countof(szFieldName), sqlite3_agent::sqlite3_column_name_fun(m_pSQLite3STMT, i));
			if (0 == strColumnName.CompareNoCase(szFieldName))
			{
				int iDataType = sqlite3_agent::sqlite3_column_type_fun(m_pSQLite3STMT, i);
				if (SQLITE_TEXT == iDataType)
				{
					CString strValue;
					strValue.Format(_T("%s"), sqlite3_agent::sqlite3_column_text_fun(m_pSQLite3STMT, i));
					COleDateTime oleGetTime;
					oleGetTime.ParseDateTime(strValue);

					COleDateTime t1 = (time_t)0;
					COleDateTime t2 = oleGetTime.m_dt;

					COleDateTimeSpan tSpan;
					tSpan = t2 - t1;

					CTime cTempTime(time_t(tSpan.GetTotalSeconds()));
					cTime = cTempTime;

					bResult = TRUE;
				}
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		}
	}
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::AsciiToUnicode
//
// ����������ASCII�ַ���ת����Unicode�ַ���
//
// ���������IN const char* szString
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const CString  CSQLite3::AsciiToUnicode(IN const char* szString)
{
	CString strString;
	int iAsciiLen = (int)strlen(szString);
	int  iUnicodeLen = ::MultiByteToWideChar( CP_ACP, 0, szString, -1, NULL, 0 );
	wchar_t *  pUnicode = NULL;
	pUnicode = new  wchar_t[iUnicodeLen + 1];
	memset(pUnicode, 0, (iUnicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar( CP_ACP, 0, szString, -1, (LPWSTR)pUnicode, iUnicodeLen);
	strString.Format(_T("%s"), ( wchar_t* )pUnicode);
	delete  pUnicode;
	return  strString;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::UnicodeToAscii
//
// ����������Unicode�ַ���ת����ASCII�ַ���
//
// ���������IN const CString& strString
// ���������char*
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
char* CSQLite3::UnicodeToAscii(IN const CString& strString)
{
	/*
		int iLen = WideCharToMultiByte(CP_ACP,0,strString,strString.GetLength(),NULL,0,NULL,NULL);
		char* pszSQL = new char[iLen+1];
		if(NULL != pszSQL)
		{
			memset(pszSQL,0,iLen+1);
			WideCharToMultiByte(CP_ACP,0,strString,strString.GetLength() ,pszSQL,iLen + 1 ,NULL,NULL);
			return pszSQL;
		}
	*/
	int iLen = strString.GetLength();
	char* pszSQL = NULL;
	if (iLen > 0)
	{
		pszSQL = new char[iLen + 1];
		if (NULL != pszSQL)
		{
			memset(pszSQL, 0, iLen + 1);
			CString strTemp(strString);
			strcpy(pszSQL, strTemp.GetBuffer(0));
		}
	}
	return pszSQL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSQLite3::AutoShrinkageDatabase
//
// ���������������Զ��������ݿ�
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2014��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CSQLite3::AutoShrinkageDatabase(void)
{
	BOOL bResult = FALSE;
	bResult = ExecuteSQL(_T("PRAGMA auto_vacuum = 1;"));
	return bResult;
}





