/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonDebitCancel.h
 * ժ    Ҫ�������������ݽṹ�������������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��1��6��
 */
#pragma once
#include "DVJsonDebit.h"
#include "DVJsonDebitCheck.h"
class CDVJsonDebitCancel :
	public CDVJsonBase
{
public:
	CDVJsonDebitCancel(void);
	~CDVJsonDebitCancel(void);
	CDVJsonDebitCancel(IN const CDVJsonDebit* pDVJsonDebit);
	CDVJsonDebitCancel(IN const CDVJsonDebitCheck* pDVJsonDebitCheck);
//���������
	CDVJsonDebitCancel& operator=(IN const CDVJsonDebitCancel& cSrc);
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
	//�ۿ�ʶ����
	CString PayIdentifier;
	//�ۿ�����
	int		DebitType;
	//�ۿ���
	int		DebitMoney;
	//�ۿ�ݽ�ʱ��(�������¼�Ĳ���ʱ��һ��)
	CTime	DebitTime;

	//������������
	int		RequestingPartyType;

	/***********************************************��̨������Ҫ��������********************************************************/
	//�ۿ����(�����������ɹ����п����и�������)
	CString	DebitOrderNumber;
};