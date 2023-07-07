/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonHerat.h
 * ժ    Ҫ�������ӿ����ݽ��������ṹ�壬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��12��14��
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonHeart : public CDVJsonBase
{
public:
	CDVJsonHeart(void);
	~CDVJsonHeart(void);
//���������
	CDVJsonHeart& operator=(IN const CDVJsonHeart& cSrc);
public:
	//��������
	void  Reset(IN const int& iType=0);
	//��������У����
	DWORD CreateInnerVerifyCode(void){return 0;}
	//�ڲ�������У��
	BOOL  InnerVerify(void){return TRUE;}
//����
public:
	//վ�����
	int		StationID;
	//վ������
	CString	StationName;
	//��������
	int		LaneID;
	//����ʱ��
	CTime	HeartTime;
};
