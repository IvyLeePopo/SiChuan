/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonDebit.cpp
 * ժ    Ҫ����Ҫ�۷ѷ��������JSON���ݽṹ�壬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��12��
 */
#include "StdAfx.h"
#include "DVJsonDebit.h"

CDVJsonDebit::CDVJsonDebit(void)
	:TestingData(FALSE)
	,NetPayDllVersion(_T(""))
	,DeviceDllVersion(_T(""))
	
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
	,EntryStationID(0)
	,EntryStationName(_T(""))
	,EntryLane(0)
	,EntryInfoUniqueFlag(_T(""))
	,EntryRoadID(0)
	,EntryAreaID(0)

	,PayProvinceID(0)
	,PayStationID(0)
	,PayStationName(_T(""))
	,PayLane(0)
	,PayRoadID(0)

	,VehicleClass(0)
	,VehicleType(0)
	,VehicleLicense(_T(""))
	,VehicleColor(0)
	,AxisCount(0)
	,TotalWeight(0)
	,LimitWeight(0)
	
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
// �������ƣ�CDVJsonDebit::Reset
//     
// ������������������
//     
// ���������IN const int& iType -- ��������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
			EntryStationID		= 0;
			EntryStationName	= _T("");
			EntryLane			= 0;
			EntryInfoUniqueFlag	= _T("");
			EntryRoadID			= 0;
			EntryAreaID			= 0;

			PayProvinceID		= 0;
			PayStationID		= 0;
			PayStationName		= _T("");
			PayLane				= 0;
			PayRoadID			= 0;

			VehicleClass		= 0;
			VehicleType			= 0;
			VehicleLicense		= _T("");
			VehicleColor		= 0;
			AxisCount			= 0;
			TotalWeight			= 0;
			LimitWeight			= 0;

			DebitStatusCode		= 0;
			DebitStatusDesc		= _T("");
			DebitOrderNumber	= _T("");
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebit::operator=
//     
// ���������������������ʹ֮����������Ի��ำֵ
//     
// ���������IN const CDVJsonDebit& cSrc	-- Դ���ݶ���
// ���������CDVJsonDebit&
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��12��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebit& CDVJsonDebit::operator=(IN const CDVJsonDebit& cSrc)
{
	if(this != &cSrc)
	{
		TestingData				=	cSrc.TestingData;
		NetPayDllVersion		=	cSrc.NetPayDllVersion;
		DeviceDllVersion		=	cSrc.DeviceDllVersion;
		//�������ݸ���
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
		//���������ݸ���
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

		VehicleClass			=	cSrc.VehicleClass;
		VehicleType				=	cSrc.VehicleType;
		VehicleLicense			=	cSrc.VehicleLicense;
		VehicleColor			=	cSrc.VehicleColor;
		AxisCount				=	cSrc.AxisCount;
		TotalWeight			    =	cSrc.TotalWeight;
		LimitWeight			    =   cSrc.LimitWeight;

		ServerRealDebitMoney	=	cSrc.ServerRealDebitMoney;
		ServerJudgeDebitIDType	=	cSrc.ServerJudgeDebitIDType;

		DebitStatusCode			=	cSrc.DebitStatusCode;
		DebitStatusDesc			=	cSrc.DebitStatusDesc;
		DebitOrderNumber		=	cSrc.DebitOrderNumber;
	}
	return *this;
}