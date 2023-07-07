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
:m_pHTSDDeviceDll(NULL)
,m_iDeviceVersion(2)
//,m_bInitConfigDevice(FALSE) 
,m_bInitConfigDevice(TRUE) //测试使用
,m_tLastCheckBoard(NULL)

{
}

CThreadDevice::~CThreadDevice()
{
}

BOOL CThreadDevice::InitInstance()
{
	m_tLastCheckBoard = time(NULL);
	return TRUE;
}

int CThreadDevice::ExitInstance()
{
	if(NULL != m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll->Destroy();
		delete m_pHTSDDeviceDll;
		m_pHTSDDeviceDll = NULL;
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadDevice, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_GET_DATA,OnMsgStartGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_STOP_GET_DATA,OnMsgStopGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY,OnMsgGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_SHOW_DEAL_INFO,OnMsgShowDealInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_DEBIT_FLOW,OnMsgStartDebitFlow)

	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_DEBIT_CANCEL,OnMsgDebitCancelFlow)
END_MESSAGE_MAP()


// CThreadDevice message handlers

BOOL CThreadDevice::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		::PostQuitMessage(0);
		return FALSE;
	}

	CString strLog;
	if(NULL == m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll = new CHTSDDeviceDll();
	}
	else
	{
		//判断是否需要加载组件
		static time_t tLastInitDllTime = 0x00;
		if( (time(NULL) - tLastInitDllTime) > 10)
		{
			if(!m_pHTSDDeviceDll->GetDllInitState())
			{
				CString strDllFilePath;
				strDllFilePath.Format(_T("%s\\%s"),m_sTranstParam.PathCommDir,FILE_NAME_DEVICE_DLL);
				m_pHTSDDeviceDll->Init(strDllFilePath);
				tLastInitDllTime = time(NULL);
			}
		}
		//判断是否需要初始化组件
		static time_t tLastInitDevTime = 0x00;
		if( (time(NULL) - tLastInitDevTime) > 10)
		{
			if(!m_pHTSDDeviceDll->GetDevInitState())
			{
				m_pHTSDDeviceDll->InitDev(m_nThreadID,NULL,WM_THREAD_DEVICE_NOTIFY,m_sTranstParam.ProvinceID,0x00);
				tLastInitDevTime = time(NULL);
			}
		}

		
		//主动查询心跳，超时无任意应答，串口异常
		if(time(NULL) - m_tLastCheckBoard > 60)
		{
			m_bInitConfigDevice = TRUE;
		}

		//if判断只是测试使用
		static time_t tLastInit = 0x00;
		if(time(NULL) - tLastInit > 30)
		{
			//往串口下发设备初始化指令
			if(m_bInitConfigDevice && m_pHTSDDeviceDll->GetDevInitState())
			{
				//m_bInitConfigDevice = FALSE;
				CString strData = InitConfigDevice();
				m_pHTSDDeviceDll->InitConfigDevice(strData,2);
			}

			tLastInit = time(NULL);
		}

	}
	//线程存活日志记录检测
	static time_t tLastDeviceLogTime = 0;
	if( (time(NULL) - tLastDeviceLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
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
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		//获取硬件当前状态
		if(!m_pHTSDDeviceDll->CheckDeviceStatus(dwState))
			throw -2;
		//获取驱动加载状态
		if(!GetDeviceInitState())
		{
			dwReturnState |= 0x01;
		}
		//获取硬件串口状态
		dwReturnState |= dwState;
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
	return dwReturnState;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::OnMsgStartGetData
//     
// 功能描述：执行启动获取数据的操作
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgStartGetData(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;
		//启动获取扫码数据功能
		m_pHTSDDeviceDll->StartGetData((int)wParam);
		strLog.Format(_T("[设备控制线程],接收到指令，控制设备进行扫码，超时时间:%d"),(int)wParam);
		//如果是1.0版本的硬件，则还需要进行IO控制(请扫码)
		if(1 == m_iDeviceVersion)
		{
			m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SCAN);
			strLog.Format(_T("[设备控制线程],接收到指令，控制设备进行扫码，超时时间:%d;同时启动请扫码亮灯操作"),(int)wParam);
		}
		//保存日志
		GetGlobalApp()->RecordLog(strLog);
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
// 函数名称：CThreadDevice::OnMsgStopGetData
//     
// 功能描述：执行停止获取数据的操作
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgStopGetData(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;
		//停止获取扫码数据功能
		m_pHTSDDeviceDll->StopGetData();
		strLog.Format(_T("[设备控制线程],接收到指令，控制设备停止扫码操作"));
		//如果是1.0版本的硬件，则还需要进行IO控制(灭灯)
		if(1 == m_iDeviceVersion)
		{
			m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
			strLog.Format(_T("[设备控制线程],接收到指令，控制设备停止扫码;同时启动灭灯操作"));
		}
		//保存日志
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
// 函数名称：CThreadDevice::OnMsgGetData
//     
// 功能描述：获取智能设备回馈的数据
//     
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年2月26日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_NOTIFY
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgGetData(WPARAM wParam,LPARAM lParam)
{
	WORD wSubType = LOBYTE(wParam);
	WORD wCmdType = HIBYTE(wParam);
	BOOL bGetData = (BOOL)lParam;

	WORD wCmd= (WORD)wParam;

	CString* pInfo = NULL;
	//解析数据
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;
		//判断是否获取到成功的数据
		if(FALSE == bGetData)
			throw -3;
		//获取数据
		CString strData = _T("");
		int nSrcType = 0x00;
		if(!m_pHTSDDeviceDll->GetData(strData,wCmd,nSrcType))
			throw -4;

		pInfo = new CString();
		if(NULL == pInfo)
			throw -5;

		pInfo->Format(_T("%s"),strData);

		
		//如果是主动查询心跳		
		if((0x01 ==wCmdType) && (0x01== wSubType))
		{
			DWORD dwData = _ttoi(strData);
			if(0x00 == dwData)
			{
				m_tLastCheckBoard = time(NULL);
			}
		}
	

		//测试使用
		if((0x05 ==wCmdType) && (0x05== wSubType))
		{
			DWORD dwData = _ttoi(strData);
			if(0x01 == dwData)
				m_bInitConfigDevice = FALSE;

			return;
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
		case -4:
			break;
		case -5:
			break;
		}
		bGetData = FALSE;
	}
	//通知业务管理线程已经获取用户信息//m_sTranstParam.ActiveNotifyMsgID    WM_THREAD_DEVICE_NOTIFY
	::PostThreadMessage(m_sTranstParam.CallerThreadID,m_sTranstParam.ActiveNotifyMsgID,(WPARAM)wParam,(LPARAM)pInfo);
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
void CThreadDevice::OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam)
{
	try
	{

		CString* pstrInfo = (CString*)wParam;
		if(NULL == pstrInfo)
			throw -1;
		if(NULL == m_pHTSDDeviceDll)
			throw -2;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -3;

		int nType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"),*pstrInfo);
		//设置显示交互信息
		m_pHTSDDeviceDll->SetDisplayContext(nType,strData);

		//清理资源
		delete pstrInfo;
		pstrInfo = NULL;
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
void CThreadDevice::OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		CString* pstrXML = (CString*)wParam;
		if(NULL == pstrXML)
			throw -1;
		if(NULL == m_pHTSDDeviceDll)
			throw -2;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -3;

		int nType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"),*pstrXML);

		//启动扣款流程

		m_pHTSDDeviceDll->DebitMoney(strData,nType);

		strLog.Format(_T("[设备控制线程],接收到指令，开始扣款扫码流程操作"));
		//保存日志
		GetGlobalApp()->RecordLog(strLog);
		//清理资源
		delete pstrXML;
		pstrXML = NULL;
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
}




BOOL CThreadDevice::GetC0SuccFailedCount(OUT int& dwSucce, OUT int& dwFailed, OUT int& nReSendCount, OUT CString& strA2Frame, OUT CString& strC0Frame)
{
	BOOL bRet = FALSE;
	dwSucce = 0;
	dwFailed = 0;
	nReSendCount = 0;
	strA2Frame = _T("");
	strC0Frame = _T("");
	try
	{
		if(NULL == m_pHTSDDeviceDll)
			throw -1;
		//获取硬件当前状态
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -2;

		bRet = m_pHTSDDeviceDll->GetCSuccFailCount(dwSucce,dwFailed,nReSendCount,strA2Frame,strC0Frame);
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


	return bRet;
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
void CThreadDevice::OnMsgDebitCancelFlow(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	try
	{
		CString* pstrXML = (CString*)wParam;
		if(NULL == pstrXML)
			throw -1;
		if(NULL == m_pHTSDDeviceDll)
			throw -2;
		if(!m_pHTSDDeviceDll->GetDevInitState())
			throw -3;

		int nType = (int)lParam;

		CString strData;
		strData.Format(_T("%s"),*pstrXML);

		//撤消订单
		m_pHTSDDeviceDll->DebitCancel(strData,nType);

		strLog.Format(_T("[设备控制线程],接收到指令，开始执行撤单操作"));
		//保存日志
		GetGlobalApp()->RecordLog(strLog);
		//清理资源
		delete pstrXML;
		pstrXML = NULL;
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
}

const CString CThreadDevice::InitConfigDevice(int nFormatType)
{
	CString strDataString;

	if(0x01 == nFormatType)
	{
		Json::Value root;
		Json::Value DeviceParamContext;
		Json::Value Data;

		Data["ProvinceID"]			= Json::Value(m_sTranstParam.ProvinceID);
		Data["AreaID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.AreaID);
		Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.RoadID);
		Data["StationID"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationID);
		Data["LaneID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.LaneID);
		Data["StationName"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationTab);
		Data["ServerType"]			= Json::Value(m_sTranstParam.LaneConfigInfo.STServerTpye);
		Data["ServerIP"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerIP);
		Data["ConfigLocalIP0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0);
		Data["ConfigLocalSub0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0);
		Data["ConfigLocalGate0"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0);
		Data["ConfigLocalIP1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1);
		Data["ConfigLocalSub1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1);
		Data["ConfigLocalGate1"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1);
		Data["ConfigLocalDefGate"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate);
		Data["DevicePayMode"]		= Json::Value(3);
		Data["CurrentTime"]			= Json::Value(CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));


		DeviceParamContext["Data"] = Data;
		DeviceParamContext["Version"] = Json::Value(_T("1.0"));

		root["DeviceParamContext"] = DeviceParamContext;

		Json::FastWriter fast_writer;
		strDataString.Format(_T("%s"),fast_writer.write(root).c_str());
	}

	return strDataString;

}

