/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadDevice.cpp
 * 摘    要：硬件控制线程，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年7月23日
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
