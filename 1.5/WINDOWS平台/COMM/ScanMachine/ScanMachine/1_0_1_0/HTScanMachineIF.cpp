/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ScanMachineIF.cpp
 * 摘    要：扫码器功能定义基本类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
#include "StdAfx.h"
#include "HTSMDeviceBaseIF.h"

CHTSMDeviceBaseIF::CHTSMDeviceBaseIF()
{
}

CHTSMDeviceBaseIF::CHTSMDeviceBaseIF(IN const int& iSMType)
	:m_iSMType(0)
	,m_dwCodeLen(0)
	,m_iLastCommandType(SM_CMD_UNKNOW)
	,m_strProductName(_T(""))
	,m_strBootRevision(_T(""))
	,m_strSerialNumber(_T(""))
	,m_strFirmwareVersion(_T(""))
{
}

CHTSMDeviceBaseIF::~CHTSMDeviceBaseIF(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceBaseIF::IsNeedReadVersionInfo
//     
// 功能描述：
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
const BOOL CHTSMDeviceBaseIF::IsNeedReadVersionInfo(void)
{
	if(m_strProductName.IsEmpty() || m_strBootRevision.IsEmpty() || m_strSerialNumber.IsEmpty() || m_strFirmwareVersion.IsEmpty())
	{
		return TRUE;
	}
	return FALSE;
}