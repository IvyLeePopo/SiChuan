/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitParamContext.h
 * ժ    Ҫ���ۿ���ϸ����XML�ṹ�����������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#pragma once
#include "dvxmlbase.h"

class CDVXMLDebitParamContext :
	public CDVXMLBase
{
public:
	CDVXMLDebitParamContext(void);
	~CDVXMLDebitParamContext(void);
//Interface
public:
	//����XML�ַ���
	const BOOL Init(IN const CString& strXMLString);
//Attributes
public:
	//��ʱ�ȴ�ʱ��
	int		OverTime;
	//�ۿ�ģʽ
	int		OperationMode;
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
	//����������
	CString		EntryAreaID;
	//���·�α���
	CString		EntryRoadID;
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
};
