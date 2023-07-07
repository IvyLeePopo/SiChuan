/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 *
 * 文件名称：HTSDDeviceDll2.cpp
 * 摘    要：TR300模块加载类
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年5月3日
 */
#include "StdAfx.h"
#include "HTSDDeviceDll2.h"
#include "TWSDNetPay_Dll.h"

CHTSDDeviceDll2::CHTSDDeviceDll2(void)
	: m_hModule(NULL)
	, m_bInitDll(FALSE)
	, m_bInitDev(FALSE)
	, m_strLastError(_T(""))
	, fnIF_InitDev(NULL)
	, fnIF_Destroy(NULL)
	, fnIF_GetLastErrorDesc(NULL)
	, fnIF_QueryDevice(NULL)
	, fnIF_SetDevice(NULL)
	, fnIF_DebitMoney(NULL)
	, fnIF_DebitCancel(NULL)
	, fnIF_InitDeviecCMD(NULL)
	, fnIF_AgencyDebitQuery(NULL)
	, fnIF_TransferImage(NULL)
	, fnIF_TransTickNo(NULL)
	, fn_GreenChannelTest(NULL)
	, fnIF_ExcuteComCommond(NULL)
	, fnIF_TakeComFeedbackCommond(NULL)
	,fnIF_TerminalSystemVersionStatus(NULL)
	,fnIF_ExchangeTerminalSystem(NULL)
{
}

CHTSDDeviceDll2::~CHTSDDeviceDll2(void)
{
	FreeLibrary(m_hModule);
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSDNetPayDll::Init
//
// 功能描述：初始化动态库
//
// 输入参数：IN const CString& strDllFilePath
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSDDeviceDll2::Init(IN const CString& strDllFilePath)
{
	//获取动态库文件所在路径目录
	CString strDir = strDllFilePath.Left(strDllFilePath.ReverseFind('\\'));
	//获取当前工作目录
	TCHAR szOldDir[260] = {0};
	::GetCurrentDirectory(260, szOldDir);
	//设置工作目录
	//remove 2018-04 ::SetCurrentDirectory(strDir);

	//加载动态库
	if (NULL != (m_hModule = ::LoadLibrary(strDllFilePath)))
	{
		fnIF_InitDev				= (defIF_InitDev)::GetProcAddress(m_hModule, "IF_InitDev");
		fnIF_Destroy				= (defIF_Destroy)::GetProcAddress(m_hModule, "IF_Destroy");
		fnIF_GetLastErrorDesc		= (defIF_GetLastErrorDesc)::GetProcAddress(m_hModule, "IF_GetLastError");

		fnIF_QueryDevice			= (defIF_QueryDevice)::GetProcAddress(m_hModule, "IF_QueryDevice");
		fnIF_SetDevice				= (defIF_SetDevice)::GetProcAddress(m_hModule, "IF_SetDevice");
		fnIF_DebitMoney				= (defIF_StartDebit)::GetProcAddress(m_hModule, "IF_StartDebit");
		fnIF_DebitCancel			= (defIF_StartDebitCancle)::GetProcAddress(m_hModule, "IF_StartDebitCancle");
		fnIF_InitDeviecCMD          = (defIF_InitPara)::GetProcAddress(m_hModule, "IF_InitPara");

		fnIF_BreakDebit          = (defIF_BreakDebit)::GetProcAddress(m_hModule, "IF_BreakDebit");

		fnIF_CheckDeviceStatus		= (defIF_CheckDeviceStatus)::GetProcAddress(m_hModule, "IF_CheckDeviceStatus");
		fnIF_AgencyDebitQuery       = (defIF_AgencyDebitQuery)::GetProcAddress(m_hModule, "IF_AgencyDebitQuery");
		fnIF_TransferImage          = (defIF_TransferImage)::GetProcAddress(m_hModule, "IF_TransferImage");
		fnIF_TransTickNo            = (defIF_TransTickNo)::GetProcAddress(m_hModule, "IF_TransTickNo");
		fn_GreenChannelTest         = (def_GreenChannelTest)::GetProcAddress(m_hModule, "GreenChannelTest");
		fnIF_ExcuteComCommond = (defIF_ExcuteComCommond )::GetProcAddress(m_hModule,"IF_ExcuteComCommond");
		fnIF_TakeComFeedbackCommond = (defIF_TakeComFeedbackCommond )::GetProcAddress(m_hModule,"IF_TakeComFeedbackCommond");

		fnIF_TerminalSystemVersionStatus = (defIF_TerminalSystemVersionStatus)::GetProcAddress(m_hModule,"IF_TerminalSystemVersionStatus");
		fnIF_ExchangeTerminalSystem = (defIF_ExchangeTerminalSystem)::GetProcAddress(m_hModule,"IF_ExchangeTerminalSystem");


		if (	   (NULL != fnIF_InitDev)
		           && (NULL != fnIF_Destroy)
		           && (NULL != fnIF_GetLastErrorDesc)

		           && (NULL != fnIF_QueryDevice)
		           && (NULL != fnIF_SetDevice)
		           && (NULL != fnIF_DebitMoney)
		           && (NULL != fnIF_DebitCancel)
		           && (NULL != fnIF_InitDeviecCMD)

		           && (NULL != fnIF_BreakDebit)
		           && (NULL != fnIF_CheckDeviceStatus)
		   )
		{
			m_bInitDll = TRUE;
		}
	}
	else
	{
		char debug_chs[100] = {0};
		sprintf(debug_chs, "Load error(%d):%s", GetLastError(), strDllFilePath);
		GetGlobalApp()->RecordLog(debug_chs);
	}
	//回复工作目录
	//remove 2018-04::SetCurrentDirectory(szOldDir);
	//返回执行状态
	return m_bInitDll;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::InitDev
//
// 功能描述：组件初始化
//
// 输入参数：无
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::InitDev(void (*NotifyFun)(int type, const std::string& sendStr))
{
	if (m_bInitDll)
	{
		m_bInitDev = fnIF_InitDev(NotifyFun);
		return m_bInitDev;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::Destroy
//
// 功能描述：清理资源
//
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::Destroy(void)
{
	if (m_bInitDll)
	{
		m_bInitDev = false;
		return fnIF_Destroy();
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::GetLastErrorDesc
//
// 功能描述：获取上一次错误信息
//
// 输入参数：void
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::GetLastErrorDesc(IN OUT CString& strLastDesc)
{
	if (m_bInitDll && m_bInitDev)
	{
		//std::string strError;
		if (fnIF_GetLastErrorDesc(strLastDesc))
		{
			//strLastDesc.Format(_T("%s"),strError.c_str());
			return TRUE;
		}
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::CheckDeviceStatus
//
// 功能描述：执行检测硬件状态操作
//
// 输入参数：IN OUT DWORD& dwState
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::CheckDeviceStatus(IN OUT DWORD& dwState)
{
	if (m_bInitDll && m_bInitDev)
		return fnIF_CheckDeviceStatus(dwState);
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::DebitMoney
//
// 功能描述：执行扣款操作
//
// 输入参数：CString& strDebitInfo
//         ：int iType
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::DebitMoney(IN const CString& strDebitInfo, IN OUT CString& strResultInfo, IN const int& inType)
{
	if (m_bInitDll && m_bInitDev)
	{
		std::string strDebit((LPCTSTR)strDebitInfo);

		return fnIF_DebitMoney(strDebit, inType, strResultInfo);
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::DebitCancel
//
// 功能描述：执行退款操作
//
// 输入参数：IN const CString& strCancelInfo
//         ：IN const int& inType
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::DebitCancel(IN const CString& strCancelInfo, IN const int& inType)
{
	if (m_bInitDll && m_bInitDev)
	{
		std::string strCancel((LPCTSTR)strCancelInfo);
		return fnIF_DebitCancel(strCancel, inType);
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll2::InitConfigDevice
//
// 功能描述：执行设备初始化操作
//
// 输入参数：IN const CString& strInitInfo
//         ：IN const int& inType
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年5月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll2::InitConfigDevice(IN const CString& strInitInfo, IN const int& inType)
{
	if (m_bInitDll && m_bInitDev)
	{
		std::string strInit((LPCTSTR)strInitInfo);
		return fnIF_InitDeviecCMD(strInit, inType);
	}
	return FALSE;
}


BOOL CHTSDDeviceDll2::SetDevice(IN const CString& para)
{

	if (m_bInitDll && m_bInitDev)
	{
		std::string strInit((LPCTSTR)para);
		return fnIF_SetDevice(strInit, 0);
	}
	return FALSE;
}



bool CHTSDDeviceDll2::BreakDebit(const CString& payID)
{
	if (m_bInitDll && m_bInitDev)
	{
		std::string strPayid((LPCTSTR)payID);
		return fnIF_BreakDebit(strPayid);
	}
	return false;
}

bool CHTSDDeviceDll2::AgencyDebitQuery(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult)
{
	if (m_bInitDll && m_bInitDev && fnIF_AgencyDebitQuery != NULL)
	{
		return fnIF_AgencyDebitQuery(szParamContext, iFormatType, iResult);
	}
	return false;
}

bool CHTSDDeviceDll2::TransferImage(const std::string& szImageInfo)
{
	if (m_bInitDll && m_bInitDev && fnIF_TransferImage != NULL)
	{
		return fnIF_TransferImage(szImageInfo);
	}
	return false;
}

bool CHTSDDeviceDll2::TransTickNo(const std::string& para)
{
	if (m_bInitDll && m_bInitDev && fnIF_TransTickNo != NULL)
	{
		return fnIF_TransTickNo(para);
	}
	return false;
}

bool CHTSDDeviceDll2::GreenChannelTest(int testType, const std::string& testPara, std::string& result)
{
	if (m_bInitDll && m_bInitDev && fn_GreenChannelTest != NULL)
	{
		char chs[1000] = {0};
		bool ret = fn_GreenChannelTest(testType, testPara, chs);
		//std::copy(chs,chs+strlen(chs),std::back_inserter(result));
		result = chs;
		return ret;
	}
	return false;
}

bool CHTSDDeviceDll2::ExcuteComCommond(const std::string& hexStr)
{
	
	if (m_bInitDll && m_bInitDev && fnIF_ExcuteComCommond != NULL)
	{
		char chs[1000] = {0};
		bool ret = fnIF_ExcuteComCommond(hexStr);
		return ret;
	}
	return false;
}
bool CHTSDDeviceDll2::TakeComFeedbackCommond(CString& jsonStr)
{
	if (m_bInitDll && m_bInitDev && fnIF_TakeComFeedbackCommond != NULL)
	{
		char chs[1000] = {0};
		bool ret = fnIF_TakeComFeedbackCommond(jsonStr);
		
		
		return ret;
	}
	return false;
}



bool CHTSDDeviceDll2::QueryTerminalSystemVersionStatus(int& status)
{
	if(m_bInitDll &&m_bInitDev&& fnIF_TerminalSystemVersionStatus!=NULL)
		return fnIF_TerminalSystemVersionStatus(status);
	
	return false;
}
bool CHTSDDeviceDll2::ExchangeTerminalSystem(const int& targetSystemVersion)
{
	if(m_bInitDll &&m_bInitDev&& fnIF_ExchangeTerminalSystem!=NULL)
		return fnIF_ExchangeTerminalSystem(targetSystemVersion);
	return false;
}