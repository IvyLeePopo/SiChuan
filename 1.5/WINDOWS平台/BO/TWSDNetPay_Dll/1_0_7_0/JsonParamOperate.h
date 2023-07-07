/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonParamOperate.h
 * 摘    要：接口参数JSON格式操作类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月15日
 */
#pragma once
#include "jsonoperationif.h"

class CJsonParamOperate :
	public CJsonOperationIF
{
public:
	CJsonParamOperate(void);
	~CJsonParamOperate(void);
//重载函数
public:
	//将接收到的数据结构转化为标准的JSON格式 -- 支付请求参数数据变换(扣费结果)
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult);
	//将接收到的数据格式转化为标准的JSON格式 -- 用户数据参数数据变换
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount);

	//将JSON格式字符串转化为标准数据结构类
	CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType);
private:
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign) {return _T("");}
};
