/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonDebit.h
 * 摘    要：控制发起扣款请求的操作，进行数据存取以及格式化流程，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#pragma once
#include "JsonOperationIF.h"
#include "TWSDNetPay_Dll.h"

class CJsonDebitOperate : public CJsonOperationIF
{
public:
	CJsonDebitOperate(void);
	~CJsonDebitOperate(void);
//重载函数
public:
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(扣费)
	CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum);
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(查询)
	CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum);
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求(撤单)
	CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel);
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求（二次处理）
	CString FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond);

	//将服务器返回的JSON格式转化为标准数据结构类
	CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType);

private:
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign) {return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount) {return _T("");}

	/******************************内部辅助函数******************************/

	//解析字符串，获得支付请求（扣费）服务端回馈数据
	CDVJsonDebit* fnAnalysisStringForDebit(IN const CString& strResponseResult);
	//解析字符串，获得支付请求（查询）服务端回馈数据
	CDVJsonDebitCheck* fnAnalysisStringForDebitCheck(IN const CString& strResponseResult);
	//解析字符串，获得支付请求（撤单）服务端回馈数据
	CDVJsonDebitCancel* fnAnalysisStringForDebitCancel(IN const CString& strResponseResult);
	//解析字符串，获得支付请求（二次处理）服务端回馈数据
	CDVJsonDebit* fnAnalysisStringForDebitSecond(IN const CString& strResponseResult);
};
