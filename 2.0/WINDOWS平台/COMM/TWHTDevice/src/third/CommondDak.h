/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: CommondDak.h
* 摘   要:  此模块实现完整的协议通讯功能
* 1、协议数据的发送和接收
* 2、收到数据后，自动发送应答回馈
* 3、可以在多线程环境下使用
* 4、超过一定时间没有收到有效的通讯数据，会重置通讯串口
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-06-10
* 修改记录: 
*/
#ifndef COMMOND_DAK_H
#define COMMOND_DAK_H
#include "PipeIn.h"
#include "PipeOut.h"

/**
 * 会自动回复C0帧
 */
typedef int TWCmdKey;
struct RespondResult
{
	time_t genTime;
	int resultCode;
	RespondResult()
	{
		memset(this,0,sizeof(RespondResult));
		genTime = time(NULL);
	}
};
class CmdDataInfo
{
public:
	CmdDataInfo()
	{
		memset(this,0,sizeof(CmdDataInfo));
		createTime = time(NULL);
	}
	~CmdDataInfo()
	{
		//TW_TRACE_DIARY("删除CMD data");
		delete data.DATA;
		data.DATA = NULL;
		//TW_TRACE_DIARY("删除CMD data over");
	}
	time_t createTime;
	time_t lastSendTime;
	int sendCount;
	bool sendSuccess;
	TWCmdKey crk;
	TWProtocol_BaseFrame data;
};

class CCommondDak:public CWinThread 
{
	DECLARE_DYNCREATE(CCommondDak)
public:
	CCommondDak();
	virtual ~CCommondDak();
public:
	TWCmdKey CalcCmdKey(const TWProtocol_BaseFrame& info);
	TWCmdKey CalcRespondKey(const TWProtocol_BaseFrame& info);/*获取应答信息的关键字*/
	bool SendCmd(const TWProtocol_BaseFrame& info);
	bool SendVIPCmd(const TWProtocol_BaseFrame& info); //优先发送
	bool TakeCmd(TWProtocol_BaseFrame&info);
	bool QuerySendResult(TWCmdKey crk );/*查询发送结果*/
	bool SendSelfCmd(const std::string& cmdInfo);//组装好的数据帧
public:
	virtual afx_msg BOOL InitInstance();
    virtual afx_msg int ExitInstance();
	virtual afx_msg BOOL OnIdle(LONG lCount);
private:
	void SetDefault();
    void Stop();
    void SendProc();
    void RecvProc();
    void ClearCmd();
	BOOL CheckComStatus();
private:
	CSerialPort mIOCore;
	int mPortNumber;
	CPipeIn* mpCmdIn;
	CPipeOut* mpCmdOut;
	CRITICAL_SECTION mCmdsMutex; 
	CRITICAL_SECTION mResultMutex; 
	std::list<CmdDataInfo> mCmdQueue;
	std::list<TWProtocol_BaseFrame> mResultQueue;
	std::list<std::string> m_SelfSendQueue;
	time_t mLastReadTime;
	bool m_bExit;
	
private:
	CCommondDak(const CCommondDak&);
	CCommondDak& operator = (const CCommondDak&);
};


#endif // COMMOND_DAK_H
