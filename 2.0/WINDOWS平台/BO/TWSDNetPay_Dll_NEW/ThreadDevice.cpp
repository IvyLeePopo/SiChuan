/*
* Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
*
* 文件名称：ThreadDevice.cpp
* 摘    要：硬件控制线程，实现文件
* 当前版本：1.0.0.1
* 作    者：ROCY
* 创建日期：2016年7月23日
*/
// ThreadDevice.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadDevice.h"


// CThreadDevice

IMPLEMENT_DYNCREATE(CThreadDevice, CThreadBase)

CThreadDevice::CThreadDevice()
	: m_iDeviceVersion(2)
	, m_pHTSDDeviceDll(NULL)
	, m_dwLastCheckState(0)
	, m_bInitConfigDevice(FALSE)
	, m_tLastInit(0)
{
	m_tLastInit = time(NULL) - 20;

	m_pHTSDDeviceDll = new CHTSDDeviceDll2();
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
	return TRUE;
}

int CThreadDevice::ExitInstance()
{

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadDevice, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_SHOW_DEAL_INFO, OnMsgShowDealInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_DEBIT_FLOW, OnMsgStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_DEBIT_CANCEL, OnMsgDebitCancelFlow)
END_MESSAGE_MAP()


// CThreadDevice message handlers

BOOL CThreadDevice::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}
	CString strLog;
	CString strError;
	/*
	if (NULL == m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll = new CHTSDDeviceDll2();
	}
	else 
	*/
	if(NULL!=m_pHTSDDeviceDll)
	{
		//判断是否需要加载组件
		static time_t tLastInitDllTime = 0x00;
		if ( (time(NULL) - tLastInitDllTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDllInitState())
			{
				CString strDllFilePath;
				strDllFilePath.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, "HTTWSDDevice_V2.dll");//FILE_NAME_DEVICE_DLL);
				m_pHTSDDeviceDll->Init(strDllFilePath);
				tLastInitDllTime = time(NULL);
			}
			tLastInitDllTime = time(NULL);//add 2018-01
		}

		//判断是否需要初始化组件
		static time_t tLastInitDevTime = 0x00;
		if ( (time(NULL) - tLastInitDevTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDevInitState())
			{
				if (!m_pHTSDDeviceDll->InitDev(fnGetNotifyFromCommModule))
				{
					m_pHTSDDeviceDll->GetLastErrorDesc(strError);
				}
				tLastInitDevTime = time(NULL);
			}
			tLastInitDllTime = time(NULL);//add 2018-01
		}

		//发送初始化指令，频率:10秒/次
		if (time(NULL) - m_tLastInit > 10)
		{
			//往串口下发设备初始化指令
			if ( !m_bInitConfigDevice )
			{
				m_bInitConfigDevice = InitConfigDevice();
			}

			m_tLastInit = time(NULL);
		}
	}

	//线程存活日志记录检测
	static time_t tLastDeviceLogTime = 0;
	if ( (time(NULL) - tLastDeviceLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[设备控制线程],线程存活日志检测"));
		GetGlobalApp()->RecordLog(strLog);
		//重设检测时间
		tLastDeviceLogTime = time(NULL);
	}

	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::GetDeviceState
//
// 功能描述：获取硬件设备状态
//
// 输入参数：void
// 输出参数：DWORD，按位进行标记，定义如下：0位,驱动加载失败;1,串口打开失败;2,无心跳(通讯故障)
// 编写人员：ROCY
// 编写时间：2016年8月7日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
		//获取硬件当前状态
		if (!m_pHTSDDeviceDll->CheckDeviceStatus(dwState))
			throw - 2;
		
		//获取驱动加载状态
		if (!GetDeviceInitState())
		{
			dwReturnState |= 0x01;
		}
		//获取硬件串口状态
		dwReturnState |= dwState;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			break;
		case -2://add 2017-08
			dwReturnState = 0x000000FF;
			break;
		}
	}
	return dwReturnState;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::OnMsgShowDealInfo
//
// 功能描述：接收到信息，要求执行费显功能
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_SHOW_DEAL_INFO
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgShowDealInfo(WPARAM wParam, LPARAM lParam)
{
	BOOL bSaveError = FALSE;
	BYTE byCmdType		= RDM_SET_MMI_INFO;
	BYTE byExecuteCode	= OP_FAILED;
	try
	{
		CString* pstrInfo = (CString*)wParam;
		if (NULL == pstrInfo)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		CString strData;
		strData.Format(_T("%s"), *pstrInfo);
		//设置显示交互信息
		m_pHTSDDeviceDll->SetDevice(strData);
		//	m_pHTSDDeviceDll->(0x00,strData);

		//清理资源
		delete pstrInfo;
		pstrInfo = NULL;
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
		bSaveError = TRUE;
	}
	//通知业务管理线程已经获取硬件回馈业务数据
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)NULL);
}

/*add 2017-05-19 */
//#include <json.h>
bool TmpFillDebitResult( CString& ptr)
{

	if (ptr.IsEmpty())
	{
		return false;
	}
	Json::Reader reader;
	Json::Value root, context, data, tradeitem, transtime, transplat, securecode, ordercode, payid, consumetime;
	std::string info(ptr.GetBuffer(0));
	if (!reader.parse(info, root))
	{
		return false;
	}
	context = root["DebitResultContext"];
	data = context["Data"];
	tradeitem = data["TradeKeyItem"];
	transtime = tradeitem["DebitTime"];
	if (transtime.type() == Json::nullValue)
	{
		return false;
	}
	GetGlobalApp()->m_sCurrentDebitResult.DebitTime =  GetGlobalApp()->FixUTC( GetGlobalApp()->CString2time_t(transtime.asCString()));
	//add 2018-01 将收到的时间转换为东八区
	transplat = tradeitem["PayPlatformType"];
	if (transplat.type() != Json::stringValue)
	{
		return false;
	}
	GetGlobalApp()->m_sCurrentDebitResult.PayPlatformType = _ttoi(transplat.asCString());

	securecode = tradeitem["TradeSecurityCode"];
	if (securecode.type() != Json::stringValue)
	{
		return false;
	}
	strncpy(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode, securecode.asCString(), 40);

	ordercode = tradeitem["DebitOrder"];
	if (ordercode.type() != Json::stringValue)
	{
		return false;
	}
	strncpy(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder, ordercode.asCString(), 40);

	payid = tradeitem["PayIdentifier"];
	if (payid.type() != Json::stringValue)
	{
		return false;
	}
	strncpy(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier, payid.asCString(), 40);

	consumetime = tradeitem["ConsumeTime"];
	if (payid.type() != Json::stringValue)
	{
		return false;
	}
	GetGlobalApp()->m_sCurrentDebitResult.ConsumeTime = _ttoi(consumetime.asCString());

	strcpy(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc, "成功");
	return true;

}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::OnMsgStartDebitFlow
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
// 备注说明：WM_THREAD_DEVICE_START_DEBIT_FLOW
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgStartDebitFlow(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString* pStrResult = NULL;
	BOOL bSaveError		= FALSE;

	BYTE byCmdType		= RDM_DEBITE_START;
	BYTE byExecuteCode	= OP_FAILED;

	try
	{
		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		int iType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"), *pstrXML);

		pStrResult = new CString();
		if (NULL == pStrResult)
			throw - 4;

		strLog.Format(_T("[设备控制线程],接收到指令，开始扣费流程操作"));
		GetGlobalApp()->RecordLog(strLog);
		//启动扣款流程
		CString strResult;
		if (!m_pHTSDDeviceDll->DebitMoney(strData, strResult, iType))
			throw - 5;
		pStrResult->Format(_T("%s"), strResult);
		strLog.Format(_T("[设备控制线程],扣费结果:%s"),strResult);
		GetGlobalApp()->RecordLog(strLog);
		TmpFillDebitResult(strResult);
		//清理资源
		delete pstrXML;
		pstrXML = NULL;
		strLog.Format(_T("[设备控制线程],扣费流程执行完毕"));
		
		//标识执行成功
		byExecuteCode	= OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[设备控制线程],接收到的参数异常,指针为空"));
			break;
		case -2:
			strLog.Format(_T("[设备控制线程],通讯模块状态异常,指针为空"));
			break;
		case -3:
			strLog.Format(_T("[设备控制线程],通讯模块未能成功初始化"));
			break;
		case -4:
			strLog.Format(_T("[设备控制线程],内部资源申请异常"));
			break;
		case -5:
		{
			CString strError;
			m_pHTSDDeviceDll->GetLastErrorDesc(strError);

			byExecuteCode = 0xFF;
			strLog.Format(_T("[设备控制线程],执行扣费操作失败，错误原因描述：%s"), strError);
			if(strError.Find("操作被取消")!=-1)
			{
				byExecuteCode = OP_CANCLE;
			}
			
			else if (strError.Find("账户余额不足") != -1)
			{
				byExecuteCode = OP_NO_MONEY;
			}
			else if (strError.Find("二维码非法") != -1)
			{
				byExecuteCode = OP_QRCODE_ILLEGAL;
			}
			else if (strError.Find("二维码超出有效期") != -1)
			{
				byExecuteCode = OP_QRCODE_OT;
			}
			else if (strError.Find("支付凭证异常") != -1)
			{
				byExecuteCode = OP_NO_ACCOUNT_INFO;
			}
			else if (strError.Find("扣款超时") != -1)
			{
				byExecuteCode = OP_DEBIT_OT;
			}
			else if (strError.Find("密码错误") != -1)
			{
				byExecuteCode = OP_PSW_ERR;
			}
			else if (strError.Find("参数错误") != -1)
			{
				byExecuteCode = OP_PARAM_ERR;
			}
			else if (strError.Find("上次交易未结束") != -1)
			{
				byExecuteCode = OP_LAST_TRADE_NOT_END;
			}
			else if (strError.Find("组件未初始化") != -1)
			{
				byExecuteCode = OP_COM_NOT_INIT;
			}
			else if (strError.Find("组件校验非法") != -1)
			{
				byExecuteCode = OP_COM_ILLEGAL;
			}
			else if (strError.Find("支付平台网络故障") != -1)
			{
				//byExecuteCode = OP_PAY_PLATFORM_ERR;
			}
			else if( strError.Find("与下位机通讯异常")!=-1)
			{
				//byExecuteCode = OP_DISCONNECT_TERMINAL;
			}
		}
		break;
		}

		bSaveError = TRUE;
	}
	//保存日志
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog, bSaveError ? LOG_LEVEL_ERROR : LOG_LEVEL_RUN);
	}
	//通知业务管理线程已经获取硬件回馈业务数据
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)pStrResult);

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::OnMsgDebitCancelFlow
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
// 备注说明：WM_THREAD_DEVICE_START_DEBIT_FLOW
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgDebitCancelFlow(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString* pStrResult = NULL;
	BOOL bSaveError = FALSE;
	BYTE byCmdType		= RDM_DEBITE_CANCEL;
	BYTE byExecuteCode	= OP_FAILED;
	try
	{
		CString* pStrParam = (CString*)wParam;
		if (NULL == pStrParam)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		int iType = (int)lParam;

		pStrResult = new CString();
		if (NULL == pStrResult)
			throw - 4;

		CString strData;
		strData.Format(_T("%s"), *pStrParam);
		//撤消订单
		CString strResult;
		strLog.Format(_T("[设备控制线程],接收到指令，开始执行撤单操作"));
		//保存日志
		GetGlobalApp()->RecordLog(strLog);
	
		/*
		CString find_key ="\"PayIdentifier\":\"";
		CString find_pay;
		int find_first = strData.Find(find_key);
		if(find_first!=-1)
		{
			int find_last = strData.Find("\"",find_first+find_key.GetLength());
			find_pay = strData.Mid(find_first+find_key.GetLength(),find_last-find_first-find_key.GetLength());
		}
		this->BreakDebit(find_pay);
		*/

		//执行撤单操作
		if (!m_pHTSDDeviceDll->DebitCancel(strData, 0x00))
			throw - 5;
		/*add 2017-05-19*/
		strResult = "0";
		pStrResult->Format(_T("%s"), strResult);
		//清理资源
		delete pStrParam;
		pStrParam = NULL;

		strLog.Format(_T("[设备控制线程],撤单操作执行完毕"));

		//标识执行成功
		byExecuteCode	= OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[设备控制线程],接收到的参数异常,指针为空"));
			break;
		case -2:
			strLog.Format(_T("[设备控制线程],通讯模块动态库异常,指针为空"));
			break;
		case -3:
			strLog.Format(_T("[设备控制线程],获取通讯模块动态库状态异常"));
			break;
		case -4:
			strLog.Format(_T("[设备控制线程],内部资源申请异常"));
			break;
		case -5:
		{
			byExecuteCode = RCC_INNER_ERROR;//add 2017-07-04
			CString strError;
			m_pHTSDDeviceDll->GetLastErrorDesc(strError);
			strLog.Format(_T("[设备控制线程],执行撤单操作失败，错误原因描述：%s"), strError);
			if(strError.Find("取消")!=-1)
			{
				//byExecuteCode = OP_CANCLE;
			}
			
			else if (strError.Find("账户余额不足") != -1)
			{
				byExecuteCode = OP_NO_MONEY;
			}
			else if (strError.Find("二维码非法") != -1)
			{
				byExecuteCode = OP_QRCODE_ILLEGAL;
			}
			else if (strError.Find("二维码超出有效期") != -1)
			{
				byExecuteCode = OP_QRCODE_OT;
			}
			else if (strError.Find("支付凭证异常") != -1)
			{
				byExecuteCode = OP_NO_ACCOUNT_INFO;
			}
			else if (strError.Find("扣款超时") != -1)
			{
				byExecuteCode = OP_DEBIT_OT;
			}
			else if (strError.Find("密码错误") != -1)
			{
				byExecuteCode = OP_PSW_ERR;
			}
			else if (strError.Find("参数错误") != -1)
			{
				byExecuteCode = OP_PARAM_ERR;
			}
			else if (strError.Find("上次交易未结束") != -1)
			{
				byExecuteCode = OP_LAST_TRADE_NOT_END;
			}
			else if (strError.Find("组件未初始化") != -1)
			{
				byExecuteCode = OP_COM_NOT_INIT;
			}
			else if (strError.Find("组件校验非法") != -1)
			{
				byExecuteCode = OP_COM_ILLEGAL;
			}
			else if (strError.Find("支付平台网络故障") != -1)
			{
				byExecuteCode = OP_PAY_PLATFORM_ERR;
			}
		}
		break;
		}
		bSaveError = TRUE;
	}
	//保存日志
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog, bSaveError ? LOG_LEVEL_ERROR : LOG_LEVEL_RUN);
	}
	//通知业务管理线程已经获取硬件回馈业务数据
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)pStrResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::InitConfigDevice
//
// 功能描述：发送初始化数据到终端
//
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2017年4月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadDevice::InitConfigDevice(void)
{
	BOOL bReturn	= FALSE;
	BOOL bSaveError = FALSE;

	BYTE byCmdType		= RDM_DEVICE_INIT;
	BYTE byExecuteCode	= OP_FAILED;
	CString strLog;

	try
	{
		CString strDataString;

		Json::Value root;
		Json::Value DeviceParamContext;
		Json::Value Data;

		Data["ProvinceID"]			= Json::Value(m_sTranstParam.ProvinceID);
		Data["AreaID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.str_AreaID);
		//Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.RoadID);
		Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.str_RoadID);
		Data["StationID"]			= Json::Value(m_sTranstParam.LaneConfigInfo.str_StationID);
		Data["LaneID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.str_LaneID);
		Data["StationName"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationTab);
		Data["ServerType"]			= Json::Value(m_sTranstParam.LaneConfigInfo.STServerTpye);
		Data["ServerIP"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerIP);
		Data["ServerPort"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerPort);

		Data["ConfigLocalIPMode"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIPMode);
		Data["ConfigLocalIP0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0);
		Data["ConfigLocalSub0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0);
		Data["ConfigLocalGate0"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0);
		Data["ConfigLocalIP1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1);
		Data["ConfigLocalSub1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1);
		Data["ConfigLocalGate1"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1);
		Data["ConfigLocalDefGate"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate);
		Data["DevicePayMode"]		= Json::Value(3);
		Data["CurrentTime"]			= Json::Value(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));

		/*add 2017-05-19*/
		Data["DeviceDllVersion"]			= Json::Value(theApp.GetVersionOfFile("HTTWSDDevice.dll"));
		char szBuff[MAX_PATH] = {0};  
		GetModuleFileName(AfxGetStaticModuleState()->m_hCurrentInstanceHandle, szBuff, MAX_PATH);
		Data["NetPayDllVersion"]			= Json::Value(theApp.GetVersionOfFile(szBuff));
		//Data["NetPayDllVersion"]			= Json::Value(theApp.GetVersionOfFile("TWSDNetPay.dll"));

		/*add 2017-08*/
		Data["UpdatePort"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebUpdatePort);
		/*add 2018-01*/
		Data["UseAgencyPayDll"]= Json::Value(m_sTranstParam.LaneConfigInfo.UseAngencyDll);
		Data["ReturnPayChannelCodeUnify"]= Json::Value(m_sTranstParam.LaneConfigInfo.ReturnPayChannelCodeUnify);



		DeviceParamContext["Data"]		= Data;
		DeviceParamContext["Version"]	= Json::Value(_T("1.0"));
		root["DeviceParamContext"]		= DeviceParamContext;

		Json::FastWriter fast_writer;
		strDataString.Format(_T("%s"), fast_writer.write(root).c_str());

		if (NULL == m_pHTSDDeviceDll)
			throw - 1;

		strLog.Format(_T("[设备控制线程],设备初始化操作,初始化参数:%s"), strDataString);
		GetGlobalApp()->RecordLog(strLog);
		if (!m_pHTSDDeviceDll->InitConfigDevice(strDataString))
			throw - 2;

		strLog.Format(_T("[设备控制线程],设备初始化操作完成"));

		bReturn = TRUE;
		//标识执行成功
		byExecuteCode	= OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[设备控制线程],通讯模块动态库异常,指针为空"));
			break;
		case -2:
		{
			CString strError;
			m_pHTSDDeviceDll->GetLastErrorDesc(strError);
			strLog.Format(_T("[设备控制线程],执行设备初始化操作失败，错误原因描述：%s"), strError);
		}
		break;
		}

		bSaveError = TRUE;
	}
	//保存日志
	if (!strLog.IsEmpty())
	{
		GetGlobalApp()->RecordLog(strLog, bSaveError ? LOG_LEVEL_ERROR : LOG_LEVEL_RUN);
	}

	//通知业务管理线程已经获取硬件回馈业务数据
	::PostThreadMessage(m_sTranstParam.CallerThreadID, m_sTranstParam.ActiveNotifyMsgID, (WPARAM)MAKELONG(MAKEWORD(byCmdType, byExecuteCode), 0), (LPARAM)NULL);

	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::fnGetNotifyFromCommModule
//
// 功能描述：用于获取通讯模块主动回馈的回掉函数
//
// 输入参数：int iType
//         ：const std::string& strData
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年5月7日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::fnGetNotifyFromCommModule(int iType, const std::string& strData)
{
}

bool CThreadDevice::BreakDebit(const CString& payID)
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("句柄无效");
		return false;
	}
	return m_pHTSDDeviceDll->BreakDebit(payID);
}

bool CThreadDevice::Init_Dev()
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("句柄无效");
		return false;
	}
	return m_pHTSDDeviceDll->InitDev(fnGetNotifyFromCommModule);
}
bool CThreadDevice::Destroy_Dev()
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("句柄无效");
		return false;
	}
	return m_pHTSDDeviceDll->Destroy();
}

bool CThreadDevice::AgencyDebitQuery(const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult)
{
	if (NULL == m_pHTSDDeviceDll)
	{
		GetGlobalApp()->RecordLog("句柄无效");
		return false;
	}
	return m_pHTSDDeviceDll->AgencyDebitQuery(szParamContext, iFormatType,  iResult);
}


bool CThreadDevice::TransferImage(const std::string& szImageInfo)
{
    if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }
    return m_pHTSDDeviceDll->TransferImage(szImageInfo);   
}

bool CThreadDevice::TransTickNo(const std::string& para)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }
    return m_pHTSDDeviceDll->TransTickNo(para);   
}

bool CThreadDevice::HardwareTestGreenChannel(int testType,const std::string& testPara,std::string& result)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }

    return m_pHTSDDeviceDll->GreenChannelTest(testType,testPara,result);   
}

bool CThreadDevice::ExcuteComCommond(const std::string& hexStr)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }

    return m_pHTSDDeviceDll->ExcuteComCommond(hexStr);   
}

bool CThreadDevice::TakeComFeedbackCommond(CString& jsonStr)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }

    return m_pHTSDDeviceDll->TakeComFeedbackCommond(jsonStr);  
}


bool CThreadDevice::QueryTerminalSystemVersionStatus(int& status)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }
	return m_pHTSDDeviceDll->QueryTerminalSystemVersionStatus(status);
}
bool CThreadDevice::ExchangeTerminalSystem(const int& targetSystemVersion)
{
	if (NULL == m_pHTSDDeviceDll)
    { 
        GetGlobalApp()->RecordLog("句柄无效");
        return false;
    }
	return m_pHTSDDeviceDll->ExchangeTerminalSystem(targetSystemVersion);
}
