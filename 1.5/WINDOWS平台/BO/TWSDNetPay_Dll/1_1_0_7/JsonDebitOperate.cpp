/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonDebitOperate.cpp
 * 摘    要：控制互联网扣款数据，进行数据存取以及格式化流程，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年2月27日
 */
#include "StdAfx.h"
#include "JsonDebitOperate.h"

CJsonDebitOperate::CJsonDebitOperate(void)
{
}

CJsonDebitOperate::~CJsonDebitOperate(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonDebitOperate::FormatSendData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式
//     
// 输入参数：IN const CDVJsonDebit* pDVJsonDebit	--	需要转化的数据结构类
// 编写人员：ROCY
// 编写时间：2015年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：PC发往HTTP端
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebit* pDVJsonDebit, IN const int SerialNum)
{
	CString strReturn;
	
	if(NULL != pDVJsonDebit)
	{
		// 表示整个 json 对象
		Json::Value root;
		// 临时JSON对象
		Json::Value DebitCondition;

		Json::Value DebitConditionBase;

		Json::Value DebitConditionAddtional;

		Json::Value AddtionalEntryStation;

		Json::Value AddtionalPayStation;

		Json::Value AddtionalVehicleInfo;

		Json::Value DebitConditionNFCPayInfo;
		/***********************************************************构造JSAO你数据结构***********************************************************/
		
		//标识版本
		root["Version"]				= Json::Value(pDVJsonDebit->Version);
		//标识数据模式(正式数据或测试数据)
		root["IsTestData"]			= Json::Value(pDVJsonDebit->TestingData);
		//标识数据类型
		root["PackageType"]			= Json::Value(pDVJsonDebit->PackageType);
		//安全认证
		root["Token"]				= Json::Value(pDVJsonDebit->Token);

		//新增序列号
		root["SN"]					= Json::Value( SerialNum );

		//标识省份
		root["ProvinceID"]			= Json::Value(pDVJsonDebit->ProvinceID);
		//递交次数
		root["SubmitCount"]			= Json::Value(pDVJsonDebit->SubmitCount);
		//递交时间
		root["SubmitTime"]			= Json::Value(pDVJsonDebit->SubmitTime);
		//交易标记
		root["RecordMatchCode"]		= Json::Value(pDVJsonDebit->RecordMatchCode);
		//扣款凭证
		root["PayIdentifier"]		= Json::Value(pDVJsonDebit->PayIdentifier);
		//交易安全认证码
		root["TradeSecurityCode"]	= Json::Value(pDVJsonDebit->TradeSecurityCode);
		
		//是否测试数据
		root["TestingData"]			= Json::Value((int)pDVJsonDebit->TestingData);
		//业务动态库版本号
		root["NetPayDllVersion"]	= Json::Value(pDVJsonDebit->NetPayDllVersion);
		//设备动态库版本号
		root["DeviceDllVersion"]	= Json::Value(pDVJsonDebit->DeviceDllVersion);
		//用户数据数据来源
		root["AccountInfoSrcType"]	= Json::Value(pDVJsonDebit->AccountInfoSrcType);
		//用户数据具体设备号
		root["AccountInfoDeviceID"]	= Json::Value(pDVJsonDebit->AccountInfoDeviceID);
		//用户数据具体类型
		root["AccountInfoDataType"]	= Json::Value(pDVJsonDebit->AccountInfoDataType);
		//设备唯一编码
		root["DeviceSerialNo"]		= Json::Value(pDVJsonDebit->DeviceSerialNo);
		//全字段加密结果
		root["EnDataString"]		= Json::Value(pDVJsonDebit->CreateEnDataString());

		//填充基本信息
		DebitConditionBase["ChannelUserId"]			= Json::Value(pDVJsonDebit->IDBindInPayPlatform);
		DebitConditionBase["ChannelType"]			= Json::Value(pDVJsonDebit->IDBindInPayPlatformType);
		DebitConditionBase["PayPlatformQRCode"]		= Json::Value(pDVJsonDebit->QRCodeData);
		DebitConditionBase["VirtualCardID"]			= Json::Value(pDVJsonDebit->SMVirtualCardID);
		DebitConditionBase["EntryInfoUniqueFlag"]	= Json::Value(pDVJsonDebit->EntryInfoUniqueFlag);
		DebitConditionBase["DebitMoney"]			= Json::Value(pDVJsonDebit->DebitMoney);
		DebitConditionBase["DebitType"]				= Json::Value(pDVJsonDebit->DebitType);
		DebitConditionBase["DebitTime"]				= Json::Value(pDVJsonDebit->DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		DebitConditionBase["TollDate"]				= Json::Value(pDVJsonDebit->DebitTollDate);
		DebitConditionBase["TollShift"]				= Json::Value(pDVJsonDebit->DebitTollShift);

		//add 2018-03
		DebitConditionBase["PassCertificateType"]	= Json::Value(pDVJsonDebit->PassCertificateType);
		DebitConditionBase["TicketNo"]	= Json::Value(pDVJsonDebit->TicketNo);
		DebitConditionBase["Distance"]	= Json::Value(pDVJsonDebit->Distance);
		//add 2018-04
		//DebitConditionBase["ImageID"]	= Json::Value(pDVJsonDebit->imageId);

		//填充附加信息 -- 入站信息
		AddtionalEntryStation["EntryProvinceId"]	= Json::Value(pDVJsonDebit->EntryProvinceID);
		AddtionalEntryStation["EntryStationId"]		= Json::Value(pDVJsonDebit->EntryStationID);
		AddtionalEntryStation["EntryStationName"]	= Json::Value(pDVJsonDebit->EntryStationName);
		AddtionalEntryStation["EntryLane"]			= Json::Value(pDVJsonDebit->EntryLane);
		AddtionalEntryStation["EntryTime"]			= Json::Value(pDVJsonDebit->EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")));
		AddtionalEntryStation["EntryRoadNo"]		= Json::Value(pDVJsonDebit->EntryRoadID);
		AddtionalEntryStation["EntryAreaNo"]		= Json::Value(pDVJsonDebit->EntryAreaID);
		
		//add 2018-03
		AddtionalEntryStation["EntryLicense"]		= Json::Value(pDVJsonDebit->EntryLicense);
		AddtionalEntryStation["EntryVColor"]		= Json::Value(pDVJsonDebit->EntryVColor);
		AddtionalEntryStation["EntryVClass"]		= Json::Value(pDVJsonDebit->EntryVClass);
		AddtionalEntryStation["EntryVType"]		= Json::Value(pDVJsonDebit->EntryVType);
		




		//填充附加信息 -- 扣费站信息
		AddtionalPayStation["PayProvinceId"]		= Json::Value(pDVJsonDebit->PayProvinceID);
		AddtionalPayStation["PayStationId"]			= Json::Value(pDVJsonDebit->PayStationID);
		AddtionalPayStation["PayStationName"]		= Json::Value(pDVJsonDebit->PayStationName);
		AddtionalPayStation["PayLane"]				= Json::Value(pDVJsonDebit->PayLane);
		AddtionalPayStation["PayRoadNo"]			= Json::Value(pDVJsonDebit->PayRoadID);
		AddtionalPayStation["PayOperatorId"]		= Json::Value(pDVJsonDebit->PayOperatorID);
		AddtionalPayStation["PayOperatorName"]		= Json::Value(pDVJsonDebit->PayOperatorName);



		//填充附加信息 -- 扣费车辆信息
		AddtionalVehicleInfo["VehicleClass"]		= Json::Value(pDVJsonDebit->VehicleClass);
		AddtionalVehicleInfo["VehicleType"]			= Json::Value(pDVJsonDebit->VehicleType);
		AddtionalVehicleInfo["VehicleLicense"]		= Json::Value(pDVJsonDebit->VehicleLicense);
		AddtionalVehicleInfo["VehicleColor"]		= Json::Value(pDVJsonDebit->VehicleColor);
		AddtionalVehicleInfo["AxisCount"]			= Json::Value(pDVJsonDebit->AxisCount);
		AddtionalVehicleInfo["TotalWeight"]			= Json::Value(pDVJsonDebit->TotalWeight);
		AddtionalVehicleInfo["LimitWeight"]			= Json::Value(pDVJsonDebit->LimitWeight);

		//add 2018-04
		AddtionalVehicleInfo["ImageID"]	= Json::Value(pDVJsonDebit->imageId);
		//add 2018-03
		AddtionalVehicleInfo["AutoColor"]			= Json::Value(pDVJsonDebit->AutoColor);
		AddtionalVehicleInfo["AutoLicense"]			= Json::Value(pDVJsonDebit->AutoLicense);
		//填充银联NFC支付信息
		CString strBase64Temp;
		GetGlobalApp()->HexToBase64(pDVJsonDebit->NFCTerminal,strBase64Temp);
		DebitConditionNFCPayInfo["Terminal"]	= Json::Value(strBase64Temp);

		GetGlobalApp()->HexToBase64(pDVJsonDebit->NFCSerialNo,strBase64Temp);
		DebitConditionNFCPayInfo["Sn"]			= Json::Value(strBase64Temp);

		GetGlobalApp()->HexToBase64(pDVJsonDebit->NFCPayData,strBase64Temp);
		DebitConditionNFCPayInfo["PayData"]		= Json::Value(strBase64Temp);

		GetGlobalApp()->HexToBase64(pDVJsonDebit->NFCRevData,strBase64Temp);
		DebitConditionNFCPayInfo["RevData"]		= Json::Value(strBase64Temp);

		//格式化
		DebitConditionAddtional["EntryStation"] = AddtionalEntryStation;
		DebitConditionAddtional["PayStation"]	= AddtionalPayStation;
		DebitConditionAddtional["VehicleInfo"]	= AddtionalVehicleInfo;

		DebitCondition["NFCPayInfo"]	= DebitConditionNFCPayInfo;
		DebitCondition["Base"]			= DebitConditionBase;
		DebitCondition["Addtional"]		= DebitConditionAddtional;

		root["Data"] = DebitCondition;

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
		//调试使用，读/写取数据
		#ifdef _JSON_DEBUG_LOG_STRING
			try
			{
				CFile cFile;
				size_t ttLen = 0;
				TCHAR* szBuff = NULL;
				CString strResult(strReturn);
				CString strFilePath;
				strFilePath.Format(_T("%s\\DebitMoney.txt"),::GetGlobalApp()->m_strPathTempDir);
				if(cFile.Open(strFilePath,CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate))
				{
					////读数据
					//szBuff = new TCHAR[cFile.GetLength()+1];
					//memset(szBuff,0,cFile.GetLength()+1);
					//cFile.Read(szBuff,cFile.GetLength());
					//cFile.Close();
					//strResult.Format(_T("%s"),(LPCTSTR)szBuff);
					//ttLen = strResult.GetLength();
					//delete szBuff;
					//szBuff = NULL;

					//写数据
					cFile.SeekToBegin();
					cFile.Write(strResult.GetBuffer(0),strResult.GetLength());
					strResult.ReleaseBuffer();
					cFile.Flush();
					cFile.SetLength(strResult.GetLength());
					cFile.Close();
				}
			}
			catch(...)
			{
			}
		#endif
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString	CJsonDebitOperate::FormatSendData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- 扣费状态查询
//     
// 输入参数：IN const CDVJsonDebitCheck* pDVJsonDebitCheck
// 编写人员：ROCY
// 编写时间：2015年12月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：PC->Http
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebitCheck* pDVJsonDebitCheck, IN const int SerialNum)
{
	CString strReturn;
	if(NULL != pDVJsonDebitCheck)
	{
		// 表示整个 json 对象
		Json::Value root;
		// 临时JSON对象
		Json::Value DebitCheckCondition;

		Json::Value DebitCheckBase;
		/***********************************************************构造JSAO你数据结构***********************************************************/
		
		//标识版本
		root["Version"]				= Json::Value(pDVJsonDebitCheck->Version);
		//标识数据类型
		root["PackageType"]			= Json::Value(pDVJsonDebitCheck->PackageType);
		//安全认证
		root["Token"]				= Json::Value(pDVJsonDebitCheck->Token);

		//新增序列号
		root["SN"]					= Json::Value( SerialNum );

		//标识省份
		root["ProvinceID"]			= Json::Value(pDVJsonDebitCheck->ProvinceID);
		//递交次数
		root["SubmitCount"]			= Json::Value(pDVJsonDebitCheck->SubmitCount);
		//递交时间
		root["SubmitTime"]			= Json::Value(pDVJsonDebitCheck->SubmitTime);
		//扣款凭证
		root["PayIdentifier"]		= Json::Value(pDVJsonDebitCheck->PayIdentifier);

		////填充请求信息
		//DebitCheckBase["ChannelUserId"]		= Json::Value(pDVJsonDebitCheck->IDBindInPayPlatform);
		//DebitCheckBase["ChannelType"]		= Json::Value(pDVJsonDebitCheck->IDBindInPayPlatformType);
		//DebitCheckBase["PayPlatformQRCode"]	= Json::Value(pDVJsonDebitCheck->QRCodeData);
		//DebitCheckBase["PayIdentifier"]	= Json::Value(pDVJsonDebitCheck->PayIdentifier);
		//DebitCheckBase["DebitMoney"]		= Json::Value(pDVJsonDebitCheck->DebitMoney);
		//DebitCheckBase["DebitType"]			= Json::Value(pDVJsonDebitCheck->DebitType);
		//DebitCheckBase["DebitTime"]			= Json::Value(pDVJsonDebitCheck->DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		////格式化
		//DebitCheckCondition["Base"]	= DebitCheckBase;
		//root["Data"] = DebitCheckCondition;

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CString	CJsonDebitOperate::FormatSendData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- 支付请求(撤单)
//     
// 输入参数：IN const CDVJsonDebitCancel* pDVJsonDebitCancel
// 编写人员：ROCY
// 编写时间：2016年1月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：PC->Http
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString	CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebitCancel* pDVJsonDebitCancel)
{
	CString strReturn;
	if(NULL != pDVJsonDebitCancel)
	{
		// 表示整个 json 对象
		Json::Value root;
		// 临时JSON对象
		Json::Value DebitCancelCondition;

		Json::Value DebitCancelBase;
		/***********************************************************构造JSAO数据结构***********************************************************/
		
		//标识版本
		root["Version"]				= Json::Value(pDVJsonDebitCancel->Version);
		//标识数据类型
		root["PackageType"]			= Json::Value(pDVJsonDebitCancel->PackageType);
		//安全认证
		root["Token"]				= Json::Value(pDVJsonDebitCancel->Token);
		//标识省份
		root["ProvinceID"]			= Json::Value(pDVJsonDebitCancel->ProvinceID);
		//递交次数
		root["SubmitCount"]			= Json::Value(pDVJsonDebitCancel->SubmitCount);
		//递交时间
		root["SubmitTime"]			= Json::Value(pDVJsonDebitCancel->SubmitTime);
		//扣款凭证
		root["PayIdentifier"]		= Json::Value(pDVJsonDebitCancel->PayIdentifier);

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonDebitOperate::FormatSendData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- 支付请求（二次处理）
//     
// 输入参数：IN const CDVJsonDebitSecond* pDVJsonDebitSecond
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2016年5月30日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：本地-》Http服务端
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CJsonDebitOperate::FormatSendData2Json(IN const CDVJsonDebitSecond* pDVJsonDebitSecond)
{
	CString strReturn;
	
	if(NULL != pDVJsonDebitSecond)
	{
		// 表示整个 json 对象
		Json::Value root;
		// 临时JSON对象
		Json::Value DebitCondition;

		Json::Value DebitConditionBase;

		Json::Value DebitConditionAddtional;

		Json::Value AddtionalEntryStation;

		Json::Value AddtionalPayStation;

		Json::Value AddtionalVehicleInfo;
		/***********************************************************构造JSAO你数据结构***********************************************************/
		
		//标识版本
		root["Version"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.Version);
		//标识数据类型
		root["PackageType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PackageType);
		//安全认证
		root["Token"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.Token);
		//标识省份
		root["ProvinceID"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.ProvinceID);
		//递交次数
		root["SubmitCount"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SubmitCount);
		//递交时间
		root["SubmitTime"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SubmitTime);
		//交易标记
		root["RecordMatchCode"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.RecordMatchCode);
		//上次扣款凭证
		root["ReversePayIdentifier"]= Json::Value(pDVJsonDebitSecond->m_strLastPayIdentifier);
		//扣款凭证
		root["PayIdentifier"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayIdentifier);
		//交易安全认证码
		root["TradeSecurityCode"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.TradeSecurityCode);

		//填充基本信息
		DebitConditionBase["ChannelUserId"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.IDBindInPayPlatform);
		DebitConditionBase["ChannelType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.IDBindInPayPlatformType);
		DebitConditionBase["PayPlatformQRCode"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.QRCodeData);
		DebitConditionBase["VirtualCardID"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.SMVirtualCardID);
		DebitConditionBase["EntryInfoUniqueFlag"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryInfoUniqueFlag);
		DebitConditionBase["DebitMoney"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitMoney);
		DebitConditionBase["DebitType"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitType);
		DebitConditionBase["DebitTime"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.DebitTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		//填充附加信息 -- 入站信息
		AddtionalEntryStation["EntryProvinceId"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryProvinceID);
		AddtionalEntryStation["EntryStationId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryStationID);
		AddtionalEntryStation["EntryStationName"]	= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryStationName);
		AddtionalEntryStation["EntryLane"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryLane);
		AddtionalEntryStation["EntryTime"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.EntryTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		//填充附加信息 -- 扣费站信息
		AddtionalPayStation["PayProvinceId"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayProvinceID);
		AddtionalPayStation["PayStationId"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayStationID);
		AddtionalPayStation["PayStationName"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayStationName);
		AddtionalPayStation["PayLane"]				= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.PayLane);

		//填充附加信息 -- 扣费车辆信息
		AddtionalVehicleInfo["VehicleClass"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleClass);
		AddtionalVehicleInfo["VehicleType"]			= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleType);
		AddtionalVehicleInfo["VehicleLicense"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleLicense);
		AddtionalVehicleInfo["VehicleColor"]		= Json::Value(pDVJsonDebitSecond->m_cDVJsonDebit.VehicleColor);

		//格式化
		DebitConditionAddtional["EntryStation"] = AddtionalEntryStation;
		DebitConditionAddtional["PayStation"]	= AddtionalPayStation;
		DebitConditionAddtional["VehicleInfo"]	= AddtionalVehicleInfo;

		DebitCondition["Base"]			= DebitConditionBase;
		DebitCondition["Addtional"]		= DebitConditionAddtional;

		root["Data"] = DebitCondition;

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
		//调试使用，读/写取数据
		#ifdef _JSON_DEBUG_LOG_STRING
			try
			{
				CFile cFile;
				size_t ttLen = 0;
				TCHAR* szBuff = NULL;
				CString strResult(strReturn);
				CString strFilePath;
				strFilePath.Format(_T("%s\\DebitMoney.txt"),::GetGlobalApp()->m_strPathTempDir);
				if(cFile.Open(strFilePath,CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate))
				{
					////读数据
					//szBuff = new TCHAR[cFile.GetLength()+1];
					//memset(szBuff,0,cFile.GetLength()+1);
					//cFile.Read(szBuff,cFile.GetLength());
					//cFile.Close();
					//strResult.Format(_T("%s"),(LPCTSTR)szBuff);
					//ttLen = strResult.GetLength();
					//delete szBuff;
					//szBuff = NULL;

					//写数据
					cFile.SeekToBegin();
					cFile.Write(strResult.GetBuffer(0),strResult.GetLength());
					strResult.ReleaseBuffer();
					cFile.Flush();
					cFile.SetLength(strResult.GetLength());
					cFile.Close();
				}
			}
			catch(...)
			{
			}
		#endif
	}
	return strReturn;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：LPVOID	CJsonDebitOperate::FormatResponseResult2Class
//     
// 功能描述：将服务器返回的JSON格式转化为标准数据结构类
//     
// 输入参数：IN const CString& strResponseResult	--	需要转化的JSON数据
//         ：IN const int& iDataType				--	需要转换的数据类型
// 编写人员：ROCY
// 编写时间：2015年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonDebitOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonBase*  pDVJsonBase = NULL;
	switch(iDataType)
	{
	case JSON_DT_SEND_DIBIT:
		pDVJsonBase = fnAnalysisStringForDebit(strResponseResult);
		break;
	case JSON_DT_SEND_DIBIT_CHECK:
		pDVJsonBase = fnAnalysisStringForDebitCheck(strResponseResult);
		break;
	case JSON_DT_SEND_DIBIT_CANCEL:
		pDVJsonBase = fnAnalysisStringForDebitCancel(strResponseResult);
		break;
	case JSON_DT_SEND_DIBIT_SECOND:
		pDVJsonBase = fnAnalysisStringForDebitSecond(strResponseResult);
		break;
	}
	return pDVJsonBase;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonDebitOperate::fnAnalysisStringForDebit
//     
// 功能描述：解析字符串，获得扣款操作回馈数据
//     
// 输入参数：IN const CString& strResponseResult
// 输出参数：CDVJsonDebit*
// 编写人员：ROCY
// 编写时间：2015年12月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebit* CJsonDebitOperate::fnAnalysisStringForDebit(IN const CString& strResponseResult)
{
	CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();

	try
	{
		if(NULL == pDVJsonDebit)
			throw -1;

		//进行数据格式转换
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//获取版本
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebit->Version					= root["Version"].asCString();
		}
		//获取数据包类型
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebit->PackageType				= root["PackageType"].asInt();
		}
		//获取响应状态代码
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebit->ServerResponseCode		= root["Status"].asInt();
		}
		//获取响应状态描述
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebit->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//获取服务器接收数据时间
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebit->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//获取服务器响应数据时间
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebit->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}
		//获取服务器实际扣款时间
		if(Json::stringValue == root["ServerDebitTime"].type())
		{
			pDVJsonDebit->ServerDebitTime	= CTime(GetGlobalApp()->CString2time_t(root["ServerDebitTime"].asCString()));
			pDVJsonDebit->ServerDebitTime = GetGlobalApp()->FixUTC(pDVJsonDebit->ServerDebitTime);///add 2018-01
		}

		//获取扣款详细状态代码
		if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusCode			= root["ErrorCodeByPayPlatform"].asInt();
		}
		//获取扣款详细状态描述
		if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusDesc			= root["ErrorDescByPayPlatform"].asCString();
		}
		//获取扣款订单号
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebit->DebitOrderNumber		= root["DebitOrderNumber"].asCString();
		}
		//获取交易安全码
		if(Json::stringValue == root["TradeSecurityCode"].type())
		{
			pDVJsonDebit->TradeSecurityCode		= root["TradeSecurityCode"].asCString();
		}
		//获取交易识别码
		if(Json::stringValue == root["PayIdentifier"].type())
		{
			pDVJsonDebit->PayIdentifier		= root["PayIdentifier"].asCString();
			((CDVJsonBase*)pDVJsonDebit)->PayIdentifier	= root["PayIdentifier"].asCString();
		}
		//获取支付平台类型
		if(Json::intValue == root["PayChannelType"].type())
		{
			pDVJsonDebit->QRCodeType		= root["PayChannelType"].asInt();
		}
		//获取虚拟卡号
		if(Json::stringValue == root["PayVirtualCardID"].type())
		{
			CString strTempID(root["PayVirtualCardID"].asCString());
			pDVJsonDebit->PayVirtualCardID		= strTempID.IsEmpty()?_T(""):GetGlobalApp()->FmtStr(_T("%s%s"),GetGlobalApp()->m_strVirtualCardPrefix,root["PayVirtualCardID"].asCString()); 
		}
		//调试用
	//	pDVJsonDebit->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://申请资源失败
			break;
		case -2://数据解析失败
			break;
		}
	}
	return pDVJsonDebit;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonDebitOperate::fnAnalysisStringForDebitCheck
//     
// 功能描述：解析字符串，获得扣款状态查询回馈数据
//     
// 输入参数：IN const CString& strResponseResult
// 输出参数：CDVJsonDebitCheck*
// 编写人员：ROCY
// 编写时间：2015年12月17日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck* CJsonDebitOperate::fnAnalysisStringForDebitCheck(IN const CString& strResponseResult)
{
	CDVJsonDebitCheck* pDVJsonDebitCheck = new CDVJsonDebitCheck();

	try
	{
		if(NULL == pDVJsonDebitCheck)
			throw -1;

		//进行数据格式转换
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//获取版本
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebitCheck->Version					= root["Version"].asCString();
		}
		//获取数据包类型
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebitCheck->PackageType				= root["PackageType"].asInt();
		}
		//获取响应状态代码
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebitCheck->ServerResponseCode		= root["Status"].asInt();
		}
		//获取响应状态描述
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebitCheck->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//获取服务器接收数据时间
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebitCheck->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//获取服务器响应数据时间
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebitCheck->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}
		//获取服务器实际扣款时间
		if(Json::stringValue == root["ServerDebitTime"].type())
		{
			pDVJsonDebitCheck->ServerDebitTime	= CTime(GetGlobalApp()->CString2time_t(root["ServerDebitTime"].asCString()));
		}
		//获取扣款详细状态代码
		if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
		{
			pDVJsonDebitCheck->DebitStatusCode			= root["ErrorCodeByPayPlatform"].asInt();
		}
		//获取扣款详细状态描述
		if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
		{
			pDVJsonDebitCheck->DebitStatusDesc			= root["ErrorDescByPayPlatform"].asCString();
		}
		//获取扣款订单号
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebitCheck->DebitOrderNumber			= root["DebitOrderNumber"].asCString();
		}
		//获取交易识别码
		if(Json::stringValue == root["PayIdentifier"].type())
		{
			pDVJsonDebitCheck->PayIdentifier		= root["PayIdentifier"].asCString();
			((CDVJsonBase*)pDVJsonDebitCheck)->PayIdentifier	= root["PayIdentifier"].asCString();
		}
		//获取支付凭证所属第三方支付平台类型
		if(Json::intValue == root["PayChannelType"].type())
		{
			pDVJsonDebitCheck->QRCodeType		= root["PayChannelType"].asInt();
		}

		//调试用
	//	pDVJsonDebitCheck->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://申请资源失败
			break;
		case -2://数据解析失败
			break;
		}
	}
	return pDVJsonDebitCheck;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonDebitOperate::fnAnalysisStringForDebitCancel
//     
// 功能描述：解析字符串，获得支付请求（撤单）服务端回馈数据
//     
// 输入参数：IN const CString& strResponseResult
// 输出参数：CDVJsonDebitCancel*
// 编写人员：ROCY
// 编写时间：2016年1月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel* CJsonDebitOperate::fnAnalysisStringForDebitCancel(IN const CString& strResponseResult)
{
	CDVJsonDebitCancel* pDVJsonDebitCancel = new CDVJsonDebitCancel();

	try
	{
		if(NULL == pDVJsonDebitCancel)
			throw -1;

		//进行数据格式转换
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//获取版本
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebitCancel->Version					= root["Version"].asCString();
		}
		//获取数据包类型
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebitCancel->PackageType				= root["PackageType"].asInt();
		}
		//获取响应状态代码
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebitCancel->ServerResponseCode		= root["Status"].asInt();
		}
		//获取响应状态描述
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebitCancel->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//获取服务器接收数据时间
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebitCancel->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//获取服务器响应数据时间
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebitCancel->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}

		//获取撤单的交易识别码
		if(Json::stringValue == root["PayIdentifier"].type())
		{
			pDVJsonDebitCancel->PayIdentifier	= root["PayIdentifier"].asCString();
			((CDVJsonBase*)pDVJsonDebitCancel)->PayIdentifier	= root["PayIdentifier"].asCString();
		}

		//获取撤单的订单号
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebitCancel->ServerReceiveDataTime		= root["DebitOrderNumber"].asCString();
		}

		//调试用
	//	pDVJsonDebitCancel->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://申请资源失败
			break;
		case -2://数据解析失败
			break;
		}
	}
	return pDVJsonDebitCancel;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonDebitOperate::fnAnalysisStringForDebitSecond
//     
// 功能描述：解析字符串，获得支付请求（二次处理）服务端回馈数据
//     
// 输入参数：IN const CString& strResponseResult
// 输出参数：CDVJsonDebit*
// 编写人员：ROCY
// 编写时间：2016年5月30日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：Http->PC
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebit* CJsonDebitOperate::fnAnalysisStringForDebitSecond(IN const CString& strResponseResult)
{
	CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();

	try
	{
		if(NULL == pDVJsonDebit)
			throw -1;

		//进行数据格式转换
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
			throw -2;
		strResult.ReleaseBuffer();
		//获取版本
		if(Json::stringValue == root["Version"].type())
		{
			pDVJsonDebit->Version					= root["Version"].asCString();
		}
		//获取数据包类型
		if(Json::intValue == root["PackageType"].type())
		{
			pDVJsonDebit->PackageType				= root["PackageType"].asInt();
		}
		//获取响应状态代码
		if(Json::intValue == root["Status"].type())
		{
			pDVJsonDebit->ServerResponseCode		= root["Status"].asInt();
		}
		//获取响应状态描述
		if(Json::stringValue == root["Message"].type())
		{
			pDVJsonDebit->ServerResponseResultDesc	= root["Message"].asCString();
		}
		//获取服务器接收数据时间
		if(Json::stringValue == root["ReceveTime"].type())
		{
			pDVJsonDebit->ServerReceiveDataTime		= root["ReceveTime"].asCString();
		}
		//获取服务器响应数据时间
		if(Json::stringValue == root["ResponseTime"].type())
		{
			pDVJsonDebit->ServerResponseDataTime	= root["ResponseTime"].asCString();
		}

		//获取扣款详细状态代码
		if(Json::intValue == root["ErrorCodeByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusCode			= root["ErrorCodeByPayPlatform"].asInt();
		}
		//获取扣款详细状态描述
		if(Json::stringValue == root["ErrorDescByPayPlatform"].type())
		{
			pDVJsonDebit->DebitStatusDesc			= root["ErrorDescByPayPlatform"].asCString();
		}
		//获取扣款订单号
		if(Json::stringValue == root["DebitOrderNumber"].type())
		{
			pDVJsonDebit->DebitOrderNumber		= root["DebitOrderNumber"].asCString();
		}
		//获取交易安全码
		if(Json::stringValue == root["TradeSecurityCode"].type())
		{
			pDVJsonDebit->TradeSecurityCode		= root["TradeSecurityCode"].asCString();
		}
		//获取支付平台类型
		if(Json::intValue == root["PayChannelType"].type())
		{
			pDVJsonDebit->ServerJudgeDebitIDType		= root["PayChannelType"].asInt();
		}
		//获取支付平台类型
		if(Json::stringValue == root["PayVirtualCardID"].type())
		{
			CString strTempID(root["PayVirtualCardID"].asCString());
			pDVJsonDebit->PayVirtualCardID		= strTempID.IsEmpty()?_T(""):GetGlobalApp()->FmtStr(_T("%s%s"),GetGlobalApp()->m_strVirtualCardPrefix,root["PayVirtualCardID"].asCString()); 
		}
		//调试用
	//	pDVJsonDebit->ServerResponseCode		= HTTP_RC_DEBIT_SUCCESS;//HTTP_RC_DEBIT_NEED_PASSWORD;//HTTP_RC_DEBIT_SUCCESS;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://申请资源失败
			break;
		case -2://数据解析失败
			break;
		}
	}
	return pDVJsonDebit;
}