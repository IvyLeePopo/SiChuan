/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonOperationIF.cpp
 * 摘    要：Json与标准数据类进行互相转换的接口定义基类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年12月14日
 */
#include "StdAfx.h"
#include "JsonOperationIF.h"
#include "TWSDNetPay_Dll.h"

CJsonOperationIF::CJsonOperationIF(void)
	:m_strLastErrorDesc(_T(""))
{
}

CJsonOperationIF::~CJsonOperationIF(void)
{
}
