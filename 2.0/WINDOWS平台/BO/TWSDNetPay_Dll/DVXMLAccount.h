/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLAccountContext.h
 * 摘    要：用户数据信息XML结构体，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLAccountContext :
	public CDVXMLBase
{
public:
	CDVXMLAccountContext(void);
	~CDVXMLAccountContext(void);
//Interface
public:
	//解析XML字符串
	const BOOL	  Init(IN const CString& strXMLString);
	//提供XML字符串
	const CString GetXMLString(IN const CString& strData,IN const int& iDataType,IN const int& iSource);
//Attributes
public:
	//数据类型
	int		Type;
	//数据来源
	int		Src;
	//数据内容
	CString Context;
};
