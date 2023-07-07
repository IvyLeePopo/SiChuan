/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonNFCOperate.h
 * 摘    要：NFC签到JSON操作类
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月20日
 */
#pragma once
#include "JsonOperationIF.h"

class CJsonNFCOperate : public CJsonOperationIF
{
public:
	CJsonNFCOperate(void);
	~CJsonNFCOperate(void);
//重载函数
public:
	//将接收到的数据结构转化为标准的JSON格式 -- 签到
	CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign);
	//将服务器返回的JSON格式转化为标准数据结构类
	CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType);
protected:
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel){return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart) {return _T("");}

	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount) {return _T("");}
};
