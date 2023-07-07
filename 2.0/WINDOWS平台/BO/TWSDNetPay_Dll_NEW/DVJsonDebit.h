/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVJsonDebit.h
 * ժ    Ҫ����Ҫ�۷ѷ��������JSON���ݽṹ�壬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��12��
 */
#pragma once
#include "DVJsonBase.h"

class CDVJsonDebit : public CDVJsonBase
{
public:
	CDVJsonDebit(void);
	~CDVJsonDebit(void);
//���������
	CDVJsonDebit& operator=(IN const CDVJsonDebit& cSrc);
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

	/***********************************************����������Ҫ��������********************************************************/
	
	/*********************������Ϣ*********************/

	//�Ƿ��������
	BOOL	TestingData;
	CString	NetPayDllVersion;
	CString DeviceDllVersion;
	
	//��ά��󶨵�֧��ƽ̨ID
	CString IDBindInPayPlatform;
	//��ά��󶨵�֧��ƽ̨����
	int		IDBindInPayPlatformType;
	//����Ψһƾ֤(��������)
	CString RecordMatchCode;
	//���׼�¼��ȫ��֤��
	CString TradeSecurityCode;
	//�ۿ�QR��
	CString	QRCodeData;
	//�ۿ�����(1,���·�ο۷ѣ�4,����·�ο۷�; 2������շѣ�3,��Ʊ)
	int		DebitType;
	//�ۿ���
	int		DebitMoney;
	//�ۿ�ݽ�ʱ��(�������¼�Ĳ���ʱ��һ��)
	CTime	DebitTime;
	//���⿨�ţ�ͨ��ƾ֤��
	CString SMVirtualCardID;
	//֧������
	CString PayVirtualCardID;
	//ͳ����
	int		DebitTollDate;
	//ͳ�ư��
	int		DebitTollShift;
	//�û������豸��Դ(1,ɨ����)
	int		AccountInfoSrcType;
	//�û����ݾ����豸��
	int		AccountInfoDeviceID;
	//�û����ݾ�������
	int		AccountInfoDataType;
	
	/*********************������Ϣ*********************/
	//���ʡ��
	int		EntryProvinceID;
	//���վ��
	int		EntryStationID;
	//���վ��
	CString	EntryStationName;
	//��ڳ�����
	int		EntryLane;
	//��վʱ��
	CTime	EntryTime;
	//�����ϢΨһ��ʶ
	CString EntryInfoUniqueFlag;
	//�������·��
	int		EntryRoadID;
	//��������������
	int		EntryAreaID;

	//�۷ѵص�����ʡ��
	int		PayProvinceID;
	//�۷�վ��
	int		PayStationID;
	//�۷�վ��
	CString PayStationName;
	//�۷ѳ���
	int		PayLane;
	//����վ������·�κ�
	int		PayRoadID;

	//�۷���س�������
	int		VehicleClass;
	//�۷���س�������
	int		VehicleType;
	//�۷���س�������
	CString	VehicleLicense;
	//�۷���س���������ɫ
	int		VehicleColor;
	//�۷ѳ�������
	int		AxisCount;
	//�۷ѳ�������
	int		TotalWeight;
	//�۷ѳ�������
	int		LimitWeight;
	
	/***********************************************��̨������Ҫ��������********************************************************/

	//������ʵ�ʿۿ���
	int		ServerRealDebitMoney;
	//�������ж�֧��ƽ̨����(1,΢�ţ�2,֧������
	int		ServerJudgeDebitIDType;
	//�ۿ�״̬����
	int		DebitStatusCode;
	//�ۿ�״̬����
	CString DebitStatusDesc;
	//�ۿ����(�������ۿ�ɹ����и�������)
	CString	DebitOrderNumber;
};
