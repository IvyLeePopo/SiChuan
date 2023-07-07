/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLBase.h
 * ժ    Ҫ������XML���ݸ�ʽ�࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��17��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLUnipay :
	public CDVXMLBase
{
public:
	CDVXMLUnipay(void);
	~CDVXMLUnipay(void);
//Interface
public:
	//����XML�ַ���
	const BOOL Init(IN const CString& strXMLString);
	//����NFC��صĸ�ʽ������
	const CString ConstructNFCFrameData(IN const CString& strData,IN const int& iType);
//Attributes
public:
	//ģ��ʶ����
	int Module;
	//��������
	int Type;
	//�������ݣ�16���ƣ�
	CString Context;
};
