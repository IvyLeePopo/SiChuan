/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: SerialPort.cpp
* 摘   要:  实现串口
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-09
* 修改记录:
*/
#include "stdafx.h"
#include "SerialPort.h"
#include "SystemTrace.h"
#include "WorkingPara.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSerialPort::CSerialPort()
{
    SetDefaultValue();
//  mMsgConv.Create(IDD_MESSAGEWINDOWCONV);
//  mMsgConv.SetMessageConvParent(this);
    //mComKernel.SetMain(0,mMsgConv.m_hWnd,1);//发送给句柄
    //mComKernel.SetNotifyNum(1);

}
CSerialPort::~CSerialPort()
{
    ComClose();
}
int CSerialPort::GetLastReadTime()
{
    return mLastReadTime;
}
//////////////////////////////////////////////////////////////
// 函数名称:CSerialPort::ComOpen
//
// 功能描述:打开指定串口
//
// 输入参数:int port,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CSerialPort::ComOpen(int port)
{
    mPort = port;
    if (mPort < 0 || mPort > 99)
    {
        mPort = 0; //当前
    }
    std::string strInfo = "串口号：" + Int2Str(mPort) + ",波特率:" + Int2Str(mBaud);
    TW_TRACE_DIARY_TYPE(strInfo,TW_DIARY_MSG_THREAD_MAIN);
    return mComKernel.Open(mPort, mBaud, mDataBits, mCheckType, mStopBits);
}
//////////////////////////////////////////////////////////////
// 函数名称:CSerialPort::SetComOption
//
// 功能描述:设置串口
//
// 输入参数:int baud, int data_bits, int stop_bits, int check_type,
// 输出参数:void 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CSerialPort::SetComOption(int baud, int data_bits, int stop_bits, int check_type)
{
    mBaud = baud;
    mDataBits = data_bits;
    mStopBits = stop_bits;
    mCheckType = check_type;
}
//////////////////////////////////////////////////////////////
// 函数名称:CSerialPort::ComReOpen
//
// 功能描述:重新打开串口
//
// 输入参数:
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CSerialPort::ComReOpen()
{
    if (mComKernel.IsOpen())
    {
        mComKernel.Close();
    }
    return mComKernel.Open(mPort, mBaud, mDataBits, mCheckType, mStopBits);
}
void CSerialPort::ComClose()
{
    mComKernel.Close();
}
//////////////////////////////////////////////////////////////
// 函数名称:CSerialPort::ComRead
//
// 功能描述:读串口数据
//
// 输入参数:char* buf, DWORD len,
// 输出参数:int 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
int CSerialPort::ComRead(char* buf, DWORD len )
{
    int rLen = mComKernel.Read(buf, len);
    if (rLen > 0)
    {
        mLastReadTime = time(NULL);
    }
    return rLen;
}
//////////////////////////////////////////////////////////////
// 函数名称:CSerialPort::ComWrite
//
// 功能描述:写串口数据
//
// 输入参数:const char* buf, int len,
// 输出参数:bool 
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CSerialPort::ComWrite(const char* buf, int len)
{
    mComKernel.Write((LPVOID)buf, len);
    return true;//SerialCom使用异步写，默认成功
}
void CSerialPort::ComClearFlush(int mode )
{
    if (mode == COM_FLUSH_ALL)
    {
        mComKernel.ClearInputBuffer();
        mComKernel.ClearOutputBuffer();
    }
    else if (mode == COM_FLUSH_READ)
    {
        mComKernel.ClearInputBuffer();
    }
    else if (mode == COM_FLUSH_WRITE)
    {
        mComKernel.ClearOutputBuffer();
    }
}
void CSerialPort::SetEventOwner(CPortEventBase* eventObj, int id )
{
    mEventBase = eventObj;
    mEventId = id;
}
void CSerialPort::SetThreadOwner(int threadID)
{
    mComKernel.SetMain(threadID, 0, 0); //发送给句柄
    mComKernel.SetNotifyNum(1);
}
void CSerialPort::SetDefaultValue()
{
    mBaud = SystemParaComBaud();//COM_BAUD_9600;
    mDataBits = COM_DATA_8;
    mStopBits = COM_STOP_1;
    mCheckType = COM_CHECK_NONE;

    mEventId = 0;
    mEventBase = NULL;
    mLastReadTime = 0;

}

bool CSerialPort::ComIsOpen()
{
    return mComKernel.IsOpen();
}

void CSerialPort::HandleWindowMsg(WPARAM wParam, LPARAM lParam)
{
    //收到数据进行处理
    if (mEventBase)
    {
        mEventBase->OnReceiveData(mEventId);
    }
}