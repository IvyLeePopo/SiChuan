/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�TableDebitCancel.h
 * ժ    Ҫ���������ݺ��������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��25��
 */

#pragma once
#include "SQLite3Base.h"
#include "DVDebitCancel.h"


class CTableDebitCancel
{
public:
	CTableDebitCancel(void);
	~CTableDebitCancel(void);
//Operations
public:
	//�������ݿ����·��
	inline void SetDBFilePath(IN const CString& strFilePath){m_strSQLiteDBFilePath = strFilePath;}
	//���ó������ݻ���ʱ�䣨��λ�����ӣ�
	inline void SetLastDelayTime(IN const int& iLastDelayTime){m_llLastDelayTime = iLastDelayTime;}
	//����һ����������
	const BOOL Save(IN const CDVDebitCancel& cDebitCancle);
	//ɾ��һ����������(���ݴ�����)
	const BOOL Delete(void);
	//����һ����������
	const BOOL Retrieval(IN OUT CDVDebitCancel& cDebitCancle);
	//����һ���������ݵĴ�����
	const BOOL UpdateProcessMark(IN const CString& PayIdentifier);
private:
	//��ʼ�����ݿ�
	const BOOL	  InitDB(IN const CString& strDBPath);
	//�ر����ݿ�
	const BOOL	  CloseDB(void);	
	//������
	const BOOL CreateTable(void);
	//��ȡ������ϸ��
	const BOOL ReadItem(IN OUT CDVDebitCancel& cData);
	//��ȡ��¼У����
	const CString GetVerifyCode(IN const CDVDebitCancel& cData);
//Attributes
public:
	//���ݿ��Ƿ��Ѿ���ʼ��
	BOOL	m_bInitDatabase;
	//���泷��ʱ��(��λ������)
	LONGLONG	m_llLastDelayTime;
	//���ݿ���
	CSQLite3* m_pSQLite3DB;
	//���ݱ���
	CString m_strProcessTableName;
	//���µĴ���������Ϣ
	CString m_strLastErrorDesc;
	//�����ļ�����·��
	CString m_strSQLiteDBFilePath;
};
