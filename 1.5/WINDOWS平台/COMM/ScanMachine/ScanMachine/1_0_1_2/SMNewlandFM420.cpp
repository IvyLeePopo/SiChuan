/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：SMNewlandFM420.cpp
 * 摘    要：新大陆NSL-FM420扫码器通讯协议解析控制类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
#include "StdAfx.h"
#include "SMNewlandFM420.h"

CSMNewlandFM420::CSMNewlandFM420(IN const int& iSMType)
	:CScanMachineIF(iSMType)
{
	m_dwCodeLen = 18;
}

CSMNewlandFM420::~CSMNewlandFM420(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::AllowFunctionCode
//     
// 功能描述：开启或关闭设置码功能
//     
// 输入参数：IN const BOOL& bAllow
//         ：TRUE  - 开启
//         ：FALSE - 关闭
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::AllowFunctionCode(IN const BOOL& bAllow)
{
	CString strCommand;
	strCommand.Format(bAllow?_T("NLS0302000;"):_T("NLS0006000;"));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::ResetDefaultParam
//     
// 功能描述：重置为出厂参数
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::ResetDefaultParam(void)
{
	CString strCommand;
	strCommand.Format(_T("NLS0006000;"));
	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::ResetPrivateParam
//     
// 功能描述：重新设置为用户指定的模式
//     
// 输入参数：void
// 输出参数：const CString
// 编写人员：ROCY
// 编写时间：2015年11月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::ResetPrivateParam(IN OUT BYTE& byDataConvertType)
{
	CString strTemp;
	CString strCommand;
	
	//开启设置码功能
	strTemp		= AllowFunctionCode(TRUE);
	strCommand += strTemp;
	
	//设置扫码方式为命令触发
	strTemp		= SetScanMode();
	strCommand += strTemp;
	
	//设置灵敏度
//	strTemp		= SetAllowDecodeVoice(TRUE);
//	strCommand += strTemp;
	
	//设置声音
//	strTemp		= SetDecodeVoiceType(1);
//	strCommand += strTemp;
	
	//设置读码光照
//	strTemp		= SetSupplementalLightMode(1);
//	strCommand += strTemp;
	
	//设置读码对焦灯光
//	strTemp		= SetFocusingMode(1);
//	strCommand += strTemp;
	
	//设置读码延时时间
	strTemp		= SetDelayTime();
	strCommand += strTemp;
	
	//设置只允许扫描QR码
	strTemp		= AllowOnlyScanQRCode();
	strCommand += strTemp;
	
	//设置上述更改为用户默认设置
	strTemp		= SubmitPrivateParam();
	strCommand += strTemp;
	
	//关闭设置码功能
	strTemp = AllowFunctionCode(FALSE);
	strCommand += strTemp;

	strCommand = strCommand.Left(strCommand.GetLength() - 1);

	//设置转换数据格式为不需要
	byDataConvertType = 0;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::Start
//     
// 功能描述：开启扫码操作
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::Start()
{
	CString strCommand;
	strCommand.Format(_T("%02X%02X"),0x1B,0x31);
	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::Stop
//     
// 功能描述：停止扫码操作
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::Stop(void)
{
	CString strCommand;
	strCommand.Format(_T("%02X%02X"),0x1B,0x30);
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::MakeVoice
//     
// 功能描述：发出声音
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::MakeVoice(void)
{
	CString strCommand;
	strCommand.Format(_T(""));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::FormatReadData
//     
// 功能描述：格式化返回来的数据
//     
// 输入参数：IN OUT LPBYTE byData
//         ：IN const DWORD& dwSize
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年11月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString  CSMNewlandFM420::FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	CString strFormatData;
	if(!CheckCommandExecuteResult(byData,dwSize))
		return strFormatData;
	for(DWORD i=0;i<dwSize-1;++i)
	{
		byData[i] = byData[i+1];
	}
	--dwSize;
	strFormatData.Format(_T("%s"),(LPTSTR)byData);

	return strFormatData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::CheckCommandExecuteResult
//     
// 功能描述：检查命令执行结果是否正确
//     
// 输入参数：IN OUT LPBYTE byData
//         ：IN OUT DWORD& dwSize
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2015年12月7日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSMNewlandFM420::CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	if(    (dwSize>0)
		&& (0x06 == byData[0])
		)
	{
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::BeginSetPrivateParam
//     
// 功能描述：开启用户设置操作流程
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：本设备固件不支持该功能
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::BeginSetPrivateParam(void)
{
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SubmitPrivateParam
//     
// 功能描述：递交用户设置参数(保存)
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SubmitPrivateParam(void)
{
	CString strCommand;
	strCommand.Format(_T("NLS0001150;"));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::GetSysInfo
//     
// 功能描述：获取扫码器系统信息
//     
// 输入参数：IN OUT CString& strSysInfo
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::GetSysInfo(void)
{
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetRS232CommMode
//     
// 功能描述：串口通讯设置
//     
// 输入参数：IN const int& iRate = 9600
//         ：IN const int& iParityBit = 0
//         ：IN const int& iStopBit = 1
//         ：IN const int& iDataBit = 8
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetRS232CommMode(IN const int& iRate/* = 9600*/,IN const int& iParityBit/* = 0*/,IN const int& iStopBit/* = 1*/,IN const int& iDataBit/* = 8*/)
{
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::CheckDeviceStatus
//     
// 功能描述：检测设备状态
//     
// 输入参数：IN const CString& strState -- 设备状态描述
// 输出参数：const int
//         ：0		-	设备正常
//		   ：非0	-	设备异常
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const int	CSMNewlandFM420::CheckDeviceStatus(IN const CString& strState)
{
	return 0;
}

/************************************************************************************************************硬件设置************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetSupplementalLightMode
//     
// 功能描述：设置扫码补光模式
//     
// 输入参数：IN const int& iLightMode
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetSupplementalLightMode(IN const int& iLightMode)
{
	CString strCommand;
	switch(iLightMode)
	{
	case SM_FUN_SUPPORT_LIGHT_ALLOW:
		strCommand.Format(_T("NLS0200010;"));
		break;
	case SM_FUN_SUPPORT_LIGHT_FORBID:
		strCommand.Format(_T("NLS0200010;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetFocusingMode
//     
// 功能描述：设置对焦模式
//     
// 输入参数：IN const int& iMode
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetFocusingMode(IN const int& iMode)
{
	CString strCommand;
	switch(iMode)
	{
	case SM_FUN_FOCUS_LIGHT:
		strCommand.Format(_T("NLS0201000;"));
		break;
	case SM_FUN_FOCUS_FLICKER:
		strCommand.Format(_T("NLS0201000;"));
		break;
	case SM_FUN_FOCUS_NONE:
		strCommand.Format(_T("NLS0201000;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetAllowDecodeVoice
//     
// 功能描述：设置是否允许解码完毕后发声
//     
// 输入参数：IN const BOOL& bAllowVoice
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetAllowDecodeVoice(IN const BOOL& bAllowVoice)
{
	CString strCommand;
	strCommand.Format(bAllowVoice?_T("NLS0203010;"):_T("NLS0203010;"));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetDecodeVoiceType
//     
// 功能描述：设置解码完毕后的声音种类
//     
// 输入参数：IN const int& iVoiceType
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetDecodeVoiceType(IN const int& iVoiceType)
{
	CString strTemp;
	CString strCommand;
	strTemp.Format(_T("NLS0203020;"));//声音类型
	strCommand += strTemp;
	strTemp.Format(_T("NLS0203030;"));//音量为高
	strCommand += strTemp;
	return strCommand;
}

/************************************************************************************************************扫码模式设置************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetDelayTime
//     
// 功能描述：设置扫码超时时间
//     
// 输入参数：IN const int& iDealyTime=2000
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetDelayTime(IN const int& iDealyTime)
{
	CString strCommand;
	strCommand.Format(_T("NLS0313000=%d;"),iDealyTime);
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetDelayTimeMode
//     
// 功能描述：设置延时模式(完全延时：无论是否读取到数据都延时指定时间；非完全延时：有数据则返回)
//     
// 输入参数：IN const int& iDelayMode = 1
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetDelayTimeMode(IN const int& iDelayMode)
{
	CString strCommand;
	switch(iDelayMode)
	{
	case SM_FUN_DELAY_WAIT:
		strCommand.Format(_T("NLS0201000;"));
		break;
	case SM_FUN_DELAY_NO_WAIT:
		strCommand.Format(_T("NLS0201000;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetScanMode
//     
// 功能描述：设置扫描模式
//     
// 输入参数：IN const int& iScanMode = 1
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetScanMode(IN const int& iScanMode)
{
	CString strCommand;
	switch(iScanMode)
	{
	case SM_FUN_SCAN_CMD:
		strCommand.Format(_T("NLS0302000;"));
		break;
	case SM_FUN_SCAN_AUTO:
		strCommand.Format(_T("NLS0302000;"));
		break;
	case SM_FUN_SCAN_MANUAL:
		strCommand.Format(_T("NLS0302000;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetSensitivityRate
//     
// 功能描述：设置灵敏度
//     
// 输入参数：IN const int& iSensitivityRate
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetSensitivityRate(IN const int& iSensitivityRate )
{
	CString strCommand;
	switch(iSensitivityRate)
	{
	case SM_FUN_SENSITIVITY_LOW:
		strCommand.Format(_T("NLS0312010;"));
		break;
	case SM_FUN_SENSITIVITY_MID:
		strCommand.Format(_T("NLS0312010;"));
		break;
	case SM_FUN_SENSITIVITY_HIGH:
		strCommand.Format(_T("NLS0312010;"));
		break;
	}
	return strCommand;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::SetSensitivityValue
//     
// 功能描述：设置灵敏度数值
//     
// 输入参数：IN const int& iSensitivityValue
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::SetSensitivityValue(IN const int& iSensitivityValue)
{
	return _T("");
}

/************************************************************************************************************数据筛选设置************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString	CSMNewlandFM420::AllowOnlyScanQRCode
//     
// 功能描述：只允许扫描QR码
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString	CSMNewlandFM420::AllowOnlyScanQRCode(void)
{
	CString strTemp;
	CString strCommand;
	strTemp.Format(_T("NLS0001010;"));//首先禁用所有扫码
	strCommand += strTemp;
	strTemp.Format(_T("NLS0502020;"));//放开QR码
	strCommand += strTemp;
	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMNewlandFM420::ResetQRCodeParam
//     
// 功能描述：重设QR码的参数为默认
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMNewlandFM420::ResetQRCodeParam(void)
{
	return _T("");
}