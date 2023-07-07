

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "SystemSwitch.h"
#include "ProtocolHelper.h"
#include "HttpConnection.h"
//��־�滻
void LogSome(const std::string& info)
{
	std::string logdata = "[ϵͳά���߳�]"+info;
	GetGlobalApp()->RecordLog(logdata.c_str());
	return ;
}


IMPLEMENT_DYNCREATE(CSystemSwitch, CThreadBase)

CSystemSwitch::CSystemSwitch()
{
	m_InExchanging = false;
	m_PCNetworkOK =false;
	::InitializeCriticalSection(&m_idleMutex);
	::InitializeCriticalSection(&m_signalMutex);
	ResetTerminalSignal();
}

CSystemSwitch::~CSystemSwitch()
{
}

void CSystemSwitch::waitTillIdle()
{
	{
		if (this->m_bExit)
		{
			return ;
		}
		SetTerminalSignal();
		CScopeLock tmpIdle(&m_idleMutex);
		this->m_bExit = true;
	}

	return ;
	

}

BOOL CSystemSwitch::InitInstance()
{
	
	return TRUE;
}

int CSystemSwitch::ExitInstance()
{
	::DeleteCriticalSection(&m_idleMutex);
	::DeleteCriticalSection(&m_signalMutex);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSystemSwitch, CWinThread)
END_MESSAGE_MAP()

BOOL CSystemSwitch::OnIdle(LONG lCount)
{
	CScopeLock tmpIdle(&m_idleMutex);
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}


	//fordebug
	/*
	{
		CheckAndSleep(20);
		ExcuteMiddlewareSwitch();
		return CThreadBase::OnIdle(lCount);
	}
	*/

	//
	SystemDetect();
	CheckAndSleep(5);

	


	return CThreadBase::OnIdle(lCount);
}

void CSystemSwitch::SystemDetect()
{
	if(m_InExchanging)
	{
		LogSome("In Exchaning!");
		return ;
	}
	LogSome("Check PC Network");
	if(IsPCNetworkAvailable())
	{
		NotifyTerminalSwitch2V1_5();
		return;
	}
	LogSome("Network not available!");
	if(NotifyTerminalSwitch2V2_0())
	{
		LogSome("�ն����л���2.0����̬�⼴���л�");
		ExcuteMiddlewareSwitch();
		m_InExchanging = true;
	}
}
bool CSystemSwitch::IsPCNetworkAvailable()
{
	static time_t lastTestHeartBeatTime = 0;
	time_t currTime = time(NULL);
	if(currTime-lastTestHeartBeatTime>5)
	{

		HttpConnection testHeartConn;
		std::string remoteIP = std::string((LPCSTR) GetGlobalApp()->m_sTranstParam.LaneConfigInfo.WebServerIP);
		int remotePort=  GetGlobalApp()->m_sTranstParam.LaneConfigInfo.WebServerPort;
		std::string postData = "HeartCheck";
		std::string unuseFeedback;
		testHeartConn.ChangeRemotePeer(remoteIP,remotePort);
		if(testHeartConn.Get("/heartbeat",postData,unuseFeedback))
		{
			LogSome("����ָ�");
			m_PCNetworkOK = true;
		}
		else
		{
			LogSome("�����������"+unuseFeedback);
			m_PCNetworkOK = false;
		}
		
		lastTestHeartBeatTime= currTime;
	}


	return m_PCNetworkOK;
	/*
	static bool hasQuery = false;
	if(!hasQuery)
	{//��һ�μ������״̬����Ҫ�ȴ�һ��ʱ��
		Sleep(10*1000);
	}
	hasQuery = true;
	return (GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist==FALSE);
	*/
	//return false;
}
bool CSystemSwitch::NotifyTerminalSwitch2V1_5()
{
	int terminalRunStatus =0;
	while(true)
	{
		if(!QueryTerminalRunningStatus(terminalRunStatus))
		{
			LogSome("��ѯ�ն�����״̬ʧ�ܣ�������");
			Sleep(500);
			continue;
		}
		if(terminalRunStatus==TERMINALRUN_V1_5)
		{
			return true;
		}
		LogSome("��ǰ��M1ϵͳ����Ҫ�����л�");
		if(!SendSwitchCommond(15))
		{
			LogSome("�����л�ָ��ʧ��");
		}
		else
		{
			LogSome("�����л�ָ��ɹ�");
		}
		for(int i=0;i<6;++i)
		{
			if(QueryTerminalRunningStatus(terminalRunStatus)
				&&(terminalRunStatus==TERMINALRUN_V1_5))
			{
				break;
			}
			Sleep(1000*10);
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
			LogSome("��ѯ�ն�����״̬ʧ�ܣ�������");
			Sleep(500);
			continue;
		}
		if(terminalRunStatus==TERMINALRUN_V2_0)
		{
			return true;
		}
		LogSome("׼���л���2.0");
		if(!SendSwitchCommond(20))
		{
			LogSome("�����л�ָ��ʧ��");
		}
		else
		{
			LogSome("�����л�ָ��ɹ�");
		}

		//�ȴ�һ����
		for(int i=0;i<6;++i)
		{
			if(QueryTerminalRunningStatus(terminalRunStatus)
				&&(terminalRunStatus==TERMINALRUN_V2_0))
			{
				break;
			}
			Sleep(1000*10);
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
	//����֪ͨ�����ƽ̨�����а汾�л�
	Json::Value rootN;
	rootN["Type"]=Json::Value("ExchSystem");
	rootN["SwitchTo"]=Json::Value("2.0");
	
	Json::FastWriter jsonWriter;
	std::string notifyStr = jsonWriter.write(rootN);
	char tmpInfo[200]={0};
	strcpy(tmpInfo,notifyStr.c_str());
	GetGlobalApp()->send2Bus(KEY_COMMON_FRAME,tmpInfo,notifyStr.size(),KEY_BUSINESS);
}

void CSystemSwitch::CheckAndSleep(int seconds)
{
	for(int i=0;i<seconds;++i)
	{
		if(CheckTerminalSignal())
		{
			break;
		}
		Sleep(1000);
	}
	return;
}
void CSystemSwitch::SetTerminalSignal()
{
	CScopeLock signalLock(&m_signalMutex);
	m_terminalSignal = true;
	return;
}
void CSystemSwitch::ResetTerminalSignal()
{
	CScopeLock signalLock(&m_signalMutex);
	m_terminalSignal = false;
	return;
}
bool CSystemSwitch::CheckTerminalSignal()
{
	CScopeLock signalLock(&m_signalMutex);
	return m_terminalSignal;
}