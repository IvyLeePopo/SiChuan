/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadBase.cpp
 * 摘    要：车道软件内部线程类统一继承接口，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月11日
 */

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadBase.h"

// CThreadBase

IMPLEMENT_DYNCREATE(CThreadBase, CWinThread)

CThreadBase::CThreadBase()
	:m_bExit(FALSE)
	,m_hMainWnd(NULL)
	,m_bWorking(FALSE)
	,m_strLastErrorDesc(_T(""))
	,m_strPathLocalConfigFile(_T(""))
{
	memset(&m_sTranstParam,0,sizeof(tagTranstParam));
}

CThreadBase::~CThreadBase()
{
}

BOOL CThreadBase::InitInstance()
{
	//初始化互锁变量
//	::InitializeCriticalSection(&m_cs);
	return TRUE;
}

int CThreadBase::ExitInstance()
{
	//清除互锁变量
//	::DeleteCriticalSection(&m_cs);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadBase, CWinThread)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadBase::OnIdle
//     
// 功能描述：基本线程类空闲处理函数
//     
// 输入参数：LONG lCount
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2015年11月11日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadBase::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}
	Sleep(1);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadBase::SetParam
//     
// 功能描述：获取基本配置信息
//     
// 输入参数：IN const tagTranstParam* pTranstParam		--	车道配置信息传递结构体
//         ：IN const HWND& hMainWnd					--	主窗体句柄
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadBase::SetParam(IN const tagTranstParam* pTranstParam,IN const HWND& hMainWnd)
{
	memcpy(&m_sTranstParam,pTranstParam,sizeof(tagTranstParam));

	m_hMainWnd = hMainWnd;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadBase::IsAllowSendMsgToUI
//     
// 功能描述：判断是否允许向主UI窗体发送消息
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CThreadBase::IsAllowSendMsgToUI(IN const HWND& hWnd)
{
	BOOL bReturn = FALSE;
	HWND hCheckWnd = NULL;
	if(!::IsWindow(hWnd))
		hCheckWnd = (NULL != GetGlobalApp()->m_pMainWnd)?GetGlobalApp()->m_pMainWnd->m_hWnd:NULL;
	else
		hCheckWnd = hWnd;
	if(    !m_bExit
		&& (NULL != GetGlobalApp()->m_pMainWnd)
		&& (::IsWindow(hCheckWnd))
		)
	{
		bReturn = TRUE;
	}
	return bReturn;
}