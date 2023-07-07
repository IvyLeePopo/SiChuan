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
	: m_iDeviceVersion(2)
	, m_pHTSDDeviceDll(NULL)
	, m_dwLastCheckState(0)
	, m_bInitConfigDevice(FALSE)
	, m_tLastInit(0)
{
	m_tLastInit = time(NULL) - 20;

	m_pHTSDDeviceDll = new CHTSDDeviceDll2();
}

CThreadDevice::~CThreadDevice()
{
	if (NULL != m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll->Destroy();
		delete m_pHTSDDeviceDll;
		m_pHTSDDeviceDll = NULL;
	}
}

BOOL CThreadDevice::InitInstance()
{
	return TRUE;
}

int CThreadDevice::ExitInstance()
{

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadDevice, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_SHOW_DEAL_INFO, OnMsgShowDealInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_DEBIT_FLOW, OnMsgStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_DEBIT_CANCEL, OnMsgDebitCancelFlow)
END_MESSAGE_MAP()


// CThreadDevice message handlers

BOOL CThreadDevice::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}
	CString strLog;
	CString strError;
	/*
	if (NULL == m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll = new CHTSDDeviceDll2();
	}
	else 
	*/
	if(NULL!=m_pHTSDDeviceDll)
	{
		//�ж��Ƿ���Ҫ�������
		static time_t tLastInitDllTime = 0x00;
		if ( (time(NULL) - tLastInitDllTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDllInitState())
			{
				CString strDllFilePath;
				strDllFilePath.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, "HTTWSDDevice_V2.dll");//FILE_NAME_DEVICE_DLL);
				m_pHTSDDeviceDll->Init(strDllFilePath);
				tLastInitDllTime = time(NULL);
			}
			tLastInitDllTime = time(NULL);//add 2018-01
		}

		//�ж��Ƿ���Ҫ��ʼ�����
		static time_t tLastInitDevTime = 0x00;
		if ( (time(NULL) - tLastInitDevTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDevInitState())
			{
				if (!m_pHTSDDeviceDll->InitDev(fnGetNotifyFromCommModule))
				{
					m_pHTSDDeviceDll->GetLastErrorDesc(strError);
				}
				tLastInitDevTime = time(NULL);
			}
			tLastInitDllTime = time(NULL);//add 2018-01
		}

		//���ͳ�ʼ��ָ�Ƶ��:10��/��
		if (time(NULL) - m_tLastInit > 10)
		{
			//�������·��豸��ʼ��ָ��
			if ( !m_bInitConfigDevice )
			{
				m_bInitConfigDevice = InitConfigDevice();
			}

			m_tLastInit = time(NULL);
		}
	}

	//�̴߳����־��¼���
	static time_t tLastDeviceLogTime = 0;
	if ( (time(NULL) - tLastDeviceLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
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
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		//��ȡӲ����ǰ״̬
		if (!m_pHTSDDeviceDll->CheckDeviceStatus(dwState))
			throw - 2;
		
		//��ȡ��������״̬
		if (!GetDeviceInitState())
		{
			dwReturnState |= 0x01;
		}
		//��ȡӲ������״̬
		dwReturnState |= dwState;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		case -2://add 2017-08
			dwReturnState = 0x000000FF;
			break;
		}
	}
	return dwReturnState;
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
void CThreadDevice::OnMsgShowDealInfo(WPARAM wParam, LPARAM lParam)
{
	BOOL bSaveError = FALSE;
	BYTE byCmdType		= RDM_SET_MMI_INFO;
	BYTE byExecuteCode	= OP_FAILED;
	try
	{
		CString* pstrInfo = (CString*)wParam;
		if (NULL == pstrInfo)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		CString strData;
		strData.Format(_T("%s"), *pstrInfo);
		//������ʾ������Ϣ
		m_pHTSDDeviceDll->SetDevice(strData);
		//	m_pHTSDDeviceDll->(0x00,strData);

		//������Դ
		delete pstrInfo;
		pstrInfo = NULL;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		}
		bSaveError = TRUE;
	}
	//֪ͨҵ������߳��Ѿ���ȡӲ������ҵ������
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)NULL);
}

/*add 2017-05-19 */
//#include <json.h>
bool TmpFillDebitResult( CString& ptr)
{

	if (ptr.IsEmpty())
	{
		return false;
	}
	Json::Reader reader;
	Json::Value root, context, data, tradeitem, transtime, transplat, securecode, ordercode, payid, consumetime;
	std::string info(ptr.GetBuffer(0));
	if (!reader.parse(info, root))
	{
		return false;
	}
	context = root["DebitResultContext"];
	data = context["Data"];
	tradeitem = data["TradeKeyItem"];
	transtime = tradeitem["DebitTime"];
	if (transtime.type() == Json::nullValue)
	{
		return false;
	}
	GetGlobalApp()->m_sCurrentDebitResult.DebitTime =  GetGlobalApp()->FixUTC( GetGlobalApp()->CString2time_t(transtime.asCString()));
	//add 2018-01 ���յ���ʱ��ת��Ϊ������
	transplat = tradeitem["PayPlatformType"];
	if (transplat.type() != Json::stringValue)
	{
		return false;
	}
	GetGlobalApp()->m_sCurrentDebitResult.PayPlatformType = _ttoi(transplat.asCString());

	securecode = tradeitem["TradeSecurityCode"];
	if (securecode.type() != Json::stringValue)
	{
		return false;
	}
	strncpy(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode, securecode.asCString(), 40);

	ordercode = tradeitem["DebitOrder"];
	if (ordercode.type() != Json::stringValue)
	{
		return false;
	}
	strncpy(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder, ordercode.asCString(), 40);

	payid = tradeitem["PayIdentifier"];
	if (payid.type() != Json::stringValue)
	{
		return false;
	}
	strncpy(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier, payid.asCString(), 40);

	consumetime = tradeitem["ConsumeTime"];
	if (payid.type() != Json::stringValue)
	{
		return false;
	}
	GetGlobalApp()->m_sCurrentDebitResult.ConsumeTime = _ttoi(consumetime.asCString());

	strcpy(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc, "�ɹ�");
	return true;

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
void CThreadDevice::OnMsgStartDebitFlow(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString* pStrResult = NULL;
	BOOL bSaveError		= FALSE;

	BYTE byCmdType		= RDM_DEBITE_START;
	BYTE byExecuteCode	= OP_FAILED;

	try
	{
		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		int iType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"), *pstrXML);

		pStrResult = new CString();
		if (NULL == pStrResult)
			throw - 4;

		strLog.Format(_T("[�豸�����߳�],���յ�ָ���ʼ�۷����̲���"));
		GetGlobalApp()->RecordLog(strLog);
		//�����ۿ�����
		CString strResult;
		if (!m_pHTSDDeviceDll->DebitMoney(strData, strResult, iType))
			throw - 5;
		pStrResult->Format(_T("%s"), strResult);
		strLog.Format(_T("[�豸�����߳�],�۷ѽ��:%s"),strResult);
		GetGlobalApp()->RecordLog(strLog);
		TmpFillDebitResult(strResult);
		//������Դ
		delete pstrXML;
		pstrXML = NULL;
		strLog.Format(_T("[�豸�����߳�],�۷�����ִ�����"));
		
		//��ʶִ�гɹ�
		byExecuteCode	= OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[�豸�����߳�],���յ��Ĳ����쳣,ָ��Ϊ��"));
			break;
		case -2:
			strLog.Format(_T("[�豸�����߳�],ͨѶģ��״̬�쳣,ָ��Ϊ��"));
			break;
		case -3:
			strLog.Format(_T("[�豸�����߳�],ͨѶģ��δ�ܳɹ���ʼ��"));
			break;
		case -4:
			strLog.Format(_T("[�豸�����߳�],�ڲ���Դ�����쳣"));
			break;
		case -5:
		{
			CString strError;
			m_pHTSDDeviceDll->GetLastErrorDesc(strError);

			byExecuteCode = 0xFF;
			strLog.Format(_T("[�豸�����߳�],ִ�п۷Ѳ���ʧ�ܣ�����ԭ��������%s"), strError);
			if(strError.Find("������ȡ��")!=-1)
			{
				byExecuteCode = OP_CANCLE;
			}
			
			else if (strError.Find("�˻�����") != -1)
			{
				byExecuteCode = OP_NO_MONEY;
			}
			else if (strError.Find("��ά��Ƿ�") != -1)
			{
				byExecuteCode = OP_QRCODE_ILLEGAL;
			}
			else if (strError.Find("��ά�볬����Ч��") != -1)
			{
				byExecuteCode = OP_QRCODE_OT;
			}
			else if (strError.Find("֧��ƾ֤�쳣") != -1)
			{
				byExecuteCode = OP_NO_ACCOUNT_INFO;
			}
			else if (strError.Find("�ۿʱ") != -1)
			{
				byExecuteCode = OP_DEBIT_OT;
			}
			else if (strError.Find("�������") != -1)
			{
				byExecuteCode = OP_PSW_ERR;
			}
			else if (strError.Find("��������") != -1)
			{
				byExecuteCode = OP_PARAM_ERR;
			}
			else if (strError.Find("�ϴν���δ����") != -1)
			{
				byExecuteCode = OP_LAST_TRADE_NOT_END;
			}
			else if (strError.Find("���δ��ʼ��") != -1)
			{
				byExecuteCode = OP_COM_NOT_INIT;
			}
			else if (strError.Find("���У��Ƿ�") != -1)
			{
				byExecuteCode = OP_COM_ILLEGAL;
			}
			else if (strError.Find("֧��ƽ̨�������") != -1)
			{
				//byExecuteCode = OP_PAY_PLATFORM_ERR;
			}
			else if( strError.Find("����λ��ͨѶ�쳣")!=-1)
			{
				//byExecuteCode = OP_DISCONNECT_TERMINAL;
			}
		}
		break;
		}

		bSaveError = TRUE;
	}
	//������־
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog, bSaveError ? LOG_LEVEL_ERROR : LOG_LEVEL_RUN);
	}
	//֪ͨҵ������߳��Ѿ���ȡӲ������ҵ������
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)pStrResult);

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
void CThreadDevice::OnMsgDebitCancelFlow(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString* pStrResult = NULL;
	BOOL bSaveError = FALSE;
	BYTE byCmdType		= RDM_DEBITE_CANCEL;
	BYTE byExecuteCode	= OP_FAILED;
	try
	{
		CString* pStrParam = (CString*)wParam;
		if (NULL == pStrParam)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		int iType = (int)lParam;

		pStrResult = new CString();
		if (NULL == pStrResult)
			throw - 4;

		CString strData;
		strData.Format(_T("%s"), *pStrParam);
		//��������
		CString strResult;
		strLog.Format(_T("[�豸�����߳�],���յ�ָ���ʼִ�г�������"));
		//������־
		GetGlobalApp()->RecordLog(strLog);
	
		/*
		CString find_key ="\"PayIdentifier\":\"";
		CString find_pay;
		int find_first = strData.Find(find_key);
		if(find_first!=-1)
		{
			int find_last = strData.Find("\"",find_first+find_key.GetLength());
			find_pay = strData.Mid(find_first+find_key.GetLength(),find_last-find_first-find_key.GetLength());
		}
		this->BreakDebit(find_pay);
		*/

		//ִ�г�������
		if (!m_pHTSDDeviceDll->DebitCancel(strData, 0x00))
			throw - 5;
		/*add 2017-05-19*/
		strResult = "0";
		pStrResult->Format(_T("%s"), strResult);
		//������Դ
		delete pStrParam;
		pStrParam = NULL;

		strLog.Format(_T("[�豸�����߳�],��������ִ�����"));

		//��ʶִ�гɹ�
		byExecuteCode	= OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[�豸�����߳�],���յ��Ĳ����쳣,ָ��Ϊ��"));
			break;
		case -2:
			strLog.Format(_T("[�豸�����߳�],ͨѶģ�鶯̬���쳣,ָ��Ϊ��"));
			break;
		case -3:
			strLog.Format(_T("[�豸�����߳�],��ȡͨѶģ�鶯̬��״̬�쳣"));
			break;
		case -4:
			strLog.Format(_T("[�豸�����߳�],�ڲ���Դ�����쳣"));
			break;
		case -5:
		{
			byExecuteCode = RCC_INNER_ERROR;//add 2017-07-04
			CString strError;
			m_pHTSDDeviceDll->GetLastErrorDesc(strError);
			strLog.Format(_T("[�豸�����߳�],ִ�г�������ʧ�ܣ�����ԭ��������%s"), strError);
			if(strError.Find("ȡ��")!=-1)
			{
				//byExecuteCode = OP_CANCLE;
			}
			
			else if (strError.Find("�˻�����") != -1)
			{
				byExecuteCode = OP_NO_MONEY;
			}
			else if (strError.Find("��ά��Ƿ�") != -1)
			{
				byExecuteCode = OP_QRCODE_ILLEGAL;
			}
			else if (strError.Find("��ά�볬����Ч��") != -1)
			{
				byExecuteCode = OP_QRCODE_OT;
			}
			else if (strError.Find("֧��ƾ֤�쳣") != -1)
			{
				byExecuteCode = OP_NO_ACCOUNT_INFO;
			}
			else if (strError.Find("�ۿʱ") != -1)
			{
				byExecuteCode = OP_DEBIT_OT;
			}
			else if (strError.Find("�������") != -1)
			{
				byExecuteCode = OP_PSW_ERR;
			}
			else if (strError.Find("��������") != -1)
			{
				byExecuteCode = OP_PARAM_ERR;
			}
			else if (strError.Find("�ϴν���δ����") != -1)
			{
				byExecuteCode = OP_LAST_TRADE_NOT_END;
			}
			else if (strError.Find("���δ��ʼ��") != -1)
			{
				byExecuteCode = OP_COM_NOT_INIT;
			}
			else if (strError.Find("���У��Ƿ�") != -1)
			{
				byExecuteCode = OP_COM_ILLEGAL;
			}
			else if (strError.Find("֧��ƽ̨�������") != -1)
			{
				byExecuteCode = OP_PAY_PLATFORM_ERR;
			}
		}
		break;
		}
		bSaveError = TRUE;
	}
	//������־
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog, bSaveError ? LOG_LEVEL_ERROR : LOG_LEVEL_RUN);
	}
	//֪ͨҵ������߳��Ѿ���ȡӲ������ҵ������
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)pStrResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::InitConfigDevice
//
// �������������ͳ�ʼ�����ݵ��ն�
//
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��4��17��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadDevice::InitConfigDevice(void)
{
	BOOL bReturn	= FALSE;
	BOOL bSaveError = FALSE;

	BYTE byCmdType		= RDM_DEVICE_INIT;
	BYTE byExecuteCode	= OP_FAILED;
	CString strLog;

	try
	{
		CString strDataString;

		Json::Value root;
		Json::Value DeviceParamContext;
		Json::Value Data;

		Data["ProvinceID"]			= Json::Value(m_sTranstParam.ProvinceID);
		Data["AreaID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.str_AreaID);
		//Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.RoadID);
		Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.str_RoadID);
		Data["StationID"]			= Json::Value(m_sTranstParam.LaneConfigInfo.str_StationID);
		Data["LaneID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.str_LaneID);
		Data["StationName"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationTab);
		Data["ServerType"]			= Json::Value(m_sTranstParam.LaneConfigInfo.STServerTpye);
		Data["ServerIP"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerIP);
		Data["ServerPort"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerPort);

		Data["ConfigLocalIPMode"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIPMode);
		Data["ConfigLocalIP0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0);
		Data["ConfigLocalSub0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0);
		Data["ConfigLocalGate0"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0);
		Data["ConfigLocalIP1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1);
		Data["ConfigLocalSub1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1);
		Data["ConfigLocalGate1"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1);
		Data["ConfigLocalDefGate"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate);
		Data["DevicePayMode"]		= Json::Value(3);
		Data["CurrentTime"]			= Json::Value(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));

		/*add 2017-05-19*/
		Data["DeviceDllVersion"]			= Json::Value(theApp.GetVersionOfFile("HTTWSDDevice.dll"));
		char szBuff[MAX_PATH] = {0};  
		GetModuleFileName(AfxGetStaticModuleState()->m_hCurrentInstanceHandle, szBuff, MAX_PATH);
		Data["NetPayDllVersion"]			= Json::Value(theApp.GetVersionOfFile(szBuff));
		//Data["NetPayDllVersion"]			= Json::Value(theApp.GetVersionOfFile("TWSDNetPay.dll"));

		/*add 2017-08*/
		Data["UpdatePort"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebUpdatePort);
		/*add 2018-01*/
		Data["UseAgencyPayDll"]= Json::Value(m_sTranstParam.LaneConfigInfo.UseAngencyDll);
		Data["ReturnPayChannelCodeUnify"]= Json::Value(m_sTranstParam.LaneConfigInfo.ReturnPayChannelCodeUnify);



		DeviceParamContext["Data"]		= Data;
		DeviceParamContext["Version"]	= Json::Value(_T("1.0"));
		root["DeviceParamContext"]		= DeviceParamContext;

		Json::FastWriter fast_writer;
		strDataString.Format(_T("%s"), fast_writer.write(root).c_str());

		if (NULL == m_pHTSDDeviceDll)
			throw - 1;

		strLog.Format(_T("[�豸�����߳�],�豸��ʼ������,��ʼ������:%s"), strDataString);
		GetGlobalApp()->RecordLog(strLog);
		if (!m_pHTSDDeviceDll->InitConfigDevice(strDataString))
			throw - 2;

		strLog.Format(_T("[�豸�����߳�],�豸��ʼ���������"));

		bReturn = TRUE;
		//��ʶִ�гɹ�
		byExecuteCode	= OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[�豸�����߳�],ͨѶģ�鶯̬���쳣,ָ��Ϊ��"));
			break;
		case -2:
		{
			CString strError;
			m_pHTSDDeviceDll->GetLastErrorDesc(strError);
			strLog.Format(_T("[�豸�����߳�],ִ���豸��ʼ������ʧ�ܣ�����ԭ��������%s"), strError);
		}
		break;
		}

		bSaveError = TRUE;
	}
	//������־
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog, bSaveError ? LOG_LEVEL_ERROR : LOG_LEVEL_RUN);
	}

	//֪ͨҵ������߳��Ѿ���ȡӲ������ҵ������
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)NULL);

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::fnGetNotifyFromCommModule
//
// �������������ڻ�ȡͨѶģ�����������Ļص�����
//
// ���������int iType
//         ��const std::string& strData
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��5��7��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::fnGetNotifyFromCommModule(int iType, const std::string& strData)
{
}

bool CThreadDevice::BreakDebit(const CString& payID)
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("�����Ч");
		return false;
	}
	return m_pHTSDDeviceDll->BreakDebit(payID);
}

bool CThreadDevice::Init_Dev()
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("�����Ч");
		return false;
	}
	return m_pHTSDDeviceDll->InitDev(fnGetNotifyFromCommModule);
}
bool CThreadDevice::Destroy_Dev()
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("�����Ч");
		return false;
	}
	return m_pHTSDDeviceDll->Destroy();
}

bool CThreadDevice::AgencyDebitQuery(const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult)
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("�����Ч");
		return false;
	}
	return m_pHTSDDeviceDll->AgencyDebitQuery(szParamContext, iFormatType,  iResult);
}


bool CThreadDevice::TransferImage(const std::string& szImageInfo)
{
    if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }
    return m_pHTSDDeviceDll->TransferImage(szImageInfo);   
}

bool CThreadDevice::TransTickNo(const std::string& para)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }
    return m_pHTSDDeviceDll->TransTickNo(para);   
}

bool CThreadDevice::HardwareTestGreenChannel(int testType,const std::string& testPara,std::string& result)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }

    return m_pHTSDDeviceDll->GreenChannelTest(testType,testPara,result);   
}

bool CThreadDevice::ExcuteComCommond(const std::string& hexStr)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }

    return m_pHTSDDeviceDll->ExcuteComCommond(hexStr);   
}

bool CThreadDevice::TakeComFeedbackCommond(CString& jsonStr)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }

    return m_pHTSDDeviceDll->TakeComFeedbackCommond(jsonStr);  
}


bool CThreadDevice::QueryTerminalSystemVersionStatus(int& status)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }
	return m_pHTSDDeviceDll->QueryTerminalSystemVersionStatus(status);
}
bool CThreadDevice::ExchangeTerminalSystem(const int& targetSystemVersion)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("�����Ч");
        return false;
    }
	return m_pHTSDDeviceDll->ExchangeTerminalSystem(targetSystemVersion);
}
