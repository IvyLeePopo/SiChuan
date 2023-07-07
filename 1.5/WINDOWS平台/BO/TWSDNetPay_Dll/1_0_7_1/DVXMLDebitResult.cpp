/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitResult.cpp
 * 摘    要：扣款结果参数XML数据结构，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#include "StdAfx.h"
#include "DVXMLDebitResult.h"

CDVXMLDebitResult::CDVXMLDebitResult(void)
	:ResultType(0)
	,ExecuteCode(-1)
	,ExecuteDesc(_T(""))

	,PayIdentifier(_T(""))
	,PayPlatformType(0)
	,TradeSecurityCode(_T(""))
	,DebitOrder(_T(""))
{
	XMLFunctionDesc.Format(_T("DebitResultContext"));
}

CDVXMLDebitResult::~CDVXMLDebitResult(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVXMLDebitResult::GetXMLString
//     
// 功能描述：生成XML字符串
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CDVXMLDebitResult::GetXMLString(void)
{
	CString strXMLString;
	CMarkup xml; 
	xml.SetDoc( strXMLString );

	xml.AddElem(XMLFunctionDesc);
	xml.IntoElem();

	xml.AddElem(_T("Version"),XMLVersion); 
	xml.AddElem(_T("Data")); 
	xml.IntoElem();

	xml.AddElem(_T("ResultType"),	ResultType); 
	xml.AddElem(_T("ExecuteCode"),	ExecuteCode); 
	xml.AddElem(_T("ExecuteDesc"),	ExecuteDesc); 

	xml.AddElem(_T("TradeKeyItem")); 
	xml.IntoElem();
	xml.AddElem(_T("TradeSecurityCode"),	TradeSecurityCode); 
	xml.AddElem(_T("PayPlatformType"),		PayPlatformType); 
	xml.AddElem(_T("PayIdentifier"),		PayIdentifier); 
	xml.AddElem(_T("DebitOrder"),			DebitOrder); 
	xml.AddElem(_T("DebitTime"),			DebitTime.Format(_T("%Y-%m-%d %H:%M:%S"))); 
	xml.AddElem(_T("ConsumeTime"),			ConsumeTime);
	xml.OutOfElem();

	xml.OutOfElem();
	xml.OutOfElem();

	strXMLString = xml.GetDoc();

	return strXMLString;
}