/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonNFCOperate.cpp
 * ժ    Ҫ��NFC��JSON���ݸ�ʽ������
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��20��
 */
#include "StdAfx.h"
#include "JsonNFCOperate.h"
#include "TWSDNetPay_Dll.h"

CJsonNFCOperate::CJsonNFCOperate(void)
{
}

CJsonNFCOperate::~CJsonNFCOperate(void)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonNFCOperate::FormatSendData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- NFCǩ��
//     
// ���������IN const CDVJsonHeart* pDVJsonHeart
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2017��09��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
 CString CJsonNFCOperate::FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSignIn)
{
	CString strReturn;
	if(NULL != pDVJsonNFCSignIn)
	{
		// ��ʾ���� json ����
		Json::Value root;
		/***********************************************************����JSAO���ݽṹ***********************************************************/
		
		//��ʶ�汾
		root["Version"]				= Json::Value(pDVJsonNFCSignIn->Version);
		//��ʶ��������
		root["PackageType"]			= Json::Value(pDVJsonNFCSignIn->PackageType);
		//��ȫ��֤
		root["Token"]				= Json::Value(pDVJsonNFCSignIn->Token);
		//��ʶʡ��
		root["ProvinceID"]			= Json::Value(pDVJsonNFCSignIn->ProvinceID);
		//�ݽ�����
		root["SubmitCount"]			= Json::Value(pDVJsonNFCSignIn->SubmitCount);
		//�ݽ�ʱ��
		root["SubmitTime"]			= Json::Value(pDVJsonNFCSignIn->SubmitTime);
		//NFC�ն�ģ�����
		root["Terminal"]			= Json::Value(pDVJsonNFCSignIn->NFCTerminal);
		//NFC�ն�ģ��Ӳ�����
		root["Sn"]					= Json::Value(pDVJsonNFCSignIn->NFCSerialNo);
		//ǩ������
		root["Data"]				= Json::Value(pDVJsonNFCSignIn->NFCSignData);
		//��΢��ƷӲ���ն����
		root["TWProductSerialNo"]	= Json::Value(pDVJsonNFCSignIn->TWProductSerialNo);

		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	 return strReturn;
 }

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonNFCOperate::FormatResponseResult2Class
//     
// ���������������������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
//     
// ���������IN const CString& strResponseResult
//         ��IN const int& iDataType
// ���������CDVJsonBase*
// ��д��Ա��ROCY
// ��дʱ�䣺2017��09��18��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonNFCOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonNFCSignIn* pDVJsonNFCSignIn = new CDVJsonNFCSignIn();
	try
	{
		if(NULL == pDVJsonNFCSignIn)
			throw -1;

		//�������ݸ�ʽת��
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
		{
			strResult.ReleaseBuffer();
			throw -2;
		}
		
		if(Json::nullValue == root.type())
			throw -3;

		//��ȡ�汾
		if(Json::nullValue == root["Version"].type())
			throw -4;
		pDVJsonNFCSignIn->Version					= root["Version"].asCString();
		//��ȡ���ݰ�����
		if(Json::nullValue == root["PackageType"].type())
			throw -4;
		pDVJsonNFCSignIn->PackageType				= root["PackageType"].asInt();
		//��ȡ��Ӧ״̬����
		if(Json::nullValue == root["Status"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerResponseCode		= root["Status"].asInt();

		//��ȡ������������ǩ������
		if(Json::nullValue == root["ReData"].type())
			throw -4;
		CString strTemp = root["ReData"].asCString();
		::GetGlobalApp()->Base64ToHex(strTemp,pDVJsonNFCSignIn->NFCSignData);
			
		//��ȡ��Ӧ״̬����
		if(Json::nullValue == root["Message"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerResponseResultDesc = root["Message"].asCString();
		//��ȡ��������������ʱ��
		if(Json::nullValue == root["ReceveTime"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerReceiveDataTime	= root["ReceveTime"].asCString();
		//��ȡ��������Ӧ����ʱ��
		if(Json::nullValue == root["ResponseTime"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerResponseDataTime	= root["ResponseTime"].asCString();

		}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://������Դʧ��
			break;
		case -2://���ݽ���ʧ��
			break;
		}
	}
	//���ؽ������
	return pDVJsonNFCSignIn;
}
