/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitFlow.cpp
 * 摘    要：整合扣费指令XML格式化结构数据，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月21日
 */
#include "StdAfx.h"
#include "DVXMLDebitFlow.h"

CDVXMLDebitFlow::CDVXMLDebitFlow(void)
{
	XMLFunctionDesc.Format(_T("DisplayContext"));
}

CDVXMLDebitFlow::~CDVXMLDebitFlow(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVXMLDebitFlow::Init
//     
// 功能描述：解析XML字符串
//     
// 输入参数：IN const CString& strXMLString
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CDVXMLDebitFlow::Init(IN const CString& strXMLString)
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
		Type = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Money")))
			throw -5;
		Money = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Weight")))
			throw -6;
		Weight = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("VehicleType")))
			throw -7;
		VehicleType = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("VehicleClass")))
			throw -8;
		VehicleClass = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("CurrentTime")))
			throw -9;
		CurrentTime = _ttol(xml.GetChildData());

		xml.OutOfElem();
	
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据头 DisplayContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据版本 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据区 Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据项(数据类型) Type"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据项(扣费金额) Money"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据项(扣费车重) Weight"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据项(扣费车种) VehicleType"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据项(扣费车型) VehicleClass"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("扣费流程XML内部数据解析失败,无法找到指定数据项(下发时间) CurrentTime"));
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVXMLDebitFlow::ConstructFormatData
//     
// 功能描述：构造XML格式化数据
//     
// 输入参数：无参数
// 输出参数：const CString
// 编写人员：ROCY
// 编写时间：2017年9月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CDVXMLDebitFlow::ConstructFormatData()
{
	CMarkup xmlDebit;
	xmlDebit.SetDoc( XMLHead );
	xmlDebit.AddElem( XMLFunctionDesc );//root
	xmlDebit.IntoElem();

	xmlDebit.AddElem( _T("Version"),	_T("1.0") );
	xmlDebit.AddElem( _T("ProvinceID"), ProvinceID );
	xmlDebit.AddElem( _T("VerifyCode"),	0 );
	xmlDebit.AddElem( _T("Data") );

	xmlDebit.IntoElem();

	xmlDebit.AddElem( _T("Type"),		0x03);

	xmlDebit.AddElem( _T("Money"),		 Money );
	xmlDebit.AddElem( _T("Weight"),		 Weight );
	xmlDebit.AddElem( _T("VehicleType"), VehicleType );
	xmlDebit.AddElem( _T("VehicleClass"),VehicleClass );
	xmlDebit.AddElem( _T("CurrentTime"), CTime(CurrentTime).Format("%Y-%m-%d %H:%M:%S") );

	xmlDebit.OutOfElem();
	xmlDebit.OutOfElem();

	CString strXMLString = xmlDebit.GetDoc();

	return strXMLString;
}
