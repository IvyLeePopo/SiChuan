/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: CommondDak.cpp
* ժ   Ҫ:
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-06-10
* �޸ļ�¼:
*/
#include "stdafx.h"
#include "CommondDak.h"
#include "ThreadHelper.h"
#include "WorkingPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CCommondDak, CWinThread)

CCommondDak::CCommondDak()
{
	SetDefault();
	::InitializeCriticalSection(&mCmdsMutex);
	::InitializeCriticalSection(&mResultMutex);
	if (! mIOCore.ComOpen(mPortNumber))
	{
		TW_TRACE_DIARY_TYPE("�򿪴���ʧ��:" + Int2Str(mPortNumber), TW_DIARY_MSG_THREAD_API);
		TW_TRACE_DIARY_TYPE("windows����:" + Int2Str(GetLastError()), TW_DIARY_MSG_THREAD_API);
	}

	mpCmdIn = (CPipeIn*)::AfxBeginThread(RUNTIME_CLASS(CPipeIn), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (mpCmdIn)
	{
		TW_TRACE_DIARY_TYPE("��ʼ��������߳�...", TW_DIARY_MSG_THREAD_API);
		mpCmdIn->m_bAutoDelete = false;
		mpCmdIn->ResumeThread();
		mpCmdIn->SetPipe(&mIOCore);
		TW_TRACE_DIARY_TYPE("������������߳����", TW_DIARY_MSG_THREAD_API);
	}
	else
	{
		TW_TRACE_DIARY_TYPE("������մ���ʧ��", TW_DIARY_MSG_THREAD_API);
	}

	mpCmdOut = (CPipeOut*)::AfxBeginThread(RUNTIME_CLASS(CPipeOut), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (mpCmdOut)
	{
		TW_TRACE_DIARY_TYPE("��ʼ������߳�...", TW_DIARY_MSG_THREAD_API);
		mpCmdOut->m_bAutoDelete = false;
		mpCmdOut->ResumeThread();
		mpCmdOut->SetPipe(&mIOCore);
		TW_TRACE_DIARY_TYPE("����������߳����", TW_DIARY_MSG_THREAD_API);
	}
	else
	{
		TW_TRACE_DIARY_TYPE("����ʹ���ʧ��", TW_DIARY_MSG_THREAD_API);
	}

}
CCommondDak::~CCommondDak()
{
	Stop();
	delete mpCmdIn;
	mpCmdIn = NULL;
	delete mpCmdOut;
	mpCmdOut = NULL;
	::DeleteCriticalSection(&mCmdsMutex);
	::DeleteCriticalSection(&mResultMutex);
}
//////////////////////////////////////////////////////////////
// ��������:CCommondDak::CalcCmdKey
//
// ��������:�˺��������ڼ���ĳ�����ݵ�key����key����Ψһ��ʾĳ�����ݣ������޵�ʱ�䷶Χ�ڣ�
//
// �������:const TWProtocol_BaseFrame& info,
// �������:TWCmdKey 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-19
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
TWCmdKey CCommondDak::CalcCmdKey(const TWProtocol_BaseFrame& info)
{
	TWCmdKey crk = (info.SN<<16) | (info.TOPCMD << 8) | info.SUBCMD;
	return crk;
}
//////////////////////////////////////////////////////////////
// ��������:CCommondDak::CalcRespondKey
//
// ��������:���ڼ���ĳ�����ݵĻظ����ݶ�Ӧ��key���ûظ����ݲ����ǽ���Ӧ��C0��
//
// �������:const TWProtocol_BaseFrame& info,
// �������:TWCmdKey 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-19
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
TWCmdKey CCommondDak::CalcRespondKey(const TWProtocol_BaseFrame& info)
{
	TWProtocol_BaseFrame replyInfo = info;
	if (info.TOPCMD >= 0xA1 && info.TOPCMD <= 0xAF)
	{
		replyInfo.TOPCMD = info.TOPCMD + 0x10;
	}
	else if (info.TOPCMD >= 0xD1 && info.TOPCMD <= 0xD2)
	{
		replyInfo.TOPCMD = info.TOPCMD + 0x10;
	}
	replyInfo.SN = 0;//�ն˲�֧��SN
	return CalcCmdKey(replyInfo);
}
//////////////////////////////////////////////////////////////
// ��������:CCommondDak::SendCmd
//
// ��������:���һ��������Ͷ�����
//
// �������:const TWProtocol_BaseFrame& info,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CCommondDak::SendCmd(const TWProtocol_BaseFrame& info)
{
	CmdDataInfo cdi;
	cdi.data = info;
	cdi.crk = CalcCmdKey(info);
	CScopeLock tmpL(&mCmdsMutex);
	mCmdQueue.push_back(cdi);	
	return true;
}

bool CCommondDak::SendVIPCmd(const TWProtocol_BaseFrame& info)
{
	CmdDataInfo cdi;
	cdi.data = info;
	cdi.crk = CalcCmdKey(info);
	CScopeLock tmpL(&mCmdsMutex);
	mCmdQueue.push_front(cdi);
	return true;
}

bool CCommondDak::SendSelfCmd(const std::string& cmdInfo)
{
	CScopeLock tmpL(&mCmdsMutex);
	m_SelfSendQueue.push_back(cmdInfo);
	return true;
}

//////////////////////////////////////////////////////////////
// ��������:CCommondDak::TakeCmd
//
// ��������:�ڴ˴���ȡһ������֡���ɹ�����True��ʧ�ܷ���false
//
// �������:TWProtocol_BaseFrame&info,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CCommondDak::TakeCmd(TWProtocol_BaseFrame&info)
{
	CScopeLock tmpL(&mResultMutex);
	if (mResultQueue.empty())
	{
		return false;
	}
	info = mResultQueue.front();
	mResultQueue.pop_front();
	return true;
}

void CCommondDak::SetDefault()
{
	m_bExit = false;
	mPortNumber = (int)SystemParaComPort();
	mLastReadTime = time(NULL);
}
//////////////////////////////////////////////////////////////
// ��������:CCommondDak::QuerySendResult
//
// ��������:��ѯĳ������ķ��ͽ�������ͳɹ����߷���ʧ��
//
// �������:TWCmdKey crk,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CCommondDak::QuerySendResult(TWCmdKey crk )
{
	CScopeLock tmpL(&mCmdsMutex);
	for (std::list<CmdDataInfo>::const_iterator it = mCmdQueue.begin(); it != mCmdQueue.end(); ++it)
	{
		if (it->crk == crk && it->sendSuccess)
		{
			return true;
		}
	}
	return false;
}
BOOL CCommondDak::InitInstance()
{
	return true;
}
int CCommondDak::ExitInstance()
{
	return CWinThread::ExitInstance();
}
BOOL CCommondDak::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return false;
	}
	SendProc();
	RecvProc();
	ClearCmd();
	CheckComStatus();
	Sleep(10);
	return true;
}

//////////////////////////////////////////////////////////////
// ��������:CCommondDak::Stop
//
// ��������:������ֹ��ͨѶ�߳�
//
// �������:
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void CCommondDak::Stop()
{
	TW_TRACE_DIARY_TYPE("ֹͣͨѶ�߳�" , TW_DIARY_MSG_THREAD_MAIN);
	if (this->m_bExit)
	{
		TW_TRACE_DIARY_TYPE("ͨѶ�߳��Ѿ�ֹͣ" , TW_DIARY_MSG_THREAD_MAIN);
		return ;
	}
	this->m_bExit = true;
	::PostThreadMessage(this->m_nThreadID, WM_QUIT, 0, 0);
	//::WaitForSingleObject(this->m_hThread,INFINITE);//ֱ��ʹ�õȴ��������޵ȴ�����ʹ������ķ���MsgWaitForMultipleObjects
	DWORD dRet;
	MSG msg;
	TW_TRACE_DIARY_TYPE("�ȴ�ͨѶ�߳̽���" , TW_DIARY_MSG_THREAD_MAIN);
	while (1)
	{
		dRet =::MsgWaitForMultipleObjects(1, &this->m_hThread, FALSE, INFINITE, QS_ALLINPUT);

		if (dRet == WAIT_OBJECT_0 + 1)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			break;
		}
	}
	TW_TRACE_DIARY_TYPE("ͨѶ�߳�ֹͣ�ɹ�" , TW_DIARY_MSG_THREAD_MAIN);
}
//////////////////////////////////////////////////////////////
// ��������:CCommondDak::ClearCmd
//
// ��������:�˴�������������ʹ����ﵽҪ������Ѿ����ͳɹ�������
//
// �������:
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void CCommondDak::ClearCmd()
{
	static time_t last_check_time = 0;
	time_t currTime = time(NULL);
	if ( (currTime - last_check_time)  < 10 )
	{
		return ;
	}
	last_check_time = currTime;
	CScopeLock tmpL(&mCmdsMutex);
	while (!mCmdQueue.empty())
	{
		//TW_TRACE_DIARY("cmdsize:"+Int2Str(mCmdQueue.size()));
		if ( (currTime - mCmdQueue.front().createTime) > 60*60)//ĳ��֡�ĳ�ʱʱ�䣬����Ϊ1Сʱ
		{
			mCmdQueue.pop_front();
			continue;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////
// ��������:CCommondDak::SendProc
//
// ��������:�����Ͷ����е����ݣ����������߳�
//
// �������:
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void CCommondDak::SendProc()
{
	CScopeLock tmpL(&mCmdsMutex);

	//������������
	while(!m_SelfSendQueue.empty())
	{
		std::string rawCmd = m_SelfSendQueue.front();
		RemoveWhiteSpaceInStr(rawCmd);
		m_SelfSendQueue.pop_front();

		mpCmdOut->WriteOneSelfCmd(rawCmd);
	}
	
	for (std::list<CmdDataInfo>::iterator it = mCmdQueue.begin(); it != mCmdQueue.end(); ++it)
	{
		if (it->sendSuccess || it->sendCount >= 3)
		{
			continue;
		}
		time_t currT = time(NULL);
		if ( (currT - it->lastSendTime) > 1 )
		{
		
			mpCmdOut->WriteOneCmd(it->data);
			it->lastSendTime = currT;
			it->sendCount++;
			if(it->data.TOPCMD == TWPC_C_REPLY)
			{
				it->sendCount = 3;//
			}
		}
		break;

	}
}

//////////////////////////////////////////////////////////////
// ��������:CCommondDak::RecvProc
//
// ��������:��ȡ�����߳��е��������ݡ�
//
// �������:
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void CCommondDak::RecvProc()
{
	TWProtocol_BaseFrame tmpF;
	CScopeLock recvL(&mResultMutex);
	while (mpCmdIn->ReadOneCmd(tmpF))
	{
		mLastReadTime = time(NULL);
		if (tmpF.TOPCMD == TWPC_C_REPLY && tmpF.SUBCMD == TWPC_C_REPLY_SUCCESS)
		{
			//Ĭ���������ǰ���ָ���
			CScopeLock tmpL(&mCmdsMutex);
			for (std::list<CmdDataInfo>::iterator it = mCmdQueue.begin(); it != mCmdQueue.end(); ++it)
			{
				if (it->sendSuccess || it->sendCount >= 3)
				{
					continue;
				}
				time_t currT = time(NULL);
				if ( (currT - it->lastSendTime) < 2 )
				{
					it->sendSuccess = true;
				}
				break;
			}

		}
		else if(tmpF.TOPCMD == TWPC_C_REPLY )
		{//����ʧ��
		}
		else if((tmpF.TOPCMD==TWPC_B_BEAT && tmpF.SUBCMD ==  TWPC_B_BEAT_AUTO) )
		{
			//not rely c0 and b1 
			//���ڴ������Ϣ֡Ҳ����ظ�
			mResultQueue.push_back(tmpF);
		}
		else
		{
			mResultQueue.push_back(tmpF);
			TWProtocol_BaseFrame c0Write(TWPC_C_REPLY,TWPC_C_REPLY_SUCCESS,0);
			c0Write.LEN = 4;
			c0Write.DATA = new unsigned char[4];
			c0Write.DATA[0] = tmpF.TOPCMD;
		    c0Write.DATA[1] = tmpF.SUBCMD;
		    c0Write.DATA[2] = HIBYTE(0);
		    c0Write.DATA[3] = LOBYTE(0);
			CScopeLock wL(&mCmdsMutex);
			CmdDataInfo c0cmd;
			c0cmd.data = c0Write;
			mCmdQueue.push_back(c0cmd);	
		}
	}
}

//////////////////////////////////////////////////////////////
// ��������:CCommondDak::CheckComStatus
//
// ��������:���ͨѶ�ڵ�״̬����������һ��ʱ����û���յ���ЧЭ�����ݣ�������ô���
//
// �������:
// �������:BOOL 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
BOOL CCommondDak::CheckComStatus()
{
    static time_t lastCheckTime = time(NULL);
    time_t currTime = time(NULL);
    if ( (currTime - lastCheckTime) / 60 < 1)
    {
        return false;/*һ���Ӽ��һ��*/
    }
    lastCheckTime = currTime;
    //int lastReadTime = mLastReadTime;//mIOCore.GetLastReadTime();
    if ( (currTime - mLastReadTime) > 40 )
    {
        TW_TRACE_DIARY_TYPE("��ʱ��δ�յ��������ݣ����ô���", TW_DIARY_MSG_THREAD_MAIN);
        if (!mIOCore.ComOpen(mPortNumber))
        {
            TW_TRACE_DIARY_TYPE("�򿪴���ʧ��:" + Int2Str(mPortNumber), TW_DIARY_MSG_THREAD_MAIN);
        }
    }
    return true;

}