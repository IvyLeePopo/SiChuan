/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：PayKeyItems.h
 * 摘    要：交易关键参数
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年4月27日
 */
#pragma once

class CPayKeyItems
{
public:
	CPayKeyItems(void);
	~CPayKeyItems(void);
//Attribute
public:
	//是否有效
	BOOL	Valid;
	//超时等待时间
	int		OverTime;
	//扣款模式
	int		OperationMode;

	/*****************Debit详细项************************/
	//交易识别码
	CString	PayIdentifier;
	//交易金额(单位：分)
	int		Money;
	//过车时间
	CTime	SubTime;
	//交易主分类
	int		DebitMainType;
	//第三方支付凭证
	CString PayCertificateCode;
	//支付凭证类型
	int		PayCertificateType;

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
	int		TotalWeight;
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
	//入口区域编码
	CString		EntryAreaID;
	//入口路段号
	CString		EntryRoadID;

	/*****************NFC详细项************************/
	//NFC消费报文
	CString NFCPayData;
	//NFC冲正报文
	CString NFCRevData;

	//add 2018-04
	CString imageId;//车牌识别系统生成的图像ID
	CString taskId;

};
