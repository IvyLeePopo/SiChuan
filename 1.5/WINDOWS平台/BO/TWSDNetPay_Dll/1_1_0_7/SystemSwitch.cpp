

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "SystemSwitch.h"
#include "ProtocolHelper.h"
//日志替换
void LogSome(const std::string& info)
{
	std::string logdata = "[系统维护线程]"+info;
	GetGlobalApp()->RecordLog(logdata.c_str());
	return ;
}


IMPLEMENT_DYNCREATE(CSystemSwitch, CThreadBase)

CSystemSwitch::CSystemSwitch()
{
	

}

CSystemSwitch::~CSystemSwitch()
{
}


BOOL CSystemSwitch::InitInstance()
{
	
	return TRUE;
}

int CSystemSwitch::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSystemSwitch, CWinThread)
END_MESSAGE_MAP()

BOOL CSystemSwitch::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}
	SystemDetect();
	Sleep(1000*10);

	return CThreadBase::OnIdle(lCount);
}

void CSystemSwitch::SystemDetect()
{
	LogSome("Check PC Network");
	if(IsPCNetworkAvailable())
	{
		NotifyTerminalSwitch2V1_5();
		return;
	}
	LogSome("Network not available!");
	if(NotifyTerminalSwitch2V2_0())
	{
		LogSome("终端已切换至2.0，动态库即将切换");
		ExcuteMiddlewareSwitch();
	}
}
bool CSystemSwitch::IsPCNetworkAvailable()
{
	return (GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist==TRUE);
	//return false;
}
bool CSystemSwitch::NotifyTerminalSwitch2V1_5()
{
	int terminalRunStatus =0;
	while(true)
	{
		if(!QueryTerminalRunningStatus(terminalRunStatus))
		{
			LogSome("查询终端运行状态失败，重试中");
			Sleep(500);
			continue;
		}
		if(terminalRunStatus==TERMINALRUN_V1_5)
		{
			return true;
		}
		LogSome("当前非M1系统，需要进行切换");
		if(!SendSwitchCommond(15))
		{
			LogSome("发送切换指令失败");
		}
		else
		{
			LogSome("发送切换指令成功");
		}
		Sleep(1000);
	}
	return false;
}
bool CSystemSwitch::NotifyTerminalSwitch2V2_0()
{
	int terminalRunStatus =0;
	while(true)
	{
		if(!QueryTerminalRunningStatus(terminalRunStatus))
		{
			LogSome("查询终端运行状态失败，重试中");
			Sleep(500);
			continue;
		}
		if(terminalRunStatus==TERMINALRUN_V2_0)
		{
			return true;
		}
		LogSome("准备切换至2.0");
		if(!SendSwitchCommond(20))
		{
			LogSome("发送切换指令失败");
		}
		else
		{
			LogSome("发送切换指令成功");
		}
		Sleep(1000);
	}
	return false;
}
bool CSystemSwitch::QueryTerminalRunningStatus(int& stauts)
{
	return GetGlobalApp()->m_pThreadManage->m_pThreadDevice->QueryTerminalSystemVersionStatus(stauts);
	//return false;
}

bool CSystemSwitch::SendSwitchCommond(const int& targetSystemVer)
{
	return GetGlobalApp()->m_pThreadManage->m_pThreadDevice->ExchangeTerminalSystem(targetSystemVer);
	//return false;
}

void CSystemSwitch::ExcuteMiddlewareSwitch()
{
	//发送通知到组件平台，进行版本切换
	Json::Value rootN;
	rootN["Type"]=Json::Value("ExchSystem");
	rootN["SwitchTo"]=Json::Value("2.0");
	
	Json::FastWriter jsonWriter;
	std::string notifyStr = jsonWriter.write(rootN);
	char tmpInfo[200]={0};
	strcpy(tmpInfo,notifyStr.c_str());
	GetGlobalApp()->send2Bus(KEY_COMMON_FRAME,tmpInfo,notifyStr.size(),KEY_BUSINESS);
}