/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonParamDebit.h
 * ժ    Ҫ���ۿ�ӿڲ���JSON���ݸ�ʽ�����������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��15��
 */
#pragma once
#include "dvjsonbase.h"

class CDVJsonParamDebitMoney :
	public CDVJsonBase
{
public:
	CDVJsonParamDebitMoney(void);
	~CDVJsonParamDebitMoney(void);
//���������
	CDVJsonParamDebitMoney& operator=(IN const CDVJsonParamDebitMoney& cSrc);
//Interface
public:
	//��������
	void  Reset(IN const int& iType=0);
	//��������У����
	DWORD CreateInnerVerifyCode(void){return 0;}
	//�ڲ�������У��
	BOOL  InnerVerify(void){return TRUE;}
//����
public:
	//��ʱ�ȴ�ʱ��
	int		OverTime;
	//�ۿ�ģʽ
	int		OperationMode;
	int     PayModel;
	/*****************Debit��ϸ��************************/
	//����ʶ����
	CString	PayIdentifier;
	//������֧��ƾ֤����
	CString PayCertificateCode;
	//������֧��ƾ֤���
	int		PayCertificateType;
	//���׽��(��λ����)
	int		Money;
	//����ʱ��
	CTime	SubTime;
	//����������
	int		DebitMainType;

	/*****************Vehicle��ϸ��************************/
	//����
	int		VehicleType;
	//����
	int		VehicleClass;
	//������ɫ
	int		VLColor;
	//����
	CString	VehicleLicense;
	//����
	int		AxisCount;
	//����
	int		Weight;
	//����
	int		LimitWeight;
	//add 2018-05
	CString AutoPlate;
	int AutoPlateColor;

	/*****************Operation��ϸ��************************/
	//ͨ��ƾ֤����
	int		PassCertificateType;
	//ͨ��ƾ֤����
	CString CardID;
	//ͳ����
	int		TollDate;
	//ͳ�ư��
	int		ShiftID;
	//��ӡƱ�ݺ�
	CString TicketNo;
	//�շ�Ա����
	CString OperatorID;
	//�շ�Ա����
	CString OperatorName;

	/*****************Entry��ϸ��************************/
	//����������
	int		EntryAreaID;
	CString str_EntryAreaID;
	//���·�α���
	int		EntryRoadID;
	CString str_EntryRoadID;
	//���վ��
	int		EntryStationID;
	CString str_EntryStationID;
	//���վ��
	CString EntryStationName;
	//���ʱ��
	CTime	EntryTime;
	//��ڳ���
	int		EntryLane;
	CString str_EntryLane;
	//��ڳ���
	CString	EntryVehicleLicense;
	//��ڳ�����ɫ
	int		EntryVLColor;
	//��ڳ���
	int		EntryVehicleType;
	//��ڳ���
	int		EntryVehicleClass;
};
