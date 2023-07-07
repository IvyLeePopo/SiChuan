/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitCancel.cpp
 * ժ    Ҫ������XML���ݽṹ�壬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
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
// �������ƣ�CDVXMLDebitCancel::Init
//     
// ������������������ĳ��������ַ���
//     
// ���������IN const CString& strXMLString
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
			m_strLastErrorDesc.Format(_T("���������������ʧ��,�޷��ҵ�ָ������ͷ DebitCancelParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("���������������ʧ��,�޷��ҵ�ָ�����ݰ汾 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("���������������ʧ��,�޷��ҵ�ָ�������� Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("���������������ʧ��,�޷��ҵ�ָ��������(��ʱʱ��) TradeKeyItem"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("���������������ʧ��,�޷��ҵ�ָ��������(�ۿ�ģʽ) PayIdentifier"));
			break;
		}
	}

	return bReturn;
}