/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：SMHoneywell3301g.cpp
 * 摘    要：霍尔韦尔3310g扫码器协议描述类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
#include "StdAfx.h"
#include "SMHoneywell3301g.h"

CSMHoneywell3301g::CSMHoneywell3301g(IN const int& iSMType)
	:CScanMachineIF(iSMType)
	,m_strCommandHead(_T("164D0D"))
{
	m_dwCodeLen = 18;
}

CSMHoneywell3301g::~CSMHoneywell3301g(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::AllowFunctionCode
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
const CString CSMHoneywell3301g::AllowFunctionCode(IN const BOOL& bAllow)
{
	CString strCommand;
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::ResetDefaultParam
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
const CString CSMHoneywell3301g::ResetDefaultParam(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("DEFOVR."))
		,m_strCommandHead,FormatCommandData(_T("DEFALT.")));

	//设置命令类型
	m_iLastCommandType = SM_CMD_RESET_FACTORY;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::ResetPrivateParam
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
const CString CSMHoneywell3301g::ResetPrivateParam(IN OUT BYTE& byDataConvertType)
{
	CString strTemp;
	CString strCommand;
	
	//设置扫码方式为命令触发
	strTemp		= SetScanMode();
	strCommand += strTemp;
	
	//设置解码成功后是否发出声音
	strTemp		= SetAllowDecodeVoice(TRUE);
	strCommand += strTemp;
	
	//设置声音
	strTemp		= SetDecodeVoiceType();
	strCommand += strTemp;
	
	//设置读码光照
	strTemp		= SetSupplementalLightMode(SM_FUN_SUPPORT_LIGHT_FORBID);
	strCommand += strTemp;
	
	//设置读码对焦灯光
	strTemp		= SetFocusingMode(SM_FUN_FOCUS_FLICKER);
	strCommand += strTemp;
	
	//设置读码延时时间
	strTemp		= SetDelayTime();
	strCommand += strTemp;
	
	//设置扫描手机屏幕模式
	strTemp		= SetScanPhoneReadMode();
	strCommand += strTemp;
	
	//设置只允许扫描QR码
	strTemp		= AllowOnlyScanQRCode();
	strCommand += strTemp;
	
	//设置上述更改为用户默认设置
	strTemp		= SubmitPrivateParam();
	strCommand += strTemp;

	//设置转换数据格式为需要
	byDataConvertType = 1;

	//设置命令类型
	m_iLastCommandType = SM_CMD_INIT;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::Start
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
const CString CSMHoneywell3301g::Start()
{
	CString strCommand;
	strCommand.Format(_T("%02X%02X%02X%02X"),0x16,'T',0x0D,0x2E);
	//设置命令类型
	m_iLastCommandType = SM_CMD_START;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::Stop
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
const CString CSMHoneywell3301g::Stop(void)
{
	CString strCommand;
	strCommand.Format(_T("%02X%02X%02X%02X"),0x16,'U',0x0D,0x2E);

	//设置命令类型
	m_iLastCommandType = SM_CMD_STOP;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::GetHeartCmd
//     
// 功能描述：发送心跳指令(实际就是一个简单的查询指令)
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年1月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMHoneywell3301g::GetHeartCmd(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("cbrena.")));
	//设置命令类型
	m_iLastCommandType = SM_CMD_HEART;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::MakeVoice
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
const CString CSMHoneywell3301g::MakeVoice(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("MNUCDS.")));
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::FormatReadData
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
CString CSMHoneywell3301g::FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	CString strFormatData(_T("FAILED"));
	BOOL bFindSuccessFlag = FALSE;
	for(DWORD i = 0;i<dwSize;++i)
	{
		if(0x06 == byData[i])
		{
			bFindSuccessFlag = TRUE;
			break;
		}
	}

	//if(bFindSuccessFlag)
	//{
	//	for(DWORD i=0;i<dwSize-1;++i)
	//	{
	//		byData[i] = byData[i+1];
	//	}
	//	--dwSize;
	//}

	strFormatData.Format(_T("%s"),(LPTSTR)byData);
	strFormatData.TrimLeft();
	strFormatData.TrimRight();

	return strFormatData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::BeginSetPrivateParam
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
const CString CSMHoneywell3301g::BeginSetPrivateParam(void)
{
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SubmitPrivateParam
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
const CString CSMHoneywell3301g::SubmitPrivateParam(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("MNUCDS.")));
	//设置命令类型
	m_iLastCommandType = SM_CMD_RESET_PERSION;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::GetSysInfo
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
const CString CSMHoneywell3301g::GetSysInfo(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("REVINF.")));
	//设置命令类型
	m_iLastCommandType = SM_CMD_VER;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetRS232CommMode
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
const CString CSMHoneywell3301g::SetRS232CommMode(IN const int& iRate/* = 9600*/,IN const int& iParityBit/* = 0*/,IN const int& iStopBit/* = 1*/,IN const int& iDataBit/* = 8*/)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("232WRD2."))
		,m_strCommandHead,FormatCommandData(_T("232BAD5.")));
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::CheckDeviceStatus
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
const int  CSMHoneywell3301g::CheckDeviceStatus(IN const CString& strState)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::CheckCommandExecuteResult
//     
// 功能描述：检查命令执行结果是否正确
//     
// 输入参数：IN OUT LPBYTE byData
//         ：IN OUT DWORD& dwSize
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年1月30日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSMHoneywell3301g::CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	BOOL bReturn = FALSE;

	int iSuccessFlagCount = 0;
	for(DWORD i = 0;i<dwSize;++i)
	{
		if(0x06 == *byData++)
		{
			++iSuccessFlagCount;
		}
	}

	switch(m_iLastCommandType)
	{
	case SM_CMD_INIT://初始化
		{
			if(iSuccessFlagCount>3)
			{
				bReturn = TRUE;
			}
		}
		break;
	case SM_CMD_VER://读版本
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_START://扫码触发
		{
			if(dwSize>=18)
			{
				bReturn = TRUE;
			}
		}
		break;
	case SM_CMD_STOP://停止扫码
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_HEART://心跳
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_RESET_FACTORY://重置出厂
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_RESET_PERSION://重置用户默认
		{
			bReturn = TRUE;
		}
		break;
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：BOOL	CSMHoneywell3301g::AnalysisVersionString
//     
// 功能描述：解析版本信息字符串
//     
// 输入参数：IN const CString& strVersionString
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年1月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL	CSMHoneywell3301g::AnalysisVersionString(IN const CString& strVersionString)
{
	BOOL bReturn = FALSE;
	CString strRealString(strVersionString);
	if(strRealString.GetLength()<128)
		return bReturn;

	int iBegin	= strRealString.Find(_T("Product Name:")) + (int)_tcslen(_T("Product Name:"));
	int iCount	= strRealString.Find(_T("Boot Revision:")) - strRealString.Find(_T("Product Name:")) - (int)_tcslen(_T("Boot Revision:"));
	m_strProductName		= strRealString.Mid(iBegin,iCount);

	iBegin	= strRealString.Find(_T("Boot Revision: :")) + (int)_tcslen(_T("Boot Revision: :"));
	iCount	= 6;
	m_strBootRevision		= strRealString.Mid(iBegin,iCount);
	
	iBegin	= strRealString.Find(_T("Serial Number:")) + (int)_tcslen(_T("Serial Number:"));
	iCount	= strRealString.Find(_T("Supported IF:")) - strRealString.Find(_T("Serial Number:")) - (int)_tcslen(_T("Supported IF:")) - 1;
	m_strSerialNumber		= strRealString.Mid(iBegin,iCount);
	
	iBegin	= strRealString.Find(_T("Engine Firmware")) + (int)_tcslen(_T("Engine Firmware"));
	iCount	= strRealString.GetLength() - iBegin;
	m_strFirmwareVersion	= strRealString.Mid(iBegin,iCount);

	bReturn = TRUE;
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString	CSMHoneywell3301g::FormatCommandData
//     
// 功能描述：将命令数据（可视字符串）转化成相应ASCII码的16进制格式
//     
// 输入参数：IN const CString& strCmd
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年12月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMHoneywell3301g::FormatCommandData(IN const CString& strCmd)
{
	CString strTempCmd(strCmd);
	BYTE	byData[512]={0};
	memcpy(&byData[0],strTempCmd.GetBuffer(0),strTempCmd.GetLength());strTempCmd.ReleaseBuffer();
	CString strASCIICmdData = ::Bin2Hex(byData,strTempCmd.GetLength());
	return strASCIICmdData;
}

/************************************************************************************************************硬件设置************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetScanPhoneReadMode
//     
// 功能描述：设置扫描手机模式
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年12月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CSMHoneywell3301g::SetScanPhoneReadMode(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("PAPHHC.")));
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetSupplementalLightMode
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
const CString CSMHoneywell3301g::SetSupplementalLightMode(IN const int& iLightMode)
{
	CString strCommand;
	switch(iLightMode)
	{
	case SM_FUN_SUPPORT_LIGHT_ALLOW:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNLED1.")));
		break;
	case SM_FUN_SUPPORT_LIGHT_FORBID:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNLED0.")));
		break;
	}
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetFocusingMode
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
const CString CSMHoneywell3301g::SetFocusingMode(IN const int& iMode)
{
	CString strCommand;
	switch(iMode)
	{
	case SM_FUN_FOCUS_LIGHT:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNAIM1.")));
		break;
	case SM_FUN_FOCUS_FLICKER:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNAIM2.")));
		break;
	case SM_FUN_FOCUS_NONE:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNAIM0.")));
		break;
	}
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetAllowDecodeVoice
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
const CString CSMHoneywell3301g::SetAllowDecodeVoice(IN const BOOL& bAllowVoice)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,
		bAllowVoice ? FormatCommandData(_T("BEPBEP1.")):FormatCommandData(_T("BEPBEP0.")));	//有声
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetDecodeVoiceType
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
const CString CSMHoneywell3301g::SetDecodeVoiceType(IN const int& iVoiceType)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("BEPLVL3."))			//声音类型
		,m_strCommandHead,FormatCommandData(_T("BEPFQ14200.")));	//音量为高
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

/************************************************************************************************************扫码模式设置************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetDelayTime
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
const CString CSMHoneywell3301g::SetDelayTime(IN const int& iDealyTime)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(FmtStr(_T("232LPT%03d."),iDealyTime)));
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetDelayTimeMode
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
const CString CSMHoneywell3301g::SetDelayTimeMode(IN const int& iDelayMode)
{
	CString strCommand;

	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetScanMode
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
const CString CSMHoneywell3301g::SetScanMode(IN const int& iScanMode)
{
	CString strCommand;
	switch(iScanMode)
	{
	case SM_FUN_SCAN_CMD:	//命令触发
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("HSTCEN1.")));
		break;
	case SM_FUN_SCAN_AUTO:	//自动扫描
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("TRGST3.")));
		break;
	case SM_FUN_SCAN_MANUAL://手动
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("PAPHHF.")));
		break;
	}
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetSensitivityRate
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
const CString CSMHoneywell3301g::SetSensitivityRate(IN const int& iSensitivityRate )
{
	CString strCommand;
	switch(iSensitivityRate)
	{
	case SM_FUN_SENSITIVITY_LOW:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(FmtStr(_T("TRGPMS%X."),5)));
		break;
	case SM_FUN_SENSITIVITY_MID:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(FmtStr(_T("TRGPMS%X."),15)));
		break;
	case SM_FUN_SENSITIVITY_HIGH:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(FmtStr(_T("TRGPMS%X."),20)));
		break;
	}

	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::SetSensitivityValue
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
const CString CSMHoneywell3301g::SetSensitivityValue(IN const int& iSensitivityValue)
{
	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return _T("");
}

/************************************************************************************************************数据筛选设置************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString	CSMHoneywell3301g::AllowOnlyScanQRCode
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
const CString CSMHoneywell3301g::AllowOnlyScanQRCode(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("ALLENA0."))		//禁止所有码
		,m_strCommandHead,FormatCommandData(_T("QRCENA1.")));	//允许读QR码

	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CSMHoneywell3301g::ResetQRCodeParam
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
const CString CSMHoneywell3301g::ResetQRCodeParam(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("QRCDFT.")));//重设QR码参数

	//设置命令类型
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}
