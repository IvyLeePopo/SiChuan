/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: CommondDak.h
* ժ   Ҫ:  ��ģ��ʵ��������Э��ͨѶ����
* 1��Э�����ݵķ��ͺͽ���
* 2���յ����ݺ��Զ�����Ӧ�����
* 3�������ڶ��̻߳�����ʹ��
* 4������һ��ʱ��û���յ���Ч��ͨѶ���ݣ�������ͨѶ����
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-06-10
* �޸ļ�¼: 
*/
#ifndef COMMOND_DAK_H
#define COMMOND_DAK_H
#include "PipeIn.h"
#include "PipeOut.h"

/**
 * ���Զ��ظ�C0֡
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
		//TW_TRACE_DIARY("ɾ��CMD data");
		delete data.DATA;
		data.DATA = NULL;
		//TW_TRACE_DIARY("ɾ��CMD data over");
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
	TWCmdKey CalcRespondKey(const TWProtocol_BaseFrame& info);/*��ȡӦ����Ϣ�Ĺؼ���*/
	bool SendCmd(const TWProtocol_BaseFrame& info);
	bool SendVIPCmd(const TWProtocol_BaseFrame& info); //���ȷ���
	bool TakeCmd(TWProtocol_BaseFrame&info);
	bool QuerySendResult(TWCmdKey crk );/*��ѯ���ͽ��*/
	bool SendSelfCmd(const std::string& cmdInfo);//��װ�õ�����֡
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
