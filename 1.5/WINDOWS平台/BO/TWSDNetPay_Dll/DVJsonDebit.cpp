/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonDebit.cpp
 * 摘    要：需要扣费发起的请求JSON数据结构体，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#include "StdAfx.h"
#include "DVJsonDebit.h"

CDVJsonDebit::CDVJsonDebit(void)
	:TestingData(FALSE)
	,NetPayDllVersion(_T(""))
	,DeviceDllVersion(_T(""))
	,TWProductSerialNo(_T(""))

	,VerifyCode(_T(""))
	,EnDataString(_T(""))
	
	,IDBindInPayPlatform(_T(""))
	,IDBindInPayPlatformType(0)
	,RecordMatchCode(_T(""))
	,TradeSecurityCode(_T(""))
	,QRCodeData(_T(""))
	,DebitType(0)
	,DebitMoney(0)
	,SMVirtualCardID(_T(""))
	,PayVirtualCardID(_T(""))
	,DebitTollDate(0)
	,DebitTollShift(0)
	,AccountInfoSrcType(0)
	,AccountInfoDeviceID(0)
	,AccountInfoDataType(0)

	,EntryProvinceID(0)
	,EntryStationID(_T(""))
	,EntryStationName(_T(""))
	,EntryLane(_T(""))
	,EntryInfoUniqueFlag(_T(""))
	,EntryRoadID(_T("0"))
	,EntryAreaID(_T("0"))

	,PayProvinceID(0)
	,PayStationID(_T(""))
	,PayStationName(_T(""))
	,PayLane(_T(""))
	,PayRoadID(_T("0"))
	,PayOperatorID(_T(""))
	,PayOperatorName(_T(""))

	,VehicleClass(0)
	,VehicleType(0)
	,VehicleLicense(_T(""))
	,VehicleColor(0)
	,AxisCount(0)
	,TotalWeight(0)
	,LimitWeight(0)

	,NFCTerminal(_T(""))
	,NFCSerialNo(_T(""))
	,NFCPayData(_T(""))
	,NFCRevData(_T(""))
	
	,ServerRealDebitMoney(0)
	,ServerJudgeDebitIDType(0)
	,DebitStatusCode(0)
	,DebitStatusDesc(_T(""))
	,DebitOrderNumber(_T(""))
{
}

CDVJsonDebit::~CDVJsonDebit(void) 
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebit::Reset
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
void CDVJsonDebit::Reset(IN const int& iType)
{
	switch(iType)
	{
	case 0:
		{
			TestingData				= FALSE;
			NetPayDllVersion		= _T("");
			DeviceDllVersion		= _T("");
			TWProductSerialNo		= _T("");

			VerifyCode				= _T("");
			EnDataString			= _T("");
			
			IDBindInPayPlatform		= _T("");
			IDBindInPayPlatformType = 0;
			RecordMatchCode		= _T("");
			TradeSecurityCode	= _T("");
			QRCodeData			= _T("");
			DebitType			= 0;
			DebitMoney			= 0;
			SMVirtualCardID		= _T("");
			PayVirtualCardID	= _T("");
			PayIdentifier		= _T("");
			DebitTime			= CTime(0);
			DebitTollDate		= 0;
			DebitTollShift		= 0;
			AccountInfoSrcType	= 0;
			AccountInfoDeviceID	= 0;
			AccountInfoDataType	= 0;

			EntryProvinceID		= 0;
			EntryStationID		= _T("");
			EntryStationName	= _T("");
			EntryLane			= _T("");
			EntryInfoUniqueFlag	= _T("");
			EntryRoadID			= _T("0");
			EntryAreaID			= _T("0");

			PayProvinceID		= 0;
			PayStationID		= _T("");
			PayStationName		= _T("");
			PayLane				= _T("");
			PayRoadID			= _T("0");
			PayOperatorID		= _T("");
			PayOperatorName		= _T("");

			VehicleClass		= 0;
			VehicleType			= 0;
			VehicleLicense		= _T("");
			VehicleColor		= 0;
			AxisCount			= 0;
			TotalWeight			= 0;
			LimitWeight			= 0;

			NFCTerminal			= _T("");
			NFCSerialNo			= _T("");
			NFCPayData			= _T("");
			NFCRevData			= _T("");

			DebitStatusCode		= 0;
			DebitStatusDesc		= _T("");
			DebitOrderNumber	= _T("");
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebit::operator=
//     
// 功能描述：重载运算符，使之俩个对象可以互相赋值
//     
// 输入参数：IN const CDVJsonDebit& cSrc	-- 源数据对象
// 输出参数：CDVJsonDebit&
// 编写人员：ROCY
// 编写时间：2015年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebit& CDVJsonDebit::operator=(IN const CDVJsonDebit& cSrc)
{
	if(this != &cSrc)
	{
		TestingData				=	cSrc.TestingData;
		NetPayDllVersion		=	cSrc.NetPayDllVersion;
		DeviceDllVersion		=	cSrc.DeviceDllVersion;
		TWProductSerialNo		=	cSrc.TWProductSerialNo;
		VerifyCode				=	cSrc.VerifyCode;
		EnDataString			=	cSrc.EnDataString;
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
		ServerDebitTime			=	cSrc.ServerDebitTime;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//本地类数据复制
		IDBindInPayPlatform		=	cSrc.IDBindInPayPlatform;
		IDBindInPayPlatformType	=	cSrc.IDBindInPayPlatformType;
		RecordMatchCode			=	cSrc.RecordMatchCode;
		QRCodeData				=	cSrc.QRCodeData;
		DebitType				=	cSrc.DebitType;
		DebitMoney				=	cSrc.DebitMoney;
		SMVirtualCardID			=	cSrc.SMVirtualCardID;
		PayVirtualCardID		=	cSrc.PayVirtualCardID;
		PayIdentifier			=	cSrc.PayIdentifier;
		DebitTime				=	cSrc.DebitTime;
		DebitTollDate			=	cSrc.DebitTollDate;
		DebitTollShift			=	cSrc.DebitTollShift;
		AccountInfoSrcType		=	cSrc.AccountInfoSrcType;
		AccountInfoDeviceID		=	cSrc.AccountInfoDeviceID;
		AccountInfoDataType		=	cSrc.AccountInfoDataType;

		EntryProvinceID			=	cSrc.EntryProvinceID;
		EntryStationID			=	cSrc.EntryStationID;
		EntryStationName		=	cSrc.EntryStationName;
		EntryLane				=	cSrc.EntryLane;
		EntryInfoUniqueFlag		=	cSrc.EntryInfoUniqueFlag;
		EntryRoadID				=	cSrc.EntryRoadID;
		EntryAreaID				=	cSrc.EntryAreaID;

		PayProvinceID			=	cSrc.PayProvinceID;
		PayStationID			=	cSrc.PayStationID;
		PayStationName			=	cSrc.PayStationName;
		PayLane					=	cSrc.PayLane;
		PayRoadID				=	cSrc.PayRoadID;
		PayOperatorID			=	cSrc.PayOperatorID;
		PayOperatorName			=	cSrc.PayOperatorName;

		VehicleClass			=	cSrc.VehicleClass;
		VehicleType				=	cSrc.VehicleType;
		VehicleLicense			=	cSrc.VehicleLicense;
		VehicleColor			=	cSrc.VehicleColor;
		AxisCount				=	cSrc.AxisCount;
		TotalWeight			    =	cSrc.TotalWeight;
		LimitWeight			    =   cSrc.LimitWeight;

		NFCTerminal				=	cSrc.NFCTerminal;
		NFCSerialNo				=	cSrc.NFCSerialNo;
		NFCPayData				=	cSrc.NFCPayData;
		NFCRevData				=	cSrc.NFCRevData;

		ServerRealDebitMoney	=	cSrc.ServerRealDebitMoney;
		ServerJudgeDebitIDType	=	cSrc.ServerJudgeDebitIDType;

		DebitStatusCode			=	cSrc.DebitStatusCode;
		DebitStatusDesc			=	cSrc.DebitStatusDesc;
		DebitOrderNumber		=	cSrc.DebitOrderNumber;
	}
	return *this;
}