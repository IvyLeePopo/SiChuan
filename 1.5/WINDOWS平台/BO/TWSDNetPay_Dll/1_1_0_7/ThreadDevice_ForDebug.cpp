/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadDevice.cpp
 * ժ    Ҫ��Ӳ�������̣߳�ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��7��23��
 */
// ThreadDevice.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadDevice_ForDebug.h"

// CThreadDevice_ForDebug

IMPLEMENT_DYNCREATE(CThreadDevice_ForDebug, CThreadBase)

CThreadDevice_ForDebug::CThreadDevice_ForDebug()	
{

}


CThreadDevice_ForDebug::~CThreadDevice_ForDebug()
{

}

BOOL CThreadDevice_ForDebug::InitInstance()
{
	return TRUE;
}

int CThreadDevice_ForDebug::ExitInstance()
{

	return CWinThread::ExitInstance();
}



// CThreadDevice_ForDebug message handlers

BOOL CThreadDevice_ForDebug::OnIdle(LONG lCount)
{
	//GetGlobalApp()->RecordLog("OnIdle-------");
	if(m_bExit)
	{
		
		//MessageBox(0,"CThreadDevice_ForDebug","",0);

		::PostQuitMessage(0);
		return FALSE;
	}

	Sleep(100);
	return  CThreadBase::OnIdle(lCount);
}
