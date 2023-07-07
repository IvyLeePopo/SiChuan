/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonHeartOperate.h
 * 摘    要：心跳数据结构体描述，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年12月14日
 */
#pragma once
#include "jsonoperationif.h"
#include "TWSDNetPay_Dll.h"

class CJsonHeartOperate :
	public CJsonOperationIF
{
public:
	CJsonHeartOperate(void);
	~CJsonHeartOperate(void);
//重载函数
public:
	//将接收到的数据结构转化为标准的JSON格式 -- 心跳
	 CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart);

	//将服务器返回的JSON格式转化为标准数据结构类
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
	CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign) {return _T("");}

	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return _T("");}
	//该函数不需要外部调用，屏蔽且并不实现
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount) {return _T("");}

};
