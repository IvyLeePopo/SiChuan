/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitResult.h
 * ժ    Ҫ���ۿ�������XML���ݽṹ�������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitResult :
	public CDVXMLBase
{
public:
	CDVXMLDebitResult(void);
	~CDVXMLDebitResult(void);
//Interface
public:
	//����XML�ַ���
	const BOOL Init(IN const CString& strXMLString);
	//����XML�ַ���
	const CString GetXMLString(void);
//Attributes
public:
	//�������
	int		ResultType;
	//ִ��״̬
	int		ExecuteCode;
	//ִ�н������
	CString ExecuteDesc;


	//����ʶ����
	CString PayIdentifier;
	//������֧��ƽ̨����
	int		PayPlatformType;
	//���װ�ȫ��
	CString TradeSecurityCode;
	//������
	CString DebitOrder;
	//�������ۿ�ʱ��
	CTime	DebitTime;

	//����ʱ�䣬��λ������
	DWORD	ConsumeTime;
};
