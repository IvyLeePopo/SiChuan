/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVJsonHerat.h
 * 摘    要：心跳接口数据交互描述结构体，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年12月14日
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonHeart : public CDVJsonBase
{
public:
	CDVJsonHeart(void);
	~CDVJsonHeart(void);
//运算符重载
	CDVJsonHeart& operator=(IN const CDVJsonHeart& cSrc);
public:
	//重置数据
	void  Reset(IN const int& iType=0);
	//生成数据校验码
	DWORD CreateInnerVerifyCode(void){return 0;}
	//内部数据码校验
	BOOL  InnerVerify(void){return TRUE;}
//属性
public:
	//站点编码
	CString		StationID;
	//站点名称
	CString		StationName;
	//车道编码
	CString		LaneID;
	//心跳时间
	CTime	HeartTime;
};
