/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonBase.cpp
 * 摘    要：Json数据载体的基本类，其它类继承于它，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#include "StdAfx.h"
#include "DVJsonBase.h"

CDVJsonBase::CDVJsonBase(void)
	:m_dwConsumeTime(0)
	,m_hCallerWnd(NULL)
	,m_dwThreadID(0)

	,Version(_T(""))
	,PackageType(0)
	,ProvinceID(0)
	,Token(_T(""))
	,SubmitCount(0)
	,SubmitTime(_T(""))

	,QRCodeData(_T(""))
	,QRCodeType(0)
	,PayIdentifier(_T(""))

	,ServerResponseCode(0)
	,ServerReceiveDataTime(_T(""))
	,ServerResponseDataTime(_T(""))
{
}

CDVJsonBase::~CDVJsonBase(void)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonBase::Reset
//     
// 功能描述：重置数据
//     
// 输入参数：IN const int& iType -- 重置类型
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年1月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CDVJsonBase::Reset(IN const int& iType)
{
}