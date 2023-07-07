/*
* Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
*
* 文件名称：ThreadManage.cpp
* 摘    要：业务后台管理线程，实现文件
* 当前版本：1.0.0.1
* 作    者：ROCY
* 创建日期：2016年7月24日
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
	//关闭计时器
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
	//清除最小定时器精度
	timeEndPeriod(m_nAccuracy);

	//add 2017-08 下面的这段代码，是为了避免在管理线程退出的时候，资源可能没有被释放
	//虽然，现有的代码在OnIdle里面进行了资源释放，但是Onidle里面的对应位置不一定会执行。
	this->fnReleaseResource();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadManage, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY, OnMsgReceiveNorify)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_START_DEBIT_FLOW, OnMsgManageStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_DEBIT_CANCEL, OnMsgDebitCancel)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnIdle
//
// 功能描述：线程空闲处理函数
//
// 输入参数：LONG lCount
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadManage::OnIdle(LONG lCount)
{
	 CScopeLock tmpIdle(&m_idleMutex);
	if (m_bExit)
	{
		//清理资源
		fnReleaseResource();
		//对本线程抛出退出消息
		::PostQuitMessage(0);
		return FALSE;
	}

	//检测驱动加载情况(30秒检查一次)
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
	//线程存活日志记录检测
	static time_t tLastManageLogTime = 0;
	if ( (time(NULL) - tLastManageLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[业务管理线程],线程存活日志检测"));
		GetGlobalApp()->RecordLog(strLog);
		//重设检测时间
		tLastManageLogTime = time(NULL);
	}

	//检查是否需要删除日志(限定时间段执行该操作)
	CTime cCurrentTime = CTime::GetCurrentTime();
	if ( (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iDeleteLogStartHour ) && (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iDeleteLogEndHour) )
	{
		//每隔指定时间检索一次是否需要删除日志
		static time_t tLastDeleteLogTime = 0;
		if ( (time(NULL) - tLastDeleteLogTime) > GetGlobalApp()->m_iDeleteLogSpanTime)
		{
			//检索并删除日志
			CheckAndDeleteLogFile();
			//重设检测时间
			tLastDeleteLogTime = time(NULL);
		}
	}
	TransmitComDataFeedback();
	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}




////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgReceiveNorify
//
// 功能描述：获取组件推送消息
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：FFFF
// 编写时间：2017年3月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_NOTIFY
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
		case RDM_SET_MMI_INFO://显示交互
		case RDM_SET_MMI_BYTE:
		case RDM_SET_MMI_AD:
			break;
		case RDM_DEBITE_START://扣款回馈
		{
			//关闭扣款超时定时器
			StopTimer(TIMER_ID_DEBIT_MONEY);
			//构造回馈上位机的通知代码
			byFunCode	= RCC_FUN_DEBIT;

			int iThrowError = 0x00;
			//判断附加回馈参数是否异常
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
				//结束当前交易(异常结束)
//					GetGlobalApp()->RecordLog("1231232131237129837129312739218");
				if (byExcuteCode == OP_CANCLE)
				{
					FinishTrade(TRUE);//add 2018-01 操作被取消，属于正常结束
					return ;
				}
				else
				{
					FinishTrade(FALSE);
					//抛出异常
					throw iThrowError;
				}

			}

			//解析扣款结果
			CJsonParamOperate jsonParamOperation;
			CDVJsonParamDebitResult* jsonDebitResult = NULL;
			//		jsonDebitResult = (CDVJsonParamDebitResult*)jsonParamOperation.FormatResponseResult2Class(*pStrInfo,JSON_DT_PARAM_DEBIT_RESULT);

			//if(NULL != jsonDebitResult)
			{
				//构造回馈上位机的通知代码
				byRCCState	= RCC_OK;//jsonDebitResult->ExecuteCode;
				//判断扣费执行结果
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

				//结束当前交易(正常结束)
				FinishTrade(TRUE);

				//清除资源
				delete jsonDebitResult;
				jsonDebitResult = NULL;
			}
		}
		break;
		case RDM_DEBITE_CANCEL://撤单回馈
		{
			//关闭撤单超时定时器
			StopTimer(TIMER_ID_DEBIT_CANCEL);
			byFunCode	= RCC_FUN_CANCEL;
			byRCCState	= byExcuteCode;
			pStrInfo = (CString*)lParam;
			if (NULL == pStrInfo)
				throw - 1;

			if ((*pStrInfo).IsEmpty())
				throw - 2;
			//直接把结果通知上去
			wResult = _ttoi(*pStrInfo);
			//构造回馈上位机的通知代码
			byRCCState = wResult;

		}
		break;
		case RDM_DEVICE_INIT:	//初始化结果
			break;
		case RDM_HEART:			//心跳查询
			break;
		case RDM_READ_HW_VER:	//硬件普通参数读取
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
		case RDM_SET_HEART_TIME://硬件普通参数设置
		case RDM_SET_WELCOME_SC:
		case RDM_SET_DEVICE_TIME:
		case RDM_SET_NET0_IP_INFO:
		case RDM_SET_NET1_IP_INFO:
			break;
		case RDM_PARAM_WRITE_MAX_CODE://关键参数设置
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
			strLog.Format(_T("[业务管理线程],响应智能硬件控制线程回馈处理完成，返回的附带参数描述数据为空！"));
			break;
		case -3:
			strLog.Format(_T("[业务管理线程],响应智能硬件控制线程回馈处理完成，未知的业务类型[%d]"), byCmdType);
			break;
		}
	}

	//保存日志
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog);
	}

	//清理资源
	if (NULL != pStrInfo)
	{
		delete pStrInfo;
		pStrInfo = NULL;
	}

	//仅仅当回馈是扣费或撤单，才需要通知上位机
	if (   (RDM_DEBITE_START  == byCmdType)
	        || (RDM_DEBITE_CANCEL == byCmdType)
	   )
	{
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应智能硬件控制线程回馈处理完成，格式化结果(功能代码:%d,执行状况:%d)"), byFunCode, byRCCState));
		//通知调用者
		::GetGlobalApp()->NotifyCaller(byFunCode, byRCCState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgStartDebitFlow
//
// 功能描述：接收到消息，启动扣款流程（整合操作模式）
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月5日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：WM_THREAD_MANAGE_START_DEBIT_FLOW
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgManageStartDebitFlow(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//标记扣款流程开始
		GetGlobalApp()->m_iDebitTaskNo = 0x01;

		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pThreadDevice)
			throw - 2;
		//清除上一次的用户信息
		memset(&m_sAccountInfo, 0, sizeof(tagAccountInfo));
		//通知硬件控制线程
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_START_DEBIT_FLOW, (WPARAM)wParam, (LPARAM)lParam); //把数据解析方式抛送过去

		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:外部主动调用扣款操作(整合操作)，交易标识:%s"), GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier));
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
// 函数名称：CThreadManage::OnMsgDebitCancel
//
// 功能描述：执行撤单操作
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：FF
// 编写时间：2017年1月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_MANAGE_DEBIT_CANCEL
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

		//关闭检查扣款状态定时器
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//关闭上次的定时器
		StopTimer(TIMER_ID_DEBIT_CANCEL);

		//启动撤单超时定时器
		StartTimer(TIMER_ID_DEBIT_CANCEL, 10 * 1000);

		//重置扣款模式
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;
		//标识交易处理完成
		GetGlobalApp()->m_bIsTrading = FALSE;

		//进行资源申请
		CString* pstrString = new CString();
		if (NULL == pstrString)
			throw - 2;
		pstrString->Format(_T("%s"), GetGlobalApp()->ConstructCancleInfo(iType, *pstrCancelInfo));
		//递交撤单
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL, (WPARAM)pstrString, (LPARAM)lParam);
		//沉默800毫秒后清屏
		::Sleep(800);
		CString* pstrStringClear = new CString();
		if (NULL == pstrStringClear)
			throw - 2;


		pstrStringClear->Format(_T("%s"), GetGlobalApp()->GetClearScreenJson()); //"<?xml version=\"1.0\" encoding=\"GB2312\"?><DisplayContext><Version>1.0</Version><ProvinceID>99</ProvinceID><VerifyCode>0</VerifyCode><Data><Type>1</Type></Data></DisplayContext>"));
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_SHOW_DEAL_INFO, (WPARAM)pstrStringClear, (LPARAM)iType);
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:外部主动调用撤单操作，交易标识:%s"), *pstrCancelInfo));
		//释放资源
		delete pstrCancelInfo;
		pstrCancelInfo = NULL;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[业务管理线程]:外部主动调用撤单接口,传入参数(交易识别码)指针为空"));
			break;
		case -2:
			strLog.Format(_T("[业务管理线程]:内部申请资源失败"));
		}
		//缓存日志
		if (strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc, LOG_LEVEL_ERROR);
		}
	}
}

/****************************************内部辅助函数********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::fnReleaseResource
//
// 功能描述：清除本类申请的资源
//
// 输入参数：无参数
// 输出参数：voi
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::fnReleaseResource()
{
	CString strLog;
	try
	{
		DWORD dwSleepTime = 0;

		CString strLog;

		/**********************************************清理各种线程资源**********************************************/
		strLog.Format(_T("[业务管理线程],正在开始退出工作线程..."));
		//记录日志
		GetGlobalApp()->RecordLog(strLog, LOG_LEVEL_RUN, TRUE);

		//线程退出标记
		DWORD dwThreadExitFlag	= 0x00;
		//允许重试的次数
		int iAllowTryCount		= 10;
		//已经重试次数
		int iHaveTryCount		= 0;
		//每次重试的时候间隔时间（单位：毫秒）
		DWORD	dwSpanTime		= 100;

		//停止智能硬件控制线程
		if (NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
		{
			strLog.Format(_T("[业务管理线程],开始终止业务线程..."));
			//设置该线程推出标记为真
			m_pThreadDevice->m_bExit = TRUE;
			//沉默50毫秒
			::Sleep(50);

			//每次等待1000毫秒，重试次数不超过10次
			GetGlobalApp()->RecordLog(_T("[业务管理线程],等待终止结束..."));
			while (OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadDevice->m_hThread, 1000))
			{
				Sleep(dwSpanTime);
				++iHaveTryCount;
				if (iHaveTryCount >= iAllowTryCount)
				{
					//标记该线程未能正常退出
					dwThreadExitFlag |= 0x01;
					//终止尝试
					break;
				}
			}

			delete m_pThreadDevice;
			m_pThreadDevice = NULL;
			::Sleep(1);

			strLog.Format(_T("[业务管理线程],%s"), GetGlobalApp()->FmtStr(_T("【设备控制线程】已经停止(重试次数 %d)..."), iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog, LOG_LEVEL_RUN, TRUE);
		}
		else
		{
			strLog.Format(_T("[业务管理线程],工作线程不存在或已经退出"));
		}
	

		iHaveTryCount = 0;
		//停止NFC签到通讯线程
		if (NULL != m_pSwitchThread)
		{
			HANDLE switchHandle = m_pSwitchThread->m_hThread;
			m_pSwitchThread->waitTillIdle();
			//设置该线程推出标记为真
			//m_pSwitchThread->m_bExit = TRUE;
			//沉默50毫秒
			::Sleep(50);

			//每次等待1000毫秒，重试次数不超过10次
			while (OP_SUCCESS != GetGlobalApp()->fnCloseThread(switchHandle, 1000))
			{
				Sleep(dwSpanTime);
				++iHaveTryCount;
				if (iHaveTryCount >= iAllowTryCount)
				{
					//标记该线程未能正常退出
					dwThreadExitFlag |= 0x01;
					//终止尝试
					break;
				}
			}

			strLog.Format(_T("[业务管理线程],%s"), GetGlobalApp()->FmtStr(_T("【版本管理线程】已经停止(重试次数 %d)..."), iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog, LOG_LEVEL_RUN, TRUE);
		}

		::Sleep(dwSleepTime);
	}
	catch (int& iErrorCode)
	{
	}
}


//add 2018-04 此处代码从1.5相关位置拷贝
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
				//获取该文件的时间与现在时间的相差天数
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
		//清除数据
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
// 函数名称：CThreadManage::CheckAndDeleteLogFile
//
// 功能描述：检索并删除日志
//
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
			//获取该文件的时间与现在时间的相差天数
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
		//清除数据
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
// 函数名称：CThreadDevice::StartWorkThreads
//
// 功能描述：启动工作线程
//
// 输入参数：IN const tagTranstParam& sTranstParam
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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

		//启动硬件控制线程
		m_pThreadDevice = (CThreadDevice *)::AfxBeginThread(RUNTIME_CLASS(CThreadDevice), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		if (NULL == m_pThreadDevice)
			throw - 2;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_DEVICE_NOTIFY;
		strConfigFilePath.Format(_T("%s\\%s"), GetGlobalApp()->m_strWorkDir, FILE_NAME_DEVICE_CONFIG);
		m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadDevice->SetParam(&sTranstParam, NULL);
		m_pThreadDevice->m_bAutoDelete = FALSE;
		//将该线程重新唤醒
		m_pThreadDevice->ResumeThread();
		//沉默200毫秒
		Sleep(200);

		//add 2018-05
		//启动版本管理线程
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
// 函数名称：CThreadManage::StartTimer
//
// 功能描述：设置定时器
//
// 输入参数：IN const UINT& nEventID
//         ：IN const DWORD& dwDelayTime
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::StartTimer(IN const UINT& nEventID, IN const DWORD& dwDelayTime)
{
	switch (nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://扣款流程定时器
	{
		if (0 == m_nTimerIDForDebitMoney)
		{
			TIMECAPS   tc;
			//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
			if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
			{
				//分辨率的值不能超出系统的取值范围
				m_nAccuracy = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
				//调用timeBeginPeriod函数设置定时器的分辨率
				timeBeginPeriod(m_nAccuracy);
				//设置扣款流程定时器
				m_nTimerIDForDebitMoney = timeSetEvent(
				                              dwDelayTime,						//延时(MS)
				                              m_nAccuracy,						//延迟分辨率
				                              (LPTIMECALLBACK)OneMilliSecondProc,	//回调函数
				                              (DWORD)this,						//使得回调函数有机会处理给定的类的数据
				                              TIME_ONESHOT);						//只调用一次(TIME_PERIODIC:周期调用)
			}
		}
	}
	break;


	case TIMER_ID_DEBIT_CANCEL: //撤单超时定时器
	{
		if (0 == m_nTimerIDForDebitCancel)
		{
			TIMECAPS tc;
			//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
			if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
			{
				//分辨率的值不能超出系统的取值范围
				m_nAccuracy = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
				//调用timeBeginPeriod函数设置定时器的分辨率
				timeBeginPeriod(m_nAccuracy);
				//设置扣款状态查询定时器
				m_nTimerIDForDebitCancel = timeSetEvent(
				                               dwDelayTime,						//延时(MS)
				                               m_nAccuracy,						//延迟分辨率
				                               (LPTIMECALLBACK)OneMilliSecondProc,	//回调函数
				                               (DWORD)this,						//使得回调函数有机会处理给定的类的数据
				                               TIME_ONESHOT);						//只调用一次(TIME_PERIODIC:周期调用)
			}

		}
	}
	break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::StopTimer
//
// 功能描述：停止定时器
//
// 输入参数：IN const UINT& nEventID
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年9月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::StopTimer(IN const UINT& nEventID)
{
	switch (nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://扣款流程定时器
	{
		if (0 != m_nTimerIDForDebitMoney)
		{
			timeKillEvent(m_nTimerIDForDebitMoney);
			m_nTimerIDForDebitMoney = 0;
		}
	}
	break;

	case TIMER_ID_DEBIT_CANCEL: //撤单超时定时器
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
// 函数名称：CThreadManage::FinishTrade
//
// 功能描述：交易结束的处理
//
// 输入参数：IN const BOOL& bNormal -- 正常或异常结束交易
// 输出参数：CString 该次交易的识别码
// 编写人员：ROCY
// 编写时间：2016年9月11日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CThreadManage::FinishTrade(IN const BOOL& bNormal)
{
	CString strPayIdentifier = GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;
	//停止接收处理支付子平台回馈的数据
	GetGlobalApp()->m_bIsTrading		= FALSE;
	GetGlobalApp()->m_bCurrentDebitMode = FALSE;

	//关闭扣款超时定时器
	StopTimer(TIMER_ID_DEBIT_MONEY);

	//超时截至导致的交易结束，需要自动发起撤单
	if (!bNormal)
	{
		//清理扣款相关资源
		GetGlobalApp()->m_sCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
		//构造撤单数据
		CString* strData = new CString();
		if (NULL != strData)
		{
			Json::Value root;
			Json::Value DebitCancelParamContext;
			Json::Value Data;
			// 标识关键数据项
			Json::Value KeyItem;

			KeyItem["PayIdentifier"]			= Json::Value(strPayIdentifier);
			Data["TradeKeyItem"]				= KeyItem;

			DebitCancelParamContext["Data"]		= Data;
			DebitCancelParamContext["Version"]	= Json::Value(_T("1.0"));

			root["DebitCancelParamContext"]		= DebitCancelParamContext;

			//转化为字符串
			Json::FastWriter fast_writer;
			strData->Format(_T("%s"), fast_writer.write(root).c_str());
			//发送撤单请求
			m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL, (WPARAM)strData, (LPARAM)2); //把数据解析方式抛送过去
		}
	}
	//标识扣款流程结束
	if (GetGlobalApp()->m_bCurrentDebitMode)
	{
		GetGlobalApp()->m_iDebitTaskNo = 0x00;
	}

	return strPayIdentifier;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OneMilliSecondProc
//
// 功能描述：定时器响应函数
//
// 输入参数：UINT wTimerID	-- 定时器ID
//         ：UINT msg		-- 定时器消息
//         ：DWORD dwUser	-- 调用者指针
//         ：DWORD dwl		-- 定时器参数1
//         ：DWORD dw2		-- 定时器参数2
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK CThreadManage::OneMilliSecondProc(UINT nTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	if (((CThreadManage*)dwUser)->m_nTimerIDForDebitMoney == nTimerID)	//调用者指定超时时间已到
	{
		//交易因为超时截至而完毕
		CString strTradeID = ((CThreadManage*)dwUser)->FinishTrade(TRUE);
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:扣款操作超时截至（预定超时,%d 秒），自动调用撤单操作，交易标识:%s"), ::GetGlobalApp()->m_sCurrentDebitInfo.OverTime / 1000, strTradeID));
		CString* pstrString = new CString();
		if (NULL == pstrString)
			throw - 2;
		pstrString->Format(_T("%s"), GetGlobalApp()->ConstructCancleInfo(0x00, strTradeID));
		//递交撤单
		((CThreadManage*)dwUser)->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL, (WPARAM)pstrString, (LPARAM)0x00);
		//通知调用者返回结果，当前操作失败 -- 扣款超时
		BYTE byFunCode		= 0x01;
		BYTE byExecuteState	= 0x06;
		::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
	}
	else if (((CThreadManage*)dwUser)->m_nTimerIDForDebitCancel == nTimerID) //撤单超时
	{
		//关闭撤单超时定时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_DEBIT_CANCEL);
		//通知调用者返回结果，当前操作失败
		BYTE byFunCode		= 0x02;
		BYTE byExecuteState	= 0x06;
		::GetGlobalApp()->NotifyCaller(byFunCode, byExecuteState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::SetAccountDataReadyState
//
// 功能描述：设置用户数据是否准备就绪
//
// 输入参数：IN const BOOL& bState
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年9月14日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
		
		//发送消息；
		GetGlobalApp()->send2Bus(24,byteArr,byteSize,KEY_BUSINESS);
		delete byteArr;
	}
}