/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
*
* �ļ����ƣ�ThreadManage.cpp
* ժ    Ҫ��ҵ���̨�����̣߳�ʵ���ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��7��24��
*/
// ThreadManage.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadManage.h"
#include "FileFindExt.h"
#include "JsonHelper.h"
#include "TypeHelper.h"
// CThreadManage

IMPLEMENT_DYNCREATE(CThreadManage, CThreadBase)

CThreadManage::CThreadManage()
	: m_pThreadDevice(NULL)
	, m_bAccountInfoIsReady(FALSE)
	, m_bDeviceLoadState(FALSE)
	, m_nAccuracy(0)
	, m_nTimerIDForScanCode(0)
	, m_nTimerIDForDebitMoney(0)
	, m_nTimerIDForCheckDebitState(0)
	, m_dwCheckDebitSpanTime(0)
	, m_dwCheckDebitAccumulateTime(0)
	, m_dwInputPasswordDelayTime(30 * 1000)
	, m_nTimerIDForDebitCancel(0)
{
	::InitializeCriticalSection(&m_idleMutex);
}

CThreadManage::~CThreadManage()
{
}

BOOL CThreadManage::InitInstance()
{
	StartWorkThreads(m_sTranstParam);
	return TRUE;
}

int CThreadManage::ExitInstance()
{
	//�رռ�ʱ��
	if (0 != m_nTimerIDForScanCode)
	{
		timeKillEvent(m_nTimerIDForScanCode);
	}
	if (0 != m_nTimerIDForDebitMoney)
	{
		timeKillEvent(m_nTimerIDForDebitMoney);
	}
	if (0 != m_nTimerIDForCheckDebitState)
	{
		timeKillEvent(m_nTimerIDForCheckDebitState);
	}
	if (0 != m_nTimerIDForDebitCancel)
	{
		timeKillEvent(m_nTimerIDForDebitCancel);
	}
	//�����С��ʱ������
	timeEndPeriod(m_nAccuracy);

	//add 2017-08 �������δ��룬��Ϊ�˱����ڹ����߳��˳���ʱ����Դ����û�б��ͷ�
	//��Ȼ�����еĴ�����OnIdle�����������Դ�ͷţ�����Onidle����Ķ�Ӧλ�ò�һ����ִ�С�
	this->fnReleaseResource();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadManage, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY, OnMsgReceiveNorify)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_START_DEBIT_FLOW, OnMsgManageStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_DEBIT_CANCEL, OnMsgDebitCancel)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OnIdle
//
// �����������߳̿��д�����
//
// ���������LONG lCount
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadManage::OnIdle(LONG lCount)
{
	 CScopeLock tmpIdle(&m_idleMutex);
	if (m_bExit)
	{
		//������Դ
		fnReleaseResource();
		//�Ա��߳��׳��˳���Ϣ
		::PostQuitMessage(0);
		return FALSE;
	}

	//��������������(30����һ��)
	static time_t tLastCheckDeviceLoadState = time(NULL);
	if ( (time(NULL) - tLastCheckDeviceLoadState) > 30)
	{
		if (NULL != m_pThreadDevice)
		{
			m_bDeviceLoadState = m_pThreadDevice->GetDeviceInitState();
		}

		tLastCheckDeviceLoadState = time(NULL);
	}

	CString strLog;
	//�̴߳����־��¼���
	static time_t tLastManageLogTime = 0;
	if ( (time(NULL) - tLastManageLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[ҵ������߳�],�̴߳����־���"));
		GetGlobalApp()->RecordLog(strLog);
		//������ʱ��
		tLastManageLogTime = time(NULL);
	}

	//����Ƿ���Ҫɾ����־(�޶�ʱ���ִ�иò���)
	CTime cCurrentTime = CTime::GetCurrentTime();
	if ( (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iDeleteLogStartHour ) && (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iDeleteLogEndHour) )
	{
		//ÿ��ָ��ʱ�����һ���Ƿ���Ҫɾ����־
		static time_t tLastDeleteLogTime = 0;
		if ( (time(NULL) - tLastDeleteLogTime) > GetGlobalApp()->m_iDeleteLogSpanTime)
		{
			//������ɾ����־
			CheckAndDeleteLogFile();
			//������ʱ��
			tLastDeleteLogTime = time(NULL);
		}
	}
	TransmitComDataFeedback();
	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}




////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OnMsgReceiveNorify
//
// ������������ȡ���������Ϣ
//
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��FFFF
// ��дʱ�䣺2017��3��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_NOTIFY
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgReceiveNorify(WPARAM wParam, LPARAM lParam)
{
	BYTE byCmdType		= LOBYTE(LOWORD(wParam));
	BYTE byExcuteCode	= HIBYTE(LOWORD(wParam));
	CString* pStrInfo	= NULL;
	WORD wResult = 0x00;

	BYTE byFunCode		= RCC_NONE;
	BYTE byRCCState		= RCC_INNER_ERROR;

	CString strLog;


	try
	{
		switch (byCmdType)
		{
		case RDM_SET_MMI_INFO://��ʾ����
		case RDM_SET_MMI_BYTE:
		case RDM_SET_MMI_AD:
			break;
		case RDM_DEBITE_START://�ۿ����
		{
			//�رտۿʱ��ʱ��
			StopTimer(TIMER_ID_DEBIT_MONEY);
			//���������λ����֪ͨ����
			byFunCode	= RCC_FUN_DEBIT;

			int iThrowError = 0x00;
			//�жϸ��ӻ��������Ƿ��쳣
			pStrInfo = (CString*)lParam;
			if (NULL == pStrInfo)
			{
				iThrowError = -1;
			}

			if (iThrowError >= 0)
			{
				if ((*pStrInfo).IsEmpty())
				{
					iThrowError = -2;
				}
			}

			byRCCState = byExcuteCode;


			if (0 != iThrowError)
			{
				//������ǰ����(�쳣����)
//					GetGlobalApp()->RecordLog("1231232131237129837129312739218");
				if (byExcuteCode == OP_CANCLE)
				{
					FinishTrade(TRUE);//add 2018-01 ������ȡ����������������
					return ;
				}
				else
				{
					FinishTrade(FALSE);
					//�׳��쳣
					throw iThrowError;
				}

			}

			//�����ۿ���
			CJsonParamOperate jsonParamOperation;
			CDVJsonParamDebitResult* jsonDebitResult = NULL;
			//		jsonDebitResult = (CDVJsonParamDebitResult*)jsonParamOperation.FormatResponseResult2Class(*pStrInfo,JSON_DT_PARAM_DEBIT_RESULT);

			//if(NULL != jsonDebitResult)
			{
				//���������λ����֪ͨ����
				byRCCState	= RCC_OK;//jsonDebitResult->ExecuteCode;
				//�жϿ۷�ִ�н��
				//if(0 == jsonDebitResult->ExecuteCode)
				{
					GetGlobalApp()->m_sCurrentDebitResult.Valid				= TRUE;

					GetGlobalApp()->m_sCurrentDebitResult.ResultType		= 0x00;
					GetGlobalApp()->m_sCurrentDebitResult.ExecuteCode		= RCC_OK;
					//GetGlobalApp()->m_sCurrentDebitResult.PayPlatformType	= jsonDebitResult->PayPlatformType;
					//GetGlobalApp()->m_sCurrentDebitResult.DebitTime			= jsonDebitResult->ServerDebitTime.GetTime();
					//GetGlobalApp()->m_sCurrentDebitResult.ConsumeTime		= ::GetTickCount() - ::GetGlobalApp()->m_dwTradeBeginOvertime;
					//_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc,_countof(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc),jsonDebitResult->ExecuteDesc);

					//_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode,_countof(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode),jsonDebitResult->TradeSecurityCode.GetBuffer(0));jsonDebitResult->TradeSecurityCode.ReleaseBuffer();
					//_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier,_countof(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier),jsonDebitResult->PayIdentifier.GetBuffer(0));jsonDebitResult->PayIdentifier.ReleaseBuffer();
					//_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder,_countof(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder),jsonDebitResult->DebitOrder.GetBuffer(0));jsonDebitResult->DebitOrder.ReleaseBuffer();
				}

				//������ǰ����(��������)
				FinishTrade(TRUE);

				//�����Դ
				delete jsonDebitResult;
				jsonDebitResult = NULL;
			}
		}
		break;
		case RDM_DEBITE_CANCEL://��������
		{
			//�رճ�����ʱ��ʱ��
			StopTimer(TIMER_ID_DEBIT_CANCEL);
			byFunCode	= RCC_FUN_CANCEL;
			byRCCState	= byExcuteCode;
			pStrInfo = (CString*)lParam;
			if (NULL == pStrInfo)
				throw - 1;

			if ((*pStrInfo).IsEmpty())
				throw - 2;
			//ֱ�Ӱѽ��֪ͨ��ȥ
			wResult = _ttoi(*pStrInfo);
			//���������λ����֪ͨ����
			byRCCState = wResult;

		}
		break;
		case RDM_DEVICE_INIT:	//��ʼ�����
			break;
		case RDM_HEART:			//������ѯ
			break;
		case RDM_READ_HW_VER:	//Ӳ����ͨ������ȡ
		case RDM_READ_CAN_ID:
		case RDM_READ_MAX_CODE:
		case RDM_READ_FM_VER:
		case RDM_READ_SCAN_TIME_LIMIT:
		case RDM_READ_SERIAL_NO:
		case RDM_READ_WELCOME_SC:
		case RDM_READ_NET0_IP_INFO:
		case RDM_READ_NET1_IP_INFO:
		case RDM_READ_DEVICE_TMP:
		case RDM_READ_DEVICE_TIME:
			break;
		case RDM_SET_HEART_TIME://Ӳ����ͨ��������
		case RDM_SET_WELCOME_SC:
		case RDM_SET_DEVICE_TIME:
		case RDM_SET_NET0_IP_INFO:
		case RDM_SET_NET1_IP_INFO:
			break;
		case RDM_PARAM_WRITE_MAX_CODE://�ؼ���������
		case RDM_PARAM_WRITE_CAN_ID:
		case RDM_PARAM_WRITE_SERIAL_NO:
		case RDM_PARAM_WRITE_SACN_TIME_LIMIT:
		case RDM_PARAM_TEST_SC:
		case RDM_PARAM_TT_DATA:
			break;
		default:
		{
			throw - 3;
		};
		break;
		}
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
		case -2:
			strLog.Format(_T("[ҵ������߳�],��Ӧ����Ӳ�������̻߳���������ɣ����صĸ���������������Ϊ�գ�"));
			break;
		case -3:
			strLog.Format(_T("[ҵ������߳�],��Ӧ����Ӳ�������̻߳���������ɣ�δ֪��ҵ������[%d]"), byCmdType);
			break;
		}
	}

	//������־
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog);
	}

	//������Դ
	if (NULL != pStrInfo)
	{
		delete pStrInfo;
		pStrInfo = NULL;
	}

	//�����������ǿ۷ѻ򳷵�������Ҫ֪ͨ��λ��
	if (   (RDM_DEBITE_START  == byCmdType)
	        || (RDM_DEBITE_CANCEL == byCmdType)
	   )
	{
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�],��Ӧ����Ӳ�������̻߳���������ɣ���ʽ�����(���ܴ���:%d,ִ��״��:%d)"), byFunCode, byRCCState));
		//֪ͨ������
		::GetGlobalApp()->NotifyCaller(byFunCode, byRCCState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OnMsgStartDebitFlow
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
// ��ע˵����WM_THREAD_MANAGE_START_DEBIT_FLOW
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgManageStartDebitFlow(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//��ǿۿ����̿�ʼ
		GetGlobalApp()->m_iDebitTaskNo = 0x01;

		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pThreadDevice)
			throw - 2;
		//�����һ�ε��û���Ϣ
		memset(&m_sAccountInfo, 0, sizeof(tagAccountInfo));
		//֪ͨӲ�������߳�
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_START_DEBIT_FLOW, (WPARAM)wParam, (LPARAM)lParam); //�����ݽ�����ʽ���͹�ȥ

		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�]:�ⲿ�������ÿۿ����(���ϲ���)�����ױ�ʶ:%s"), GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier));
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
// �������ƣ�CThreadManage::OnMsgDebitCancel
//
// ����������ִ�г�������
//
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��FF
// ��дʱ�䣺2017��1��16��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_MANAGE_DEBIT_CANCEL
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgDebitCancel(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString* pstrCancelInfo = (CString*)wParam;
	int iType = (int)lParam;
	try
	{
		if (NULL == pstrCancelInfo)
			throw - 1;

		//�رռ��ۿ�״̬��ʱ��
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//�ر��ϴεĶ�ʱ��
		StopTimer(TIMER_ID_DEBIT_CANCEL);

		//����������ʱ��ʱ��
		StartTimer(TIMER_ID_DEBIT_CANCEL, 10 * 1000);

		//���ÿۿ�ģʽ
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;
		//��ʶ���״������
		GetGlobalApp()->m_bIsTrading = FALSE;

		//������Դ����
		CString* pstrString = new CString();
		if (NULL == pstrString)
			throw - 2;
		pstrString->Format(_T("%s"), GetGlobalApp()->ConstructCancleInfo(iType, *pstrCancelInfo));
		//�ݽ�����
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL, (WPARAM)pstrString, (LPARAM)lParam);
		//��Ĭ800���������
		::Sleep(800);
		CString* pstrStringClear = new CString();
		if (NULL == pstrStringClear)
			throw - 2;


		pstrStringClear->Format(_T("%s"), GetGlobalApp()->GetClearScreenJson()); //"<?xml version=\"1.0\" encoding=\"GB2312\"?><DisplayContext><Version>1.0</Version><ProvinceID>99</ProvinceID><VerifyCode>0</VerifyCode><Data><Type>1</Type></Data></DisplayContext>"));
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO, (WPARAM)pstrStringClear, (LPARAM)iType);
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�]:�ⲿ�������ó������������ױ�ʶ:%s"), *pstrCancelInfo));
		//�ͷ���Դ
		delete pstrCancelInfo;
		pstrCancelInfo = NULL;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[ҵ������߳�]:�ⲿ�������ó����ӿ�,�������(����ʶ����)ָ��Ϊ��"));
			break;
		case -2:
			strLog.Format(_T("[ҵ������߳�]:�ڲ�������Դʧ��"));
		}
		//������־
		if (strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
		}
	}
}

/****************************************�ڲ���������********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadDevice::fnReleaseResource
//
// ������������������������Դ
//
// ����������޲���
// ���������voi
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::fnReleaseResource()
{
	CString strLog;
	try
	{
		DWORD dwSleepTime = 0;

		CString strLog;

		/**********************************************��������߳���Դ**********************************************/
		strLog.Format(_T("[ҵ������߳�],���ڿ�ʼ�˳������߳�..."));
		//��¼��־
		GetGlobalApp()->RecordLog(strLog, LOG_LEVEL_RUN, TRUE);

		//�߳��˳����
		DWORD dwThreadExitFlag	= 0x00;
		//�������ԵĴ���
		int iAllowTryCount		= 10;
		//�Ѿ����Դ���
		int iHaveTryCount		= 0;
		//ÿ�����Ե�ʱ����ʱ�䣨��λ�����룩
		DWORD	dwSpanTime		= 100;

		//ֹͣ����Ӳ�������߳�
		if (NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
		{
			strLog.Format(_T("[ҵ������߳�],��ʼ��ֹҵ���߳�..."));
			//���ø��߳��Ƴ����Ϊ��
			m_pThreadDevice->m_bExit = TRUE;
			//��Ĭ50����
			::Sleep(50);

			//ÿ�εȴ�1000���룬���Դ���������10��
			GetGlobalApp()->RecordLog(_T("[ҵ������߳�],�ȴ���ֹ����..."));
			while (OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadDevice->m_hThread, 1000))
			{
				Sleep(dwSpanTime);
				++iHaveTryCount;
				if (iHaveTryCount >= iAllowTryCount)
				{
					//��Ǹ��߳�δ�������˳�
					dwThreadExitFlag |= 0x01;
					//��ֹ����
					break;
				}
			}

			delete m_pThreadDevice;
			m_pThreadDevice = NULL;
			::Sleep(1);

			strLog.Format(_T("[ҵ������߳�],%s"), GetGlobalApp()->FmtStr(_T("���豸�����̡߳��Ѿ�ֹͣ(���Դ��� %d)..."), iHaveTryCount));
			//��¼��־
			GetGlobalApp()->RecordLog(strLog, LOG_LEVEL_RUN, TRUE);
		}
		else
		{
			strLog.Format(_T("[ҵ������߳�],�����̲߳����ڻ��Ѿ��˳�"));
		}
	

		iHaveTryCount = 0;
		//ֹͣNFCǩ��ͨѶ�߳�
		if (NULL != m_pSwitchThread)
		{
			HANDLE switchHandle = m_pSwitchThread->m_hThread;
			m_pSwitchThread->waitTillIdle();
			//���ø��߳��Ƴ����Ϊ��
			//m_pSwitchThread->m_bExit = TRUE;
			//��Ĭ50����
			::Sleep(50);

			//ÿ�εȴ�1000���룬���Դ���������10��
			while (OP_SUCCESS != GetGlobalApp()->fnCloseThread(switchHandle, 1000))
			{
				Sleep(dwSpanTime);
				++iHaveTryCount;
				if (iHaveTryCount >= iAllowTryCount)
				{
					//��Ǹ��߳�δ�������˳�
					dwThreadExitFlag |= 0x01;
					//��ֹ����
					break;
				}
			}

			strLog.Format(_T("[ҵ������߳�],%s"), GetGlobalApp()->FmtStr(_T("���汾�����̡߳��Ѿ�ֹͣ(���Դ��� %d)..."), iHaveTryCount));
			//��¼��־
			GetGlobalApp()->RecordLog(strLog, LOG_LEVEL_RUN, TRUE);
		}

		::Sleep(dwSleepTime);
	}
	catch (int& iErrorCode)
	{
	}
}


//add 2018-04 �˴������1.5���λ�ÿ���
void CheckAndDeleteLogFile_V2(void)
{
	CFileFindExt cFindFile;
	CString strDir;
	CString strExtName(_T("log"));
	strDir.Format(_T("%s"), ::GetGlobalApp()->m_sTranstParam.PathLogDir);
	try
	{
		if (!cFindFile.SearchFiles(strDir, strExtName))
			throw - 1;

		int iDeleteCount = 0;
		CTime cCurrentTime = CTime::GetCurrentTime();
		for (int i = 0; i < cFindFile.GetSize(); ++i)
		{
			CString strFilePath;
			CString strFileName = cFindFile.GetAt(i);
			strFilePath.Format(_T("%s\\%s"), strDir, strFileName);

			int iYear = 0;
			int iMon  = 0;
			int iDay  = 0;
			if ( 15 == strFileName.GetLength() )
			{
				iYear = _ttoi(strFileName.Mid(3, 4));
				iMon  = _ttoi(strFileName.Mid(7, 2));
				iDay  = _ttoi(strFileName.Mid(9, 2));
			}
			else
			{
				BOOL bNeedProcess = FALSE;
				CString strDate;
				if (strFileName.GetLength() > 15)
				{
					if (0 == strFileName.Left(4).CompareNoCase(_T("BASE")))
					{
						bNeedProcess = TRUE;
					}
					else if (0 == strFileName.Left(5).CompareNoCase(_T("UPDATE")))
					{
						bNeedProcess = TRUE;
					}
				}

				if (bNeedProcess)
				{
					strDate = strFileName.Right(8);

					iYear = _ttoi(strDate.Mid(0, 4));
					iMon  = _ttoi(strDate.Mid(4, 2));
					iDay  = _ttoi(strDate.Mid(6, 2));
				}
			}

			//add 2018-04 zhuyabing
			struct stat statbuf;
			if (stat(strFilePath, &statbuf) == 0)
			{
				struct tm time_tm =  *localtime(&statbuf.st_mtime);
				//CString info_str;
				//info_str.Format("%d-%d-%d",time_tm.tm_year,time_tm.tm_mon,time_tm.tm_mday);
				iYear = 1900 + time_tm.tm_year;
				iMon = 1 + time_tm.tm_mon;
				iDay = time_tm.tm_mday;
				CString file_str;
				file_str.Format("%s[]%d-%d-%d", strFilePath, iYear, iMon, iDay);
				//MessageBox(0,file_str,"",0);

			}


			if ( (0 != iYear) && (0 != iMon) && (0 != iDay) )
			{
				CTime cFileTime(iYear, iMon, iDay, 0, 0, 0);
				//��ȡ���ļ���ʱ��������ʱ����������
				CTimeSpan span = cCurrentTime - cFileTime ;

				int iSpanDay = span.GetDays();
				//iSpanDay = (cCurrentTime.GetTime() - cFileTime.GetTime())/(24*60*60);


				if (iSpanDay < GetGlobalApp()->m_iDeleteLogSaveDays)
					continue;

				if (!::PathFileExists(strFilePath))
					throw - 3;

				::DeleteFile(strFilePath);
				if (++iDeleteCount >= 20)
				{
					break;
				}
			}
		}
		//�������
		cFindFile.Close();
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
// �������ƣ�CThreadManage::CheckAndDeleteLogFile
//
// ����������������ɾ����־
//
// ���������void
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::CheckAndDeleteLogFile(void)
{
	return CheckAndDeleteLogFile_V2();
	CFileFindExt cFindFile;
	CString strDir;
	CString strExtName(_T("log"));
	strDir.Format(_T("%s"), ::GetGlobalApp()->m_sTranstParam.PathLogDir);
	try
	{
		if (!cFindFile.SearchFiles(strDir, strExtName))
			throw - 1;

		int iDeleteCount = 0;
		CTime cCurrentTime = CTime::GetCurrentTime();
		for (int i = 0; i < cFindFile.GetSize(); ++i)
		{
			CString strFilePath;
			CString strFileName = cFindFile.GetAt(i);
			strFilePath.Format(_T("%s\\%s"), strDir, strFileName);
			if ( strFileName.GetLength() != 15 )
				continue;
			int iYear = _ttoi(strFileName.Mid(3, 4));
			int iMon  = _ttoi(strFileName.Mid(7, 2));
			int iDay  = _ttoi(strFileName.Mid(9, 2));
			CTime cFileTime(iYear, iMon, iDay, 0, 0, 0);
			//��ȡ���ļ���ʱ��������ʱ����������
			CTimeSpan span = cCurrentTime - cFileTime ;
			int iSpanDay = span.GetDays();
			if (iSpanDay < GetGlobalApp()->m_iDeleteLogSaveDays)
				continue;
			if (!::PathFileExists(strFilePath))
				throw - 3;
			::DeleteFile(strFilePath);
			if (++iDeleteCount >= 10)
			{
				break;
			}
		}
		//�������
		cFindFile.Close();
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
// �������ƣ�CThreadDevice::StartWorkThreads
//
// �������������������߳�
//
// ���������IN const tagTranstParam& sTranstParam
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CThreadManage::StartWorkThreads(IN const tagTranstParam& sTranstParam)
{
	BOOL bReturn = FALSE;
	try
	{
		CString strConfigFilePath;
		tagTranstParam sTranstParam = {0};
		memcpy(&sTranstParam, &m_sTranstParam, sizeof(tagTranstParam));

		//����Ӳ�������߳�
		m_pThreadDevice = (CThreadDevice *)::AfxBeginThread(RUNTIME_CLASS(CThreadDevice), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		if (NULL == m_pThreadDevice)
			throw - 2;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_DEVICE_NOTIFY;
		strConfigFilePath.Format(_T("%s\\%s"), GetGlobalApp()->m_strWorkDir, FILE_NAME_DEVICE_CONFIG);
		m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadDevice->SetParam(&sTranstParam, NULL);
		m_pThreadDevice->m_bAutoDelete = FALSE;
		//�����߳����»���
		m_pThreadDevice->ResumeThread();
		//��Ĭ200����
		Sleep(200);

		//add 2018-05
		//�����汾�����߳�
		//m_pSwitchThread=NULL;
		
		m_pSwitchThread = (CSystemSwitch *)::AfxBeginThread(RUNTIME_CLASS(CSystemSwitch), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		if(NULL==m_pSwitchThread)
			throw -2;
		m_pSwitchThread->ResumeThread();
		Sleep(200);
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
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::StartTimer
//
// �������������ö�ʱ��
//
// ���������IN const UINT& nEventID
//         ��IN const DWORD& dwDelayTime
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::StartTimer(IN const UINT& nEventID, IN const DWORD& dwDelayTime)
{
	switch (nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://�ۿ����̶�ʱ��
	{
		if (0 == m_nTimerIDForDebitMoney)
		{
			TIMECAPS   tc;
			//���ú���timeGetDevCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������
			if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
			{
				//�ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ
				m_nAccuracy = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
				//����timeBeginPeriod�������ö�ʱ���ķֱ���
				timeBeginPeriod(m_nAccuracy);
				//���ÿۿ����̶�ʱ��
				m_nTimerIDForDebitMoney = timeSetEvent(
				                              dwDelayTime,						//��ʱ(MS)
				                              m_nAccuracy,						//�ӳٷֱ���
				                              (LPTIMECALLBACK)OneMilliSecondProc,	//�ص�����
				                              (DWORD)this,						//ʹ�ûص������л��ᴦ��������������
				                              TIME_ONESHOT);						//ֻ����һ��(TIME_PERIODIC:���ڵ���)
			}
		}
	}
	break;


	case TIMER_ID_DEBIT_CANCEL: //������ʱ��ʱ��
	{
		if (0 == m_nTimerIDForDebitCancel)
		{
			TIMECAPS tc;
			//���ú���timeGetDevCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������
			if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
			{
				//�ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ
				m_nAccuracy = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
				//����timeBeginPeriod�������ö�ʱ���ķֱ���
				timeBeginPeriod(m_nAccuracy);
				//���ÿۿ�״̬��ѯ��ʱ��
				m_nTimerIDForDebitCancel = timeSetEvent(
				                               dwDelayTime,						//��ʱ(MS)
				                               m_nAccuracy,						//�ӳٷֱ���
				                               (LPTIMECALLBACK)OneMilliSecondProc,	//�ص�����
				                               (DWORD)this,						//ʹ�ûص������л��ᴦ��������������
				                               TIME_ONESHOT);						//ֻ����һ��(TIME_PERIODIC:���ڵ���)
			}

		}
	}
	break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::StopTimer
//
// ����������ֹͣ��ʱ��
//
// ���������IN const UINT& nEventID
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��9��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::StopTimer(IN const UINT& nEventID)
{
	switch (nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://�ۿ����̶�ʱ��
	{
		if (0 != m_nTimerIDForDebitMoney)
		{
			timeKillEvent(m_nTimerIDForDebitMoney);
			m_nTimerIDForDebitMoney = 0;
		}
	}
	break;

	case TIMER_ID_DEBIT_CANCEL: //������ʱ��ʱ��
	{
		if (0 != m_nTimerIDForDebitCancel)
		{
			timeKillEvent(m_nTimerIDForDebitCancel);
			m_nTimerIDForDebitCancel = 0;
		}
	}
	break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::FinishTrade
//
// �������������׽����Ĵ���
//
// ���������IN const BOOL& bNormal -- �������쳣��������
// ���������CString �ôν��׵�ʶ����
// ��д��Ա��ROCY
// ��дʱ�䣺2016��9��11��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CThreadManage::FinishTrade(IN const BOOL& bNormal)
{
	CString strPayIdentifier = GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;
	//ֹͣ���մ���֧����ƽ̨����������
	GetGlobalApp()->m_bIsTrading		= FALSE;
	GetGlobalApp()->m_bCurrentDebitMode = FALSE;

	//�رտۿʱ��ʱ��
	StopTimer(TIMER_ID_DEBIT_MONEY);

	//��ʱ�������µĽ��׽�������Ҫ�Զ����𳷵�
	if (!bNormal)
	{
		//����ۿ������Դ
		GetGlobalApp()->m_sCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
		//���쳷������
		CString* strData = new CString();
		if (NULL != strData)
		{
			Json::Value root;
			Json::Value DebitCancelParamContext;
			Json::Value Data;
			// ��ʶ�ؼ�������
			Json::Value KeyItem;

			KeyItem["PayIdentifier"]			= Json::Value(strPayIdentifier);
			Data["TradeKeyItem"]				= KeyItem;

			DebitCancelParamContext["Data"]		= Data;
			DebitCancelParamContext["Version"]	= Json::Value(_T("1.0"));

			root["DebitCancelParamContext"]		= DebitCancelParamContext;

			//ת��Ϊ�ַ���
			Json::FastWriter fast_writer;
			strData->Format(_T("%s"), fast_writer.write(root).c_str());
			//���ͳ�������
			m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL, (WPARAM)strData, (LPARAM)2); //�����ݽ�����ʽ���͹�ȥ
		}
	}
	//��ʶ�ۿ����̽���
	if (GetGlobalApp()->m_bCurrentDebitMode)
	{
		GetGlobalApp()->m_iDebitTaskNo = 0x00;
	}

	return strPayIdentifier;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OneMilliSecondProc
//
// ������������ʱ����Ӧ����
//
// ���������UINT wTimerID	-- ��ʱ��ID
//         ��UINT msg		-- ��ʱ����Ϣ
//         ��DWORD dwUser	-- ������ָ��
//         ��DWORD dwl		-- ��ʱ������1
//         ��DWORD dw2		-- ��ʱ������2
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK CThreadManage::OneMilliSecondProc(UINT nTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	if (((CThreadManage*)dwUser)->m_nTimerIDForDebitMoney == nTimerID)	//������ָ����ʱʱ���ѵ�
	{
		//������Ϊ��ʱ���������
		CString strTradeID = ((CThreadManage*)dwUser)->FinishTrade(TRUE);
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�]:�ۿ������ʱ������Ԥ����ʱ,%d �룩���Զ����ó������������ױ�ʶ:%s"), ::GetGlobalApp()->m_sCurrentDebitInfo.OverTime / 1000, strTradeID));
		CString* pstrString = new CString();
		if (NULL == pstrString)
			throw - 2;
		pstrString->Format(_T("%s"), GetGlobalApp()->ConstructCancleInfo(0x00, strTradeID));
		//�ݽ�����
		((CThreadManage*)dwUser)->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL, (WPARAM)pstrString, (LPARAM)0x00);
		//֪ͨ�����߷��ؽ������ǰ����ʧ�� -- �ۿʱ
		BYTE byFunCode		= 0x01;
		BYTE byExecuteState	= 0x06;
		::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
	}
	else if (((CThreadManage*)dwUser)->m_nTimerIDForDebitCancel == nTimerID) //������ʱ
	{
		//�رճ�����ʱ��ʱ��
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_DEBIT_CANCEL);
		//֪ͨ�����߷��ؽ������ǰ����ʧ��
		BYTE byFunCode		= 0x02;
		BYTE byExecuteState	= 0x06;
		::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::SetAccountDataReadyState
//
// ���������������û������Ƿ�׼������
//
// ���������IN const BOOL& bState
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��9��14��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::SetAccountDataReadyState(IN const BOOL& bState)
{
	m_bAccountInfoIsReady = bState;
	if (!bState)
	{
		memset(&m_sAccountInfo, 0, sizeof(tagAccountInfo));
	}
}


bool CThreadManage::SendBreakDebit(const CString&payID)
{
	if (NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
	{
		return m_pThreadDevice->BreakDebit(payID);
	}
	return false;

}

bool CThreadManage::Init_Dev()
{
	if (NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
	{
		return m_pThreadDevice->Init_Dev();
	}
	return false;
}
bool CThreadManage::Destroy_Dev()
{
	if (NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
	{
		return m_pThreadDevice->Destroy_Dev();
	}
	return false;
}


void CThreadManage::ThreadQuit()
{	
	{
        if (this->m_bExit)
        {
            return ;
        }
        CScopeLock tmpIdle(&m_idleMutex);
		fnReleaseResource();
        this->m_bExit = true;
        
    }


}

void CThreadManage::TransmitComDataFeedback()
{
	if (NULL == m_pThreadDevice || m_pThreadDevice->m_bExit)
	{
		return ;
	}
	CString str;
	if(!m_pThreadDevice->TakeComFeedbackCommond(str))
	{
		return;
	};
	
	std::string jsonStr=std::string((LPCSTR)  str );
	Json::Value rootN;
	Json::Reader jsonReader;
	if(!jsonReader.parse(jsonStr,rootN))
	{
		return ;
	}
	
	std::string topStr,subStr,snStr,lenStr,verifyStr,dataStr;
	bool readSuccess =true;	
	readSuccess = readSuccess? ReadJsonStr(rootN,"TOPCMD",topStr):false;
	readSuccess = readSuccess? ReadJsonStr(rootN,"SUBCMD",subStr):false;
	readSuccess = readSuccess? ReadJsonStr(rootN,"SN",snStr):false;
	readSuccess = readSuccess? ReadJsonStr(rootN,"LEN",lenStr):false;
	readSuccess = readSuccess? ReadJsonStr(rootN,"VERIFY",verifyStr):false;
	readSuccess = readSuccess? ReadJsonStr(rootN,"DATA",dataStr):false;

	if(!readSuccess)
	{
		return ;
	}
	

	
	std::string cmdStr = "DF"+lenStr+snStr+topStr+subStr+dataStr+verifyStr+"FD";
	//theApp.RecordLog("send itc");
	//theApp.RecordLog(cmdStr.c_str());
	//MessageBox(0,cmdStr.c_str(),"abc",0);
	if(topStr=="BE")
	{
		
		size_t cmdSize = cmdStr.size();
		size_t byteSize = cmdSize/2;
		char* byteArr = new char[byteSize];
		memset(byteArr,0,byteSize);
		if(!Hex2Bin(cmdStr.c_str(),byteArr,cmdSize))
		{
			return;
		}
		
		//������Ϣ��
		GetGlobalApp()->send2Bus(24,byteArr,byteSize,KEY_BUSINESS);
		delete byteArr;
	}
}