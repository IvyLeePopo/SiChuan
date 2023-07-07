/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonHeartOperate.cpp
 * ժ    Ҫ���������ݽṹ��������ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��12��14��
 */
#include "StdAfx.h"
#include "JsonHeartOperate.h"

CJsonHeartOperate::CJsonHeartOperate(void)
{
}

CJsonHeartOperate::~CJsonHeartOperate(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonHeartOperate::FormatSendData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ����
//     
// ���������IN const CDVJsonHeart* pDVJsonHeart
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
 CString CJsonHeartOperate::FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart)
 {
	 CString strReturn;
	 return strReturn;
 }

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonHeartOperate::FormatResponseResult2Class
//     
// ���������������������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
//     
// ���������IN const CString& strResponseResult
//         ��IN const int& iDataType
// ���������CDVJsonBase*
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��25��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonHeartOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonHeart* pDVJsonHeart = new CDVJsonHeart();
	try
	{
		if(NULL == pDVJsonHeart)
			throw -1;

		//�������ݸ�ʽת��
/*	
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
		{
			strResult.ReleaseBuffer();
			throw -2;
		}
		
		if(Json::nullValue != root.type())
			throw -3;

		//��ȡ�汾
		if(Json::nullValue == root["Version"].type())
			throw -4;
		pDVJsonEntryQuery->Version					= root["Version"].asCString();
		//��ȡ���ݰ�����
		if(Json::nullValue == root["PackageType"].type())
			throw -4;
		pDVJsonEntryQuery->PackageType				= root["PackageType"].asInt();
		//��ȡ��Ӧ״̬����
		if(Json::nullValue == root["Status"].type())
			throw -4;
		pDVJsonEntryQuery->ServerResponseCode		= root["Status"].asInt();
		//��ȡ��Ӧ״̬����
		if(Json::nullValue == root["Message"].type())
			throw -4;
		pDVJsonEntryQuery->ServerResponseResultDesc = root["Message"].asCString();
		//��ȡ��������������ʱ��
		if(Json::nullValue == root["ReceveTime"].type())
			throw -4;
		pDVJsonEntryQuery->ServerReceiveDataTime	= root["ReceveTime"].asCString();
		//��ȡ��������Ӧ����ʱ��
		if(Json::nullValue == root["ResponseTime"].type())
			throw -4;
		pDVJsonEntryQuery->ServerResponseDataTime	= root["ResponseTime"].asCString();
*/

		/********************************************************************�������********************************************************************/
		
		//�������
		pDVJsonHeart->ServerResponseCode		= HTTP_RC_DATA_VALID;
		pDVJsonHeart->ServerReceiveDataTime		= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonHeart->ServerResponseDataTime	= GetGlobalApp()->GetCurrentTimeString(1);
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
	return pDVJsonHeart;
}
