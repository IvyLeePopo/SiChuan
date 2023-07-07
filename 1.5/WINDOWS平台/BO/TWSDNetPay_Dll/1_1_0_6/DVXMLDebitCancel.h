/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitCancel.h
 * 摘    要：撤单XML数据结构体，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitCancel :
	public CDVXMLBase
{
public:
	CDVXMLDebitCancel(void);
	~CDVXMLDebitCancel(void);
//Interface
public:
//解析XML字符串
	const BOOL Init(IN const CString& strXMLString);
//Attributes
public:
	//交易识别码
	CString PayIdentifier;
};
