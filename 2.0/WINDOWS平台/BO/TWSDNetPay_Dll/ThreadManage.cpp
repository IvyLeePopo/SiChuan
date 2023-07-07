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

// CThreadManage

IMPLEMENT_DYNCREATE(CThreadManage, CThreadBase)

CThreadManage::CThreadManage()
:m_pThreadDevice(NULL)
,m_bAccountInfoIsReady(FALSE)
,m_bDeviceLoadState(FALSE)
,m_nAccuracy(0)
,m_nTimerIDForScanCode(0)
,m_nTimerIDForDebitMoney(0)
,m_nTimerIDForCheckDebitState(0)
,m_dwCheckDebitSpanTime(0)
,m_dwCheckDebitAccumulateTime(0)
,m_dwInputPasswordDelayTime(30*1000)
,m_nTimerIDForDebitCancel(0)
{
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
	if(0 != m_nTimerIDForScanCode)
	{
		timeKillEvent(m_nTimerIDForScanCode);
	}
	if(0 != m_nTimerIDForDebitMoney)
	{
		timeKillEvent(m_nTimerIDForDebitMoney);
	}
	if(0 != m_nTimerIDForCheckDebitState)
	{
		timeKillEvent(m_nTimerIDForCheckDebitState);
	}
	if(0 != m_nTimerIDForDebitCancel)
	{
		timeKillEvent(m_nTimerIDForDebitCancel);
	}
	//�����С��ʱ������
	timeEndPeriod(m_nAccuracy);

	//������Դ
	fnReleaseResource();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadManage, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY,OnMsgReceiveNorify)

	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_GET_DATA,OnMsgStartGetAccountInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_STOP_GET_DATA,OnMsgStopGetAccountInfo)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_START_DEBIT_FLOW,OnMsgManageStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_DEBIT_CANCEL,OnMsgDebitCancel)
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
	if(m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}

	//��������������(10����һ��)
	static time_t tLastCheckDeviceLoadState = time(NULL);
	if( (time(NULL) - tLastCheckDeviceLoadState) > 10)
	{
		if(NULL != m_pThreadDevice)
		{
			m_bDeviceLoadState = m_pThreadDevice->GetDeviceInitState();
		}

		tLastCheckDeviceLoadState = time(NULL);
	}

	CString strLog;
	//�̴߳����־��¼���
	static time_t tLastManageLogTime = 0;
	if( (time(NULL) - tLastManageLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[ҵ������߳�],�̴߳����־���"));
		GetGlobalApp()->RecordLog(strLog);
		//������ʱ��
		tLastManageLogTime = time(NULL);
	}

	//����Ƿ���Ҫɾ����־(�޶�ʱ���ִ�иò���)
	CTime cCurrentTime = CTime::GetCurrentTime();
	if( (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iDeleteLogStartHour ) && (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iDeleteLogEndHour) )
	{
		//ÿ��ָ��ʱ�����һ���Ƿ���Ҫɾ����־
		static time_t tLastDeleteLogTime = 0;
		if( (time(NULL) - tLastDeleteLogTime) > GetGlobalApp()->m_iDeleteLogSpanTime)
		{
			//������ɾ����־
			CheckAndDeleteLogFile();
			//������ʱ��
			tLastDeleteLogTime = time(NULL);
		}
	}

	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OnMsgStartGetAccountInfo
//     
// ��������������Ҫ��ִ��������Ӳ����ȡ�û���Ϣ�Ĳ���
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_START_GET_DATA
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgStartGetAccountInfo(WPARAM wParam,LPARAM lParam)
{
	int iOverTime = (int)wParam;
	if(NULL != m_pThreadDevice)
	{
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_START_GET_DATA,(WPARAM)0,(LPARAM)0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OnMsgStartGetAccountInfo
//     
// ��������������Ҫ��ִ��ֹͣ��Ӳ����ȡ�û���Ϣ�Ĳ���
//     
// ���������WPARAM wParam
//         ��LPARAM lParam
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵������Ӧ��Ϣ WM_THREAD_DEVICE_STOP_GET_DATA
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgStopGetAccountInfo(WPARAM wParam,LPARAM lParam)
{
	if(NULL != m_pThreadDevice)
	{
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA,(WPARAM)0,(LPARAM)0);
		//�����ʱ��
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//ֹͣ���մ���֧����ƽ̨����������
		GetGlobalApp()->m_bIsTrading = FALSE;
		//����ۿ������Դ
		GetGlobalApp()->m_sCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::OnMsgGetAccountInfo
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
void CThreadManage::OnMsgReceiveNorify(WPARAM wParam,LPARAM lParam)
{
	WORD wSubType = LOBYTE(wParam);
	WORD wCmdType = HIBYTE(wParam);
	DWORD dwParam	= 0x00;
	DWORD dlParam  = 0x00;
	CString* pInfo = NULL;
	WORD dResult;
	CString strC0;
	CString strFailedCount;
	WORD wSuccCount;
	WORD wFailedCount;

	CJsonParamOperate jsonParamOperation;
	CDVJsonParamDebitResult* jsonDebitResult = NULL;

	BOOL bReady = FALSE;
	try
	{
		pInfo = (CString*)lParam;
		if(NULL == pInfo)
			throw -1;

		if((*pInfo).IsEmpty())
			throw -2;

		switch(wCmdType)
		{
		case 0:
			strC0.Format(_T("%s"),*pInfo); //����C0��� data:s/f (%d#%d)
			if(-1 == strC0.Find(_T("#")))
				throw -3;

			wSuccCount   = _ttoi(strC0.Left(strC0.Find(_T("#"))));
			strFailedCount = strC0.Right(strC0.GetLength() - strC0.Find(_T("#"))-1);
			wFailedCount = _ttoi(strFailedCount);

			dwParam = MAKELONG(MAKEWORD(0x00,0x00),MAKEWORD(0x00,0x00));
			dlParam = MAKELONG(wSuccCount, wFailedCount);

			bReady = TRUE;

			break;
		case 1:
			m_HKResult.strB1ResultData.Format(_T("%s"),*pInfo);//��������
			bReady = TRUE;
			break;
		case 2:
			m_HKResult.strB2ResultData.Format(_T("%s"),*pInfo);//���Ի���
			//ֱ�Ӱѽ��֪ͨ��ȥ
			dResult = _ttoi(m_HKResult.strB2ResultData);
			dwParam = MAKELONG(MAKEWORD(0x04,dResult),MAKEWORD(0x00,0x00));
			bReady = TRUE;

			break;

		case 3:

			StopTimer(TIMER_ID_DEBIT_MONEY); //�رտۿʱ��ʱ��
			m_HKResult.strB3ResultData.Format(_T("%s"),*pInfo);//�ۿ���

			//�����ۿ���
			/*CJsonParamOperate jsonParamOperation;
			CDVJsonParamDebitResult* jsonDebitResult = NULL;*/
			jsonDebitResult = (CDVJsonParamDebitResult*)jsonParamOperation.FormatResponseResult2Class(m_HKResult.strB3ResultData,JSON_DT_PARAM_DEBIT_RESULT);

			if(NULL != jsonDebitResult)
			{
				jsonDebitResult->ExecuteCode;
				jsonDebitResult->ResultType;

//				dwParam = MAKELONG(MAKEWORD(jsonDebitResult->ResultType, jsonDebitResult->ExecuteCode),MAKEWORD(0x00,0x00));
			
				dwParam = MAKELONG(MAKEWORD(1, jsonDebitResult->ExecuteCode),MAKEWORD(0x00,0x00));
			
				bReady = TRUE;

				delete jsonDebitResult;
				jsonDebitResult = NULL;
			}


			break;
		case 4:

			StopTimer(TIMER_ID_DEBIT_CANCEL);//�رճ�����ʱ��ʱ��
			m_HKResult.strB4ResultData.Format(_T("%s"),*pInfo);//�������	
			//ֱ�Ӱѽ��֪ͨ��ȥ
			dResult = _ttoi(m_HKResult.strB4ResultData);
			dwParam = MAKELONG(MAKEWORD(0x02,dResult),MAKEWORD(0x00,0x00));
			bReady = TRUE;

			break;
		case 5:
			m_HKResult.strB5ResultData.Format(_T("%s"),*pInfo);//��ʼ�����
			bReady = TRUE;

			break;
		case 6:
			m_HKResult.strB6ResultData.Format(_T("%s"),*pInfo);//Ӳ�����ý��
			bReady = TRUE;

			break;
		case 7:
			m_HKResult.strB7ResultData.Format(_T("%s"),*pInfo);//δ����
			bReady = TRUE;

			break;
		case 8:
			//ֱ�Ӱѽ��֪ͨ��ȥ
			dwParam = MAKELONG(MAKEWORD(0x08,0x00),_ttoi(*pInfo)); 
			bReady = TRUE;

			break;

		default:
			return;
		}

		if(bReady)
		{
			SetAccountDataReadyState(TRUE);
		}

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
		case -2:
			break;
		}
		//֪ͨ�������û����ݻ�ȡʧ��
		dwParam = MAKELONG(MAKEWORD(0x03,0x05),MAKEWORD(0x00,0x00));
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�],��Ӧ����Ӳ�������̻߳����������(��ȡ�û���Ϣ)����ʽ�����(���ܴ���:%d,ִ��״��:%d)"),0x03,0x05));
	}
	//������Դ
	if(NULL != pInfo)
	{
		delete pInfo;
		pInfo = NULL;
	}

	//֪ͨ������
	if(::IsWindow(m_sTranstParam.CallerWnd))
	{
		::SendMessage(m_sTranstParam.CallerWnd,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)dwParam,(LPARAM)dlParam);
	}
	if(0 != m_sTranstParam.CallerThreadID)
	{
		::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)dwParam,(LPARAM)dlParam);
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
void CThreadManage::OnMsgManageStartDebitFlow(WPARAM wParam,LPARAM lParam)
{
	try
	{
		//��ǿۿ����̿�ʼ
		GetGlobalApp()->m_iDebitTaskNo = 0x01;

		CString* pstrXML = (CString*)wParam;
		if(NULL == pstrXML)
			throw -1;
		if(NULL == m_pThreadDevice)
			throw -2;
		//�����һ�ε��û���Ϣ
		memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
		//֪ͨӲ�������߳�
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_START_DEBIT_FLOW,(WPARAM)wParam,(LPARAM)lParam);//�����ݽ�����ʽ���͹�ȥ

		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�]:�ⲿ�������ÿۿ����(���ϲ���)�����ױ�ʶ:%s"),GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier));
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
void CThreadManage::OnMsgDebitCancel(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	CString* pstrCancelInfo = (CString*)wParam;
	int nType = (int)lParam;
	try
	{
		if(NULL == pstrCancelInfo)
			throw -1;

		//�رռ��ۿ�״̬��ʱ��
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//�ر��ϴεĶ�ʱ��
		StopTimer(TIMER_ID_DEBIT_CANCEL);

		//����������ʱ��ʱ��
		StartTimer(TIMER_ID_DEBIT_CANCEL,10*1000);

		//���ÿۿ�ģʽ
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;
		//��ʶ���״������
		GetGlobalApp()->m_bIsTrading = FALSE;

		//������Դ����
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -2;

		pstrString->Format(_T("%s"),GetGlobalApp()->ConstructCancleInfo(nType,*pstrCancelInfo));


		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL,(WPARAM)pstrString,(LPARAM)lParam);//�����ݽ�����ʽ���͹�ȥ

		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�]:�ⲿ�������ó���������������Ϣ:%s"),*pstrCancelInfo));	

		//�ͷ���Դ
		delete pstrCancelInfo;
		pstrCancelInfo = NULL;

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[ҵ������߳�]:�ⲿ�������ó����ӿ�,�������(����ʶ����)ָ��Ϊ��"));
			break;
		case -2:
			strLog.Format(_T("[ҵ������߳�]:�ڲ�������Դʧ��"));
		}
		//������־
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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

		m_sDVJsonDebit.Reset();

		strLog.Format(_T("[ҵ������߳�],���ڿ�ʼ�˳������߳�..."));
		//��¼��־
		GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);

		//�߳��˳����
		DWORD dwThreadExitFlag	= 0x00;
		//�������ԵĴ���
		int iAllowTryCount		= 10;
		//�Ѿ����Դ���
		int iHaveTryCount		= 0;
		//ÿ�����Ե�ʱ����ʱ�䣨��λ�����룩
		DWORD	dwSpanTime		= 100;

		//ֹͣ����Ӳ�������߳�
		if(NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
		{
			//���ø��߳��Ƴ����Ϊ��
			m_pThreadDevice->m_bExit = TRUE;
			//��Ĭ50����
			::Sleep(50);

			//ÿ�εȴ�1000���룬���Դ���������10��
			while(OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadDevice->m_hThread,1000))
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

			strLog.Format(_T("[ҵ������߳�],%s"),GetGlobalApp()->FmtStr(_T("���豸�����̡߳��Ѿ�ֹͣ(���Դ��� %d)..."),iHaveTryCount));
			//��¼��־
			GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);
		}

		::Sleep(dwSleepTime);
	}
	catch(int& iErrorCode)
	{
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::GetNetPayRecordMatchCode
//     
// ������������ȡ������֧���Ľ���ƥ����
//     
// ���������IN const CString& strEncKey -- AES�㷨��Կ
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��4��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
// ƥ��ʹ�õ�������
// 1.���վ��	��7λ�ַ�)
// 2.���ʱ��	��14λ�ַ�)
// 3.����վ��	��7λ�ַ�)
// 4.����ʱ��	��14λ�ַ�)
// 5.���ڳ�����	��3λ�ַ���
// 6.���⿨��	��20λ�ַ���
// 7.���׽��	��10λ�ַ���
// ʹ��ԭ��
// �������ƴ�ӳ�һ���ַ���������:75��S����S����AES���ܣ�ʹ����ԿK���õ�����D��16�����ַ�������
// ������D����MD5����õ�ƥ����M
// У��ԭ�򣺸���������ʽ����µ�ƥ����M1��M��M1���ƥ������
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadManage::GetNetPayRecordMatchCode(IN const CString& strEncKey)
{
	CString strMatchCode;
	//У����Կ
	CString strRealKey(strEncKey);
	if(strRealKey.IsEmpty())
	{
		switch(this->m_sTranstParam.ProvinceID)
		{
		case 63:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_63);
			break;
		case 44:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_44);
			break;
		default:
			//Ĭ����Կ
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA);
			break;
		}
	}
	//#ifdef _DEBUG
	//	CString strData;
	//	//���վ��
	//	CString strEntryST;
	//	strEntryST = _T("6300902");
	//	//���ʱ��
	//	CString strEntryTime;
	//	CTime cEntryTime(GetGlobalApp()->m_pTaskCoreValue->EntryTime);
	//	strEntryTime = _T("20160421150636");
	//	//����վ��
	//	CString strExitST;
	//	strExitST = _T("6300903");
	//	//����ʱ��
	//	CString strExitTime;
	//	CTime cExitTime(GetGlobalApp()->m_pTaskCoreValue->OperateTime);
	//	strExitTime = _T("20160421151138");
	//	//���ڳ���
	//	CString strExitLane;
	//	strExitLane = _T("110");
	//	//���⿨��
	//	CString strVirtualCardID;
	//	strVirtualCardID = _T("63110420230010000081");
	//	//���׽��
	//	CString strTradeMoney;
	//	strTradeMoney = _T("0000000500");
	//
	//	strData.Format(_T("%s%s%s%s%s%s%s"),strEntryST,strEntryTime,strExitST,strExitTime,strExitLane,strVirtualCardID,strTradeMoney);
	//#else
	/**************************�����������**************************/
	CString strData;
	//���վ��
	CString strEntryST;
	if(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
	{
		strEntryST.Format(_T("%07d"),GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID);
	}
	else
	{
		strEntryST.Format(_T("%010d"),GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID);
	}
	//���ʱ��
	CString strEntryTime;
	CTime cEntryTime(GetGlobalApp()->m_sCurrentDebitInfo.EntryTime);
	strEntryTime.Format(_T("%04d%02d%02d%02d%02d%02d"),cEntryTime.GetYear(),cEntryTime.GetMonth(),cEntryTime.GetDay(),cEntryTime.GetHour(),cEntryTime.GetMinute(),cEntryTime.GetSecond());
	//����վ��
	CString strExitST;
	if(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
	{
		strExitST.Format(_T("%07d"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
	}
	else
	{
		strExitST.Format(_T("%010d"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
	}
	//����ʱ��
	CString strExitTime;
	CTime cExitTime(GetGlobalApp()->m_sCurrentDebitInfo.SubTime);
	strExitTime.Format(_T("%04d%02d%02d%02d%02d%02d"),cExitTime.GetYear(),cExitTime.GetMonth(),cExitTime.GetDay(),cExitTime.GetHour(),cExitTime.GetMinute(),cExitTime.GetSecond());
	//���ڳ���
	CString strExitLane;
	strExitLane.Format(_T("%03d"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);

	//���⿨��
	CString strVirtualCardID;
	strVirtualCardID.Format(_T("%s"),_T("00000000000000000000"));

	//else
	//{
	//	strVirtualCardID.Format(_T("%s"),GetGlobalApp()->m_sCurrentDebitInfo.CardID);
	//	if(strVirtualCardID.GetLength()<20)
	//	{
	//		int iRemainCount = 20 - strVirtualCardID.GetLength();
	//		CString strTemp;
	//		CString strTemp1;
	//		for(int i = 0;i<iRemainCount;++i)
	//		{
	//			strTemp += _T("0");
	//		}
	//		strTemp1.Format(_T("%s%s"),strTemp,strVirtualCardID);

	//		strVirtualCardID = strTemp1;
	//	}
	//}
	//���׽��
	CString strTradeMoney;
	strTradeMoney.Format(_T("%010d"),GetGlobalApp()->m_sCurrentDebitInfo.Money);

	strData.Format(_T("%s%s%s%s%s%s%s"),strEntryST,strEntryTime,strExitST,strExitTime,strExitLane,strVirtualCardID,strTradeMoney);
	//#endif

	//	strData.Format(_T("044120300820160829160329044120300920160829160329054000000000000000000000000000200"));
	/**************************���ܴ���**************************/

	////AES����
	CAesECB cAesECB(strRealKey);
	CString strEncData = cAesECB.EncryptData(strData);

	//MD5У��
	CMD5 md5((LPCSTR)strEncData);
	strMatchCode = md5.GetMD5();
	//����ƥ����
	return strMatchCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CThreadManage::GetNetPaySecurityCode
//     
// ������������ȡ���װ�ȫ��
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadManage::GetNetPaySecurityCode(void)
{
	return GetNetPayRecordMatchCode();
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
	CFileFindExt cFindFile;
	CString strDir;
	CString strExtName(_T("log"));
	strDir.Format(_T("%s"),::GetGlobalApp()->m_sTranstParam.PathLogDir);
	try
	{
		if(!cFindFile.SearchFiles(strDir,strExtName))
			throw -1;

		int iDeleteCount = 0;
		CTime cCurrentTime = CTime::GetCurrentTime();
		for(int i = 0;i<cFindFile.GetSize();++i)
		{
			CString strFilePath;
			CString strFileName = cFindFile.GetAt(i);
			strFilePath.Format(_T("%s\\%s"),strDir,strFileName);
			if( strFileName.GetLength() != 15 )
				continue;
			int iYear = _ttoi(strFileName.Mid(3,4));
			int iMon  = _ttoi(strFileName.Mid(7,2));
			int iDay  = _ttoi(strFileName.Mid(9,2));
			CTime cFileTime(iYear,iMon,iDay,0,0,0);
			//��ȡ���ļ���ʱ��������ʱ����������
			CTimeSpan span = cCurrentTime - cFileTime ; 
			int iSpanDay = span.GetDays();
			if(iSpanDay < GetGlobalApp()->m_iDeleteLogSaveDays)
				continue;
			if(!::PathFileExists(strFilePath))
				throw -3;
			::DeleteFile(strFilePath);
			if(++iDeleteCount>=10)
			{
				break;
			}
		}
		//�������
		cFindFile.Close();
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
		memcpy(&sTranstParam,&m_sTranstParam,sizeof(tagTranstParam));

		//����Ӳ�������߳�
		m_pThreadDevice = (CThreadDevice *)::AfxBeginThread(RUNTIME_CLASS(CThreadDevice),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadDevice)
			throw -2;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_DEVICE_NOTIFY;
		strConfigFilePath.Format(_T("%s\\%s"),GetGlobalApp()->m_strWorkDir,FILE_NAME_DEVICE_CONFIG);
		m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadDevice->SetParam(&sTranstParam,NULL);
		m_pThreadDevice->m_bAutoDelete = FALSE;
		//�����߳����»���
		m_pThreadDevice->ResumeThread();
		//��Ĭ200����
		Sleep(200);
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
void CThreadManage::StartTimer(IN const UINT& nEventID,IN const DWORD& dwDelayTime)
{
	switch(nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://�ۿ����̶�ʱ��
		{
			if(0 == m_nTimerIDForDebitMoney)
			{
				TIMECAPS   tc; 
				//���ú���timeGetDevCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//�ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
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
			if(0 == m_nTimerIDForDebitCancel)
			{
				TIMECAPS tc;
				//���ú���timeGetDevCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//�ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
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
	switch(nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://�ۿ����̶�ʱ��
		{
			if(0 != m_nTimerIDForDebitMoney)
			{
				timeKillEvent(m_nTimerIDForDebitMoney);
				m_nTimerIDForDebitMoney = 0;
			}			
		}
		break;

	case TIMER_ID_DEBIT_CANCEL: //������ʱ��ʱ��
		{
			if(0 != m_nTimerIDForDebitCancel)
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


	m_sDVJsonDebit.Reset();	

	//��ʱ�������µĽ��׽�������Ҫ�Զ����𳷵� ---------- 	//���Խ׶β��������𳷵�
	if(!bNormal)
	{
		//����ۿ������Դ
		GetGlobalApp()->m_sCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
	//	//���쳷������
	//	CString* strData = new CString();
	//	if(NULL != strData)
	//	{
	//		Json::Value root;
	//		Json::Value DebitCancelParamContext;
	//		Json::Value Data;

	//		Data["PayIdentifier"] = Json::Value(strPayIdentifier);

	//		DebitCancelParamContext["Data"] = Data;
	//		DebitCancelParamContext["Version"] = Json::Value(_T("1.0"));

	//		root["DebitCancelParamContext"] = DebitCancelParamContext;

	//		//ת��Ϊ�ַ���
	//		Json::FastWriter fast_writer;
	//		strData->Format(_T("%s"),fast_writer.write(root).c_str());

	//	

	//		//���ͳ�������
	//		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL,(WPARAM)strData,(LPARAM)2);//�����ݽ�����ʽ���͹�ȥ

	//	}
	}
	//��ʶ�ۿ����̽���
	if(GetGlobalApp()->m_bCurrentDebitMode)
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
void CALLBACK CThreadManage::OneMilliSecondProc(UINT nTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	if(((CThreadManage*)dwUser)->m_nTimerIDForDebitMoney == nTimerID)	//������ָ����ʱʱ���ѵ�
	{
		//������Ϊ��ʱ���������
		CString strTradeID = ((CThreadManage*)dwUser)->FinishTrade(FALSE);
		//��¼��־
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[ҵ������߳�]:�ۿ������ʱ������Ԥ����ʱ,%d �룩���Զ����ó������������ױ�ʶ:%s"),::GetGlobalApp()->m_sCurrentDebitInfo.OverTime/1000,strTradeID));

		//֪ͨ�����߷��ؽ������ǰ����ʧ�� ----�ۿʱ
		if(0 != ((CThreadManage*)dwUser)->m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(((CThreadManage*)dwUser)->m_sTranstParam.CallerThreadID,((CThreadManage*)dwUser)->m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(0x01,0x06),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(((CThreadManage*)dwUser)->m_sTranstParam.CallerWnd))
		{
			::PostMessage(((CThreadManage*)dwUser)->m_sTranstParam.CallerWnd,((CThreadManage*)dwUser)->m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(0x01,0x06),MAKEWORD(0,0)),(LPARAM)0);
		}
	}
	else if(((CThreadManage*)dwUser)->m_nTimerIDForDebitCancel == nTimerID) //������ʱ
	{
		//�رճ�����ʱ��ʱ��
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_DEBIT_CANCEL);
		//֪ͨ�����߷��ؽ������ǰ����ʧ��
		if(0 != ((CThreadManage*)dwUser)->m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(((CThreadManage*)dwUser)->m_sTranstParam.CallerThreadID,((CThreadManage*)dwUser)->m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(0x02,0x06),MAKEWORD(0,0)),(LPARAM)0);
		}
		if(::IsWindow(((CThreadManage*)dwUser)->m_sTranstParam.CallerWnd))
		{
			::PostMessage(((CThreadManage*)dwUser)->m_sTranstParam.CallerWnd,((CThreadManage*)dwUser)->m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(0x02,0x06),MAKEWORD(0,0)),(LPARAM)0);
		}
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
	if(!bState)
	{
		memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
	}
}


BOOL CThreadManage::GetHKResult(IN WORD nCmdType,OUT CString& sResult)
{
	BYTE wSubType = LOBYTE(nCmdType);
	BYTE wCmdType = HIBYTE(nCmdType);

	BOOL bRet = FALSE;
	sResult.Empty();
	switch(wCmdType)
	{
	case 0x01:
		if(!m_HKResult.strB1ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB1ResultData);
			//m_HKResult.strB1ResultData.Empty();
			bRet = TRUE;
		}
		break;
	case 0x02:
		if(!m_HKResult.strB2ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB2ResultData);
			m_HKResult.strB2ResultData.Empty();
			bRet = TRUE;
		}
		break;

	case 0x03:
		if(!m_HKResult.strB3ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB3ResultData);
			m_HKResult.strB3ResultData.Empty();
			bRet = TRUE;
		}
		break;
	case 0x04:
		if(!m_HKResult.strB4ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB4ResultData);
			m_HKResult.strB4ResultData.Empty();
			bRet = TRUE;
		}
		break;
	case 0x05:
		if(!m_HKResult.strB5ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB5ResultData);
			m_HKResult.strB5ResultData.Empty();
			bRet = TRUE;
		}
		break;
	case 0x06:
		if(!m_HKResult.strB6ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB6ResultData);
			m_HKResult.strB6ResultData.Empty();
			bRet = TRUE;
		}
		break;
	case 0x07:
		if(!m_HKResult.strB7ResultData.IsEmpty())
		{
			sResult.Format(_T("%s"),m_HKResult.strB7ResultData);
			m_HKResult.strB7ResultData.Empty();
			bRet = TRUE;
		}
		break;
	}

	return bRet;

}