/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonAccount.cpp
 * 摘    要：获取用户信息JSON数据格式解析，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#include "StdAfx.h"
#include "DVJsonParamAccount.h"

CDVJsonParamAccount::CDVJsonParamAccount(void)
	:DataType(0)
	,DataSrc(0)
	,DataContext(_T(""))
{
	Version.Format(_T("1.0"));
}

CDVJsonParamAccount::~CDVJsonParamAccount(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonParamAccount::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonParamAccount& cSrc
// 输出参数：CDVJsonParamAccount&
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonParamAccount& CDVJsonParamAccount::operator=(IN const CDVJsonParamAccount& cSrc)
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
		DataType		=	cSrc.DataType;
		DataSrc			=	cSrc.DataSrc;
		DataContext		=	cSrc.DataContext;
	}
	return *this;
}