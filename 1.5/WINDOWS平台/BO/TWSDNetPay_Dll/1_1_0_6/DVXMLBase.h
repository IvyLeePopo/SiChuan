/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLBase.h
 * ժ    Ҫ������XML���ݸ�ʽ�Ĺ��û��࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��3��12��
 */
#pragma once
#include "BaseDefine.h"

class CDVXMLBase
{
public:
	CDVXMLBase(void);
	~CDVXMLBase(void);
//Interface
public:
	//��ȡ������Ϣ
	const CString GetLastErrorDesc(){return m_strLastErrorDesc;}
	//����XML�ַ���
	virtual const BOOL	  Init(IN const CString& strXMLString)=0;
//Attributes
public:
	//����XML�ļ����ļ�ͷ
	CString	XMLHead;
	//�̳�����XML�ļ��ṹ�����壬�����ֵ����ݷ�������
	CString XMLFunctionDesc;
	//XMl�ļ�Э��汾
	CString XMLVersion;
	//У����
	CString XMLVerifyCode;
	//��������
	CString m_strLastErrorDesc;
};
