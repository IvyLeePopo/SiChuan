/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonNFCOperate.h
 * ժ    Ҫ��NFCǩ��JSON������
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��20��
 */
#pragma once
#include "JsonOperationIF.h"

class CJsonNFCOperate : public CJsonOperationIF
{
public:
	CJsonNFCOperate(void);
	~CJsonNFCOperate(void);
//���غ���
public:
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ǩ��
	CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign);
	//�����������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
	CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType);
protected:
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart) {return _T("");}

	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount) {return _T("");}
};
