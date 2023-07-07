/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonHeart.cpp
 * 摘    要：心跳接口数据交互描述结构体，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年12月14日
 */
#include "StdAfx.h"
#include "DVJsonHeart.h"

CDVJsonHeart::CDVJsonHeart(void)
	:StationID(_T(""))
	,StationName(_T(""))
	,LaneID(_T(""))
{
}

CDVJsonHeart::~CDVJsonHeart(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonHeart::Reset
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
void CDVJsonHeart::Reset(IN const int& iType)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonHeart::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonHeart& cSrc
// 输出参数：CDVJsonHeart&
// 编写人员：ROCY
// 编写时间：2015年12月14日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonHeart& CDVJsonHeart::operator=(IN const CDVJsonHeart& cSrc)
{
	if(&cSrc != this)
	{
		//基类数据复制
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
		StationID	= cSrc.StationID;
		StationName	= cSrc.StationName;
		LaneID		= cSrc.LaneID;
		HeartTime	= cSrc.HeartTime;
	}
	return *this;
}