/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitCancel.cpp
 * 摘    要：撤单XML数据结构体，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#include "StdAfx.h"
#include "DVXMLDebitCancel.h"

CDVXMLDebitCancel::CDVXMLDebitCancel(void)
	:PayIdentifier(_T(""))
{
	XMLFunctionDesc.Format(_T("DebitCancelParamContext"));
}

CDVXMLDebitCancel::~CDVXMLDebitCancel(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVXMLDebitCancel::Init
//     
// 功能描述：解析传入的撤单参数字符串
//     
// 输入参数：IN const CString& strXMLString
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CDVXMLDebitCancel::Init(IN const CString& strXMLString)
{
	BOOL bReturn = FALSE;
	CMarkup xml;
	try
	{
		xml.SetDoc( strXMLString );

		xml.ResetMainPos();

		if(!xml.FindElem(XMLFunctionDesc))
			throw -1;

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Version")))
			throw -2;
		XMLVersion = xml.GetChildData();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Data")))
			throw -3;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("TradeKeyItem")))
			throw -4;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("PayIdentifier")))
			throw -5;
		PayIdentifier = xml.GetChildData();

		xml.OutOfElem();

		xml.OutOfElem();
	
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("撤单传入参数解析失败,无法找到指定数据头 DebitCancelParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("撤单传入参数解析失败,无法找到指定数据版本 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("撤单传入参数解析失败,无法找到指定数据区 Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("撤单传入参数解析失败,无法找到指定数据项(超时时间) TradeKeyItem"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("撤单传入参数解析失败,无法找到指定数据项(扣款模式) PayIdentifier"));
			break;
		}
	}

	return bReturn;
}