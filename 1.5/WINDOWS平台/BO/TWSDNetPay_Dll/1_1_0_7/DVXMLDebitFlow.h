/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitFlow.h
 * ժ    Ҫ�����Ͽ۷�ָ��XML��ʽ���ṹ����,�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��21��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitFlow :
	public CDVXMLBase
{
public:
	CDVXMLDebitFlow(void);
	~CDVXMLDebitFlow(void);
//Interface
public:
	//����XML�ַ���
	const BOOL Init(IN const CString& strXMLString);
	//����XML��ʽ������
	const CString ConstructFormatData();
//Attributes
public:
	//ʡ�ݱ���
	int ProvinceID;
	//��������
	int Type;
	//���ѽ���λ����
	int Money;
	//��������
	int	Weight;
	//����
	int	VehicleType;
	//����
	int	VehicleClass;
	//ʱ��
	time_t CurrentTime;
};

