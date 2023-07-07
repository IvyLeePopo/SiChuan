/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonBase.h
 * 摘    要：Json数据载体的基本类，其它类继承于它，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#pragma once
#include "BaseDefine.h"

class CDVJsonBase
{
public:
	CDVJsonBase(void);
	~CDVJsonBase(void);
//Interface
public:
	//重置数据
	void  Reset(IN const int& iType=0);
	//生成数据校验码
	DWORD CreateInnerVerifyCode(void){return 0;}
	//内部数据码校验
	BOOL  InnerVerify(void){return TRUE;}
//公共属性
public:
	//消耗时间 -- 一般指数据从发送到服务器返回的处理时间
	DWORD	m_dwConsumeTime;
	//调用者窗体句柄
	HWND	m_hCallerWnd;
	//调用者线程ID
	DWORD	m_dwThreadID;
	
	/**************************发起请求需要填充的数据(公共)**************************/
	
	//数据包版本
	CString	Version;
	//数据包类型
	int		PackageType;
	//数据包发送地所属省份
	int		ProvinceID;
	//安全认证
	CString	Token;
	//递交次数
	int		SubmitCount;
	//递交时间
	CString	SubmitTime;
	//QR码数据内容
	CString	QRCodeData;
	//QR码来源
	int		QRCodeType;
	//扣款识别码
	CString PayIdentifier;
	/**************************后台回馈需要填充的数据(公共)**************************/

	//服务器扣款时间
	CTime	ServerDebitTime;

	//服务端反馈代码
	int		ServerResponseCode;	
	//错误信息描述
	CString ServerResponseResultDesc;

	//服务器接收到请求的时间
	CString ServerReceiveDataTime;
	//服务器回馈数据的时间
	CString ServerResponseDataTime;
};
