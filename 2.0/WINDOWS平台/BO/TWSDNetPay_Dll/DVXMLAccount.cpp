/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLAccountContext.cpp
 * ժ    Ҫ���û�������ϢXML�ṹ�壬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
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
// �������ƣ�CDVXMLAccountContext::Init
//     
// ��������������XML�ַ���
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
			m_strLastErrorDesc.Format(_T("�û���Ϣ����ʧ��,�޷��ҵ�ָ������ͷ DebitCancelParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�û���Ϣ����ʧ��,�޷��ҵ�ָ�����ݰ汾 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("�û���Ϣ����ʧ��,�޷��ҵ�ָ�������� Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("�û���Ϣ����ʧ��,�޷��ҵ�ָ��������(��������) Type"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("�û���Ϣ����ʧ��,�޷��ҵ�ָ��������(������Դ) Source"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("�û���Ϣ����ʧ��,�޷��ҵ�ָ��������(��������) Context"));
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVXMLAccountContext::GetXMLString
//     
// �����������ṩXML�ַ���
//     
// ���������IN const CString& strData
//         ��IN const int& iDataType
//         ��IN const int& iSource
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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