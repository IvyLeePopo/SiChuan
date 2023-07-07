/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonParamDebitResult.cpp
 * 摘    要：获取扣款接口参数JSON数据格式解析，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#include "StdAfx.h"
#include "DVJsonParamDebitResult.h"

CDVJsonParamDebitResult::CDVJsonParamDebitResult(void)
	:ResultType(0)
	,ExecuteCode(-1)
	,ExecuteDesc(_T(""))

	,PayIdentifier(_T(""))
	,PayPlatformType(0)
	,TradeSecurityCode(_T(""))
	,DebitOrder(_T(""))
{
	Version.Format(_T("1.0"));
}

CDVJsonParamDebitResult::~CDVJsonParamDebitResult(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonParamDebitResult::operator=
//     
// 功能描述：
//     
// 输入参数：IN const CDVJsonParamDebitResult& cSrc
// 输出参数：CDVJsonParamDebitResult&
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonParamDebitResult& CDVJsonParamDebitResult::operator=(IN const CDVJsonParamDebitResult& cSrc)
{
	if(this != &cSrc)
	{
		//基类数据复制
		m_hCallerWnd			=	cSrc.m_hCallerWnd;
		m_dwThreadID			=	cSrc.m_dwThreadID;

		Version					=	cSrc.Version;
		PackageType				=	cSrc.PackageType;
		ProvinceID				=	cSrc.ProvinceID;
		Token					=	cSrc.Token;
		SubmitCount				=	cSrc.SubmitCount;
		SubmitTime				=	cSrc.SubmitTime;
		QRCodeData				=	cSrc.QRCodeData;
		QRCodeType				=	cSrc.QRCodeType;
		m_hCallerWnd			=	cSrc.m_hCallerWnd;
		m_dwThreadID			=	cSrc.m_dwThreadID;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//本地类数据复制
		ResultType				=	cSrc.ResultType;
		ExecuteCode				=	cSrc.ExecuteCode;
		ExecuteDesc				=	cSrc.ExecuteDesc;
	
		PayIdentifier			=	cSrc.PayIdentifier;
		PayPlatformType			=	cSrc.PayPlatformType;
		TradeSecurityCode		=	cSrc.TradeSecurityCode;
		DebitOrder				=	cSrc.DebitOrder;
	}
	return *this;
}