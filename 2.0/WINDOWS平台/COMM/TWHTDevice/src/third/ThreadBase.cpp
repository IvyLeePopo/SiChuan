/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadBase.cpp
 * ժ    Ҫ����������ڲ��߳���ͳһ�̳нӿڣ�ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��11��
 */

#include "stdafx.h"

#include "ThreadBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CThreadBase

IMPLEMENT_DYNCREATE(CThreadBase, CWinThread)

CThreadBase::CThreadBase()
	:m_bExit(FALSE)
	,m_hMainWnd(NULL)
	,m_nMainThreadID(0)
	,m_bWorking(FALSE)
	,m_strLastErrorDesc(_T(""))
	,m_strPathLocalConfigFile(_T(""))
{
	//memset(&m_sTranstParam,0,sizeof(tagTranstParam));
}

CThreadBase::~CThreadBase()
{
}

BOOL CThreadBase::InitInstance()
{
	//��ʼ����������
//	::InitializeCriticalSection(&m_cs);
	return TRUE;
}

int CThreadBase::ExitInstance()
{
	//�����������
//	::DeleteCriticalSection(&m_cs);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadBase, CWinThread)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadBase::OnIdle
//     
// ���������������߳�����д�����
//     
// ���������LONG lCount
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��11��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CThreadBase::SetParam
//     
// ������������ȡ����������Ϣ
//     
// ���������IN const tagTranstParam* pTranstParam		--	����������Ϣ���ݽṹ��
//         ��IN const HWND& hMainWnd					--	��������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
/*void CThreadBase::SetParam(IN const tagTranstParam* pTranstParam,IN const HWND& hMainWnd,IN const UINT& nMainThreadID)
{
	memcpy(&m_sTranstParam,pTranstParam,sizeof(tagTranstParam));

	m_nMainThreadID = nMainThreadID;
	m_hMainWnd		= hMainWnd;
}
*/ 