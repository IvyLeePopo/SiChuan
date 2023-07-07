/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitFlow.h
 * 摘    要：整合扣费指令XML格式化结构数据,声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月21日
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitFlow :
	public CDVXMLBase
{
public:
	CDVXMLDebitFlow(void);
	~CDVXMLDebitFlow(void);
//Interface
public:
	//解析XML字符串
	const BOOL Init(IN const CString& strXMLString);
	//构造XML格式化数据
	const CString ConstructFormatData();
//Attributes
public:
	//省份编码
	int ProvinceID;
	//数据类型
	int Type;
	//消费金额，单位：分
	int Money;
	//车辆重量
	int	Weight;
	//车种
	int	VehicleType;
	//车型
	int	VehicleClass;
	//时间
	time_t CurrentTime;
};

