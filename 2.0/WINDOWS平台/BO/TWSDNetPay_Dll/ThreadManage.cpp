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
	//关闭计时器
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
	//清除最小定时器精度
	timeEndPeriod(m_nAccuracy);

	//清理资源
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
	if(m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}

	//检测驱动加载情况(10秒检查一次)
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
	//线程存活日志记录检测
	static time_t tLastManageLogTime = 0;
	if( (time(NULL) - tLastManageLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[业务管理线程],线程存活日志检测"));
		GetGlobalApp()->RecordLog(strLog);
		//重设检测时间
		tLastManageLogTime = time(NULL);
	}

	//检查是否需要删除日志(限定时间段执行该操作)
	CTime cCurrentTime = CTime::GetCurrentTime();
	if( (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iDeleteLogStartHour ) && (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iDeleteLogEndHour) )
	{
		//每隔指定时间检索一次是否需要删除日志
		static time_t tLastDeleteLogTime = 0;
		if( (time(NULL) - tLastDeleteLogTime) > GetGlobalApp()->m_iDeleteLogSpanTime)
		{
			//检索并删除日志
			CheckAndDeleteLogFile();
			//重设检测时间
			tLastDeleteLogTime = time(NULL);
		}
	}

	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgStartGetAccountInfo
//     
// 功能描述：接收要求执行启动从硬件获取用户信息的操作
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_START_GET_DATA
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
// 函数名称：CThreadManage::OnMsgStartGetAccountInfo
//     
// 功能描述：接收要求执行停止从硬件获取用户信息的操作
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_STOP_GET_DATA
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgStopGetAccountInfo(WPARAM wParam,LPARAM lParam)
{
	if(NULL != m_pThreadDevice)
	{
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA,(WPARAM)0,(LPARAM)0);
		//清除定时器
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//停止接收处理支付子平台回馈的数据
		GetGlobalApp()->m_bIsTrading = FALSE;
		//清理扣款相关资源
		GetGlobalApp()->m_sCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgGetAccountInfo
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
			strC0.Format(_T("%s"),*pInfo); //测试C0结果 data:s/f (%d#%d)
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
			m_HKResult.strB1ResultData.Format(_T("%s"),*pInfo);//心跳设置
			bReady = TRUE;
			break;
		case 2:
			m_HKResult.strB2ResultData.Format(_T("%s"),*pInfo);//费显回馈
			//直接把结果通知上去
			dResult = _ttoi(m_HKResult.strB2ResultData);
			dwParam = MAKELONG(MAKEWORD(0x04,dResult),MAKEWORD(0x00,0x00));
			bReady = TRUE;

			break;

		case 3:

			StopTimer(TIMER_ID_DEBIT_MONEY); //关闭扣款超时定时器
			m_HKResult.strB3ResultData.Format(_T("%s"),*pInfo);//扣款结果

			//解析扣款结果
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

			StopTimer(TIMER_ID_DEBIT_CANCEL);//关闭撤单超时定时器
			m_HKResult.strB4ResultData.Format(_T("%s"),*pInfo);//撤单结果	
			//直接把结果通知上去
			dResult = _ttoi(m_HKResult.strB4ResultData);
			dwParam = MAKELONG(MAKEWORD(0x02,dResult),MAKEWORD(0x00,0x00));
			bReady = TRUE;

			break;
		case 5:
			m_HKResult.strB5ResultData.Format(_T("%s"),*pInfo);//初始化结果
			bReady = TRUE;

			break;
		case 6:
			m_HKResult.strB6ResultData.Format(_T("%s"),*pInfo);//硬件设置结果
			bReady = TRUE;

			break;
		case 7:
			m_HKResult.strB7ResultData.Format(_T("%s"),*pInfo);//未定义
			bReady = TRUE;

			break;
		case 8:
			//直接把结果通知上去
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
		//通知调用者用户数据获取失败
		dwParam = MAKELONG(MAKEWORD(0x03,0x05),MAKEWORD(0x00,0x00));
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应智能硬件控制线程回馈处理完成(获取用户信息)，格式化结果(功能代码:%d,执行状况:%d)"),0x03,0x05));
	}
	//清理资源
	if(NULL != pInfo)
	{
		delete pInfo;
		pInfo = NULL;
	}

	//通知调用者
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
void CThreadManage::OnMsgManageStartDebitFlow(WPARAM wParam,LPARAM lParam)
{
	try
	{
		//标记扣款流程开始
		GetGlobalApp()->m_iDebitTaskNo = 0x01;

		CString* pstrXML = (CString*)wParam;
		if(NULL == pstrXML)
			throw -1;
		if(NULL == m_pThreadDevice)
			throw -2;
		//清除上一次的用户信息
		memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
		//通知硬件控制线程
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_START_DEBIT_FLOW,(WPARAM)wParam,(LPARAM)lParam);//把数据解析方式抛送过去

		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:外部主动调用扣款操作(整合操作)，交易标识:%s"),GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier));
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
void CThreadManage::OnMsgDebitCancel(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	CString* pstrCancelInfo = (CString*)wParam;
	int nType = (int)lParam;
	try
	{
		if(NULL == pstrCancelInfo)
			throw -1;

		//关闭检查扣款状态定时器
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//关闭上次的定时器
		StopTimer(TIMER_ID_DEBIT_CANCEL);

		//启动撤单超时定时器
		StartTimer(TIMER_ID_DEBIT_CANCEL,10*1000);

		//重置扣款模式
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;
		//标识交易处理完成
		GetGlobalApp()->m_bIsTrading = FALSE;

		//进行资源申请
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -2;

		pstrString->Format(_T("%s"),GetGlobalApp()->ConstructCancleInfo(nType,*pstrCancelInfo));


		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL,(WPARAM)pstrString,(LPARAM)lParam);//把数据解析方式抛送过去

		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:外部主动调用撤单操作，交易信息:%s"),*pstrCancelInfo));	

		//释放资源
		delete pstrCancelInfo;
		pstrCancelInfo = NULL;

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[业务管理线程]:外部主动调用撤单接口,传入参数(交易识别码)指针为空"));
			break;
		case -2:
			strLog.Format(_T("[业务管理线程]:内部申请资源失败"));
		}
		//缓存日志
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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

		m_sDVJsonDebit.Reset();

		strLog.Format(_T("[业务管理线程],正在开始退出工作线程..."));
		//记录日志
		GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);

		//线程退出标记
		DWORD dwThreadExitFlag	= 0x00;
		//允许重试的次数
		int iAllowTryCount		= 10;
		//已经重试次数
		int iHaveTryCount		= 0;
		//每次重试的时候间隔时间（单位：毫秒）
		DWORD	dwSpanTime		= 100;

		//停止智能硬件控制线程
		if(NULL != m_pThreadDevice && !(m_pThreadDevice->m_bExit))
		{
			//设置该线程推出标记为真
			m_pThreadDevice->m_bExit = TRUE;
			//沉默50毫秒
			::Sleep(50);

			//每次等待1000毫秒，重试次数不超过10次
			while(OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadDevice->m_hThread,1000))
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

			strLog.Format(_T("[业务管理线程],%s"),GetGlobalApp()->FmtStr(_T("【设备控制线程】已经停止(重试次数 %d)..."),iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);
		}

		::Sleep(dwSleepTime);
	}
	catch(int& iErrorCode)
	{
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::GetNetPayRecordMatchCode
//     
// 功能描述：获取互联网支付的交易匹配码
//     
// 输入参数：IN const CString& strEncKey -- AES算法密钥
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年4月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
// 匹配使用到的因子
// 1.入口站号	（7位字符)
// 2.入口时间	（14位字符)
// 3.出口站号	（7位字符)
// 4.出口时间	（14位字符)
// 5.出口车道号	（3位字符）
// 6.虚拟卡号	（20位字符）
// 7.交易金额	（10位字符）
// 使用原则
// 根据序号拼接成一个字符串（长度:75）S，对S进行AES加密（使用密钥K）得到密文D（16进制字符串），
// 对密文D进行MD5运算得到匹配码M
// 校验原则：根据上述方式获得新的匹配码M1，M与M1相等匹配正常
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadManage::GetNetPayRecordMatchCode(IN const CString& strEncKey)
{
	CString strMatchCode;
	//校验密钥
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
			//默认密钥
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA);
			break;
		}
	}
	//#ifdef _DEBUG
	//	CString strData;
	//	//入口站点
	//	CString strEntryST;
	//	strEntryST = _T("6300902");
	//	//入口时间
	//	CString strEntryTime;
	//	CTime cEntryTime(GetGlobalApp()->m_pTaskCoreValue->EntryTime);
	//	strEntryTime = _T("20160421150636");
	//	//出口站点
	//	CString strExitST;
	//	strExitST = _T("6300903");
	//	//出口时间
	//	CString strExitTime;
	//	CTime cExitTime(GetGlobalApp()->m_pTaskCoreValue->OperateTime);
	//	strExitTime = _T("20160421151138");
	//	//出口车道
	//	CString strExitLane;
	//	strExitLane = _T("110");
	//	//虚拟卡号
	//	CString strVirtualCardID;
	//	strVirtualCardID = _T("63110420230010000081");
	//	//交易金额
	//	CString strTradeMoney;
	//	strTradeMoney = _T("0000000500");
	//
	//	strData.Format(_T("%s%s%s%s%s%s%s"),strEntryST,strEntryTime,strExitST,strExitTime,strExitLane,strVirtualCardID,strTradeMoney);
	//#else
	/**************************构造加密明文**************************/
	CString strData;
	//入口站点
	CString strEntryST;
	if(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
	{
		strEntryST.Format(_T("%07d"),GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID);
	}
	else
	{
		strEntryST.Format(_T("%010d"),GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID);
	}
	//入口时间
	CString strEntryTime;
	CTime cEntryTime(GetGlobalApp()->m_sCurrentDebitInfo.EntryTime);
	strEntryTime.Format(_T("%04d%02d%02d%02d%02d%02d"),cEntryTime.GetYear(),cEntryTime.GetMonth(),cEntryTime.GetDay(),cEntryTime.GetHour(),cEntryTime.GetMinute(),cEntryTime.GetSecond());
	//出口站点
	CString strExitST;
	if(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
	{
		strExitST.Format(_T("%07d"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
	}
	else
	{
		strExitST.Format(_T("%010d"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
	}
	//出口时间
	CString strExitTime;
	CTime cExitTime(GetGlobalApp()->m_sCurrentDebitInfo.SubTime);
	strExitTime.Format(_T("%04d%02d%02d%02d%02d%02d"),cExitTime.GetYear(),cExitTime.GetMonth(),cExitTime.GetDay(),cExitTime.GetHour(),cExitTime.GetMinute(),cExitTime.GetSecond());
	//出口车道
	CString strExitLane;
	strExitLane.Format(_T("%03d"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);

	//虚拟卡号
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
	//交易金额
	CString strTradeMoney;
	strTradeMoney.Format(_T("%010d"),GetGlobalApp()->m_sCurrentDebitInfo.Money);

	strData.Format(_T("%s%s%s%s%s%s%s"),strEntryST,strEntryTime,strExitST,strExitTime,strExitLane,strVirtualCardID,strTradeMoney);
	//#endif

	//	strData.Format(_T("044120300820160829160329044120300920160829160329054000000000000000000000000000200"));
	/**************************加密处理**************************/

	////AES加密
	CAesECB cAesECB(strRealKey);
	CString strEncData = cAesECB.EncryptData(strData);

	//MD5校验
	CMD5 md5((LPCSTR)strEncData);
	strMatchCode = md5.GetMD5();
	//返回匹配码
	return strMatchCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::GetNetPaySecurityCode
//     
// 功能描述：获取交易安全码
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadManage::GetNetPaySecurityCode(void)
{
	return GetNetPayRecordMatchCode();
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
			//获取该文件的时间与现在时间的相差天数
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
		//清除数据
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
		memcpy(&sTranstParam,&m_sTranstParam,sizeof(tagTranstParam));

		//启动硬件控制线程
		m_pThreadDevice = (CThreadDevice *)::AfxBeginThread(RUNTIME_CLASS(CThreadDevice),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadDevice)
			throw -2;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_DEVICE_NOTIFY;
		strConfigFilePath.Format(_T("%s\\%s"),GetGlobalApp()->m_strWorkDir,FILE_NAME_DEVICE_CONFIG);
		m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadDevice->SetParam(&sTranstParam,NULL);
		m_pThreadDevice->m_bAutoDelete = FALSE;
		//将该线程重新唤醒
		m_pThreadDevice->ResumeThread();
		//沉默200毫秒
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
void CThreadManage::StartTimer(IN const UINT& nEventID,IN const DWORD& dwDelayTime)
{
	switch(nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://扣款流程定时器
		{
			if(0 == m_nTimerIDForDebitMoney)
			{
				TIMECAPS   tc; 
				//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//分辨率的值不能超出系统的取值范围     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
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
			if(0 == m_nTimerIDForDebitCancel)
			{
				TIMECAPS tc;
				//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//分辨率的值不能超出系统的取值范围     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
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
	switch(nEventID)
	{
	case TIMER_ID_DEBIT_MONEY://扣款流程定时器
		{
			if(0 != m_nTimerIDForDebitMoney)
			{
				timeKillEvent(m_nTimerIDForDebitMoney);
				m_nTimerIDForDebitMoney = 0;
			}			
		}
		break;

	case TIMER_ID_DEBIT_CANCEL: //撤单超时定时器
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


	m_sDVJsonDebit.Reset();	

	//超时截至导致的交易结束，需要自动发起撤单 ---------- 	//测试阶段不主动发起撤单
	if(!bNormal)
	{
		//清理扣款相关资源
		GetGlobalApp()->m_sCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
	//	//构造撤单数据
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

	//		//转化为字符串
	//		Json::FastWriter fast_writer;
	//		strData->Format(_T("%s"),fast_writer.write(root).c_str());

	//	

	//		//发送撤单请求
	//		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_DEBIT_CANCEL,(WPARAM)strData,(LPARAM)2);//把数据解析方式抛送过去

	//	}
	}
	//标识扣款流程结束
	if(GetGlobalApp()->m_bCurrentDebitMode)
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
void CALLBACK CThreadManage::OneMilliSecondProc(UINT nTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	if(((CThreadManage*)dwUser)->m_nTimerIDForDebitMoney == nTimerID)	//调用者指定超时时间已到
	{
		//交易因为超时截至而完毕
		CString strTradeID = ((CThreadManage*)dwUser)->FinishTrade(FALSE);
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:扣款操作超时截至（预定超时,%d 秒），自动调用撤单操作，交易标识:%s"),::GetGlobalApp()->m_sCurrentDebitInfo.OverTime/1000,strTradeID));

		//通知调用者返回结果，当前操作失败 ----扣款超时
		if(0 != ((CThreadManage*)dwUser)->m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(((CThreadManage*)dwUser)->m_sTranstParam.CallerThreadID,((CThreadManage*)dwUser)->m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(0x01,0x06),MAKEWORD(0,0)),(LPARAM)0);
		}

		if(::IsWindow(((CThreadManage*)dwUser)->m_sTranstParam.CallerWnd))
		{
			::PostMessage(((CThreadManage*)dwUser)->m_sTranstParam.CallerWnd,((CThreadManage*)dwUser)->m_sTranstParam.ActiveNotifyMsgID,(WPARAM)MAKELONG(MAKEWORD(0x01,0x06),MAKEWORD(0,0)),(LPARAM)0);
		}
	}
	else if(((CThreadManage*)dwUser)->m_nTimerIDForDebitCancel == nTimerID) //撤单超时
	{
		//关闭撤单超时定时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_DEBIT_CANCEL);
		//通知调用者返回结果，当前操作失败
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