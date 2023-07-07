/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonOperationIF.h
 * 摘    要：Json与标准数据类进行互相转换的接口定义基类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#pragma once
#include "BaseDefine.h"
#include "DVJsonBase.h"
#include "DVJsonDebit.h"
#include "DVJsonDebitCheck.h"
#include "DVJsonDebitCancel.h"
#include "DVJsonHeart.h"
#include "DVJsonDebitSecond.h"
#include "DVJsonNFCSignIn.h"

#include "DVJsonParamAccount.h"
#include "DVJsonParamDebitMoney.h"
#include "DVJsonParamDebitCancel.h"
#include "DVJsonParamDebitResult.h"
#include "DVJsonParamMMI.h"
//控制调试输出
//#ifndef _JSON_DEBUG_LOG_STRING
//#define _JSON_DEBUG_LOG_STRING 1
//#endif

class CJsonOperationIF
{
public:
	CJsonOperationIF();
	~CJsonOperationIF(void);
	const CString GetLastErrorDesc(){return m_strLastErrorDesc;}
//接口定义
public:
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(扣费)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum)=0;
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(查询)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum)=0;
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(撤单)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel)=0;
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(二次处理)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond)=0;

	//将接收到的数据结构转化为标准的JSON格式 -- 心跳
	virtual CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart)=0;
	//将接收到的数据结构转化为标准的JSON格式 -- NFC签到
	virtual CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign)=0;

	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求参数数据变换(扣费结果)
	virtual CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult)=0;
	//将接收到的数据格式转化为标准的JSON格式 -- 用户数据参数数据变换
	virtual CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount)=0;

	//将服务器返回的JSON格式转化为标准数据结构类
	virtual CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)=0;
//属性
protected:
	//临时调用者窗体句柄
	HWND	m_hCallerWnd;
	//产生的错误信息描述
	CString	m_strLastErrorDesc;
};
