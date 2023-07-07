/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonParamDebitCancel.h
 * 摘    要：撤单接口参数JSON数据格式解析，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonParamDebitCancel :
	public CDVJsonBase
{
public:
	CDVJsonParamDebitCancel(void);
	~CDVJsonParamDebitCancel(void);
//运算符重载
	CDVJsonParamDebitCancel& operator=(IN const CDVJsonParamDebitCancel& cSrc);
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
	//交易识别码
	CString PayIdentifier;
};
