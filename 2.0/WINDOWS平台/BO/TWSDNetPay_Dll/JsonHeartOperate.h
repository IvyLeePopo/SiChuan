/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonHeartOperate.h
 * ժ    Ҫ���������ݽṹ�������������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��12��14��
 */
#pragma once
#include "jsonoperationif.h"
#include "TWSDNetPay_Dll.h"

class CJsonHeartOperate :
	public CJsonOperationIF
{
public:
	CJsonHeartOperate(void);
	~CJsonHeartOperate(void);
//���غ���
public:
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ����
	 CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart);

	//�����������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
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
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount) {return _T("");}

};
