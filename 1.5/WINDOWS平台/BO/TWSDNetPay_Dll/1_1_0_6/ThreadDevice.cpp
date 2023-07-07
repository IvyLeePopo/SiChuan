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

#include "sqlite3_agent.h"

// CThreadDevice

IMPLEMENT_DYNCREATE(CThreadDevice, CThreadBase)

CThreadDevice::CThreadDevice()
	: m_pHTSDDeviceDll(NULL)
	, m_iDeviceVersion(2)
{
	m_firmwareUpdateFlag = false;
	m_firmwareUpdateTime = 0;
	nLocalDBSpan = 300000;
	nArraySpan = 30000;

	dwLocalDBCheck = ::GetTickCount();
	dwArrayCheck = ::GetTickCount();

	m_csUpdateLDB	= _T("");
	m_csLDBTable	= _T("");
	m_csDevLog		= _T("");

	m_strDefaultGGText.Format(_T("高速纵横欢迎您"));

	std::locale::global(std::locale(""));


	mShareMemNameOfDevId = "mem_dev_id";
	mhShareMemOfDevId = NULL;
	mpShareMemOfDevId = NULL;

	mhShareMemOfDevId = CreateFileMapping(
	                        INVALID_HANDLE_VALUE,    // 物理文件句柄
	                        NULL,                    // 默认安全级别
	                        PAGE_READWRITE,          // 可读可写
	                        0,                       // 高位文件大小
	                        100,                // 地位文件大小
	                        mShareMemNameOfDevId.c_str()                   // 共享内存名称
	                    );


	mpShareMemOfDevId = (char *)MapViewOfFile(
	                        mhShareMemOfDevId,            // 共享内存的句柄
	                        FILE_MAP_ALL_ACCESS, // 可读写许可
	                        0,
	                        0,
	                        100
	                    );


}

void CThreadDevice::Write_dev_id()
{
	if (mpShareMemOfDevId == NULL)
	{
		return ;
	}
	Sleep(1);
	strcpy(mpShareMemOfDevId, GetGlobalApp()->m_strTWProductSerialNo.GetBuffer(0));

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
	ReadWebConfig();

	return TRUE;
}

int CThreadDevice::ExitInstance()
{
	if (NULL != m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll->Destroy();
		delete m_pHTSDDeviceDll;
		m_pHTSDDeviceDll = NULL;
	}

	if (mpShareMemOfDevId != NULL)
	{
		UnmapViewOfFile(mpShareMemOfDevId);
		mpShareMemOfDevId = NULL;
	}
	if (mhShareMemOfDevId != NULL)
	{
		CloseHandle(mhShareMemOfDevId);
	}

	//MessageBox(0,"","",0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadDevice, CThreadBase)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_GET_DATA, OnMsgStartGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_STOP_GET_DATA, OnMsgStopGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_NOTIFY, OnMsgGetData)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_SHOW_DEAL_INFO, OnMsgShowDealInfo)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI, OnMsgControlResultMMI)
	ON_THREAD_MESSAGE(WM_THREAD_DEVICE_START_DEBIT_FLOW, OnMsgStartDebitFlow)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_ISSUE_DATA, OnMsgIssueData)

	ON_THREAD_MESSAGE(WM_THREAD_REMOTE_UPRGADE_RESULT, OnMsgRemoteUpgradeResult)



END_MESSAGE_MAP()


// CThreadDevice message handlers

BOOL CThreadDevice::OnIdle(LONG lCount)
{
	//GetGlobalApp()->RecordLog("OnIdle-------");
	if (m_bExit)
	{

		//MessageBox(0,"CThreadDevice","",0);

		c_update_device.m_aryUpdateInfo.RemoveAll();

		::PostQuitMessage(0);
		return FALSE;
	}

	CString strLog;
	if (NULL == m_pHTSDDeviceDll)
	{
		m_pHTSDDeviceDll = new CHTSDDeviceDll();
	}
	else
	{
		//判断是否需要加载组件
		static time_t tLastInitDllTime = 0x00;
		if ( (time(NULL) - tLastInitDllTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDllInitState())
			{
				CString strDllFilePath;
				strDllFilePath.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, FILE_NAME_DEVICE_DLL);
				m_pHTSDDeviceDll->Init(strDllFilePath);
				tLastInitDllTime = time(NULL);
			}
		}
		//判断是否需要初始化组件
		static time_t tLastInitDevTime = 0x00;
		if ( (time(NULL) - tLastInitDevTime) > 10)
		{
			if (!m_pHTSDDeviceDll->GetDevInitState())
			{
				m_pHTSDDeviceDll->InitDev(m_nThreadID, NULL, WM_THREAD_DEVICE_NOTIFY, m_sTranstParam.ProvinceID, 0x00);
				tLastInitDevTime = time(NULL);
			}
		}

		//检查是否需要往硬件进行更新时间
		if (m_pHTSDDeviceDll->GetDevInitState())
		{
			static time_t tLastUpdateDeviceTime = 0x00;
			if ( (time(NULL) - tLastUpdateDeviceTime) > ::GetGlobalApp()->m_dwUpdateDevieTimeSpanTime)
			{
				BOOL bUpdate = UpdateDeviceTime();

				tLastUpdateDeviceTime = time(NULL);
			}
		}
		//系统重启6秒后，尝试重置终端的欢迎界面以及广告位信息
		if (m_pHTSDDeviceDll->GetDevInitState())
		{
			static bool bInitGGInfoDefault = false;
			static time_t tLaseResetGGInfoDefault = time(NULL);

			if (!bInitGGInfoDefault)
			{
				if ( (time(NULL) - tLaseResetGGInfoDefault) > 6)
				{
					m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
					::Sleep(100);
					SetPayErrorMsgInfo(0x00, true);
					tLaseResetGGInfoDefault = time(NULL);
					bInitGGInfoDefault = true;
				}
			}

			//如果终端序号为空且当前不在交易流程当中，则进行获取
			if (    GetGlobalApp()->m_strTWProductSerialNo.IsEmpty()
			        && !GetGlobalApp()->m_bIsTrading)
			{
				m_pHTSDDeviceDll->GetDeviceFirewareVer(GetGlobalApp()->m_strTWProductSerialNo);
				Write_dev_id();
				// GetGlobalApp()->RecordLog("ABC:"+GetGlobalApp()->m_strTWProductSerialNo);
			}

			//add 2018-01 如果终端刚经历了升级，则隔断时间，在进行版本号的获取
			if (m_firmwareUpdateFlag &&
			        (time(NULL) - m_firmwareUpdateTime) < 60 * 2
			   )
			{
				//GetGlobalApp()->RecordLog("dddddddddddddddddddddddddd");
			}
			else
			{
				if (    GetGlobalApp()->m_strTWProductFirmwareVer.IsEmpty()
				        && !GetGlobalApp()->m_bIsTrading)
				{					
					m_pHTSDDeviceDll->RealFirmwareVer(GetGlobalApp()->m_strTWProductFirmwareVer);
				}
				if (    GetGlobalApp()->m_strTWProductSlaveFirmwareVer.IsEmpty()
				        && !GetGlobalApp()->m_bIsTrading)
				{
					m_pHTSDDeviceDll->RealSlaveFirmwareVer(GetGlobalApp()->m_strTWProductSlaveFirmwareVer);
				}
				m_firmwareUpdateFlag = false;
			}


		}
		//广告播报处理主函数
		ITMSMain();
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

	::Sleep(10);
	//GetGlobalApp()->RecordLog("OnIdle++++");
	return  CThreadBase::OnIdle(lCount);
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
void CThreadDevice::OnMsgStartGetData(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	try
	{
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;
		//启动获取扫码数据功能
		m_pHTSDDeviceDll->StartGetData((int)wParam);
		strLog.Format(_T("[设备控制线程],接收到指令，控制设备进行扫码，超时时间:%d"), (int)wParam);
		//如果是1.0版本的硬件，则还需要进行IO控制(请扫码)
		if (1 == m_iDeviceVersion)
		{
			m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SCAN);
			strLog.Format(_T("[设备控制线程],接收到指令，控制设备进行扫码，超时时间:%d"), (int)wParam);
		}
		//保存日志
		GetGlobalApp()->RecordLog(strLog);
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
void CThreadDevice::OnMsgStopGetData(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	try
	{
		int iChangeMMI = (int)lParam;

		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;
		//停止获取扫码数据功能
		m_pHTSDDeviceDll->StopGetData(iChangeMMI);
		strLog.Format(_T("[设备控制线程],接收到指令，控制设备停止扫码操作"));

		//判断是否启用NFC支付功能
		if (::GetGlobalApp()->m_bNFCIsUse)
		{
			CNFCProtocol cNFCProtocol;
			CDVXMLUnipay xmlUnipay;
			CString strContext	= cNFCProtocol.ConstructStopExecuteCmd();
			CString strSendData = xmlUnipay.ConstructNFCFrameData(strContext, UMS_STOP);
			if (!m_pHTSDDeviceDll->IssuedData(strSendData))
				throw - 3;
			strLog.Format(_T("[设备控制线程],接收到指令，控制设备停止获取支付凭证操作(停止扫码和停止NFC读卡)，交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}

		//保存日志
		GetGlobalApp()->RecordLog(strLog);
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
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_NOTIFY
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgGetData(WPARAM wParam, LPARAM lParam)
{
	CString strLog = _T("");
	BOOL bGetData = (BOOL)wParam;

	tagAccountInfo* pAccountInfo = NULL;
	//解析数据
	try
	{
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;

		//判断是否获取到成功的数据
		//版本1057 新增郎为设备处理
		if ( 1 == theApp.m_iDeviceType )
		{
			strLog.Format(_T("[设备控制线程],获取到【郎为】底层设备返回的扫码消息(%d)"), bGetData );
		}
		else
		{
			if (FALSE == bGetData)
				throw - 3;
		}

		//获取数据
		CString strData;
		int iDataType = 0x00;
		int iSrcType  = 0x00;
		if (!m_pHTSDDeviceDll->GetData(strData, iDataType, iSrcType))
			throw - 4;
		//版本1057 新增郎为设备处理
		if ( 1 == theApp.m_iDeviceType )
		{
			strLog.Format(_T("[设备控制线程],获取到【郎为】底层设备返回数据(data:%s, Type:%d, Src:%d)"), strData, iDataType, iSrcType );
			GetGlobalApp()->RecordLog(strLog);
		}

		pAccountInfo = new tagAccountInfo();
		if (NULL == pAccountInfo)
			throw - 5;
		memset(pAccountInfo, 0, sizeof(tagAccountInfo));

		//复制数据
		pAccountInfo->DataType		= iDataType;
		pAccountInfo->DataSize		= strData.GetLength();
		pAccountInfo->SrcType		= iSrcType;
		memcpy_s(pAccountInfo->DataContext, _countof(pAccountInfo->DataContext), strData.GetBuffer(0), strData.GetLength());
		strData.ReleaseBuffer();

		strLog.Format(_T("[设备控制线程],获取到数据(来源:%d)"), iSrcType );
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
		case -4:
			break;
		case -5:
			break;
		}
		bGetData = FALSE;
	}
	GetGlobalApp()->RecordLog(strLog);
	//通知业务管理线程已经获取用户信息
	::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_DEVICE_NOTIFY, (WPARAM)bGetData, (LPARAM)pAccountInfo);
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
	try
	{
		int iMMIType = (int)lParam;

		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		CString strData;
		strData.Format(_T("%s"), *pstrXML);

		//设置显示交互信息
		int iID = 0x01;
		m_pHTSDDeviceDll->SetDisplayContext(iID, strData);

		//判断是否需要恢复广告位的错误信息
		if (0x04 == iMMIType)
		{
			//延迟一定时间，默认100毫秒
			::Sleep(::GetGlobalApp()->m_iClearMMIDelayTime);
			SetPayErrorMsgInfo();
		}

		//清理资源
		delete pstrXML;
		pstrXML = NULL;
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
void CThreadDevice::OnMsgStartDebitFlow(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	try
	{
		CString* pstrXML = (CString*)wParam;
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;

		//版本1057 新增郎为设备处理
		if ( 1 == theApp.m_iDeviceType )
		{
			strLog.Format(_T("[设备控制线程],郎为设备扫码特殊处理，首先启动（StartGetData）接口"));
			GetGlobalApp()->RecordLog(strLog);

			m_pHTSDDeviceDll->StartGetData( 30000 );// 30秒超时
		}

		if (GetGlobalApp()->m_bIsExternMMIDetail)
		{
			//复位广告（错误描述信息）
			SetPayErrorMsgInfo();
			::Sleep(100);
		}

		CString strData;
		strData.Format(_T("%s"), *pstrXML);

		//判断是否启用NFC支付功能
		if (GetGlobalApp()->m_bNFCIsUse
		        /*add zhuyabing 2017-10-21*/
		        && GetGlobalApp()->m_bNFCSignInSuccess)
		{
			//扣费操作必须保证签到成功
			// remove zhuybing 2017-10-21
			// if(!GetGlobalApp()->m_bNFCSignInSuccess)
			// 	throw -5;

			CDVXMLDebitFlow xmlDebit;
			if (!xmlDebit.Init(strData))
				throw - 6;
			//warning zhuyabing 2017-11-06 zhuyabing 在0元的情况下，不应该直接退出，而是能启用扫码

			//if(0 == xmlDebit.Money)
			//	throw -7;

			CNFCProtocol cNFCProtocol;
			CDVXMLUnipay xmlUnipay;
			CString strContext = cNFCProtocol.ConstructStartConsumeCmd(xmlDebit.Money);
			CString strSendData = xmlUnipay.ConstructNFCFrameData(strContext, UMS_DEBITING);

			m_LastNFCPayData = strSendData;

			if (!m_pHTSDDeviceDll->IssuedData(strSendData))
				throw - 8;
			//设置银联支付状态
			GetGlobalApp()->SetUnipayWorkFlowState(UMS_DEBITING);
			//记录日志
			strLog.Format(_T("[设备控制线程],接收到指令，执行扣款流程操作(扫码和非接)，交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}
		else
		{
			strLog.Format(_T("[设备控制线程],接收到指令，执行扣款流程操作(扫码),交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
		}

		//设置启动扣款流程
		int iID = 0x01;
		if (!m_pHTSDDeviceDll->SetDisplayContext(iID, strData))
			throw - 4;
		//清理资源
		delete pstrXML;
		pstrXML = NULL;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[设备控制线程],扣款流程操作出现错误，参数指针为空,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -2:
			strLog.Format(_T("[设备控制线程],扣款流程操作出现错误，通讯组件指针为空,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -3:
			strLog.Format(_T("[设备控制线程],扣款流程操作出现错误，通讯组件未初始化,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -4:
			strLog.Format(_T("[设备控制线程],扣款流程操作出现错误，执行下发0x57指令失败,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -5:
			strLog.Format(_T("[设备控制线程],接收到指令，执行扣款流程操作，NFC模块未签到成功，本次交易仅允许扫码，不允许刷卡,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -6:
			strLog.Format(_T("[设备控制线程],接收到指令，执行扣款流程操作，解析整合扣费数据出现异常，本次交易不启动NFC刷卡操作,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -7:
			strLog.Format(_T("[设备控制线程],接收到指令，执行扣款流程操作，消费金额为0，本次交易不启动NFC刷卡操作,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		case -8:
			strLog.Format(_T("[设备控制线程],扣款流程操作出现错误，执行下发NFC刷卡指令失败,交易标识:%s"), GetGlobalApp()->m_pCurrentDebitInfo.PayIdentifier);
			break;
		}
	}
	//保存日志
	GetGlobalApp()->RecordLog(strLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::OnMsgControlResultMMI
//
// 功能描述：接收到消息，要求展示缴费结果交互界面
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgControlResultMMI(WPARAM wParam, LPARAM lParam)
{
	int iResultMode = (int)wParam;
	try
	{
		if (NULL == m_pHTSDDeviceDll)
			throw - 1;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 2;

		if (GetGlobalApp()->m_bIsAllowUseNewMMIDetailMode)
		{
			//最新版交互模式启用
			switch (iResultMode)
			{
			case 1://缴费成功
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SUCCESS);
				break;
			case 2://缴费失败
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case 3://恢复初始界面
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
				break;

			case SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT:	//请输入密码
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR:	//密码错误
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1:	//支付码无效
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2:	//支付码超时
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_BL_ERR:		//余额不足
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_BL_ERR);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_NET_ERR:	//网络超时
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_NET_ERR);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_PL_STOP:	//暂停服务
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_PL_STOP);
				break;
			default://恢复默认显示内容
				SetPayErrorMsgInfo();
				break;
			}
		}
		else
		{
			//河南运营活动版本适配交互细节
			int iDelayTime = 5;
			switch (iResultMode)
			{
			case 1://缴费成功
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_SUCCESS);
				break;
			case 2://缴费失败
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case 3://恢复初始界面
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_ALL_OFF);
				break;

			case SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT:	//请输入密码
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR:	//密码错误
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1:	//二维码非法
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2:	//二维码超时
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_BL_ERR:		//余额不足
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_BL_ERR);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_NET_ERR:	//网络超时
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_NET_ERR);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			case SM_PRPTOCOL_LIGHT_FALI_PL_STOP:	//暂停服务
				SetPayErrorMsgInfo(SM_PRPTOCOL_LIGHT_FALI_PL_STOP);
				::Sleep(iDelayTime);
				m_pHTSDDeviceDll->SetIOState(SM_PRPTOCOL_LIGHT_FAIL);
				break;
			default://恢复默认显示内容
				SetPayErrorMsgInfo();
				break;
			}
		}
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
// 函数名称：CThreadDevice::OnMsgIssueData
//
// 功能描述：执行下发数据功能操作
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_NFC_ISSUE_DATA
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgIssueData(WPARAM wParam, LPARAM lParam)
{
	CString strLog;
	CString strFlowType;
	CString strRevData;
	CString strIssueData;
	CString* pstrXML = (CString*)wParam;
	try
	{
		if (NULL == pstrXML)
			throw - 1;
		if (NULL == m_pHTSDDeviceDll)
			throw - 2;
		if (!m_pHTSDDeviceDll->GetDevInitState())
			throw - 3;
		strRevData.Format(_T("%s"), *pstrXML);
		//获取流程类型
		switch (lParam)
		{
		case UMS_TEST_LINK:
			strFlowType.Format(_T("链路测试"));
			break;
		case UMS_SIGNING:
			strFlowType.Format(_T("签到"));
			break;
		case UMS_DEBITING:
			strFlowType.Format(_T("扣费"));
			break;
		case UMS_CORRECT:
			strFlowType.Format(_T("冲正"));
			break;
		case UMS_STOP:
			strFlowType.Format(_T("终止上次操作"));
			break;
		case UMS_PARA_QUERY:
			strFlowType.Format(_T("参数查询"));
			break;
		}
		//构造下发数据XML字符串
		CDVXMLUnipay cDVXMLUnipay;
		strIssueData = cDVXMLUnipay.ConstructNFCFrameData(strRevData, lParam);

		if (!m_pHTSDDeviceDll->IssuedData(strIssueData))
			throw - 4;

		strLog.Format(_T("[设备控制线程],NFC数据交互，调用下发数据接口成功,当前流程：%s"), strFlowType);
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			strLog.Format(_T("[设备控制线程],NFC数据交互过程出现异常，接收到的参数指针为空,当前流程：%s"), strFlowType);
			break;
		case -2:
			strLog.Format(_T("[设备控制线程],NFC数据交互过程出现异常，通讯组件指针为空,当前流程：%s"), strFlowType);
			break;
		case -3:
			strLog.Format(_T("[设备控制线程],NFC数据交互过程出现异常，通讯组件未能成功初始化,当前流程：%s"), strFlowType);
			break;
		case -4:
			strLog.Format(_T("[设备控制线程],NFC数据交互过程出现异常，执行下发数据操作失败,当前流程：%s"), strFlowType);
			break;
		}
	}
	//记录日志
	GetGlobalApp()->RecordLog( strLog );
	//清理资源
	if (NULL != pstrXML)
	{
		delete pstrXML;
		pstrXML = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::UpdateDeviceTime
//
// 功能描述：往智能硬件设备更新时间
//
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadDevice::UpdateDeviceTime(void)
{
	//重构下发时间XML
	CMarkup xmlTime;
	xmlTime.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
	xmlTime.AddElem( _T("DisplayContext") );//root
	xmlTime.IntoElem();

	xmlTime.AddElem( _T("Version"),	_T("1.0") );
	xmlTime.AddElem( _T("ProvinceID"),	::GetGlobalApp()->m_sTranstParam.ProvinceID );
	xmlTime.AddElem( _T("VerifyCode"),	0 );
	xmlTime.AddElem( _T("Data") );

	xmlTime.IntoElem();

	xmlTime.AddElem( _T("Type"),			0x02 );

	xmlTime.AddElem( _T("Money"),			0x00 );
	xmlTime.AddElem( _T("Weight"),			0x00 );
	xmlTime.AddElem( _T("VehicleType"),		0x00 );
	xmlTime.AddElem( _T("VehicleClass"),	0x00 );
	xmlTime.AddElem( _T("CurrentTime"),	CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S") );

	xmlTime.OutOfElem();
	xmlTime.OutOfElem();

	CString strXMLString = xmlTime.GetDoc();

	int iID = 0x01;
	return m_pHTSDDeviceDll->SetDisplayContext(iID, strXMLString);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::SetPayErrorMsgInfo
//
// 功能描述：控制显示支付异常的信息（占用广告位置）
//
// 输入参数：IN const int& iErrorCode = 0
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年10月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::SetPayErrorMsgInfo(IN const int& iErrorCode, IN const bool& bForceDefault)
{
	static bool bIsNeedReset = false;
	CString strErrorMsg;
	//构造错误信息
	switch (iErrorCode)
	{
	case 0://默认信息
		strErrorMsg.Format(_T("%s"), m_strDefaultGGText);
		if (!bForceDefault)
		{
			if (!bIsNeedReset)
			{
				strErrorMsg.Empty();
			}
		}
		break;
	case SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT:
		strErrorMsg.Format(_T("请输密码"));
		break;
	case SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR:
		strErrorMsg.Format(_T("密码错误"));
		break;
	case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1:
		strErrorMsg.Format(_T("二维码非法"));
		break;
	case SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2:
		strErrorMsg.Format(_T("二维码超时"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_BL_ERR:
		strErrorMsg.Format(_T("余额不足"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NET_ERR:
		strErrorMsg.Format(_T("网络超时"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_PL_STOP:
		strErrorMsg.Format(_T("暂停服务"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NFC_01:
		strErrorMsg.Format(_T("卡片不支持闪付"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NFC_02:
		strErrorMsg.Format(_T("卡片未初始化"));
		break;
	case SM_PRPTOCOL_LIGHT_FALI_NFC_03:
		strErrorMsg.Format(_T("银联闪付暂停服务"));
		break;
	}
	//构造指令
	CString strCMD;
	CMarkup xmlShow;
	xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
	xmlShow.AddElem( _T("DisplayContext") );//root
	xmlShow.IntoElem();

	xmlShow.AddElem( _T("Version"),		_T("1.0") );
	xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
	xmlShow.AddElem( _T("VerifyCode"),	0 );

	xmlShow.AddElem( _T("Data") );
	xmlShow.IntoElem();
	xmlShow.AddElem( _T("Type"),		102 );
	xmlShow.AddElem( _T("Context"),		strErrorMsg );
	xmlShow.OutOfElem();

	xmlShow.OutOfElem();
	strCMD = xmlShow.GetDoc();
	strCMD.Replace(_T(" "), _T(""));
	strCMD.Replace(_T("\r"), _T(""));
	strCMD.Replace(_T("\n"), _T(""));

	//设置显示交互信息
	if (   ( NULL != m_pHTSDDeviceDll)
	        && ( FALSE != m_pHTSDDeviceDll->GetDevInitState())
	        && ( FALSE == strErrorMsg.IsEmpty())
	   )
	{
		int iID = 0x01;
		if ( m_pHTSDDeviceDll->SetDisplayContext(iID, strCMD) )
		{
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[设备控制线程]:更新错误提示信息(%s)"), strErrorMsg));
		}
	}

	//确认下次是否需要重复发送广告位默认信息
	if (0 != iErrorCode)
	{
		bIsNeedReset = true;
	}
	else
	{
		bIsNeedReset = false;
	}
}


void CThreadDevice::ReadWebConfig()
{
	CString m_csCfgFile = _T("");

	m_csCfgFile.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, FILE_NAME_APP_CONFIG);

	m_csUpdateLDB.Format(_T("%s\\TWSDNetPayLog\\TWITMS.db3"), m_sTranstParam.PathCommDir);

	m_csLDBTable.Format( _T("ITMSUpdate") );

	nLocalDBSpan	=	::GetPrivateProfileInt(_T("ITMS"),	_T("LocalDBSpan"),		300000,		m_csCfgFile );
	nArraySpan		=	::GetPrivateProfileInt(_T("ITMS"),	_T("ArraySpan"),		30000,		m_csCfgFile );
#ifdef _DEBUG
	nLocalDBSpan = 20000;
#endif
}




void CThreadDevice::ITMSMain()
{
	if ( !GetGlobalApp()->m_bIsTrading ) //非扣款过程
	{
		if ( c_update_device.m_aryUpdateInfo.GetSize() > 0 )
		{
			ArrayCheck();
		}
		else
		{
			LocalDBCheck();
		}
	}
}



void CThreadDevice::LocalDBCheck()
{
	DWORD	dwCurrent = ::GetTickCount();
	if ( dwCurrent >= dwLocalDBCheck ) //正常计时，未到49.7天的溢出周期
	{
		if ( dwCurrent - dwLocalDBCheck >= nLocalDBSpan) //默认 300 S检测一次
		{
			int iRet = 0;
			CString	csErr = _T("");
			LockFileUpdate();

			c_update_device.m_aryUpdateInfo.RemoveAll();
			iRet = GetLocalDB2Array(m_csUpdateLDB, m_csLDBTable, c_update_device, 1, csErr);
			FreeFileUpdate();

			if ( iRet < 0 )
			{
				m_csDevLog.Format(_T("[设备控制线程]: 本地播报数据库提取数据异常(%s)"), csErr );
				GetGlobalApp()->RecordLog(m_csDevLog);
			}

			dwLocalDBCheck = ::GetTickCount();
		}
	}
	else//当前获取的时间片小于上一次，已到溢出周期
	{
		dwLocalDBCheck = dwCurrent;	//重置上次时间片，等待下次循环处理
	}
}



void CThreadDevice::ArrayCheck()
{
	CString	csErr = _T("");
	int iSet = 0;

	DWORD	dwCurrent = ::GetTickCount();
	if ( dwCurrent >= dwArrayCheck ) //正常计时，未到49.7天的溢出周期
	{
		if ( dwCurrent - dwArrayCheck >= nArraySpan) //默认 60 S检测一次
		{
			m_csDevLog.Format(_T("[设备控制线程]: 播报指令队列处理"));
			GetGlobalApp()->RecordLog(m_csDevLog);

			if ( c_update_device.m_aryUpdateInfo.GetSize() > 0 )
			{
				bool	bFind = false;
				int		iPos = 0;
				for (int i = 0; i < c_update_device.m_aryUpdateInfo.GetSize(); ++i)
				{
					if ( 0 == c_update_device.m_aryUpdateInfo[i].TransferMark )
					{
						bFind = true;
						iPos = i;
						break;
					}
				}
				if ( bFind ) //还有未更新播报
				{
					//判断是否终端欢迎界面更新指令
					if (0x03 == c_update_device.m_aryUpdateInfo[iPos].Type)
					{
						GGPublish( iPos );
					}
					else
					{
						CTime	ctCurDate = CTime::GetCurrentTime();
						CTime	ctBegDate( c_update_device.m_aryUpdateInfo[iPos].BeginDate );
						CTime	ctEndDate( c_update_device.m_aryUpdateInfo[iPos].EndDate );

						int nCurDate = ctCurDate.GetYear() * 10000 +	ctCurDate.GetMonth() * 100 +	ctCurDate.GetDay();
						int nBegDate = ctBegDate.GetYear() * 10000 +	ctBegDate.GetMonth() * 100 +	ctBegDate.GetDay();
						int nEndDate = ctEndDate.GetYear() * 10000 +	ctEndDate.GetMonth() * 100 +	ctEndDate.GetDay();

						if ( nBegDate <= nEndDate ) //开始日期小于等于结束日期（可能为同一天）
						{
							if ( nCurDate < nBegDate ) //时间周期未到，等待
							{
								::Sleep(20);
							}
							else if ( nCurDate > nEndDate ) //时间周期已过，删除
							{
								//1051版本调整，增加对播报异常的本地标记更新处理
								c_update_device.m_aryUpdateInfo[iPos].TransferMark = 1;//播报异常也设置标记，不再处理等待统一更新
								c_update_device.ErrorCount = 0;

								if (c_update_device.m_aryUpdateInfo[iPos].BeginDate > 0 && c_update_device.m_aryUpdateInfo[iPos].EndDate > 0)
								{
									csErr.Format( _T("[设备控制线程]: 获取到信息(%d, %s)时间周期(Begin:%s; End:%s)已过，不再处理"), \
									              c_update_device.m_aryUpdateInfo[iPos].Type, c_update_device.m_aryUpdateInfo[iPos].InfoNo,
									              CTime(c_update_device.m_aryUpdateInfo[iPos].BeginDate).Format(_T("%Y-%m-%d %H:%M:%S")), CTime(c_update_device.m_aryUpdateInfo[iPos].EndDate).Format(_T("%Y-%m-%d %H:%M:%S")) );
								}
								else
								{
									csErr.Format( _T("[设备控制线程]: 获取到信息(%d, %s)时间周期(Begin:%I64d; End:%I64d)已过，不再处理"), \
									              c_update_device.m_aryUpdateInfo[iPos].Type, c_update_device.m_aryUpdateInfo[iPos].InfoNo,
									              c_update_device.m_aryUpdateInfo[iPos].BeginDate, c_update_device.m_aryUpdateInfo[iPos].EndDate);
								}
								GetGlobalApp()->RecordLog( csErr );
							}
							else//时间周期正常，进行播放
							{
								GGPublish( iPos );
							}
						}
						else
						{
							//1051版本调整，增加对播报异常的本地标记更新处理
							c_update_device.m_aryUpdateInfo[iPos].TransferMark = 1;//播报异常也设置标记，不再处理等待统一更新
							c_update_device.ErrorCount = 0;

							csErr.Format( _T("[设备控制线程]: 获取到信息(%d, %s)时间周期(Begin:%I64d; End:%I64d)异常，不再处理"), \
							              c_update_device.m_aryUpdateInfo[iPos].Type, c_update_device.m_aryUpdateInfo[iPos].InfoNo,
							              c_update_device.m_aryUpdateInfo[iPos].BeginDate, c_update_device.m_aryUpdateInfo[iPos].EndDate );
							GetGlobalApp()->RecordLog( csErr );
						}
					}
				}
				else//播报已经全部更新
				{
					int iRet = 0;

					LockFileUpdate();
					iRet = SetLocalDBTransMark( m_csUpdateLDB, m_csLDBTable, c_update_device, csErr );
					FreeFileUpdate();

					if ( 0 == iRet )
					{
						c_update_device.ErrorCount = 0;
						c_update_device.m_aryUpdateInfo.RemoveAll();

						GetGlobalApp()->m_pThreadManage->m_pThreadUpdate->PostThreadMessage(WM_THREAD_UPDATE_CONFIRM, (WPARAM)(LPCTSTR)c_update_device.UpdateCode, (LPARAM)0);
					}
					else
					{
						c_update_device.ErrorCount += 1;
					}

					if ( c_update_device.ErrorCount > 3 )
					{
						LockFileUpdate();
						::DeleteFile( m_csUpdateLDB );
						FreeFileUpdate();

						c_update_device.ErrorCount = 0;
						c_update_device.m_aryUpdateInfo.RemoveAll();

						csErr.Format( _T("[设备控制线程]: 执行信息播报异常超过三次，删除当前更新信息!") );
						GetGlobalApp()->RecordLog( csErr );
					}
				}
			}

			dwArrayCheck = ::GetTickCount();
		}
	}
	else//当前获取的时间片小于上一次，已到溢出周期
	{
		dwArrayCheck = dwCurrent;	//重置上次时间片，等待下次循环处理
	}
}



void CThreadDevice::GGPublish(int nPos)
{
	CString csErr = _T("");
	CString csData = _T("");

	//XML构造
	try
	{
		if ( c_update_device.m_aryUpdateInfo.GetSize() <= nPos )
		{
			c_update_device.m_aryUpdateInfo.RemoveAll();
			csErr.Format( _T("[设备控制线程]: 播报指令队列异常，删除后重试！") );
			GetGlobalApp()->RecordLog( csErr );
			return;
		}

		int nType = c_update_device.m_aryUpdateInfo[nPos].Type;
		switch ( nType )
		{
		case 1:			nType = 102;			break;		//文字广告
		case 2:			nType = 103;			break;		//语音广告
		case 3:			nType = 104;			break;		//控制欢迎界面
		default:		nType = 102;			break;
		}

		CMarkup xmlShow;
		xmlShow.SetDoc( _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n") );
		xmlShow.AddElem( _T("DisplayContext") );//root
		xmlShow.IntoElem();

		xmlShow.AddElem( _T("Version"),		_T("1.0") );
		xmlShow.AddElem( _T("ProvinceID"),	m_sTranstParam.ProvinceID );
		xmlShow.AddElem( _T("VerifyCode"),	0 );

		xmlShow.AddElem( _T("Data") );
		xmlShow.IntoElem();
		xmlShow.AddElem( _T("Type"),		nType );
		xmlShow.AddElem( _T("Context"),		c_update_device.m_aryUpdateInfo[nPos].Content );
		xmlShow.OutOfElem();

		xmlShow.OutOfElem();
		csData = xmlShow.GetDoc();

		//更新默认广告内容
		if (    (0 != strlen(c_update_device.m_aryUpdateInfo[nPos].Content))
		        && (102 == nType)
		   )
		{
			m_strDefaultGGText.Format(_T("%s"), c_update_device.m_aryUpdateInfo[nPos].Content);
			m_strDefaultGGText.TrimLeft();
			m_strDefaultGGText.TrimRight();
		}
	}
	catch (...)
	{
		csErr.Format( _T("[设备控制线程]: 构造播报XML数据未知异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csErr );
		return;
	}


	//底层命令发送
	try
	{
		if (NULL == m_pHTSDDeviceDll)
		{
			return;
		}
		if (!m_pHTSDDeviceDll->GetDevInitState())
		{
			return;
		}

		csData.Replace(_T("\r"), _T(""));
		csData.Replace(_T("\n"), _T(""));

		//设置显示交互信息
		int iID = 0x01;
		if ( m_pHTSDDeviceDll->SetDisplayContext(iID, csData) )
		{
			c_update_device.m_aryUpdateInfo[nPos].TransferMark = 1;//播报成功
			c_update_device.ErrorCount = 0;

			m_csDevLog.Format(_T("[设备控制线程]: 播报指令(%s)发送成功"), csData );
			GetGlobalApp()->RecordLog(m_csDevLog);
		}
		else
		{
			c_update_device.ErrorCount += 1;

			m_csDevLog.Format(_T("[设备控制线程]: 播报指令(%s)发送失败"), csData );
			GetGlobalApp()->RecordLog(m_csDevLog);
		}

		if ( c_update_device.ErrorCount > 3 )
		{
			LockFileUpdate();
			::DeleteFile( m_csUpdateLDB );
			FreeFileUpdate();

			c_update_device.ErrorCount = 0;
			c_update_device.m_aryUpdateInfo.RemoveAll();

			csErr.Format( _T("[设备控制线程]: 执行信息播报异常超过三次，删除当前更新信息!") );
			GetGlobalApp()->RecordLog( csErr );
		}

	}
	catch (...)
	{
		csErr.Format( _T("[设备控制线程]: 执行信息播报命令未知异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csErr );
		return;
	}
}


int CThreadDevice::GetLocalDB2Array(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, int nType, CString& csError)
{
	int	nReturn = 0;
	int rc, i, ncols;
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *tail;
	CString csSQL = _T("");

	errno_t err = 0;
	if ((err = _access_s( strDBFile, 0 )) == 0 )
	{
		rc = sqlite3_agent::sqlite3_open_fun( strDBFile, &db);	//SQLITE_OK
		if (rc != SQLITE_OK)
		{
			csError.Format( _T("Can't open DB(%s) database: %s\n"), strDBFile, sqlite3_agent::sqlite3_errmsg_fun(db) );
			sqlite3_agent::sqlite3_close_fun(db);
			return -1;
		}

		if ( 0 == nType ) //不增加条件选取
		{
			csSQL.Format( _T(" select * from %s limit 9; "), strTBName);//目前最多只会有两条数据
		}
		else//条件选取
		{
			csSQL.Format( _T(" select * from %s where (TransferMark&1)<>1 limit 9; "), strTBName);//目前最多只会有两条数据
		}
		rc = sqlite3_agent::sqlite3_prepare_fun(db, csSQL.GetBuffer(0), csSQL.GetLength(), &stmt, &tail);	//SQLITE_OK
		csSQL.ReleaseBuffer();
		if (rc != SQLITE_OK)
		{
			csError.Format( _T(" DB(%s) table(%s) select SQL prepare error: %s\n"), strDBFile, strTBName, sqlite3_agent::sqlite3_errmsg_fun(db) );
			sqlite3_agent::sqlite3_close_fun(db);
			return -2;
		}

		rc = sqlite3_agent::sqlite3_step_fun(stmt);			//SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW, SQLITE_ERROR, SQLITE_MISUSE
		ncols = sqlite3_agent::sqlite3_column_count_fun(stmt);

		if ( rc == SQLITE_ROW)	//至少一条数据
		{
			do
			{
				CString csDateName = _T("");
				tagUpdateInfo	sData;
				::memset(&sData, 0, sizeof(tagUpdateInfo));

				for (i = 0; i < ncols; ++i)
				{
					//获取列原始名称，用 sqlite3_column_origin_name，否则用_dll sqlite3_column_name_dll 获取临时名称
					csDateName.Format( _T("%s"), sqlite3_agent::sqlite3_column_origin_name_fun(stmt, i) );

					//InfoType int NOT NULL,	InfoNo varchar(16),	InfoName varchar(32),	InfoVersion varchar(16),
					if ( 0 == csDateName.CompareNoCase("InfoType") )			{	sData.Type = sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else if ( 0 == csDateName.CompareNoCase("InfoNo") )		{::sprintf(sData.InfoNo, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					else if ( 0 == csDateName.CompareNoCase("InfoName") )	{::sprintf(sData.InfoName, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					else if ( 0 == csDateName.CompareNoCase("InfoVersion") )	{::sprintf(sData.InfoVersion, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}

					//UpdateTime datetime,	LoadTime datetime,	BeginDate datetime,	EndDate datetime,
					else if ( 0 == csDateName.CompareNoCase("UpdateTime") )	{	sData.UpdateTime	= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}
					else if ( 0 == csDateName.CompareNoCase("LoadTime") )	{	sData.LoadTime		= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}
					else if ( 0 == csDateName.CompareNoCase("BeginDate") )	{	sData.BeginDate		= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}
					else if ( 0 == csDateName.CompareNoCase("EndDate") )		{	sData.EndDate		= theApp.CString2time_t( (LPCTSTR)sqlite3_agent::sqlite3_column_text_fun(stmt, i) );	}

					//IsCondense int,	FileType int,	Content varchar(128),	MD5 varchar(40),
					else if ( 0 == csDateName.CompareNoCase("IsCondense") )	{	sData.IsCondense = sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else if ( 0 == csDateName.CompareNoCase("FileType") )	{	sData.FileType = sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else if ( 0 == csDateName.CompareNoCase("Content") )		{::sprintf(sData.Content, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					else if ( 0 == csDateName.CompareNoCase("MD5") )			{::sprintf(sData.MD5, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}

					//Spare int,	SpareString varchar(128),	TransferMark int,
					else if ( 0 == csDateName.CompareNoCase("Spare") )		{	sData.Spare	= sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					//	else if( 0 == csDateName.CompareNoCase("SpareString") )	{::sprintf(sData.sp, "%s", sqlite3_agent::sqlite3_column_text_fun(stmt, i));}
					//	else if( 0 == csDateName.CompareNoCase("TransferMark") ){	sData.TransferMark= sqlite3_agent::sqlite3_column_int_fun(stmt, i);}
					else
					{
						// default:	in this case , do nothing!
					}
				}

				rc = sqlite3_agent::sqlite3_step_fun(stmt);
				nReturn += 1;
				//屏蔽掉远程播控（广告文字/语音）
				if (3 == sData.Type)
				{
					sData.TransferMark = 0;	//用于队列中的命令发送控制
					cUpdate.m_aryUpdateInfo.Add( sData );
				}
			}
			while (rc == SQLITE_ROW);
		}
		else //SQLITE_DONE, SQLITE_BUSY, SQLITE_ERROR	SQLite处理 无数据、忙、或者错误
		{
			if ( rc == SQLITE_DONE )
			{
				nReturn = 0;	// no data to transfer
			}
			else
			{
				csError.Format( _T(" DB(%s) table(%s) SQL step error: %s\n"), strDBFile, strTBName, sqlite3_agent::sqlite3_errmsg_fun(db) );
				nReturn = -3;	// error in the select
			}
		}

		sqlite3_agent::sqlite3_finalize_fun(stmt);	//SQLITE_OK	SQLITE_ABORT	SQLITE_BUSY
		sqlite3_agent::sqlite3_close_fun(db);		//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY
	}
	else
	{
		nReturn = -9;
	}

	return nReturn;
}

int CThreadDevice::SetLocalDBTransMark(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, CString& csError )
{
	sqlite3 *db;
	char *zErr;
	int rc;
	CString csSQL = _T("");
	CString csTemp = _T("");

	errno_t err = 0;
	if ((err = _access_s( strDBFile, 0 )) == 0 )
	{
		rc = sqlite3_agent::sqlite3_open_fun( strDBFile, &db);
		if ( rc != SQLITE_OK )
		{
			csError.Format( _T("Can't open DB(%s) database: %s\n"), strDBFile, sqlite3_agent::sqlite3_errmsg_fun(db) );
			sqlite3_agent::sqlite3_close_fun(db);
			return -1;
		}

		//update the table		PRIMARY KEY(StartTime,SerialNum,DeviceID))
		for (int i = 0; i < cUpdate.m_aryUpdateInfo.GetSize(); ++i)
		{
			csSQL.Format( _T(" update %s set TransferMark=(TransferMark | 1) where InfoType=%d and InfoNo='%s' ;"), \
			              strTBName, cUpdate.m_aryUpdateInfo[i].Type, cUpdate.m_aryUpdateInfo[i].InfoNo );
			rc = sqlite3_agent::sqlite3_exec_fun(db, csSQL.GetBuffer(0), NULL, NULL, &zErr);
			csSQL.ReleaseBuffer();

			if ( rc != SQLITE_OK )
			{
				if (zErr != NULL)
				{
					csError.Format( _T(" DB(%s) table(%s) update transmark SQL error: %s"), strDBFile, strTBName, zErr );
					sqlite3_agent::sqlite3_free_fun(zErr);
				}

				sqlite3_agent::sqlite3_close_fun(db);	//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY
				return -3;

			}
			else
			{
				continue;
			}
		}
		sqlite3_agent::sqlite3_close_fun(db);
		return 0;
	}
	else
	{
		return -9;	// no database
	}
}


void CThreadDevice::RestartNFCPay()
{
	if (!m_pHTSDDeviceDll->IssuedData(m_LastNFCPayData))
		return ;
	//设置银联支付状态
	GetGlobalApp()->SetUnipayWorkFlowState(UMS_DEBITING);
}

void CThreadDevice::RemoteControlFirmware(const std::string&para)
{
	m_pHTSDDeviceDll->UpgradeFirware(para.c_str());
}

afx_msg void CThreadDevice::OnMsgRemoteUpgradeResult(WPARAM wParam, LPARAM lParam)
{	//	固件升级结果
	//获取到固件升级结果后，需要重新获取硬件版本号.
	theApp.m_strTWProductFirmwareVer = ""; //此处新加的变量处理，与m_strTWProductFirmwareVer原有的使用区域不会发生冲突，因此不用使用控制资源（添加多线程控制，也相对复杂）
	theApp.m_strTWProductSlaveFirmwareVer = "";

	m_firmwareUpdateFlag = true;
	m_firmwareUpdateTime = time(NULL);
	::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_REMOTE_UPRGADE_RESULT, wParam, lParam);
}