/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: SerialPort.cpp
* ժ   Ҫ:  ʵ�ִ���
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-09
* �޸ļ�¼:
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
    //mComKernel.SetMain(0,mMsgConv.m_hWnd,1);//���͸����
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
// ��������:CSerialPort::ComOpen
//
// ��������:��ָ������
//
// �������:int port,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CSerialPort::ComOpen(int port)
{
    mPort = port;
    if (mPort < 0 || mPort > 99)
    {
        mPort = 0; //��ǰ
    }
    std::string strInfo = "���ںţ�" + Int2Str(mPort) + ",������:" + Int2Str(mBaud);
    TW_TRACE_DIARY_TYPE(strInfo,TW_DIARY_MSG_THREAD_MAIN);
    return mComKernel.Open(mPort, mBaud, mDataBits, mCheckType, mStopBits);
}
//////////////////////////////////////////////////////////////
// ��������:CSerialPort::SetComOption
//
// ��������:���ô���
//
// �������:int baud, int data_bits, int stop_bits, int check_type,
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
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
// ��������:CSerialPort::ComReOpen
//
// ��������:���´򿪴���
//
// �������:
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
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
// ��������:CSerialPort::ComRead
//
// ��������:����������
//
// �������:char* buf, DWORD len,
// �������:int 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
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
// ��������:CSerialPort::ComWrite
//
// ��������:д��������
//
// �������:const char* buf, int len,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CSerialPort::ComWrite(const char* buf, int len)
{
    mComKernel.Write((LPVOID)buf, len);
    return true;//SerialComʹ���첽д��Ĭ�ϳɹ�
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
    mComKernel.SetMain(threadID, 0, 0); //���͸����
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
    //�յ����ݽ��д���
    if (mEventBase)
    {
        mEventBase->OnReceiveData(mEventId);
    }
}