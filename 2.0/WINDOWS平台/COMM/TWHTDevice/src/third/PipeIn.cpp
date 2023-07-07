/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: PipeIn.cpp
* 摘   要: 此模块用于接收通讯数据，
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-06-10
* 修改记录:
*/
#include "stdafx.h"
#include "PipeIn.h"
#include "ThreadHelper.h"
#include <algorithm>
#include "TypeHelper.h"
#include "RawFrameConv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNCREATE(CPipeIn, CWinThread)
CPipeIn::CPipeIn()
{
	coreRead = NULL;
	this->m_bExit = false;	
	::InitializeCriticalSection(&mComReadMutex);
	::InitializeCriticalSection(&mCmdsMutex);
}
CPipeIn::~CPipeIn()
{
	Stop();
	::DeleteCriticalSection(&mComReadMutex);
	::DeleteCriticalSection(&mCmdsMutex);
}

BEGIN_MESSAGE_MAP(CPipeIn, CWinThread)
	ON_THREAD_MESSAGE(ON_COM_RECEIVE, CPipeIn::OnDataReceived)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// 函数名称:CPipeIn::SetPipe
//
// 功能描述:设置输入源
//
// 输入参数:CSerialPort* outer,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CPipeIn::SetPipe(CSerialPort* outer)
{
	coreRead = outer;
	if (coreRead)
	{
		coreRead->SetThreadOwner(this->m_nThreadID);
	}
	return true;
}
//////////////////////////////////////////////////////////////
// 函数名称:CPipeIn::ReadOneCmd
//
// 功能描述:从输入源中，获取一个输入
//
// 输入参数:TWProtocol_BaseFrame& dst,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CPipeIn::ReadOneCmd(TWProtocol_BaseFrame& dst)
{
	CScopeLock cmdL(&mCmdsMutex);
	if(mUnreadCmds.empty())
	{
		return false;
	}
	dst = mUnreadCmds.front();
	mUnreadCmds.pop_front();
	return true;
}


BOOL CPipeIn::InitInstance()
{
	return true;
}
int CPipeIn::ExitInstance()
{
	return CWinThread::ExitInstance();
}
BOOL CPipeIn::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return false;
	}
	ComDataProc();
	Sleep(10);
	return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:CPipeIn::ComDataProc
//
// 功能描述:从串口数据中，获取一个转换的数据
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
void CPipeIn::ComDataProc()
{
	CScopeLock readL(&mComReadMutex);
	CScopeLock cmdL(&mCmdsMutex);
	TWProtocol_BaseFrame newFrame, replyFrame;
	for(std::list<std::list<unsigned char> >::iterator it = mCompleteComData.begin();it!=mCompleteComData.end();++it)
	{
		if (RawDataToFrame(*it, newFrame, replyFrame))
		{
			mUnreadCmds.push_back(newFrame);
		}
	}
	mCompleteComData.clear();
}
void CPipeIn::Stop()
{
	TW_TRACE_DIARY_TYPE("停止数据接收线程" , TW_DIARY_MSG_THREAD_MAIN);
	if (this->m_bExit)
	{
		TW_TRACE_DIARY_TYPE("数据接收线程已经停止" , TW_DIARY_MSG_THREAD_MAIN);
		return ;
	}
	this->m_bExit = true;
	::PostThreadMessage(this->m_nThreadID, WM_QUIT, 0, 0);
	//::WaitForSingleObject(this->m_hThread,INFINITE);//直接使用等待可能无限等待，故使用下面的方法MsgWaitForMultipleObjects
	DWORD dRet;
	MSG msg;
	TW_TRACE_DIARY_TYPE("等待数据接收线程结束" , TW_DIARY_MSG_THREAD_MAIN);
	while (1)
	{
		dRet = ::MsgWaitForMultipleObjects(1, &this->m_hThread, FALSE, INFINITE, QS_ALLINPUT);

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
	TW_TRACE_DIARY_TYPE("数据接收线程停止成功" , TW_DIARY_MSG_THREAD_MAIN);
}
//////////////////////////////////////////////////////////////
// 函数名称:CPipeIn::OnDataReceived
//
// 功能描述:用于接收串口数据的消息通知，
//
// 输入参数:WPARAM wParam, LPARAM lParam,
// 输出参数:void 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CPipeIn::OnDataReceived(WPARAM wParam, LPARAM lParam)
{
	CScopeLock tmpL(&mComReadMutex);

	std::list<unsigned char> tmpArr;

	char data[2000] = {0};
	int len = coreRead->ComRead(data, 1000);
	while (len > 0)
	{
		std::copy(data, data + len, std::back_insert_iterator<std::list<unsigned char>>(tmpArr));
		len = coreRead->ComRead(data, 1000);
	}
	mLastData.insert(mLastData.end(), tmpArr.begin(), tmpArr.end());
	bool has_new_data = false;
	/*提取完整的数据信息。即DF ---------FD*/
	while (true)
	{
		if (mLastData.empty())
		{
			break;
		}
		std::list<unsigned char>::iterator pos = std::find(mLastData.begin(), mLastData.end(), TWPD_STARTFLAG);
		if (pos == mLastData.end())
		{
			break;
		}
		else if (pos != mLastData.begin()) /*第一个元素不是协议的开头数据，需要删除*/
		{
			std::list<unsigned char> unuseData;
			unuseData.insert(unuseData.end(), mLastData.begin(), pos);
			std::string log_info = "删除无效数据" + Bin2Hex(unuseData);
			TW_TRACE_DIARY(log_info);
			//std::list<unsigned char>::iterator tmp_it = pos ;
			//std::advance(tmp_it, 1); /**/
			
			mLastData.erase(mLastData.begin(), pos/*tmp_it*/);


			//TW_TRACE_DIARY(Bin2Hex(mLastData));
			continue;
		}
		std::list<unsigned char>::iterator end_pos = std::find(mLastData.begin(), mLastData.end(), TWPD_ENDFLAG);
		if (end_pos == mLastData.end())
		{
			break;
		}
		std::list<unsigned char> oneData;
		std::list<unsigned char>::iterator realEndPos = end_pos;
		std::advance(realEndPos, 1);
		oneData.insert(oneData.end(), mLastData.begin(), realEndPos);
		mCompleteComData.push_back(oneData);
		mLastData.erase(mLastData.begin(), realEndPos);
		std::string info = "收到设备数据(" + Int2Str((int)oneData.size()) + "字节)" + Bin2Hex(oneData);
        TW_TRACE_DIARY_TYPE(info, TW_DIARY_MSG_FRAME_RECV);
	}
}