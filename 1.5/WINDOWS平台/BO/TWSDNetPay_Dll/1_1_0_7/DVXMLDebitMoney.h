/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitParamContext.h
 * 摘    要：扣款明细数据XML结构解析，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitParamContext :
	public CDVXMLBase
{
public:
	CDVXMLDebitParamContext(void);
	~CDVXMLDebitParamContext(void);
//Interface
public:
	//解析XML字符串
	const BOOL Init(IN const CString& strXMLString);
//Attributes
public:
	//超时等待时间
	int		OverTime;
	//扣款模式
	int		OperationMode;
	/*****************Debit详细项************************/
	//交易识别码
	CString	PayIdentifier;
	//第三方支付凭证内容
	CString PayCertificateCode;
	//第三方支付凭证类别
	int		PayCertificateType;
	//交易金额(单位：分)
	int		Money;
	//过车时间
	CTime	SubTime;
	//交易主分类
	int		DebitMainType;

	/*****************Vehicle详细项************************/
	//车种
	int		VehicleType;
	//车型
	int		VehicleClass;
	//车牌颜色
	int		VLColor;
	//车牌
	CString	VehicleLicense;
	//轴数
	int		AxisCount;
	//重量
	int		Weight;
	//限重
	int		LimitWeight;
	//add 2018-03
	CString AutoLicense;
	int AutoColor;

	/*****************Operation详细项************************/
	//通行凭证类型
	int		PassCertificateType;
	//通行凭证编码
	CString CardID;
	//统计日
	int		TollDate;
	//统计班次
	int		ShiftID;
	//打印票据号
	CString TicketNo;
	//收费员工号
	CString OperatorID;
	//收费员姓名
	CString OperatorName;
	//add 2018-03
	int Distance;

	/*****************Entry详细项************************/
	//入口区域编码
	CString		EntryAreaID;
	//入口路段编码
	CString		EntryRoadID;
	//入口站点
	CString	EntryStationID;
	//入口站名
	CString EntryStationName;
	//入口时间
	CTime	EntryTime;
	//入口车道
	CString	EntryLane;
	//入口车牌
	CString	EntryVehicleLicense;
	//入口车牌颜色
	int		EntryVLColor;
	//入口车种
	int		EntryVehicleType;
	//入口车型
	int		EntryVehicleClass;
};
