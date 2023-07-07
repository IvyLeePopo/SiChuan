/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLAccountContext.cpp
 * 摘    要：用户数据信息XML结构体，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
 */
#include "StdAfx.h"
#include "DVXMLAccount.h"

CDVXMLAccountContext::CDVXMLAccountContext(void)
	:Type(0)
	,Src(0)
	,Context(_T(""))
{
	XMLFunctionDesc.Format(_T("AccountContext"));
}

CDVXMLAccountContext::~CDVXMLAccountContext(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVXMLAccountContext::Init
//     
// 功能描述：解析XML字符串
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
const BOOL CDVXMLAccountContext::Init(IN const CString& strXMLString)
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
		if(!xml.FindChildElem(_T("Type")))
			throw -4;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Source")))
			throw -5;
		Src = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Context")))
			throw -6;
		Context = xml.GetChildData();

		xml.OutOfElem();

		xml.OutOfElem();
	
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("用户信息解析失败,无法找到指定数据头 DebitCancelParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("用户信息解析失败,无法找到指定数据版本 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("用户信息解析失败,无法找到指定数据区 Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("用户信息解析失败,无法找到指定数据项(数据类型) Type"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("用户信息解析失败,无法找到指定数据项(数据来源) Source"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("用户信息解析失败,无法找到指定数据项(数据内容) Context"));
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVXMLAccountContext::GetXMLString
//     
// 功能描述：提供XML字符串
//     
// 输入参数：IN const CString& strData
//         ：IN const int& iDataType
//         ：IN const int& iSource
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CDVXMLAccountContext::GetXMLString(IN const CString& strData,IN const int& iDataType,IN const int& iSource)
{
	CString strXMLString;
	CMarkup xml;
	xml.SetDoc(XMLHead);
	xml.AddElem(XMLFunctionDesc);
	xml.IntoElem(); 
	xml.AddElem(_T("Version"),XMLVersion);

	xml.AddElem(_T("Data"));

	xml.IntoElem(); 
	xml.AddElem(_T("Type"),iDataType);
	xml.AddElem(_T("Source"),iSource);
	xml.AddElem(_T("Context"),strData);
	xml.OutOfElem();

	xml.OutOfElem();

	strXMLString = xml.GetDoc();

	return strXMLString;
}