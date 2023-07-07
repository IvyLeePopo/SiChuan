/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�JsonOperationIF.h
 * ժ    Ҫ��Json���׼��������л���ת���Ľӿڶ�����࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��12��
 */
#pragma once
#include "BaseDefine.h"
#include "DVJsonBase.h"
#include "DVJsonDebit.h"
#include "DVJsonDebitCheck.h"
#include "DVJsonDebitCancel.h"
#include "DVJsonHeart.h"
#include "DVJsonDebitSecond.h"
#include "DVJsonNFCSignIn.h"

#include "DVJsonParamAccount.h"
#include "DVJsonParamDebitMoney.h"
#include "DVJsonParamDebitCancel.h"
#include "DVJsonParamDebitResult.h"
#include "DVJsonParamMMI.h"
//���Ƶ������
//#ifndef _JSON_DEBUG_LOG_STRING
//#define _JSON_DEBUG_LOG_STRING 1
//#endif

class CJsonOperationIF
{
public:
	CJsonOperationIF();
	~CJsonOperationIF(void);
	const CString GetLastErrorDesc(){return m_strLastErrorDesc;}
//�ӿڶ���
public:
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(�۷�)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum)=0;
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(��ѯ)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum)=0;
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(����)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel)=0;
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������(���δ���)
	virtual CString	FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond)=0;

	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ����
	virtual CString	FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart)=0;
	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- NFCǩ��
	virtual CString FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSign)=0;

	//�����յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������������ݱ任(�۷ѽ��)
	virtual CString FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult)=0;
	//�����յ������ݸ�ʽת��Ϊ��׼��JSON��ʽ -- �û����ݲ������ݱ任
	virtual CString FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount)=0;

	//�����������ص�JSON��ʽת��Ϊ��׼���ݽṹ��
	virtual CDVJsonBase* FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)=0;
//����
protected:
	//��ʱ�����ߴ�����
	HWND	m_hCallerWnd;
	//�����Ĵ�����Ϣ����
	CString	m_strLastErrorDesc;
};
