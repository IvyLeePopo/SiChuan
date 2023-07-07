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
#include "TWSDNetPay_Dll.h"


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
	CString strXMLString = _T("<?xml version=\"1.0\" encoding=\"GB2312\" ?> \r\n");
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
	//remove zhuyabing 2017-06 xml.AddElem(_T("ConsumeTime"),			ConsumeTime);
	xml.OutOfElem();

	xml.OutOfElem();
	xml.OutOfElem();

	strXMLString = xml.GetDoc();

	return strXMLString;
}

const BOOL CDVXMLDebitResult::Init(IN const CString& strXMLString)

{
	BOOL bReturn = FALSE;
	CString strTemp;
	CMarkup xml;
	try
	{
		xml.SetDoc( strXMLString );

		xml.ResetMainPos();

		/****************************************************************数据格式判断****************************************************************/

		if(!xml.FindElem(XMLFunctionDesc))
			throw -1;
		xml.IntoElem(); //root

		xml.ResetPos();
		if(!xml.FindChildElem(_T("Version")))
			throw -2;
		XMLVersion = xml.GetChildData();

		xml.ResetPos();
		if(!xml.FindChildElem(_T("Data")))
			throw -3;
		xml.IntoElem(); //Data

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("ResultType")))
			throw -4;
		ResultType = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("ExecuteCode")))
			throw -5;
		ExecuteCode = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("ExecuteDesc")))
			throw -6;
		ExecuteDesc = xml.GetChildData();

		/*****************DebitResult详细项************************/
		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("TradeKeyItem")))
			throw -7;
		xml.IntoElem(); //TradeKeyItem

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("TradeSecurityCode")))
			throw -8;
		TradeSecurityCode = (xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("PayPlatformType")))
			throw -9;
		PayPlatformType = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("PayIdentifier")))
			throw -10;
		PayIdentifier = (xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("DebitOrder")))
			throw -11;
		DebitOrder = (xml.GetChildData());



		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("DebitTime")))
			throw -12;
		strTemp = xml.GetChildData();
		DebitTime = CTime(GetGlobalApp()->CString2time_t(strTemp));

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("ConsumeTime")))
			throw -13;
		ConsumeTime = _ttoi(xml.GetChildData());
	
		xml.OutOfElem(); //TradeKeyItem
		xml.OutOfElem(); //Data

		xml.OutOfElem(); //root

	
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据头 %s"), XMLFunctionDesc);
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据版本 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 ResultType"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 ExecuteCode"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 ExecuteDesc"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 TradeKeyItem"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 TradeSecurityCode"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 PayPlatformType"));
			break;
		case -10:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 PayIdentifier"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 DebitOrder"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 DebitTime"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("扣款结果传出参数解析失败,无法找到指定数据区 ConsumeTime"));
			break;	
	
		}
	}

	return bReturn;
}
