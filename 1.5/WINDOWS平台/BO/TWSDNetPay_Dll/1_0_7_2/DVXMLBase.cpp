/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLBase.cpp
 * 摘    要：所有XML数据格式的公用基类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年3月12日
 */
#include "StdAfx.h"
#include "DVXMLBase.h"

CDVXMLBase::CDVXMLBase(void)
	:XMLHead(_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>"))
	,XMLFunctionDesc(_T(""))
	,XMLVersion(_T("1.0"))
	,XMLVerifyCode(_T(""))
	,m_strLastErrorDesc(_T(""))
{
}

CDVXMLBase::~CDVXMLBase(void)
{
}
