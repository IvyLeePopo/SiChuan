/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�PayKeyItems.h
 * ժ    Ҫ�����׹ؼ�����
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��4��27��
 */
#pragma once

class CPayKeyItems
{
public:
	CPayKeyItems(void);
	~CPayKeyItems(void);
//Attribute
public:
	//�Ƿ���Ч
	BOOL	Valid;
	//��ʱ�ȴ�ʱ��
	int		OverTime;
	//�ۿ�ģʽ
	int		OperationMode;

	/*****************Debit��ϸ��************************/
	//����ʶ����
	CString	PayIdentifier;
	//���׽��(��λ����)
	int		Money;
	//����ʱ��
	CTime	SubTime;
	//����������
	int		DebitMainType;
	//������֧��ƾ֤
	CString PayCertificateCode;
	//֧��ƾ֤����
	int		PayCertificateType;

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
	int		TotalWeight;
	//����
	int		LimitWeight;
	//add 2018-03
	CString AutoLicense;
	int AutoColor;

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
	//add 2018-03
	int Distance;

	/*****************Entry��ϸ��************************/
	//���վ��
	CString	EntryStationID;
	//���վ��
	CString EntryStationName;
	//���ʱ��
	CTime	EntryTime;
	//��ڳ���
	CString	EntryLane;
	//��ڳ���
	CString	EntryVehicleLicense;
	//��ڳ�����ɫ
	int		EntryVLColor;
	//��ڳ���
	int		EntryVehicleType;
	//��ڳ���
	int		EntryVehicleClass;
	//����������
	CString		EntryAreaID;
	//���·�κ�
	CString		EntryRoadID;

	/*****************NFC��ϸ��************************/
	//NFC���ѱ���
	CString NFCPayData;
	//NFC��������
	CString NFCRevData;

	//add 2018-04
	CString imageId;//����ʶ��ϵͳ���ɵ�ͼ��ID
	CString taskId;

};
