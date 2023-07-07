/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadDevice.h
 * ժ    Ҫ��Ӳ�������̣߳������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��7��23��
 */
#pragma once
#include "ThreadBase.h"
#include "HTSDDeviceDll.h"
#include "NFCProtocol.h"
#include "DVXMLUnipay.h"

// CThreadDevice_ForDebug

class CThreadDevice_ForDebug : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadDevice_ForDebug)

public://protected --> public ����δ֪��ԭ��ExitInstance���߳��˳���ʱ��û�б�ִ�У������Ҫ�ֶ�������Դ��
	CThreadDevice_ForDebug();           // protected constructor used by dynamic creation
	virtual ~CThreadDevice_ForDebug();

public:
	CString debug_str;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

};


