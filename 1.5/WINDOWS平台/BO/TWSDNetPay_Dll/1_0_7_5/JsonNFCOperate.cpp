/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonNFCOperate.cpp
 * 摘    要：NFC的JSON数据格式操作类
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月20日
 */
#include "StdAfx.h"
#include "JsonNFCOperate.h"
#include "TWSDNetPay_Dll.h"

CJsonNFCOperate::CJsonNFCOperate(void)
{
}

CJsonNFCOperate::~CJsonNFCOperate(void)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonNFCOperate::FormatSendData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- NFC签到
//     
// 输入参数：IN const CDVJsonHeart* pDVJsonHeart
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2017年09月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
 CString CJsonNFCOperate::FormatSendData2Json(IN const CDVJsonNFCSignIn* pDVJsonNFCSignIn)
{
	CString strReturn;
	if(NULL != pDVJsonNFCSignIn)
	{
		// 表示整个 json 对象
		Json::Value root;
		/***********************************************************构造JSAO数据结构***********************************************************/
		
		//标识版本
		root["Version"]				= Json::Value(pDVJsonNFCSignIn->Version);
		//标识数据类型
		root["PackageType"]			= Json::Value(pDVJsonNFCSignIn->PackageType);
		//安全认证
		root["Token"]				= Json::Value(pDVJsonNFCSignIn->Token);
		//标识省份
		root["ProvinceID"]			= Json::Value(pDVJsonNFCSignIn->ProvinceID);
		//递交次数
		root["SubmitCount"]			= Json::Value(pDVJsonNFCSignIn->SubmitCount);
		//递交时间
		root["SubmitTime"]			= Json::Value(pDVJsonNFCSignIn->SubmitTime);
		//NFC终端模块编码
		root["Terminal"]			= Json::Value(pDVJsonNFCSignIn->NFCTerminal);
		//NFC终端模块硬件序号
		root["Sn"]					= Json::Value(pDVJsonNFCSignIn->NFCSerialNo);
		//签到数据
		root["Data"]				= Json::Value(pDVJsonNFCSignIn->NFCSignData);
		//特微产品硬件终端序号
		root["TWProductSerialNo"]	= Json::Value(pDVJsonNFCSignIn->TWProductSerialNo);

		//转化为字符串
		Json::FastWriter fast_writer;
		strReturn.Format(_T("%s"),fast_writer.write(root).c_str());
	}
	 return strReturn;
 }

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonNFCOperate::FormatResponseResult2Class
//     
// 功能描述：将服务器返回的JSON格式转化为标准数据结构类
//     
// 输入参数：IN const CString& strResponseResult
//         ：IN const int& iDataType
// 输出参数：CDVJsonBase*
// 编写人员：ROCY
// 编写时间：2017年09月18日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonNFCOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonNFCSignIn* pDVJsonNFCSignIn = new CDVJsonNFCSignIn();
	try
	{
		if(NULL == pDVJsonNFCSignIn)
			throw -1;

		//进行数据格式转换
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
		{
			strResult.ReleaseBuffer();
			throw -2;
		}
		
		if(Json::nullValue == root.type())
			throw -3;

		//获取版本
		if(Json::nullValue == root["Version"].type())
			throw -4;
		pDVJsonNFCSignIn->Version					= root["Version"].asCString();
		//获取数据包类型
		if(Json::nullValue == root["PackageType"].type())
			throw -4;
		pDVJsonNFCSignIn->PackageType				= root["PackageType"].asInt();
		//获取响应状态代码
		if(Json::nullValue == root["Status"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerResponseCode		= root["Status"].asInt();

		//获取服务器回馈的签到数据
		if(Json::nullValue == root["ReData"].type())
			throw -4;
		CString strTemp = root["ReData"].asCString();
		::GetGlobalApp()->Base64ToHex(strTemp,pDVJsonNFCSignIn->NFCSignData);
			
		//获取响应状态描述
		if(Json::nullValue == root["Message"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerResponseResultDesc = root["Message"].asCString();
		//获取服务器接收数据时间
		if(Json::nullValue == root["ReceveTime"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerReceiveDataTime	= root["ReceveTime"].asCString();
		//获取服务器响应数据时间
		if(Json::nullValue == root["ResponseTime"].type())
			throw -4;
		pDVJsonNFCSignIn->ServerResponseDataTime	= root["ResponseTime"].asCString();

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
	//返回解析结果
	return pDVJsonNFCSignIn;
}
