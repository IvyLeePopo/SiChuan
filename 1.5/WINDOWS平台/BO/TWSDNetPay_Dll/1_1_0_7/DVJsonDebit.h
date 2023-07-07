/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonDebit.h
 * 摘    要：需要扣费发起的请求JSON数据结构体，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#pragma once
#include "DVJsonBase.h"

class CDVJsonDebit : public CDVJsonBase
{
public:
	CDVJsonDebit(void);
	~CDVJsonDebit(void);
//运算符重载
	CDVJsonDebit& operator=(IN const CDVJsonDebit& cSrc);
//Interface
public:
	//重置数据
	void  Reset(IN const int& iType=0);
	//生成数据校验码
	CString CreateInnerVerifyCode(void){return _T("");}
	//生成全字段数据加密结果
	const CString CreateEnDataString(void) const {return _T("");}
	//内部数据码校验
	BOOL  InnerVerify(void){return TRUE;}
//属性
public:

	/***********************************************发起请求需要填充的数据********************************************************/
	
	/*********************基本信息*********************/

	//是否测试数据
	BOOL	TestingData;
	CString	NetPayDllVersion;
	CString DeviceDllVersion;
	CString TWProductSerialNo;//特微硬件终端序号
	
	/*********************加密认证*********************/

	//校验码(AES算法后进行摘要)
	CString VerifyCode;
	//交易全字段加密后数据
	CString EnDataString;
	//硬件终端设备编码（唯一号）
	CString DeviceSerialNo;
	
	//二维码绑定的支付平台ID
	CString IDBindInPayPlatform;
	//二维码绑定的支付平台类型
	int		IDBindInPayPlatformType;
	//交易唯一凭证(车道产生)
	CString RecordMatchCode;
	//交易记录安全认证码
	CString TradeSecurityCode;
	//扣款QR码
	CString	QRCodeData;
	//扣款类型(1,封闭路段扣费；4,开放路段扣费; 2，入口收费；3,补票)
	int		DebitType;
	//扣款金额
	int		DebitMoney;
	//扣款递交时间(与该条记录的产生时间一致)
	CTime	DebitTime;
	//虚拟卡号（通行凭证）
	CString SMVirtualCardID;
	//支付卡号
	CString PayVirtualCardID;
	//统计日
	int		DebitTollDate;
	//统计班次
	int		DebitTollShift;
	//用户数据设备来源(1,扫码器)
	int		AccountInfoSrcType;
	//用户数据具体设备号
	int		AccountInfoDeviceID;
	//用户数据具体类型
	int		AccountInfoDataType;
	
	/*********************附加信息*********************/
	//入口省份
	int		EntryProvinceID;
	//入口站点
	CString	EntryStationID;
	//入口站名
	CString	EntryStationName;
	//入口车道号
	CString	EntryLane;
	//入站时间
	CTime	EntryTime;
	//入口信息唯一标识
	CString EntryInfoUniqueFlag;
	//入口所属路段
	CString		EntryRoadID;
	//入口所属区域编码
	CString		EntryAreaID;

	//扣费地点所属省份
	int		PayProvinceID;
	//扣费站点
	CString	PayStationID;
	//扣费站名
	CString PayStationName;
	//扣费车道
	CString	PayLane;
	//付费站点所属路段号
	CString		PayRoadID;
	//扣费站点操作员工号
	CString PayOperatorID;
	//扣费站点操作员姓名
	CString PayOperatorName;

	//扣费相关车辆车型
	int		VehicleClass;
	//扣费相关车辆种类
	int		VehicleType;
	//扣费相关车辆牌照
	CString	VehicleLicense;
	//扣费相关车辆车牌颜色
	int		VehicleColor;
	//扣费车辆轴数
	int		AxisCount;
	//扣费车辆载重
	int		TotalWeight;
	//扣费车辆限重
	int		LimitWeight;

	/*****************NFC详细项************************/
	//NFC模块终端号
	CString NFCTerminal;
	//NFC模块硬件序列号
	CString NFCSerialNo;
	//NFC消费报文
	CString NFCPayData;
	//NFC冲正报文
	CString NFCRevData;

	/***********************************************后台回馈需要填充的数据********************************************************/

	//服务器实际扣款金额
	int		ServerRealDebitMoney;
	//服务器判定支付平台类型(1,微信；2,支付宝）
	int		ServerJudgeDebitIDType;
	//扣款状态代码
	int		DebitStatusCode;
	//扣款状态描述
	CString DebitStatusDesc;
	//扣款订单号(仅仅当扣款成功才有该项数据)
	CString	DebitOrderNumber;

	//add 2018-03
	CString AutoLicense;
	int AutoColor;
	int PassCertificateType;
	CString TicketNo;
	int Distance;
	CString EntryLicense;
	int EntryVColor;
	int EntryVClass;
	int EntryVType;

	CString imageId;

	


};
