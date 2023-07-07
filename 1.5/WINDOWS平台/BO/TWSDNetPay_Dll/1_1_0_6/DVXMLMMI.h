/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLMMI.h
 * ժ    Ҫ����������XML���ݽṹ�������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLMMI :
	public CDVXMLBase
{
public:
	CDVXMLMMI(void);
	~CDVXMLMMI(void);
//Interface
public:
	//����XML�ַ���
	const BOOL	  Init(IN const CString& strXMLString);
//Attributes
public:
	//�����������⴦����
	int ClearMMI;
	//������ʾ����
	CArray<tagMMIItemText,tagMMIItemText&>		Text;
	//��������
	CArray<tagMMIItemVoice,tagMMIItemVoice&>	Voice;
	//ͼ������
	CArray<tagMMIItemGraph,tagMMIItemGraph&>	Graph;

	//ý�����ݣ����ڹ�沥�ţ�
	CArray<tagMMIMedia,tagMMIMedia&>			Media;
};
