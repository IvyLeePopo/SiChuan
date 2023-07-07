/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitResult.h
 * 摘    要：扣款结果参数XML数据结构，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitResult :
	public CDVXMLBase
{
public:
	CDVXMLDebitResult(void);
	~CDVXMLDebitResult(void);
//Interface
public:
	//解析XML字符串
	const BOOL Init(IN const CString& strXMLString);
	//生成XML字符串
	const CString GetXMLString(void);
//Attributes
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
	DWORD	ConsumeTime;
};
