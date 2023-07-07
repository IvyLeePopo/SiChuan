// ThreadNFCSignIn.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadNFCSignIn.h"

// CThreadNFCSignIn

IMPLEMENT_DYNCREATE(CThreadNFCSignIn, CThreadHttp)

CThreadNFCSignIn::CThreadNFCSignIn()
{
}

CThreadNFCSignIn::~CThreadNFCSignIn()
{
}

BOOL CThreadNFCSignIn::InitInstance()
{
	fnReadSpecailConfig();
	return TRUE;
}

int CThreadNFCSignIn::ExitInstance()
{
	//������Դ
	fnReleaseResource();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadNFCSignIn, CThreadHttp)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_ISSUE_DATA,OnMsgTransportSignInData)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_POS,OnMsgRevSignDataFromPOS)
END_MESSAGE_MAP()


// CThreadNFCSignIn message handlers
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadNFCSignIn::OnIdle
//     
// �������������д�����
//     
// ���������LONG lCount
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadNFCSignIn::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		//�Ա��߳��׳��˳���Ϣ
		::PostQuitMessage(0);

		return FALSE;
	}

	//��������ǽ�ģ��
	if(::GetGlobalApp()->m_bNFCIsUse)
	{
		BOOL bNeedReSignIn = FALSE;
		CString strReSignText;
		//��ǩ����1������ǽ�ģ�鵱ǰ����ǩ��ʧ��״̬�����ҷǽ�ģ�鲻��ǩ�������У������ǩ������
		if(!bNeedReSignIn)
		{
			if(    !GetGlobalApp()->m_bNFCSignInSuccess
				&& (UMS_SIGNING != GetGlobalApp()->GetUnipayWorkFlowState()))
			{
				static time_t tLastCheckUnipaySignTime = time(NULL);
				if( (time(NULL)-tLastCheckUnipaySignTime) >  GetGlobalApp()->m_iNFCCheckSignSpanTime)
				{
					bNeedReSignIn = TRUE;
					strReSignText.Format(_T("�ϴ���ǩʧ�ܻ��ʼ��ǩ��"));
					tLastCheckUnipaySignTime = time(NULL);
				}
			}
		}

		//��ǩ����2������ǽ�֧��ʧ��(����3��),�����Ѿ�ǩ���ɹ������������ǩ��
		if(    !bNeedReSignIn
			&& (3 == GetGlobalApp()->m_iNFCPayFailCount)
			)
		{
			bNeedReSignIn = TRUE;
			strReSignText.Format(_T("����ʧ����ǩ(����3��)"));
		}

		//��ǩ����3��ÿ��̶�ʱ�����ǩ��
		if(!bNeedReSignIn)
		{
			CTime cCurrentTime = CTime::GetCurrentTime();
			if(    (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iNFCDefaultStartReSignHour ) 
				&& (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iNFCDefaultEndReSignHour))
			{
				if(!GetGlobalApp()->m_bNFCAutoSignIsExecute)
				{
					time_t tLastCheckAutoSignTime = time(NULL);
					if(    ((time(NULL) - tLastCheckAutoSignTime) > 10)
						&& (UMS_SIGNING != GetGlobalApp()->GetUnipayWorkFlowState())
						)
					{
						bNeedReSignIn = TRUE;
						strReSignText.Format(_T("ÿ�춨ʱ��ǩ"));
					}
				}
				else
				{
					bNeedReSignIn = FALSE;
				}
			}
		}

		//ִ����ǩ�����������жϣ�
		if(bNeedReSignIn)
		{
			static time_t tLastCheckSignInOpTime = 0;
			if((time(NULL) - tLastCheckSignInOpTime)>30)
			{
				CString strLog;
				strLog.Format(_T("[NFCǩ���߳�]:ϵͳ�ж�NFC�ǽ�ģ����Ҫ����ǩ������(%s)��׼������ǩ������"),strReSignText);
				GetGlobalApp()->RecordLog(strLog);
				//֪ͨҵ������̣߳���Ҫ����ǩ������
				::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_NFC_START_SIGN,(WPARAM)0,(LPARAM)0);
				::GetGlobalApp()->m_tNFCStartSignInTime = time(NULL);
				//����ʱ��
				tLastCheckSignInOpTime = time(NULL);
			}
		}
	}

	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadNFCSignIn::OnMsgTransportSignInData
//     
// ��������������ǩ�����ݸ��ƶ�
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
void CThreadNFCSignIn::OnMsgTransportSignInData(WPARAM wParam,LPARAM lParam)
{
	//���ù���״̬
	m_bWorking = TRUE;

	CString strLog;
	DWORD	dwServerReturnResponseCode = 0x00;

	//��������
	WORD wSendDataType				= LOWORD(lParam);
	//�Ƿ���Ҫ����������
	BYTE byIsNeedResponseCaller		= LOBYTE(HIWORD(lParam));
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
		if(!DataConvertFromJson2String(wSendDataType,pDVJsonReceive,strURL,wPort,strSendData,hWnd,iCallMode,&pJsonIF,0))
			throw -2;
		strLog.Format(_T("[NFCǩ���߳�]:�ѽ��յ�����[����:%s,%s]��׼����������"),GetSendDataTypeDesc(wSendDataType),pDVJsonReceive->PayIdentifier);
		GetGlobalApp()->RecordLog(strLog);
		strLog.Empty();

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
			//�׳��쳣������ͨѶ��ʱ
			strResponseResult.Format(_T("����ʱ����Ӧ"));
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
//		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[NFCǩ���߳�]:�������ݵ�������(����[����:%s])����ϸ����:%s;��ȡ��������:%s"),GetSendDataTypeDesc(wSendDataType),strSendData,strResponseResult));
#endif
		//�жϷ������������
		dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;

		//֪ͨ�����߽��
		CString strNeedNotifyCaller;
		if(0 != m_sTranstParam.CallerThreadID)
		{
			strNeedNotifyCaller.Format(_T(",��֪ͨ������"));
			::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP,(WPARAM)pDVJsonBase,MAKELONG(wSendDataType,HTTP_RC_DATA_LEGAL ));
		}
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[NFCǩ���߳�]:�ѽ��յ�����������[��������:%s]��״̬:����ɹ�%s"),GetSendDataTypeDesc(wSendDataType),strNeedNotifyCaller));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://�������ָ��Ϊ��
			strLog.Format(_T("[NFCǩ���߳�]:��������������ʧ��,�������ָ��Ϊ��,����ҵ������̴߳���Ϊ:0x01(�ڲ�����)"));
			break;
		case -2://���ݸ�ʽת�������쳣
			strLog.Format(_T("[NFCǩ���߳�]:��������������ʧ��,���ݸ�ʽת�������쳣,����ҵ������̴߳���Ϊ:0x02(�ڲ�����)"));
			break;
		case -3://�޷��������ݷ����߳�
			strLog.Format(_T("[NFCǩ���߳�]:��������������ʧ��,�޷��������ݷ����߳�,����ҵ������̴߳���Ϊ:0x03(�ڲ�����)"));
			break;
		case -4://����������ʾʧ��
			strLog.Format(_T("[NFCǩ���߳�]:��������������ʧ��,����ҵ������̴߳���Ϊ:0x04(�ڲ�����)�����������쳣���޷�����"));
			break;
		case -0xF0://�۷ѻ��ѯ����������ǳɹ��������������룬�ж��ڲ�����
			strLog.Format(_T("[NFCǩ���߳�]:ͨѶ�쳣,����ҵ������̴߳���Ϊ:0x05(�ڲ�����)����ϸ����:%s"),strResponseResult);
			break;
		}
		wReturnCode = abs(iErrorCode);
	
		//�������ý��
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP,(WPARAM)0,MAKELONG(wSendDataType,wReturnCode));
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

	//���ù���״̬
	m_bWorking = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadNFCSignIn::OnMsgRevSignDataFromPOS
//     
// �������������մ�NFCģ���ϴ���ǩ������
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_POS
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::OnMsgRevSignDataFromPOS(WPARAM wParam,LPARAM lParam)
{
	CString strSignInData;
	try
	{
		CString* pstrData = (CString*)wParam;
		if(NULL == pstrData)
			throw -1;
		strSignInData.Format(_T("%s"),*pstrData);
		ContructNFCSignInData(strSignInData);

		delete pstrData;
		pstrData = NULL;
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
// �������ƣ�CThreadNFCSignIn::fnReleaseResource
//     
// ������������������������Դ
//     
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::fnReleaseResource()
{
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
const BOOL CThreadNFCSignIn::DataConvertFromJson2String(IN const WORD& wSendDataType,IN OUT CDVJsonBase* pDVJsonReceive,IN OUT CString& strURL,IN OUT INTERNET_PORT& wPort,IN OUT CString& strSendData,IN OUT HWND& hWnd,IN OUT int& iCallMode,IN OUT CJsonOperationIF** pJsonIF, int nSN )
{
	BOOL bReturn = TRUE;

	try
	{
		switch(wSendDataType)
		{
		case JSON_DT_SEND_NFC_SIGN_IN:		//NFCǩ��
			{
				//�����Ӧ��������ָ��
				CDVJsonNFCSignIn* pDVJsonNFCSignIn = (CDVJsonNFCSignIn* )pDVJsonReceive;
				if(NULL == pDVJsonNFCSignIn)
				{
					throw -1;
				}
				//��õ����ߴ�����
				hWnd = pDVJsonNFCSignIn->m_hCallerWnd;

				//�������ݲ�����
				*pJsonIF = new CJsonNFCOperate();
				if(NULL == *pJsonIF)
				{
					throw -2;
				}
				//ת����JSON���ݸ�ʽ
				strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonNFCSignIn);
				//��÷����������Ϣ
				strURL = m_strHttpNFCSignInURL;
				wPort  = m_wHttpNFCSignInPort;
				iCallMode = 2;
			}
			break;
		default:
			throw -3;
		}
	}
	catch(int& iErrorCode)
	{
		bReturn = FALSE;

		switch(iErrorCode)
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

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadNFCSignIn::fnReadSpecailConfig
//     
// ������������ȡ������ص�������Ϣ(��ָ�������ļ�)
//     
// ����������޲���
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::fnReadSpecailConfig()
{
	TCHAR szTemp[512]={0};
	
	TCHAR szIP[32]={0};
	WORD  wPort = 80;
	BOOL  bAddPort = FALSE;

	//��ȡ����������Ϣ
	CString strCommConfigFile;
	strCommConfigFile.Format(_T("%s\\%s"),m_sTranstParam.PathCommDir,FILE_NAME_APP_CONFIG);
	//1.0.3.4�汾�������������Ի���IP����60.205.140.198����Ϊ��60.205.140.198
	::GetPrivateProfileString(_T("ReflectWeb"),_T("IP"),_T("60.205.140.198"),szIP,_countof(szIP),strCommConfigFile);
	wPort = (WORD)::GetPrivateProfileInt(_T("ReflectWeb"),_T("Port"),8081,strCommConfigFile);
	bAddPort = (BOOL)::GetPrivateProfileInt(_T("ReflectWeb"),_T("AddPort"),0,strCommConfigFile);
	//���̵߳ȴ�ʱ��,��λ:���룬Ĭ�ϳ�ʱʱ��Ϊ 8 ��
	m_dwWaitThreadWorkTimeout = (DWORD)::GetPrivateProfileInt(_T("ReflectWeb"),_T("WaitThreadWorkTimeout"),8000,strCommConfigFile);

	if(::PathFileExists(m_strPathLocalConfigFile))
	{
		if(bAddPort)
		{
			//ǩ��URL
			m_wHttpNFCSignInPort		= wPort;
			m_strHttpNFCSignInURL.Format(_T("http://%s:%d/signIn"),szIP,m_wHttpNFCSignInPort);
		}
		else
		{
			//ǩ��URL
			m_wHttpNFCSignInPort		= wPort;
			m_strHttpNFCSignInURL.Format(_T("http://%s/signIn"),szIP);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadNFCSignIn::ContructNFCSignInData
//     
// �������������첢����ǩ�����ݸ��ƶ�
//     
// ���������IN const CString& strSignData
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::ContructNFCSignInData(IN const CString& strSignData)
{
	try
	{
		//���ۿ���Ϣ�ṹ��
		CDVJsonNFCSignIn* pDVJsonNFCSignIn = new CDVJsonNFCSignIn();
		if(NULL == pDVJsonNFCSignIn)
			throw -1;
		CString strPayId = GetGlobalApp()->GetGuidString(1);
		CString strTemp;

		//��ʶ�Ƿ��������
		pDVJsonNFCSignIn->TestingData			= ::GetGlobalApp()->m_bIsTestData;
		//��ȡ����汾��
		pDVJsonNFCSignIn->NetPayDllVersion		= ::GetGlobalApp()->m_strNetPayDllVersion;
		pDVJsonNFCSignIn->DeviceDllVersion		= ::GetGlobalApp()->m_strDeviceDllVersion;
		pDVJsonNFCSignIn->TWProductSerialNo		= ::GetGlobalApp()->m_strTWProductSerialNo;

		//��䱨��ͷ��Ϣ
		pDVJsonNFCSignIn->Version				= _T("1.5");
		pDVJsonNFCSignIn->PackageType			= JSON_DT_SEND_NFC_SIGN_IN;
		pDVJsonNFCSignIn->Token					= _T("");
		pDVJsonNFCSignIn->ProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonNFCSignIn->SubmitCount			= 0x01;
		pDVJsonNFCSignIn->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonNFCSignIn->m_hCallerWnd			= NULL;
		
		GetGlobalApp()->HexToBase64(GetGlobalApp()->m_strNFCTerminal,pDVJsonNFCSignIn->NFCTerminal);
		GetGlobalApp()->HexToBase64(GetGlobalApp()->m_strNFCSerialNo,pDVJsonNFCSignIn->NFCSerialNo);
		GetGlobalApp()->HexToBase64(strSignData,pDVJsonNFCSignIn->NFCSignData);

		//����ǩ������
		PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pDVJsonNFCSignIn,(LPARAM)MAKELONG(JSON_DT_SEND_NFC_SIGN_IN,MAKEWORD(0x01,0x00)));
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