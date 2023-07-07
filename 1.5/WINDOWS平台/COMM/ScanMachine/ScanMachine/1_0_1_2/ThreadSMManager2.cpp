/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadSMManager.cpp
 * ժ    Ҫ��ɨ���������߳��࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
// ThreadSMManager.cpp : implementation file
//

#include "stdafx.h"
#include "ThreadSMManager2.h"
#include "ScanMachine.h"
extern CScanMachineApp theApp;

// CThreadSMManager2

IMPLEMENT_DYNCREATE(CThreadSMManager2, CThreadBase)

CThreadSMManager2::CThreadSMManager2()
	:m_bOpen(FALSE)
	,m_bCOMLastOpenState(FALSE)
	,m_pCOM(NULL)
	,m_tLastRecordCOMStateTime(0)
	,m_hSyncComData(INVALID_HANDLE_VALUE)
	,m_lpReceiveDataBuff(NULL)
	,m_dwReceiveDataIndex(0)

	,m_pSMProtocol(NULL)
	,m_dwScanStartTime(0)
	,m_dwSpanDelayTimeForControlLight(250)
	,m_bAllowCloseScanTipLight(FALSE)
	,m_iCheckHeartErrorSpanTime(30)
	,m_bSerialCommError(FALSE)

	,m_tNoDataFromCOMLastTime(0)

	,m_iNoNewDataTime(20)
	,m_bPrintCommandDataInLog(FALSE)
	,m_tLastComReadDataTime(0)
	,m_tLastComWriteDataTime(0)
	,m_iResetCOMErrorSpanTime(0)
	,m_bAbandonEchoData(TRUE)
{
	memset(&m_sCOMState,0,sizeof(tagSMInnerDeviceState));
	memset(&m_sHeartState,0,sizeof(tagSMInnerDeviceState));

	m_sHeartState.StateType = 2;

	m_strPathLocalConfigFile.Format(_T("%s\\%s"), theApp.csAppDir, FILE_NAME_DEVICE_SM_CONFIG);

	m_dwSMState = 0;
	m_dwMessageID = 0;
	::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
	m_sSMResult.ScanResult = -9;

	::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
}

CThreadSMManager2::~CThreadSMManager2()
{
}

BOOL CThreadSMManager2::InitInstance()
{
	m_pSMProtocol = new CHTSMDeviceControl();

	ASSERT(m_pSMProtocol);

	m_pSMProtocol->SetParam(m_nThreadID);

	fnReadSpecailConfig();

	m_lpReceiveDataBuff = new BYTE[m_sLocalCOM.InBufferSize];

	ASSERT(m_lpReceiveDataBuff);

	memset(m_lpReceiveDataBuff,0,m_sLocalCOM.InBufferSize);

	m_pCOM = new CSerialCom();
	if(NULL != m_pCOM)
	{
		m_pCOM->SetMain(m_nThreadID);
		m_pCOM->SetNotifyNum(1);
	}

	CString strSyncComDataHandleName;
	strSyncComDataHandleName.Format(_T("TOLL_LANE_COM_SYNC_DLL_%d"),m_nThreadID);
	m_hSyncComData = ::CreateEvent(NULL,TRUE,FALSE,strSyncComDataHandleName);

	CString strLog;
	m_pSMProtocol->InitInnerDevice(strLog);

	m_tLastComReadDataTime = time(NULL);

	return TRUE;
}

int CThreadSMManager2::ExitInstance()
{
	fnReleaseResource();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadSMManager2, CThreadBase)

	ON_THREAD_MESSAGE(WM_THREAD_SM_START_SCAN,OnMsgStartScan)
	ON_THREAD_MESSAGE(WM_THREAD_SM_STOP_SCAN,OnMsgStopScan)
	ON_THREAD_MESSAGE(WM_THREAD_SM_CONTROL_BOARD,OnMsgControlSerialBoard)
	ON_THREAD_MESSAGE(WM_THREAD_SM_SET_LIGHT_MODE,OnMsgSetLightMode)
	ON_THREAD_MESSAGE(WM_THREAD_SM_SET_DISPLAY,OnMsgSetDisplay)
	ON_THREAD_MESSAGE(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,OnMsgDealResponseForProtocolProcessResult)

	ON_THREAD_MESSAGE(ON_COM_RECEIVE,OnMsgReceiveDataFromCOM)
	ON_THREAD_MESSAGE(ON_COM_TXEMPTY,OnMsgDataSendEnd)
	ON_THREAD_MESSAGE(ON_COM_ERROR,OnMsgComError)

END_MESSAGE_MAP()


DWORD CThreadSMManager2::GetSMState()
{
	return m_dwSMState;
}

void CThreadSMManager2::SetMessageID(DWORD dwMessage)
{
	m_dwMessageID = dwMessage;
}

void CThreadSMManager2::GetSMResult(tagSMInnerDeviceState& sState)
{
	::memset(&sState, 0, sizeof(tagSMInnerDeviceState));
	::memcpy(&sState, &m_sSMResult, sizeof(tagSMInnerDeviceState));
	//��ȡ�������Ժ����ԭ��������
	::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
}

void CThreadSMManager2::SetDisplay(tagScanDisplay sDisplay)
{
	::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
	::memcpy(&sScanDisplay, &sDisplay, sizeof(tagScanDisplay));
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::fnReleaseResource
//     
// ������������������������Դ
//     
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnReleaseResource()
{
	//�ص�
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
	::Sleep(10);
	//�رմ���
	CloseCom();

	if(NULL != m_pCOM)
	{
		delete m_pCOM;
		m_pCOM = NULL;
	}

	if(NULL != m_lpReceiveDataBuff)
	{
		delete[] m_lpReceiveDataBuff;
		m_lpReceiveDataBuff = NULL;
	}

	if(NULL != m_pSMProtocol)
	{
		delete m_pSMProtocol;
		m_pSMProtocol = NULL;
	}

	if(INVALID_HANDLE_VALUE != m_hSyncComData)
		::CloseHandle(m_hSyncComData);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::fnReadSpecailConfig
//     
// ������������ȡɨ��������������Ϣ
//     
// ���������void
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnReadSpecailConfig(void)
{
	int iSMCount = ::GetPrivateProfileInt(_T("SMControl"),_T("Count"),1,m_strPathLocalConfigFile);
	//��ȡ��������쳣�ļ��ʱ�䣨��λ���룩
	m_iCheckHeartErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"),_T("CheckHeartErrorSpanTime"),30,m_strPathLocalConfigFile);
	if(  (m_iCheckHeartErrorSpanTime<=0) || (m_iCheckHeartErrorSpanTime>=240) )
	{
		m_iCheckHeartErrorSpanTime = 60;
	}

	//�Ƿ��շ����ݶ���ʾ����־�ļ�����
	m_bPrintCommandDataInLog = (BOOL)::GetPrivateProfileInt(_T("SMControl"),_T("PrintCommandDataInLog "),0,m_strPathLocalConfigFile);;
	//�Ƿ����ظ�����(�����һ�����ݶ���)
	m_bAbandonEchoData       = (BOOL)::GetPrivateProfileInt(_T("SMControl"),_T("AbandonEchoData "),1,m_strPathLocalConfigFile);
	//��ȡ�����쳣����Ҫ��λ�ļ��ʱ��
	m_iResetCOMErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"),_T("ResetCOMErrorSpanTime"),20,m_strPathLocalConfigFile);
	//��ȡ�����ַ�֮���ʱ�������������ʱ��ֵ����Ϊ���ݴ�����ϣ�
	m_iNoNewDataTime		 = ::GetPrivateProfileInt(_T("SMControl"),_T("NoNewDataTime"),20,m_strPathLocalConfigFile);
	
	CString strSectionName(_T("SMControl"));

	memset(&m_sLocalCOM,0,sizeof(tagSMInnerDeviceInit));

	strSectionName.Format(_T("SerialParam"));
	//���뻺������С
	m_sLocalCOM.InBufferSize	= ::GetPrivateProfileInt(strSectionName,_T("InBufferSize"),1024,m_strPathLocalConfigFile);
	//�����������С
	m_sLocalCOM.OutBufferSize	= ::GetPrivateProfileInt(strSectionName,_T("OutBufferSize"),1024,m_strPathLocalConfigFile);
	//���ں�
	m_sLocalCOM.Port			= ::GetPrivateProfileInt(strSectionName,_T("Port"),1,m_strPathLocalConfigFile);
	if(m_sLocalCOM.Port>64)
		m_sLocalCOM.Port = 64;
	//������
	m_sLocalCOM.BaudRate		= ::GetPrivateProfileInt(strSectionName,_T("BaudRate"),9600,m_strPathLocalConfigFile);
	//У��λ
	m_sLocalCOM.ParityBit		= ::GetPrivateProfileInt(strSectionName,_T("ParityBit"),0,m_strPathLocalConfigFile);
	//ֹͣλ
	m_sLocalCOM.StopBit			= ::GetPrivateProfileInt(strSectionName,_T("StopBit"),1,m_strPathLocalConfigFile);
	//����λ
	m_sLocalCOM.DataBit			= ::GetPrivateProfileInt(strSectionName,_T("DataBit"),8,m_strPathLocalConfigFile);

	m_sCOMState.StateType = 1;
	m_sCOMState.DeviceID  = m_sLocalCOM.Port;

	strSectionName.Format(_T("Device"));

	//ɨ�����豸����
	m_pSMProtocol->m_cInnerDeviceInfo.SMType					= ::GetPrivateProfileInt(strSectionName,_T("DeviceType"),1,m_strPathLocalConfigFile);
	//��Ч���ݳ���	
	m_pSMProtocol->m_cInnerDeviceInfo.EffectiveDataLen			= ::GetPrivateProfileInt(strSectionName,_T("EffectiveDataLen"),12,m_strPathLocalConfigFile);
	//���δ��ڶ�ȡ���ʱ�䣬��λ������
	m_pSMProtocol->m_cInnerDeviceInfo.SingleReadDataSpanTime	= ::GetPrivateProfileInt(strSectionName,_T("SingleReadDataSpanTime"),12,m_strPathLocalConfigFile);
	//Ԥ�贮�ڵȴ���ʱ,��λ������
	m_pSMProtocol->m_cInnerDeviceInfo.SMRoundTimeOutValue		= ::GetPrivateProfileInt(strSectionName,_T("RoundTimeOutValue"),12,m_strPathLocalConfigFile);
	//ɨ����Ĭ�϶�ȡ�ȴ���ʱ����λ������
	m_pSMProtocol->m_cInnerDeviceInfo.SMDefaultScanTimeOutValue = ::GetPrivateProfileInt(strSectionName,_T("DefaultScanTimeOut"),12,m_strPathLocalConfigFile);
	//ɨ��/ָֹͣ��������֮��ļ��ʱ��
	m_dwSpanDelayTimeForControlLight							= ::GetPrivateProfileInt(strSectionName,_T("DelayTimeForControlLight"),200,m_strPathLocalConfigFile);
	//��ȡɨ�����ݺ��Ƿ�ر�ɨ����ʾ��
	m_bAllowCloseScanTipLight									= (BOOL)::GetPrivateProfileInt(strSectionName,_T("AllowCloseScanTipLight"),0,m_strPathLocalConfigFile);
	
	for(int i = 0;i<iSMCount;++i)
	{
		tagSMInnerDeviceInit sSMInit = {0};

		tagSMInnerDeviceState sState={0};

		strSectionName.Format(_T("SMConfig%d"),i+1);
		
		//�豸ID����
		sSMInit.DeviceID		= ::GetPrivateProfileInt(strSectionName,_T("DeviceID"),1,m_strPathLocalConfigFile);
		//���ں�
		sSMInit.COM.Port		= ::GetPrivateProfileInt(strSectionName,_T("Port"),1,m_strPathLocalConfigFile);
		//������
		sSMInit.COM.BaudRate	= ::GetPrivateProfileInt(strSectionName,_T("BaudRate"),9600,m_strPathLocalConfigFile);
		//У��λ
		sSMInit.COM.ParityBit	= ::GetPrivateProfileInt(strSectionName,_T("ParityBit"),0,m_strPathLocalConfigFile);
		//ֹͣλ
		sSMInit.COM.StopBit		= ::GetPrivateProfileInt(strSectionName,_T("StopBit"),1,m_strPathLocalConfigFile);
		//����λ
		sSMInit.COM.DataBit		= ::GetPrivateProfileInt(strSectionName,_T("DataBit"),8,m_strPathLocalConfigFile);
	
		sState.DeviceID = sSMInit.DeviceID;

		sState.LastInitTime = time(NULL)-25;
		//����һ���ڹ��豸
		BOOL bExists = FALSE;
		for(INT_PTR j = 0;j<m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.GetSize();++j)
		{
			if(sState.DeviceID == m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.GetAt(j).DeviceID)
			{
				bExists = TRUE;
				break;
			}
		}

		if(!bExists)
		{
			m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.Add(sSMInit);
			
			m_pSMProtocol->m_cInnerDeviceInfo.SMStateList.Add(sState);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OpenCom
//     
// ������������ʼ����������
//     
// �����������
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager2::OpenCom()
{	
	if(NULL != m_pCOM)
	{
		m_pCOM->SetNotifyNum(1);
		m_bOpen = m_pCOM->Open(m_sLocalCOM.Port);

		CString strDesc = theApp.FmtStr(_T("���ش���[�˿ں�:%d,������:%d,ֹͣλ:%d,У��λ:%d,����λ:%d]��%s"),m_sLocalCOM.Port,m_sLocalCOM.BaudRate,m_sLocalCOM.StopBit,m_sLocalCOM.ParityBit,m_sLocalCOM.DataBit,m_bOpen?_T("�ɹ�"):_T("ʧ��"));
		_tcscpy_s(m_sCOMState.StateDesc,_countof(m_sCOMState.StateDesc),strDesc.GetBuffer(0));
		strDesc.ReleaseBuffer();
		m_sCOMState.IsInit = m_bOpen;
		//������־
		//theApp.RecordMsgTemp(strDesc);
	}
	return m_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::CloseCom
//     
// �����������رմ�������
//     
// ����������޲���
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager2::CloseCom()
{
	BOOL bReturn = FALSE;
	if(NULL != m_pCOM)
	{
		m_pCOM->Close();
		m_bOpen = FALSE;
		bReturn = TRUE;
	}
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::WriteDataToComm
//     
// ����������������д���ݣ��ֽ�����ʽ
//     
// ���������IN const LPBYTE byData
//         ��IN const DWORD& dwCommandDataSize
// ���������DWORD
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CThreadSMManager2::WriteDataToComm(IN const LPBYTE byData,IN const DWORD& dwCommandDataSize)
{
	DWORD dwWriteDataLen = 0;
	if(NULL != m_pCOM)
	{
		dwWriteDataLen = m_pCOM->Write(byData,dwCommandDataSize);
		if(m_bPrintCommandDataInLog)
		{
			CString strTemp;
			CString strCmd;
			for(DWORD i = 0;i<dwCommandDataSize;++i)
			{
				strTemp.Format(_T("%02X "),byData[i]);
				strCmd += strTemp;
			}
			theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,�������ݵ��豸:%d -- %s"),dwCommandDataSize,strCmd));
		}
	}
	return dwCommandDataSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::WriteDataToComm
//     
// ��������������˿ڲ������� -- ������д���ݣ��ַ�����ʽ
//     
// ���������IN const CString& strWriteData	--	��Ҫд�����ڵ���������
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager2::WriteDataToComm(IN const CString& strWriteData)
{
	BOOL bReturn = FALSE;
	CString strRealData(strWriteData);
	strRealData.TrimRight();
	strRealData.TrimLeft();
	DWORD dwWriteDataLen = 0;
	BYTE byData[2048]={0};
	DWORD dwCommandDataSize = 0;

	if(0 == strRealData.GetLength()%2)
	{
		//ת����ֽ���
		theApp.Hex2Bin(strRealData.GetBuffer(0),byData,strRealData.GetLength());
		dwCommandDataSize = strRealData.GetLength()/2;

		dwWriteDataLen = WriteDataToComm(byData,dwCommandDataSize);

		bReturn = (dwWriteDataLen == dwCommandDataSize)?TRUE:FALSE;
	}
	else
		bReturn = FALSE;
	//�������·���ָ���״̬Ϊ��
	m_pSMProtocol->m_bNeedReSendLastData = FALSE;
	//������ʾ
	return bReturn;
}
// CThreadSMManager2 message handlers

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnIdle
//     
// ����������ɨ�����̹߳����࣬���д�����
//     
// ���������LONG lCount
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager2::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		//�رմ�������
		CloseCom();
		//�����˳���Ϣ
		::PostQuitMessage(0);

		return FALSE;
	}
	//�ж��Ƿ���Ҫ���´򿪴��ڣ�����2������ 1�����ڳ���ָ��ʱ��û�ɹ����յ��κ����ݣ�
	if(   ((time(NULL) - m_tLastComReadDataTime)  > m_iResetCOMErrorSpanTime)
//		&&((time(NULL) - m_tLastComWriteDataTime) > m_iResetCOMErrorSpanTime)
		)
	{
		//�Ƿ�����ִ�д򿪴��ڲ���
		static time_t tLastOpenCOMTime = 0;
		if( (time(NULL) - tLastOpenCOMTime) > 20)
		{
			//��¼��־
			theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,����[�˿�:%d]״̬�����쳣���޶�ʱ����δ�ɹ�[�յ�/����]�κ����ݣ�����Ҫ��������"),m_sLocalCOM.Port));
			//�رմ�������
			CloseCom();
			//����ϴ�״̬
			m_bCOMLastOpenState = FALSE;
			//��Ĭ1��
			Sleep(1000);
			//�򿪴���
			m_bOpen = OpenCom();
			if(!m_bOpen)
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,����[�˿�:%d]��ʧ��"),m_sLocalCOM.Port));
			}
			//��¼����ʱ��
			tLastOpenCOMTime = time(NULL);
		}
	}

	//����״̬�����仯
	if(m_bCOMLastOpenState != m_bOpen)
	{
		m_bCOMLastOpenState = m_bOpen;
		//��⴮���Ƿ��ܴ�
		m_dwSMState &= 0xFFFFFFFD;
		m_dwSMState |= m_bOpen?0x00:0x02;

		//��¼��־
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,�򿪱��ش���[�˿�:%d,������:%d,����λ:%d,ֹͣλ:%d,У��λ:%d] %s"),m_sLocalCOM.Port,m_sLocalCOM.BaudRate,m_sLocalCOM.DataBit,m_sLocalCOM.StopBit,m_sLocalCOM.ParityBit,m_bOpen?_T("�ɹ�"):_T("ʧ��")));
	}
	
	//������ڴ򿪳ɹ�,���������Ϣ
	if(m_bOpen)
	{
		if(NULL != m_pSMProtocol)
		{
			//ָ��ʱ��û��������Ϣ,�϶�����ͨѶ�쳣��Ĭ��30�룩
			if( (time(NULL) - m_pSMProtocol->m_tLastHeartTimeForBoard) > m_iCheckHeartErrorSpanTime)
			{
				m_bSerialCommError = TRUE;
			}
			else
			{
				m_bSerialCommError = FALSE;
			}
		}
	}
	else
	{
		m_dwSMState = 0x02;
	}

	//����״̬��5����һ��
	static time_t tLastCheckSerialCommTime = 0x00;
	if( (time(NULL) - tLastCheckSerialCommTime) > 5)
	{
		m_dwSMState &= 0xFFFFFFFB;
		m_dwSMState |= m_bSerialCommError?0x04:0x00;
	}

	//�̼߳��״̬,120����һ��
	static time_t tLastSMLogTime = 0x00;
	if( (time(NULL) - tLastSMLogTime) > 120)
	{
	//	theApp.RecordMsgTemp(FmtStr(_T("ɨ�빤���߳�,�̴߳������־��¼����ǰ������� %d"),::theApp.m_nTask));
		tLastSMLogTime = time(NULL);
	}

	Sleep(100);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgStartScan
//     
// ������������ʼɨ��
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_SM_START_SCAN
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgStartScan(WPARAM wParam,LPARAM lParam)
{
	WORD wDelayScanTime = LOWORD(lParam);
	//��¼��־
	theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,���յ�����,��ʼ����ɨ��Ԥ�������")));
	//���Ƶƹ�
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SCAN);
	//��ʱ����
	Sleep(m_dwSpanDelayTimeForControlLight);
	//ִ��ɨ������
	fnSMScanCode();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgStopScan
//     
// ����������ֹͣɨ��
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_SM_STOP_SCAN
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgStopScan(WPARAM wParam,LPARAM lParam)
{
	//��¼��־
	theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,���յ�����,ֹͣɨ��")));
	//���Ƶƹ�
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
	//��ʱ����
	Sleep(m_dwSpanDelayTimeForControlLight);
	//ִ��ֹͣɨ������
	fnSMStopScan();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgDataSendEnd
//     
// ����������֪ͨ�������������Ѿ��������
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgDataSendEnd(WPARAM wParam,LPARAM lParam)
{
	m_tLastComWriteDataTime = time(NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgComError
//     
// ����������֪ͨ�����ߴ��ڷ�������
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��9��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ ON_COM_ERROR
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgComError(WPARAM wParam,LPARAM lParam)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgReceiveDataFromCOM
//     
// �����������ڲ�������Ϣ���������� -- ��ȡ����������
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��29��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgReceiveDataFromCOM(WPARAM wParam,LPARAM lParam)
{
	if(NULL == m_lpReceiveDataBuff)
		return;

	BOOL bAnalyseState = FALSE;
	BYTE byPort = (BYTE)wParam;
	CString strLog;
	CString strReceiveData;

	if(NULL != m_pCOM)
	{
		DWORD dwReadBytes = 0;
		DWORD dwSize = m_sLocalCOM.InBufferSize - m_dwReceiveDataIndex;
		if((dwReadBytes = m_pCOM->Read(m_lpReceiveDataBuff + m_dwReceiveDataIndex,dwSize))>0)
		{
			//��ö�ȡ���ݵ�����ʱ��
			m_tLastComReadDataTime = time(NULL);
			//����ת������
			CString strTemp(theApp.Bin2Hex(m_lpReceiveDataBuff + m_dwReceiveDataIndex,dwReadBytes));
			for(int i = 0;i<strTemp.GetLength()/2;++i)
			{
				strReceiveData += (strTemp.Mid(i,2) + _T(" "));
			}
			strLog.Format(_T("ɨ�빤���߳�,�ײ㴦��:���յ�����%d����(%d�ֽ�),��������:%s"),byPort,dwReadBytes,strReceiveData);

			m_dwReceiveDataIndex += dwReadBytes;
			// ������������������������С,ǿ�ƽ��������������óɻ�������С,
			if( dwReadBytes > dwSize )
			{
				dwReadBytes = dwSize;
				return;
			}
			//Ҫ������������ݽ���
			BYTE* pBegin  = NULL;
			BYTE* pEnd    = NULL;
			BYTE* pValue  = m_lpReceiveDataBuff;
			int valueSize = m_dwReceiveDataIndex;
			if(!m_pSMProtocol->IsAllowAnalyseReceiveDataOnBytesNum(m_dwReceiveDataIndex))
				return;
			m_pSMProtocol->AnalyseReciveData(pValue,valueSize,pBegin,pEnd);

			// ��û���ҵ�֡ͷ,˵����Щ����������,ȫ����,�ҷ���0
			if ( NULL == pBegin )
			{
				memset( m_lpReceiveDataBuff, 0x00, m_sLocalCOM.InBufferSize);
				m_dwReceiveDataIndex = 0;
				strLog.Format(_T("ɨ�빤���߳�,�ײ㴦��:���յ�����%d����(%d�ֽ�),��������:%s,�����޷��ҵ�֡ͷ�����ݱ�������"),byPort,dwReadBytes,strReceiveData);
				theApp.RecordMsgTemp(strLog,TRUE);
				return;
			}
			
			// ���ҵ�֡ͷ,û���ҵ�֡β,��֡ͷ֮ǰ���������
			if ( (NULL != pBegin) && (NULL == pEnd) )
			{
				if( pBegin != pValue )
				{
					// Ҫ������ֽ���
					int iDel = pBegin - m_lpReceiveDataBuff;

					// �õ��µĻ�������Ч�ֽ�
					m_dwReceiveDataIndex -= iDel;

					// ������ʱ����
					BYTE* pTemp = new BYTE[m_dwReceiveDataIndex];

					// �����õ���������������ʱ����
					memcpy( pTemp, pBegin, m_dwReceiveDataIndex );

					// ������������0
					memset( m_lpReceiveDataBuff, 0x00,m_sLocalCOM.InBufferSize);

					// ����ʱ�������������ص�������
					memcpy( m_lpReceiveDataBuff, pTemp, m_dwReceiveDataIndex );

					// ɾ����ʱ����
					delete [] pTemp;
				}

				return;
			}
			ASSERT(pBegin);
			ASSERT(pEnd);

			//���յ�һ��Э��֡(֡ͷ֡β�Ϸ�)
			BYTE* byTemp    = pBegin;
			DWORD dwFramLen = pEnd - pBegin + 1;

			CString strFrameData;
			strReceiveData.Empty();
			for(int i = 0;i<dwFramLen;++i)
			{
				strReceiveData += theApp.FmtStr(_T("%02X "),*pBegin++);
			}
			pBegin = byTemp;

			BOOL bNotifyCallerFail = FALSE;
			BYTE byResponseType = 0x00;
			if(m_pSMProtocol->ProcessReceiveFrameData(pBegin,dwFramLen))
			{
				if(0xC0 == *(byTemp+1))
				{
					strLog.Format(_T("ɨ�빤���߳�,�ײ㴦��:���յ�����[%d]����(��������֡),��������:%02X ֡,%s"),m_sLocalCOM.Port,*(byTemp+1),strReceiveData);
					theApp.RecordMsgTemp(strLog);
					strLog.Empty();
				}
				else
				{
					strLog.Format(_T("ɨ�빤���߳�,�ײ㴦��:���յ�����[%d]����(��������֡),��������:%02X ֡"),m_sLocalCOM.Port,*(byTemp+1));
				}
				//�Ƿ��ӡ��־
				if(m_bPrintCommandDataInLog)
				{
					if(0xC0 != *(byTemp+1))
					{
						CString strCmdLog;
						strCmdLog.Format(_T("ɨ�빤���߳�,�ײ㴦��:���յ�����[%d]����(��������֡),��������:%s"),m_sLocalCOM.Port,strReceiveData);
						theApp.RecordMsgTemp(strCmdLog);
						strLog.Empty();
					}
				}
			}
			else
			{
				byResponseType = 0x02;
				//���ڽ��յ����쳣���ݽ��д���(��֡ͷ)
				if(dwFramLen>6)
				{
					//��������ָ��֡���ݵĻ���
					if(   ( CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5 == *(byTemp+5) )
						&&( ( SM_CMD_START == *(byTemp+6) ) || ( 0x57 == *(byTemp+6) ) )
						)
					{
						static int iReceiveEchoCount = 0x00;
						static CString strLastRecData = _T("");
						if(strReceiveData != strLastRecData)
						{
							iReceiveEchoCount = 0;
							strLastRecData = strReceiveData;
						}
						//�ж��Ƿ���Ҫ֪ͨ������(�������յ�3����ͬ���쳣���ݣ�֪ͨ�����߻�ȡɨ����ʧ�ܣ�
						if(++iReceiveEchoCount>3)
						{
							//֪ͨ�����ߣ�ɨ��ʧ��
							tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
							if(NULL != pSMRemoteState)
							{
								memset(pSMRemoteState,0,sizeof(tagSMInnerDeviceState));

								pSMRemoteState->ScanResult = 0x01;
								PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,0x00),(LPARAM)pSMRemoteState);
							}
							strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(���ʧ��),���ý��[��ȡɨ����Ϣʧ��]"),m_sLocalCOM.Port,strReceiveData);
						}
						else
						{
							//����C0֡
							CString strC0String = m_pSMProtocol->SendCmdResponse(byResponseType,CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5);
							BOOL bSendState = WriteDataToComm(strC0String);
							strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(У��ʧ��),��������:%s�������Ѿ� %d �Σ�����C0֪֡ͨ��λ���ط�������[%s]"),m_sLocalCOM.Port,strReceiveData,iReceiveEchoCount,bSendState?_T("�ɹ�"):_T("ʧ��"));
						}
					}
					else
					{
						strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(У��ʧ��),��������:%s"),m_sLocalCOM.Port,strReceiveData);
					}
				}
				else
				{
					strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(У��ʧ��,���ݳ���С��6�ֽ�),��������:%s"),m_sLocalCOM.Port,strReceiveData);
				}
			}

			//��¼��־
			theApp.RecordMsgTemp(strLog,TRUE);
			strLog.Empty();

			//�������������Ҫ���·������ݣ����ڴ˽��д���(���������յ�����ʧ�ܣ����һ���ʧ�ܵ�����ָ������ΪA5֡��ʱ�򣬲Ž��и����
			if(m_pSMProtocol->m_bNeedReSendLastData)
			{
				if(!m_pSMProtocol->m_strLastSendFrameData.IsEmpty())
				{
					static int iReSendLastCmdCount = 0x00;
					static CString strLastVerifyCmd = _T("");
					if(m_pSMProtocol->m_strLastSendFrameData != strLastVerifyCmd)
					{
						strLastVerifyCmd = m_pSMProtocol->m_strLastSendFrameData;
						iReSendLastCmdCount = 0;
					}

					//ͬһ��ָ��ֻ�����ط�3��
					if(++iReSendLastCmdCount<4)
					{
						//������д����
						if(WriteDataToComm(m_pSMProtocol->m_strLastSendFrameData))
						{
							strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ���λ������C0֡����ʶ������λ��ָ��ʧ�ܣ����½��з����ϴε����ݡ��ɹ��������� %d "),iReSendLastCmdCount);
						}
						else
						{
							strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ���λ������C0֡����ʶ������λ��ָ��ʧ�ܣ����½��з����ϴε����ݡ�ʧ�ܡ�,���� %d "),iReSendLastCmdCount);
						}
					}
					else
					{
						iReSendLastCmdCount = 0;
						//�ط������Ѿ�����3�Σ��ж�ɨ��ʧ�ܣ�֪ͨ������
						tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
						if(NULL != pSMRemoteState)
						{
							memset(pSMRemoteState,0,sizeof(tagSMInnerDeviceState));

							pSMRemoteState->ScanResult = 0x01;
							PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,0x00),(LPARAM)pSMRemoteState);
							strLog.Format(_T("ɨ������߳�,�ײ㴦��:����Ӳ��������ȡָ��ʧ�ܣ�����ط�ͬһ֡�����Ѿ�����3�Σ��ж�ɨ�����ʧ�� "));
						}
					}
					//��¼��־
					theApp.RecordMsgTemp(strLog);
					strLog.Empty();
				}
			}

			//������������0
			memset( m_lpReceiveDataBuff, 0x00,m_sLocalCOM.InBufferSize);
			m_dwReceiveDataIndex = 0;
		}
	}
	else
	{
		strLog.Format(_T("ɨ�빤���߳�,�ײ㴦��:���յ�����%d����!"),byPort);
	}
	//��¼��־
	if(!strLog.IsEmpty())
	{
		theApp.RecordMsgTemp(strLog);
		strLog.Empty();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgDealResponseForProtocolProcessResult
//     
// �����������ڲ�������Ϣ���������� -- ��������Э�����ʽ���������
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgDealResponseForProtocolProcessResult(WPARAM wParam,LPARAM lParam)
{
	//��ȡִ�е���������
	WORD wCmdType  = LOWORD(wParam);
	//��ȡ�豸ID����
	WORD wDeviceID = HIWORD(wParam);
	//��־��Ϣ
	CString strLog;

	tagSMInnerDeviceState* pSMRemoteState = (tagSMInnerDeviceState*)lParam;

	//���д���
	try
	{	
		if(NULL == pSMRemoteState)
			throw -1;
		//�����������
		switch(wCmdType)
		{
		case SM_CMD_INIT:
		case SM_CMD_VER:
		case SM_CMD_STOP:
		case SM_CMD_RESET_FACTORY:
		case SM_CMD_RESET_PERSION:
			{
				strLog = theApp.FmtStr(_T("ɨ�빤���߳�,%s"),pSMRemoteState->StateDesc);
			}
			break;
		case SM_CMD_START:
			{
				if(0 == pSMRemoteState->ScanResult)
				{
					strLog = theApp.FmtStr(_T("ɨ�빤���߳�,��������3:ɨ����[%d]��ȡ����:%s"),pSMRemoteState->DeviceID,pSMRemoteState->Code);
				}
				else
				{
					strLog = theApp.FmtStr(_T("ɨ�빤���߳�,��������3:δ��ȡ�κ���Ч����"));
				}
				//��¼��־
				theApp.RecordMsgTemp(strLog);

				pSMRemoteState->ScanConsumeTime = GetTickCount() - m_dwScanStartTime;
				_tcscpy_s(pSMRemoteState->StateDesc,_countof(pSMRemoteState->StateDesc),strLog.GetBuffer(0));strLog.ReleaseBuffer();
				//ɨ������ʽ������
				fnProcessInnerDeviceResponseForStartScan(pSMRemoteState);
			}
			break;
		}
		//ɾ������
		delete pSMRemoteState;
		pSMRemoteState = NULL;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("ɨ�빤���߳�,�ô�ɨ�����,Э��������������ָ���쳣"));
			break;
		}
		//��¼��־
		theApp.RecordMsgTemp(strLog);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgSetLightMode
//     
// �������������յ�������Ϣ������ -- ���ƴ��ڰ����ģʽ
//     
// ���������WPARAM wParam  -- �ӳ�ʱ�䣨���룩
//         ��LPARAM lParam	-- ����ģʽ(0,ɨ��;1,�ɹ�;2,ʧ��;3,ȫ��)
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgSetLightMode(WPARAM wParam,LPARAM lParam)
{
	int iLightMode = (int)lParam;
	int iDelayTime = (int)wParam;
	Sleep(iDelayTime);
	if(   (iLightMode >= CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SUCCESS)
		&&(iLightMode <= CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF)
		)
	{
		fnSMControlLight(iLightMode);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgControlSerialBoard
//     
// �������������ܵ�������Ϣ������ -- ���ƴ��ڰ����
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��8��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_SM_CONTROL_BOARD
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgControlSerialBoard(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	tagSMMainBoardParam* pCMDParam = (tagSMMainBoardParam*)lParam;
	try
	{
		if(NULL == pCMDParam)
			throw -1;
		if(NULL == m_pSMProtocol)
			throw -2;

		CString strSendData;
		switch(pCMDParam->CMDFlag)
		{
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A1:
			strSendData = m_pSMProtocol->UpdateConfigInfoFromBoard(_T(""));
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A2:
			strSendData = m_pSMProtocol->ReadConfigInfoFromBoard();
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A3:
			{
				int iLightMode = pCMDParam->ParamData[0];
				strSendData = m_pSMProtocol->SetLightMode(iLightMode);

				CString strLightMode;
				switch(iLightMode)
				{
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF:
					strLightMode.Format(_T("ȫϨ��"));
					break;
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SCAN:
					strLightMode.Format(_T("��ʾɨ��"));
					break;
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SUCCESS:
					strLightMode.Format(_T("ɨ��ͨ�гɹ�"));
					break;
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_FAIL:
					strLightMode.Format(_T("ɨ��ͨ��ʧ��"));
					break;
				}
				strLog.Format(_T("ɨ�빤���߳�,����ɨ����ʾ�ƣ�����Ϊ:%s"),strLightMode);
				
			}
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A4:
			strSendData = m_pSMProtocol->GetLightMode();
			break;

			//������A5��ʾָ��
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5:
			strSendData = m_pSMProtocol->SetScanDisplay(sScanDisplay);
			strLog.Format(_T("ɨ�빤���߳�,��������:(Type:%d, Money:%d, Weight:%d, VType:%d, VClass:%d, Time:%s, EntryST:%s)"),
				sScanDisplay.Type, sScanDisplay.Money, sScanDisplay.Weight, sScanDisplay.VehicleType,
				sScanDisplay.VehicleClass,	CTime(sScanDisplay.tShowtime).Format("%Y-%m-%d %H:%M:%S"),sScanDisplay.EntrySTName );
			break;

		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A6:
			strSendData = m_pSMProtocol->ResetBoard();
			break;
		}

		if(!WriteDataToComm(strSendData))
			throw -3;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
		strLog.Format(_T("ɨ�빤���߳�,����IOʧ��"));
	}
	//��¼��־
	if(!strLog.IsEmpty())
	{
		theApp.RecordMsgTemp(strLog);
	}

	if(NULL != pCMDParam)
	{
		delete pCMDParam;
		pCMDParam = NULL;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::fnProcessInnerDeviceResponseForStartScan
//     
// ���������������ڲ��ҽ��豸��ִ��ɨ��ָ���Ļ�����Ϣ
//     
// ���������IN tagSMInnerDeviceState* pSMState	--  ɨ����״̬
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState)
{
	BOOL bGetData = FALSE;
	CString strError;
	BOOL bNeedNotifyCaller = TRUE;
	try
	{
		if(NULL == pSMState)
			throw -1;

		if(OP_SUCCESS != pSMState->ScanResult)
			throw -2;

		//��ʽ������
		static CString strLastCode = _T("");
		CString strCurrentCode(pSMState->Code);
		if(strCurrentCode != strLastCode)
		{
			strLastCode = strCurrentCode;
		}
		else
		{
			if(m_bAbandonEchoData)
			{
				bNeedNotifyCaller = FALSE;
				throw -3;
			}
		}

		//��ʽ������
		::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
		::memcpy(&m_sSMResult, pSMState, sizeof(tagSMInnerDeviceState));
		m_sSMResult.ScanResult = 0;

		bGetData = TRUE;
		//��¼��־
		strError.Format(_T("ɨ�빤���߳�,�ô�ɨ�����,�Ѿ���ȡ���ݣ�����֪ͨ��Ϣ��������"));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://���յ�������Ϊ��
		case -2://���յ������ݷǷ�
			strError.Format(_T("ɨ�빤���߳�,�ô�ɨ����ϣ�δ�ܻ�ȡ����,����֪ͨ��Ϣ��������"));
			break;
		case -3://��ǰ���յ��ظ�����
			strError.Format(_T("ɨ�빤���߳�,�ô�ɨ����ϣ����յ��ظ������ݣ��ڲ�����������������Ϣ��������"));
			break;
		}
	}
	//֪ͨ������
	if(bNeedNotifyCaller)
	{
		if(::IsWindow(m_hMainWnd))
		{
			::PostMessage(m_hMainWnd, m_dwMessageID, (WPARAM)bGetData,(LPARAM)0);
		}
		else if(0 != m_nMainThreadID)
		{
			::PostThreadMessage(m_nMainThreadID, m_dwMessageID, (WPARAM)bGetData,(LPARAM)0);
		}
	}
	//��¼��־
	theApp.RecordMsgTemp(strError);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::fnSMScanCode
//     
// �����������ڲ����̲������� -- ִ��ɨ�����̲���
//     
// ���������void
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnSMScanCode(void)
{
	//��ʼ��ʱ
	GetLocalTime(&m_stBeginTime);
	m_dwScanStartTime = GetTickCount();

	//��������
	CString strSendData;
	CString strInnerDeviceCommand;
	DWORD	dwCommandDataSize = 0;
	//��ǹ���״̬
	m_bWorking = TRUE;
	m_tNoDataFromCOMLastTime = time(NULL);

	CString strError;
	//��ʼִ��ɨ�����
	try
	{
		if(!m_bOpen || (NULL == m_pSMProtocol))
			throw -1;
		//��¼��־
		//theApp.RecordMsgTemp(FmtStr(_T("ɨ�빤���߳�,��ʼɨ������")));

		//strError.Format(_T("��������%d:����׼��ɨ��,��ʱ[%s]��ʼ"),0x01,GetCurrentTimeString(1));
		//theApp.RecordMsgTemp(FmtStr(_T("ɨ�빤���߳�,%s"),strError));

		//��ȡ�ڹ��豸��ɨ����������
		strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"),SM_CMD_START,m_pSMProtocol->GetInnerDeviceProtocolClass()->Start());
		//��ȡ��ʽ����������
		strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
		if(strSendData.IsEmpty())
			throw -2;

		////��¼��־
		//strError.Format(_T("��������%d:��ȡ����񻯵Ķ�������������[%s]"),0x02,strSendData);
		//theApp.RecordMsgTemp(FmtStr(_T("ɨ�빤���߳�,%s"),strError));
	
		//������д����
		if(!WriteDataToComm(strSendData))
			throw -3;

		//��¼��־
		strError.Format(_T("��������%d:ɨ�������ѷ��ͣ�����ɨ�����ݣ����Ժ�"),0x01);
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,%s"),strError));	

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://����δ�򿪻�����಻����
			strError.Format(_T("ɨ��ʧ��,�ڲ��쳣:����δ�򿪻�����಻����"));
			break;
		case -2://�޷���ȡ��Ч��ɨ������
			strError.Format(_T("ɨ��ʧ��,�ڲ��쳣:�޷���ȡ��Ч��ɨ������"));
			break;
		case -3://������дɨ����������ʧ��
			strError.Format(_T("ɨ��ʧ��,�ڲ��쳣:������дɨ����������ʧ��"));
			break;
		}
		
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,%s"),strError));	
	}
	//��ǹ���״̬
	m_bWorking = FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::fnSMStopScan
//     
// �����������ڲ����̲������� -- ִ��ֹͣ���̲���
//     
// ���������void
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnSMStopScan(void)
{
	CString strSendData;
	CString strInnerDeviceCommand;
	DWORD	dwCommandDataSize = 0;
	BYTE	byCmdData[1024]={0};
	try
	{
		if(!m_bOpen || (NULL == m_pSMProtocol))
			throw -1;
		CHTSMDeviceBaseIF* pSMIF = m_pSMProtocol->GetInnerDeviceProtocolClass();
		if(NULL == pSMIF)
			throw -2;
		//��ȡֹͣɨ������
		strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"),SM_CMD_STOP,pSMIF->Stop());
		if(strInnerDeviceCommand.IsEmpty())
			throw -2;
		//��ȡ��ʽ����������
		strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
		if(strSendData.IsEmpty())
			throw -3;
		//������д����
		if(!WriteDataToComm(strSendData))
			throw -4;
		//д��־
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,����ǿ��ֹͣɨ��ָ��[�ɹ�]")));
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
		}
		//д��־
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ�빤���߳�,����ǿ��ֹͣɨ��ָ��[ʧ��]")));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::fnSMControlLight
//     
// �����������ڲ����̿��ƺ��� -- ���Ƶƹ�
//     
// ���������IN const int& iLightType
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnSMControlLight(IN const int& iLightType)
{
	tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
	if(NULL != pParam)
	{
		memset(pParam,0,sizeof(tagSMMainBoardParam));

		pParam->Type		= 1;
		pParam->CMDFlag		= CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A3;
		pParam->ParamData[0]= iLightType;
		pParam->ParamSize	= 0x01;

		OnMsgControlSerialBoard((WPARAM)0,(LPARAM)pParam);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager2::OnMsgSetDisplay
//     
// �������������յ�������Ϣ������ -- ������ʾ����
//     
// ���������WPARAM wParam  -- �ӳ�ʱ�䣨���룩
//         ��LPARAM lParam	-- ����ģʽ(0,ɨ��;1,�ɹ�;2,ʧ��;3,ȫ��)
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgSetDisplay(WPARAM wParam,LPARAM lParam)
{
	int iSerial = (int)wParam;//��ȡ��Ϣ���к�
	if( iSerial == sScanDisplay.iSerial )//�������кŵ��ڵ�ǰ���кţ�ִ����ʾ����
	{
		tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
		if(NULL != pParam)
		{
			memset(pParam,0,sizeof(tagSMMainBoardParam));

			pParam->Type		= 1;
			pParam->CMDFlag		= CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5;
			pParam->ParamData[0]= 0;
			pParam->ParamSize	= 0x01;

			OnMsgControlSerialBoard((WPARAM)0,(LPARAM)pParam);
		}
	}
}
