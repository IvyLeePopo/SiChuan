/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：JsonHeartOperate.cpp
 * 摘    要：心跳数据结构体描述，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年12月14日
 */
#include "StdAfx.h"
#include "JsonHeartOperate.h"

CJsonHeartOperate::CJsonHeartOperate(void)
{
}

CJsonHeartOperate::~CJsonHeartOperate(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonHeartOperate::FormatSendData2Json
//     
// 功能描述：将接收到的数据结构转化为标准的JSON格式 -- 心跳
//     
// 输入参数：IN const CDVJsonHeart* pDVJsonHeart
// 输出参数：CString
// 编写人员：ROCY
// 编写时间：2015年12月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
 CString CJsonHeartOperate::FormatSendData2Json(IN const CDVJsonHeart* pDVJsonHeart)
 {
	 CString strReturn;
	 return strReturn;
 }

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CJsonHeartOperate::FormatResponseResult2Class
//     
// 功能描述：将服务器返回的JSON格式转化为标准数据结构类
//     
// 输入参数：IN const CString& strResponseResult
//         ：IN const int& iDataType
// 输出参数：CDVJsonBase*
// 编写人员：ROCY
// 编写时间：2015年12月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonBase* CJsonHeartOperate::FormatResponseResult2Class(IN const CString& strResponseResult,IN const int& iDataType)
{
	CDVJsonHeart* pDVJsonHeart = new CDVJsonHeart();
	try
	{
		if(NULL == pDVJsonHeart)
			throw -1;

		//进行数据格式转换
/*	
		Json::Reader reader;
		Json::Value  root;
		CString strResult(strResponseResult);
		if(!reader.parse(strResult.GetBuffer(0),root))
		{
			strResult.ReleaseBuffer();
			throw -2;
		}
		
		if(Json::nullValue != root.type())
			throw -3;

		//获取版本
		if(Json::nullValue == root["Version"].type())
			throw -4;
		pDVJsonEntryQuery->Version					= root["Version"].asCString();
		//获取数据包类型
		if(Json::nullValue == root["PackageType"].type())
			throw -4;
		pDVJsonEntryQuery->PackageType				= root["PackageType"].asInt();
		//获取响应状态代码
		if(Json::nullValue == root["Status"].type())
			throw -4;
		pDVJsonEntryQuery->ServerResponseCode		= root["Status"].asInt();
		//获取响应状态描述
		if(Json::nullValue == root["Message"].type())
			throw -4;
		pDVJsonEntryQuery->ServerResponseResultDesc = root["Message"].asCString();
		//获取服务器接收数据时间
		if(Json::nullValue == root["ReceveTime"].type())
			throw -4;
		pDVJsonEntryQuery->ServerReceiveDataTime	= root["ReceveTime"].asCString();
		//获取服务器响应数据时间
		if(Json::nullValue == root["ResponseTime"].type())
			throw -4;
		pDVJsonEntryQuery->ServerResponseDataTime	= root["ResponseTime"].asCString();
*/

		/********************************************************************调试填充********************************************************************/
		
		//调试填充
		pDVJsonHeart->ServerResponseCode		= HTTP_RC_DATA_VALID;
		pDVJsonHeart->ServerReceiveDataTime		= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonHeart->ServerResponseDataTime	= GetGlobalApp()->GetCurrentTimeString(1);
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
	return pDVJsonHeart;
}
