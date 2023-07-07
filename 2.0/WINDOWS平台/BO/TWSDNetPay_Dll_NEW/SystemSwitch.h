
#pragma once
#include "BaseDefine.h"
#include "ThreadBase.h"
#include <afxinet.h>
#include <process.h> 

#define TERMINALRUN_V1_5  10
#define TERMINALRUN_V2_0  20
#define TERMINALRUN_V1_5_TO_V2_0 11
#define TERMINALRUN_V2_0_TO_V1_5 21


class CSystemSwitch : public CThreadBase
{
	DECLARE_DYNCREATE(CSystemSwitch)

protected:
	CSystemSwitch();           // protected constructor used by dynamic creation
	virtual ~CSystemSwitch();
public:
	void waitTillIdle();
private:
	void CheckAndSleep(int seconds);
	void SetTerminalSignal();
	void ResetTerminalSignal();
	bool CheckTerminalSignal();
private:
	bool m_terminalSignal;
	CRITICAL_SECTION m_signalMutex;
	CRITICAL_SECTION m_idleMutex;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
protected:
	
	DECLARE_MESSAGE_MAP()
//���غ���
protected:
	
private:
	void SystemDetect();
	bool IsPCNetworkAvailable();
	bool NotifyTerminalSwitch2V1_5();//�л���1.5
	bool NotifyTerminalSwitch2V2_0();//�л���2.0
	bool SendSwitchCommond(const int& targetSystemVer);
	bool QueryTerminalRunningStatus(int& stauts);

	void ExcuteMiddlewareSwitch();
	
//����
private:
	bool m_InExchanging;//��ʾ�����л���
	bool m_PCNetworkOK;
	
};


