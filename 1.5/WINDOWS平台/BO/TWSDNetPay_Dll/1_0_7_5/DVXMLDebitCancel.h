/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitCancel.h
 * ժ    Ҫ������XML���ݽṹ�壬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitCancel :
	public CDVXMLBase
{
public:
	CDVXMLDebitCancel(void);
	~CDVXMLDebitCancel(void);
//Interface
public:
//����XML�ַ���
	const BOOL Init(IN const CString& strXMLString);
//Attributes
public:
	//����ʶ����
	CString PayIdentifier;
};
