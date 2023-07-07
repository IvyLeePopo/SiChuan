/*
* Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
* 
* 文件名称：JsonParamOperate.cpp
* 摘    要：接口参数JSON格式操作类，实现文件
* 当前版本：1.0.0.1
* 作    者：ROCY
* 创建日期：2016年8月15日
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
// 函数名称：CJsonParamOperate::FormatParamData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- 扣款参数格式转换
//     
// 输入参数：IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult
// 输出参数：CString
// 编写人员：FFFF
// 编写时间：2017年3月8日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonParamOperate::FormatParamData2Json(IN const CDVJsonParamDebitMoney* pDVJsonParamDebit)
{
	CString strReturn;
	try{
		if(NULL == pDVJsonParamDebit)
			throw -1;
		//表示整个json对象
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

		/***********************************************************构造JSAO数据结构***********************************************************/	
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

		//转化为字符串
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
// 函数名称：CJsonParamOperate::FormatParamData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- 支付请求参数数据变换(扣费结果)
//     
// 输入参数：IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonParamOperate::FormatParamData2Json(IN const CDVJsonParamDebitResult* pDVJsonParamDebitResult)
{
	CString strReturn;
	try
	{
		if(NULL == pDVJsonParamDebitResult)
			throw -1;
		// 表示整个 json 对象
		Json::Value root;
		// 标识用户参数
		Json::Value ResultContext;
		// 标识详细数据项
		Json::Value Data;
		// 标识关键项
		Json::Value KeyItem;
		/***********************************************************构造JSAO数据结构***********************************************************/

		//标识版本
		ResultContext["Version"]		= Json::Value(pDVJsonParamDebitResult->Version);
		//标识扣款类型
		Data["ResultType"]				= Json::Value(pDVJsonParamDebitResult->ResultType);
		//标识执行状态
		Data["ExecuteCode"]				= Json::Value(pDVJsonParamDebitResult->ExecuteCode);
		//标识执行结果描述
		Data["ExecuteDesc"]				= Json::Value(pDVJsonParamDebitResult->ExecuteDesc);

		//关键数据项 -- 交易安全码
		KeyItem["TradeSecurityCode"]	= Json::Value(pDVJsonParamDebitResult->TradeSecurityCode);
		//关键数据项 -- 第三方支付平台类型
		KeyItem["PayPlatformType"]		= Json::Value(pDVJsonParamDebitResult->PayPlatformType);
		//关键数据项 -- 交易识别码
		KeyItem["PayIdentifier"]		= Json::Value(pDVJsonParamDebitResult->PayIdentifier);
		//关键数据项 -- 订单号
		KeyItem["DebitOrder"]			= Json::Value(pDVJsonParamDebitResult->DebitOrder);
		//关键数据项 -- 服务端扣款时间
		KeyItem["DebitTime"]			= Json::Value(pDVJsonParamDebitResult->DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		//关键数据项 -- 消耗时间
		//remove zhuyabing 2017-06 KeyItem["ConsumeTime"]			= Json::Value(pDVJsonParamDebitResult->ConsumeTime);

		Data["TradeKeyItem"]		= KeyItem;
		ResultContext["Data"]		= Data;
		root["DebitResultContext"]	= ResultContext;
		//转化为字符串
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
// 函数名称：CJsonParamOperate::FormatParamData2Json
//     
// 功能描述：将接收到的数据格式转化为标准的JSON格式 -- 用户数据参数数据变换
//     
// 输入参数：IN const CDVJsonParamAccount* pDVJsonParamAccount
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonParamOperate::FormatParamData2Json(IN const CDVJsonParamAccount* pDVJsonParamAccount)
{
	CString strReturn;
	try
	{
		if(NULL == pDVJsonParamAccount)
			throw -1;
		// 表示整个 json 对象
		Json::Value root;
		// 标识用户参数
		Json::Value ParamAccount;
		// 标识详细数据项
		Json::Value Data;
		/***********************************************************构造JSON数据结构***********************************************************/

		//标识版本
		ParamAccount["Version"]	= Json::Value(pDVJsonParamAccount->Version);
		//标识数据类型
		Data["Type"]			= Json::Value(pDVJsonParamAccount->DataType);
		//标识数据来源
		Data["Source"]			= Json::Value(pDVJsonParamAccount->DataSrc);
		//标识数据内容
		Data["Context"]			= Json::Value(pDVJsonParamAccount->DataContext);

		ParamAccount["Data"]	= Data;
		root["AccountContext"]	= ParamAccount;
		//转化为字符串
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
// 函数名称：CJsonParamOperate::FormatResponseResult2Class
//     
// 功能描述：将JSON格式字符串转化为标准数据结构类
//     
// 输入参数：IN const CString& strResponseResult
//         ：IN const int& iDataType
// 输出参数：CDVJsonBase*
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
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
				//进行数据格式转换
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

				//获取用户信息区
				if(Json::nullValue == root["AccountContext"].type())
					throw -4;
				AccountContext = root["AccountContext"];

				//获取版本
				if(Json::stringValue != AccountContext["Version"].type())
					throw -4;
				pDVJsonBase->Version = AccountContext["Version"].asCString();

				//获取数据信息区
				if(Json::nullValue == AccountContext["Data"].type())
					throw -4;
				Data = AccountContext["Data"];

				//获取用户数据类型
				if(Json::intValue != Data["Type"].type())
					throw -4;
				((CDVJsonParamAccount*)pDVJsonBase)->DataType = Data["Type"].asInt();

				//获取用户数据来源
				if(Json::intValue != Data["Source"].type())
					throw -4;
				((CDVJsonParamAccount*)pDVJsonBase)->DataSrc = Data["Source"].asInt();

				//获取用户数据内容
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
				//进行数据格式转换
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

				//获取交易数据区
				if(Json::nullValue == root["DebitParamContext"].type())
					throw -4;
				DebitParamContext = root["DebitParamContext"];

				//获取数据明细区
				if(Json::nullValue == DebitParamContext["Data"].type())
					throw -4;
				Data = DebitParamContext["Data"];

				//获取超时时间
				if(Json::intValue != Data["OverTime"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->OverTime = Data["OverTime"].asInt();

				//获取扣款操作模式
				if(Json::nullValue == Data["OperationMode"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperationMode = 1;//默认整合扣款
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

				//获取扣款信息区
				if(Json::nullValue == Data["Debit"].type())
					throw -4;
				Debit = Data["Debit"];

				//获取扣款金额
				if(Json::intValue != Debit["Money"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->Money = Debit["Money"].asInt();

				//获取交易识别码
				if(Json::stringValue != Debit["PayIdentifier"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->PayIdentifier = Debit["PayIdentifier"].asCString();

				//只有非整合操作才需要凭证数据
				if(2 == ((CDVJsonParamDebitMoney*)pDVJsonBase)->OperationMode)
				{
					//获取支付凭证类型
					if(Json::intValue != Debit["PayCertificateType"].type())
						throw -4;
					((CDVJsonParamDebitMoney*)pDVJsonBase)->PayCertificateType = Debit["PayCertificateType"].asInt();

					//获取支付凭证内容
					if(Json::stringValue != Debit["PayCertificateCode"].type())
						throw -4;
					((CDVJsonParamDebitMoney*)pDVJsonBase)->PayCertificateCode = Debit["PayCertificateCode"].asCString();
				}

				//获取扣款主分类
				if(Json::intValue != Debit["Type"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->DebitMainType = Debit["Type"].asInt();

				//获取过车时间
				if(Json::stringValue != Debit["SubTime"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->SubTime = CTime(GetGlobalApp()->CString2time_t(Debit["SubTime"].asCString()));


				/***********************************获取车辆信息区***********************************/
				//获取车辆信息区
				if(Json::nullValue == Data["Vehicle"].type())
					throw -4;
				Vehicle = Data["Vehicle"];

				//获取车种
				if(Json::intValue != Vehicle["Type"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleType = Vehicle["Type"].asInt();

				//获取车型
				if(Json::intValue != Vehicle["Class"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleClass = Vehicle["Class"].asInt();

				//获取车牌
				if(Json::stringValue != Vehicle["License"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VehicleLicense = Vehicle["License"].asCString();

				//获取车牌颜色
				if(Json::intValue != Vehicle["VLColor"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->VLColor = Vehicle["VLColor"].asInt();

				//获取轴数
				if(Json::intValue != Vehicle["AxisCount"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->AxisCount = Vehicle["AxisCount"].asInt();

				//获取装载重量
				if(Json::intValue != Vehicle["Weight"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->Weight = Vehicle["Weight"].asInt();

				//获取限重
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



				/***********************************获取交易信息区***********************************/
				//获取交易信息区
				if(Json::nullValue == Data["Operation"].type())
					throw -4;
				Operation = Data["Operation"];

				//获取通行凭证类型
				if(Json::intValue != Operation["PassCertificateType"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->PassCertificateType = Operation["PassCertificateType"].asInt();

				//获取通行凭证卡号
				if(Json::stringValue != Operation["CardID"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->CardID = Operation["CardID"].asCString();

				//获取统计日
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

				//获取统计班次
				if(Json::intValue != Operation["ShiftID"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->ShiftID = Operation["ShiftID"].asInt();

				//获取打印票据号
				if(Json::stringValue == Operation["TicketNo"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->TicketNo = Operation["TicketNo"].asCString();
				}

				//获取收费员工号
				if(Json::stringValue == Operation["OperatorID"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorID = Operation["OperatorID"].asCString();
				}
				//获取收费员姓名
				if(Json::stringValue == Operation["OperatorName"].type())
				{
					((CDVJsonParamDebitMoney*)pDVJsonBase)->OperatorName = Operation["OperatorName"].asCString();
				}


				/***********************************获取入口信息区***********************************/
				//获取入口信息区
				if(Json::nullValue == Data["Entry"].type())
					throw -4;
				Entry = Data["Entry"];

				//获取入口区域编码
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
				

				//获取入口路段编码
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
				

				//获取入口站点编码
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
				
				
				
				//获取入口站点名称
				if(Json::stringValue != Entry["StationName"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryStationName = Entry["StationName"].asCString();

				//获取入口车道编码
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

				//获取入口车种
				if(Json::intValue != Entry["VType"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleType = Entry["VType"].asInt();

				//获取入口车型
				if(Json::intValue != Entry["VClass"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleClass = Entry["VClass"].asInt();

				//获取入口车牌
				if(Json::stringValue != Entry["License"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVehicleLicense = Entry["License"].asCString();

				//获取入口车牌颜色
				if(Json::intValue != Entry["VColor"].type())
					throw -4;
				((CDVJsonParamDebitMoney*)pDVJsonBase)->EntryVLColor = Entry["VColor"].asInt();

				//获取入口时间
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
				//进行数据格式转换
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

				//获取用户信息区
				if(Json::nullValue == root["DebitCancelParamContext"].type())
					throw -4;
				DebitCancelParamContext = root["DebitCancelParamContext"];

				//获取版本
				if(Json::stringValue != DebitCancelParamContext["Version"].type())
					throw -4;
				((CDVJsonParamDebitCancel*)pDVJsonBase)->Version = DebitCancelParamContext["Version"].asCString();

				//获取数据信息区
				if(Json::nullValue == DebitCancelParamContext["Data"].type())
					throw -4;
				Data = DebitCancelParamContext["Data"];

				//获取交易数据信息区
				if(Json::nullValue == Data["TradeKeyItem"].type())
					throw -4;
				TradeKeyItem = Data["TradeKeyItem"];

				//获取交易识别码
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
				//进行数据格式转换
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

				//获取用户信息区
				if(Json::nullValue == root["DebitResultContext"].type())
					throw -4;
				DebitResultContext = root["DebitResultContext"];

				//获取版本
				if(Json::stringValue != DebitResultContext["Version"].type())
					throw -4;
				pDVJsonBase->Version = DebitResultContext["Version"].asCString();

				//获取数据信息区
				if(Json::nullValue == DebitResultContext["Data"].type())
					throw -4;
				Data = DebitResultContext["Data"];

				//获取结果类型
				if(Json::intValue != Data["ResultType"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ResultType = Data["ResultType"].asInt();

				//获取执行状态
				if(Json::intValue != Data["ExecuteCode"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ExecuteCode = Data["ExecuteCode"].asInt();

				//获取执行结果描述
				if(Json::nullValue == Data["ExecuteDesc"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->ExecuteDesc = Data["ExecuteDesc"].asCString();

				//获取数据信息区
				if(Json::nullValue == Data["TradeKeyItem"].type())
					throw -4;
				TradeKeyItem = Data["TradeKeyItem"];


				//获取交易识别码
				if(Json::stringValue != TradeKeyItem["PayIdentifier"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->PayIdentifier = TradeKeyItem["PayIdentifier"].asCString();

				//获取第三方支付平台类型
				if(Json::intValue != TradeKeyItem["PayPlatformType"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->PayPlatformType = TradeKeyItem["PayPlatformType"].asInt();

				//获取交易安全码
				if(Json::stringValue != TradeKeyItem["TradeSecurityCode"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->TradeSecurityCode = TradeKeyItem["TradeSecurityCode"].asCString();

				//获取交订单号
				if(Json::stringValue != TradeKeyItem["DebitOrder"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->DebitOrder = TradeKeyItem["DebitOrder"].asCString();

				//获取服务器扣款时间
				if(Json::stringValue != TradeKeyItem["DebitTime"].type())
					throw -4;
				((CDVJsonParamDebitResult*)pDVJsonBase)->DebitTime = ::GetGlobalApp()->CString2time_t(TradeKeyItem["DebitTime"].asCString());

				//获取消耗时间
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
				//进行数据格式转换
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

				//获取交互界面设置信息区
				if(Json::nullValue == root["MMIParamContext"].type())
					throw -4;
				MMIParamContext = root["MMIParamContext"];

				//获取版本号
				if(Json::stringValue != MMIParamContext["Version"].type())
					throw -4;
				((CDVJsonParamMMI*)pDVJsonBase)->Version = MMIParamContext["Version"].asCString();

				//获取数据区域
				if(Json::nullValue == MMIParamContext["Data"].type())
					throw -4;
				Data = MMIParamContext["Data"];

				//获取清屏标识
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
				//非清屏指令
				if(0 == ((CDVJsonParamMMI*)pDVJsonBase)->ClearMMI)
				{
					//获取设置项内容
					if(Json::nullValue == Data["ItemData"].type())
						throw -4;
					ItemData = Data["ItemData"];

					//获取文字显示项
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
						//判断数据是否合法
						if(sText.Sub>0)
						{
							((CDVJsonParamMMI*)pDVJsonBase)->Text.Add(sText);
						}

					}

					//获取声音显示项
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

					//获取图形显示项
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
