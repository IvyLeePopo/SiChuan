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
	:m_pThreadHttp(NULL)
	,m_pThreadDevice(NULL)
	,m_pThreadUpdate(NULL)
	,m_pThreadNFCSignIn(NULL)
	,m_pDVJsonDebitCheck(NULL)
	,m_bAccountInfoIsReady(FALSE)
	,m_bDeviceLoadState(FALSE)
	,m_nAccuracy(0)
	,m_nTimerIDForScanCode(0)
	,m_nTimerIDForDebitMoney(0)
	,m_nTimerIDForCheckDebitState(0)
	,m_nTimerIDForNFCGetCorrentData(0)
	,m_nTimerIDForNFCGetNormalData(0)
	,m_dwCheckDebitSpanTime(0)
	,m_dwCheckDebitAccumulateTime(0)
	,m_dwInputPasswordDelayTime(30*1000)
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
	if(0 != m_nTimerIDForNFCGetCorrentData)
	{
		timeKillEvent(m_nTimerIDForNFCGetCorrentData);
	}
	if(0 != m_nTimerIDForNFCGetNormalData)
	{
		timeKillEvent(m_nTimerIDForNFCGetNormalData);
	}
	//清除最小定时器精度
	timeEndPeriod(m_nAccuracy);

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadManage, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_GET_DATA,OnMsgStartGetAccountInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_STOP_GET_DATA,OnMsgStopGetAccountInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY,OnMsgReceiveAccountInfo)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT,OnMsgDealSubPayPlatformResponse)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE,OnMsgCheckDebitPasswordState)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_ONLY_DEBIT_MONEY,OnMsgOnlyDebitMoney)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_START_DEBIT_FLOW,OnMsgManageStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_MANAGE_DEBIT_CANCEL,OnMsgDebitCancel)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP,OnMsgRevNFCSignDataFromHTTP)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_START_SIGN,OnMsgNFCStartSignInFlow)
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
		//清理资源
	//	fnReleaseResource();

		//对本线程抛出退出消息
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


void CThreadManage::ThreadQuit()
{
	//关闭计时器
	if(0 != m_nTimerIDForScanCode)
	{
		timeKillEvent(m_nTimerIDForScanCode);
		m_nTimerIDForScanCode = 0;
	}
	if(0 != m_nTimerIDForDebitMoney)
	{
		timeKillEvent(m_nTimerIDForDebitMoney);
		m_nTimerIDForDebitMoney = 0;
	}
	if(0 != m_nTimerIDForCheckDebitState)
	{
		timeKillEvent(m_nTimerIDForCheckDebitState);
		m_nTimerIDForCheckDebitState = 0;
	}
	if(0 != m_nTimerIDForNFCGetCorrentData)
	{
		timeKillEvent(m_nTimerIDForNFCGetCorrentData);
		m_nTimerIDForNFCGetCorrentData = 0;
	}
	if(0 != m_nTimerIDForNFCGetNormalData)
	{
		timeKillEvent(m_nTimerIDForNFCGetNormalData);
		m_nTimerIDForNFCGetNormalData = 0;
	}
	//清除最小定时器精度
	timeEndPeriod(m_nAccuracy);

	//清理资源
	fnReleaseResource();

	m_bExit = TRUE;
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
		StopTimer(TIMER_ID_DEBIT_CHECK);
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//停止接收处理支付子平台回馈的数据
		GetGlobalApp()->m_bIsTrading = FALSE;
		//清理扣款相关资源
		GetGlobalApp()->m_pCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgGetAccountInfo
//     
// 功能描述：获取到用户信息后的处理过程
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_NOTIFY
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgReceiveAccountInfo(WPARAM wParam,LPARAM lParam)
{
	BOOL bGetData	= (BOOL)wParam;
	DWORD dwParam	= 0x00;
	BYTE byFunCode = 0x03;
	BYTE byExecuteState = 0x00;
	BYTE bySrcType = 0x00;
	tagAccountInfo* pAccountInfo	= (tagAccountInfo*)lParam;
	try
	{
		//获取用户信息
		if(NULL == pAccountInfo)
			throw -1;
		if( 0 == pAccountInfo->DataSize )
			throw -2;

		//NFC非接模块
		bySrcType = pAccountInfo->SrcType;
		if(0x02 == bySrcType)
		{
			if(::GetGlobalApp()->m_bNFCIsUse)
			{
				//停止计时器
				StopTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA);
				//解析数据帧
				int iFrameType = UFT_UNKNOW;
				CNFCProtocol cNFCProtocol;
				iFrameType = cNFCProtocol.Analyze(pAccountInfo->DataContext);
				switch(iFrameType)
				{
				//签到相关
				case UFT_N_SIGN_IN_RES_DATA:
				case UFT_N_SIGN_VERIFY_ERR:
				case UFT_N_SIGN_CONFIRM:
					NFCSignInDataProcessFromPOS(cNFCProtocol);
					break;
				//消费相关
				case UFT_N_CONSUME_RES_W2:
				case UFT_N_CONSUME_RES_W3:
				case UFT_N_CONSUME_TIP:
				case UFT_N_CONSUME_RES_DATA:
				case UFT_N_CONSUME_ERR:
				case UFT_N_CONSUME_NO_SIGN:
					NFCConsumeDataProcessFromPOS(cNFCProtocol);
					break;
				//冲正相关
				case UFT_N_CORRECT_RES_DATA:
					NFCCorrectDataProcessFromPOS(cNFCProtocol);
					break;
				case UFT_N_TEST_LINK_SUCCESS:
				case UFT_N_TEST_LINK_ERROR:
					NFCLinkTestDataProcessFromPOS(cNFCProtocol);
					break;
				//其它
				default:
					NFCProcessErrorFrame(cNFCProtocol);
					break;
				}
			}
		}
		else//默认的扫码数据
		{
			//整合操作，直接向支付子平台推送信息
			if(GetGlobalApp()->m_bCurrentDebitMode)
			{
				//判断当前流程是否允许进行扣款业务处理
				if(!GetGlobalApp()->m_bIsTrading)
					throw -3;
				//标记扣款流程接收到支付凭证
				GetGlobalApp()->m_iDebitTaskNo = 0x02;
				//构造消费数据并通知支付子平台通讯线程开始工作
				ContructDebitMoneyInfo(pAccountInfo);
				
				//如果启用了银联支付，则在此需要停止读卡操作
				if(GetGlobalApp()->m_bNFCIsUse)
				{
					GetGlobalApp()->SetUnipayWorkFlowState(UMS_STOP);
					NFCSendDefaultCmdToPos(3);
				}
			}
			else//单体操作，保存用户数据
			{
				//保存用户信息
				memcpy(&m_sAccountInfo,pAccountInfo,sizeof(tagAccountInfo));
				SetAccountDataReadyState(TRUE);
				//通知调用者用户数据已经准备就绪
				byExecuteState = 0x00;
				dwParam = MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0x00,0x00));
				//记录日志
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应智能硬件控制线程回馈处理完成(获取用户信息)，格式化结果(功能代码:%d,执行状况:%d)"),0x03,0x00));
			}
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
		byExecuteState = 0x05;
		dwParam = MAKELONG(MAKEWORD(byFunCode,byExecuteState),MAKEWORD(0x00,0x00));
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应智能硬件控制线程回馈处理完成(获取用户信息)，格式化结果(功能代码:%d,执行状况:%d)"),0x03,0x05));
	}

	//清理资源
	if(NULL != pAccountInfo)
	{
		delete pAccountInfo;
		pAccountInfo = NULL;
	}
	//通知调用者
	if(0x02 != bySrcType)
	{
		::GetGlobalApp()->NotifyCaller(byFunCode,byExecuteState);
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
	CString strLog;
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
		m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_START_DEBIT_FLOW,(WPARAM)wParam,(LPARAM)0);
		//记录日志
		if(GetGlobalApp()->m_bNFCIsUse)
		{
			strLog.Format(_T("[业务管理线程],外部主动调用整合扣款操作(支持扫码支付和非接支付)，交易标识:%s"),GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}
		else
		{
			strLog.Format(_T("[业务管理线程],外部主动调用整合扣款操作(仅支持扫码支付)，交易标识:%s"),GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}
		GetGlobalApp()->RecordLog(strLog);
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
// 函数名称：CThreadManage::OnMsgOnlyDebitMoney
//     
// 功能描述：执行扣款操作
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_MANAGE_ONLY_DEBIT_MONEY
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgOnlyDebitMoney(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		//构造消费数据并通知支付子平台通讯线程开始工作
		if(!ContructDebitMoneyInfo())
			throw -1;
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:外部主动调用扣款操作(单体操作)，交易标识:%s"),GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[业务管理线程]:外部主动调用扣款操作(单体操作),构造并发送扣款数据操作失败"));
			break;
		}
		//缓存日志
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
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
// 编写人员：ROCY
// 编写时间：2016年8月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_MANAGE_DEBIT_CANCEL
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgDebitCancel(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	CString* pstrPayIdentifier = (CString*)wParam;
	try
	{
		if(NULL == pstrPayIdentifier)
			throw -1;
		//关闭检查扣款状态定时器
		StopTimer(TIMER_ID_DEBIT_MONEY);
		//关闭扣款超时检测定时器
		StopTimer(TIMER_ID_DEBIT_CHECK);
		//重置扣款模式
		GetGlobalApp()->m_bCurrentDebitMode = FALSE;
		//标识交易处理完成
		GetGlobalApp()->m_bIsTrading = FALSE;
		//清除扣款状态查询结构体
		if(NULL == m_pDVJsonDebitCheck)
		{
			delete m_pDVJsonDebitCheck;
			m_pDVJsonDebitCheck = NULL;
		}
		m_sDVJsonDebit.Reset();	
		//标识扣款流程结束
		if(GetGlobalApp()->m_bCurrentDebitMode)
		{
			//关闭扫码器
			if(0x01 == GetGlobalApp()->m_iDebitTaskNo)
			{
				if(NULL != m_pThreadDevice)
				{
					m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA,(WPARAM)0,(LPARAM)0);
				}
			}
			GetGlobalApp()->m_iDebitTaskNo = 0x00;
		}

		m_pThreadHttp->bLoopBreak = TRUE;	//需要跳出扣费执行循环

		//构造撤单数据并通知支付子平台通讯线程开始工作
		CString strPayIdentifier;
		strPayIdentifier.Format(_T("%s"),*pstrPayIdentifier);
		if(!ContructDebitCancelInfo(strPayIdentifier))
			throw -2;
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:外部主动调用撤单操作，交易标识:%s"),strPayIdentifier));		
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("[业务管理线程]:外部主动调用撤单接口,传入参数(交易识别码)指针为空"));
			break;
		case -2:
			strLog.Format(_T("[业务管理线程]:外部主动调用撤单接口,构造并发送撤单数据操作失败"));
			break;
		}
		//缓存日志
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc,LOG_LEVEL_ERROR);
		}
	}
	//清理资源
	if(NULL != pstrPayIdentifier)
	{
		delete pstrPayIdentifier;
		pstrPayIdentifier = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgDealSubPayPlatformResponse
//     
// 功能描述：处理支付子平台的回馈信息
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgDealSubPayPlatformResponse(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	CString strResponseType;
	int		iMMIDetailType = SM_PRPTOCOL_LIGHT_FAIL;
	//获得请求数据的类型
	WORD wOpType  = LOWORD(lParam);
	//获得服务器回馈的结果
	WORD wOpState = HIWORD(lParam);

	//未处于交易当中，不接收回馈
	if(   (JSON_DT_SEND_DIBIT	    == wOpType)
		||(JSON_DT_SEND_DIBIT_CHECK == wOpType)
		)
	{
		/**************************为了保证车道/云端状态一致，如果此时交易表示已经被重置，则需要发起一次撤单，保证当前正在被处理的交易被终止**************************/
		if(!GetGlobalApp()->m_bIsTrading)
		{
			//仅仅在非输入密码的支付场景进行该项处理
			if(JSON_DT_SEND_DIBIT == wOpType)
			{
				CDVJsonBase* pDVJsonBaseTemp = (CDVJsonBase*)wParam;
				if(NULL != pDVJsonBaseTemp)
				{
					//对已取消的交易进行撤单操作
					//FinishHadCancelTrade(pDVJsonBaseTemp);
					////记录日志
					//GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:扣款/查询结果回馈处理，当前交易已被重置，自动调用撤单操作，交易标识:%s"),pDVJsonBaseTemp->PayIdentifier));
					//清理资源
					delete pDVJsonBaseTemp;
					pDVJsonBaseTemp = NULL;
				}
			}
			//直接返回，不再处理
			return;
		}
	}

	//临时交易状态
	BOOL bIsInputPassword	= FALSE;
	BOOL bDebitSuccess		= FALSE;

	CDVJsonBase* pDVJsonBase = (CDVJsonBase*)wParam;

	CString strTip;
	CDVJsonDebit* pDVLocalDebit = new CDVJsonDebit();


	BYTE byFunCode		= 0x01;
	BYTE byExecuteState = RCC_INNER_ERROR;

	try
	{
		//获取业务处理类型
		switch(wOpType)
		{
		case JSON_DT_SEND_DIBIT:
			strResponseType.Format(_T("扣费请求"));
			break;
		case JSON_DT_SEND_DIBIT_CHECK:
			strResponseType.Format(_T("扣费查询"));
			break;
		case JSON_DT_SEND_DIBIT_CANCEL:
			strResponseType.Format(_T("撤销订单"));
			break;
		}

		//获得合法的回馈信息
		if(HTTP_RC_DATA_LEGAL != wOpState)
			throw -1;
		//判断消息回馈类型
		if(    (JSON_DT_SEND_DIBIT			!= wOpType)
			&& (JSON_DT_SEND_DIBIT_CHECK	!= wOpType)
			&& (JSON_DT_SEND_DIBIT_CANCEL	!= wOpType)
			)
		{
			throw -2;
		}
		
		//校核指针是否非法
		if( (NULL == pDVJsonBase) || (NULL == pDVLocalDebit) )
			throw -3;

		if(JSON_DT_SEND_DIBIT == wOpType)
		{
			*pDVLocalDebit = *((CDVJsonDebit*)pDVJsonBase);
			m_sDVJsonDebit = *pDVLocalDebit;
		}
		else
		{
			*((CDVJsonBase*)pDVLocalDebit) = *pDVJsonBase;
		}

		//只处理扣款/查询回馈（撤单回馈后续处理）
		if(JSON_DT_SEND_DIBIT_CANCEL != wOpType)
		{
			switch(pDVJsonBase->ServerResponseCode)
			{
			case HTTP_RC_DEBIT_NEED_PASSWORD:			//需要用户输入密码
				//标记正在输入密码
				bIsInputPassword	= TRUE;
				byExecuteState		= RCC_INPUT_PASSWORD;
				//扩展交互细节（呈现多个不同的失败提示）
				iMMIDetailType		= SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT;
				break;
			case HTTP_RC_DEBIT_INSUFFICIENT_BALANCE:	//余额不足
				byExecuteState		= RCC_NO_MONEY;
				//扩展交互细节（呈现多个不同的失败提示）
				iMMIDetailType		= SM_PRPTOCOL_LIGHT_FALI_BL_ERR;
				break;
			case HTTP_RC_DEBIT_SRC_ILLGAL:				//二维码非法
				byExecuteState		= RCC_QRCODE_ILLEGAL;
				//扩展交互细节（呈现多个不同的失败提示）
				iMMIDetailType		= SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1;
				break;
			case HTTP_RC_DEBIT_CODE_TIMEOUT:			//二维码有效期超时
				byExecuteState		= RCC_QRCODE_OT;
				//扩展交互细节（呈现多个不同的失败提示）
				iMMIDetailType		= SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2;
				break;
			case HTTP_RC_INPUT_PASSWORD_ERROR:			//用户输入密码错误
				byExecuteState		= RCC_PSW_ERROR;
				//扩展交互细节（呈现多个不同的失败提示）
				iMMIDetailType		= SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR;
				break;
			case HTTP_RC_NO_SERVICES:
				byExecuteState		= RCC_INNER_ERROR;	//平台暂停服务
				//扩展交互细节（呈现多个不同的失败提示）
				iMMIDetailType		= SM_PRPTOCOL_LIGHT_FALI_PL_STOP;
				break;

			case HTTP_RC_DEBIT_QUERY_ERROR:				//扣费查询失败（循环执行获取状态，表示扣费进行中）
				byExecuteState		= RCC_DEBIT_QUERY_ERROR;
				break;
			case HTTP_RC_DEBIT_FAIL:					//支付失败
				byExecuteState		= RCC_INNER_ERROR;
				break;

			case HTTP_RC_DEBIT_SUCCESS:					//扣费成功
				//标记扣款成功
				bDebitSuccess		= TRUE;
				byExecuteState		= RCC_OK;
				break;
			default:									//未知异常	
				{
					//其它异常
					byExecuteState = RCC_INNER_ERROR;
				}
				break;
			}
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			if(RCC_PAY_PLATFORM_ERROR == wOpState)
			{
				byExecuteState		= wOpState;
				strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:服务器发生故障"),strResponseType);
			}
			else if(0xF0 == wOpState)
			{
				byExecuteState		= RCC_DEBIT_OT;
				strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:通讯超时"),strResponseType);
			}

			else
			{
				strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:返回数据校验非法"),strResponseType);
				byExecuteState		= RCC_PARAM_ERROR;
			}
			break;
		case -2:
			strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:无效的消息类型"),strResponseType);
			if(RCC_PAY_PLATFORM_ERROR == wOpState)
			{
				byExecuteState     = wOpState;
			}
			else
			{
				byExecuteState		= RCC_PARAM_ERROR;
			}
			break;
		case -3:
			strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:内部资源申请有异常"),strResponseType);
			byExecuteState     = RCC_INNER_ERROR;
			break;
		}
		GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_ERROR);
		strLog.Empty();
	}

	//撤单回馈后续逻辑处理(撤单回馈在此截至)
	if(JSON_DT_SEND_DIBIT_CANCEL == wOpType)
	{
		byFunCode		= 0x02;
		//撤单成功
		if((NULL != pDVJsonBase) && (HTTP_RC_DATA_VALID == pDVJsonBase->ServerResponseCode))
		{
			byExecuteState	= RCC_OK;
		}
		//保存日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应支付子平台回馈处理完成(撤销订单)，将结果通知调用者(功能代码:%d,执行状况:%d)"),byFunCode,byExecuteState));
	}
	else
	{
		/*******************************扣款回馈后续逻辑处理*******************************/

		if(bIsInputPassword)//扣款结果尚未明确
		{
			if( 0 == theApp.m_iHTTPVersion )//原有版本处理
			{
				//用户正在输入密码的处理
				if(NULL != pDVLocalDebit)
				{
					if(NULL == m_pDVJsonDebitCheck)
					{
						m_pDVJsonDebitCheck			= new CDVJsonDebitCheck(pDVLocalDebit);
						m_dwCheckDebitSpanTime		= 0;
						m_dwInputPasswordDelayTime	= GetGlobalApp()->m_pCurrentDebitInfo.OverTime*1000 - (::GetTickCount() - ::GetGlobalApp()->m_dwTradeBeginOvertime);
						if(m_dwInputPasswordDelayTime<10000)
						{
							m_dwInputPasswordDelayTime					 = 30000;
						}
					}

					//记录日志
					strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),用户正在输入密码，进行扣款结果查询"),strResponseType);
					//发起扣款结果快速查询
					PostThreadMessage(WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE,(WPARAM)0,(LPARAM)lParam);
				}
			}
			else//新版本输入密码在HTTP线程中循环，不在此处继续处理
			{
			}
		}
		else				//扣款结果已经出来(失败或成功)
		{
			//如果扣款成功，则进行保存扣款结果操作
			if(bDebitSuccess)
			{
				GetGlobalApp()->m_sCurrentDebitResult.Valid				= TRUE;

				GetGlobalApp()->m_sCurrentDebitResult.ResultType		= 0x00;
				GetGlobalApp()->m_sCurrentDebitResult.ExecuteCode		= 0x00;
				GetGlobalApp()->m_sCurrentDebitResult.PayPlatformType	= m_sDVJsonDebit.QRCodeType;
				GetGlobalApp()->m_sCurrentDebitResult.DebitTime			= pDVLocalDebit->ServerDebitTime.GetTime();
				GetGlobalApp()->m_sCurrentDebitResult.ConsumeTime		= ::GetTickCount() - ::GetGlobalApp()->m_dwTradeBeginOvertime;
				_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc,_countof(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc),m_sDVJsonDebit.DebitStatusDesc);

				CString strTempCode = GetNetPayRecordMatchCode();
				_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode,_countof(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode),strTempCode.GetBuffer(0));strTempCode.ReleaseBuffer();
				_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier,_countof(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier),m_sDVJsonDebit.PayIdentifier);
				_tcscpy_s(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder,_countof(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder),m_sDVJsonDebit.DebitOrderNumber);
				//控制显示缴费成功交互
//#ifdef _DEBUG
//				iMMIDetailType = 0x05;
//				m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)iMMIDetailType,(LPARAM)0x00);
//#else
				m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)0x01,(LPARAM)0x00);
//#endif
				//记录日志
				strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:扣费成功,已向调用者发送通知"),strResponseType);
			}
			else
			{
				//扣费查询失败（超时返回），可以继续等待，暂时不处理
				if( RCC_DEBIT_QUERY_ERROR==byExecuteState )
				{
				}
				else
				{
					//控制显示缴费失败交互
					if(GetGlobalApp()->m_bIsExternMMIDetail)
					{
						m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)iMMIDetailType,(LPARAM)0x00);
					}
					else
					{
						m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)0x02,(LPARAM)0x00);
					}
					//标识处理超时
					m_dwCheckDebitAccumulateTime = 0x0FFF;
					//记录日志
					strLog.Format(_T("[业务管理线程],响应支付子平台回馈消息(%s),处理状态:扣费失败,已向调用者发送通知"),strResponseType);
				}
			}

			//如果启用银联功能，则在此重置模块状态(1.0.6.8新增)
			if(::GetGlobalApp()->m_bNFCIsUse)
			{
				//重置模块工作状态
				GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
			}

			//扣费查询失败（超时返回），可以继续等待，暂时不处理
			if( RCC_DEBIT_QUERY_ERROR==byExecuteState )
			{
			}
			else
			{
				FinishTrade(TRUE);	//正常结束交易（非超时）
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应支付子平台回馈处理完成(扣款请求/查询)，格式化结果(功能代码:%d,执行状况:%d)"),byFunCode,byExecuteState));
			}
		}
		//保存日志
		GetGlobalApp()->RecordLog(strLog);
	}
	//清理资源
	if(NULL != pDVJsonBase)
	{
		delete pDVJsonBase;
		pDVJsonBase = NULL;
	}

	if(NULL != pDVLocalDebit)
	{
		delete pDVLocalDebit;
		pDVLocalDebit = NULL;
	}

	//扣费查询失败（超时返回），可以继续等待，暂时不处理
	if( RCC_DEBIT_QUERY_ERROR==byExecuteState )
	{
	}
	else
	{
		//通知调用者返回结果
		::GetGlobalApp()->NotifyCaller(byFunCode,byExecuteState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgCheckDebitPasswordState
//     
// 功能描述：查询用户输入密码的扣款结果状态
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgCheckDebitPasswordState(WPARAM wParam,LPARAM lParam)
{
	//进行扣费查询准备(6秒,5秒,4秒,3秒,2秒）查询
	switch(m_dwCheckDebitSpanTime)
	{
	case 0:
		m_dwCheckDebitSpanTime = 6000;
		break;
	case 6000:
		m_dwCheckDebitSpanTime = 5000;
		break;
	case 5000:
		m_dwCheckDebitSpanTime = 4000;
		break;
	case 4000:
		m_dwCheckDebitSpanTime = 3000;
		break;
	case 3000:
		m_dwCheckDebitSpanTime = 2000;
		break;
	case 2000:
		m_dwCheckDebitSpanTime = 2000;
		break;
	}
	//设置定时器
	StartTimer(TIMER_ID_DEBIT_CHECK,m_dwCheckDebitSpanTime);
	//记录日志
	GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程],响应支付管理线程的支付状态查询消息,进行定时器设置，定时时间 %d 毫秒"),m_dwCheckDebitSpanTime));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgRevNFCSignDataFromHTTP
//     
// 功能描述：获取从HTTP回馈的签到数据
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgRevNFCSignDataFromHTTP(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	CString strRevSignData;
	CString strResponseType;
	//获得请求数据的类型
	WORD wOpType  = LOWORD(lParam);
	//获得服务器回馈的结果
	WORD wOpState = HIWORD(lParam);

	if( HTTP_RC_DATA_LEGAL == wOpState)//签到数据回馈成功
	{
		//检查回馈指针是否正常
		CDVJsonBase* pDVJsonBaseTemp = (CDVJsonBase*)wParam;
		if(NULL != pDVJsonBaseTemp)
		{
			//解析并获取NFC签到回馈把报文
			strRevSignData = ((CDVJsonNFCSignIn*)pDVJsonBaseTemp)->NFCSignData;
			//清理资源
			delete pDVJsonBaseTemp;
			pDVJsonBaseTemp = NULL;
			//处理NFC签到回馈报文
			NFCSignInDataProcessFromHTTP(strRevSignData);
		}
	}
	else
	{
		//签到交互失败
		strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统检测到签到过程当中,云端交互回馈失败（第%d次交互，与上次数据交互间隔超过%d秒），判定签到失败，即将开始重新签到流程"),GetGlobalApp()->m_iNFCSignFlowDataCount,GetGlobalApp()->m_iNFCSignDataLimitSpanTime);
		//设置签到失败状态，进行下次重签准备
		GetGlobalApp()->m_bNFCSignInSuccess = false;
		GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
		//重设上次签到数据
		GetGlobalApp()->m_tNFCLastSignDataTime = time(NULL);
		//记录日志
		GetGlobalApp()->RecordLog(strLog);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::OnMsgNFCStartSignInFlow
//     
// 功能描述：开始启动签到流程
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::OnMsgNFCStartSignInFlow(WPARAM wParam,LPARAM lParam)
{
	try
	{
		if(NULL == m_pThreadDevice)
			throw -1;

		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -2;
		//设置签到状态为假
		GetGlobalApp()->m_bNFCSignInSuccess = false;
		//设置签到交互次数为0
		GetGlobalApp()->m_iNFCSignFlowDataCount = 0;
		//发送链路测试（复位）指令
		NFCSendDefaultCmdToPos(0x01);
		//设置等待POS返回超时
//		StartTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA,10000);
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

/****************************************内部辅助函数********************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::fnReleaseResource
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

		//清除扣款状态查询结构体
		if(NULL == m_pDVJsonDebitCheck)
		{
			delete m_pDVJsonDebitCheck;
			m_pDVJsonDebitCheck = NULL;
		}
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


		//CThreadUpdate*		m_pThreadUpdate;
		if(NULL != m_pThreadUpdate)
		{
			//设置该线程推出标记为真
			m_pThreadUpdate->ThrQuit();
			//沉默50毫秒
			::Sleep(50);

			//每次等待1000毫秒，重试次数不超过10次
			while(OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadUpdate->m_hThread, 1000))
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

			strLog.Format(_T("[业务管理线程],%s"),GetGlobalApp()->FmtStr(_T("【播报更新线程】已经停止(重试次数 %d)..."),iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);
		}
		::Sleep(100);


		iHaveTryCount = 0;
		//停止智能硬件控制线程
		if(NULL != m_pThreadDevice)
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

			strLog.Format(_T("[业务管理线程],%s"),GetGlobalApp()->FmtStr(_T("【设备控制线程】已经停止(重试次数 %d)..."),iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);
		}
		::Sleep(dwSleepTime);

		iHaveTryCount = 0;
		//停止支付子平台通讯线程
		if(NULL != m_pThreadHttp)
		{
			//设置该线程推出标记为真
			m_pThreadHttp->bLoopBreak = TRUE;	//需要跳出扣费执行循环
			m_pThreadHttp->m_bExit = TRUE;
			//沉默50毫秒
			::Sleep(50);

			//每次等待1000毫秒，重试次数不超过10次
			while(OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadHttp->m_hThread,1000))
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

			strLog.Format(_T("[业务管理线程],%s"),GetGlobalApp()->FmtStr(_T("【支付通讯线程】已经停止(重试次数 %d)..."),iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);
		}

		iHaveTryCount = 0;
		//停止NFC签到通讯线程
		if(NULL != m_pThreadNFCSignIn)
		{
			//设置该线程推出标记为真
			m_pThreadNFCSignIn->m_bExit = TRUE;
			//沉默50毫秒
			::Sleep(50);

			//每次等待1000毫秒，重试次数不超过10次
			while(OP_SUCCESS != GetGlobalApp()->fnCloseThread(m_pThreadNFCSignIn->m_hThread,1000))
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

			strLog.Format(_T("[业务管理线程],%s"),GetGlobalApp()->FmtStr(_T("【NFC签到线程】已经停止(重试次数 %d)..."),iHaveTryCount));
			//记录日志
			GetGlobalApp()->RecordLog(strLog,LOG_LEVEL_RUN,TRUE);
		}
	}
	catch(int& iErrorCode)
	{
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::ContructDebitMoneyInfo
//     
// 功能描述：构造发送到支付子平台的数据
//     
// 输入参数：IN const tagAccountInfo* pAccountInfo -- 智能硬件获取到的用户信息
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadManage::ContructDebitMoneyInfo(IN const tagAccountInfo* pAccountInfo)
{
	BOOL bReturn = FALSE;
	try
	{
		if(NULL == pAccountInfo)
			throw -1;
		//填充扣款信息结构体
		CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();
		if(NULL == pDVJsonDebit)
			throw -2;
		CString strPayId = GetGlobalApp()->GetGuidString(1);
		CString strTemp;
		
		//标识是否测试数据
		pDVJsonDebit->TestingData			= ::GetGlobalApp()->m_bIsTestData;
		//获取软件版本号
		pDVJsonDebit->NetPayDllVersion		= ::GetGlobalApp()->m_strNetPayDllVersion;
		pDVJsonDebit->DeviceDllVersion		= ::GetGlobalApp()->m_strDeviceDllVersion;
		pDVJsonDebit->TWProductSerialNo		= ::GetGlobalApp()->m_strTWProductSerialNo;

		//填充报文头信息
		pDVJsonDebit->Version				= _T("1.5");
		pDVJsonDebit->PackageType			= JSON_DT_SEND_DIBIT;
		pDVJsonDebit->Token					= _T("");
		pDVJsonDebit->ProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonDebit->SubmitCount			= 0x01;
		pDVJsonDebit->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonDebit->m_hCallerWnd			= NULL;
		pDVJsonDebit->RecordMatchCode		= GetNetPayRecordMatchCode();
		pDVJsonDebit->TradeSecurityCode		= _T("");
		pDVJsonDebit->PayIdentifier			= GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;

		//填充基本信息
		pDVJsonDebit->QRCodeData				= GetGlobalApp()->FmtStr(_T("%s"),pAccountInfo->DataContext);
		pDVJsonDebit->QRCodeType				= 0x00;
		pDVJsonDebit->IDBindInPayPlatform		= _T("");
		pDVJsonDebit->IDBindInPayPlatformType	= 0x00;
		pDVJsonDebit->DebitType					= JSON_CMD_TYPE_DEBIT_ONLY_PAY;

#ifdef _DEBUG
		if(0 != GetGlobalApp()->m_pCurrentDebitInfo.Money)
		{
			pDVJsonDebit->DebitMoney				= 1;
		}
#else
		pDVJsonDebit->DebitMoney				= GetGlobalApp()->m_pCurrentDebitInfo.Money;
#endif
		pDVJsonDebit->DebitTime					= GetGlobalApp()->m_pCurrentDebitInfo.SubTime;
		pDVJsonDebit->DebitTollDate				= GetGlobalApp()->m_pCurrentDebitInfo.TollDate;
		pDVJsonDebit->DebitTollShift			= GetGlobalApp()->m_pCurrentDebitInfo.ShiftID;
		pDVJsonDebit->AccountInfoSrcType		= LOWORD(pAccountInfo->DataType);
		pDVJsonDebit->AccountInfoDeviceID		= HIWORD(pAccountInfo->DataType);
		pDVJsonDebit->AccountInfoDataType		= 0x01;
		
		//填充附加信息
		pDVJsonDebit->EntryProvinceID		= m_sTranstParam.ProvinceID;
		pDVJsonDebit->EntryAreaID			= GetGlobalApp()->m_pCurrentDebitInfo.EntryAreaID;
		pDVJsonDebit->EntryRoadID			= GetGlobalApp()->m_pCurrentDebitInfo.EntryRoadID;
		pDVJsonDebit->EntryStationID		= GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID;
		pDVJsonDebit->EntryStationName		= GetGlobalApp()->m_pCurrentDebitInfo.EntryStationName;
		pDVJsonDebit->EntryLane				= GetGlobalApp()->m_pCurrentDebitInfo.EntryLane;
		pDVJsonDebit->EntryTime				= GetGlobalApp()->m_pCurrentDebitInfo.EntryTime;
		if(pDVJsonDebit->EntryStationName.IsEmpty())
		{
			pDVJsonDebit->EntryStationName.Format(_T("%s"),pDVJsonDebit->EntryStationID);
		}

		pDVJsonDebit->PayProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonDebit->PayStationID			= m_sTranstParam.LaneConfigInfo.StationID;
		pDVJsonDebit->PayStationName		= m_sTranstParam.LaneConfigInfo.StationTab;
		pDVJsonDebit->PayLane				= m_sTranstParam.LaneConfigInfo.LaneID;
		pDVJsonDebit->PayRoadID				= m_sTranstParam.LaneConfigInfo.RoadID;
		
		//新增收费员信息(版本 1.0.3.4)
		pDVJsonDebit->PayOperatorID			= GetGlobalApp()->m_pCurrentDebitInfo.OperatorID;
		pDVJsonDebit->PayOperatorName		= GetGlobalApp()->m_pCurrentDebitInfo.OperatorName;

		pDVJsonDebit->VehicleClass			= GetGlobalApp()->m_pCurrentDebitInfo.VehicleClass;
		pDVJsonDebit->VehicleType			= GetGlobalApp()->m_pCurrentDebitInfo.VehicleType;
		pDVJsonDebit->VehicleLicense		= GetGlobalApp()->m_pCurrentDebitInfo.VehicleLicense;
		pDVJsonDebit->VehicleColor			= GetGlobalApp()->m_pCurrentDebitInfo.VLColor;
		pDVJsonDebit->AxisCount				= GetGlobalApp()->m_pCurrentDebitInfo.AxisCount;
		pDVJsonDebit->TotalWeight			= GetGlobalApp()->m_pCurrentDebitInfo.TotalWeight;
		pDVJsonDebit->LimitWeight			= GetGlobalApp()->m_pCurrentDebitInfo.LimitWeight;

		//新增NFC扣费请求信息
		pDVJsonDebit->NFCTerminal			= GetGlobalApp()->m_strNFCTerminal;
		pDVJsonDebit->NFCSerialNo			= GetGlobalApp()->m_strNFCSerialNo;
		pDVJsonDebit->NFCPayData			= GetGlobalApp()->m_pCurrentDebitInfo.NFCPayData;
		pDVJsonDebit->NFCRevData			= GetGlobalApp()->m_pCurrentDebitInfo.NFCRevData;

		//发送扣款请求
		m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebit,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT,MAKEWORD(0x01,0x00)));
		//标识处理超时
		m_dwCheckDebitAccumulateTime = 0x00;
		//标记成功
		bReturn = TRUE;
		
		//清除变量
		GetGlobalApp()->m_pCurrentDebitInfo.NFCPayData = _T("");
		GetGlobalApp()->m_pCurrentDebitInfo.NFCRevData = _T("");
	}
	catch(int& iErrorCode)
	{

	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::ContructDebitMoneyInfo
//     
// 功能描述：构造发送到支付子平台的数据
//     
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年8月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadManage::ContructDebitMoneyInfo(void)
{
	BOOL bReturn = FALSE;
	try
	{
		//填充扣款信息结构体
		CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();
		if(NULL == pDVJsonDebit)
			throw -1;
		CString strPayId = GetGlobalApp()->GetGuidString(1);
		CString strTemp;

		//标识是否测试数据
		pDVJsonDebit->TestingData			= ::GetGlobalApp()->m_bIsTestData;
		//获取软件版本号
		pDVJsonDebit->NetPayDllVersion		= ::GetGlobalApp()->m_strNetPayDllVersion;
		pDVJsonDebit->DeviceDllVersion		= ::GetGlobalApp()->m_strDeviceDllVersion;
		pDVJsonDebit->TWProductSerialNo		= ::GetGlobalApp()->m_strTWProductSerialNo;

		//填充报文头信息
		pDVJsonDebit->Version				= _T("1.5");
		pDVJsonDebit->PackageType			= JSON_DT_SEND_DIBIT;
		pDVJsonDebit->Token					= _T("");
		pDVJsonDebit->ProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonDebit->SubmitCount			= 0x01;
		pDVJsonDebit->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonDebit->m_hCallerWnd			= NULL;
		pDVJsonDebit->RecordMatchCode		= GetNetPayRecordMatchCode();
		pDVJsonDebit->TradeSecurityCode		= _T("");
		pDVJsonDebit->PayIdentifier			= GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;

		//填充基本信息
		pDVJsonDebit->QRCodeData				= GetGlobalApp()->m_pCurrentDebitInfo.PayCertificateCode;
		pDVJsonDebit->QRCodeType				= 0x00;
		pDVJsonDebit->IDBindInPayPlatform		= _T("");
		pDVJsonDebit->IDBindInPayPlatformType	= 0x00;
		pDVJsonDebit->DebitType					= JSON_CMD_TYPE_DEBIT_ONLY_PAY;

#ifdef _DEBUG
		if(0 != GetGlobalApp()->m_pCurrentDebitInfo.Money)
		{
			pDVJsonDebit->DebitMoney				= 1;
		}
#else
		pDVJsonDebit->DebitMoney				= GetGlobalApp()->m_pCurrentDebitInfo.Money;
#endif
		pDVJsonDebit->DebitTime					= GetGlobalApp()->m_pCurrentDebitInfo.SubTime;
		pDVJsonDebit->DebitTollDate				= GetGlobalApp()->m_pCurrentDebitInfo.TollDate;
		pDVJsonDebit->DebitTollShift			= GetGlobalApp()->m_pCurrentDebitInfo.ShiftID;
		pDVJsonDebit->AccountInfoSrcType		= LOWORD(GetGlobalApp()->m_pCurrentDebitInfo.PayCertificateType);
		pDVJsonDebit->AccountInfoDeviceID		= HIWORD(GetGlobalApp()->m_pCurrentDebitInfo.PayCertificateType);
		pDVJsonDebit->AccountInfoDataType		= 0x01;
		
		//填充附加信息
		pDVJsonDebit->EntryProvinceID		= m_sTranstParam.ProvinceID;
		pDVJsonDebit->EntryAreaID			= GetGlobalApp()->m_pCurrentDebitInfo.EntryAreaID;
		pDVJsonDebit->EntryRoadID			= GetGlobalApp()->m_pCurrentDebitInfo.EntryRoadID;
		pDVJsonDebit->EntryStationID		= GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID;
		pDVJsonDebit->EntryStationName		= GetGlobalApp()->m_pCurrentDebitInfo.EntryStationName;
		pDVJsonDebit->EntryLane				= GetGlobalApp()->m_pCurrentDebitInfo.EntryLane;
		pDVJsonDebit->EntryTime				= GetGlobalApp()->m_pCurrentDebitInfo.EntryTime;
		if(pDVJsonDebit->EntryStationName.IsEmpty())
		{
			pDVJsonDebit->EntryStationName.Format(_T("%s"),pDVJsonDebit->EntryStationID);
		}
		pDVJsonDebit->EntryRoadID			= GetGlobalApp()->m_pCurrentDebitInfo.EntryRoadID;


		pDVJsonDebit->PayProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonDebit->PayStationID			= m_sTranstParam.LaneConfigInfo.StationID;
		pDVJsonDebit->PayStationName		= m_sTranstParam.LaneConfigInfo.StationTab;
		pDVJsonDebit->PayLane				= m_sTranstParam.LaneConfigInfo.LaneID;
		pDVJsonDebit->PayRoadID				= m_sTranstParam.LaneConfigInfo.RoadID;
		//新增收费员信息(版本 1.0.3.4)
		pDVJsonDebit->PayOperatorID			= GetGlobalApp()->m_pCurrentDebitInfo.OperatorID;
		pDVJsonDebit->PayOperatorName		= GetGlobalApp()->m_pCurrentDebitInfo.OperatorName;

		pDVJsonDebit->VehicleClass			= GetGlobalApp()->m_pCurrentDebitInfo.VehicleClass;
		pDVJsonDebit->VehicleType			= GetGlobalApp()->m_pCurrentDebitInfo.VehicleType;
		pDVJsonDebit->VehicleLicense		= GetGlobalApp()->m_pCurrentDebitInfo.VehicleLicense;
		pDVJsonDebit->VehicleColor			= GetGlobalApp()->m_pCurrentDebitInfo.VLColor;
		pDVJsonDebit->AxisCount				= GetGlobalApp()->m_pCurrentDebitInfo.AxisCount;
		pDVJsonDebit->TotalWeight			= GetGlobalApp()->m_pCurrentDebitInfo.TotalWeight;
		pDVJsonDebit->LimitWeight			= GetGlobalApp()->m_pCurrentDebitInfo.LimitWeight;
		
		//新增NFC扣费请求信息
		pDVJsonDebit->NFCTerminal			= GetGlobalApp()->m_strNFCTerminal;
		pDVJsonDebit->NFCSerialNo			= GetGlobalApp()->m_strNFCSerialNo;
		pDVJsonDebit->NFCPayData			= GetGlobalApp()->m_pCurrentDebitInfo.NFCPayData;
		pDVJsonDebit->NFCRevData			= GetGlobalApp()->m_pCurrentDebitInfo.NFCRevData;

		//发送扣款请求
		m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebit,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT,MAKEWORD(0x01,0x00)));
		//标识处理超时
		m_dwCheckDebitAccumulateTime = 0x00;
		//标记成功
		bReturn = TRUE;

		//清除变量
		GetGlobalApp()->m_pCurrentDebitInfo.NFCPayData = _T("");
		GetGlobalApp()->m_pCurrentDebitInfo.NFCRevData = _T("");
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::ContructDebitCancelInfo
//     
// 功能描述：构造撤单数据发送到支付子平台
//     
// 输入参数：IN const CString& strPayIdentifier
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadManage::ContructDebitCancelInfo(IN const CString& strPayIdentifier)
{
	BOOL bReturn = FALSE;
	try
	{
		//填充扣款信息结构体
		CDVJsonDebitCancel* pDVJsonDebitCancel = new CDVJsonDebitCancel();
		if(NULL == pDVJsonDebitCancel)
			throw -1;
		CString strPayId = GetGlobalApp()->GetGuidString(1);
		CString strTemp;
		//填充报文头信息
		pDVJsonDebitCancel->Version				= _T("1.5");
		pDVJsonDebitCancel->PackageType			= JSON_DT_SEND_DIBIT_CANCEL;
		pDVJsonDebitCancel->Token				= _T("");
		pDVJsonDebitCancel->ProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonDebitCancel->SubmitCount			= 0x01;
		pDVJsonDebitCancel->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonDebitCancel->m_hCallerWnd		= NULL;
		pDVJsonDebitCancel->PayIdentifier		= strPayIdentifier;
		((CDVJsonBase*)pDVJsonDebitCancel)->PayIdentifier = strPayIdentifier;
		//发送扣款请求
		m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebitCancel,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT_CANCEL,MAKEWORD(0x01,0x00)));
		//标记成功
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}

	return bReturn;
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
// 1.入口站号	（7/10/20位字符)
// 2.入口时间	（14位字符)
// 3.出口站号	（7/10/20位字符)
// 4.出口时间	（14位字符)
// 5.出口车道号	（3/8位字符）
// 6.虚拟卡号	（20位字符）
// 7.交易金额	（10位字符）
// 使用原则
// 根据序号拼接成一个字符串S，对S进行AES加密（使用密钥K）得到密文D（16进制字符串），
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
		case 11:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_11);
			break;
		case 12:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_12);
			break;
		case 13:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_13);
			break;
		case 14:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_14);
			break;
		case 15:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_15);
			break;

		case 31:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_31);
			break;
		case 32:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_32);
			break;
		case 33:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_33);
			break;
		case 34:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_34);
			break;
		case 35:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_35);
			break;
		case 36:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_36);
			break;
		case 37:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_37);
			break;

		case 41:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_41);
			break;
		case 42:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_42);
			break;
		case 43:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_43);
			break;
		case 44:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_44);
			break;
		case 45:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_45);
			break;

		case 50:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_50);
			break;
		case 51:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_51);
			break;
		case 52:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_52);
			break;
		case 53:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_53);
			break;
		case 54:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_54);
			break;

		case 61:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_61);
			break;
		case 62:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_62);
			break;
		case 63:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_63);
			break;
		case 64:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_64);
			break;
		case 65:
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA_65);
			break;
		default:
			//默认密钥
			strRealKey.Format(_T("%s"),AES_ENC_KEY_DATA);
			break;
		}
	}
	/**************************构造加密明文**************************/
	CString strData;
	//入口站点
	CString strEntryST;
	CString strTempEnST(_T("00000000000000000000"));
	int iSTLen = _tcslen(GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID);
	if(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
	{
		if(iSTLen<7)
		{
			strEntryST.Format(_T("%s%s"),strTempEnST.Left(7-iSTLen),GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID);
		}
		else
		{
			strEntryST.Format(_T("%s"),GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID);
		}
	}
	else
	{
		if(iSTLen<10)
		{
			strEntryST.Format(_T("%s%s"),strTempEnST.Left(10-iSTLen),GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID);
		}
		else
		{
			strEntryST.Format(_T("%s%s"),strTempEnST.Left(20-iSTLen),GetGlobalApp()->m_pCurrentDebitInfo.EntryStationID);
		}
	}

	//入口时间
	CString strEntryTime;
	CTime cEntryTime(GetGlobalApp()->m_pCurrentDebitInfo.EntryTime);
	strEntryTime.Format(_T("%04d%02d%02d%02d%02d%02d"),cEntryTime.GetYear(),cEntryTime.GetMonth(),cEntryTime.GetDay(),cEntryTime.GetHour(),cEntryTime.GetMinute(),cEntryTime.GetSecond());
	
	//出口站点
	CString strExitST;
	iSTLen = _tcslen(GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
	if(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
	{
		if(iSTLen<7)
		{
			strExitST.Format(_T("%s%s"),strTempEnST.Left(7-iSTLen),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
		}
		else
		{
			strExitST.Format(_T("%s"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
		}
	}
	else
	{
		if(iSTLen<10)
		{
			strExitST.Format(_T("%s%s"),strTempEnST.Left(10-iSTLen),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
		}
		else
		{
			strExitST.Format(_T("%s%s"),strTempEnST.Left(20-iSTLen),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
		}
	}
	
	//出口时间
	CString strExitTime;
	CTime cExitTime(GetGlobalApp()->m_pCurrentDebitInfo.SubTime);
	strExitTime.Format(_T("%04d%02d%02d%02d%02d%02d"),cExitTime.GetYear(),cExitTime.GetMonth(),cExitTime.GetDay(),cExitTime.GetHour(),cExitTime.GetMinute(),cExitTime.GetSecond());

	//出口车道
	CString strExitLane;
	int iLaneLen = _tcslen(GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
	if(   (44 == GetGlobalApp()->m_sTranstParam.ProvinceID)
		||(61 == GetGlobalApp()->m_sTranstParam.ProvinceID)
		||(62 == GetGlobalApp()->m_sTranstParam.ProvinceID)
		||(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
		)
	{
		if(iLaneLen<3)
		{
			strExitLane.Format(_T("%s%s"),strTempEnST.Left(3-iLaneLen),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
		}
		else
		{
			strExitLane.Format(_T("%s"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
		}
	}
	else
	{
		if(iLaneLen<8)
		{
			strExitLane.Format(_T("%s%s"),strTempEnST.Left(8-iLaneLen),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
		}
		else
		{
			strExitLane.Format(_T("%s"),GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
		}
	}

	//虚拟卡号
	CString strVirtualCardID;

	if(   (44 == GetGlobalApp()->m_sTranstParam.ProvinceID)
		||(62 == GetGlobalApp()->m_sTranstParam.ProvinceID)
		)
	{
		strVirtualCardID.Format(_T("%s"),_T("00000000000000000000"));
	}
	else
	{
		if(GetGlobalApp()->m_pCurrentDebitInfo.CardID.IsEmpty())
		{
			strVirtualCardID.Format(_T("%s"),_T("00000000000000000000"));
		}
		else
		{
			CString strCardTemp;
			strCardTemp.Format(_T("%s"),_T("00000000000000000000"));
			if(GetGlobalApp()->m_pCurrentDebitInfo.CardID.GetLength()<20)
			{
				int iRemainCount = 20 - GetGlobalApp()->m_pCurrentDebitInfo.CardID.GetLength();
				strVirtualCardID.Format(_T("%s%s"),strCardTemp.Left(iRemainCount),GetGlobalApp()->m_pCurrentDebitInfo.CardID);
			}
			else
			{
				CString strVT(GetGlobalApp()->m_pCurrentDebitInfo.CardID);
				strVT.TrimLeft();
				strVT.TrimRight();
				strVirtualCardID = strVT;
			}
		}
	}

	//交易金额
	CString strTradeMoney;
	strTradeMoney.Format(_T("%010d"),GetGlobalApp()->m_pCurrentDebitInfo.Money);

	strData.Format(_T("%s%s%s%s%s%s%s"),strEntryST,strEntryTime,strExitST,strExitTime,strExitLane,strVirtualCardID,strTradeMoney);

	/**************************加密处理**************************/
	
	//AES加密
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
		/*******************启动相应线程*******************/
		CString strConfigFilePath;
		//启动支付子平台通讯线程
		m_pThreadHttp = (CThreadHttp *)::AfxBeginThread(RUNTIME_CLASS(CThreadHttp),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadHttp)
			throw -1;
		tagTranstParam sTranstParam = {0};
		memcpy(&sTranstParam,&m_sTranstParam,sizeof(tagTranstParam));
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT;
		strConfigFilePath.Format(_T("%s\\%s"),GetGlobalApp()->m_strWorkDir,FILE_NAME_HTTP_CONFIG);
		m_pThreadHttp->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadHttp->SetParam(&sTranstParam,NULL);
		//将该线程重新唤醒
		m_pThreadHttp->ResumeThread();

		//沉默200毫秒
		Sleep(200);

		//启动硬件控制线程
		m_pThreadDevice = (CThreadDevice *)::AfxBeginThread(RUNTIME_CLASS(CThreadDevice),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadDevice)
			throw -2;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_DEVICE_NOTIFY;
		strConfigFilePath.Format(_T("%s\\%s"),GetGlobalApp()->m_strWorkDir,FILE_NAME_DEVICE_CONFIG);
		m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadDevice->SetParam(&sTranstParam,NULL);
		//将该线程重新唤醒
		m_pThreadDevice->ResumeThread();
		//沉默200毫秒
		Sleep(200);

		m_pThreadUpdate = (CThreadUpdate *)::AfxBeginThread(RUNTIME_CLASS(CThreadUpdate),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadUpdate)
			throw -3;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = WM_THREAD_UPDATE_RESPONSE;
		strConfigFilePath.Format(_T("%s\\%s"),GetGlobalApp()->m_strWorkDir, FILE_NAME_UPDATE_CONFIG);
		m_pThreadUpdate->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadUpdate->SetParam(&sTranstParam,NULL);
		m_pThreadUpdate->ResumeThread();
		//沉默200毫秒
		::Sleep(200);

		//启动银联签到线程
		m_pThreadNFCSignIn = (CThreadNFCSignIn *)::AfxBeginThread(RUNTIME_CLASS(CThreadNFCSignIn),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		if(NULL == m_pThreadNFCSignIn)
			throw -2;
		sTranstParam.CallerThreadID    = m_nThreadID;
		sTranstParam.ActiveNotifyMsgID = 0x00;
		strConfigFilePath.Format(_T("%s\\%s"),GetGlobalApp()->m_strWorkDir,FILE_NAME_APP_CONFIG);
		m_pThreadNFCSignIn->SetLocalConfigFilePath(strConfigFilePath);
		m_pThreadNFCSignIn->SetParam(&sTranstParam,NULL);
		//将该线程重新唤醒
		m_pThreadNFCSignIn->ResumeThread();
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
		case -3:
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
	case TIMER_ID_SCAN_CODE://获取用户信息定时器
		{
			if(0 == m_nTimerIDForScanCode)
			{
				TIMECAPS   tc; 
				//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//分辨率的值不能超出系统的取值范围     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
					//调用timeBeginPeriod函数设置定时器的分辨率     
					timeBeginPeriod(m_nAccuracy);
					//设置获取用户信息定时器
					m_nTimerIDForScanCode = timeSetEvent(
						dwDelayTime,						//延时(MS)
						m_nAccuracy,						//延迟分辨率
						(LPTIMECALLBACK)OneMilliSecondProc,	//回调函数
						(DWORD)this,						//使得回调函数有机会处理给定的类的数据
						TIME_ONESHOT);						//只调用一次(TIME_PERIODIC:周期调用)
				}
			}
		}
		break;
	case TIMER_ID_DEBIT_CHECK://扣款状态查询定时器
		{
			if(0 == m_nTimerIDForCheckDebitState)
			{
				TIMECAPS   tc; 
				//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//分辨率的值不能超出系统的取值范围     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
					//调用timeBeginPeriod函数设置定时器的分辨率     
					timeBeginPeriod(m_nAccuracy);
					//设置扣款状态查询定时器
					m_nTimerIDForCheckDebitState = timeSetEvent(
						dwDelayTime,						//延时(MS)
						m_nAccuracy,						//延迟分辨率
						(LPTIMECALLBACK)OneMilliSecondProc,	//回调函数
						(DWORD)this,						//使得回调函数有机会处理给定的类的数据
						TIME_ONESHOT);						//只调用一次(TIME_PERIODIC:周期调用)
				}
			}
		}
		break;
	case TIMER_ID_NFC_WAIT_CORRECT_DATA://NFC冲正数据获取定时器
		{
			if(0 == m_nTimerIDForNFCGetCorrentData)
			{
				TIMECAPS   tc; 
				//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//分辨率的值不能超出系统的取值范围     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
					//调用timeBeginPeriod函数设置定时器的分辨率     
					timeBeginPeriod(m_nAccuracy);
					//设置NFC冲正数据获取定时器
					m_nTimerIDForNFCGetCorrentData = timeSetEvent(
						dwDelayTime,						//延时(MS)
						m_nAccuracy,						//延迟分辨率
						(LPTIMECALLBACK)OneMilliSecondProc,	//回调函数
						(DWORD)this,						//使得回调函数有机会处理给定的类的数据
						TIME_ONESHOT);						//只调用一次(TIME_PERIODIC:周期调用)
				}
			}
		}
		break;
	case TIMER_ID_NFC_WAIT_NORMAL_DATA://NFC数据（不包括冲正）等待超时获取定时器
		{
			if(0 == m_nTimerIDForNFCGetNormalData)
			{
				TIMECAPS   tc; 
				//利用函数timeGetDevCaps取出系统分辨率的取值范围，如果无错则继续
				if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)     
				{
					//分辨率的值不能超出系统的取值范围     
					m_nAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);     
					//调用timeBeginPeriod函数设置定时器的分辨率     
					timeBeginPeriod(m_nAccuracy);
					//设置NFC冲正数据获取定时器
					m_nTimerIDForNFCGetNormalData = timeSetEvent(
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
	case TIMER_ID_SCAN_CODE://获取用户信息定时器
		{
			if(0 == m_nTimerIDForScanCode)
			{
				timeKillEvent(m_nTimerIDForScanCode);
				m_nTimerIDForScanCode = 0;
			}
		}
		break;
	case TIMER_ID_DEBIT_CHECK://扣款状态查询定时器
		{
			if(0 != m_nTimerIDForCheckDebitState)
			{
				timeKillEvent(m_nTimerIDForCheckDebitState);
				m_nTimerIDForCheckDebitState = 0;
			}
		}
		break;
	case TIMER_ID_NFC_WAIT_NORMAL_DATA://NFC普通数据获取超时截止定时器
		{
			if(0 != m_nTimerIDForNFCGetNormalData)
			{
				timeKillEvent(m_nTimerIDForNFCGetNormalData);
				m_nTimerIDForNFCGetNormalData = 0;
			}
		}
		break;
	case TIMER_ID_NFC_WAIT_CORRECT_DATA://NFC冲正数据获取超时截止定时器
		{
			if(0 != m_nTimerIDForNFCGetCorrentData)
			{
				timeKillEvent(m_nTimerIDForNFCGetCorrentData);
				m_nTimerIDForNFCGetCorrentData = 0;
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
	CString strReturn = GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;
	//停止接收处理支付子平台回馈的数据
	GetGlobalApp()->m_bIsTrading		= FALSE;
	GetGlobalApp()->m_bCurrentDebitMode = FALSE;

	//关闭扣款超时定时器
	StopTimer(TIMER_ID_DEBIT_MONEY);
	//关闭检查扣款状态定时器
	StopTimer(TIMER_ID_DEBIT_CHECK);
	//清除扣款状态查询结构体
	if(NULL != m_pDVJsonDebitCheck)
	{
		delete m_pDVJsonDebitCheck;
		m_pDVJsonDebitCheck = NULL;
	}
	m_sDVJsonDebit.Reset();	
	
	m_pThreadHttp->bLoopBreak = TRUE;	//需要跳出扣费执行循环

	//超时截至导致的交易结束，需要自动发起撤单
	if(!bNormal)
	{
		//清理扣款相关资源
		GetGlobalApp()->m_pCurrentDebitInfo.Valid	= FALSE;
		GetGlobalApp()->m_sCurrentDebitResult.Valid = FALSE;
		//构造撤单数据
		CDVJsonDebitCancel* pDVJsonDebitCancle = new CDVJsonDebitCancel(&m_sDVJsonDebit);
		if(NULL != pDVJsonDebitCancle)
		{
			pDVJsonDebitCancle->ProvinceID	  = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
			pDVJsonDebitCancle->Version		  = _T("1.0");
			pDVJsonDebitCancle->SubmitTime	  = GetGlobalApp()->GetCurrentTimeString(1);
			//设置撤单报文
			pDVJsonDebitCancle->PackageType   = JSON_DT_SEND_DIBIT_CANCEL;
			//获取交易识别码
			pDVJsonDebitCancle->PayIdentifier = GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;
			((CDVJsonBase*)pDVJsonDebitCancle)->PayIdentifier = GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;
			//发送撤单请求
			m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebitCancle,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT_CANCEL,MAKEWORD(0x00,0x00)));
		}
	}
	//标识扣款流程结束
	if(GetGlobalApp()->m_bCurrentDebitMode)
	{
		GetGlobalApp()->m_iDebitTaskNo = 0x00;
	}

	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::FinishTrade
//     
// 功能描述：交易结束的处理
//     
// 输入参数：IN const CDVJsonBase* pDVJsonBase -- 返回交易的基本信息结构指针
// 输出参数：无
// 编写人员：ROCY
// 编写时间：2017年2月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::FinishHadCancelTrade(IN const CDVJsonBase* pDVJsonBase)
{
	//关闭扣款超时定时器
	StopTimer(TIMER_ID_DEBIT_MONEY);
	//关闭检查扣款状态定时器
	StopTimer(TIMER_ID_DEBIT_CHECK);
	//清除扣款状态查询结构体
	if(NULL != m_pDVJsonDebitCheck)
	{
		delete m_pDVJsonDebitCheck;
		m_pDVJsonDebitCheck = NULL;
	}
	m_sDVJsonDebit.Reset();	
	
	if(NULL != pDVJsonBase)
	{
		m_pThreadHttp->bLoopBreak = TRUE;	//需要跳出扣费执行循环

		//构造撤单数据
		CDVJsonDebitCancel* pDVJsonDebitCancle = new CDVJsonDebitCancel(&m_sDVJsonDebit);
		if(NULL != pDVJsonDebitCancle)
		{
			pDVJsonDebitCancle->ProvinceID	  = pDVJsonBase->ProvinceID;
			pDVJsonDebitCancle->Version		  = _T("1.0");
			pDVJsonDebitCancle->SubmitTime	  = GetGlobalApp()->GetCurrentTimeString(1);
			//设置撤单报文
			pDVJsonDebitCancle->PackageType   = JSON_DT_SEND_DIBIT_CANCEL;
			//获取交易识别码
			pDVJsonDebitCancle->PayIdentifier = pDVJsonBase->PayIdentifier;
			((CDVJsonBase*)pDVJsonDebitCancle)->PayIdentifier = pDVJsonBase->PayIdentifier;
			//发送撤单请求
			m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebitCancle,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT_CANCEL,MAKEWORD(0x00,0x00)));
		}

		//标识扣款流程结束
		if(GetGlobalApp()->m_bCurrentDebitMode)
		{
			GetGlobalApp()->m_iDebitTaskNo = 0x00;
		}
	}
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
	if(((CThreadManage*)dwUser)->m_nTimerIDForScanCode == nTimerID)
	{
		//关闭扫码超时计时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_SCAN_CODE);
	}
	else if(((CThreadManage*)dwUser)->m_nTimerIDForDebitMoney == nTimerID)	//调用者指定超时时间已到
	{
		//关闭检查扣款状态定时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_DEBIT_MONEY);

		//交易因为超时截至而完毕
		CString strTradeID = ((CThreadManage*)dwUser)->FinishTrade(FALSE);
		
		//设置交互界面
		((CThreadManage*)dwUser)->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)0x02,(LPARAM)0);

		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:扣款操作超时截至（预定超时,%d 秒），自动调用撤单操作，交易标识:%s"),::GetGlobalApp()->m_pCurrentDebitInfo.OverTime/1000,strTradeID));

		//通知调用者返回结果，当前操作失败
		::GetGlobalApp()->NotifyCaller(0x01,RCC_DEBIT_OT);
	}
	else if(((CThreadManage*)dwUser)->m_nTimerIDForCheckDebitState == nTimerID) //用户输入密码，检测超时
	{
		//关闭检查扣款状态定时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_DEBIT_CHECK);

		//获取查询已经使用的时间
		((CThreadManage*)dwUser)->m_dwCheckDebitAccumulateTime += ((CThreadManage*)dwUser)->m_dwCheckDebitSpanTime;
		
		//判断是否超时
		if(((CThreadManage*)dwUser)->m_dwCheckDebitAccumulateTime >= ((CThreadManage*)dwUser)->m_dwInputPasswordDelayTime)
		{
			//交易因为超时截至而完毕
			CString strTradeID = ((CThreadManage*)dwUser)->FinishTrade(FALSE);
			
			//设置交互界面
			((CThreadManage*)dwUser)->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI,(WPARAM)0x02,(LPARAM)0);
				
			//记录日志
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:扣款操作超时截至（检测累加,%d ），自动调用撤单操作，交易标识:%s"),((CThreadManage*)dwUser)->m_dwCheckDebitAccumulateTime,strTradeID));

			//通知调用者返回结果，当前操作失败
			::GetGlobalApp()->NotifyCaller(0x01,RCC_DEBIT_OT);
		}
		else
		{
			//防止由于支付状态检测结构体指针非法调用，需要增加检测
			if(NULL != ((CThreadManage*)dwUser)->m_pDVJsonDebitCheck )
			{
				CDVJsonDebitCheck* pDVJsonDebitCheck = new CDVJsonDebitCheck(((CThreadManage*)dwUser)->m_pDVJsonDebitCheck);
				if(NULL != pDVJsonDebitCheck)
				{
					pDVJsonDebitCheck->PayIdentifier= GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;
					((CDVJsonBase*)pDVJsonDebitCheck)->PayIdentifier= GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier;
					pDVJsonDebitCheck->PackageType  = JSON_DT_SEND_DIBIT_CHECK;
					pDVJsonDebitCheck->SubmitTime   = GetGlobalApp()->GetCurrentTimeString(1);
					//记录日志
					GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:扣款操作超时未截至（已累加 %d 毫秒），根据规则，自动进行下一次的扣款状态检测，交易标识:%s"),((CThreadManage*)dwUser)->m_dwCheckDebitAccumulateTime,pDVJsonDebitCheck->PayIdentifier));
					((CThreadManage*)dwUser)->m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebitCheck,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT_CHECK,0x01));
				}
				//通知下一次查询
				((CThreadManage*)dwUser)->PostThreadMessage(WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE,(WPARAM)0x00,(LPARAM)0x00);
			}
		}
	}
	else if(((CThreadManage*)dwUser)->m_nTimerIDForNFCGetCorrentData == nTimerID)//NFC支付，获取冲正数据超时截止
	{
		//关闭检查扣款状态定时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_NFC_WAIT_CORRECT_DATA);

		//判断是否允许，没有冲正数据也能够发起NFC扣费（向云端发起请求）
		if(GetGlobalApp()->m_bNFCIsAllowDebitWithoutCancelData)
		{
			//构造扣费数据
			((CThreadManage*)dwUser)->ContructDebitMoneyInfo();
		}
		else
		{
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[业务管理线程]:NFC支付流程，获取冲正数据超时，判定交易失败，交易标识:%s"),GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier));
			//获取超时，交易失败(由于此时没有递交请求到云端，不需要自动撤单）
			((CThreadManage*)dwUser)->FinishTrade(TRUE);
			//通知调用者返回结果，当前操作失败
			::GetGlobalApp()->NotifyCaller(0x01,RCC_DEBIT_OT);
		}
	}
	else if(((CThreadManage*)dwUser)->m_nTimerIDForNFCGetNormalData == nTimerID)//NFC支付，获取普通数据（不含冲正）超时截止
	{
		//关闭检查扣款状态定时器
		((CThreadManage*)dwUser)->StopTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA);

		switch(::GetGlobalApp()->GetUnipayWorkFlowState())
		{
		case UMS_DEBITING://如果当前是消费流程
			//直接判定交易失败，通知调用者
			((CThreadManage*)dwUser)->NFCLocalTradeFailProcess();
			break;
		case UMS_SIGNING://如果当前是签到流程
			//判定签到失败，等待下次重新签到
			{
				//记录日志
				CString strLog;
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，等待签到数据超时截止，签到流程失败(模块数据交互次数:%d)"),GetGlobalApp()->m_iNFCSignFlowDataCount);
				GetGlobalApp()->RecordLog(strLog);
				//设置签到状态为假
				GetGlobalApp()->m_bNFCSignInSuccess = false;
				//重置NFC支付状态
				GetGlobalApp()->m_bNFCPayFail = FALSE;
				GetGlobalApp()->m_iNFCPayFailCount = 0;
				//重置NFC签到交互次数
				GetGlobalApp()->m_iNFCSignFlowDataCount = 0;
				//重置NFC模块当前工作流程
				GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
			}
			break;
		default:		 //其它
			break;
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



////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::NFCSignInDataProcessFromHTTP
//     
// 功能描述：NFC签到数据处理函数（数据从HTTP回传）,转发给POS机具
//     
// 输入参数：IN const CString& strData
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：参见接口协议的函数的参数描述：IssueData
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCSignInDataProcessFromHTTP(IN const CString& strData)
{
	try
	{
		//正常签到数据处理
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -1;
		if(NULL == m_pThreadDevice)
			throw -2;
		CNFCProtocol cNFCProtocol;
		pstrString->Format(_T("%s"),cNFCProtocol.ConstructSignDataFromHttpCmd(strData));

		m_pThreadDevice->PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pstrString,(LPARAM)UMS_SIGNING);
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
// 函数名称：CThreadManage::NFCSignInDataProcessFromPOS
//     
// 功能描述：NFC签到数据处理函数（数据从POS非接模块回传）
//     
// 输入参数：IN CString& strData
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCSignInDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol)
{
	CString strLog;
	//签到流程成功标记
	BOOL bSuccess = FALSE;
	CString* pstrSendData = NULL;
	try
	{
		GetGlobalApp()->m_iNFCSignFlowDataCount = GetGlobalApp()->m_iNFCSignFlowDataCount+1;
		//签到业务处理
		switch(cNFCProtocol.FramrType)
		{
		case UFT_N_SIGN_IN_RES_DATA:
			{
				//首先判断距离上次接收签到数据时间间隔是否超过限制，如果超过限制，直接判定当前签到失败
				if( (time(NULL) - GetGlobalApp()->m_tNFCLastSignDataTime) > GetGlobalApp()->m_iNFCSignDataLimitSpanTime)
				{
					strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统检测到签到过程当中数据交互时间间隔较长（第%d次交互，与上次数据交互间隔超过%d秒），判定签到失败，即将开始重新签到流程"),GetGlobalApp()->m_iNFCSignFlowDataCount,GetGlobalApp()->m_iNFCSignDataLimitSpanTime);
					//设置签到失败状态，进行下次重签准备
					GetGlobalApp()->m_bNFCSignInSuccess = false;
					GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
					//重设上次签到数据
					GetGlobalApp()->m_tNFCLastSignDataTime = time(NULL);
					//签到失败，要求POS强制结束流程
					NFCSendDefaultCmdToPos(0x03);
				}
				else
				{
					if(NULL == m_pThreadNFCSignIn)
						throw -1;
					pstrSendData = new CString();
					if(NULL == pstrSendData)
						throw -2;
					//进行数据转换
					pstrSendData->Format(_T("%04X%s"),cNFCProtocol.Context.GetLength()/2,cNFCProtocol.Context);
					//转发签到数据到云端
					m_pThreadNFCSignIn->PostThreadMessage(WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_POS,(WPARAM)pstrSendData,(LPARAM)0);
					//记录日志
					strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统正在进行签到操作，模块数据交互次数:%d"),GetGlobalApp()->m_iNFCSignFlowDataCount);
					//每次收到POS的签到数据，都需要设置上次签到数据时间，保证超时机制有效
					GetGlobalApp()->m_tNFCLastSignDataTime = time(NULL);
					//设置等待超时
//					StartTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA,25000);
				}
			}
			break;
		case UFT_N_SIGN_CONFIRM:
			{
				//记录日志
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统签到【成功】,耗时 %d 秒(模块数据交互次数:%d)"),(time(NULL) - GetGlobalApp()->m_tNFCStartSignInTime),GetGlobalApp()->m_iNFCSignFlowDataCount);
				GetGlobalApp()->RecordLog(strLog,0,TRUE);
				strLog.Empty();
				//已完成签到，设置相应的控制位
				GetGlobalApp()->m_bNFCSignInSuccess = true;
				//重置NFC支付状态
				GetGlobalApp()->m_bNFCPayFail = FALSE;
				GetGlobalApp()->m_iNFCPayFailCount = 0;
				//重置NFC签到交互次数
				GetGlobalApp()->m_iNFCSignFlowDataCount = 0;
				//判断是否每天重签已经完成（在指定时间点内，如果有过签到成功记录，则认为是每天自动重签完成）
				CTime cCurrentTime = CTime::GetCurrentTime();
				if(    (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iNFCDefaultStartReSignHour) 
					&& (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iNFCDefaultEndReSignHour) )
				{
					GetGlobalApp()->m_bNFCAutoSignIsExecute = TRUE;
				}
				else
				{
					GetGlobalApp()->m_bNFCAutoSignIsExecute = FALSE;
				}
			}
			break;
		case UFT_N_SIGN_VERIFY_ERR:
			{
				//记录日志
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，非接模块回馈数据校验不成功，签到流程失败(模块数据交互次数:%d)"),GetGlobalApp()->m_iNFCSignFlowDataCount);
				//设置签到状态为假
				GetGlobalApp()->m_bNFCSignInSuccess = false;
				//重置NFC支付状态
				GetGlobalApp()->m_bNFCPayFail = FALSE;
				GetGlobalApp()->m_iNFCPayFailCount = 0;
				//重置NFC签到交互次数
				GetGlobalApp()->m_iNFCSignFlowDataCount = 0;
				//签到失败，要求POS强制结束流程
				NFCSendDefaultCmdToPos(0x03);
			}
			break;
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
	//记录日志
	if(!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::NFCConsumeDataProcessFromPOS
//     
// 功能描述：NFC消费数据处理函数
//     
// 输入参数：IN CNFCProtocol& cNFCProtocol
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCConsumeDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol)
{
	CString strLog;
	try
	{
		//资源合法性判读
		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -1;
		if(NULL == m_pThreadDevice)
			throw -2;
		//判断类型
		switch(cNFCProtocol.FramrType)
		{
		case UFT_N_CONSUME_RES_W2:	//银行卡消费类型
			{
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，查询消费类型操作成功(银行卡消费类型)"));
				//构造银行卡消费指令
				pstrString->Format(_T("%s"),cNFCProtocol.ConstructExecuteConsumeCmd(0x01));
				//发送数据到POS模块
				m_pThreadDevice->PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pstrString,(LPARAM)UMS_DEBITING);
				//启动NFC消费数据流水获取等待计时器
//				StartTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA,3000);
			}
			break;
		case UFT_N_CONSUME_RES_W3:	//POS通消费类型
			{
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，查询消费类型操作成功(POS通消费类型)"));
				//构造POS通消费指令
				pstrString->Format(_T("%s"),cNFCProtocol.ConstructExecuteConsumeCmd(0x02));
				//发送数据到POS模块
				m_pThreadDevice->PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pstrString,(LPARAM)UMS_DEBITING);
				//启动NFC消费数据流水获取等待计时器
//				StartTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA,3000);
			}
			break;
		case UFT_N_CONSUME_TIP:		//消费提示
			{
				//仅仅记录日志，不做处理
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，获取刷卡提示回馈，不需要处理"));
			}
			break;
		case UFT_N_CONSUME_RES_DATA://消费数据
			{
				//获取到消费报文，保存当前消费报文（需要增加长度）
				WORD wDataLen = cNFCProtocol.Context.GetLength()/2;
				CString strDataLen;
				strDataLen.Format(_T("%04X"),wDataLen);
				GetGlobalApp()->m_pCurrentDebitInfo.NFCPayData.Format(_T("%s%s"),strDataLen,cNFCProtocol.Context);
				//记录日志
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，成功获取刷卡数据"));
				GetGlobalApp()->RecordLog(strLog);
				//沉默150毫秒
				//Sleep(150);
				//进行冲正数据获取
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，进行冲正数据获取"));
				GetGlobalApp()->RecordLog(strLog);
				strLog.Empty();
				pstrString->Format(_T("%s"),cNFCProtocol.ConstructCorrectCmd());
				//发送数据到POS模块	
				m_pThreadDevice->PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pstrString,(LPARAM)UMS_CORRECT);
				//启动NFC冲正流水数据获取等待计时器
//				StartTimer(TIMER_ID_NFC_WAIT_CORRECT_DATA,GetGlobalApp()->m_iNFCGetCancelDataLimitTime * 1000);
			}
			break;
		case UFT_N_CONSUME_ERR:		//消费数据组装错误（刷卡出错）
			{
				//记录日志
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统无法获取非接模块刷卡消费数据(非接模块错误代码:%02X)，本次交易失败，交易标识:%s,已通知调用者"),cNFCProtocol.CurrentConsumeErrorCode,GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
				//后续处理
				NFCLocalTradeFailProcess(RCC_INNER_ERROR);
			}
			break;
		case UFT_N_CONSUME_NO_SIGN:	//模块未能签到
			{
				//未签到，交易失败
				strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统检查模块未签到成功，需要先进行签到操作，本次交易失败，交易标识:%s,已通知调用者"),GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
				//后续处理
				NFCLocalTradeFailProcess(RCC_INNER_ERROR);				
			}
			break;
		}
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
		}
	}
	//记录日志
	if(!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::NFCCorrectDataProcessFromPOS
//     
// 功能描述：NFC冲正数据处理函数
//     
// 输入参数：IN CNFCProtocol& cNFCProtocol
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCCorrectDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol)
{
	//停止计时器
	StopTimer(TIMER_ID_NFC_WAIT_CORRECT_DATA);
	//记录日志
	CString strLog;
	strLog.Format(_T("[业务管理线程]:NFC非接支付流程，成功获取冲正数据"));
	GetGlobalApp()->RecordLog(strLog);
	strLog.Format(_T("[业务管理线程]:NFC非接支付流程，系统获取NFC消费相关数据成功（包含预冲正信息），递交扣费请求，交易标识:%s"),GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
	GetGlobalApp()->RecordLog(strLog);
	//缓存NFC冲正数据（需要在之前增加长度）
	WORD wDataLen = cNFCProtocol.Context.GetLength()/2;
	CString strDataLen;
	strDataLen.Format(_T("%04X"),wDataLen);
	GetGlobalApp()->m_pCurrentDebitInfo.NFCRevData.Format(_T("%s%s"),strDataLen,cNFCProtocol.Context);
	//构造扣费数据
	CString strTempQRCode = GetGlobalApp()->m_pCurrentDebitInfo.PayCertificateCode;
	GetGlobalApp()->m_pCurrentDebitInfo.PayCertificateCode = _T("111111");

	//停止获取扫码数据，并且不再变换终端屏幕
	m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_STOP_GET_DATA,(WPARAM)0,(LPARAM)0x01);

	ContructDebitMoneyInfo();
	GetGlobalApp()->m_pCurrentDebitInfo.PayCertificateCode = strTempQRCode;
	//重置模块工作状态
	GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
	//后续业务处理(停止POS消费，获取冲正流水)
	strLog.Format(_T("[业务管理线程]:NFC非接支付流程,支付业务数据获取完毕，停止POS端消费操作"));
	GetGlobalApp()->RecordLog(strLog);
	//先下发NAK，停止POS机具消费流程
	NFCSendDefaultCmdToPos(0x03);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::NFCLinkTestDataProcessFromPOS
//     
// 功能描述：NFC测试连接回馈处理函数
//     
// 输入参数：IN CNFCProtocol& cNFCProtocol
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCLinkTestDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol)
{
	//如果是处于签到流程，则进行发起签到数据
	try
	{
		if(NULL == m_pThreadDevice)
			throw -1;

		CString* pstrString = new CString();
		if(NULL == pstrString)
			throw -2;
		//设置签到状态为假
		GetGlobalApp()->m_bNFCSignInSuccess = false;
		//设置签到交互次数为0
		GetGlobalApp()->m_iNFCSignFlowDataCount = 0;
		//获取签到报文
		CNFCProtocol cNFCProtocol;
		pstrString->Format(_T("%s"),cNFCProtocol.ConstructStartSignInCmd());
		//发送签到数据到设备控制线程
		m_pThreadDevice->PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pstrString,(LPARAM)UMS_SIGNING);
		//设置签到数据第一次交互时间
		GetGlobalApp()->m_tNFCLastSignDataTime = time(NULL);
		//设置模块当前工作流程为签到
		GetGlobalApp()->SetUnipayWorkFlowState(UMS_SIGNING);
		//设置等待POS返回超时
//		StartTimer(TIMER_ID_NFC_WAIT_NORMAL_DATA,10000);
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
// 函数名称：CThreadManage::NFCLocalTradeFailProcess
//     
// 功能描述：NFC交易失败处理（数据未递交云端）
//     
// 输入参数：IN const int& iExecuteCode
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCLocalTradeFailProcess(IN const int& iExecuteCode)
{
	GetGlobalApp()->m_bNFCPayFail		= TRUE;
	if(GetGlobalApp()->m_iNFCPayFailCount > 3)
	{
		GetGlobalApp()->m_iNFCPayFailCount = 1;
	}
	else
	{
		GetGlobalApp()->m_iNFCPayFailCount += 1;
	}
	GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
	//交易失败，回馈调用者
	FinishTrade(TRUE);	//正常结束交易（非超时）
	//通知调用者返回结果
	BYTE byFunCode		= 0x01;
	GetGlobalApp()->NotifyCaller(byFunCode,iExecuteCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::NFCProcessErrorFrame
//     
// 功能描述：NFC模块回馈数据帧异常处理
//     
// 输入参数：IN CNFCProtocol& cNFCProtocol
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCProcessErrorFrame(IN CNFCProtocol& cNFCProtocol)
{
	CString strLog;
	switch(::GetGlobalApp()->GetUnipayWorkFlowState())
	{
	case UMS_UNKNOW://不处理
		break;
	case UMS_SIGNING:
		strLog.Format(_T("[业务管理线程]:NFC业务流程，系统获取POS模块回馈数据,收到无法解析的帧，判定本次签到流程失败。"));
		//设置签到失败状态，进行下次重签准备
		GetGlobalApp()->m_bNFCSignInSuccess = false;
		//重设上次签到数据
		GetGlobalApp()->m_tNFCLastSignDataTime = time(NULL);
		break;
	case UMS_DEBITING:
		strLog.Format(_T("[业务管理线程]:NFC业务流程，系统获取POS模块回馈数据,收到无法解析的帧，判定本次交易流程失败。"));
		NFCLocalTradeFailProcess();
		break;
	case UMS_CORRECT:
		strLog.Format(_T("[业务管理线程]:NFC业务流程，系统获取POS模块回馈数据,收到无法解析的帧，判定本次冲正流程失败。"));
		break;
	}
	//重置POS模块当前工作流程状态
	GetGlobalApp()->SetUnipayWorkFlowState(UMS_UNKNOW);
	//记录日志
	if(!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::NFCSendDefaultCmdToPos
//     
// 功能描述：NFC交易流程，发送NAK指令
//     
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadManage::NFCSendDefaultCmdToPos(IN const int& iCmdType)
{
	CString* pstrString = NULL;
	pstrString = new CString();
	WORD wCMDType = UMS_UNKNOW;
	if(NULL != pstrString)
	{
		CNFCProtocol cNFCProtocol;
		
		switch(iCmdType)
		{
		case 0x01:
			wCMDType = UMS_TEST_LINK;
			pstrString->Format(_T("%s"),cNFCProtocol.ConstructTestLinkCmd());
			break;
		case 0x02:
			wCMDType = UMS_SIGNING;
			pstrString->Format(_T("%s"),cNFCProtocol.ConstructStartSignInCmd());
			break;
		case 0x03:
			wCMDType = UMS_STOP;
			pstrString->Format(_T("%s"),cNFCProtocol.ConstructStopExecuteCmd());
			break;
		default:
			wCMDType = UMS_UNKNOW;
			break;
		}
		//发送数据到POS模块	
		m_pThreadDevice->PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pstrString,(LPARAM)wCMDType/*UMS_STOP*/);
	}
	//设置模块当前工作状态为未知
	GetGlobalApp()->SetUnipayWorkFlowState(wCMDType);
}