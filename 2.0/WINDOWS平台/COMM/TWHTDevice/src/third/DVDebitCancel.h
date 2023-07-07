/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVDebitCancel.h
 * 摘    要：撤单数据本地保存数据描述类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月25日
 */
#pragma once

class CDVDebitCancel
{
public:
	CDVDebitCancel(void);
	~CDVDebitCancel(void);
//Attributes
public:
	//省份标识
	int		ProvinceID;
	//路段标识
	int		RoadID;
	//站点编码
	int		StationID;
	//撤单数据的处理标识(0,未处理;1,已处理)
	int		ProcessMark;
	//撤单交易识别码
	CString PayIdentifier;
	//递交时间
	CString	SubTime;
	//记录MD5摘要
	CString VerifyCode;
	//撤单创建时间
	CTime	CreateTime;
	//撤单实际执行时间
	CTime	ExecuteTime;
};
