#include "StdAfx.h"
#include "HTSDDeviceDll.h"


CHTSDDeviceDll::CHTSDDeviceDll(void)
	:m_hModule(NULL)
	,m_bInitDll(FALSE)
	,m_bInitDev(FALSE)
	,m_strLastError(_T(""))
	,fnIF_InitDev(NULL)
	,fnIF_Destroy(NULL)
	,fnIF_GetLastErrorDesc(NULL)
	,fnIF_StartGetData(NULL)
	,fnIF_StopGetData(NULL)
	,fnIF_CheckDeviceStatus(NULL)
	,fnIF_GetData(NULL)
	,fnIF_Authority(NULL)
	,fnIF_GetDeviceFirewareVer(NULL)
	,fnIF_GetDisplayContext(NULL)
	,fnIF_SetDisplayContext(NULL)
	,fnIF_IssuedData(NULL)
	,fnIF_SetIOState(NULL)
	,fnIF_UpgradeFirware(NULL)
	,fnIF_IsUpgrading(NULL)
	,fnIF_RealFirmwareVer(NULL)
	,fnIF_RealSlaveFirmwareVer(NULL)
	,fnIF_TerminalSystemVersionStatus(NULL)
	,fnIF_ExchangeTerminalSystem(NULL)
{
}

CHTSDDeviceDll::~CHTSDDeviceDll(void)
{
	this->Destroy();
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CAgencyPay::Init
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
const BOOL CHTSDDeviceDll::Init(IN const CString& strDllFilePath)
{
	//获取动态库文件所在路径目录
	CString strDir = strDllFilePath.Left(strDllFilePath.ReverseFind('\\'));
	//获取当前工作目录
	TCHAR szOldDir[260] = {0};
	::GetCurrentDirectory(260,szOldDir);
	//设置工作目录
	//remove 2018-04 ::SetCurrentDirectory(strDir);
	//加载动态库
	if(NULL != (m_hModule = ::LoadLibrary(strDllFilePath)))
	{
		fnIF_InitDev				= (defIF_InitDev)::GetProcAddress(m_hModule,"IF_InitDev");
		fnIF_Destroy				= (defIF_Destroy)::GetProcAddress(m_hModule,"IF_Destroy");
		fnIF_GetLastErrorDesc		= (defIF_GetLastErrorDesc)::GetProcAddress(m_hModule,"IF_GetLastErrorDesc");
		fnIF_StartGetData			= (defIF_StartGetData)::GetProcAddress(m_hModule,"IF_StartGetData");
		fnIF_StopGetData			= (defIF_StopGetData)::GetProcAddress(m_hModule,"IF_StopGetData");
		fnIF_CheckDeviceStatus		= (defIF_CheckDeviceStatus)::GetProcAddress(m_hModule,"IF_CheckDeviceStatus");
		fnIF_GetData				= (defIF_GetData)::GetProcAddress(m_hModule,"IF_GetData");
		fnIF_Authority				= (defIF_Authority)::GetProcAddress(m_hModule,"IF_Authority");
		fnIF_GetDeviceFirewareVer	= (defIF_GetDeviceFirewareVer)::GetProcAddress(m_hModule,"IF_GetDeviceFirewareVer");
		fnIF_GetDisplayContext		= (defIF_GetDisplayContext)::GetProcAddress(m_hModule,"IF_GetDisplayContext");
		fnIF_SetDisplayContext		= (defIF_SetDisplayContext)::GetProcAddress(m_hModule,"IF_SetDisplayContext");
		fnIF_IssuedData				= (defIF_IssuedData)::GetProcAddress(m_hModule,"IF_IssuedData");
		fnIF_SetIOState				= (defIF_SetIOState)::GetProcAddress(m_hModule,"IF_SetIOState");

		//add 2017-12
		fnIF_UpgradeFirware         = (defIF_UpgradeFirware)::GetProcAddress(m_hModule,"IF_UpgradeFirware");
		fnIF_IsUpgrading            = (defIF_IsUpgrading)::GetProcAddress(m_hModule,"IF_IsUpgrading");
		fnIF_RealFirmwareVer            = (defIF_RealFirmwareVer)::GetProcAddress(m_hModule,"IF_RealDeviceFirmwareVer");
		fnIF_RealSlaveFirmwareVer            = (defIF_RealSlaveFirmwareVer)::GetProcAddress(m_hModule,"IF_RealSlaveDeviceFirmwareVer");
		fnIF_TerminalSystemVersionStatus = (defIF_TerminalSystemVersionStatus)::GetProcAddress(m_hModule,"IF_TerminalSystemVersionStatus");
		fnIF_ExchangeTerminalSystem = (defIF_ExchangeTerminalSystem)::GetProcAddress(m_hModule,"IF_ExchangeTerminalSystem");

		if(		(NULL != fnIF_InitDev)
			 && (NULL != fnIF_Destroy)
			 && (NULL != fnIF_GetLastErrorDesc)
			 && (NULL != fnIF_StartGetData)
			 && (NULL != fnIF_StopGetData)
			 && (NULL != fnIF_CheckDeviceStatus)
			 && (NULL != fnIF_GetData)
			 && (NULL != fnIF_Authority)
			 && (NULL != fnIF_GetDeviceFirewareVer)
			 && (NULL != fnIF_GetDisplayContext)
			 && (NULL != fnIF_SetDisplayContext)
			 && (NULL != fnIF_IssuedData)
			 && (NULL != fnIF_SetIOState)
			 )
		{
			m_bInitDll = TRUE;
		}
	}
	//回复工作目录
	//remove 2018-04 ::SetCurrentDirectory(szOldDir);
	//返回执行状态



	return m_bInitDll;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::InitDev
//     
// 功能描述：组件初始化
//     
// 输入参数：IN const UINT& nThreadID
//         ：IN const HWND& hWnd
//         ：IN const unsigned int& nNotifyMsgID
//         ：IN const int& iProvinceID
//         ：IN const int& iOverTime
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月24日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::InitDev(IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const int& iProvinceID,IN const int& iOverTime)
{
	if(m_bInitDll)
	{
		m_bInitDev = fnIF_InitDev(nThreadID,hWnd,nNotifyMsgID,iProvinceID,iOverTime);
		return m_bInitDev;
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::Destroy
//     
// 功能描述：清理资源
//     
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::Destroy(void)
{
	if(m_bInitDll)
	{
		BOOL bRet = fnIF_Destroy();
		
		if( m_hModule )
		{
			::FreeLibrary( m_hModule );
		}
		m_hModule = NULL;
		m_bInitDll= false;
		return bRet;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::GetLastErrorDesc
//     
// 功能描述：获取上一次错误信息
//     
// 输入参数：void
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CHTSDDeviceDll::GetLastErrorDesc(void)
{
	if(m_bInitDll)
		return fnIF_GetLastErrorDesc();
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::StartGetData
//     
// 功能描述：执行启动获取数据操作
//     
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::StartGetData(IN const int& iOvertime)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_StartGetData(iOvertime);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::StopGetData
//     
// 功能描述：执行停止获取数据操作
//     
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::StopGetData(IN const int& iChangeMMI)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_StopGetData(iChangeMMI);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::CheckDeviceStatus
//     
// 功能描述：执行检测硬件状态操作
//     
// 输入参数：IN OUT DWORD& dwState
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::CheckDeviceStatus(IN OUT DWORD& dwState)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_CheckDeviceStatus(dwState);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::GetData
//     
// 功能描述：获取数据结果
//     
// 输入参数：IN OUT CString& strData
//         ：IN OUT int& iDataType
//         ：IN OUT int& iSrcType
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::GetData(IN OUT CString& strData,IN OUT int& iDataType,IN OUT int& iSrcType)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_GetData(strData,iDataType,iSrcType);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::Authority
//     
// 功能描述：权限认证操作
//     
// 输入参数：IN const CString& strData
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::Authority(IN const CString& strData)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_Authority(strData);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::GetDeviceFirewareVer
//     
// 功能描述：获取固件版本信息
//     
// 输入参数：IN OUT CString& strXML
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::GetDeviceFirewareVer(IN OUT CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_GetDeviceFirewareVer(strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::GetDisplayContext
//     
// 功能描述：获取显示内容信息
//     
// 输入参数：IN OUT int& iID
//         ：CString& strXML
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::GetDisplayContext(IN OUT int& iID, CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_GetDisplayContext(iID,strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::SetDisplayContext
//     
// 功能描述：设置显示内容信息
//     
// 输入参数：IN const int& iID
//         ：IN const CString& strXML
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年8月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::SetDisplayContext(IN const int& iID,IN const CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_SetDisplayContext(iID,strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSDDeviceDll::IssuedData
//     
// 功能描述：执行数据下发操作
//     
// 输入参数：IN const CString& strXML
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::IssuedData(IN const CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_IssuedData(strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：SetIOState
//     
// 功能描述：执行控制IO操作
//     
// 输入参数：IN const int& iIOState
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年7月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::SetIOState(IN const int& iIOState)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_SetIOState(iIOState);
	return FALSE;
}

BOOL CHTSDDeviceDll::UpgradeFirware(const CString& strJson)
{
	if(m_bInitDll && fnIF_UpgradeFirware!=NULL)
		return fnIF_UpgradeFirware(strJson);
	return FALSE;
}
BOOL CHTSDDeviceDll::IsUpgrading()
{
	if(m_bInitDll && fnIF_IsUpgrading!=NULL)
		return fnIF_IsUpgrading();
	return FALSE;
}

BOOL CHTSDDeviceDll::RealFirmwareVer(IN OUT CString& strXML)
{
	if(m_bInitDll && fnIF_RealFirmwareVer!=NULL)
		return fnIF_RealFirmwareVer(strXML);
	return false;
}

BOOL CHTSDDeviceDll::RealSlaveFirmwareVer(IN OUT CString& strXML)
{
	if(m_bInitDll && fnIF_RealSlaveFirmwareVer!=NULL)
		return fnIF_RealSlaveFirmwareVer(strXML);
	return false;
}

bool CHTSDDeviceDll::QueryTerminalSystemVersionStatus(int& status)
{
	if(m_bInitDll && fnIF_TerminalSystemVersionStatus!=NULL)
		return fnIF_TerminalSystemVersionStatus(status);
	
	return false;
}
bool CHTSDDeviceDll::ExchangeTerminalSystem(const int& targetSystemVersion)
{
	if(m_bInitDll && fnIF_ExchangeTerminalSystem!=NULL)
		return fnIF_ExchangeTerminalSystem(targetSystemVersion);
	return false;
}