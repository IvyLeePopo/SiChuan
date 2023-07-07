/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�NFCProtocol.h
 * ժ    Ҫ��������Ŀ�ǽ�ģ��Э������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��21��
 */
#pragma once
#include "BaseDefine.h"

class CNFCProtocol
{
public:
	CNFCProtocol(void);
	~CNFCProtocol(void);
//Operations
public:
	//����NFCģ�����������,���ؽ����ö������ UNIPAY_FRAME_TYPE
	const int Analyze(IN const CString& strRevData);
	//����ִֹͣ��ָ��
	const CString ConstructStopExecuteCmd(void);
	//���췢��ǩ��ָ��
	const CString ConstructStartSignInCmd(void);
	//����HTTP������ǩ��ָ��
	const CString ConstructSignDataFromHttpCmd(IN const CString& strContext);
	//���췢������ָ��
	const CString ConstructStartConsumeCmd(IN const int& iMoney);
	//����ִ�о�����������ָ��
	const CString ConstructExecuteConsumeCmd(IN const int& iConsumeType);
	//�������ָ��
	const CString ConstructCorrectCmd(void);
	//�������Ӳ���ָ��
	const CString ConstructTestLinkCmd(void);

	//add 2018-04
	const CString ConstructParaQueryCmd(void);
	const CString ConstructSoftVerQueryCmd(void);
//Attribute
public:
	//����֡���ͣ���ö������ UNIPAY_FRAME_TYPE��
	int FramrType;
	//�Ƿ��Ѿ�����
	BOOL IsAyalyze;
	
	//����ͷ
	BYTE Head;
	//����β
	BYTE End;
	//����
	WORD Length;
	//·��
	BYTE Path;
	//У����
	BYTE LrcVerify;
	//���ױ�ʶID
	CString TradeID;
	//������
	CString Context;
	BYTE	BinData[5120];

	//��ǰ���ѵ��̻���
	BYTE	MerchantBin[15];
	CString MerchantHex;
	//��ǰ���ѵ��ն˺�
	BYTE	TerminalBin[8];
	CString TerminalHex;
	//��ǰ���Ѵ�����루����У�
	BYTE	CurrentConsumeErrorCode;
};
