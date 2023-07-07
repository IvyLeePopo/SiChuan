/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: CommondDak.cpp
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-06-10
* 修改记录:
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
		TW_TRACE_DIARY_TYPE("打开串口失败:" + Int2Str(mPortNumber), TW_DIARY_MSG_THREAD_API);
		TW_TRACE_DIARY_TYPE("windows错误:" + Int2Str(GetLastError()), TW_DIARY_MSG_THREAD_API);
	}

	mpCmdIn = (CPipeIn*)::AfxBeginThread(RUNTIME_CLASS(CPipeIn), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (mpCmdIn)
	{
		TW_TRACE_DIARY_TYPE("开始命令接收线程...", TW_DIARY_MSG_THREAD_API);
		mpCmdIn->m_bAutoDelete = false;
		mpCmdIn->ResumeThread();
		mpCmdIn->SetPipe(&mIOCore);
		TW_TRACE_DIARY_TYPE("启动命令接收线程完毕", TW_DIARY_MSG_THREAD_API);
	}
	else
	{
		TW_TRACE_DIARY_TYPE("命令接收创建失败", TW_DIARY_MSG_THREAD_API);
	}

	mpCmdOut = (CPipeOut*)::AfxBeginThread(RUNTIME_CLASS(CPipeOut), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (mpCmdOut)
	{
		TW_TRACE_DIARY_TYPE("开始命令发送线程...", TW_DIARY_MSG_THREAD_API);
		mpCmdOut->m_bAutoDelete = false;
		mpCmdOut->ResumeThread();
		mpCmdOut->SetPipe(&mIOCore);
		TW_TRACE_DIARY_TYPE("启动命令发送线程完毕", TW_DIARY_MSG_THREAD_API);
	}
	else
	{
		TW_TRACE_DIARY_TYPE("命令发送创建失败", TW_DIARY_MSG_THREAD_API);
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
// 函数名称:CCommondDak::CalcCmdKey
//
// 功能描述:此函数，用于计算某个数据的key，该key可以唯一表示某个数据（在有限的时间范围内）
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:TWCmdKey 
// 编写人员:朱亚冰
// 编写时间:2017-07-19
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
TWCmdKey CCommondDak::CalcCmdKey(const TWProtocol_BaseFrame& info)
{
	TWCmdKey crk = (info.SN<<16) | (info.TOPCMD << 8) | info.SUBCMD;
	return crk;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCommondDak::CalcRespondKey
//
// 功能描述:用于计算某个数据的回复数据对应的key，该回复数据并不是接收应答（C0）
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:TWCmdKey 
// 编写人员:朱亚冰
// 编写时间:2017-07-19
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
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
	replyInfo.SN = 0;//终端不支持SN
	return CalcCmdKey(replyInfo);
}
//////////////////////////////////////////////////////////////
// 函数名称:CCommondDak::SendCmd
//
// 功能描述:添加一个命令到发送队列中
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
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
// 函数名称:CCommondDak::TakeCmd
//
// 功能描述:在此处获取一个数据帧，成功返回True，失败返回false
//
// 输入参数:TWProtocol_BaseFrame&info,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
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
// 函数名称:CCommondDak::QuerySendResult
//
// 功能描述:查询某个命令的发送结果，发送成功或者发送失败
//
// 输入参数:TWCmdKey crk,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
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
// 函数名称:CCommondDak::Stop
//
// 功能描述:用于终止此通讯线程
//
// 输入参数:
// 输出参数:void 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCommondDak::Stop()
{
	TW_TRACE_DIARY_TYPE("停止通讯线程" , TW_DIARY_MSG_THREAD_MAIN);
	if (this->m_bExit)
	{
		TW_TRACE_DIARY_TYPE("通讯线程已经停止" , TW_DIARY_MSG_THREAD_MAIN);
		return ;
	}
	this->m_bExit = true;
	::PostThreadMessage(this->m_nThreadID, WM_QUIT, 0, 0);
	//::WaitForSingleObject(this->m_hThread,INFINITE);//直接使用等待可能无限等待，故使用下面的方法MsgWaitForMultipleObjects
	DWORD dRet;
	MSG msg;
	TW_TRACE_DIARY_TYPE("等待通讯线程结束" , TW_DIARY_MSG_THREAD_MAIN);
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
	TW_TRACE_DIARY_TYPE("通讯线程停止成功" , TW_DIARY_MSG_THREAD_MAIN);
}
//////////////////////////////////////////////////////////////
// 函数名称:CCommondDak::ClearCmd
//
// 功能描述:此处用于清除，发送次数达到要求或者已经发送成功的命令
//
// 输入参数:
// 输出参数:void 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
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
		if ( (currTime - mCmdQueue.front().createTime) > 60*60)//某个帧的超时时间，设置为1小时
		{
			mCmdQueue.pop_front();
			continue;
		}
		break;
	}
}

//////////////////////////////////////////////////////////////
// 函数名称:CCommondDak::SendProc
//
// 功能描述:将发送队列中的数据，给到发送线程
//
// 输入参数:
// 输出参数:void 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCommondDak::SendProc()
{
	CScopeLock tmpL(&mCmdsMutex);

	//发送特殊数据
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
// 函数名称:CCommondDak::RecvProc
//
// 功能描述:获取接收线程中的数据内容。
//
// 输入参数:
// 输出参数:void 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
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
			//默认是针对最前面的指令的
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
		{//发送失败
		}
		else if((tmpF.TOPCMD==TWPC_B_BEAT && tmpF.SUBCMD ==  TWPC_B_BEAT_AUTO) )
		{
			//not rely c0 and b1 
			//对于错误的信息帧也不会回复
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
// 函数名称:CCommondDak::CheckComStatus
//
// 功能描述:检查通讯口的状态，当串口在一段时间内没有收到有效协议数据，则会重置串口
//
// 输入参数:
// 输出参数:BOOL 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
BOOL CCommondDak::CheckComStatus()
{
    static time_t lastCheckTime = time(NULL);
    time_t currTime = time(NULL);
    if ( (currTime - lastCheckTime) / 60 < 1)
    {
        return false;/*一分钟检测一次*/
    }
    lastCheckTime = currTime;
    //int lastReadTime = mLastReadTime;//mIOCore.GetLastReadTime();
    if ( (currTime - mLastReadTime) > 40 )
    {
        TW_TRACE_DIARY_TYPE("长时间未收到串口数据，重置串口", TW_DIARY_MSG_THREAD_MAIN);
        if (!mIOCore.ComOpen(mPortNumber))
        {
            TW_TRACE_DIARY_TYPE("打开串口失败:" + Int2Str(mPortNumber), TW_DIARY_MSG_THREAD_MAIN);
        }
    }
    return true;

}