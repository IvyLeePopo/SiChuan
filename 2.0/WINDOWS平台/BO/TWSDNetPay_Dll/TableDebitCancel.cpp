/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�TableDebitCancel.cpp
 * ժ    Ҫ��
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��25��
 */
#include "StdAfx.h"
#include "TableDebitCancel.h"

CTableDebitCancel::CTableDebitCancel(void)
	:m_bInitDatabase(FALSE)
	,m_pSQLite3DB(NULL)
	,m_strProcessTableName(_T(""))
	,m_strLastErrorDesc(_T(""))
	,m_strSQLiteDBFilePath(_T(""))
{
	m_pSQLite3DB = new CSQLite3();
	ASSERT(m_pSQLite3DB);
}

CTableDebitCancel::~CTableDebitCancel(void)
{
	if(NULL != m_pSQLite3DB)
	{
		CloseDB();

		delete m_pSQLite3DB;
		m_pSQLite3DB = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::Save
//     
// ��������������һ����������
//     
// ���������IN const CDVDebitCancel& cDebitCancle
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::Save(IN const CDVDebitCancel& cDebitCancle)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//�ж��Ƿ��кϷ������ݱ�
		if(!CreateTable())
			throw -1;

		//����������
		strSQL.Format(_T("INSERT INTO %s (ProvinceID,RoadID,StationID,ProcessMark,PayIdentifier,CreateTime,ExecuteTime)VALUES(%d,%d,%d,%d,'%s','%s','%s')"),m_strProcessTableName,
			cDebitCancle.ProvinceID,
			cDebitCancle.RoadID,
			cDebitCancle.StationID,
			cDebitCancle.ProcessMark,
			cDebitCancle.PayIdentifier,
			cDebitCancle.CreateTime.Format(_T("")),
			cDebitCancle.ExecuteTime.Format(_T("")));

		//ִ�����ݲ���SQL���
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
		{
			throw -2;
		}

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�����ںϷ������ݱ�
			break;
		case -2://�������ݲ���ʧ��
			break;
		}
	}
	//�ر����ݿ�
	CloseDB();
	//���ؽ��
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::Delete
//     
// ����������ɾ��һ����������(���ݴ�����)
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::Delete(void)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//�ж��Ƿ��кϷ������ݱ�
		if(!CreateTable())
			throw -1;

		//����ɾ�����
		strSQL.Format(_T("DELETE FROM %s WHERE ProcessMark=1 "),m_strProcessTableName);

		//ִ������ɾ��SQL���
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -2;

		bReturn = TRUE;

		//����������
		strSQL.Format(_T("SELECT * FROM %s WHERE ProcessMark=0 LIMIT 1"),m_strProcessTableName);

		//ִ�����ݼ���SQL���
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -3;
		//�ж��Ƿ���Ҫɾ���ļ�
		if(0 == m_pSQLite3DB->EffectRows(strSQL))
		{
			CString strFilePath = m_strSQLiteDBFilePath;
			m_pSQLite3DB->CloseConnection();
			//ɾ���ļ�
			if(CloseDB())
			{
				DeleteFile(strFilePath);
			}
		}	
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�����ںϷ������ݱ�
			break;
		case -2://ִ������ɾ������ʧ��
			break;
		case -3://ִ�����ݼ�������ʧ��
			break;
		}
	}
	//�ر����ݿ�
	CloseDB();
	//���ؽ��
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::Retrieval
//     
// ��������������һ����������
//     
// ���������IN OUT CDVDebitCancel& cDebitCancle
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::Retrieval(IN OUT CDVDebitCancel& cDebitCancle)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//�ж��Ƿ��кϷ������ݱ�
		if(!CreateTable())
			throw -1;

		//����������
		strSQL.Format(_T("SELECT * FROM %s WHERE LENGTH(PayIdentifier)>0 AND ProcessMark=0 ORDER BY CreateTime LIMIT 1"),m_strProcessTableName);

		//ִ�����ݼ���SQL���
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -2;
		
		//�жϼ�¼���Ƿ��������
		if(0 == m_pSQLite3DB->EffectRows(strSQL))
			throw -3;

		//��ȡ����
		if(m_pSQLite3DB->IsEOF())
			throw -3;

		//��ȡ������ϸ��
		if(!ReadItem(cDebitCancle))
			throw -4;

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�����ںϷ������ݱ�
			break;
		case -2://ִ�����ݼ�������ʧ��
			break;
		case -3://�޷���������Ч����
			break;
		case -4://��ȡ������ϸ�����
			break;
		}
	}
	//�ر����ݿ�
	CloseDB();
	//���ؽ��
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::UpdateProcessMark
//     
// ��������������һ���������ݵĴ�����
//     
// ���������IN const CString& PayIdentifier
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::UpdateProcessMark(IN const CString& PayIdentifier)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//�ж��Ƿ��кϷ������ݱ�
		if(!CreateTable())
			throw -1;

		//����ɾ�����
		strSQL.Format(_T("UPDATE %s SET ProcessMark=1 WHERE PayIdentifier='%s' "),m_strProcessTableName,PayIdentifier);

		//ִ�����ݸ���SQL���
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -2;

		bReturn = TRUE;

		//����������
		strSQL.Format(_T("SELECT * FROM %s WHERE ProcessMark=0 LIMITE 1"),m_strProcessTableName);

		//ִ�����ݼ���SQL���
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -3;
		//�ж��Ƿ���Ҫɾ���ļ�
		if(0 == m_pSQLite3DB->EffectRows(strSQL))
		{
			CString strFilePath = m_strSQLiteDBFilePath;
			m_pSQLite3DB->CloseConnection();
			//ɾ���ļ�
			if(CloseDB())
			{
				DeleteFile(strFilePath);
			}
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�����ںϷ������ݱ�
			break;
		case -2://ִ������ɾ������ʧ��
			break;
		case -3://ִ�����ݼ�������ʧ��
			break;
		}
	}
	//�ر����ݿ�
	CloseDB();
	//���ؽ��
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::InitDB
//     
// ������������ʼ�����ݿ�
//     
// ���������IN const CString& strDBPath
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::InitDB(IN const CString& strDBPath)
{
	//����µ����ݿ�
	m_strSQLiteDBFilePath = strDBPath;
	if(m_pSQLite3DB->IsOpenConnection())
	{
		m_pSQLite3DB->CloseConnection();
	}
	//���´����ݿ�
	m_bInitDatabase = m_pSQLite3DB->OpenConnection(m_strSQLiteDBFilePath);
	return m_bInitDatabase;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::CloseDB
//     
// �����������ر����ݿ�
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::CloseDB(void)
{
	BOOL bReturn = FALSE;
	if(NULL != m_pSQLite3DB) 
	{
		m_pSQLite3DB->CloseRecordSet();

		bReturn = m_pSQLite3DB->CloseConnection();

		m_bInitDatabase = !bReturn;
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::CreateTable
//     
// ����������������
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::CreateTable(void)
{
	BOOL bReturn = FALSE;

	CString strSQL;
	try
	{
		//�ж����ݿ�ָ���Ƿ�Ƿ�
		if(NULL == m_pSQLite3DB)
			throw -1;
		//�ж����ݿ�״̬�Ƿ����������������³�ʼ�����ݿ�
		if(!m_bInitDatabase)
		{
			if(!InitDB(m_strSQLiteDBFilePath))
			{
				throw -2;
			}
		}

		//���촴�����SQL���
		strSQL.Format(_T("CREATE TABLE %s (ProvinceID int NOT NULL,RoadID int NOT NULL,StationID int NOT NULL,ProcessMark int NOT NULL,PayIdentifier varchar(64) NOT NULL,CreateTime varchar(64) NOT NULL,ExecuteTime varchar(64) NOT NULL,PRIMARY KEY(PayIdentifier))"),m_strProcessTableName);

		//���Դ������ݱ�
		if(!m_pSQLite3DB->CheckTableExists(m_strProcessTableName))
		{
			if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			{
				throw -3;
			}
		}

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://���ݿ�ָ��Ƿ�
			break;
		case -2://���ݿ�δ��������ʼ��
			break;
		case -3://�������ݱ�ʧ��
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CTableDebitCancel::ReadItem
//     
// ������������ȡ������ϸ��
//     
// ���������IN OUT cDebitCancle& cData
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::ReadItem(IN OUT CDVDebitCancel& cData)
{
	BOOL bReturn = FALSE;
	try
	{
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("ProvinceID"),cData.ProvinceID))
			throw -4;
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("RoadID"),cData.RoadID))
			throw -4;
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("StationID"),cData.StationID))
			throw -4;
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("ProcessMark"),cData.ProcessMark))
			throw -4;
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("PayIdentifier"),cData.PayIdentifier))
			throw -4;
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("CreateTime"),cData.CreateTime))
			throw -4;
		//��ȡ������ϸ��
		if(!m_pSQLite3DB->GetColumnValue(_T("ExecuteTime"),cData.ExecuteTime))
			throw -4;

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�����ںϷ������ݱ�
			break;
		case -4://��ȡ������ϸ�����
			break;
		}
	}
	return bReturn;
}

