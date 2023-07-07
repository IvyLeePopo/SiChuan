/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
* 
* �ļ����ƣ�ThreadDevice.cpp
* ժ    Ҫ��Ӳ�������̣߳�ʵ���ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��7��23��
*/
// ThreadDevice.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadDevice.h"


// CThreadDevice

IMPLEMENT_DYNCREATE(CThreadDevice, CThreadBase)

CThreadDevice::CThreadDevice()
:m_pHTSDDeviceDll(NULL)
,m_iDeviceVersion(2)
//,m_bInitConfigDevice(FALSE) 
,m_bInitConfigDevice(TRUE) //����ʹ��
,m_tLastCheckBoard(NULL)

{
}

CThreadDevice::~CThreadDevice()
{
}

BOOL CThreadDevice::InitInstance()
{
	m_tLastCheckBoard = time(NULL);
	return TRUE;
}

int CThreadDevice::ExitInstance()
{
	if(NULL != m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll->Destroy();
		delete m_pHTSDDeviceDll;
		m_pHTSDDeviceDll = NULL;
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadDevice, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_GET_DATA,OnMsgStartGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_STOP_GET_DATA,OnMsgStopGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY,OnMsgGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_SHOW_DEAL_INFO,OnMsgShowDealInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_DEBIT_FLOW,OnMsgStartDebitFlow)

	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_DEBIT_CANCEL,OnMsgDebitCancelFlow)
END_MESSAGE_MAP()


// CThreadDevice message handlers

BOOL CThreadDevice::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}

	CString strLog;
	if(NULL == m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll = new CHTSDDeviceDll();
	}
	else
	{
		//�ж��Ƿ���Ҫ�������
		static time_t tLastInitDllTime = 0x00;
		if( (time(NULL) - tLastInitDllTime) > 10)
		{
			if(!m_pHTSDDeviceDll->GetDllInitState())
			{
				CString strDllFilePath;
				strDllFilePath.Format(_T("%s\\%s"),m_sTranstParam.PathCommDir,FILE_NAME_DEVICE_DLL);
				m_pHTSDDeviceDll->Init(strDllFilePath);
				tLastInitDllTime = time(NULL);
			}
		}
		//�ж��Ƿ���Ҫ��ʼ�����
		static time_t tLastInitDevTime = 0x00;
		if( (time(NULL) - tLastInitDevTime) > 10)
		{
			if(!m_pHTSDDeviceDll->GetDevInitState())
			{
				m_pHTSDDeviceDll->InitDev(m_nThreadID,NULL,WM_THREAD_DEVICE_NOTIFY,m_sTranstParam.ProvinceID,0x00);
				tLastInitDevTime = time(NULL);
			}
		}

		
		//������ѯ��������ʱ������Ӧ�𣬴����쳣
		if(time(NULL) - m_tLastCheckBoard > 60)
		{
			m_bInitConfigDevice = TRUE;
		}

		//if�ж�ֻ�ǲ���ʹ��
		static time_t tLastInit = 0x00;
		if(time(NULL) - tLastInit > 30)
		{
			//�������·��豸��ʼ��ָ��
			if(m_bInitConfigDevice && m_pHTSDDeviceDll->GetDevInitState())
			{
				//m_bInitConfigDevice = FALSE;
				CString strData = InitConfigDevice();
				m_pHTSDDeviceDll->InitConfigDevice(strData,2);
			}

			tLastInit = time(NULL);
		}

	}
	//�̴߳����־��¼���
	static time_t tLastDeviceLogTime = 0;
	if( (time(NULL) - tLastDeviceLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[�豸�����߳�],�̴߳����־���"));
		GetGlobalApp()->RecordLog(strLog);
		//������ʱ��
		tLastDeviceLogTime = time(NULL);
	}


	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::GetDeviceState
//     
// ������������ȡӲ���豸״̬
//     
// ���������void
// ���������DWORD����λ���б�ǣ��������£�0λ,��������ʧ��;1,���ڴ�ʧ��;2,������(ͨѶ����)
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��7��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CThreadDevice::GetDeviceState(void)
{
	DWORD dwState = 0x00;
	DWORD dwReturnState = 0x00;
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		//��ȡӲ����ǰ״̬
		if(!m_pHTSDDeviceDll->CheckDeviceStatus(dwState))
			throw -2;
		//��ȡ��������״̬
		if(!GetDeviceInitState())
		{
			dwReturnState |= 0x01;
		}
		//��ȡӲ������״̬
		dwReturnState |= dwState;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		}
	}
	return dwReturnState;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgStartGetData
//     
// ����������ִ��������ȡ���ݵĲ���
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgStartGetData(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;
		//������ȡɨ�����ݹ���
		m_pHTSDDeviceDll->StartGetData((int)wParam);
		strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸����ɨ�룬��ʱʱ��:%d"),(int)wParam);
		//�����1.0�汾��Ӳ��������Ҫ����IO����(��ɨ��)
		if(1 == m_iDeviceVersion)
		{
			m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SCAN);
			strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸����ɨ�룬��ʱʱ��:%d;ͬʱ������ɨ�����Ʋ���"),(int)wParam);
		}
		//������־
		GetGlobalApp()->RecordLog(strLog);
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgStopGetData
//     
// ����������ִ��ֹͣ��ȡ���ݵĲ���
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgStopGetData(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;
		//ֹͣ��ȡɨ�����ݹ���
		m_pHTSDDeviceDll->StopGetData();
		strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸ֹͣɨ�����"));
		//�����1.0�汾��Ӳ��������Ҫ����IO����(���)
		if(1 == m_iDeviceVersion)
		{
			m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
			strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸ֹͣɨ��;ͬʱ������Ʋ���"));
		}
		//������־
		GetGlobalApp()->RecordLog(strLog);
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgGetData
//     
// ������������ȡ�����豸����������
//     
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��2��26��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_NOTIFY
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgGetData(WPARAM wParam,LPARAM lParam)
{
	WORD wSubType = LOBYTE(wParam);
	WORD wCmdType = HIBYTE(wParam);
	BOOL bGetData = (BOOL)lParam;

	WORD wCmd= (WORD)wParam;

	CString* pInfo = NULL;
	//��������
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;
		//�ж��Ƿ��ȡ���ɹ�������
		if(FALSE == bGetData)
			throw -3;
		//��ȡ����
		CString strData = _T("");
		int nSrcType = 0x00;
		if(!m_pHTSDDeviceDll->GetData(strData,wCmd,nSrcType))
			throw -4;

		pInfo = new CString();
		if(NULL == pInfo)
			throw -5;

		pInfo->Format(_T("%s"),strData);

		
		//�����������ѯ����		
		if((0x01 ==wCmdType) && (0x01== wSubType))
		{
			DWORD dwData = _ttoi(strData);
			if(0x00 == dwData)
			{
				m_tLastCheckBoard = time(NULL);
			}
		}
	

		//����ʹ��
		if((0x05 ==wCmdType) && (0x05== wSubType))
		{
			DWORD dwData = _ttoi(strData);
			if(0x01 == dwData)
				m_bInitConfigDevice = FALSE;

			return;
		}

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		case -4:
			break;
		case -5:
			break;
		}
		bGetData = FALSE;
	}
	//֪ͨҵ������߳��Ѿ���ȡ�û���Ϣ//m_sTranstParam.ActiveNotifyMsgID    WM_THREAD_DEVICE_NOTIFY
	::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)wParam,(LPARAM)pInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgShowDealInfo
//     
// �������������յ���Ϣ��Ҫ��ִ�з��Թ���
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_SHOW_DEAL_INFO
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam)
{
	try
	{

		CString* pstrInfo = (CString*)wParam;
		if(NULL == pstrInfo)
			throw -1;
		if(NULL == m_pHTSDDeviceDll)
			throw -2;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -3;

		int nType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"),*pstrInfo);
		//������ʾ������Ϣ
		m_pHTSDDeviceDll->SetDisplayContext(nType,strData);

		//������Դ
		delete pstrInfo;
		pstrInfo = NULL;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgStartDebitFlow
//     
// �������������յ���Ϣ�������ۿ����̣����ϲ���ģʽ��
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��5��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����WM_THREAD_DEVICE_START_DEBIT_FLOW
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		CString* pstrXML = (CString*)wParam;
		if(NULL == pstrXML)
			throw -1;
		if(NULL == m_pHTSDDeviceDll)
			throw -2;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -3;

		int nType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"),*pstrXML);

		//�����ۿ�����

		m_pHTSDDeviceDll->DebitMoney(strData,nType);

		strLog.Format(_T("[�豸�����߳�],���յ�ָ���ʼ�ۿ�ɨ�����̲���"));
		//������־
		GetGlobalApp()->RecordLog(strLog);
		//������Դ
		delete pstrXML;
		pstrXML = NULL;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		}
	}
}




BOOL CThreadDevice::GetC0SuccFailedCount(OUT int& dwSucce, OUT int& dwFailed, OUT int& nReSendCount, OUT CString& strA2Frame, OUT CString& strC0Frame)
{
	BOOL bRet = FALSE;
	dwSucce = 0;
	dwFailed = 0;
	nReSendCount = 0;
	strA2Frame = _T("");
	strC0Frame = _T("");
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		//��ȡӲ����ǰ״̬
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;

		bRet = m_pHTSDDeviceDll->GetCSuccFailCount(dwSucce,dwFailed,nReSendCount,strA2Frame,strC0Frame);
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		}
	}


	return bRet;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgDebitCancelFlow
//     
// �������������յ���Ϣ�������ۿ����̣����ϲ���ģʽ��
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��5��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����WM_THREAD_DEVICE_START_DEBIT_FLOW
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgDebitCancelFlow(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		CString* pstrXML = (CString*)wParam;
		if(NULL == pstrXML)
			throw -1;
		if(NULL == m_pHTSDDeviceDll)
			throw -2;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -3;

		int nType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"),*pstrXML);

		//��������
		m_pHTSDDeviceDll->DebitCancel(strData,nType);

		strLog.Format(_T("[�豸�����߳�],���յ�ָ���ʼִ�г�������"));
		//������־
		GetGlobalApp()->RecordLog(strLog);
		//������Դ
		delete pstrXML;
		pstrXML = NULL;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		}
	}
}

const CString CThreadDevice::InitConfigDevice(int nFormatType)
{
	CString strDataString;

	if(0x01 == nFormatType)
	{
		Json::Value root;
		Json::Value DeviceParamContext;
		Json::Value Data;

		Data["ProvinceID"]			= Json::Value(m_sTranstParam.ProvinceID);
		Data["AreaID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.AreaID);
		Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.RoadID);
		Data["StationID"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationID);
		Data["LaneID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.LaneID);
		Data["StationName"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationTab);
		Data["ServerType"]			= Json::Value(m_sTranstParam.LaneConfigInfo.STServerTpye);
		Data["ServerIP"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerIP);
		Data["ConfigLocalIP0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0);
		Data["ConfigLocalSub0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0);
		Data["ConfigLocalGate0"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0);
		Data["ConfigLocalIP1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1);
		Data["ConfigLocalSub1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1);
		Data["ConfigLocalGate1"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1);
		Data["ConfigLocalDefGate"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate);
		Data["DevicePayMode"]		= Json::Value(3);
		Data["CurrentTime"]			= Json::Value(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));


		DeviceParamContext["Data"] = Data;
		DeviceParamContext["Version"] = Json::Value(_T("1.0"));

		root["DeviceParamContext"] = DeviceParamContext;

		Json::FastWriter fast_writer;
		strDataString.Format(_T("%s"),fast_writer.write(root).c_str());
	}

	return strDataString;

}

