/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 *
 * �ļ����ƣ�ThreadHttp.cpp
 * ժ    Ҫ��������շ�����WEB������������ͨѶ������ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��12��
 */
// ThreadHttp.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadHttp.h"

// CThreadHttp

IMPLEMENT_DYNCREATE(CThreadHttp, CThreadBase)

CThreadHttp::CThreadHttp()
	: m_wQueryEntryPort(80)
	, m_wQueryExitPort(80)
	, m_wDebitPort(80)
	, m_wDebitSecondPort(80)
	, m_wDebitCheckPort(80)
	, m_wDebitCancelPort(80)
	, m_wOpenLoopPort(80)
	, m_wCloseLoopPort(80)
	, m_wPassStationPort(80)
	, m_wSendRecordPort(80)
	, m_wHeartPort(80)

	, m_strQueryEntryURL(_T(""))
	, m_strQueryExitURL(_T(""))
	, m_strDebitURL(_T(""))
	, m_strDebitSecondURL(_T(""))
	, m_strDebitCheckURL(_T(""))
	, m_strDebitCancelURL(_T(""))
	, m_strOpenLoopURL(_T(""))
	, m_strCloseLoopURL(_T(""))
	, m_strPassStationURL(_T(""))
	, m_strHeartURL(_T(""))

	, m_strSendRecordURL(_T(""))

	, m_dwHeratFailForCenterReflectWeb(0)
	, m_dwHeratFailForOperationServer(0)
	, m_dwHeratFailWarningTimes(2)

	, m_bResendWorking(FALSE)
	, m_tResendDataSpanTime(30)
	, m_dwWaitThreadWorkTimeout(6000)
	, goDie(false)
	, dieNumber(0)
	, isAlive(false)
{
	::InitializeCriticalSection(&idleMutex);
	isFirstRun=true;

}

CThreadHttp::~CThreadHttp()
{
	//::DeleteCriticalSection(&idleMutex);
}


BOOL CThreadHttp::InitInstance()
{
	m_bLoopBreak = FALSE;

	//���ñ��������ļ�
	m_strPathLocalConfigFile.Format(_T("%s\\ReflectWeb.ini"), m_sTranstParam.PathCommDir);
	//��ȡ�����ļ�����ñ�׼��������Ϣ�����URL��ָ���˿�
	fnReadSpecailConfig();

	return TRUE;
}

int CThreadHttp::ExitInstance()
{
	::DeleteCriticalSection(&idleMutex);
	fnReleaseResource();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadHttp, CWinThread)
	ON_THREAD_MESSAGE(WM_THREAD_HTTP_SEND_DATA_REQUEST, OnMsgSendQueryToReflectWeb)
	ON_THREAD_MESSAGE(WM_THREAD_HTTP_SEND_HEART, OnMsgSendHeart)
END_MESSAGE_MAP()

UINT CThreadHttp::GetRandNumber(IN const UINT& nMin, IN const UINT& nMax)
{
	UINT nReturn = 0;
	UINT nRealMax = (0 == nMax) ? 1 : nMax;
	srand( (unsigned)::GetTickCount());
	nReturn = rand() % (nMax - nMin + 1)  + nMin ;
	//��������
	return nReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::CheckUnProcessDebitCancelRecord
//
// ������������⴦���Ƿ���δ�ݽ��ĳ�������
//
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::CheckUnProcessDebitCancelRecord()
{
	// return ;//debug
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::fnReleaseResource
//
// ������������Դ������
//
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�������غ���
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::fnReleaseResource()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::fnReadSpecailConfig
//
// ������������Դ������
//
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�������غ���
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::fnReadSpecailConfig()
{
	TCHAR szTemp[512] = {0};

	TCHAR szIP[32] = {0};
	WORD  wPort = 80;
	BOOL  bAddPort = FALSE;

	//��ȡ����������Ϣ
	CString strCommConfigFile;
	strCommConfigFile.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, FILE_NAME_APP_CONFIG);
	//1.0.3.4�汾�������������Ի���IP����60.205.140.198����Ϊ��60.205.140.198
	::GetPrivateProfileString(_T("ReflectWeb"), _T("IP"), _T("60.205.140.198"), szIP, _countof(szIP), strCommConfigFile);
	wPort = (WORD)::GetPrivateProfileInt(_T("ReflectWeb"), _T("Port"), 8081, strCommConfigFile);
	bAddPort = (BOOL)::GetPrivateProfileInt(_T("ReflectWeb"), _T("AddPort"), 0, strCommConfigFile);
	//���̵߳ȴ�ʱ��,��λ:���룬Ĭ�ϳ�ʱʱ��Ϊ 6 ��
	m_dwWaitThreadWorkTimeout = (DWORD)::GetPrivateProfileInt(_T("ReflectWeb"), _T("WaitThreadWorkTimeout"), 6000, strCommConfigFile);

	switch (m_sTranstParam.ProvinceID)
	{
	case 63:	//�ຣ
	{
		if (::PathFileExists(m_strPathLocalConfigFile))
		{
			m_wDebitPort = (WORD)::GetPrivateProfileInt(_T("Debit"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Debit"), _T("URL"), _T("http://10.63.0.91/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitURL.Format(_T("%s"), szTemp);

			m_wDebitSecondPort = (WORD)::GetPrivateProfileInt(_T("DebitSecond"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitSecond"), _T("URL"), _T("http://10.63.0.91/repay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitSecondURL.Format(_T("%s"), szTemp);

			m_wDebitCheckPort = (WORD)::GetPrivateProfileInt(_T("DebitCheck"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCheck"), _T("URL"), _T("http://10.63.0.91/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCheckURL.Format(_T("%s"), szTemp);

			m_wDebitCancelPort = (WORD)::GetPrivateProfileInt(_T("DebitCancel"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCancel"), _T("URL"), _T("http://10.63.0.91/home1"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCancelURL.Format(_T("%s"), szTemp);

			m_wHeartPort = (WORD)::GetPrivateProfileInt(_T("Heart"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Heart"), _T("URL"), _T("http://10.63.0.91/heartbeat"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strHeartURL.Format(_T("%s"), szTemp);
		}
		else
		{
			if (bAddPort)
			{
				//�ۿ�URL
				m_wDebitPort		= wPort;
				m_strDebitURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitPort);

				//��ѯURL
				m_wDebitCheckPort	= wPort;
				m_strDebitCheckURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitCheckPort);

				//����URL
				m_wDebitCancelPort	= wPort;
				m_strDebitCancelURL.Format(_T("http://%s:%d/pay/reverse"), szIP, m_wDebitCancelPort);

				//����URL
				m_wHeartPort		= wPort;
				m_strHeartURL.Format(_T("http://%s:%d/heartbeat"), szIP, m_wHeartPort);

			}
			else
			{
				//�ۿ�URL
				m_strDebitURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitPort		= wPort;
				//��ѯURL
				m_strDebitCheckURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitCheckPort	= wPort;
				//����URL
				m_strDebitCancelURL.Format(_T("http://%s/pay/reverse"), szIP);
				m_wDebitCancelPort	= wPort;
				//����URL
				m_strHeartURL.Format(_T("http://%s/heartbeat"), szIP);
				m_wHeartPort		= wPort;
			}
		}
	}
	break;
	case 44:	//�㶫
	{
		if (::PathFileExists(m_strPathLocalConfigFile))
		{
			switch (m_sTranstParam.LaneConfigInfo.RoadID)
			{
			case 30:
			case 31:
			{
				m_wDebitPort = (WORD)::GetPrivateProfileInt(_T("Debit"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("Debit"), _T("URL"), _T("http://10.44.125.241/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitURL.Format(_T("%s"), szTemp);

				m_wDebitSecondPort = (WORD)::GetPrivateProfileInt(_T("DebitSecond"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("DebitSecond"), _T("URL"), _T("http://10.44.125.241/repay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitSecondURL.Format(_T("%s"), szTemp);

				m_wDebitCheckPort = (WORD)::GetPrivateProfileInt(_T("DebitCheck"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("DebitCheck"), _T("URL"), _T("http://10.44.125.241/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitCheckURL.Format(_T("%s"), szTemp);

				m_wDebitCancelPort = (WORD)::GetPrivateProfileInt(_T("DebitCancel"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("DebitCancel"), _T("URL"), _T("http://10.44.125.241/pay/reverse"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitCancelURL.Format(_T("%s"), szTemp);

				m_wHeartPort = (WORD)::GetPrivateProfileInt(_T("Heart"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("Heart"), _T("URL"), _T("http://10.44.125.241/heartbeat"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strHeartURL.Format(_T("%s"), szTemp);
			}
			break;
			}
		}
		else
		{
			if (bAddPort)
			{
				//�ۿ�URL
				m_wDebitPort		= wPort;
				m_strDebitURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitPort);

				//��ѯURL
				m_wDebitCheckPort	= wPort;
				m_strDebitCheckURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitCheckPort);

				//����URL
				m_wDebitCancelPort	= wPort;
				m_strDebitCancelURL.Format(_T("http://%s:%d/pay/reverse"), szIP, m_wDebitCancelPort);

				//����URL
				m_wHeartPort		= wPort;
				m_strHeartURL.Format(_T("http://%s:%d/heartbeat"), szIP, m_wHeartPort);

			}
			else
			{
				//�ۿ�URL
				m_strDebitURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitPort		= wPort;
				//��ѯURL
				m_strDebitCheckURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitCheckPort	= wPort;
				//����URL
				m_strDebitCancelURL.Format(_T("http://%s/pay/reverse"), szIP);
				m_wDebitCancelPort	= wPort;
				//����URL
				m_strHeartURL.Format(_T("http://%s/heartbeat"), szIP);
				m_wHeartPort		= wPort;
			}
		}
	}
	break;
	default: //����ʡ
	{
		if (::PathFileExists(m_strPathLocalConfigFile))
		{
			m_wDebitPort = (WORD)::GetPrivateProfileInt(_T("Debit"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Debit"), _T("URL"), _T("http://60.205.140.198:8081/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitURL.Format(_T("%s"), szTemp);

			m_wDebitSecondPort = (WORD)::GetPrivateProfileInt(_T("DebitSecond"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitSecond"), _T("URL"), _T("http://60.205.140.198:8081/repay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitSecondURL.Format(_T("%s"), szTemp);

			m_wDebitCheckPort = (WORD)::GetPrivateProfileInt(_T("DebitCheck"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCheck"), _T("URL"), _T("http://60.205.140.198:8081/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCheckURL.Format(_T("%s"), szTemp);

			m_wDebitCancelPort = (WORD)::GetPrivateProfileInt(_T("DebitCancel"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCancel"), _T("URL"), _T("http://60.205.140.198:8081/pay/reverse"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCancelURL.Format(_T("%s"), szTemp);

			m_wHeartPort = (WORD)::GetPrivateProfileInt(_T("Heart"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Heart"), _T("URL"), _T("http://60.205.140.198:8081/heartbeat"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strHeartURL.Format(_T("%s"), szTemp);
		}
		else
		{
			if (bAddPort)
			{
				//�ۿ�URL
				m_wDebitPort		= wPort;
				m_strDebitURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitPort);

				//��ѯURL
				m_wDebitCheckPort	= wPort;
				m_strDebitCheckURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitCheckPort);

				//����URL
				m_wDebitCancelPort	= wPort;
				m_strDebitCancelURL.Format(_T("http://%s:%d/pay/reverse"), szIP, m_wDebitCancelPort);

				//����URL
				m_wHeartPort		= wPort;
				m_strHeartURL.Format(_T("http://%s:%d/heartbeat"), szIP, m_wHeartPort);
 
			}
			else
			{
				//�ۿ�URL
				m_strDebitURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitPort		= wPort;
				//��ѯURL
				m_strDebitCheckURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitCheckPort	= wPort;
				//����URL
				m_strDebitCancelURL.Format(_T("http://%s/pay/reverse"), szIP);
				m_wDebitCancelPort	= wPort;
				//����URL
				m_strHeartURL.Format(_T("http://%s/heartbeat"), szIP);
				m_wHeartPort		= wPort;
			}
		}
	}
	break;
	}
	//�������ʧ�ܴ����ﵽ��������ֵ
	m_dwHeratFailWarningTimes	= (DWORD)::GetPrivateProfileInt(_T("Heart"), _T("FailWarningTims"), 2, m_strPathLocalConfigFile);
	m_m_dwHeartCheckMin			= (DWORD)::GetPrivateProfileInt(_T("Heart"), _T("CheckMin"), 3600, m_strPathLocalConfigFile);
	m_m_dwHeartCheckMax			= (DWORD)::GetPrivateProfileInt(_T("Heart"), _T("CheckMax"), 3720, m_strPathLocalConfigFile);

	m_tResendDataSpanTime = ::GetGlobalApp()->m_tResendDataSpanTime;
}

// CThreadHttp message handlers

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::OnIdle
//
// �������������շ�����WEB������ͨѶ�߳��࣬���д�����
//
// ���������LONG lCount
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadHttp::OnIdle(LONG lCount)
{
	CScopeLock tmpIdle(&idleMutex);
	if (m_bExit)
	{
		dieNumber = 2017;
		::PostQuitMessage(0);
		return FALSE;
	}
	

	CString strLog;
	//��¼�ϴμ��ʱ��
	static time_t tLastHeartOpertateTime = 0;
	static INT64 iCheckHeartTimes = 0x00;
	static time_t tLastCheckResendTime = 0x00;

	static time_t tLastHeartSpanTime = 0;

	if ( (time(NULL) - tLastHeartOpertateTime) > tLastHeartSpanTime)
	{
		//MessageBox(0,"hearet","",0);
		//����һ���������
		OnMsgSendHeart(3, 1000);
		//MessageBox(0, "afeter beat", "", 0);
		//������ʱ��
		tLastHeartOpertateTime	= time(NULL);
		//�������������
		tLastHeartSpanTime		= GetRandNumber(m_m_dwHeartCheckMin, m_m_dwHeartCheckMax);
	}



	//����Ƿ���Ҫ������δ�ݽ��ĳ�������
	if ( (time(NULL) - tLastCheckResendTime) > m_tResendDataSpanTime)
	{
		CheckUnProcessDebitCancelRecord();
		//�����ϴμ��ʱ��
		tLastCheckResendTime = time(NULL);
	}

	//�̴߳����־��¼���
	static time_t tLastHttpLogTime = 0;

	if ( (time(NULL) - tLastHttpLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[֧��ͨѶ�߳�],�̴߳����־���"));
		GetGlobalApp()->RecordLog(strLog);
		//������ʱ��
		tLastHttpLogTime = time(NULL);
	}

	::Sleep(100);



	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::OnMsgSendQueryToReflectWeb
//
// ������������ָ����WEB���������Ͳ�ѯ����
//
// ���������WPARAM wParam	--	���͵���������ָ��
//         ��LPARAM lParam	--	���͵���������(�۷ѡ��۷�״̬��ѯ������)
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_HTTP_SEND_DATA_REQUEST
//			 ������Ϣ WM_WND_CHECK_SERVER_RESPONSE_QUERY_RESULT
////////////////////////////////////////////////////////////////////////////////////////////////
/*
void CThreadHttp::OnMsgSendQueryToReflectWeb(WPARAM wParam,LPARAM lParam)
{
	//���ù���״̬
	m_bWorking = TRUE;

	//����
//	Lock();

	CString strLog;
	DWORD	dwServerReturnResponseCode = 0x00;

	//��������
	WORD wSendDataType				= LOWORD(lParam);
	//�Ƿ���Ҫ����������
	BYTE byIsNeedResponseCaller		= LOBYTE(HIWORD(lParam));
	//�Ƿ���Ҫ�������صĳ�������
	BYTE byIsNeedRetrievalLocalData	= HIBYTE(HIWORD(lParam));;
	//�����������
	WORD wReturnCode = 0x00;
	//���ݲ�����ָ��
	CJsonOperationIF* pJsonIF = NULL;
	//ת������Ҫ���͵����ݸ�ʽ����
	CString strSendData;
	//�����Ҫ���ݵ�����
	CDVJsonBase* pDVJsonReceive = (CDVJsonBase*)wParam;

	//��ȡ��ǰʹ�õ�URL��ַ
	CString strURL;
	//��ȡ��ǰʹ�õĶ˿ں�
	INTERNET_PORT wPort = 80;
	//��ǰ���÷�ʽ
	int	iCallMode = 1;//Ĭ��GET��ʽ��ȡ����
	//�����ߵĴ�����
	HWND hWnd = NULL;

	//���������ص�����
	CString strResponseResult;
	//���������ص�״̬��
	DWORD dwResponseStateCode = 0x00;
	//����������ʱ��
	DWORD dwConsumeTime = 0x00;

	//�������
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
	try
	{
		if(NULL == pDVJsonReceive)
			throw -1;
		//���ݷ��͹������������ͣ��������ݸ�ʽ��ת��������÷����������Ϣ
		if(!DataConvertFromJson2String(wSendDataType,pDVJsonReceive,strURL,wPort,strSendData,hWnd,iCallMode,&pJsonIF))
			throw -2;
		strLog.Format(_T("[֧��ͨѶ�߳�]:�ѽ��յ�����[����:%s,%s]��׼����������"),GetSendDataTypeDesc(wSendDataType),pDVJsonReceive->PayIdentifier);
		GetGlobalApp()->RecordLog(strLog);
		strLog.Empty();

		//��ǿۿ����̽��յ�֧��ƾ֤(����ģʽ��Ч)
		if(GetGlobalApp()->m_bCurrentDebitMode)
		{
			if(JSON_DT_SEND_DIBIT == wSendDataType)
			{
				GetGlobalApp()->m_iDebitTaskNo = 0x03;
			}
		}

		DWORD	dwThreadID = 0;
		static tagHttpParam sParam = {0};

		//�������ṹ��
		sParam.URL					= _T("");
		sParam.ConsumeTime			= 0;
		sParam.SendData				= _T("");
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout= m_dwWaitThreadWorkTimeout - 2000;
		sParam.SessionControlTimeout= m_dwWaitThreadWorkTimeout - 1000;

		sParam.CallMode	= iCallMode;
		sParam.Port		= wPort;
		sParam.URL		= strURL;
		//ת����UTF-8��ʽ
		if(strSendData.IsEmpty())
		{
			strSendData.Format(_T("Illegal Data Format"));
		}
		sParam.SendData	= StringConvertByCodePage(strSendData,CP_ACP,CP_UTF8);

		//�������ݣ���ȡ����
		if( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
			throw -3;
		//�ȴ���ʱ
		if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,m_dwWaitThreadWorkTimeout))
		{
			//��ֹ�߳�
			sParam.NeedEndThread = TRUE;
		}
		//��ȡ���
		dwConsumeTime		= sParam.ConsumeTime;
		dwResponseStateCode	= sParam.ServerResponseCode;
		strResponseResult	= sParam.ServerResponseData;

		//�����������ֹ�������̣߳����ȡ���Ľ���������ǳɹ����ж���ʱ
		if( ( TRUE == sParam.NeedEndThread ) || ( OP_SUCCESS != sParam.ExecuteCode ) )
		{
			if( OP_SUCCESS != sParam.ExecuteCode )
			{
				//�����Ҫ��������
				PostThreadMessage(WM_THREAD_HTTP_SEND_HEART,(WPARAM)0x03,(LPARAM)0);
			}
			//����ǳ���������Ҫ�ж��Ƿ���Ҫ���泷������
			if(JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
			{
				//���泷����Ϣ
				CDVDebitCancel cData;
				cData.PayIdentifier = ((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier;
				cData.ProvinceID    = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
				cData.RoadID		= ::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.RoadID;
				cData.StationID		= _ttoi(::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
				cData.SubTime		= pDVJsonReceive->SubmitTime;
				cData.CreateTime	= CTime(time(NULL));
				m_cTableDebitCancel.Save(cData);
			}
			//�׳��쳣������ͨѶ��ʱ
			throw -0xF0;
		}

		//������õ�����,�ó���ʽ�����
		CDVJsonBase* pDVJsonBase = pJsonIF->FormatResponseResult2Class(strResponseResult,wSendDataType);
		if(NULL == pDVJsonBase)
			throw -4;
		pDVJsonBase->ProvinceID      = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
		pDVJsonBase->m_dwConsumeTime = dwConsumeTime;

#ifdef _DEBUG
		//��¼��־
		strSendData.TrimRight();
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:�������ݵ�������(����[����:%s])����ϸ����:%s;��ȡ��������:%s"),GetSendDataTypeDesc(wSendDataType),strSendData,strResponseResult));
#endif
		//�жϷ������������
		dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;

		//���÷��������״̬Ϊ����
		m_dwHeratFailForCenterReflectWeb	= 0;
		m_dwHeratFailForOperationServer		= 0;

		//����ǿ۷ѻ�۷Ѳ�ѯ
		if(   (JSON_DT_SEND_DIBIT	    == wSendDataType)
			||(JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
			)
		{
			//���ش��벻�������б��ɹ�/��������/��ά�볬ʱ/��ά��Ƿ�/����/������󣩣�����Ϊ���ڲ������쳣
			if(   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_SRC_ILLGAL				!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_INPUT_PASSWORD_ERROR			!= pDVJsonBase->ServerResponseCode)
				)
			{
				//�׳��쳣
				throw -5;
			}
		}
		else if(JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
		{
			//�����Ƿ���Ҫ�����صĳ�������
			if(byIsNeedRetrievalLocalData)
			{
				m_cTableDebitCancel.UpdateProcessMark(((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier);
				m_cTableDebitCancel.Delete();
			}
		}
		else
		{
			throw -7;
		}

		//֪ͨ�����߽��
		CString strNeedNotifyCaller(_T(",����Ҫ֪ͨ������"));
		if(0x01 == byIsNeedResponseCaller)
		{
			if(0 != m_sTranstParam.CallerThreadID)
			{
				strNeedNotifyCaller.Format(_T(",��֪ͨ������"));
				::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT,(WPARAM)pDVJsonBase,MAKELONG(wSendDataType,HTTP_RC_DATA_LEGAL ));
			}
		}
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:�ѽ��յ�����������[��������:%s,%s]��״̬:����ɹ�%s"),GetSendDataTypeDesc(wSendDataType),pDVJsonBase->PayIdentifier,strNeedNotifyCaller));
		//���ͨѶ״̬�쳣�����ڴ˽��о���
		if(   (FALSE != GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
			||(FALSE != GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist)
			)
		{
			//���÷��������״̬Ϊ����
			m_dwHeratFailForCenterReflectWeb = 0;
			m_dwHeratFailForOperationServer = 0;
			//���÷�����״̬ -- �շ�ר������������쳣
			GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
			//���÷�����״̬ -- ��Ӫƽ̨�������쳣
			GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�������ָ��Ϊ��
			strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,�������ָ��Ϊ��,����ҵ������̴߳���Ϊ:0x01(�ڲ�����)"));
			break;
		case -2://���ݸ�ʽת�������쳣
			strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,���ݸ�ʽת�������쳣,����ҵ������̴߳���Ϊ:0x02(�ڲ�����)"));
			break;
		case -3://�޷��������ݷ����߳�
			strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,�޷��������ݷ����߳�,����ҵ������̴߳���Ϊ:0x03(�ڲ�����)"));
			break;
		case -4://����������ʾʧ��
			strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,����ҵ������̴߳���Ϊ:0x04(�ڲ�����)�����������쳣���޷�����"));
			break;
		case -5://�۷ѻ��ѯ����������ǳɹ��������������룬�ж��ڲ�����
			strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݸ�ʽ�ɹ�[��������:%s],����ҵ������̴߳���Ϊ:0x05(�ڲ�����)����ϸ����:%s"),GetSendDataTypeDesc(wSendDataType),strResponseResult);
			break;
		case -6://����ʧ��
			strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݸ�ʽ�ɹ�(����ʧ�ܣ�,����ҵ������̴߳���Ϊ:0x06(�ڲ�����)"));
			break;
		case -7://�Ƿ����������
			strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ�ܣ��Ƿ�����,����ҵ������̴߳���Ϊ:0x07(�ڲ�����)"));
			break;
		}
		wReturnCode = abs(iErrorCode);

		//�������ý��
		if(0x01 == byIsNeedResponseCaller)
		{
			if(0 != m_sTranstParam.CallerThreadID)
			{
				::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT,(WPARAM)0,MAKELONG(wSendDataType,wReturnCode));
			}
		}
		//��¼��־
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc);
		}
		strLog.Empty();
	}

	//������Դ
	if(NULL != pDVJsonReceive)
	{
		delete pDVJsonReceive;
		pDVJsonReceive = NULL;
	}

	if(NULL != pJsonIF)
	{
		delete pJsonIF;
		pJsonIF = NULL;
	}

	//�رվ��
	::CloseHandle(hThreadHandle);

	//����
//	UnLock();

	//���ù���״̬
	m_bWorking = FALSE;
}
*/

//�´���汾�����Ӱ汾�жϣ��°汾ʹ���״ο۷ѣ���β�ѯ����
void CThreadHttp::OnMsgSendQueryToReflectWeb(WPARAM wParam, LPARAM lParam)
{
	// return ;//debug
	//���ù���״̬
	m_bWorking = TRUE;
	CString strLog = _T("");

	//��������
	WORD wSendDataType				= LOWORD(lParam);
	BYTE byIsNeedResponseCaller		= LOBYTE(HIWORD(lParam));	//�Ƿ���Ҫ����������
	BYTE byIsNeedRetrievalLocalData	= HIBYTE(HIWORD(lParam));	//�Ƿ���Ҫ�������صĳ�������
	WORD	wReturnCode = 0x00;									//�����������
	DWORD	dwServerReturnResponseCode = 0x00;

	CJsonOperationIF* pJsonIF = NULL;	//���ݲ�����ָ��
	CString strSendData = _T("");		//ת������Ҫ���͵����ݸ�ʽ����
	CDVJsonBase* pDVJsonReceive = (CDVJsonBase*)wParam;	//�����Ҫ���ݵ�����

	CString strURL = _T("");	//��ȡ��ǰʹ�õ�URL��ַ
	INTERNET_PORT wPort = 80;	//��ȡ��ǰʹ�õĶ˿ں�
	int	iCallMode = 1;			//Ĭ��GET��ʽ��ȡ����
	HWND hWnd = NULL;			//�����ߵĴ�����

	CString strResponseResult = _T("");	//���������ص�����
	DWORD dwResponseStateCode = 0x00;	//���������ص�״̬��
	DWORD dwConsumeTime = 0x00;			//����������ʱ��

	//�������
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;

	if ( 0 == theApp.m_iHTTPVersion )
	{
		try
		{
			if (NULL == pDVJsonReceive)
				throw - 1;
			int iPayChannelType = -1;
			//���ݷ��͹������������ͣ��������ݸ�ʽ��ת��������÷����������Ϣ
			if (!DataConvertFromJson2String(wSendDataType, pDVJsonReceive, strURL, wPort, strSendData, hWnd, iCallMode, &pJsonIF,iPayChannelType))
				throw - 2;

			strLog.Format(_T("[֧��ͨѶ�߳�]:�ѽ��յ�����[����:%s,%s]��׼����������"), GetSendDataTypeDesc(wSendDataType), pDVJsonReceive->PayIdentifier);
			GetGlobalApp()->RecordLog(strLog);
			strLog.Empty();

			//��ǿۿ����̽��յ�֧��ƾ֤(����ģʽ��Ч)
			if (GetGlobalApp()->m_bCurrentDebitMode)
			{
				if (JSON_DT_SEND_DIBIT == wSendDataType)
				{
					GetGlobalApp()->m_iDebitTaskNo = 0x03;
				}
			}

			DWORD	dwThreadID = 0;
			static tagHttpParam sParam = {0};

			//�������ṹ��
			sParam.URL					= _T("");
			sParam.ConsumeTime			= 0;
			sParam.SendData				= _T("");
			sParam.ExecuteCode			= 0xFF;
			sParam.ConsumeTime			= 0;
			sParam.ServerResponseData	= _T("");
			sParam.ServerResponseCode	= 0;
			sParam.NeedEndThread		= FALSE;

			//����֧�����ͽ��г�ʱԤ��
			if(2 == iPayChannelType)
			{
				//����֧����ǿ�ƽ�ʱ�����Ϊ15��
				sParam.SessionReceiveTimeout= 15*1000;
				sParam.SessionControlTimeout= 16*1000;
				sParam.SessionSendTimeout	= 30*1000;
			}
			else//������֧�����������õĳ�ʱʱ��
			{
				sParam.SessionReceiveTimeout= m_dwWaitThreadWorkTimeout - 1500;
				sParam.SessionControlTimeout= m_dwWaitThreadWorkTimeout - 1000;
				sParam.SessionSendTimeout	= 2*1000;
			}


			sParam.CallMode	= iCallMode;
			sParam.Port		= wPort;
			sParam.URL		= strURL;
			//ת����UTF-8��ʽ
			if (strSendData.IsEmpty())
			{
				strSendData.Format(_T("Illegal Data Format"));
			}
			sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

			//�������ݣ���ȡ����
			if ( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
				throw - 3;

			if(2 == iPayChannelType)
			{
				//�����NFC֧��,ǿ�ƽ���ʱʱ������Ϊ15��
				if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,15*1000))
				{
					//��ֹ�߳�
					sParam.NeedEndThread = TRUE;
				}
			}
			else
			{
				if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,m_dwWaitThreadWorkTimeout))
				{
					//��ֹ�߳�
					sParam.NeedEndThread = TRUE;
				}
			}
			
			//��ȡ���
			dwConsumeTime		= sParam.ConsumeTime;
			dwResponseStateCode	= sParam.ServerResponseCode;
			strResponseResult	= sParam.ServerResponseData;

			//�����������ֹ�������̣߳����ȡ���Ľ���������ǳɹ����ж���ʱ
			if ( ( TRUE == sParam.NeedEndThread ) || ( OP_SUCCESS != sParam.ExecuteCode ) )
			{
				if ( OP_SUCCESS != sParam.ExecuteCode )
				{
					//�����Ҫ��������
					PostThreadMessage(WM_THREAD_HTTP_SEND_HEART, (WPARAM)0x03, (LPARAM)0);
				}
				//����ǳ���������Ҫ�ж��Ƿ���Ҫ���泷������
				if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
				{
					//���泷����Ϣ
					CDVDebitCancel cData;
					cData.PayIdentifier = ((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier;
					cData.ProvinceID    = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
					cData.RoadID		= ::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.RoadID;
					cData.StationID		= _ttoi(::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
					cData.SubTime		= pDVJsonReceive->SubmitTime;
					cData.CreateTime	= CTime(time(NULL));
					m_cTableDebitCancel.Save(cData);
				}
				//�׳��쳣������ͨѶ��ʱ
				throw - 0xF0;
			}

			//������õ�����,�ó���ʽ�����
			CDVJsonBase* pDVJsonBase = pJsonIF->FormatResponseResult2Class(strResponseResult, wSendDataType);
			if (NULL == pDVJsonBase)
				throw - 4;
			pDVJsonBase->ProvinceID      = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
			pDVJsonBase->m_dwConsumeTime = dwConsumeTime;

#ifdef _DEBUG
			//��¼��־
			strSendData.TrimRight();
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:�������ݵ�������(����[����:%s])����ϸ����:%s;��ȡ��������:%s"), GetSendDataTypeDesc(wSendDataType), strSendData, strResponseResult));
#endif
			//�жϷ������������
			dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;
		

			//���÷��������״̬Ϊ����
			m_dwHeratFailForCenterReflectWeb	= 0;
			m_dwHeratFailForOperationServer		= 0;

			//����ǿ۷ѻ�۷Ѳ�ѯ
			if (   (JSON_DT_SEND_DIBIT	    == wSendDataType)
			        || (JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
			   )
			{
				//���ش��벻�������б��ɹ�/��������/��ά�볬ʱ/��ά��Ƿ�/����/������󣩣�����Ϊ���ڲ������쳣
				if (   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_SRC_ILLGAL				!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_INPUT_PASSWORD_ERROR			!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_NFC_NO_SUPPORT				!= pDVJsonBase->ServerResponseCode) //NFC,��Ƭ��֧������
				        && (HTTP_RC_NFC_NO_INIT					!= pDVJsonBase->ServerResponseCode) //NFC,��Ƭδ��ʼ��
				        && (HTTP_RC_NFC_NO_SERVICES				!= pDVJsonBase->ServerResponseCode) //NFC,����������ͣ����
				   )
				{
					//�׳��쳣
					throw - 5;
				}
			}
			else if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
			{
				//�����Ƿ���Ҫ�����صĳ�������
				if (byIsNeedRetrievalLocalData)
				{
					m_cTableDebitCancel.UpdateProcessMark(((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier);
					m_cTableDebitCancel.Delete();
				}
			}
			else
			{
				throw - 7;
			}

			//֪ͨ�����߽��
			CString strNeedNotifyCaller(_T(",����Ҫ֪ͨ������"));
			if (0x01 == byIsNeedResponseCaller)
			{
				if (0 != m_sTranstParam.CallerThreadID)
				{
					strNeedNotifyCaller.Format(_T(",��֪ͨ������"));
					::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)pDVJsonBase, MAKELONG(wSendDataType, HTTP_RC_DATA_LEGAL ));
				}
			}

		

			//��¼��־
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:�ѽ��յ�����������[��������:%s,%s]��״̬:����ɹ�%s"), GetSendDataTypeDesc(wSendDataType), pDVJsonBase->PayIdentifier, strNeedNotifyCaller));
			//���ͨѶ״̬�쳣�����ڴ˽��о���
			if (   (FALSE != GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
			        || (FALSE != GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist)
			   )
			{
				//���÷��������״̬Ϊ����
				m_dwHeratFailForCenterReflectWeb = 0;
				m_dwHeratFailForOperationServer = 0;
				//���÷�����״̬ -- �շ�ר������������쳣
				GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
				//���÷�����״̬ -- ��Ӫƽ̨�������쳣
				GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
			}
		}
		catch (int& iErrorCode)
		{
			switch (iErrorCode)
			{
			case -1://�������ָ��Ϊ��
				strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,�������ָ��Ϊ��,����ҵ������̴߳���Ϊ:0x01(�ڲ�����)"));
				break;
			case -2://���ݸ�ʽת�������쳣
				strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,���ݸ�ʽת�������쳣,����ҵ������̴߳���Ϊ:0x02(�ڲ�����)"));
				break;
			case -3://�޷��������ݷ����߳�
				strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,�޷��������ݷ����߳�,����ҵ������̴߳���Ϊ:0x03(�ڲ�����)"));
				break;
			case -4://����������ʾʧ��
				strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,����ҵ������̴߳���Ϊ:0x04(�ڲ�����)�����������쳣���޷�����"));
				break;
			case -5://�۷ѻ��ѯ����������ǳɹ��������������룬�ж��ڲ�����
				strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݸ�ʽ�ɹ�[��������:%s],����ҵ������̴߳���Ϊ:0x05(�ڲ�����)����ϸ����:%s"), GetSendDataTypeDesc(wSendDataType), strResponseResult);
				break;
			case -6://����ʧ��
				strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݸ�ʽ�ɹ�(����ʧ�ܣ�,����ҵ������̴߳���Ϊ:0x06(�ڲ�����)"));
				break;
			case -7://�Ƿ����������
				strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ�ܣ��Ƿ�����,����ҵ������̴߳���Ϊ:0x07(�ڲ�����)"));
				break;
			}
			wReturnCode = abs(iErrorCode);

			//�������ý��
			if (0x01 == byIsNeedResponseCaller)
			{
				if (0 != m_sTranstParam.CallerThreadID)
				{
					::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)0, MAKELONG(wSendDataType, wReturnCode));
				}
			}
			//��¼��־
			if (strLog != m_strLastErrorDesc)
			{
				m_strLastErrorDesc = strLog;
				GetGlobalApp()->RecordLog(m_strLastErrorDesc);
			}
			strLog.Empty();
		}

		//������Դ
		if (NULL != pDVJsonReceive)
		{
			delete pDVJsonReceive;
			pDVJsonReceive = NULL;
		}

		if (NULL != pJsonIF)
		{
			delete pJsonIF;
			pJsonIF = NULL;
		}

		//�رվ��
		::CloseHandle(hThreadHandle);
	}
	else//�汾��0���۷Ѵ���Ϊ�״ο۷ѣ�����ѭ����ѯ
	{
		int	iDealSN = 0;	//�������кţ�����״ο۷ѻ��Ǻ�����ѯ
		int	iLoop = 1;		//ѭ��������ܴ���
		int		iPayChannelType = -1;//����ǿ۷����󣬸����ָ�������ֿ۷����ͣ�-1/0��δ֪��1��������֧��ƽ̨��2������֧����
		bool	bWaitSleep = false;
		m_bLoopBreak = FALSE;

		if ( (JSON_DT_SEND_DIBIT == wSendDataType) || (JSON_DT_SEND_DIBIT_CHECK == wSendDataType) )
		{
			if (JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
			{
				iDealSN = 1;
			}
			iLoop = 99;	//���ڿ۷ѣ���ѯ�������Զ��ѭ��������ִֻ��һ�Σ��б��ػ��棩
		}

		for (int i = 0; i < iLoop; ++i)
		{
			if ( m_bLoopBreak )
			{
				break;
			}

			try
			{
				if (NULL == pDVJsonReceive)
				{
					m_bLoopBreak = TRUE;
					throw - 1;
				}

				//���ݷ��͹������������ͣ��������ݸ�ʽ��ת��������÷����������Ϣ
				if (!DataConvertFromJson2String(wSendDataType, pDVJsonReceive, strURL, wPort, strSendData, hWnd, iCallMode, &pJsonIF,iPayChannelType, iDealSN))
				{
					m_bLoopBreak = TRUE;
					throw - 2;
				}
				iDealSN += 1;

				strLog.Format(_T("[֧��ͨѶ�߳�]:�ѽ��յ�����[����:%s,%s]��׼����������"), GetSendDataTypeDesc(wSendDataType), pDVJsonReceive->PayIdentifier);
				GetGlobalApp()->RecordLog(strLog);
				strLog.Empty();

				//��ǿۿ����̽��յ�֧��ƾ֤(����ģʽ��Ч)
				if (GetGlobalApp()->m_bCurrentDebitMode)
				{
					if (JSON_DT_SEND_DIBIT == wSendDataType)
					{
						GetGlobalApp()->m_iDebitTaskNo = 0x03;
					}
				}

				DWORD	dwThreadID = 0;
				static tagHttpParam sParam = {0};

				//�������ṹ��
				sParam.ConsumeTime			= 0;
				sParam.SendData				= _T("");
				sParam.ExecuteCode			= 0xFF;
				sParam.ConsumeTime			= 0;
				sParam.ServerResponseData	= _T("");
				sParam.ServerResponseCode	= 0;
				sParam.NeedEndThread		= FALSE;

				//����֧�����ͽ��г�ʱԤ��
				if(2 == iPayChannelType)
				{
					//NFC֧����ǿ�ƽ�ʱ�����Ϊ15��
					sParam.SessionReceiveTimeout= 15*1000;
					sParam.SessionControlTimeout= 16*1000;
					sParam.SessionSendTimeout	= 30*1000;
				}
				else
				{
					sParam.SessionReceiveTimeout= m_dwWaitThreadWorkTimeout - 1500;
					sParam.SessionControlTimeout= m_dwWaitThreadWorkTimeout - 1000;
					sParam.SessionSendTimeout	= 2*1000;
				}
				
				sParam.CallMode	= iCallMode;
				sParam.Port		= wPort;
				sParam.URL		= strURL;
				//ת����UTF-8��ʽ
				if (strSendData.IsEmpty())
				{
					strSendData.Format(_T("Illegal Data Format"));
				}
				sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

				if ( m_bLoopBreak )	{	break;	}

				//�������ݣ���ȡ����
				if ( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
				{
					m_bLoopBreak = TRUE;
					throw - 3;
				}
				//�ȴ���ʱ
				if(2 == iPayChannelType)
				{
					//�����NFC֧��,ǿ�ƽ���ʱʱ������Ϊ15��
					if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,15*1000))
					{
						//��ֹ�߳�
						sParam.NeedEndThread = TRUE;
					}
				}
				else
				{
					if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,m_dwWaitThreadWorkTimeout))
					{
						//��ֹ�߳�
						sParam.NeedEndThread = TRUE;
					}
				}
				//��ȡ���
				dwConsumeTime		= sParam.ConsumeTime;
				dwResponseStateCode	= sParam.ServerResponseCode;
				strResponseResult	= sParam.ServerResponseData;

				//�����������ֹ�������̣߳����ȡ���Ľ���������ǳɹ����ж���ʱ
				if ( ( TRUE == sParam.NeedEndThread ) || ( OP_SUCCESS != sParam.ExecuteCode ) )
				{
					if ( OP_SUCCESS != sParam.ExecuteCode )
					{
						//�����Ҫ��������
						PostThreadMessage(WM_THREAD_HTTP_SEND_HEART, (WPARAM)0x03, (LPARAM)0);
					}
					//����ǳ���������Ҫ�ж��Ƿ���Ҫ���泷������
					if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
					{
						//���泷����Ϣ
						CDVDebitCancel cData;
						cData.PayIdentifier = ((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier;
						cData.ProvinceID    = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
						cData.RoadID		= ::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.RoadID;
						cData.StationID		= _ttoi(::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
						cData.SubTime		= pDVJsonReceive->SubmitTime;
						cData.CreateTime	= CTime(time(NULL));
						m_cTableDebitCancel.Save(cData);
					}
					//��ʱ3��
					if( iDealSN >= 3)
					{
						for(int m=0; m < 60; m++)
						{
							if( m_bLoopBreak )
							{
								break;
							}
							::Sleep(50);
						}
					}
					else
					{
						::Sleep(50);
					}
					//�׳��쳣������ͨѶ��ʱ
					throw - 0xF0;
				}

				//������õ�����,�ó���ʽ�����
				CDVJsonBase* pDVJsonBase = pJsonIF->FormatResponseResult2Class(strResponseResult, wSendDataType);
				if (NULL == pDVJsonBase)
				{
					m_bLoopBreak = TRUE;
					throw - 4;
				}
				pDVJsonBase->ProvinceID      = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
				pDVJsonBase->m_dwConsumeTime = dwConsumeTime;

#ifdef _DEBUG
				//��¼��־
				strSendData.TrimRight();
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:�������ݵ�������(����[����:%s])����ϸ����:%s;��ȡ��������:%s"), GetSendDataTypeDesc(wSendDataType), strSendData, strResponseResult));
#endif
				//�жϷ������������
				dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;

				//���÷��������״̬Ϊ����
				m_dwHeratFailForCenterReflectWeb	= 0;
				m_dwHeratFailForOperationServer		= 0;

				//����ǿ۷ѻ�۷Ѳ�ѯ
				if (   (JSON_DT_SEND_DIBIT	    == wSendDataType)
				        || (JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
				   )
				{
					//���ش��벻�������б��ɹ�/��������/��ά�볬ʱ/��ά��Ƿ�/����/�������/��ѯʧ�ܣ�������Ϊ���ڲ������쳣
					if (   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_SRC_ILLGAL				!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_INPUT_PASSWORD_ERROR			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_QUERY_ERROR			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_FAIL					!= pDVJsonBase->ServerResponseCode) //���Թ����г��ָ÷���
					        && (HTTP_RC_NFC_NO_SUPPORT				!= pDVJsonBase->ServerResponseCode) //NFC,��Ƭ��֧������
					        && (HTTP_RC_NFC_NO_INIT					!= pDVJsonBase->ServerResponseCode) //NFC,��Ƭδ��ʼ��
					        && (HTTP_RC_NFC_NO_SERVICES				!= pDVJsonBase->ServerResponseCode) //NFC,����������ͣ����
					   )
					{
						//�׳��쳣
						m_bLoopBreak = TRUE;
						throw - 5;
					}

					//������ȷ������صģ����ټ���ѭ�����ɹ�/��ά�볬ʱ/��ά��Ƿ�/����/�������
					//ֻ�з����������뼰��ѯʧ�ܣ��۷ѽ����С��������ż���ѭ����ѯ
					if (		(HTTP_RC_DEBIT_SUCCESS				== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_CODE_TIMEOUT			== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_SRC_ILLGAL			== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_INPUT_PASSWORD_ERROR		== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_FAIL					== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_NFC_NO_SUPPORT				== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_NFC_NO_INIT				== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_NFC_NO_SERVICES			== pDVJsonBase->ServerResponseCode)
					   )
					{
						bWaitSleep = false;
						m_bLoopBreak = TRUE;
					}
					else if ((HTTP_RC_DEBIT_NEED_PASSWORD	== pDVJsonBase->ServerResponseCode) //��������״̬����ѭ��
					         ||	(HTTP_RC_DEBIT_QUERY_ERROR		== pDVJsonBase->ServerResponseCode)//��ѯʧ�ܼ���ѭ��
					        )
					{
						if ((HTTP_RC_DEBIT_NEED_PASSWORD	== pDVJsonBase->ServerResponseCode))
						{
							//Ҫ���豸����֪ͨ(���ټ��10��)
							static time_t tLastSendInputPSWInfo = 0;
							if ( (time(NULL) - tLastSendInputPSWInfo) > 10)
							{
								::GetGlobalApp()->m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI, (WPARAM)SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT, (LPARAM)0x00);
								tLastSendInputPSWInfo = time(NULL);
							}
						}
						//���õȴ���ѯ���
						bWaitSleep = true;
					}
				}
				else if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
				{
					//�����Ƿ���Ҫ�����صĳ�������
					if (byIsNeedRetrievalLocalData)
					{
						m_cTableDebitCancel.UpdateProcessMark(((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier);
						m_cTableDebitCancel.Delete();
					}
				}
				else
				{
					m_bLoopBreak = TRUE;
					throw - 7;
				}

				//֪ͨ�����߽��
				CString strNeedNotifyCaller(_T(",����Ҫ֪ͨ������"));
				if (0x01 == byIsNeedResponseCaller)
				{
					if (0 != m_sTranstParam.CallerThreadID)
					{
						strNeedNotifyCaller.Format(_T(",��֪ͨ������"));


						::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)pDVJsonBase, MAKELONG(wSendDataType, HTTP_RC_DATA_LEGAL ));
					}
				}

				
				//��¼��־
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:�ѽ��յ�����������[��������:%s,%s]��״̬:����ɹ�%s"), GetSendDataTypeDesc(wSendDataType), pDVJsonBase->PayIdentifier, strNeedNotifyCaller));
				//���ͨѶ״̬�쳣�����ڴ˽��о���
				if (   (FALSE != GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
				        || (FALSE != GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist)
				   )
				{
					//���÷��������״̬Ϊ����
					m_dwHeratFailForCenterReflectWeb = 0;
					m_dwHeratFailForOperationServer = 0;
					//���÷�����״̬ -- �շ�ר������������쳣
					GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
					//���÷�����״̬ -- ��Ӫƽ̨�������쳣
					GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
				}

				//ѭ���ȴ�����Ŀǰ�ȴ���� 3000 ����
				if ( bWaitSleep )
				{
					for (int m = 0; m < 60; m++)
					{
						if ( m_bLoopBreak )
						{
							break;
						}
						::Sleep(50);
					}
				}
			}
			catch (int& iErrorCode)
			{
				switch (iErrorCode)
				{
				case -1://�������ָ��Ϊ��
					strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,�������ָ��Ϊ��,����ҵ������̴߳���Ϊ:0x01(�ڲ�����)"));
					break;
				case -2://���ݸ�ʽת�������쳣
					strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,���ݸ�ʽת�������쳣,����ҵ������̴߳���Ϊ:0x02(�ڲ�����)"));
					break;
				case -3://�޷��������ݷ����߳�
					strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,�޷��������ݷ����߳�,����ҵ������̴߳���Ϊ:0x03(�ڲ�����)"));
					break;
				case -4://����������ʾʧ��
					strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ��,����ҵ������̴߳���Ϊ:0x04(�ڲ�����)�����������쳣���޷�����"));
					break;
				case -5://�۷ѻ��ѯ����������ǳɹ��������������룬�ж��ڲ�����
					strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݸ�ʽ�ɹ�[��������:%s],����ҵ������̴߳���Ϊ:0x05(�ڲ�����)����ϸ����:%s"), GetSendDataTypeDesc(wSendDataType), strResponseResult);
					break;
				case -6://����ʧ��
					strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݸ�ʽ�ɹ�(����ʧ�ܣ�,����ҵ������̴߳���Ϊ:0x06(�ڲ�����)"));
					break;
				case -7://�Ƿ����������
					strLog.Format(_T("[֧��ͨѶ�߳�]:��������������ʧ�ܣ��Ƿ�����,����ҵ������̴߳���Ϊ:0x07(�ڲ�����)"));
					break;

				case -0xF0:
					strLog.Format(_T("[֧��ͨѶ�߳�]:�������������ݳ�ʱ, ����ҵ������̴߳���Ϊ:0xF0"));
					break;

				default:
					break;
				}
				wReturnCode = abs(iErrorCode);

				if ( -0xF0 != iErrorCode )
				{
					//�������ý��
					if (0x01 == byIsNeedResponseCaller)
					{
						if (0 != m_sTranstParam.CallerThreadID)
						{
							::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)0, MAKELONG(wSendDataType, wReturnCode));
						}
					}
				}

				//��¼��־
				if (strLog != m_strLastErrorDesc)
				{
					m_strLastErrorDesc = strLog;
					GetGlobalApp()->RecordLog(m_strLastErrorDesc);
				}
				strLog.Empty();
			}

			//����pJsonIF��hThreadHandle������ѭ���ڲ�����ʹ�ã���ÿ��ѭ��������������ͷ�
			if (NULL != pJsonIF)
			{
				delete pJsonIF;
				pJsonIF = NULL;
			}

			//�رվ��
			::CloseHandle(hThreadHandle);
			hThreadHandle = INVALID_HANDLE_VALUE;
		}

		//������Դ
		if (NULL != pDVJsonReceive)
		{
			delete pDVJsonReceive;
			pDVJsonReceive = NULL;
		}
	}

	//���ù���״̬
	m_bWorking = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::OnMsgSendHeart
//
// ������������Ҫ����һ�����������״̬
//
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��9��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::OnMsgSendHeart(WPARAM wParam, LPARAM lParam)
{

	int iCount = (int)wParam;
	int iSleep = (int)lParam;
	static time_t tLastCheckHeartTime = 0x00;
	//�ж���һ�μ������ʱ��,����10���򲻽��иôμ��
	if ( (time(NULL) - tLastCheckHeartTime) < 10)
		return;
	//�����������
	for (int i = 0; i < iCount; ++i)
	{
		//������־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:֧��ƽ̨������֤,�� %d ��"), i + 1), LOG_LEVEL_ERROR);
		CheckHeartState();
		Sleep(iSleep);
	}
	tLastCheckHeartTime = time(NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::DataConvertFromJson2String
//
// �������������ݸ�ʽת��,�ӵ����ߴ�������DVJson��ʽת�����ַ�����ʽ
//
// ���������IN const WORD& wSendDataType			--	���ݵ���������
//         ��IN OUT CDVJsonBase* pDVJsonReceive		--	���ݵĲ���ָ��
//         ��IN OUT CString& strUseURL				--	��������URL
//         ��IN OUT INTERNET_PORT& wUsePort			--	�������Ķ˿�
//         ��IN OUT CString& strSendData			--	��������Ҫ������ַ���
//         ��IN OUT HWND& hWnd						--	�����������ߴ�����
//         ��IN OUT int& iCallMode					--	�������ĵ���ģʽ(1,Get;2,Post)
//         ��IN OUT CJsonOperationIF* pJsonIF		--	��������JSON������ָ��
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��14��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CThreadHttp::DataConvertFromJson2String(IN const WORD& wSendDataType, IN OUT CDVJsonBase* pDVJsonReceive, IN OUT CString& strURL, IN OUT INTERNET_PORT& wPort, IN OUT CString& strSendData, IN OUT HWND& hWnd, IN OUT int& iCallMode, IN OUT CJsonOperationIF** pJsonIF,int& iPayChannelType, int nSN )
{
	BOOL bReturn = TRUE;

	try
	{
		switch (wSendDataType)
		{
		case JSON_DT_SEND_DIBIT:		//֧������(�۷�)
		{
			//�����Ӧ��������ָ��
			CDVJsonDebit* pDVJsonDebit = (CDVJsonDebit* )pDVJsonReceive;
			if (NULL == pDVJsonDebit)
			{
				throw - 1;
			}
			//��õ����ߴ�����
			hWnd = pDVJsonDebit->m_hCallerWnd;

			//�ж�������֧������(1.0.8.4�����ж�֧�������Ƿ�������
			if(   (TRUE != pDVJsonDebit->NFCPayData.IsEmpty())																			//����NFC֧��
				||((0 == pDVJsonDebit->QRCodeData.Left(2).CompareNoCase(_T("62"))) && (19 == pDVJsonDebit->QRCodeData.GetLength()))		//������ά��
				)
			{
				iPayChannelType = 2;
			}

			//�������ݲ�����
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//ת����JSON���ݸ�ʽ
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebit, nSN);
			//��÷����������Ϣ
			strURL = m_strDebitURL;
			wPort  = m_wDebitPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_DIBIT_CHECK:	//֧������(��ѯ)
		{
			//�����Ӧ��������ָ��
			CDVJsonDebitCheck* pDVJsonDebitCheck = (CDVJsonDebitCheck* )pDVJsonReceive;
			if (NULL == pDVJsonDebitCheck)
			{
				throw - 1;
			}
			//��õ����ߴ�����
			hWnd = pDVJsonDebitCheck->m_hCallerWnd;

			//�������ݲ�����
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//ת����JSON���ݸ�ʽ
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitCheck, nSN);
			//��÷����������Ϣ
			strURL = m_strDebitCheckURL;
			wPort  = m_wDebitCheckPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_DIBIT_CANCEL:	//֧������(����)
		{
			//�����Ӧ��������ָ��
			CDVJsonDebitCancel* pDVJsonDebitCancel = (CDVJsonDebitCancel* )pDVJsonReceive;
			if (NULL == pDVJsonDebitCancel)
			{
				throw - 1;
			}
			//��õ����ߴ�����
			hWnd = pDVJsonDebitCancel->m_hCallerWnd;

			//�������ݲ�����
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//ת����JSON���ݸ�ʽ
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitCancel);
			//��÷����������Ϣ
			strURL = m_strDebitCancelURL;
			wPort  = m_wDebitCancelPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_DIBIT_SECOND:	//֧������(���δ���)
		{
			//�����Ӧ��������ָ��
			CDVJsonDebitSecond* pDVJsonDebitSecond = (CDVJsonDebitSecond* )pDVJsonReceive;
			if (NULL == pDVJsonDebitSecond)
			{
				throw - 1;
			}
			//��õ����ߴ�����
			hWnd = pDVJsonDebitSecond->m_hCallerWnd;

			//�������ݲ�����
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//ת����JSON���ݸ�ʽ
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitSecond);
			//��÷����������Ϣ
			strURL = m_strDebitSecondURL;
			wPort  = m_wDebitSecondPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_HEART:	//�������
		{
			//�����Ӧ��������ָ��
			CDVJsonHeart* pDVJsonHeart = (CDVJsonHeart* )pDVJsonReceive;
			if (NULL == pDVJsonHeart)
			{
				throw - 1;
			}
			//��õ����ߴ�����
			hWnd = pDVJsonHeart->m_hCallerWnd;

			//�������ݲ�����
			*pJsonIF = new CJsonHeartOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//ת����JSON���ݸ�ʽ
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonHeart);
			//��÷����������Ϣ
			strURL = m_strHeartURL;
			wPort  = m_wHeartPort;
			iCallMode = 2;
		}
		break;
		default:
			throw - 3;
		}
	}
	catch (int& iErrorCode)
	{
		bReturn = FALSE;

		switch (iErrorCode)
		{
		case -1://����ָ��Ϊ��
			break;
		case -2://��Դ�޷�����
			break;
		case -3://�������ݸ�ʽ�Ƿ�
			break;
		}
	}

	return bReturn;
}
UINT  WINAPI CThreadHttp::SendData_ByWinNet(LPVOID lpParam)
{return 0;}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::SendData
//
// �������������ݵݽ� -- �̹߳�������
//
// ���������LPVOID lpParam	--	�����ṹ��ָ��
// ���������UINT
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��14��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
UINT WINAPI CThreadHttp::SendData(LPVOID lpParam)
{
	//return SendData_ByWinNet(lpParam);

	UINT dwReturn = 0x01;
	BOOL bIsHeart = FALSE;
	CString strErrorDesc;
	tagHttpParam* pParam = (tagHttpParam*)lpParam;

	//��ʱ��ʼ
	DWORD dwStart = ::GetTickCount();

	//�����������Ӷ���
	CInternetSession session(_T("HttpClient"));


	CHttpConnection *pHtCon = NULL;
	CHttpFile *pHtFile		= NULL;

	int iTaskNo = 0x00;
	try
	{
		if (NULL == lpParam)
			throw 1;

		//���ճ�ʱ
		session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, pParam->SessionReceiveTimeout);
		//��Ӧ��ʱ
		session.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, pParam->SessionControlTimeout);

		/*

				session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 1000 * 6); //��������ĳ�ʱʱ��
		        session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 2); //��������ĳ�ʱʱ��
		        session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1); //��������ĳ�ʱʱ��
		        session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 1000*2); //��������ĳ�ʱʱ��
		        session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 1000*2); //��������ĳ�ʱʱ��
		*/
		session.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 2*1000);


		//����URL����ȷ��
		INTERNET_PORT wGetPort;	//���ڱ���Ŀ��HTTP����˿�
		CString strServer;		//���ڱ����������ַ
		CString	strObject;		//���ڱ����ļ���������
		DWORD	dwServiceType;	//���ڱ����������

		//����URL����ȡ��Ϣ
		if (!AfxParseURL(pParam->URL, dwServiceType, strServer, strObject, wGetPort))
		{
			throw 2;
		}
		//�ж��Ƿ�����
		if ( -1 == strObject.Find(_T("heart")))
		{
			bIsHeart = FALSE;
		}
		else
		{
			bIsHeart = TRUE;
		}

		iTaskNo = 0x01;

		//�����Ҫǿ�ƽ�����������߳�
		if (pParam->NeedEndThread)
			goto END;

		//��������
		//BOOL bBreakPing = FALSE;
		//int iPingCount = 0x00;
		//BOOL bPing = FALSE;
		//do
		//{
		//	bPing = Ping(strServer,32);
		//	if(bPing || (++iPingCount > 4))
		//	{
		//		bBreakPing = TRUE;
		//	}
		//}while(!bBreakPing);
		//
		//if(!bPing)
		//{
		//	throw 3;
		//}


		iTaskNo = 0x02;

		//�����Ҫǿ�ƽ�����������߳�
		if (pParam->NeedEndThread)
			goto END;

		//������������

		pHtCon = session.GetHttpConnection(strServer, wGetPort);
		if ( NULL == pHtCon )
		{
			//������������ʧ��
			session.Close();
			//�׳��쳣
			throw 4;
		}

		iTaskNo = 0x03;


		//�����Ҫǿ�ƽ�����������߳�
		if (pParam->NeedEndThread)
			goto END;


		//����GET/POST����
		pHtFile = pHtCon->OpenRequest((1 == pParam->CallMode) ? CHttpConnection::HTTP_VERB_GET : CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_EXISTING_CONNECT);


		if ( NULL == pHtFile )
		{
			//��������ʧ��
			session.Close();
			delete pHtCon;
			pHtCon = NULL;
			//�׳��쳣
			throw 5;
		}


		iTaskNo = 0x04;

		//�����Ҫǿ�ƽ�����������߳�
		if (pParam->NeedEndThread)
			goto END;

		//MessageBox(0,pParam->SendData,"debug",0);
		//modify

		pHtFile->SendRequestEx(pParam->SendData.GetLength());

		//MessageBox(0, "end SendRequestEx", "", 0);

		iTaskNo = 0x05;

		pHtFile->WriteString(pParam->SendData);

		iTaskNo = 0x06;
	
		pHtFile->EndRequest();
		


		


		//��ȡ������״̬����
		if (pParam->NeedEndThread)
			goto END;

		iTaskNo = 0x07;
		pParam->ServerResponseCode = 0x00;
		pHtFile->QueryInfoStatusCode(pParam->ServerResponseCode);

		//�������������������������κ�����
		if (!bIsHeart)
		{
			//��ȡ��������(ͳһ�����ַ�������)
			if (pParam->NeedEndThread)
				goto END;
			pParam->ServerResponseData.Empty();

			if (pParam->NeedEndThread)
				goto END;

			//�����������������2K
			DWORD dwReceiveSize = 2 * 1024;
			CString strSingleData;
			while (pHtFile->ReadString(strSingleData))
			{
				//�����Ҫǿ�ƽ�����������߳�
				if (pParam->NeedEndThread)
					goto END;

				pParam->ServerResponseData += strSingleData;
				::Sleep(5);

				//�������ݲ�������ָ��ֵ
				if (pParam->ServerResponseData.GetLength() > dwReceiveSize)
				{
					break;
				}
			}
			pParam->ServerResponseData.TrimRight();

			CString strTempData = pParam->ServerResponseData;

			pParam->ServerResponseData = StringConvertByCodePage(strTempData, CP_UTF8, CP_ACP);

#ifdef _DEBUG
			::OutputDebugString(pParam->ServerResponseData);
#endif
		}

		iTaskNo = 0x08;

#ifdef _DEBUG
		if (	  (HTTP_STATUS_OK != pParam->ServerResponseCode)
		          && (HTTP_STATUS_BAD_METHOD != pParam->ServerResponseCode)
		   )
		{
			//����������������
			session.Close();
			delete pHtCon;
			pHtCon = NULL;

			delete pHtFile;
			pHtFile = NULL;
			//�׳��쳣
			throw 7;
		}
#else
		if (HTTP_STATUS_OK != pParam->ServerResponseCode)
		{
			//����������������
			session.Close();
			delete pHtCon;
			pHtCon = NULL;

			delete pHtFile;
			pHtFile = NULL;
			//�׳��쳣
			throw 7;
		}
#endif

		iTaskNo = 0x09;

		//��Ƿ��سɹ�
		dwReturn = OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case 1:
			strErrorDesc.Format(_T("%s,�����ṹ��ָ��Ϊ��"), bIsHeart ? _T("�������") : _T("ҵ������"));
			break;
		case 2:
			strErrorDesc.Format(_T("%s,����URL[%s]�����쳣,%s"), bIsHeart ? _T("�������") : _T("ҵ������"), pParam->URL, GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 3:
			strErrorDesc.Format(_T("%s,�����쳣���޷��������������������"), bIsHeart ? _T("�������") : _T("ҵ������"));
			break;
		case 4:
			strErrorDesc.Format(_T("%s,�����������ӳ����쳣,%s"), bIsHeart ? _T("�������") : _T("ҵ������"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 5:
			strErrorDesc.Format(_T("%s,������������쳣,%s"), bIsHeart ? _T("�������") : _T("ҵ������"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 6:
			strErrorDesc.Format(_T("%s,�����������״̬�쳣,%s"), bIsHeart ? _T("�������") : _T("ҵ������"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 7:
			strErrorDesc.Format(_T("%s,��ȡ������״̬���������쳣,%s"), bIsHeart ? _T("�������") : _T("ҵ������"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 8:
			strErrorDesc.Format(_T("%s,��ȡ������״̬���������쳣,%s"), bIsHeart ? _T("�������") : _T("ҵ������"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		}
		dwReturn = iErrorCode;
	}
	catch (CInternetException* e)
	{
		DWORD dwErorCode = ::GetLastError();
		TCHAR info[1024] = {0};
		e->GetErrorMessage(info, 1024);
		strErrorDesc.Format(_T("�������쳣,��ϸ����:(%s,����� %d,�������:%d)%s"), bIsHeart ? _T("�������") : _T("ҵ������"), iTaskNo, dwErorCode, info);
		e->Delete();
		dwReturn = 9;

	}
	catch (...)
	{
		DWORD dwErrorCode = ::GetLastError();
	}
END:
	if (NULL != pParam)
	{
		pParam->ExecuteCode = dwReturn;
	}
	//�ر��ļ�
	if (NULL != pHtFile)
	{
		pHtFile->Close();
		//�ͷ���Դ
		delete pHtFile;
		pHtFile = NULL;
	}

	//�ر�����
	if (NULL != pHtCon)
	{
		pHtCon->Close();
		//�ͷ���Դ
		delete pHtCon;
		pHtCon = NULL;
	}
	//�رջỰ
	session.Close();

	strErrorDesc.TrimLeft();
	strErrorDesc.TrimRight();
	if (!strErrorDesc.IsEmpty())
	{
		static CString strLastError = _T("");
		if (strLastError != strErrorDesc)
		{
			strLastError = strErrorDesc;
			//������־
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[֧��ͨѶ�߳�]:���ݽ������̲�������,%s"), strErrorDesc), LOG_LEVEL_ERROR);
		}
	}
	//ͳ����ʱ
	pParam->ConsumeTime = ::GetTickCount() - dwStart;
	//����
	return dwReturn;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::CheckHeartState
//
// ���������������������
//
// ���������void
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��13��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::CheckHeartState()
{
	//��������
	WORD wSendDataType = JSON_DT_SEND_HEART;
	//�����������
	WORD wReturnCode = 0x00;
	//ת������Ҫ���͵����ݸ�ʽ����
	CString strSendData = _T("HeartCheck");

	CString strLog;
	//��ȡ��ǰʹ�õ�URL��ַ
	CString strURL;
	//��ȡ��ǰʹ�õĶ˿ں�
	INTERNET_PORT wPort = 80;
	//��ǰ���÷�ʽ
	int	iCallMode = 1;//Ĭ��GET��ʽ��ȡ����
	//�����ߵĴ�����
	HWND hWnd = NULL;
	//���̵߳ȴ�ʱ��,��λ:����
	DWORD dwWaitThreadWorkTimeout = 5000;

	//���������ص�����
	CString strResponseResult;
	//���������ص�״̬��
	DWORD dwResponseStateCode = 0x00;
	//����������ʱ��
	DWORD dwConsumeTime = 0x00;
	//��ʼ��������
	try
	{
		//�������
		HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
		DWORD	dwThreadID   = 0;
		tagHttpParam sParam;

		//�������ṹ��
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout = 4000;
		sParam.SessionControlTimeout = 5000;

		sParam.CallMode	= iCallMode;
		sParam.Port		= m_wHeartPort;
		sParam.URL		= m_strHeartURL;
		//ת����UTF-8��ʽ
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		//�������ݣ���ȡ����
		// if( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
		// 	throw -1;
		// //�ȴ���ʱ
		
		// if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,dwWaitThreadWorkTimeout))
		// {
		// 	//��ֹ�߳�
		// 	sParam.NeedEndThread = TRUE;
		// 	//��Ĭ300����
		// 	Sleep(300);
		// 	dwResponseStateCode	= sParam.ServerResponseCode;
		// }
		// else
		// {
		// 	//��ȡ���
		// 	dwConsumeTime		= sParam.ConsumeTime;
		// 	dwResponseStateCode	= sParam.ServerResponseCode;
		// 	strResponseResult	= sParam.ServerResponseData;
		// }
		// //MessageBox(0,"wait over","",0);
		// GetGlobalApp()->RecordLog("wait over");
		// //�رվ��
		// ::CloseHandle(hThreadHandle);
		
		

		DWORD dwTime1 = ::GetTickCount();
		SendData(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

		//�жϷ������������
		if (OP_SUCCESS != sParam.ExecuteCode)
			throw - 1;

		//���÷��������״̬Ϊ����
		m_dwHeratFailForCenterReflectWeb = 0;
		m_dwHeratFailForOperationServer = 0;

	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1://�޷�����������Ϣ���߳�
			++m_dwHeratFailForCenterReflectWeb;
			break;
		case -2://HTTP����������ʧ��
			++m_dwHeratFailForOperationServer;
			break;
		}
		wReturnCode = abs(iErrorCode);
	}
	//���÷�����״̬ -- �շ�ר������������쳣
	GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= (m_dwHeratFailForCenterReflectWeb >= m_dwHeratFailWarningTimes) ? TRUE : FALSE;
	//���÷�����״̬ -- ��Ӫƽ̨�������쳣
	GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= (m_dwHeratFailForOperationServer >= m_dwHeratFailWarningTimes) ? TRUE : FALSE;
	//��¼��־
	if (GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist || GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist )
	{
		static time_t tLastCheckHeartLogTime = 0x00;
		if ( (time(NULL) - tLastCheckHeartLogTime) > 30)
		{
			strLog.Format(_T("[֧��ͨѶ�߳�]:�������,�����������쳣"));
			if (strLog != m_strLastErrorDesc)
			{
				m_strLastErrorDesc = strLog;
				GetGlobalApp()->RecordLog(m_strLastErrorDesc);
			}

			tLastCheckHeartLogTime = time(NULL);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::StartProcessThread
//
// ��������������Http�����߳�
//
// ���������IN const LPVOID lpParam	--	�̹߳��������Ĳ����ṹָ��
// ���������HANDLE	--	�߳̾��
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��14��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE CThreadHttp::StartProcessThread(IN const LPVOID lpParam)
{
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
	tagHttpParam* pParam = (tagHttpParam*)lpParam;
	if (NULL != pParam)
	{
		hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &SendData, lpParam, 0, NULL);
	}
	return hThreadHandle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::StringConvertByCodePage
//
// �������������ڸ������ַ������ݣ�ͨ��ָ���ı����ʽ���л���ת��
//
// ���������IN const CString& strConvertData	--	��Ҫת�����ַ���
//         ��IN const int& iSourceCodepage		--	��Ҫת�����ַ����ı����ʽ
//         ��IN const int& iTargetCodepage		--	ת���������ַ����ı����ʽ
// ���������CString -- ����ת�����ַ�������(���ɸ�ʽ:iTargetCodepage)
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CThreadHttp::StringConvertByCodePage(IN const CString& strConvertData, IN const int& iSourceCodepage, IN const int& iTargetCodepage)
{
	CString strReturn;
	CString strRealConvertData(strConvertData);
	int len = strRealConvertData.GetLength();
	int unicodeLen = MultiByteToWideChar(iSourceCodepage, 0, strRealConvertData, -1, NULL, 0);

	wchar_t *pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	if (NULL != pUnicode)
	{
		memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
		MultiByteToWideChar(iSourceCodepage, 0, strConvertData, -1, (LPWSTR)pUnicode, unicodeLen);

		BYTE *pTargetData = NULL ;
		int targetLen = WideCharToMultiByte(iTargetCodepage, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);

		pTargetData = new BYTE[targetLen + 1];
		if (NULL != pTargetData)
		{
			memset(pTargetData, 0, targetLen + 1);

			WideCharToMultiByte(iTargetCodepage, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, targetLen, NULL, NULL);

			strReturn.Format(_T("%s"), pTargetData);

			delete []pTargetData;
		}
		delete []pUnicode;
	}

	return strReturn ;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::GetSendDataTypeDesc
//
// ������������ȡ����������������
//
// ���������IN const int& iDataType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadHttp::GetSendDataTypeDesc(IN const int& iDataType)
{
	CString strReturn;
	switch (iDataType)
	{
	case JSON_DT_SEND_DIBIT:
		strReturn.Format(_T("�ۿ�����"));
		break;
	case JSON_DT_SEND_DIBIT_CHECK:
		strReturn.Format(_T("�۷Ѳ�ѯ"));
		break;
	case JSON_DT_SEND_DIBIT_CANCEL:
		strReturn.Format(_T("��������"));
		break;
	case JSON_DT_SEND_NFC_SIGN_IN:
		strReturn.Format(_T("NFCǩ��"));
		break;
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadHttp::GetServerResponseCodeDesc
//
// ������������ȡ������������������
//
// ���������IN const int& iResponseType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadHttp::GetServerResponseCodeDesc(IN const int& iResponseType)
{
	CString strReturn;
	switch (iResponseType)
	{
	case HTTP_RC_DATA_LEGAL:
		strReturn.Format(_T("�������ݷ��Ϲ���"));
		break;
	case HTTP_RC_DATA_VALID:
		strReturn.Format(_T("��������"));
		break;
	case HTTP_RC_BLACKLIST:
		strReturn.Format(_T("��ά���֧���û�����΢����������"));
		break;
	case HTTP_RC_NO_REGISTER:
		strReturn.Format(_T("��ά���֧���û�δע��"));
		break;
	case HTTP_RC_NO_ENTRY_BY_ID:
		strReturn.Format(_T("��ά���֧���û�����Ч�����Ϣ"));
		break;
	case HTTP_RC_ID_IS_RUNNING:
		strReturn.Format(_T("��ά���֧���û��Ѿ�·�������"));
		break;
	case HTTP_RC_DEBIT_NEED_PASSWORD:
		strReturn.Format(_T("��Ҫ��ά���֧���û���������"));
		break;
	case HTTP_RC_DEBIT_CODE_TIMEOUT:
		strReturn.Format(_T("��ά����Ч�ڳ�ʱ"));
		break;
	case HTTP_RC_DEBIT_SRC_ILLGAL:
		strReturn.Format(_T("��ά����Դ�Ƿ�"));
		break;
	case HTTP_RC_DEBIT_INSUFFICIENT_BALANCE:
		strReturn.Format(_T("����"));
		break;
	case HTTP_RC_DEBIT_FAIL:
		strReturn.Format(_T("�۷�ʧ��"));
		break;
	case HTTP_RC_DEBIT_SUCCESS:
		strReturn.Format(_T("�۷ѳɹ�"));
		break;
	case HTTP_RC_DEBIT_NO_VOUCHER:
		strReturn.Format(_T("Э���ʽ����"));
		break;
	case HTTP_RC_PROTOCOL_ERROR:
		strReturn.Format(_T("���������Ľ���ƾ֤�����Ķ�����Ϣ��ƽ̨û���ҵ�"));
		break;
	case HTTP_RC_PAYPLATROMR_ERROR:
		strReturn.Format(_T("֧��ƽ̨����"));
		break;
	case HTTP_RC_NO_CANCEL_ORDER:
		strReturn.Format(_T("�����ѹ���Ч�ڣ���ֹ����"));
		break;
	case HTTP_RC_NO_SERVICES:
		strReturn.Format(_T("ƽ̨��ͣ����"));
		break;
	default:
		strReturn.Format(_T("ƽ̨�����޷��������ڲ��쳣"));
		break;
	}
	return strReturn;
}

