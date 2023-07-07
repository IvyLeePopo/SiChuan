/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVDebitCancel.cpp
 * 摘    要：撤单请求数据结构体描述，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月25日
 */
#include "StdAfx.h"
#include "DVDebitCancel.h"

CDVDebitCancel::CDVDebitCancel(void)
	:ProvinceID(0)
	,RoadID(0)
	,StationID(0)
	,ProcessMark(0)
	,PayIdentifier(_T(""))
	,SubTime(_T(""))
	,VerifyCode(_T(""))
{
}

CDVDebitCancel::~CDVDebitCancel(void)
{
}
