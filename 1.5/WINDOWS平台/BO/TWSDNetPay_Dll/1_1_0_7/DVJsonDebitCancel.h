/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonDebitCancel.h
 * 摘    要：撤单请求数据结构体描述，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年1月6日
 */
#pragma once
#include "DVJsonDebit.h"
#include "DVJsonDebitCheck.h"
class CDVJsonDebitCancel :
	public CDVJsonBase
{
public:
	CDVJsonDebitCancel(void);
	~CDVJsonDebitCancel(void);
	CDVJsonDebitCancel(IN const CDVJsonDebit* pDVJsonDebit);
	CDVJsonDebitCancel(IN const CDVJsonDebitCheck* pDVJsonDebitCheck);
//运算符重载
	CDVJsonDebitCancel& operator=(IN const CDVJsonDebitCancel& cSrc);
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
	//扣款识别码
	CString PayIdentifier;
	//扣款类型
	int		DebitType;
	//扣款金额
	int		DebitMoney;
	//扣款递交时间(与该条记录的产生时间一致)
	CTime	DebitTime;

	//撤单请求方类型
	int		RequestingPartyType;

	/***********************************************后台回馈需要填充的数据********************************************************/
	//扣款订单号(仅仅当撤单成功才有可能有该项数据)
	CString	DebitOrderNumber;
};