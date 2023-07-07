/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLAccountContext.h
 * ժ    Ҫ���û�������ϢXML�ṹ�壬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLAccountContext :
	public CDVXMLBase
{
public:
	CDVXMLAccountContext(void);
	~CDVXMLAccountContext(void);
//Interface
public:
	//����XML�ַ���
	const BOOL	  Init(IN const CString& strXMLString);
	//�ṩXML�ַ���
	const CString GetXMLString(IN const CString& strData,IN const int& iDataType,IN const int& iSource);
//Attributes
public:
	//��������
	int		Type;
	//������Դ
	int		Src;
	//��������
	CString Context;
};
