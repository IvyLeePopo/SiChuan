/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVDebitCancel.h
 * ժ    Ҫ���������ݱ��ر������������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��25��
 */
#pragma once

class CDVDebitCancel
{
public:
	CDVDebitCancel(void);
	~CDVDebitCancel(void);
//Attributes
public:
	//ʡ�ݱ�ʶ
	int		ProvinceID;
	//·�α�ʶ
	int		RoadID;
	//վ�����
	int		StationID;
	//�������ݵĴ����ʶ(0,δ����;1,�Ѵ���)
	int		ProcessMark;
	//��������ʶ����
	CString PayIdentifier;
	//�ݽ�ʱ��
	CString	SubTime;
	//��¼MD5ժҪ
	CString VerifyCode;
	//��������ʱ��
	CTime	CreateTime;
	//����ʵ��ִ��ʱ��
	CTime	ExecuteTime;
};
