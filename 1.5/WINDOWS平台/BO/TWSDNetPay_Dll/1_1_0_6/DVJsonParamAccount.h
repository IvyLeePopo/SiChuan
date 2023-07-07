/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonAccount.h
 * 摘    要：获取用户信息JSON数据格式解析，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonParamAccount :
	public CDVJsonBase
{
public:
	CDVJsonParamAccount(void);
	~CDVJsonParamAccount(void);
//运算符重载
	CDVJsonParamAccount& operator=(IN const CDVJsonParamAccount& cSrc);
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
	//数据类型
	int	DataType;
	//数据来源
	int	DataSrc;
	//数据内容
	CString DataContext;
};
