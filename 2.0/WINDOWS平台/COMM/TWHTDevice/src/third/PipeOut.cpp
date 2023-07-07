/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: PipeOut.cpp
* 摘   要: 将输出发送到通讯口
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-06-10
* 修改记录:
*/
#include "stdafx.h"
#include "PipeOut.h"
#include "DataAuth.h"
#include "RawFrameConv.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPipeOut, CWinThread)

CPipeOut::CPipeOut()
{
	m_bExit = false;
}
CPipeOut::~CPipeOut()
{
	Stop();
}
//////////////////////////////////////////////////////////////
// 函数名称:CPipeOut::WriteOneCmd
//
// 功能描述:输出串口数据
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
bool CPipeOut::WriteOneCmd(const TWProtocol_BaseFrame& info)
{
	
	std::vector<unsigned char> beforeEscape, afterEscape;
	beforeEscape.push_back(HIBYTE(info.LEN));
	beforeEscape.push_back(LOBYTE(info.LEN));
	beforeEscape.push_back(HIBYTE(info.SN));
	beforeEscape.push_back(LOBYTE(info.SN));
	beforeEscape.push_back(info.TOPCMD);
	beforeEscape.push_back(info.SUBCMD);
	std::copy(info.DATA, info.DATA + info.LEN, std::back_insert_iterator<std::vector<unsigned char>>(beforeEscape));
	std::vector<unsigned char>::const_iterator it_first = beforeEscape.begin();
	std::vector<unsigned char>::const_iterator it_second = beforeEscape.begin();;
	std::advance(it_second, info.LEN + 6);

	unsigned short crcCode = crc16(it_first, it_second);
	beforeEscape.push_back(HIBYTE(crcCode));
	beforeEscape.push_back(LOBYTE(crcCode));

	if (!FrameEscape(beforeEscape, afterEscape))
	{
		return false;
	}
	afterEscape.insert(afterEscape.begin(), info.STX);
	afterEscape.push_back(info.ETX);

	unsigned char* newData = new unsigned char[afterEscape.size()];
	if (newData == NULL)
	{
		return false;
	}
	std::copy(afterEscape.begin(), afterEscape.end(), newData);
	
	std::string send_info = "发送数据(" + Int2Str(afterEscape.size()) + "字节):" + Bin2Hex(afterEscape);
	TW_TRACE_DIARY_TYPE(send_info,TW_DIARY_MSG_FRAME_SEND);
	coreWrite->ComWrite((const char*)newData, (int)afterEscape.size());

	delete newData;
	return true;
}
//////////////////////////////////////////////////////////////
// 函数名称:CPipeOut::SetPipe
//
// 功能描述:设置输出源
//
// 输入参数:CSerialPort* outereFra,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CPipeOut::SetPipe(CSerialPort* outereFra)
{
	coreWrite = outereFra;
	return true;
}
BOOL CPipeOut::InitInstance()
{
	return true;
}
int CPipeOut::ExitInstance()
{
	return CWinThread::ExitInstance();
}
BOOL CPipeOut::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return false;
	}

	Sleep(10);
	return true;
}
void CPipeOut::Stop()
{
	TW_TRACE_DIARY_TYPE("停止数据发送线程" , TW_DIARY_MSG_THREAD_MAIN);
	if (this->m_bExit)
	{
		TW_TRACE_DIARY_TYPE("数据发送线程已经停止" , TW_DIARY_MSG_THREAD_MAIN);
		return ;
	}
	this->m_bExit = true;
	::PostThreadMessage(this->m_nThreadID, WM_QUIT, 0, 0);
	//::WaitForSingleObject(this->m_hThread,INFINITE);//直接使用等待可能无限等待，故使用下面的方法MsgWaitForMultipleObjects
	DWORD dRet;
	MSG msg;
	TW_TRACE_DIARY_TYPE("等待数据发送线程结束" , TW_DIARY_MSG_THREAD_MAIN);
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
	TW_TRACE_DIARY_TYPE("数据发送线程停止成功" , TW_DIARY_MSG_THREAD_MAIN);
}

bool CPipeOut::WriteOneSelfCmd(const std::string& info)
{
	
	std::vector<unsigned char> beforeEscape, afterEscape;
	
	if(!Hex2Bin(info.c_str(),beforeEscape,info.size()))
	{
		std::string errInfo = "原始数据转换失败:"+info;
		TW_TRACE_DIARY_TYPE(errInfo,TW_DIARY_MSG_FRAME_SEND);
		return false;
	}

	//删除开头的DF 和FD
	if(beforeEscape.size() > 2)
	{
		beforeEscape.erase(beforeEscape.begin());
		beforeEscape.pop_back();
	}
	

	if (!FrameEscape(beforeEscape, afterEscape))
	{
		return false;
	}
	afterEscape.push_back(0xFD);
	afterEscape.insert(afterEscape.begin(),0xDF);

	int newDataLen = afterEscape.size();
	unsigned char* newData = new unsigned char[newDataLen];
	if (newData == NULL)
	{
		return false;
	}
	std::copy(afterEscape.begin(),afterEscape.end(),newData);

	std::string send_info = "发送数据(" + Int2Str(afterEscape.size()) + "字节):" + Bin2Hex(newData,newDataLen);
	TW_TRACE_DIARY_TYPE(send_info,TW_DIARY_MSG_FRAME_SEND);
	coreWrite->ComWrite((const char*)newData, (int)afterEscape.size());
	delete newData;

}