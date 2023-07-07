/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLDebitParamContext.cpp
 * ժ    Ҫ���ۿ���ϸ����XML�ṹ������ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��2��
 */
#include "StdAfx.h"
#include "DVXMLDebitMoney.h"
#include "TWSDNetPay_Dll.h"


CDVXMLDebitParamContext::CDVXMLDebitParamContext(void)
	:OverTime(0)
	,OperationMode(0)

	,PayIdentifier(_T(""))
	,PayCertificateType(0)
	,PayCertificateCode(_T(""))
	,Money(0)
	,DebitMainType(0)

	,VehicleType(0)
	,VehicleClass(0)
	,VLColor(0)
	,VehicleLicense(_T(""))
	,AxisCount(0)
	,Weight(0)
	,LimitWeight(0)

	,PassCertificateType(0)
	,CardID(_T(""))
	,TollDate(0)
	,ShiftID(0)
	,TicketNo(_T(""))
	,OperatorID(_T(""))
	,OperatorName(_T(""))

	,EntryRoadID("0")
	,EntryAreaID("0")
	,EntryStationID(_T(""))
	,EntryStationName(_T(""))
	,EntryLane(_T(""))
	,EntryVehicleLicense(_T(""))
	,EntryVLColor(0)
	,EntryVehicleType(0)
	,EntryVehicleClass(0)
	,Distance(0)
{
	XMLFunctionDesc.Format(_T("DebitParamContext"));
}

CDVXMLDebitParamContext::~CDVXMLDebitParamContext(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVXMLDebitParamContext::Init
//     
// ��������������XML�ַ���
//     
// ���������IN const CString& strXMLString
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CDVXMLDebitParamContext::Init(IN const CString& strXMLString)
{
	BOOL bReturn = FALSE;
	CString strTemp;
	CMarkup xml;
	try
	{
		xml.SetDoc( strXMLString );

		xml.ResetMainPos();

		/****************************************************************���ݸ�ʽ�ж�****************************************************************/

		if(!xml.FindElem(XMLFunctionDesc))
			throw -1;
		xml.IntoElem();

		xml.ResetPos();
		if(!xml.FindChildElem(_T("Version")))
			throw -2;
		XMLVersion = xml.GetChildData();

		xml.ResetPos();
		if(!xml.FindChildElem(_T("Data")))
			throw -3;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("OverTime")))
			throw -4;
		OverTime = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("OperationMode")))
			throw -5;
		OperationMode = _ttoi(xml.GetChildData());

		/*****************Debit��ϸ��************************/
		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Debit")))
			throw -6;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("PayIdentifier")))
			throw -7;
		PayIdentifier = xml.GetChildData();

		//�����ϲ�������Ҫ���и�������У��
		if(2 == OperationMode)
		{
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("PayCertificateType")))
				throw -35;
			PayCertificateType = _ttoi(xml.GetChildData());

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("PayCertificateCode")))
				throw -34;
			PayCertificateCode = xml.GetChildData();
		}

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Money")))
			throw -8;
		Money = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("SubTime")))
			throw -9;
		strTemp = xml.GetChildData();
		SubTime = CTime(GetGlobalApp()->CString2time_t(strTemp));

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Type")))
			throw -10;
		DebitMainType = _ttoi(xml.GetChildData());

		xml.OutOfElem();

		/*****************Vehicle��ϸ��************************/
		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Vehicle")))
			throw -11;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Type")))
			throw -12;
		VehicleType = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Class")))
			throw -13;
		VehicleClass = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("License")))
			throw -14;
		VehicleLicense = xml.GetChildData();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("VLColor")))
			throw -15;
		VLColor = _ttoi(xml.GetChildData());

		if(4 != DebitMainType)
		{
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("AxisCount")))
				throw -32;
			AxisCount = _ttoi(xml.GetChildData());

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("Weight")))
				throw -33;
			Weight = _ttoi(xml.GetChildData());

			xml.ResetChildPos();
			if(xml.FindChildElem(_T("LimitWeight")))
			{
				LimitWeight = _ttoi(xml.GetChildData());
			}
		}
		//add 2018-03
		xml.ResetChildPos();
		this->AutoColor = 0;
		this->AutoLicense="AutoLicense";
		if(xml.FindChildElem(_T("AutoPlate")))
		{

			xml.IntoElem();
			xml.ResetChildPos();
			if(xml.FindChildElem(_T("AutoLicense")))
			{
				this->AutoLicense = xml.GetChildData();
			}
			xml.ResetChildPos();
			if(xml.FindChildElem(_T("AutoColor")))
			{
				this->AutoColor = _ttoi(xml.GetChildData());
			}
			xml.OutOfElem();
		}
		

		xml.OutOfElem();

		/*****************Operation��ϸ��************************/
		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Operation")))
			throw -16;
		xml.IntoElem();
		
		if(4 != DebitMainType)
		{
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("PassCertificateType")))
				throw -17;
			PassCertificateType = _ttoi(xml.GetChildData());

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("CardID")))
				throw -18;
			CardID = xml.GetChildData();
		}

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("TollDate")))
			throw -19;
		TollDate = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("ShiftID")))
			throw -20;
		ShiftID = _ttoi(xml.GetChildData());

		xml.ResetChildPos();
		if(xml.FindChildElem(_T("TicketNo")))
		{
			TicketNo = xml.GetChildData();
		}

		//�����շ�Ա��Ϣ������ҵ������ 1.0.3.4�汾��
		xml.ResetChildPos();
		if(xml.FindChildElem(_T("OperatorID")))
		{
			OperatorID = xml.GetChildData();
		}

		xml.ResetChildPos();
		if(xml.FindChildElem(_T("OperatorName")))
		{
			OperatorName = xml.GetChildData();
		}
		xml.ResetChildPos();
		if(xml.FindChildElem(_T("Distance")))
		{
			this->Distance = _ttoi(xml.GetChildData());
		}

		xml.OutOfElem();

		if(4 != DebitMainType)
		{
			/*****************Entry��ϸ��************************/
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("Entry")))
				throw -21;
			xml.IntoElem();

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("StationID")))
				throw -22;
			EntryStationID = xml.GetChildData();

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("StationName")))
				throw -23;
			EntryStationName = xml.GetChildData();

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("Time")))
				throw -24;
			strTemp = xml.GetChildData();
			EntryTime = CTime(GetGlobalApp()->CString2time_t(strTemp));

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("LaneID")))
				throw -25;
			EntryLane = xml.GetChildData();

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("License")))
				throw -26;
			EntryVehicleLicense = xml.GetChildData();

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("VColor")))
				throw -27;
			EntryVLColor = _ttoi(xml.GetChildData());

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("VClass")))
				throw -28;
			EntryVehicleClass = _ttoi(xml.GetChildData());

			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("VType")))
				throw -29;
			EntryVehicleType = _ttoi(xml.GetChildData());
		
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("RoadID")))
				throw -34;
			EntryRoadID = xml.GetChildData();//_ttoi(xml.GetChildData());
		
			xml.ResetChildPos();
			if(!xml.FindChildElem(_T("AreaID")))
				throw -35;
			EntryAreaID = xml.GetChildData();//_ttoi(xml.GetChildData());

			xml.OutOfElem();
		}

		/****************************************************************�����߼��ж�****************************************************************/
		if(OperationMode>2 || OperationMode<1)
			throw -30;

		if(PayIdentifier.IsEmpty())
			throw -31;
	
		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ������ͷ DebitParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ�����ݰ汾 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ�������� Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��ʱʱ��) OverTime"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(�ۿ�ģʽ) OperationMode"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ�������� Debit"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(����ʶ����) PayIdentifier"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(�ۿ���) Money"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(�ݽ�ʱ��) SubTime"));
			break;
		case -10:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(�ۿ�����) Type"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ�������� Vehicle"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��������-�ͻ�����) Type"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��������-�������) Class"));
			break;
		case -14:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��������) License"));
			break;
		case -15:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(������ɫ) VLColor"));
			break;
		case -16:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ�������� Operation"));
			break;
		case -17:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(ͨ��ƾ֤����) PassCertificateType"));
			break;
		case -18:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(ͨ��ƾ֤����) CardID"));
			break;
		case -19:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(ͳ������) TollDate"));
			break;
		case -20:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(ͳ�ư��) ShiftID"));
			break;
		case -21:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ�������� Entry"));
			break;
		case -22:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(���վ�����) StationID"));
			break;
		case -23:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(���վ������) StationName"));
			break;
		case -24:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(���ʱ��) Time"));
			break;
		case -25:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��ڳ�������) LaneID"));
			break;
		case -26:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��ڳ���) License"));
			break;
		case -27:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��ڳ�����ɫ) VColor"));
			break;
		case -28:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��ڳ���) VClass"));
			break;
		case -29:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��ڳ���) VType"));
			break;
		case -32:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��������) AxisCount"));
			break;
		case -33:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(��������) Weight"));
			break;
		case -34:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(�������) AreaID"));
			break;
		case -35:
			m_strLastErrorDesc.Format(_T("�ۿ���������ʧ��,�޷��ҵ�ָ��������(·�α���) RoadID"));
			break;
		}
	}

	return bReturn;
}