/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitResult.cpp
 * ժ    Ҫ���ۿ�������XML���ݽṹ��ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
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
// �������ƣ�CDVXMLDebitResult::GetXMLString
//     
// ��������������XML�ַ���
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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

		/****************************************************************���ݸ�ʽ�ж�****************************************************************/

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

		/*****************DebitResult��ϸ��************************/
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
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ������ͷ %s"), XMLFunctionDesc);
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�����ݰ汾 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� ResultType"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� ExecuteCode"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� ExecuteDesc"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� TradeKeyItem"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� TradeSecurityCode"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� PayPlatformType"));
			break;
		case -10:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� PayIdentifier"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� DebitOrder"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� DebitTime"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("�ۿ���������������ʧ��,�޷��ҵ�ָ�������� ConsumeTime"));
			break;	
	
		}
	}

	return bReturn;
}
