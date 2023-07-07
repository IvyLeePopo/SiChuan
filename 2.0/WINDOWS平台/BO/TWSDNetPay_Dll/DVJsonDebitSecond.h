/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonDebitSecond.h
 * 摘    要：二次扣款处理的数据结构体描述类
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年5月30日
 */
#pragma once
#include "dvjsonbase.h"
#include "DVJsonDebit.h"

class CDVJsonDebitSecond :
	public CDVJsonBase
{
public:
	CDVJsonDebitSecond(IN const CDVJsonDebit* pDVJsonDebit);
	~CDVJsonDebitSecond(void);
//运算符重载
	CDVJsonDebitSecond& operator=(IN const CDVJsonDebitSecond& cSrc);
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

	//扣款信息结构体
	CDVJsonDebit m_cDVJsonDebit;
	//上一次扣款支付请求识别码
	CString		 m_strLastPayIdentifier;
};
