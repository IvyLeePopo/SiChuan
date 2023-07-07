#include "StdAfx.h"
#include "ThreadSMManager.h"
#include "ScanMachine.h"
extern CScanMachineApp theApp;

IMPLEMENT_DYNCREATE(CThreadSMManager, CThreadBase)


CThreadSMManager::CThreadSMManager(void)
	:m_bOpen(FALSE)
	,m_bCOMLastOpenState(FALSE)
	,m_hCom(INVALID_HANDLE_VALUE)
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
	::memset(&sSMResult, 0, sizeof(tagSMInnerDeviceState));
	sSMResult.ScanResult = -9;

	::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
}


CThreadSMManager::~CThreadSMManager(void)
{
}

BOOL CThreadSMManager::InitInstance()
{
	m_pSMProtocol = new CHTSMDeviceControl();

	ASSERT(m_pSMProtocol);

	m_pSMProtocol->SetParam(m_nThreadID);

	fnReadSpecailConfig();

	CString strSyncComDataHandleName;
	strSyncComDataHandleName.Format(_T("TOLL_LANE_COM_SYNC_DLL_%d"),m_nThreadID);
	m_hSyncComData = ::CreateEvent(NULL,TRUE,FALSE,strSyncComDataHandleName);

	CString strLog;
	m_pSMProtocol->InitInnerDevice(strLog);

	return TRUE;
}

int CThreadSMManager::ExitInstance()
{
	fnReleaseResource();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadSMManager, CThreadBase)

	ON_THREAD_MESSAGE(WM_THREAD_SM_START_SCAN,OnMsgStartScan)
	ON_THREAD_MESSAGE(WM_THREAD_SM_STOP_SCAN,OnMsgStopScan)
	ON_THREAD_MESSAGE(WM_THREAD_SM_CONTROL_BOARD,OnMsgControlSerialBoard)
	ON_THREAD_MESSAGE(WM_THREAD_SM_SET_LIGHT_MODE,OnMsgSetLightMode)
	ON_THREAD_MESSAGE(WM_THREAD_SM_SET_DISPLAY,OnMsgSetDisplay)
	ON_THREAD_MESSAGE(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,OnMsgDealResponseForProtocolProcessResult)

END_MESSAGE_MAP()


DWORD CThreadSMManager::GetSMState()
{
	return m_dwSMState;
}

void CThreadSMManager::SetMessageID(DWORD dwMessage)
{
	m_dwMessageID = dwMessage;
}

void CThreadSMManager::GetSMResult(tagSMInnerDeviceState& sState)
{
	::memset(&sState, 0, sizeof(tagSMInnerDeviceState));
	::memcpy(&sState, &sSMResult, sizeof(tagSMInnerDeviceState));
}

void CThreadSMManager::SetDisplay(tagScanDisplay sDisplay)
{
	::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
	::memcpy(&sScanDisplay, &sDisplay, sizeof(tagScanDisplay));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::fnReleaseResource
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
void CThreadSMManager::fnReleaseResource()
{
	//�ص�
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
	::Sleep(10);
	//�رմ���
	CloseCom();

	if(NULL != m_pSMProtocol)
	{
		delete m_pSMProtocol;
		m_pSMProtocol = NULL;
	}

	if(INVALID_HANDLE_VALUE != m_hSyncComData)
		::CloseHandle(m_hSyncComData);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::fnReadSpecailConfig
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
void CThreadSMManager::fnReadSpecailConfig(void)
{
	int iSMCount = ::GetPrivateProfileInt(_T("SMControl"),_T("Count"),1,m_strPathLocalConfigFile);
	//��ȡ��������쳣�ļ��ʱ�䣨��λ���룩
	m_iCheckHeartErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"),_T("CheckHeartErrorSpanTime"),30,m_strPathLocalConfigFile);
	if(  (m_iCheckHeartErrorSpanTime<=0) || (m_iCheckHeartErrorSpanTime>=240) )
	{
		m_iCheckHeartErrorSpanTime = 60;
	}

	//�Ƿ��շ����ݶ���ʾ����־�ļ�����
	m_bPrintCommandDataInLog = (BOOL)::GetPrivateProfileInt(_T("SMControl"),_T("PrintCommandDataInLog "),0,m_strPathLocalConfigFile);
	//�Ƿ����ظ�����(�����һ�����ݶ���)
	m_bAbandonEchoData       = (BOOL)::GetPrivateProfileInt(_T("SMControl"),_T("AbandonEchoData "),1,m_strPathLocalConfigFile);
	//��ȡ�����쳣����Ҫ��λ�ļ��ʱ��
	m_iResetCOMErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"),_T("ResetCOMErrorSpanTime"),20,m_strPathLocalConfigFile);
	//��ȡ�����ַ�֮���ʱ�������������ʱ��ֵ����Ϊ���ݴ�����ϣ�
	m_iNoNewDataTime		 = ::GetPrivateProfileInt(_T("SMControl"),_T("NoNewDataTime"),50,m_strPathLocalConfigFile);
	
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
// �������ƣ�CThreadSMManager::OpenCom
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
BOOL CThreadSMManager::OpenCom()
{	
	DCB          dpb;
	COMMTIMEOUTS comtimeout;
	CString      strTemp;
	CString		 strDesc;

	memset(&comtimeout,0,sizeof(comtimeout));
	comtimeout.ReadIntervalTimeout=MAXDWORD;
	comtimeout.ReadTotalTimeoutMultiplier=0;
	comtimeout.ReadTotalTimeoutConstant=0;
	comtimeout.WriteTotalTimeoutMultiplier=50;
	comtimeout.WriteTotalTimeoutConstant=2000;

	strTemp.Format("\\\\.\\COM%d",m_sLocalCOM.Port );
	if(INVALID_HANDLE_VALUE!=(m_hCom=CreateFile(strTemp,
												GENERIC_READ|GENERIC_WRITE,
												0,
												NULL,
												OPEN_EXISTING,
												FILE_ATTRIBUTE_NORMAL,
												NULL)))
	{
		GetCommState(m_hCom, &dpb);
		dpb.BaudRate	= m_sLocalCOM.BaudRate;
		dpb.ByteSize	= 8;
		dpb.Parity		= NOPARITY;
		dpb.StopBits	= ONESTOPBIT;
		SetCommState(m_hCom,&dpb);
		SetCommTimeouts(m_hCom,&comtimeout);
		m_bOpen = TRUE;
	}
	else 
	{
		m_bOpen = FALSE;
		m_hCom  = NULL;
	}

	m_sCOMState.IsInit = m_bOpen;
	//������־
	strDesc.Format(_T("���ش���[�˿ں�:%d,������:%d,ֹͣλ:%d,У��λ:%d,����λ:%d]��%s"),m_sLocalCOM.Port,m_sLocalCOM.BaudRate,m_sLocalCOM.StopBit,m_sLocalCOM.ParityBit,m_sLocalCOM.DataBit,m_bOpen?_T("�ɹ�"):_T("ʧ��"));
	_tcscpy_s(m_sCOMState.StateDesc,_countof(m_sCOMState.StateDesc),strDesc.GetBuffer(0));
	strDesc.ReleaseBuffer();
	//���ش��ڴ򿪱��
	return m_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::CloseCom
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
BOOL CThreadSMManager::CloseCom()
{
	BOOL bReturn = FALSE;
	if( INVALID_HANDLE_VALUE != m_hCom ) 
	{
		bReturn = ::CloseHandle(m_hCom);
		m_bOpen = !bReturn;
		//������عرվ���ɹ�����Դ��ھ������Ϊ�Ƿ�ֵ
		if(bReturn)
		{
			m_hCom = INVALID_HANDLE_VALUE;
		}

		CString strDesc;
		strDesc.Format(_T("ɨ������߳�,����[�˿�:%d]�ر�%s"),m_sLocalCOM.Port,bReturn?_T("�ɹ�"):_T("ʧ��"));
		theApp.RecordMsgTemp(strDesc);
	}
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::WriteDataToComm
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
DWORD CThreadSMManager::WriteDataToComm(IN const LPBYTE byData,IN const DWORD& dwCommandDataSize)
{
	DWORD dwWriteDataLen = 0;

	CString strCmd;
	CString strComErrDesc;

	if(m_bPrintCommandDataInLog)
	{
		CString strTemp;
		for(DWORD i = 0;i<dwCommandDataSize;++i)
		{
			strTemp.Format(_T("%02X "),byData[i]);
			strCmd += strTemp;
		}
	}

	if( INVALID_HANDLE_VALUE != m_hCom )
	{
		if(!(::WriteFile(m_hCom,byData,dwCommandDataSize,&dwWriteDataLen,NULL)))
		{
			//��������
			if( (1 == dwCommandDataSize) && (strCmd == _T("FF")) )
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,��⴮���·�����״̬[����]")));
			}
			else
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,�������ݵ��豸ʧ�ܣ��������:%d"),GetLastError()));
			}
		}
		else
		{
			//��¼���ݳɹ�����ʱ��
			m_tLastComWriteDataTime = time(NULL);
			//��������
			if( (1 == dwCommandDataSize) && (strCmd == _T("FF")) )
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,��⴮���·�����״̬[����]")));
			}
			else
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,�������ݵ��豸:%d -- %s"),dwCommandDataSize,strCmd));
			}
		}
	}
	else 
	{
		//���ھ���쳣
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,���յ��������ݵ������ڲ�����:���ھ���쳣(%d -- %s)"),dwCommandDataSize,strCmd));
	}
	//����ʵ�ʷ���ȥ���ֽ���
	return dwWriteDataLen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::WriteDataToComm
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
BOOL CThreadSMManager::WriteDataToComm(IN const CString& strWriteData)
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

		int iLastCmdTyp = m_pSMProtocol->GetInnerDeviceProtocolClass()->GetLastCommandType();
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


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::OnIdle
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
BOOL CThreadSMManager::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		//�رմ�������
		CloseCom();
		//�����˳���Ϣ
		::PostQuitMessage(0);

		return FALSE;
	}

	//�ж��Ƿ���Ҫ���´򿪴��ڣ�����2������ 1�����ڳ���ָ��ʱ��û�ɹ����յ��κ�����;2,���ڳ���ָ��ʱ��û���ͳɹ��κ�����)
	if(   ((time(NULL) - m_tLastComReadDataTime)  > m_iResetCOMErrorSpanTime)
		&&((time(NULL) - m_tLastComWriteDataTime) > m_iResetCOMErrorSpanTime)
		)
	{
		static time_t tLastSendTestCharTime = 0;
		if( (time(NULL) - tLastSendTestCharTime) > 10)
		{
			//���Է���һ���ַ�������Ƿ�ɹ�
			if(!WriteDataToComm(_T("FF")))
			{
				//�Ƿ�����ִ�д򿪴��ڲ���
				static time_t tLastOpenCOMTime = 0;
				if( (time(NULL) - tLastOpenCOMTime) > 20)
				{
					//��¼��־
					theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,����[�˿�:%d]״̬�����쳣���޶�ʱ����δ�ɹ�[�յ�/����]�κ����ݣ�����Ҫ��������"),m_sLocalCOM.Port));
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
						theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,����[�˿�:%d]��ʧ��"),m_sLocalCOM.Port));
					}
					//��¼����ʱ��
					tLastOpenCOMTime = time(NULL);
				}
			}
			//��¼���Ͳ����ַ�ʱ��
			tLastSendTestCharTime = time(NULL);
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
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,�򿪱��ش���[�˿�:%d,������:%d,����λ:%d,ֹͣλ:%d,У��λ:%d] %s"),m_sLocalCOM.Port,m_sLocalCOM.BaudRate,m_sLocalCOM.DataBit,m_sLocalCOM.StopBit,m_sLocalCOM.ParityBit,m_bOpen?_T("�ɹ�"):_T("ʧ��")));
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

	CString strTemp;
	CString strLog;
	//��ʼ��ȡ����
	if(NULL != m_hCom)
	{
		DWORD	dwError=0;
		COMSTAT	comStat;
		//���˿�״̬
		ClearCommError(m_hCom,&dwError,&comStat);
		if(comStat.cbInQue>0)//���ջ�������������
		{
			/******************************��д���ڽ���ģ��,����ReadFile����****************************************/
			DWORD	dwStart			= 0;				//����֡�ڻ���������ʼλ��
			DWORD	dwEnd			= 0;				//����֡�ڻ������Ľ�βλ��+1
			DWORD	dwRWLen			= 0;				//��������֡����
			DWORD	dwOrgDataLen	= 0;				//�յ�һ������������֡(δ��CRCУ�飬���ǳ��ȷ���)
			DWORD	dwReadNum		= 0;				//��ǰ��ȡ���ֽ���
			DWORD	dwNewTime		= 0;				//��ǰ��ȡ����ʱ��
			BYTE	byRBuff[2048]   = {0};					//�������ݻ�����(����2048�ֽ�)
			BOOL	bHaveFram = FALSE;

			CString strReceiveData;
			while(!bHaveFram)
			{
				//��ȡ1���ֽ�
				ReadFile(m_hCom,&byRBuff[dwEnd],1,&dwReadNum,NULL); 
				if(dwReadNum>0)
				{
					//���µ�ǰ��ȡ����ʱ��
					dwNewTime = ::timeGetTime();
					m_tLastComReadDataTime = time(NULL);
					//��û�������ĩβ����
					dwEnd += dwReadNum;
					//�����Ƿ��кϷ���
					BOOL bFramStart = FALSE;
					for(register DWORD dwRecCount= 0;dwRecCount<dwEnd;++dwRecCount)
					{
						if(!bFramStart && (0xDF == byRBuff[dwRecCount]))//�ػ��ͷ�������ڻ�������λ��
						{
							bFramStart = TRUE;
							dwStart = dwRecCount++;
							continue;
						}
						if(bFramStart && 0xFD == byRBuff[dwRecCount])//�ػ��β���õ�������
						{
							dwOrgDataLen = dwRecCount - dwStart + 1;
							//��ȡ�������֡,����ѭ��
							bHaveFram = TRUE;
							//�����ڲ�ѭ��
							break;
						}
					}
				}
				else 
				{ 
					if((::timeGetTime() - dwNewTime) > m_iNoNewDataTime)//����ָ������(Ĭ�� 50)û�ж������ݣ���������
					{
						//�������յ������ݰ�
						if(dwEnd>=6)
						{
							//��¼�½��յ�������
							for(int i = 0;i<dwEnd;++i)
							{
								strTemp.Format(_T("%02X "),byRBuff[dwStart+i]);
								strReceiveData += strTemp;
							}
							strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(������),��������:%s"),m_sLocalCOM.Port,strReceiveData);
							theApp.RecordMsgTemp(strLog);
							
							//���������յ�����������ӦA5ָ��������Ž���֪ͨ�����ߴ���
							if(    (CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5 == byRBuff[dwStart+5])
								&& ((SM_CMD_START == byRBuff[dwStart+6] ) || (0x57 == byRBuff[dwStart+6]))
								)
							{
								//�յ�����6���ֽڵ���Ч���ݣ��ж�ɨ��ʧ�ܣ�֪ͨ������
								tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
								if(NULL != pSMRemoteState)
								{
									memset(pSMRemoteState,0,sizeof(tagSMInnerDeviceState));

									pSMRemoteState->ScanResult = 0x01;
									PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,0x00),(LPARAM)pSMRemoteState);
									strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�������Ч���ݣ�����ָ���ֽ��������ж�ɨ�����ʧ�� "));
								}
								theApp.RecordMsgTemp(strLog);
							}
						}
						//ֱ�ӷ���
						return TRUE;
					}
				}
				//��Ĭ1����
				Sleep(1);
			}

			//��ȡ����������֡
			for(int i = 0;i<dwOrgDataLen;++i)
			{
				strTemp.Format(_T("%02X "),byRBuff[dwStart+i]);
				strReceiveData += strTemp;
			}

			//����֡�����Լ�����
			BOOL bNotifyCallerFail = FALSE;
			BYTE byResponseType = 0x00;
			int nFrameType = 0;
			if(m_pSMProtocol->ProcessReceiveFrameData(&byRBuff[dwStart],dwOrgDataLen, nFrameType))
			{
				if(0xC0 == byRBuff[dwStart + 1])
				{
					strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(��������֡),��������:%02X ֡,%s"),m_sLocalCOM.Port,byRBuff[dwStart + 1],strReceiveData);
					theApp.RecordMsgTemp(strLog);
					strLog.Empty();
				}
				else
				{
					strLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(��������֡),��������:%02X ֡"),m_sLocalCOM.Port,byRBuff[dwStart + 1]);
				}
				//�Ƿ��ӡ��־
				if(m_bPrintCommandDataInLog)
				{
					if(0xC0 != byRBuff[dwStart + 1])
					{
						CString strCmdLog;
						strCmdLog.Format(_T("ɨ������߳�,�ײ㴦��:���յ�����[%d]����(��������֡),��������:%s"),m_sLocalCOM.Port,strReceiveData);
						theApp.RecordMsgTemp(strCmdLog);
						strLog.Empty();
					}
				}
			}
			else
			{
				byResponseType = 0x02;
				//���ڽ��յ����쳣���ݽ��д���
				if(dwOrgDataLen>6)
				{
					//��������ָ��֡���ݵĻ���
					if(   (CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5== byRBuff[dwStart + 5])
						||((SM_CMD_START == byRBuff[dwStart + 6]) || (0x57 == byRBuff[dwStart + 6]))
						)
					{
						static int iReceiveEchoCount = 0x00;
						static CString strLastRecData = _T("");
						if(strReceiveData != strLastRecData)
						{
							iReceiveEchoCount = 0;
							strLastRecData = strReceiveData;
						}
						//�ж��Ƿ���Ҫ֪ͨ������(�������յ�3����ͬ�����ݣ�֪ͨ�����߻�ȡɨ����ʧ�ܣ�
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
			}

			//��¼��־
			if(!strLog.IsEmpty())
			{
				if(m_strLastErrorDesc != strLog)
				{
					m_strLastErrorDesc = strLog;
					theApp.RecordMsgTemp(strLog);
					strLog.Empty();
				}
			}

			//�ж��Ƿ���Ҫ���·������ݣ����ڴ˽��д���(���������յ�����ʧ�ܣ����һ���ʧ�ܵ�����ָ������ΪA5֡��ʱ�򣬲Ž��и����
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
					if(!strLog.IsEmpty())
					{
						if(m_strLastErrorDesc != strLog)
						{
							m_strLastErrorDesc = strLog;
							theApp.RecordMsgTemp(strLog);
						}
					}
				}
			}
		}
	}
	else
	{
		//�������´򿪴���
		m_bOpen = OpenCom();
	}

	//�̼߳��״̬,120����һ��
	static time_t tLastSMLogTime = 0x00;
	if( (time(NULL) - tLastSMLogTime) > 120)
	{
		tLastSMLogTime = time(NULL);
	}

	Sleep(5);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::OnMsgStartScan
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
void CThreadSMManager::OnMsgStartScan(WPARAM wParam,LPARAM lParam)
{
	WORD wDelayScanTime = LOWORD(lParam);
	//��¼��־
	theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,���յ�����,��ʼ����ɨ��Ԥ�������")));
	//���Ƶƹ�
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SCAN);
	//��ʱ����
	Sleep(m_dwSpanDelayTimeForControlLight);
	//ִ��ɨ������
	fnSMScanCode();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::OnMsgStopScan
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
void CThreadSMManager::OnMsgStopScan(WPARAM wParam,LPARAM lParam)
{
	//��¼��־
	theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,���յ�����,֪ͨ�����߳�ֹͣɨ��")));
	//���Ƶƹ�
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
	//��ʱ����
	Sleep(m_dwSpanDelayTimeForControlLight);
	//ִ��ֹͣɨ������
	fnSMStopScan();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::OnMsgDealResponseForProtocolProcessResult
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
void CThreadSMManager::OnMsgDealResponseForProtocolProcessResult(WPARAM wParam,LPARAM lParam)
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
				strLog = theApp.FmtStr(_T("ɨ������߳�,%s"),pSMRemoteState->StateDesc);
			}
			break;
		case SM_CMD_START:
			{
				if(0 == pSMRemoteState->ScanResult)
				{
					strLog = theApp.FmtStr(_T("ɨ�빤���߳�,��������3:�ô�ɨ�����,ɨ����[%d]��ȡ����:%s"),pSMRemoteState->DeviceID,pSMRemoteState->Code);
				}
				else
				{
					strLog = theApp.FmtStr(_T("ɨ�빤���߳�,��������3:�ô�ɨ�����,δ��ȡ�κ���Ч����"));
				}
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
			strLog.Format(_T("ɨ������߳�,�ô�ɨ�����,Э��������������ָ���쳣"));
			break;
		}
	}
	//��¼��־
	theApp.RecordMsgTemp(strLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::OnMsgSetLightMode
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
void CThreadSMManager::OnMsgSetLightMode(WPARAM wParam,LPARAM lParam)
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
// �������ƣ�CThreadSMManager::OnMsgControlSerialBoard
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
void CThreadSMManager::OnMsgControlSerialBoard(WPARAM wParam,LPARAM lParam)
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
				strLog.Format(_T("ɨ������߳�,����ɨ����ʾ�ƣ�����Ϊ:%s"),strLightMode);
				
			}
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A4:
			strSendData = m_pSMProtocol->GetLightMode();
			break;

			//������A5��ʾָ��
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5:
			strSendData = m_pSMProtocol->SetScanDisplay(sScanDisplay);
			if(strSendData.IsEmpty())
			{
				strLog.Format(_T("ɨ������߳�,��������:Type %d ��������ʱ��������"),sScanDisplay.Type);
			}
			else
			{
				strLog.Format(_T("ɨ������߳�,��������:(Type:%d, Money:%d, Weight:%d, VType:%d, VClass:%d, Time:%s, EntryST:%s)"),
					sScanDisplay.Type, sScanDisplay.Money, sScanDisplay.Weight, sScanDisplay.VehicleType,
					sScanDisplay.VehicleClass,	CTime(sScanDisplay.tShowtime).Format("%Y-%m-%d %H:%M:%S"),sScanDisplay.EntrySTName );
			}
			break;

		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A6:
			strSendData = m_pSMProtocol->ResetBoard();
			break;
		}

		if(!strSendData.IsEmpty())
		{
			if(!WriteDataToComm(strSendData))
				throw -3;
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
		strLog.Format(_T("ɨ������߳�,����IOʧ��"));
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
// �������ƣ�CThreadSMManager::fnProcessInnerDeviceResponseForStartScan
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
void CThreadSMManager::fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState)
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

		::memset(&sSMResult, 0, sizeof(tagSMInnerDeviceState));
		::memcpy(&sSMResult, pSMState, sizeof(tagSMInnerDeviceState));
		sSMResult.ScanResult = 0;

		bGetData = TRUE;
		//��¼��־
		strError.Format(_T("ɨ������߳�,�ô�ɨ�����,�Ѿ���ȡ���ݣ�����֪ͨ��Ϣ��������"));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://���յ�������Ϊ��
		case -2://���յ������ݷǷ�
			strError.Format(_T("ɨ������߳�,�ô�ɨ����ϣ�δ�ܻ�ȡ����,����֪ͨ��Ϣ��������"));
			break;
		case -3://��ǰ���յ��ظ�����
			strError.Format(_T("ɨ������߳�,�ô�ɨ����ϣ����յ��ظ������ݣ��ڲ�����������������Ϣ��������"));
			break;
		}
	}
	
	if(bNeedNotifyCaller)
	{
		//֪ͨ������
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
// �������ƣ�CThreadSMManager::fnSMScanCode
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
void CThreadSMManager::fnSMScanCode(void)
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

	CString strError;
	//��ʼִ��ɨ�����
	try
	{
		if(!m_bOpen || (NULL == m_pSMProtocol))
			throw -1;

		//��ȡ�ڹ��豸��ɨ����������
		strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"),SM_CMD_START,m_pSMProtocol->GetInnerDeviceProtocolClass()->Start());
		//��ȡ��ʽ����������
		strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
		if(strSendData.IsEmpty())
			throw -2;

		//������д����
		if(!WriteDataToComm(strSendData))
			throw -3;

		//��¼��־
		strError.Format(_T("��������%d:ɨ�������ѷ��ͣ�����ɨ�����ݣ����Ժ�"),0x01);
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,%s"),strError));	

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
		
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,%s"),strError));	
	}
	//��ǹ���״̬
	m_bWorking = FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::fnSMStopScan
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
void CThreadSMManager::fnSMStopScan(void)
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
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,����ǿ��ֹͣɨ��ָ��[�ɹ�]")));
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
		theApp.RecordMsgTemp(theApp.FmtStr(_T("ɨ������߳�,����ǿ��ֹͣɨ��ָ��[ʧ��]")));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadSMManager::fnSMControlLight
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
void CThreadSMManager::fnSMControlLight(IN const int& iLightType)
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
// �������ƣ�CThreadSMManager::OnMsgSetDisplay
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
void CThreadSMManager::OnMsgSetDisplay(WPARAM wParam,LPARAM lParam)
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
