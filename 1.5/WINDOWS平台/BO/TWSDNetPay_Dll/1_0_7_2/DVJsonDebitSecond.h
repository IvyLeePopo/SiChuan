/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonDebitSecond.h
 * ժ    Ҫ�����οۿ������ݽṹ��������
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��5��30��
 */
#pragma once
#include "dvjsonbase.h"
#include "DVJsonDebit.h"

class CDVJsonDebitSecond :
	public CDVJsonBase
{
public:
	CDVJsonDebitSecond(IN const CDVJsonDebit* pDVJsonDebit);
	~CDVJsonDebitSecond(void);
//���������
	CDVJsonDebitSecond& operator=(IN const CDVJsonDebitSecond& cSrc);
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

	//�ۿ���Ϣ�ṹ��
	CDVJsonDebit m_cDVJsonDebit;
	//��һ�οۿ�֧������ʶ����
	CString		 m_strLastPayIdentifier;
};
