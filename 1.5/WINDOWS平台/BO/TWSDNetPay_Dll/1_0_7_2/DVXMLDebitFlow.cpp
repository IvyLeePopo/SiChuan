/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitFlow.cpp
 * ժ    Ҫ�����Ͽ۷�ָ��XML��ʽ���ṹ���ݣ�ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��21��
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
// �������ƣ�CDVXMLDebitFlow::Init
//     
// ��������������XML�ַ���
//     
// ���������IN const CString& strXMLString
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ������ͷ DisplayContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ�����ݰ汾 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ�������� Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ��������(��������) Type"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ��������(�۷ѽ��) Money"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ��������(�۷ѳ���) Weight"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ��������(�۷ѳ���) VehicleType"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ��������(�۷ѳ���) VehicleClass"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("�۷�����XML�ڲ����ݽ���ʧ��,�޷��ҵ�ָ��������(�·�ʱ��) CurrentTime"));
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVXMLDebitFlow::ConstructFormatData
//     
// ��������������XML��ʽ������
//     
// ����������޲���
// ���������const CString
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
