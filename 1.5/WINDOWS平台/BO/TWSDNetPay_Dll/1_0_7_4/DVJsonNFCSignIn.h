/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonNFCSignIn.h
 * ժ    Ҫ��NFC ģ��ǩ��
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��20��
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonNFCSignIn : public CDVJsonBase
{
public:
	CDVJsonNFCSignIn(void);
	~CDVJsonNFCSignIn(void);
	//���������
	CDVJsonNFCSignIn& operator=(IN const CDVJsonNFCSignIn& cSrc);
//Interface
public:
	//��������
	void  Reset(IN const int& iType=0){}
	//��������У����
	CString CreateInnerVerifyCode(void){return _T("");}
	//����ȫ�ֶ����ݼ��ܽ��
	const CString CreateEnDataString(void) const {return _T("");}
	//�ڲ�������У��
	BOOL  InnerVerify(void){return TRUE;}
//����
public:
	/*********************������Ϣ*********************/

	//�Ƿ��������
	BOOL	TestingData;
	CString	NetPayDllVersion;
	CString DeviceDllVersion;
	CString TWProductSerialNo;//��΢Ӳ���ն����

	//NFCģ���ն����
	CString NFCTerminal;
	//NFCģ���ն����
	CString NFCSerialNo;
	//NFCǩ������
	CString NFCSignData;
};
