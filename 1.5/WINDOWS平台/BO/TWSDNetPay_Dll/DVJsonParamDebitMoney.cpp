/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonParamDebit.cpp
 * 摘    要：扣款接口参数JSON数据格式解析，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#include "StdAfx.h"
#include "DVJsonParamDebitMoney.h"

CDVJsonParamDebitMoney::CDVJsonParamDebitMoney(void)
	:OverTime(0)
	,OperationMode(0)

	,PayIdentifier(_T(""))
	,PayCertificateType(0)
	,PayCertificateCode(_T(""))
	,Money(0)
	,DebitMainType(0)

	,VehicleType(0)
	,VehicleClass(0)
	,VLColor(0)
	,VehicleLicense(_T(""))
	,AxisCount(0)
	,Weight(0)
	,LimitWeight(0)

	,PassCertificateType(0)
	,CardID(_T(""))
	,TollDate(0)
	,ShiftID(0)
	,TicketNo(_T(""))
	,OperatorID(_T(""))
	,OperatorName(_T(""))

	,EntryAreaID(_T("0"))
	,EntryRoadID(_T("0"))
	,EntryStationID(_T(""))
	,EntryStationName(_T(""))
	,EntryLane(_T(""))
	,EntryVehicleLicense(_T(""))
	,EntryVLColor(0)
	,EntryVehicleType(0)
	,EntryVehicleClass(0)
{
}

CDVJsonParamDebitMoney::~CDVJsonParamDebitMoney(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonParamDebitMoney::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonParamDebitMoney& cSrc
// 输出参数：CDVJsonParamDebitMoney&
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonParamDebitMoney& CDVJsonParamDebitMoney::operator=(IN const CDVJsonParamDebitMoney& cSrc)
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
		OverTime				=	cSrc.OverTime;
		OperationMode			=	cSrc.OperationMode;
	
		PayIdentifier			=	cSrc.PayIdentifier;
		PayCertificateCode		=	cSrc.PayCertificateCode;
		PayCertificateType		=	cSrc.PayCertificateType;
		Money					=	cSrc.Money;
		SubTime					=	cSrc.SubTime;
		DebitMainType			=	cSrc.DebitMainType;
		
		VehicleType				=	cSrc.VehicleType;
		VehicleClass			=	cSrc.VehicleClass;
		VLColor					=	cSrc.VLColor;
		VehicleLicense			=	cSrc.VehicleLicense;
		AxisCount				=	cSrc.AxisCount;
		Weight					=	cSrc.Weight;
		LimitWeight				=	cSrc.LimitWeight;

		PassCertificateType		=	cSrc.PassCertificateType;
		CardID					=	cSrc.CardID;
		TollDate				=	cSrc.TollDate;
		ShiftID					=	cSrc.ShiftID;
		TicketNo				=	cSrc.TicketNo;
		OperatorID				=	cSrc.OperatorID;
		OperatorName			=	cSrc.OperatorName;
	
		EntryAreaID				=	cSrc.EntryAreaID;
		EntryRoadID				=	cSrc.EntryRoadID;
		EntryStationID			=	cSrc.EntryStationID;
		EntryStationName		=	cSrc.EntryStationName;
		EntryTime				=	cSrc.EntryTime;
		EntryLane				=	cSrc.EntryLane;
		EntryVehicleLicense		=	cSrc.EntryVehicleLicense;
		EntryVLColor			=	cSrc.EntryVLColor;
		EntryVehicleType		=	cSrc.EntryVehicleType;
		EntryVehicleClass		=	cSrc.EntryVehicleClass;
	}
	return *this;
}