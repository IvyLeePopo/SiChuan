/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：DVXMLDebitParamContext.cpp
 * 摘    要：扣款明细数据XML结构解析，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月2日
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
// 函数名称：CDVXMLDebitParamContext::Init
//     
// 功能描述：解析XML字符串
//     
// 输入参数：IN const CString& strXMLString
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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

		/****************************************************************数据格式判断****************************************************************/

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

		/*****************Debit详细项************************/
		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("Debit")))
			throw -6;
		xml.IntoElem();

		xml.ResetChildPos();
		if(!xml.FindChildElem(_T("PayIdentifier")))
			throw -7;
		PayIdentifier = xml.GetChildData();

		//非整合操作才需要进行该项数据校验
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

		/*****************Vehicle详细项************************/
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

		/*****************Operation详细项************************/
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

		//新增收费员信息（陕西业务增添 1.0.3.4版本）
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
			/*****************Entry详细项************************/
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

		/****************************************************************数据逻辑判断****************************************************************/
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
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据头 DebitParamContext"));
			break;
		case -2:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据版本 Version"));
			break;
		case -3:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据区 Data"));
			break;
		case -4:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(超时时间) OverTime"));
			break;
		case -5:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(扣款模式) OperationMode"));
			break;
		case -6:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据区 Debit"));
			break;
		case -7:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(交易识别码) PayIdentifier"));
			break;
		case -8:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(扣款金额) Money"));
			break;
		case -9:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(递交时间) SubTime"));
			break;
		case -10:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(扣款类型) Type"));
			break;
		case -11:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据区 Vehicle"));
			break;
		case -12:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(车辆种类-客货分类) Type"));
			break;
		case -13:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(车辆类型-国标分类) Class"));
			break;
		case -14:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(车辆牌照) License"));
			break;
		case -15:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(车牌颜色) VLColor"));
			break;
		case -16:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据区 Operation"));
			break;
		case -17:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(通行凭证类型) PassCertificateType"));
			break;
		case -18:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(通行凭证编码) CardID"));
			break;
		case -19:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(统计日期) TollDate"));
			break;
		case -20:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(统计班次) ShiftID"));
			break;
		case -21:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据区 Entry"));
			break;
		case -22:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口站点编码) StationID"));
			break;
		case -23:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口站点名称) StationName"));
			break;
		case -24:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口时间) Time"));
			break;
		case -25:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口车道编码) LaneID"));
			break;
		case -26:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口车牌) License"));
			break;
		case -27:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口车牌颜色) VColor"));
			break;
		case -28:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口车型) VClass"));
			break;
		case -29:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(入口车种) VType"));
			break;
		case -32:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(车辆轴数) AxisCount"));
			break;
		case -33:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(车辆总重) Weight"));
			break;
		case -34:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(区域编码) AreaID"));
			break;
		case -35:
			m_strLastErrorDesc.Format(_T("扣款传入参数解析失败,无法找到指定数据项(路段编码) RoadID"));
			break;
		}
	}

	return bReturn;
}