/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: SerialPort.h
* 摘   要:  实现串口通信相关内容
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-09
* 修改记录: 
*/


#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H
#include "ComPortOption.h"
#include "SerialCom.h"
//#include "..\MessageWindowConv.h"
#include "SystemTrace.h"
#include "TypeHelper.h"
#include <vector>

class CPortEventBase
{
public:
    CPortEventBase(){}
    virtual ~CPortEventBase(){}
public:
    virtual bool OnReceiveData(int id){return false;}
private:
};


class CSerialPort 
	 //: public CMessageConv
{
public:
    CSerialPort();
    ~CSerialPort();
public:
    bool ComOpen(int port);
    void SetComOption(int baud, int data_bits, int stop_bits, int check_type);
    bool ComReOpen();
    void ComClose();
	bool ComIsOpen();
    int ComRead(char*buf, DWORD len );
    bool ComWrite(const char* buf, int len);
    void ComClearFlush(int mode );
    void SetEventOwner(CPortEventBase* eventObj, int id );
    void SetThreadOwner(int threadID);
    int GetLastReadTime();
public:
	void HandleWindowMsg(WPARAM wParam, LPARAM lParam);
private:
    void SetDefaultValue();
private:
    bool mIsOpen;
    int mPort;
    int mBaud;
    int mDataBits;
    int mStopBits;
    int mCheckType;
    int mEventId;
    int mCallThreadId;

    int mLastReadTime;
    
    CPortEventBase* mEventBase;
    CSerialCom mComKernel;
	//CMessageWindowConv mMsgConv;


private:
    CSerialPort(const CSerialPort&);
    CSerialPort& operator=(const CSerialPort&);
    
};

struct RawSerialData
{
    std::vector<unsigned char> data;
    bool EndWith(const unsigned char& ch)
    {
        if(data.empty())
        {
            return false;
        }
        //TW_TRACE_DIARY("Before EndWith:" + Int2Str(data.size()));
        bool flag = (data[data.size()-1]==ch);
        //TW_TRACE_DIARY("After EndWith");
        return flag;
        // return data.back()==ch;
    }
    // unsigned char* data;
    // int len;
    // RawSerialData()
    // {
    //     memset(this,0,sizeof(RawSerialData));
    // }
    // bool EndWith(const unsigned char ch)
    // {
    //     if(data==NULL)
    //     {
    //         return false;
    //     }
    //     return data[len-1]==ch;
    // }
};

#endif // SERIAL_PORT_H