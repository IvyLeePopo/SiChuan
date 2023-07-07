/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonParamDebitResult.h
 * 摘    要：获取扣款接口参数JSON数据格式解析，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonParamDebitResult :
	public CDVJsonBase
{
public:
	CDVJsonParamDebitResult(void);
	~CDVJsonParamDebitResult(void);
//运算符重载
	CDVJsonParamDebitResult& operator=(IN const CDVJsonParamDebitResult& cSrc);
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
	//结果类型
	int		ResultType;
	//执行状态
	int		ExecuteCode;
	//执行结果描述
	CString ExecuteDesc;


	//交易识别码
	CString PayIdentifier;
	//第三方支付平台类型
	int		PayPlatformType;
	//交易安全码
	CString TradeSecurityCode;
	//订单号
	CString DebitOrder;
	//服务器扣款时间
	CTime	DebitTime;

	//消耗时间，单位：毫秒
	int		ConsumeTime;
};
