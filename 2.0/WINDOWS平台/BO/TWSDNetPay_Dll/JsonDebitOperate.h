/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonDebit.h
 * ժ    Ҫ�����Ʒ���ۿ�����Ĳ������������ݴ�ȡ�Լ���ʽ�����̣������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��12��
 */
#pragma once
#include "JsonOperationIF.h"
#include "TWSDNetPay_Dll.h"

class CJsonDebitOperate : public CJsonOperationIF
{
public:
	CJsonDebitOperate(void);
	~CJsonDebitOperate(void);
//���غ���
public:
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(�۷�)
	CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit);
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(��ѯ)
	CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck);
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(����)
	CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel);
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧�����󣨶��δ���
	CString FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond);

	//�����������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
	CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType);



private:
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart){return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return _T("");}
	//�ú�������Ҫ�ⲿ���ã������Ҳ���ʵ��
	CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount) {return _T("");}

	/******************************�ڲ���������******************************/

	//�����ַ��������֧�����󣨿۷ѣ�����˻�������
	CDVJsonDebit* fnAnalysisStringForDebit(IN const CString& strResponseResult);
	//�����ַ��������֧�����󣨲�ѯ������˻�������
	CDVJsonDebitCheck* fnAnalysisStringForDebitCheck(IN const CString& strResponseResult);
	//�����ַ��������֧�����󣨳���������˻�������
	CDVJsonDebitCancel* fnAnalysisStringForDebitCancel(IN const CString& strResponseResult);
	//�����ַ��������֧�����󣨶��δ�������˻�������
	CDVJsonDebit* fnAnalysisStringForDebitSecond(IN const CString& strResponseResult);
};
