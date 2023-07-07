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

#include "sqlite3_agent.h"

// CThreadDevice

IMPLEMENT_DYNCREATE(CThreadDevice, CThreadBase)

CThreadDevice::CThreadDevice()
	: m_pHTSDDeviceDll(NULL)
	, m_iDeviceVersion(2)
{
	m_firmwareUpdateFlag = false;
	m_firmwareUpdateTime = 0;
	nLocalDBSpan = 300000;
	nArraySpan = 30000;

	dwLocalDBCheck = ::GetTickCount();
	dwArrayCheck = ::GetTickCount();

	m_csUpdateLDB	= _T("");
	m_csLDBTable	= _T("");
	m_csDevLog		= _T("");

	m_strDefaultGGText.Format(_T("�����ݺỶӭ��"));

	std::locale::global(std::locale(""));


	mShareMemNameOfDevId = "mem_dev_id";
	mhShareMemOfDevId = NULL;
	mpShareMemOfDevId = NULL;

	mhShareMemOfDevId = CreateFileMapping(
	                        INVALID_HANDLE_VALUE,    // �����ļ����
	                        NULL,                    // Ĭ�ϰ�ȫ����
	                        PAGE_READWRITE,          // �ɶ���д
	                        0,                       // ��λ�ļ���С
	                        100,                // ��λ�ļ���С
	                        mShareMemNameOfDevId.c_str()                   // �����ڴ�����
	                    );


	mpShareMemOfDevId = (char *)MapViewOfFile(
	                        mhShareMemOfDevId,            // �����ڴ�ľ��
	                        FILE_MAP_ALL_ACCESS, // �ɶ�д���
	                        0,
	                        0,
	                        100
	                    );


}

void CThreadDevice::Write_dev_id()
{
	if (mpShareMemOfDevId == NULL)
	{
		return ;
	}
	Sleep(1);
	strcpy(mpShareMemOfDevId, GetGlobalApp()->m_strTWProductSerialNo.GetBuffer(0));

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
	ReadWebConfig();

	return TRUE;
}

int CThreadDevice::ExitInstance()
{
	if (NULL != m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll->Destroy();
		delete m_pHTSDDeviceDll;
		m_pHTSDDeviceDll = NULL;
	}

	if (mpShareMemOfDevId != NULL)
	{
		UnmapViewOfFile(mpShareMemOfDevId);
		mpShareMemOfDevId = NULL;
	}
	if (mhShareMemOfDevId != NULL)
	{
		CloseHandle(mhShareMemOfDevId);
	}

	//MessageBox(0,"","",0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadDevice, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_GET_DATA, OnMsgStartGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_STOP_GET_DATA, OnMsgStopGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY, OnMsgGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_SHOW_DEAL_INFO, OnMsgShowDealInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI, OnMsgControlResultMMI)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_DEBIT_FLOW, OnMsgStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_ISSUE_DATA, OnMsgIssueData)

	ON_THREAD_MESSAGE(WM_THREAD_REMOTE_UPRGADE_RESULT, OnMsgRemoteUpgradeResult)



END_MESSAGE_MAP()


// CThreadDevice message handlers

BOOL CThreadDevice::OnIdle(LONG lCount)
{
	//GetGlobalApp()->RecordLog("OnIdle-------");
	if (m_bExit)
	{

		//MessageBox(0,"CThreadDevice","",0);

		c_update_device.m_aryUpdateInfo.RemoveAll();

		::PostQuitMessage(0);
		return FALSE;
	}

	CString strLog;
	if (NULL == m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll = new CHTSDDeviceDll();
	}
	else
	{
		//�ж��Ƿ���Ҫ�������
		static time_t tLastInitDllTime = 0x00;
		if ( (time(NULL) - tLastInitDllTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDllInitState())
			{
				CString strDllFilePath;
				strDllFilePath.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, FILE_NAME_DEVICE_DLL);
				m_pHTSDDeviceDll->Init(strDllFilePath);
				tLastInitDllTime = time(NULL);
			}
		}
		//�ж��Ƿ���Ҫ��ʼ�����
		static time_t tLastInitDevTime = 0x00;
		if ( (time(NULL) - tLastInitDevTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDevInitState())
			{
				m_pHTSDDeviceDll->InitDev(m_nThreadID, NULL, WM_THREAD_DEVICE_NOTIFY, m_sTranstParam.ProvinceID, 0x00);
				tLastInitDevTime = time(NULL);
			}
		}

		//����Ƿ���Ҫ��Ӳ�����и���ʱ��
		if (m_pHTSDDeviceDll->GetDevInitState())
		{
			static time_t tLastUpdateDeviceTime = 0x00;
			if ( (time(NULL) - tLastUpdateDeviceTime) > ::GetGlobalApp()->m_dwUpdateDevieTimeSpanTime)
			{
				BOOL bUpdate = UpdateDeviceTime();

				tLastUpdateDeviceTime = time(NULL);
			}
		}
		//ϵͳ����6��󣬳��������ն˵Ļ�ӭ�����Լ����λ��Ϣ
		if (m_pHTSDDeviceDll->GetDevInitState())
		{
			static bool bInitGGInfoDefault = false;
			static time_t tLaseResetGGInfoDefault = time(NULL);

			if (!bInitGGInfoDefault)
			{
				if ( (time(NULL) - tLaseResetGGInfoDefault) > 6)
				{
					m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
					::Sleep(100);
					SetPayErrorMsgInfo(0x00, true);
					tLaseResetGGInfoDefault = time(NULL);
					bInitGGInfoDefault = true;
				}
			}

			//����ն����Ϊ���ҵ�ǰ���ڽ������̵��У�����л�ȡ
			if (    GetGlobalApp()->m_strTWProductSerialNo.IsEmpty()
			        && !GetGlobalApp()->m_bIsTrading)
			{
				m_pHTSDDeviceDll->GetDeviceFirewareVer(GetGlobalApp()->m_strTWProductSerialNo);
				Write_dev_id();
				// GetGlobalApp()->RecordLog("ABC:"+GetGlobalApp()->m_strTWProductSerialNo);
			}

			//add 2018-01 ����ն˸վ����������������ʱ�䣬�ڽ��а汾�ŵĻ�ȡ
			if (m_firmwareUpdateFlag &&
			        (time(NULL) - m_firmwareUpdateTime) < 60 * 2
			   )
			{
				//GetGlobalApp()->RecordLog("dddddddddddddddddddddddddd");
			}
			else
			{
				if (    GetGlobalApp()->m_strTWProductFirmwareVer.IsEmpty()
				        && !GetGlobalApp()->m_bIsTrading)
				{					
					m_pHTSDDeviceDll->RealFirmwareVer(GetGlobalApp()->m_strTWProductFirmwareVer);
				}
				if (    GetGlobalApp()->m_strTWProductSlaveFirmwareVer.IsEmpty()
				        && !GetGlobalApp()->m_bIsTrading)
				{
					m_pHTSDDeviceDll->RealSlaveFirmwareVer(GetGlobalApp()->m_strTWProductSlaveFirmwareVer);
				}
				m_firmwareUpdateFlag = false;
			}


		}
		//��沥������������
		ITMSMain();
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

	::Sleep(10);
	//GetGlobalApp()->RecordLog("OnIdle++++");
	return  CThreadBase::OnIdle(lCount);
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
void CThreadDevice::OnMsgStartGetData(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	try
	{
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;
		//������ȡɨ�����ݹ���
		m_pHTSDDeviceDll->StartGetData((int)wParam);
		strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸����ɨ�룬��ʱʱ��:%d"), (int)wParam);
		//�����1.0�汾��Ӳ��������Ҫ����IO����(��ɨ��)
		if (1 == m_iDeviceVersion)
		{
			m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SCAN);
			strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸����ɨ�룬��ʱʱ��:%d"), (int)wParam);
		}
		//������־
		GetGlobalApp()->RecordLog(strLog);
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
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
void CThreadDevice::OnMsgStopGetData(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	try
	{
		int iChangeMMI = (int)lParam;

		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;
		//ֹͣ��ȡɨ�����ݹ���
		m_pHTSDDeviceDll->StopGetData(iChangeMMI);
		strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸ֹͣɨ�����"));

		//�ж��Ƿ�����NFC֧������
		if (::GetGlobalApp()->m_bNFCIsUse)
		{
			CNFCProtocol cNFCProtocol;
			CDVXMLUnipay xmlUnipay;
			CString strContext	= cNFCProtocol.ConstructStopExecuteCmd();
			CString strSendData = xmlUnipay.ConstructNFCFrameData(strContext, UMS_STOP);
			if (!m_pHTSDDeviceDll->IssuedData(strSendData))
				throw - 3;
			strLog.Format(_T("[�豸�����߳�],���յ�ָ������豸ֹͣ��ȡ֧��ƾ֤����(ֹͣɨ���ֹͣNFC����)�����ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}

		//������־
		GetGlobalApp()->RecordLog(strLog);
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
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_NOTIFY
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgGetData(WPARAM wParam, LPARAM lParam)
{
	CString strLog = _T("");
	BOOL bGetData = (BOOL)wParam;

	tagAccountInfo* pAccountInfo = NULL;
	//��������
	try
	{
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;

		//�ж��Ƿ��ȡ���ɹ�������
		//�汾1057 ������Ϊ�豸����
		if ( 1 == theApp.m_iDeviceType )
		{
			strLog.Format(_T("[�豸�����߳�],��ȡ������Ϊ���ײ��豸���ص�ɨ����Ϣ(%d)"), bGetData );
		}
		else
		{
			if (FALSE == bGetData)
				throw - 3;
		}

		//��ȡ����
		CString strData;
		int iDataType = 0x00;
		int iSrcType  = 0x00;
		if (!m_pHTSDDeviceDll->GetData(strData, iDataType, iSrcType))
			throw - 4;
		//�汾1057 ������Ϊ�豸����
		if ( 1 == theApp.m_iDeviceType )
		{
			strLog.Format(_T("[�豸�����߳�],��ȡ������Ϊ���ײ��豸��������(data:%s, Type:%d, Src:%d)"), strData, iDataType, iSrcType );
			GetGlobalApp()->RecordLog(strLog);
		}

		pAccountInfo = new tagAccountInfo();
		if (NULL == pAccountInfo)
			throw - 5;
		memset(pAccountInfo, 0, sizeof(tagAccountInfo));

		//��������
		pAccountInfo->DataType		= iDataType;
		pAccountInfo->DataSize		= strData.GetLength();
		pAccountInfo->SrcType		= iSrcType;
		memcpy_s(pAccountInfo->DataContext, _countof(pAccountInfo->DataContext), strData.GetBuffer(0), strData.GetLength());
		strData.ReleaseBuffer();

		strLog.Format(_T("[�豸�����߳�],��ȡ������(��Դ:%d)"), iSrcType );
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
		case -4:
			break;
		case -5:
			break;
		}
		bGetData = FALSE;
	}
	GetGlobalApp()->RecordLog(strLog);
	//֪ͨҵ������߳��Ѿ���ȡ�û���Ϣ
	::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_DEVICE_NOTIFY, (WPARAM)bGetData, (LPARAM)pAccountInfo);
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
	try
	{
		int iMMIType = (int)lParam;

		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		CString strData;
		strData.Format(_T("%s"), *pstrXML);

		//������ʾ������Ϣ
		int iID = 0x01;
		m_pHTSDDeviceDll->SetDisplayContext(iID, strData);

		//�ж��Ƿ���Ҫ�ָ����λ�Ĵ�����Ϣ
		if (0x04 == iMMIType)
		{
			//�ӳ�һ��ʱ�䣬Ĭ��100����
			::Sleep(::GetGlobalApp()->m_iClearMMIDelayTime);
			SetPayErrorMsgInfo();
		}

		//������Դ
		delete pstrXML;
		pstrXML = NULL;
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
void CThreadDevice::OnMsgStartDebitFlow(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	try
	{
		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		//�汾1057 ������Ϊ�豸����
		if ( 1 == theApp.m_iDeviceType )
		{
			strLog.Format(_T("[�豸�����߳�],��Ϊ�豸ɨ�����⴦������������StartGetData���ӿ�"));
			GetGlobalApp()->RecordLog(strLog);

			m_pHTSDDeviceDll->StartGetData( 30000 );// 30�볬ʱ
		}

		if (GetGlobalApp()->m_bIsExternMMIDetail)
		{
			//��λ��棨����������Ϣ��
			SetPayErrorMsgInfo();
			::Sleep(100);
		}

		CString strData;
		strData.Format(_T("%s"), *pstrXML);

		//�ж��Ƿ�����NFC֧������
		if (GetGlobalApp()->m_bNFCIsUse
		        /*add zhuyabing 2017-10-21*/
		        && GetGlobalApp()->m_bNFCSignInSuccess)
		{
			//�۷Ѳ������뱣֤ǩ���ɹ�
			// remove zhuybing 2017-10-21
			// if(!GetGlobalApp()->m_bNFCSignInSuccess)
			// 	throw -5;

			CDVXMLDebitFlow xmlDebit;
			if (!xmlDebit.Init(strData))
				throw - 6;
			//warning zhuyabing 2017-11-06 zhuyabing ��0Ԫ������£���Ӧ��ֱ���˳�������������ɨ��

			//if(0 == xmlDebit.Money)
			//	throw -7;

			CNFCProtocol cNFCProtocol;
			CDVXMLUnipay xmlUnipay;
			CString strContext = cNFCProtocol.ConstructStartConsumeCmd(xmlDebit.Money);
			CString strSendData = xmlUnipay.ConstructNFCFrameData(strContext, UMS_DEBITING);

			m_LastNFCPayData = strSendData;

			if (!m_pHTSDDeviceDll->IssuedData(strSendData))
				throw - 8;
			//��������֧��״̬
			GetGlobalApp()->SetUnipayWorkFlowState(UMS_DEBITING);
			//��¼��־
			strLog.Format(_T("[�豸�����߳�],���յ�ָ�ִ�пۿ����̲���(ɨ��ͷǽ�)�����ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}
		else
		{
			strLog.Format(_T("[�豸�����߳�],���յ�ָ�ִ�пۿ����̲���(ɨ��),���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}

		//���������ۿ�����
		int iID = 0x01;
		if (!m_pHTSDDeviceDll->SetDisplayContext(iID, strData))
			throw - 4;
		//������Դ
		delete pstrXML;
		pstrXML = NULL;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[�豸�����߳�],�ۿ����̲������ִ��󣬲���ָ��Ϊ��,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -2:
			strLog.Format(_T("[�豸�����߳�],�ۿ����̲������ִ���ͨѶ���ָ��Ϊ��,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -3:
			strLog.Format(_T("[�豸�����߳�],�ۿ����̲������ִ���ͨѶ���δ��ʼ��,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -4:
			strLog.Format(_T("[�豸�����߳�],�ۿ����̲������ִ���ִ���·�0x57ָ��ʧ��,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -5:
			strLog.Format(_T("[�豸�����߳�],���յ�ָ�ִ�пۿ����̲�����NFCģ��δǩ���ɹ������ν��׽�����ɨ�룬������ˢ��,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -6:
			strLog.Format(_T("[�豸�����߳�],���յ�ָ�ִ�пۿ����̲������������Ͽ۷����ݳ����쳣�����ν��ײ�����NFCˢ������,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -7:
			strLog.Format(_T("[�豸�����߳�],���յ�ָ�ִ�пۿ����̲��������ѽ��Ϊ0�����ν��ײ�����NFCˢ������,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -8:
			strLog.Format(_T("[�豸�����߳�],�ۿ����̲������ִ���ִ���·�NFCˢ��ָ��ʧ��,���ױ�ʶ:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		}
	}
	//������־
	GetGlobalApp()->RecordLog(strLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgControlResultMMI
//
// �������������յ���Ϣ��Ҫ��չʾ�ɷѽ����������
//
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgControlResultMMI(WPARAM wParam, LPARAM lParam)
{
	int iResultMode = (int)wParam;
	try
	{
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;

		if (GetGlobalApp()->m_bIsAllowUseNewMMIDetailMode)
		{
			//���°潻��ģʽ����
			switch (iResultMode)
			{
			case 1://�ɷѳɹ�
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SUCCESS);
				break;
			case 2://�ɷ�ʧ��
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case 3://�ָ���ʼ����
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
				break;

			case SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT:	//����������
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR:	//�������
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1:	//֧������Ч
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2:	//֧���볬ʱ
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_BL_ERR:		//����
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_BL_ERR);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_NET_ERR:	//���糬ʱ
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_NET_ERR);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_PL_STOP:	//��ͣ����
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_PL_STOP);
				break;
			default://�ָ�Ĭ����ʾ����
				SetPayErrorMsgInfo();
				break;
			}
		}
		else
		{
			//������Ӫ��汾���佻��ϸ��
			int iDelayTime = 5;
			switch (iResultMode)
			{
			case 1://�ɷѳɹ�
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SUCCESS);
				break;
			case 2://�ɷ�ʧ��
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case 3://�ָ���ʼ����
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
				break;

			case SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT:	//����������
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR:	//�������
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1:	//��ά��Ƿ�
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2:	//��ά�볬ʱ
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_BL_ERR:		//����
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_BL_ERR);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_NET_ERR:	//���糬ʱ
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_NET_ERR);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_PL_STOP:	//��ͣ����
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_PL_STOP);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			default://�ָ�Ĭ����ʾ����
				SetPayErrorMsgInfo();
				break;
			}
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::OnMsgIssueData
//
// ����������ִ���·����ݹ��ܲ���
//
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_NFC_ISSUE_DATA
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgIssueData(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString strFlowType;
	CString strRevData;
	CString strIssueData;
	CString* pstrXML = (CString*)wParam;
	try
	{
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;
		strRevData.Format(_T("%s"), *pstrXML);
		//��ȡ��������
		switch (lParam)
		{
		case UMS_TEST_LINK:
			strFlowType.Format(_T("��·����"));
			break;
		case UMS_SIGNING:
			strFlowType.Format(_T("ǩ��"));
			break;
		case UMS_DEBITING:
			strFlowType.Format(_T("�۷�"));
			break;
		case UMS_CORRECT:
			strFlowType.Format(_T("����"));
			break;
		case UMS_STOP:
			strFlowType.Format(_T("��ֹ�ϴβ���"));
			break;
		case UMS_PARA_QUERY:
			strFlowType.Format(_T("������ѯ"));
			break;
		}
		//�����·�����XML�ַ���
		CDVXMLUnipay cDVXMLUnipay;
		strIssueData = cDVXMLUnipay.ConstructNFCFrameData(strRevData, lParam);

		if (!m_pHTSDDeviceDll->IssuedData(strIssueData))
			throw - 4;

		strLog.Format(_T("[�豸�����߳�],NFC���ݽ����������·����ݽӿڳɹ�,��ǰ���̣�%s"), strFlowType);
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[�豸�����߳�],NFC���ݽ������̳����쳣�����յ��Ĳ���ָ��Ϊ��,��ǰ���̣�%s"), strFlowType);
			break;
		case -2:
			strLog.Format(_T("[�豸�����߳�],NFC���ݽ������̳����쳣��ͨѶ���ָ��Ϊ��,��ǰ���̣�%s"), strFlowType);
			break;
		case -3:
			strLog.Format(_T("[�豸�����߳�],NFC���ݽ������̳����쳣��ͨѶ���δ�ܳɹ���ʼ��,��ǰ���̣�%s"), strFlowType);
			break;
		case -4:
			strLog.Format(_T("[�豸�����߳�],NFC���ݽ������̳����쳣��ִ���·����ݲ���ʧ��,��ǰ���̣�%s"), strFlowType);
			break;
		}
	}
	//��¼��־
	GetGlobalApp()->RecordLog( strLog );
	//������Դ
	if (NULL != pstrXML)
	{
		delete pstrXML;
		pstrXML = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::UpdateDeviceTime
//
// ����������������Ӳ���豸����ʱ��
//
// ���������void
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadDevice::UpdateDeviceTime(void)
{
	//�ع��·�ʱ��XML
	CMarkup xmlTime;
	xmlTime.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
	xmlTime.AddElem( _T("DisplayContext") );//root
	xmlTime.IntoElem();

	xmlTime.AddElem( _T("Version"),	_T("1.0") );
	xmlTime.AddElem( _T("ProvinceID"),	::GetGlobalApp()->m_sTranstParam.ProvinceID );
	xmlTime.AddElem( _T("VerifyCode"),	0 );
	xmlTime.AddElem( _T("Data") );

	xmlTime.IntoElem();

	xmlTime.AddElem( _T("Type"),			0x02 );

	xmlTime.AddElem( _T("Money"),			0x00 );
	xmlTime.AddElem( _T("Weight"),			0x00 );
	xmlTime.AddElem( _T("VehicleType"),		0x00 );
	xmlTime.AddElem( _T("VehicleClass"),	0x00 );
	xmlTime.AddElem( _T("CurrentTime"),	CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") );

	xmlTime.OutOfElem();
	xmlTime.OutOfElem();

	CString strXMLString = xmlTime.GetDoc();

	int iID = 0x01;
	return m_pHTSDDeviceDll->SetDisplayContext(iID, strXMLString);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::SetPayErrorMsgInfo
//
// ����������������ʾ֧���쳣����Ϣ��ռ�ù��λ�ã�
//
// ���������IN const int& iErrorCode = 0
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��10��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::SetPayErrorMsgInfo(IN const int& iErrorCode, IN const bool& bForceDefault)
{
	static bool bIsNeedReset = false;
	CString strErrorMsg;
	//���������Ϣ
	switch (iErrorCode)
	{
	case 0://Ĭ����Ϣ
		strErrorMsg.Format(_T("%s"), m_strDefaultGGText);
		if (!bForceDefault)
		{
			if (!bIsNeedReset)
			{
				strErrorMsg.Empty();
			}
		}
		break;
	case SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT:
		strErrorMsg.Format(_T("��������"));
		break;
	case SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR:
		strErrorMsg.Format(_T("�������"));
		break;
	case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1:
		strErrorMsg.Format(_T("��ά��Ƿ�"));
		break;
	case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2:
		strErrorMsg.Format(_T("��ά�볬ʱ"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_BL_ERR:
		strErrorMsg.Format(_T("����"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NET_ERR:
		strErrorMsg.Format(_T("���糬ʱ"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_PL_STOP:
		strErrorMsg.Format(_T("��ͣ����"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NFC_01:
		strErrorMsg.Format(_T("��Ƭ��֧������"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NFC_02:
		strErrorMsg.Format(_T("��Ƭδ��ʼ��"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NFC_03:
		strErrorMsg.Format(_T("����������ͣ����"));
		break;
	}
	//����ָ��
	CString strCMD;
	CMarkup xmlShow;
	xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
	xmlShow.AddElem( _T("DisplayContext") );//root
	xmlShow.IntoElem();

	xmlShow.AddElem( _T("Version"),		_T("1.0") );
	xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
	xmlShow.AddElem( _T("VerifyCode"),	0 );

	xmlShow.AddElem( _T("Data") );
	xmlShow.IntoElem();
	xmlShow.AddElem( _T("Type"),		102 );
	xmlShow.AddElem( _T("Context"),		strErrorMsg );
	xmlShow.OutOfElem();

	xmlShow.OutOfElem();
	strCMD = xmlShow.GetDoc();
	strCMD.Replace(_T(" "), _T(""));
	strCMD.Replace(_T("\r"), _T(""));
	strCMD.Replace(_T("\n"), _T(""));

	//������ʾ������Ϣ
	if (   ( NULL != m_pHTSDDeviceDll)
	        && ( FALSE != m_pHTSDDeviceDll->GetDevInitState())
	        && ( FALSE == strErrorMsg.IsEmpty())
	   )
	{
		int iID = 0x01;
		if ( m_pHTSDDeviceDll->SetDisplayContext(iID, strCMD) )
		{
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[�豸�����߳�]:���´�����ʾ��Ϣ(%s)"), strErrorMsg));
		}
	}

	//ȷ���´��Ƿ���Ҫ�ظ����͹��λĬ����Ϣ
	if (0 != iErrorCode)
	{
		bIsNeedReset = true;
	}
	else
	{
		bIsNeedReset = false;
	}
}


void CThreadDevice::ReadWebConfig()
{
	CString m_csCfgFile = _T("");

	m_csCfgFile.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, FILE_NAME_APP_CONFIG);

	m_csUpdateLDB.Format(_T("%s\\TWSDNetPayLog\\TWITMS.db3"), m_sTranstParam.PathCommDir);

	m_csLDBTable.Format( _T("ITMSUpdate") );

	nLocalDBSpan	=	::GetPrivateProfileInt(_T("ITMS"),	_T("LocalDBSpan"),		300000,		m_csCfgFile );
	nArraySpan		=	::GetPrivateProfileInt(_T("ITMS"),	_T("ArraySpan"),		30000,		m_csCfgFile );
#ifdef _DEBUG
	nLocalDBSpan = 20000;
#endif
}




void CThreadDevice::ITMSMain()
{
	if ( !GetGlobalApp()->m_bIsTrading ) //�ǿۿ����
	{
		if ( c_update_device.m_aryUpdateInfo.GetSize() > 0 )
		{
			ArrayCheck();
		}
		else
		{
			LocalDBCheck();
		}
	}
}



void CThreadDevice::LocalDBCheck()
{
	DWORD	dwCurrent = ::GetTickCount();
	if ( dwCurrent >= dwLocalDBCheck ) //������ʱ��δ��49.7����������
	{
		if ( dwCurrent - dwLocalDBCheck >= nLocalDBSpan) //Ĭ�� 300 S���һ��
		{
			int iRet = 0;
			CString	csErr = _T("");
			LockFileUpdate();

			c_update_device.m_aryUpdateInfo.RemoveAll();
			iRet = GetLocalDB2Array(m_csUpdateLDB, m_csLDBTable, c_update_device, 1, csErr);
			FreeFileUpdate();

			if ( iRet < 0 )
			{
				m_csDevLog.Format(_T("[�豸�����߳�]: ���ز������ݿ���ȡ�����쳣(%s)"), csErr );
				GetGlobalApp()->RecordLog(m_csDevLog);
			}

			dwLocalDBCheck = ::GetTickCount();
		}
	}
	else//��ǰ��ȡ��ʱ��ƬС����һ�Σ��ѵ��������
	{
		dwLocalDBCheck = dwCurrent;	//�����ϴ�ʱ��Ƭ���ȴ��´�ѭ������
	}
}



void CThreadDevice::ArrayCheck()
{
	CString	csErr = _T("");
	int iSet = 0;

	DWORD	dwCurrent = ::GetTickCount();
	if ( dwCurrent >= dwArrayCheck ) //������ʱ��δ��49.7����������
	{
		if ( dwCurrent - dwArrayCheck >= nArraySpan) //Ĭ�� 60 S���һ��
		{
			m_csDevLog.Format(_T("[�豸�����߳�]: ����ָ����д���"));
			GetGlobalApp()->RecordLog(m_csDevLog);

			if ( c_update_device.m_aryUpdateInfo.GetSize() > 0 )
			{
				bool	bFind = false;
				int		iPos = 0;
				for (int i = 0; i < c_update_device.m_aryUpdateInfo.GetSize(); ++i)
				{
					if ( 0 == c_update_device.m_aryUpdateInfo[i].TransferMark )
					{
						bFind = true;
						iPos = i;
						break;
					}
				}
				if ( bFind ) //����δ���²���
				{
					//�ж��Ƿ��ն˻�ӭ�������ָ��
					if (0x03 == c_update_device.m_aryUpdateInfo[iPos].Type)
					{
						GGPublish( iPos );
					}
					else
					{
						CTime	ctCurDate = CTime::GetCurrentTime();
						CTime	ctBegDate( c_update_device.m_aryUpdateInfo[iPos].BeginDate );
						CTime	ctEndDate( c_update_device.m_aryUpdateInfo[iPos].EndDate );

						int nCurDate = ctCurDate.GetYear() * 10000 +	ctCurDate.GetMonth() * 100 +	ctCurDate.GetDay();
						int nBegDate = ctBegDate.GetYear() * 10000 +	ctBegDate.GetMonth() * 100 +	ctBegDate.GetDay();
						int nEndDate = ctEndDate.GetYear() * 10000 +	ctEndDate.GetMonth() * 100 +	ctEndDate.GetDay();

						if ( nBegDate <= nEndDate ) //��ʼ����С�ڵ��ڽ������ڣ�����Ϊͬһ�죩
						{
							if ( nCurDate < nBegDate ) //ʱ������δ�����ȴ�
							{
								::Sleep(20);
							}
							else if ( nCurDate > nEndDate ) //ʱ�������ѹ���ɾ��
							{
								//1051�汾���������ӶԲ����쳣�ı��ر�Ǹ��´���
								c_update_device.m_aryUpdateInfo[iPos].TransferMark = 1;//�����쳣Ҳ���ñ�ǣ����ٴ���ȴ�ͳһ����
								c_update_device.ErrorCount = 0;

								if (c_update_device.m_aryUpdateInfo[iPos].BeginDate > 0 && c_update_device.m_aryUpdateInfo[iPos].EndDate > 0)
								{
									csErr.Format( _T("[�豸�����߳�]: ��ȡ����Ϣ(%d, %s)ʱ������(Begin:%s; End:%s)�ѹ������ٴ���"), \
									              c_update_device.m_aryUpdateInfo[iPos].Type, c_update_device.m_aryUpdateInfo[iPos].InfoNo,
									              CTime(c_update_device.m_aryUpdateInfo[iPos].BeginDate).Format(_T("%Y-%m-%d %H:%M:%S")), CTime(c_update_device.m_aryUpdateInfo[iPos].EndDate).Format(_T("%Y-%m-%d %H:%M:%S")) );
								}
								else
								{
									csErr.Format( _T("[�豸�����߳�]: ��ȡ����Ϣ(%d, %s)ʱ������(Begin:%I64d; End:%I64d)�ѹ������ٴ���"), \
									              c_update_device.m_aryUpdateInfo[iPos].Type, c_update_device.m_aryUpdateInfo[iPos].InfoNo,
									              c_update_device.m_aryUpdateInfo[iPos].BeginDate, c_update_device.m_aryUpdateInfo[iPos].EndDate);
								}
								GetGlobalApp()->RecordLog( csErr );
							}
							else//ʱ���������������в���
							{
								GGPublish( iPos );
							}
						}
						else
						{
							//1051�汾���������ӶԲ����쳣�ı��ر�Ǹ��´���
							c_update_device.m_aryUpdateInfo[iPos].TransferMark = 1;//�����쳣Ҳ���ñ�ǣ����ٴ���ȴ�ͳһ����
							c_update_device.ErrorCount = 0;

							csErr.Format( _T("[�豸�����߳�]: ��ȡ����Ϣ(%d, %s)ʱ������(Begin:%I64d; End:%I64d)�쳣�����ٴ���"), \
							              c_update_device.m_aryUpdateInfo[iPos].Type, c_update_device.m_aryUpdateInfo[iPos].InfoNo,
							              c_update_device.m_aryUpdateInfo[iPos].BeginDate, c_update_device.m_aryUpdateInfo[iPos].EndDate );
							GetGlobalApp()->RecordLog( csErr );
						}
					}
				}
				else//�����Ѿ�ȫ������
				{
					int iRet = 0;

					LockFileUpdate();
					iRet = SetLocalDBTransMark( m_csUpdateLDB, m_csLDBTable, c_update_device, csErr );
					FreeFileUpdate();

					if ( 0 == iRet )
					{
						c_update_device.ErrorCount = 0;
						c_update_device.m_aryUpdateInfo.RemoveAll();

						GetGlobalApp()->m_pThreadManage->m_pThreadUpdate->PostThreadMessage(WM_THREAD_UPDATE_CONFIRM, (WPARAM)(LPCTSTR)c_update_device.UpdateCode, (LPARAM)0);
					}
					else
					{
						c_update_device.ErrorCount += 1;
					}

					if ( c_update_device.ErrorCount > 3 )
					{
						LockFileUpdate();
						::DeleteFile( m_csUpdateLDB );
						FreeFileUpdate();

						c_update_device.ErrorCount = 0;
						c_update_device.m_aryUpdateInfo.RemoveAll();

						csErr.Format( _T("[�豸�����߳�]: ִ����Ϣ�����쳣�������Σ�ɾ����ǰ������Ϣ!") );
						GetGlobalApp()->RecordLog( csErr );
					}
				}
			}

			dwArrayCheck = ::GetTickCount();
		}
	}
	else//��ǰ��ȡ��ʱ��ƬС����һ�Σ��ѵ��������
	{
		dwArrayCheck = dwCurrent;	//�����ϴ�ʱ��Ƭ���ȴ��´�ѭ������
	}
}



void CThreadDevice::GGPublish(int nPos)
{
	CString csErr = _T("");
	CString csData = _T("");

	//XML����
	try
	{
		if ( c_update_device.m_aryUpdateInfo.GetSize() <= nPos )
		{
			c_update_device.m_aryUpdateInfo.RemoveAll();
			csErr.Format( _T("[�豸�����߳�]: ����ָ������쳣��ɾ�������ԣ�") );
			GetGlobalApp()->RecordLog( csErr );
			return;
		}

		int nType = c_update_device.m_aryUpdateInfo[nPos].Type;
		switch ( nType )
		{
		case 1:			nType = 102;			break;		//���ֹ��
		case 2:			nType = 103;			break;		//�������
		case 3:			nType = 104;			break;		//���ƻ�ӭ����
		default:		nType = 102;			break;
		}

		CMarkup xmlShow;
		xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
		xmlShow.AddElem( _T("DisplayContext") );//root
		xmlShow.IntoElem();

		xmlShow.AddElem( _T("Version"),		_T("1.0") );
		xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
		xmlShow.AddElem( _T("VerifyCode"),	0 );

		xmlShow.AddElem( _T("Data") );
		xmlShow.IntoElem();
		xmlShow.AddElem( _T("Type"),		nType );
		xmlShow.AddElem( _T("Context"),		c_update_device.m_aryUpdateInfo[nPos].Content );
		xmlShow.OutOfElem();

		xmlShow.OutOfElem();
		csData = xmlShow.GetDoc();

		//����Ĭ�Ϲ������
		if (    (0 != strlen(c_update_device.m_aryUpdateInfo[nPos].Content))
		        && (102 == nType)
		   )
		{
			m_strDefaultGGText.Format(_T("%s"), c_update_device.m_aryUpdateInfo[nPos].Content);
			m_strDefaultGGText.TrimLeft();
			m_strDefaultGGText.TrimRight();
		}
	}
	catch (...)
	{
		csErr.Format( _T("[�豸�����߳�]: ���첥��XML����δ֪�쳣: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csErr );
		return;
	}


	//�ײ������
	try
	{
		if (NULL == m_pHTSDDeviceDll)
		{
			return;
		}
		if (!m_pHTSDDeviceDll->GetDevInitState())
		{
			return;
		}

		csData.Replace(_T("\r"), _T(""));
		csData.Replace(_T("\n"), _T(""));

		//������ʾ������Ϣ
		int iID = 0x01;
		if ( m_pHTSDDeviceDll->SetDisplayContext(iID, csData) )
		{
			c_update_device.m_aryUpdateInfo[nPos].TransferMark = 1;//�����ɹ�
			c_update_device.ErrorCount = 0;

			m_csDevLog.Format(_T("[�豸�����߳�]: ����ָ��(%s)���ͳɹ�"), csData );
			GetGlobalApp()->RecordLog(m_csDevLog);
		}
		else
		{
			c_update_device.ErrorCount += 1;

			m_csDevLog.Format(_T("[�豸�����߳�]: ����ָ��(%s)����ʧ��"), csData );
			GetGlobalApp()->RecordLog(m_csDevLog);
		}

		if ( c_update_device.ErrorCount > 3 )
		{
			LockFileUpdate();
			::DeleteFile( m_csUpdateLDB );
			FreeFileUpdate();

			c_update_device.ErrorCount = 0;
			c_update_device.m_aryUpdateInfo.RemoveAll();

			csErr.Format( _T("[�豸�����߳�]: ִ����Ϣ�����쳣�������Σ�ɾ����ǰ������Ϣ!") );
			GetGlobalApp()->RecordLog( csErr );
		}

	}
	catch (...)
	{
		csErr.Format( _T("[�豸�����߳�]: ִ����Ϣ��������δ֪�쳣: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csErr );
		return;
	}
}


int CThreadDevice::GetLocalDB2Array(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, int nType, CString& csError)
{
	int	nReturn = 0;
	int rc, i, ncols;
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *tail;
	CString csSQL = _T("");

	errno_t err = 0;
	if ((err = _access_s( strDBFile, 0 )) == 0 )
	{
		rc = sqlite3_agent::sqlite3_open_fun( strDBFile, &db);	//SQLITE_OK
		if (rc != SQLITE_OK)
		{
			csError.Format( _T("Can't open DB(%s) database: %s\n"), strDBFile, sqlite3_agent::sqlite3_errmsg_fun(db) );
			sqlite3_agent::sqlite3_close_fun(db);
			return -1;
		}

		if ( 0 == nType ) //����������ѡȡ
		{
			csSQL.Format( _T(" select * from %s limit 9; "), strTBName);//Ŀǰ���ֻ������������
		}
		else//����ѡȡ
		{
			csSQL.Format( _T(" select * from %s where (TransferMark&1)<>1 limit 9; "), strTBName);//Ŀǰ���ֻ������������
		}
		rc = sqlite3_agent::sqlite3_prepare_fun(db, csSQL.GetBuffer(0), csSQL.GetLength(), &stmt, &tail);	//SQLITE_OK
		csSQL.ReleaseBuffer();
		if (rc != SQLITE_OK)
		{
			csError.Format( _T(" DB(%s) table(%s) select SQL prepare error: %s\n"), strDBFile, strTBName, sqlite3_agent::sqlite3_errmsg_fun(db) );
			sqlite3_agent::sqlite3_close_fun(db);
			return -2;
		}

		rc = sqlite3_agent::sqlite3_step_fun(stmt);			//SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW, SQLITE_ERROR, SQLITE_MISUSE
		ncols = sqlite3_agent::sqlite3_column_count_fun(stmt);

		if ( rc == SQLITE_ROW)	//����һ������
		{
			do
			{
				CString csDateName = _T("");
				tagUpdateInfo	sData;
				::memset(&sData, 0, sizeof(tagUpdateInfo));

				for (i = 0; i < ncols; ++i)
				{
					//��ȡ��ԭʼ���ƣ��� sqlite3_column_origin_name��������_dll sqlite3_column_name_dll ��ȡ��ʱ����
					csDateName.Format( _T("%s"), sqlite3_agent::sqlite3_column_origin_name_fun(stmt, i) );

					//InfoType int NOT NULL,	InfoNo varchar(16),	InfoName varchar(32),	InfoVersion varchar(16),
					if ( 0 == csDateName.CompareNoCase("InfoType") )			{	sData.Type = sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else if ( 0 == csDateName.CompareNoCase("InfoNo") )		{::sprintf(sData.InfoNo, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					else if ( 0 == csDateName.CompareNoCase("InfoName") )	{::sprintf(sData.InfoName, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					else if ( 0 == csDateName.CompareNoCase("InfoVersion") )	{::sprintf(sData.InfoVersion, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}

					//UpdateTime datetime,	LoadTime datetime,	BeginDate datetime,	EndDate datetime,
					else if ( 0 == csDateName.CompareNoCase("UpdateTime") )	{	sData.UpdateTime	= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}
					else if ( 0 == csDateName.CompareNoCase("LoadTime") )	{	sData.LoadTime		= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}
					else if ( 0 == csDateName.CompareNoCase("BeginDate") )	{	sData.BeginDate		= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}
					else if ( 0 == csDateName.CompareNoCase("EndDate") )		{	sData.EndDate		= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}

					//IsCondense int,	FileType int,	Content varchar(128),	MD5 varchar(40),
					else if ( 0 == csDateName.CompareNoCase("IsCondense") )	{	sData.IsCondense = sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else if ( 0 == csDateName.CompareNoCase("FileType") )	{	sData.FileType = sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else if ( 0 == csDateName.CompareNoCase("Content") )		{::sprintf(sData.Content, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					else if ( 0 == csDateName.CompareNoCase("MD5") )			{::sprintf(sData.MD5, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}

					//Spare int,	SpareString varchar(128),	TransferMark int,
					else if ( 0 == csDateName.CompareNoCase("Spare") )		{	sData.Spare	= sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					//	else if( 0 == csDateName.CompareNoCase("SpareString") )	{::sprintf(sData.sp, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					//	else if( 0 == csDateName.CompareNoCase("TransferMark") ){	sData.TransferMark= sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else
					{
						// default:	in this case , do nothing!
					}
				}

				rc = sqlite3_agent::sqlite3_step_fun(stmt);
				nReturn += 1;
				//���ε�Զ�̲��أ��������/������
				if (3 == sData.Type)
				{
					sData.TransferMark = 0;	//���ڶ����е�����Ϳ���
					cUpdate.m_aryUpdateInfo.Add( sData );
				}
			}
			while (rc == SQLITE_ROW);
		}
		else //SQLITE_DONE, SQLITE_BUSY, SQLITE_ERROR	SQLite���� �����ݡ�æ�����ߴ���
		{
			if ( rc == SQLITE_DONE )
			{
				nReturn = 0;	// no data to transfer
			}
			else
			{
				csError.Format( _T(" DB(%s) table(%s) SQL step error: %s\n"), strDBFile, strTBName, sqlite3_agent::sqlite3_errmsg_fun(db) );
				nReturn = -3;	// error in the select
			}
		}

		sqlite3_agent::sqlite3_finalize_fun(stmt);	//SQLITE_OK	SQLITE_ABORT	SQLITE_BUSY
		sqlite3_agent::sqlite3_close_fun(db);		//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY
	}
	else
	{
		nReturn = -9;
	}

	return nReturn;
}

int CThreadDevice::SetLocalDBTransMark(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, CString& csError )
{
	sqlite3 *db;
	char *zErr;
	int rc;
	CString csSQL = _T("");
	CString csTemp = _T("");

	errno_t err = 0;
	if ((err = _access_s( strDBFile, 0 )) == 0 )
	{
		rc = sqlite3_agent::sqlite3_open_fun( strDBFile, &db);
		if ( rc != SQLITE_OK )
		{
			csError.Format( _T("Can't open DB(%s) database: %s\n"), strDBFile, sqlite3_agent::sqlite3_errmsg_fun(db) );
			sqlite3_agent::sqlite3_close_fun(db);
			return -1;
		}

		//update the table		PRIMARY KEY(StartTime,SerialNum,DeviceID))
		for (int i = 0; i < cUpdate.m_aryUpdateInfo.GetSize(); ++i)
		{
			csSQL.Format( _T(" update %s set TransferMark=(TransferMark | 1) where InfoType=%d and InfoNo='%s' ;"), \
			              strTBName, cUpdate.m_aryUpdateInfo[i].Type, cUpdate.m_aryUpdateInfo[i].InfoNo );
			rc = sqlite3_agent::sqlite3_exec_fun(db, csSQL.GetBuffer(0), NULL, NULL, &zErr);
			csSQL.ReleaseBuffer();

			if ( rc != SQLITE_OK )
			{
				if (zErr != NULL)
				{
					csError.Format( _T(" DB(%s) table(%s) update transmark SQL error: %s"), strDBFile, strTBName, zErr );
					sqlite3_agent::sqlite3_free_fun(zErr);
				}

				sqlite3_agent::sqlite3_close_fun(db);	//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY
				return -3;

			}
			else
			{
				continue;
			}
		}
		sqlite3_agent::sqlite3_close_fun(db);
		return 0;
	}
	else
	{
		return -9;	// no database
	}
}


void CThreadDevice::RestartNFCPay()
{
	if (!m_pHTSDDeviceDll->IssuedData(m_LastNFCPayData))
		return ;
	//��������֧��״̬
	GetGlobalApp()->SetUnipayWorkFlowState(UMS_DEBITING);
}

void CThreadDevice::RemoteControlFirmware(const std::string&para)
{
	m_pHTSDDeviceDll->UpgradeFirware(para.c_str());
}

afx_msg void CThreadDevice::OnMsgRemoteUpgradeResult(WPARAM wParam, LPARAM lParam)
{	//	�̼��������
	//��ȡ���̼������������Ҫ���»�ȡӲ���汾��.
	theApp.m_strTWProductFirmwareVer = ""; //�˴��¼ӵı���������m_strTWProductFirmwareVerԭ�е�ʹ�����򲻻ᷢ����ͻ����˲���ʹ�ÿ�����Դ����Ӷ��߳̿��ƣ�Ҳ��Ը��ӣ�
	theApp.m_strTWProductSlaveFirmwareVer = "";

	m_firmwareUpdateFlag = true;
	m_firmwareUpdateTime = time(NULL);
	::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_REMOTE_UPRGADE_RESULT, wParam, lParam);
}