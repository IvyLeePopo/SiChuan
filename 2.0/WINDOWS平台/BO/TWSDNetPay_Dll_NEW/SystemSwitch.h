
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
//重载函数
protected:
	
private:
	void SystemDetect();
	bool IsPCNetworkAvailable();
	bool NotifyTerminalSwitch2V1_5();//切换至1.5
	bool NotifyTerminalSwitch2V2_0();//切换至2.0
	bool SendSwitchCommond(const int& targetSystemVer);
	bool QueryTerminalRunningStatus(int& stauts);

	void ExcuteMiddlewareSwitch();
	
//属性
private:
	bool m_InExchanging;//表示正在切换中
	bool m_PCNetworkOK;
	
};


