/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLBase.h
 * 摘    要：银联XML数据格式类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月17日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLUnipay :
	public CDVXMLBase
{
public:
	CDVXMLUnipay(void);
	~CDVXMLUnipay(void);
//Interface
public:
	//解析XML字符串
	const BOOL Init(IN const CString& strXMLString);
	//构造NFC相关的格式化数据
	const CString ConstructNFCFrameData(IN const CString& strData,IN const int& iType);
//Attributes
public:
	//模块识别码
	int Module;
	//数据类型
	int Type;
	//数据内容（16进制）
	CString Context;
};
