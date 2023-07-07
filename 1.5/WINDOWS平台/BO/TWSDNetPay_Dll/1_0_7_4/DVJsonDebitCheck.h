/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonDebitCheck.h
 * ժ    Ҫ���ۿ�״̬��ѯJSON�ṹ��Ϣ�����������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��12��04��
 */
#pragma once
#include "DVJsonDebit.h"
class CDVJsonDebitCheck :
	public CDVJsonBase
{
public:
	CDVJsonDebitCheck(void);
	CDVJsonDebitCheck(IN const CDVJsonDebit* pDVJsonDebit);
	CDVJsonDebitCheck(IN const CDVJsonDebitCheck* pDVJsonDebitCheck);
	~CDVJsonDebitCheck(void);
//���������
	CDVJsonDebitCheck& operator=(IN const CDVJsonDebitCheck& cSrc);
//Interface
public:
	//��������
	void  Reset(IN const int& iType=0);
	//��������У����
	DWORD CreateInnerVerifyCode(void){return 0;}
	//�ڲ�������У��
	BOOL  InnerVerify(void){return TRUE;}
//����
public:
	/***********************************************����������Ҫ��������********************************************************/

	//���������Ľ���ƾ֤��
	CString RecordMatchCode;
	//�ۿ�ʶ����
	CString PayIdentifier;

	//��ά��󶨵�֧��ƽ̨ID
	CString IDBindInPayPlatform;
	//��ά��󶨵�֧��ƽ̨����
	int		IDBindInPayPlatformType;
	//�ۿ�����
	int		DebitType;
	//�ۿ���
	int		DebitMoney;
	//�ۿ�ݽ�ʱ��(�������¼�Ĳ���ʱ��һ��)
	CTime	DebitTime;

	//�ۿ�״̬��ѯ�ӳ�ʱ��(��λ������)
	DWORD	CheckStateDelayTime;

	/***********************************************��̨������Ҫ��������********************************************************/

	//�ۿ�״̬����
	int		DebitStatusCode;
	//�ۿ�״̬����
	CString DebitStatusDesc;
	//�ۿ����(�������ۿ�ɹ����и�������)
	CString	DebitOrderNumber;
};
