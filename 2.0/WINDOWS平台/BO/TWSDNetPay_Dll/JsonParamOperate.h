/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
* 
* �ļ����ƣ�JsonParamOperate.h
* ժ    Ҫ���ӿڲ���JSON��ʽ�����࣬�����ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��8��15��
*/
#pragma once
#include "jsonoperationif.h"

class CJsonParamOperate :
	public CJsonOperationIF
{
public:
	CJsonParamOperate(void);
	~CJsonParamOperate(void);
	//���غ���
public:
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������������ݱ任(�۷ѽ��)
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult);
	//�����յ������ݸ�ʽת��Ϊ��׼��JSON��ʽ -- �û����ݲ������ݱ任
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount);
	//�����յ������ݸ�ʽת��Ϊ��׼��JSON��ʽ -- �۷Ѳ������ݱ任
	CString FormatParamData2Json(IN const CDVJsonParamDebitMoney* pDVJsonParamDebit);

	//��JSON��ʽ�ַ���ת��Ϊ��׼���ݽṹ��
	CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType);
private:
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart){return _T("");}

};
