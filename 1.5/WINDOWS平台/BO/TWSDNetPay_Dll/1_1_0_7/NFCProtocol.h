/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：NFCProtocol.h
 * 摘    要：银联项目非接模块协议解析类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月21日
 */
#pragma once
#include "BaseDefine.h"

class CNFCProtocol
{
public:
	CNFCProtocol(void);
	~CNFCProtocol(void);
//Operations
public:
	//解析NFC模块回馈的数据,返回结果见枚举类型 UNIPAY_FRAME_TYPE
	const int Analyze(IN const CString& strRevData);
	//构造停止执行指令
	const CString ConstructStopExecuteCmd(void);
	//构造发起签到指令
	const CString ConstructStartSignInCmd(void);
	//构造HTTP回馈的签到指令
	const CString ConstructSignDataFromHttpCmd(IN const CString& strContext);
	//构造发起消费指令
	const CString ConstructStartConsumeCmd(IN const int& iMoney);
	//构造执行具体消费类型指令
	const CString ConstructExecuteConsumeCmd(IN const int& iConsumeType);
	//构造冲正指令
	const CString ConstructCorrectCmd(void);
	//构造链接测试指令
	const CString ConstructTestLinkCmd(void);

	//add 2018-04
	const CString ConstructParaQueryCmd(void);
	const CString ConstructSoftVerQueryCmd(void);
//Attribute
public:
	//数据帧类型（见枚举类型 UNIPAY_FRAME_TYPE）
	int FramrType;
	//是否已经解析
	BOOL IsAyalyze;
	
	//报文头
	BYTE Head;
	//报文尾
	BYTE End;
	//长度
	WORD Length;
	//路由
	BYTE Path;
	//校验码
	BYTE LrcVerify;
	//交易标识ID
	CString TradeID;
	//数据域
	CString Context;
	BYTE	BinData[5120];

	//当前消费的商户号
	BYTE	MerchantBin[15];
	CString MerchantHex;
	//当前消费的终端号
	BYTE	TerminalBin[8];
	CString TerminalHex;
	//当前消费错误代码（如果有）
	BYTE	CurrentConsumeErrorCode;
};
