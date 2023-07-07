/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLBase.h
 * 摘    要：所有XML数据格式的公用基类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年3月12日
 */
#pragma once
#include "BaseDefine.h"

class CDVXMLBase
{
public:
	CDVXMLBase(void);
	~CDVXMLBase(void);
//Interface
public:
	//获取错误信息
	const CString GetLastErrorDesc(){return m_strLastErrorDesc;}
	//解析XML字符串
	virtual const BOOL	  Init(IN const CString& strXMLString)=0;
//Attributes
public:
	//所有XML文件的文件头
	CString	XMLHead;
	//继承类是XML文件结构描述体，所具现的内容分类描述
	CString XMLFunctionDesc;
	//XMl文件协议版本
	CString XMLVersion;
	//校验码
	CString XMLVerifyCode;
	//错误描述
	CString m_strLastErrorDesc;
};
