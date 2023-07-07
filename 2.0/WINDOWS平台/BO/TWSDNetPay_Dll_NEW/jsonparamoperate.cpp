/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
* 
* �ļ����ƣ�JsonParamOperate.cpp
* ժ    Ҫ���ӿڲ���JSON��ʽ�����࣬ʵ���ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��8��15��
*/
#include "StdAfx.h"
#include "JsonParamOperate.h"
#include "TWSDNetPay_Dll.h"

CJsonParamOperate::CJsonParamOperate(void)
{
}

CJsonParamOperate::~CJsonParamOperate(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonParamOperate::FormatParamData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- �ۿ������ʽת��
//     
// ���������IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult
// ���������CString
// ��д��Ա��FFFF
// ��дʱ�䣺2017��3��8��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonParamOperate::FormatParamData2Json(IN const CDVJsonParamDebitMoney* pDVJsonParamDebit)
{
	CString strReturn;
	try{
		if(NULL == pDVJsonParamDebit)
			throw -1;
		//��ʾ����json����
		Json::Value 	root;

		Json::Value		DebitParamContext;

		Json::Value		Data;
		Json::Value		Version;

		Json::Value		Debit;
		Json::Value		Entry;
		Json::Value		Operation;
		Json::Value		OperationMode;
		Json::Value		OverTime;
		Json::Value		Vehicle;

		/***********************************************************����JSAO���ݽṹ***********************************************************/	
		Debit["Money"] =					Json::Value(pDVJsonParamDebit->Money);
		Debit["PayCertificateCode"] =		Json::Value(pDVJsonParamDebit->PayCertificateCode) ;
		Debit["PayCertificateType"] =		Json::Value(pDVJsonParamDebit->PayCertificateType) ;
		Debit["PayIdentifier"] =			Json::Value(pDVJsonParamDebit->PayIdentifier) ;
		Debit["SubTime"] =					Json::Value(pDVJsonParamDebit->SubTime.Format(_T("%Y-%m-%d %H:%M:%S"))) ;//Format(_T("%Y-%m-%d %H:%M:%S"))
		Debit["Type"] =						1;

		Entry["AreaID"] =					Json::Value(pDVJsonParamDebit->EntryAreaID) ;
		Entry["LaneID"] =					Json::Value(pDVJsonParamDebit->EntryLane) ;
		Entry["License"] =					Json::Value(pDVJsonParamDebit->EntryVehicleLicense) ;
		Entry["RoadID"] =					Json::Value(pDVJsonParamDebit->EntryRoadID) ;
		Entry["StationID"] =				Json::Value(pDVJsonParamDebit->EntryStationID) ;
		Entry["StationName"] =				Json::Value(pDVJsonParamDebit->EntryStationName) ;
		Entry["Time"] =						Json::Value(pDVJsonParamDebit->EntryTime.Format(_T("%Y-%m-%d %H:%M:%S"))) ;
		Entry["VClass"] =					Json::Value(pDVJsonParamDebit->EntryVehicleClass) ;
		Entry["VColor"] =					Json::Value(pDVJsonParamDebit->EntryVLColor );
		Entry["VType"] =					Json::Value(pDVJsonParamDebit->EntryVehicleType) ;

		Operation["CardID"] =				Json::Value(pDVJsonParamDebit->CardID) ;
		Operation["PassCertificateType"] =  Json::Value(pDVJsonParamDebit->PassCertificateType) ;
		Operation["ShiftID"] =				Json::Value(pDVJsonParamDebit->ShiftID );
		Operation["TollDate"] =				Json::Value(pDVJsonParamDebit->TollDate );

		Vehicle["AxisCount"] =				Json::Value(pDVJsonParamDebit->AxisCount) ;
		Vehicle["Class"] =					Json::Value(pDVJsonParamDebit->VehicleClass) ;
		Vehicle["License"] =				Json::Value(pDVJsonParamDebit->VehicleLicense );
		Vehicle["Type"] =					Json::Value(pDVJsonParamDebit->VehicleType );
		Vehicle["VLColor"] =				Json::Value(pDVJsonParamDebit->VLColor );
		Vehicle["Weight"] =					Json::Value(pDVJsonParamDebit->Weight) ;

		Data["Debit"] = Debit;
		Data["Entry"] = Entry;
		Data["Operation"] = Operation;
		Data["OperationMode"] =				Json::Value(pDVJsonParamDebit->OperationMode);
		Data["OverTime"] =					Json::Value(pDVJsonParamDebit->OverTime);
		Data["Vehicle"] = Vehicle;

		DebitParamContext["Version"] =		Json::Value(pDVJsonParamDebit->Version);
		DebitParamContext["Data"] = Data;
		root["DebitParamContext"] = DebitParamContext;

		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());

	}
	catch(int& nError)
	{
		switch(nError)
		{
		case -1:
			break;
		}

	}

	return strReturn;
}




////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonParamOperate::FormatParamData2Json
//     
// ���������������յ������ݽṹת��Ϊ��׼��JSON��ʽ -- ֧������������ݱ任(�۷ѽ��)
//     
// ���������IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonParamOperate::FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult)
{
	CString strReturn;
	try
	{
		if(NULL == pDVJsonParamDebitResult)
			throw -1;
		// ��ʾ���� json ����
		Json::Value root;
		// ��ʶ�û�����
		Json::Value ResultContext;
		// ��ʶ��ϸ������
		Json::Value Data;
		// ��ʶ�ؼ���
		Json::Value KeyItem;
		/***********************************************************����JSAO���ݽṹ***********************************************************/

		//��ʶ�汾
		ResultContext["Version"]		= Json::Value(pDVJsonParamDebitResult->Version);
		//��ʶ�ۿ�����
		Data["ResultType"]				= Json::Value(pDVJsonParamDebitResult->ResultType);
		//��ʶִ��״̬
		Data["ExecuteCode"]				= Json::Value(pDVJsonParamDebitResult->ExecuteCode);
		//��ʶִ�н������
		Data["ExecuteDesc"]				= Json::Value(pDVJsonParamDebitResult->ExecuteDesc);

		//�ؼ������� -- ���װ�ȫ��
		KeyItem["TradeSecurityCode"]	= Json::Value(pDVJsonParamDebitResult->TradeSecurityCode);
		//�ؼ������� -- ������֧��ƽ̨����
		KeyItem["PayPlatformType"]		= Json::Value(pDVJsonParamDebitResult->PayPlatformType);
		//�ؼ������� -- ����ʶ����
		KeyItem["PayIdentifier"]		= Json::Value(pDVJsonParamDebitResult->PayIdentifier);
		//�ؼ������� -- ������
		KeyItem["DebitOrder"]			= Json::Value(pDVJsonParamDebitResult->DebitOrder);
		//�ؼ������� -- ����˿ۿ�ʱ��
		KeyItem["DebitTime"]			= Json::Value(pDVJsonParamDebitResult->DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		//�ؼ������� -- ����ʱ��
		//remove zhuyabing 2017-06 KeyItem["ConsumeTime"]			= Json::Value(pDVJsonParamDebitResult->ConsumeTime);

		Data["TradeKeyItem"]		= KeyItem;
		ResultContext["Data"]		= Data;
		root["DebitResultContext"]	= ResultContext;
		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonParamOperate::FormatParamData2Json
//     
// ���������������յ������ݸ�ʽת��Ϊ��׼��JSON��ʽ -- �û����ݲ������ݱ任
//     
// ���������IN const CDVJsonParamAccount* pDVJsonParamAccount
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonParamOperate::FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount)
{
	CString strReturn;
	try
	{
		if(NULL == pDVJsonParamAccount)
			throw -1;
		// ��ʾ���� json ����
		Json::Value root;
		// ��ʶ�û�����
		Json::Value ParamAccount;
		// ��ʶ��ϸ������
		Json::Value Data;
		/***********************************************************����JSON���ݽṹ***********************************************************/

		//��ʶ�汾
		ParamAccount["Version"]	= Json::Value(pDVJsonParamAccount->Version);
		//��ʶ��������
		Data["Type"]			= Json::Value(pDVJsonParamAccount->DataType);
		//��ʶ������Դ
		Data["Source"]			= Json::Value(pDVJsonParamAccount->DataSrc);
		//��ʶ��������
		Data["Context"]			= Json::Value(pDVJsonParamAccount->DataContext);

		ParamAccount["Data"]	= Data;
		root["AccountContext"]	= ParamAccount;
		//ת��Ϊ�ַ���
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}

	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CJsonParamOperate::FormatResponseResult2Class
//     
// ������������JSON��ʽ�ַ���ת��Ϊ��׼���ݽṹ��
//     
// ���������IN const CString& strResponseResult
//         ��IN const int& iDataType
// ���������CDVJsonBase*
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonParamOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonBase* pDVJsonBase = NULL;
	try
	{
		switch(iDataType)
		{
		case JSON_DT_PARAM_ACCOUNT:
			{
				pDVJsonBase = new CDVJsonParamAccount();
				if(NULL == pDVJsonBase)
				{
					throw -1;
				}
				//�������ݸ�ʽת��
				Json::Reader reader;
				Json::Value  root;
				Json::Value  AccountContext;
				Json::Value  Data;

				CString strResult(strResponseResult);
				if(!reader.parse(strResult.GetBuffer(0),root))
				{
					strResult.ReleaseBuffer();
					throw -2;
				}

				if(Json::nullValue == root.type())
					throw -3;

				//��ȡ�û���Ϣ��
				if(Json::nullValue == root["AccountContext"].type())
					throw -4;
				AccountContext = root["AccountContext"];

				//��ȡ�汾
				if(Json::stringValue != AccountContext["Version"].type())
					throw -4;
				pDVJsonBase->Version = AccountContext["Version"].asCString();

				//��ȡ������Ϣ��
				if(Json::nullValue == AccountContext["Data"].type())
					throw -4;
				Data = AccountContext["Data"];

				//��ȡ�û���������
				if(Json::intValue != Data["Type"].type())
					throw -4;
				((CDVJsonParamAccount*)pDVJsonBase)->DataType = Data["Type"].asInt();

				//��ȡ�û�������Դ
				if(Json::intValue != Data["Source"].type())
					throw -4;
				((CDVJsonParamAccount*)pDVJsonBase)->DataSrc = Data["Source"].asInt();

				//��ȡ�û���������
				if(Json::stringValue != Data["Context"].type())
					throw -4;
				((CDVJsonParamAccount*)pDVJsonBase)->DataContext = Data["Context"].asCString();

			}
			break;
		case JSON_DT_PARAM_DEBIT_MONEY:
			{
				pDVJsonBase = new CDVJsonParamDebitMoney();
				if(NULL == pDVJsonBase)
				{
					throw -1;
				}
				//�������ݸ�ʽת��
				Json::Reader reader;
				Json::Value  root;
				Json::Value  DebitParamContext;
				Json::Value  Data;
				Json::Value  Debit;
				Json::Value  Vehicle;
				Json::Value  Operation;
				Json::Value	 Entry;

				CString strResult(strResponseResult);
				if(!reader.parse(strResult.GetBuffer(0),root))
				{
					strResult.ReleaseBuffer();
					throw -2;
				}

				if(Json::nullValue == root.type())
					throw -3;

				//��ȡ����������
				if(Json::nullValue == root["DebitParamContext"].type())
					throw -4;
				DebitParamContext = root["DebitParamContext"];

				//��ȡ������ϸ��
				if(Json::nullValue == DebitParamContext["Data"].type())
					throw -4;
				Data = DebitParamContext["Data"];

				//��ȡ��ʱʱ��
				if(Json::intValue != Data["OverTime"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->OverTime = Data["OverTime"].asInt();

				//��ȡ�ۿ����ģʽ
				if(Json::nullValue == Data["OperationMode"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperationMode = 1;//Ĭ�����Ͽۿ�
				}
				else
				{
					if(Json::intValue != Data["OperationMode"].type())
						throw -4;
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperationMode = Data["OperationMode"].asInt();
				}
				if(Data["PayModel"].type() == Json::intValue)
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->PayModel = Data["PayModel"].asInt(); 
				}
				else
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->PayModel=0;
				}

				//��ȡ�ۿ���Ϣ��
				if(Json::nullValue == Data["Debit"].type())
					throw -4;
				Debit = Data["Debit"];

				//��ȡ�ۿ���
				if(Json::intValue != Debit["Money"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->Money = Debit["Money"].asInt();

				//��ȡ����ʶ����
				if(Json::stringValue != Debit["PayIdentifier"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->PayIdentifier = Debit["PayIdentifier"].asCString();

				//ֻ�з����ϲ�������Ҫƾ֤����
				if(2 == ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperationMode)
				{
					//��ȡ֧��ƾ֤����
					if(Json::intValue != Debit["PayCertificateType"].type())
						throw -4;
					((CDVJsonParamDebitMoney*)pDVJsonBase)->PayCertificateType = Debit["PayCertificateType"].asInt();

					//��ȡ֧��ƾ֤����
					if(Json::stringValue != Debit["PayCertificateCode"].type())
						throw -4;
					((CDVJsonParamDebitMoney*)pDVJsonBase)->PayCertificateCode = Debit["PayCertificateCode"].asCString();
				}

				//��ȡ�ۿ�������
				if(Json::intValue != Debit["Type"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->DebitMainType = Debit["Type"].asInt();

				//��ȡ����ʱ��
				if(Json::stringValue != Debit["SubTime"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->SubTime = CTime(GetGlobalApp()->CString2time_t(Debit["SubTime"].asCString()));


				/***********************************��ȡ������Ϣ��***********************************/
				//��ȡ������Ϣ��
				if(Json::nullValue == Data["Vehicle"].type())
					throw -4;
				Vehicle = Data["Vehicle"];

				//��ȡ����
				if(Json::intValue != Vehicle["Type"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleType = Vehicle["Type"].asInt();

				//��ȡ����
				if(Json::intValue != Vehicle["Class"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleClass = Vehicle["Class"].asInt();

				//��ȡ����
				if(Json::stringValue != Vehicle["License"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleLicense = Vehicle["License"].asCString();

				//��ȡ������ɫ
				if(Json::intValue != Vehicle["VLColor"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VLColor = Vehicle["VLColor"].asInt();

				//��ȡ����
				if(Json::intValue != Vehicle["AxisCount"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->AxisCount = Vehicle["AxisCount"].asInt();

				//��ȡװ������
				if(Json::intValue != Vehicle["Weight"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->Weight = Vehicle["Weight"].asInt();

				//��ȡ����
				if(Json::intValue == Vehicle["LimitWeight"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->LimitWeight = Vehicle["LimitWeight"].asInt();
				}
				Json::Value autoPlateN = Vehicle["AutoPlate"];
				if(autoPlateN!=Json::nullValue)
				{
					if(Json::stringValue == autoPlateN["AutoLicense"].type())
					{
						((CDVJsonParamDebitMoney*)pDVJsonBase)->AutoPlate = autoPlateN["AutoLicense"].asCString();
					}
					if(Json::intValue == autoPlateN["AutoColor"].type())
					{
						((CDVJsonParamDebitMoney*)pDVJsonBase)->AutoPlateColor = autoPlateN["AutoColor"].asInt();
					}
				}



				/***********************************��ȡ������Ϣ��***********************************/
				//��ȡ������Ϣ��
				if(Json::nullValue == Data["Operation"].type())
					throw -4;
				Operation = Data["Operation"];

				//��ȡͨ��ƾ֤����
				if(Json::intValue != Operation["PassCertificateType"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->PassCertificateType = Operation["PassCertificateType"].asInt();

				//��ȡͨ��ƾ֤����
				if(Json::stringValue != Operation["CardID"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->CardID = Operation["CardID"].asCString();

				//��ȡͳ����
				if(Json::intValue == Operation["TollDate"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->TollDate = Operation["TollDate"].asInt();
				}
				if(0 == ((CDVJsonParamDebitMoney*)pDVJsonBase)->TollDate)
				{
					if(Json::stringValue != Operation["TollDate"].type())
						throw -4;
					((CDVJsonParamDebitMoney*)pDVJsonBase)->TollDate = _ttoi(Operation["TollDate"].asCString());
				}

				//��ȡͳ�ư��
				if(Json::intValue != Operation["ShiftID"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->ShiftID = Operation["ShiftID"].asInt();

				//��ȡ��ӡƱ�ݺ�
				if(Json::stringValue == Operation["TicketNo"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->TicketNo = Operation["TicketNo"].asCString();
				}

				//��ȡ�շ�Ա����
				if(Json::stringValue == Operation["OperatorID"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorID = Operation["OperatorID"].asCString();
				}
				//��ȡ�շ�Ա����
				if(Json::stringValue == Operation["OperatorName"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorName = Operation["OperatorName"].asCString();
				}


				/***********************************��ȡ�����Ϣ��***********************************/
				//��ȡ�����Ϣ��
				if(Json::nullValue == Data["Entry"].type())
					throw -4;
				Entry = Data["Entry"];

				//��ȡ����������
				if(Json::intValue != Entry["AreaID"].type())
					throw -4;

				if(Json::intValue != Entry["AreaID"].type())
				{
					std::string strtmp = Entry["AreaID"].asString();
					if(!strtmp.empty())
					{
						((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryAreaID.Format(_T("%s"),strtmp.c_str());
					}
				}
				else
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryAreaID = Entry["AreaID"].asInt();
					((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryAreaID.Format(_T("%d"), Entry["AreaID"].asInt());
				}
				

				//��ȡ���·�α���
				if(Json::intValue != Entry["RoadID"].type())
				{
					std::string strtmp = Entry["RoadID"].asString();
					if(!strtmp.empty())
					{
						((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryRoadID.Format(_T("%s"),strtmp.c_str());
					}
				}
				else
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryRoadID = Entry["RoadID"].asInt();
					((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryRoadID.Format(_T("%d"), Entry["RoadID"].asInt());
				}
				

				//��ȡ���վ�����
				if(Json::intValue != Entry["StationID"].type())
				{
					std::string strtmp = Entry["StationID"].asString();
					if(!strtmp.empty())
					{
						((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryStationID.Format(_T("%s"),strtmp.c_str());
					}
				}
				else
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryStationID = Entry["StationID"].asInt();
					((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryStationID.Format(_T("%d"), Entry["StationID"].asInt());

				}
				
				
				
				//��ȡ���վ������
				if(Json::stringValue != Entry["StationName"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryStationName = Entry["StationName"].asCString();

				//��ȡ��ڳ�������
				if(Json::intValue != Entry["LaneID"].type())
				{
					std::string strtmp = Entry["LaneID"].asString();
					if(!strtmp.empty())
					{
						((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryLane.Format(_T("%s"),strtmp.c_str());
					}
				}
				else
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryLane = Entry["LaneID"].asInt();
					((CDVJsonParamDebitMoney*)pDVJsonBase)->str_EntryLane.Format(_T("%d"), Entry["LaneID"].asInt());

				}

				//��ȡ��ڳ���
				if(Json::intValue != Entry["VType"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleType = Entry["VType"].asInt();

				//��ȡ��ڳ���
				if(Json::intValue != Entry["VClass"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleClass = Entry["VClass"].asInt();

				//��ȡ��ڳ���
				if(Json::stringValue != Entry["License"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleLicense = Entry["License"].asCString();

				//��ȡ��ڳ�����ɫ
				if(Json::intValue != Entry["VColor"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVLColor = Entry["VColor"].asInt();

				//��ȡ���ʱ��
				if(Json::stringValue != Entry["Time"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryTime = CTime(GetGlobalApp()->CString2time_t(Entry["Time"].asCString()));
			}
			break;
		case JSON_DT_PARAM_DEBIT_CANCLE:
			{
				pDVJsonBase = new CDVJsonParamDebitCancel();
				if(NULL == pDVJsonBase)
				{
					throw -1;
				}
				//�������ݸ�ʽת��
				Json::Reader reader;
				Json::Value  root;
				Json::Value  DebitCancelParamContext;
				Json::Value  Data;
				Json::Value  TradeKeyItem;

				CString strResult(strResponseResult);
				if(!reader.parse(strResult.GetBuffer(0),root))
				{
					strResult.ReleaseBuffer();
					throw -2;
				}

				if(Json::nullValue == root.type())
					throw -3;

				//��ȡ�û���Ϣ��
				if(Json::nullValue == root["DebitCancelParamContext"].type())
					throw -4;
				DebitCancelParamContext = root["DebitCancelParamContext"];

				//��ȡ�汾
				if(Json::stringValue != DebitCancelParamContext["Version"].type())
					throw -4;
				((CDVJsonParamDebitCancel*)pDVJsonBase)->Version = DebitCancelParamContext["Version"].asCString();

				//��ȡ������Ϣ��
				if(Json::nullValue == DebitCancelParamContext["Data"].type())
					throw -4;
				Data = DebitCancelParamContext["Data"];

				//��ȡ����������Ϣ��
				if(Json::nullValue == Data["TradeKeyItem"].type())
					throw -4;
				TradeKeyItem = Data["TradeKeyItem"];

				//��ȡ����ʶ����
				if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
					throw -4;
				((CDVJsonParamDebitCancel*)pDVJsonBase)->PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();
			}
			break;
		case JSON_DT_PARAM_DEBIT_RESULT:
			{
				pDVJsonBase = new CDVJsonParamDebitResult();
				if(NULL == pDVJsonBase)
				{
					throw -1;
				}
				//�������ݸ�ʽת��
				Json::Reader reader;
				Json::Value  root;
				Json::Value  DebitResultContext;
				Json::Value  Data;
				Json::Value  TradeKeyItem;

				CString strResult(strResponseResult);
				if(!reader.parse(strResult.GetBuffer(0),root))
				{
					strResult.ReleaseBuffer();
					throw -2;
				}

				if(Json::nullValue == root.type())
					throw -3;

				//��ȡ�û���Ϣ��
				if(Json::nullValue == root["DebitResultContext"].type())
					throw -4;
				DebitResultContext = root["DebitResultContext"];

				//��ȡ�汾
				if(Json::stringValue != DebitResultContext["Version"].type())
					throw -4;
				pDVJsonBase->Version = DebitResultContext["Version"].asCString();

				//��ȡ������Ϣ��
				if(Json::nullValue == DebitResultContext["Data"].type())
					throw -4;
				Data = DebitResultContext["Data"];

				//��ȡ�������
				if(Json::intValue != Data["ResultType"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ResultType = Data["ResultType"].asInt();

				//��ȡִ��״̬
				if(Json::intValue != Data["ExecuteCode"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ExecuteCode = Data["ExecuteCode"].asInt();

				//��ȡִ�н������
				if(Json::nullValue == Data["ExecuteDesc"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ExecuteDesc = Data["ExecuteDesc"].asCString();

				//��ȡ������Ϣ��
				if(Json::nullValue == Data["TradeKeyItem"].type())
					throw -4;
				TradeKeyItem = Data["TradeKeyItem"];


				//��ȡ����ʶ����
				if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();

				//��ȡ������֧��ƽ̨����
				if(Json::intValue != TradeKeyItem["PayPlatformType"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->PayPlatformType = TradeKeyItem["PayPlatformType"].asInt();

				//��ȡ���װ�ȫ��
				if(Json::stringValue != TradeKeyItem["TradeSecurityCode"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->TradeSecurityCode = TradeKeyItem["TradeSecurityCode"].asCString();

				//��ȡ��������
				if(Json::stringValue != TradeKeyItem["DebitOrder"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->DebitOrder = TradeKeyItem["DebitOrder"].asCString();

				//��ȡ�������ۿ�ʱ��
				if(Json::stringValue != TradeKeyItem["DebitTime"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->DebitTime = ::GetGlobalApp()->CString2time_t(TradeKeyItem["DebitTime"].asCString());

				//��ȡ����ʱ��
				if(Json::intValue != TradeKeyItem["ConsumeTime"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ConsumeTime = TradeKeyItem["ConsumeTime"].asInt();
			}
			break;
		case JSON_DT_PARAM_MMI:
			{
				pDVJsonBase = new CDVJsonParamMMI();
				if(NULL == pDVJsonBase)
				{
					throw -1;
				}
				//�������ݸ�ʽת��
				Json::Reader reader;
				Json::Value  root;
				Json::Value  MMIParamContext;
				Json::Value  Data;
				Json::Value  ItemData;
				Json::Value  aryTextView;
				Json::Value  aryVoiceView;
				Json::Value  aryGraphView;

				CString strResult(strResponseResult);
				if(!reader.parse(strResult.GetBuffer(0),root))
				{
					strResult.ReleaseBuffer();
					throw -2;
				}

				if(Json::nullValue == root.type())
					throw -3;

				//��ȡ��������������Ϣ��
				if(Json::nullValue == root["MMIParamContext"].type())
					throw -4;
				MMIParamContext = root["MMIParamContext"];

				//��ȡ�汾��
				if(Json::stringValue != MMIParamContext["Version"].type())
					throw -4;
				((CDVJsonParamMMI*)pDVJsonBase)->Version = MMIParamContext["Version"].asCString();

				//��ȡ��������
				if(Json::nullValue == MMIParamContext["Data"].type())
					throw -4;
				Data = MMIParamContext["Data"];

				//��ȡ������ʶ
				if( (Json::booleanValue != Data["ClearMMI"].type()) && (Json::intValue != Data["ClearMMI"].type()))
					throw -4;
				if(Json::booleanValue == Data["ClearMMI"].type())
				{
					((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI = (int)Data["ClearMMI"].asBool();
				}
				else if(Json::intValue == Data["ClearMMI"].type())
				{
					((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI = Data["ClearMMI"].asInt();
				}
				//������ָ��
				if(0 == ((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI)
				{
					//��ȡ����������
					if(Json::nullValue == Data["ItemData"].type())
						throw -4;
					ItemData = Data["ItemData"];

					//��ȡ������ʾ��
					if(Json::nullValue == ItemData["Text"].type())
						throw -4;
					aryTextView = ItemData["Text"];
					for (int i=0; i < aryTextView.size();++i)
					{
						tagMMIItemText sText = {0};
						if(Json::intValue == aryTextView[i]["Type"].type())
						{
							sText.Type = aryTextView[i]["Type"].asInt();
						}
						if(Json::intValue == aryTextView[i]["Sub"].type())
						{
							sText.Sub = aryTextView[i]["Sub"].asInt();
						}
						if(Json::intValue == aryTextView[i]["Row"].type())
						{
							sText.Row = aryTextView[i]["Row"].asInt();
						}
						if(Json::intValue == aryTextView[i]["IsScroll"].type())
						{
							sText.IsScroll = aryTextView[i]["IsScroll"].asBool();
						}
						if(Json::stringValue == aryTextView[i]["Context"].type())
						{
							_tcscpy_s(sText.Context,_countof(sText.Context),aryTextView[i]["Context"].asCString());
						}
						//�ж������Ƿ�Ϸ�
						if(sText.Sub>0)
						{
							((CDVJsonParamMMI*)pDVJsonBase)->Text.Add(sText);
						}

					}

					//��ȡ������ʾ��
					if(Json::nullValue == ItemData["Voice"].type())
						throw -4;
					aryVoiceView = root["Voice"];
					for (int i=0; i < aryVoiceView.size();++i)
					{
						tagMMIItemVoice sVoice = {0};
						if(Json::intValue == aryVoiceView[i]["Type"].type())
						{
							sVoice.Type = aryVoiceView[i]["Type"].asInt();
						}
						if(Json::intValue == aryVoiceView[i]["No"].type())
						{
							sVoice.No = aryVoiceView[i]["No"].asInt();
						}

						((CDVJsonParamMMI*)pDVJsonBase)->Voice.Add(sVoice);
					}

					//��ȡͼ����ʾ��
					if(Json::nullValue == ItemData["Voice"].type())
						throw -4;
					aryGraphView = root["Voice"];
					for (int i=0; i < aryGraphView.size();++i)
					{
						tagMMIItemGraph sGraph = {0};
						if(Json::intValue == aryGraphView[i]["Type"].type())
						{
							sGraph.Type = aryGraphView[i]["Type"].asInt();
						}
						if(Json::intValue == aryGraphView[i]["No"].type())
						{
							sGraph.No = aryGraphView[i]["No"].asInt();
						}

						((CDVJsonParamMMI*)pDVJsonBase)->Graph.Add(sGraph);
					}
				}
			}
			break;
		}

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:

			break;
		case -2:
			break;
		case -3:
			break;
		case -4:
			break;
		default:
			break;
		}
	}

	return pDVJsonBase;
}
