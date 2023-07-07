/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonNFCSignIn.h
 * 摘    要：NFC 模块签到
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月20日
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonNFCSignIn : public CDVJsonBase
{
public:
	CDVJsonNFCSignIn(void);
	~CDVJsonNFCSignIn(void);
	//运算符重载
	CDVJsonNFCSignIn& operator=(IN const CDVJsonNFCSignIn& cSrc);
//Interface
public:
	//重置数据
	void  Reset(IN const int& iType=0){}
	//生成数据校验码
	CString CreateInnerVerifyCode(void){return _T("");}
	//生成全字段数据加密结果
	const CString CreateEnDataString(void) const {return _T("");}
	//内部数据码校验
	BOOL  InnerVerify(void){return TRUE;}
//属性
public:
	/*********************基本信息*********************/

	//是否测试数据
	BOOL	TestingData;
	CString	NetPayDllVersion;
	CString DeviceDllVersion;
	CString TWProductSerialNo;//特微硬件终端序号

	//NFC模块终端序号
	CString NFCTerminal;
	//NFC模块终端序号
	CString NFCSerialNo;
	//NFC签到报文
	CString NFCSignData;
};
