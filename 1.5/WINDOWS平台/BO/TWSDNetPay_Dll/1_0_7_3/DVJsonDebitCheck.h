/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonDebitCheck.h
 * 摘    要：扣款状态查询JSON结构信息描述，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年12月04日
 */
#pragma once
#include "DVJsonDebit.h"
class CDVJsonDebitCheck :
	public CDVJsonBase
{
public:
	CDVJsonDebitCheck(void);
	CDVJsonDebitCheck(IN const CDVJsonDebit* pDVJsonDebit);
	CDVJsonDebitCheck(IN const CDVJsonDebitCheck* pDVJsonDebitCheck);
	~CDVJsonDebitCheck(void);
//运算符重载
	CDVJsonDebitCheck& operator=(IN const CDVJsonDebitCheck& cSrc);
//Interface
public:
	//重置数据
	void  Reset(IN const int& iType=0);
	//生成数据校验码
	DWORD CreateInnerVerifyCode(void){return 0;}
	//内部数据码校验
	BOOL  InnerVerify(void){return TRUE;}
//属性
public:
	/***********************************************发起请求需要填充的数据********************************************************/

	//车道产生的交易凭证号
	CString RecordMatchCode;
	//扣款识别码
	CString PayIdentifier;

	//二维码绑定的支付平台ID
	CString IDBindInPayPlatform;
	//二维码绑定的支付平台类型
	int		IDBindInPayPlatformType;
	//扣款类型
	int		DebitType;
	//扣款金额
	int		DebitMoney;
	//扣款递交时间(与该条记录的产生时间一致)
	CTime	DebitTime;

	//扣款状态查询延迟时间(单位：毫秒)
	DWORD	CheckStateDelayTime;

	/***********************************************后台回馈需要填充的数据********************************************************/

	//扣款状态代码
	int		DebitStatusCode;
	//扣款状态描述
	CString DebitStatusDesc;
	//扣款订单号(仅仅当扣款成功才有该项数据)
	CString	DebitOrderNumber;
};
