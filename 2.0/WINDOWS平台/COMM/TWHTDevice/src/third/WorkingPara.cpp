/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: WorkingPara.cpp
* 摘   要:  定义系统运行时候使用的参数
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-07
* 修改记录:
*/
#include "stdafx.h"
#include "fs_funcs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////
// 函数名称:GetConfigPath
//
// 功能描述:获取配置文件的地址，和DLL相同路径
//
// 输入参数:
// 输出参数:std::string 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string GetConfigPath()
{
	char modePath[300] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);
	std::string pathStr = modePath;
	size_t pos = pathStr.find_last_of('\\');
	if (pos == std::string::npos)
	{
		return "";
	}
	pathStr = pathStr.substr(0, pos) + "\\HTSMConfig.ini";
	return pathStr;
}

//////////////////////////////////////////////////////////////
// 函数名称:SystemParaComRetryCount
//
// 功能描述:串口重发次数
//
// 输入参数:
// 输出参数:size_t 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
size_t SystemParaComRetryCount()
{
	return 3;
}
//////////////////////////////////////////////////////////////
// 函数名称:SystemParaComOvertime
//
// 功能描述:串口默认超时设置，单位：秒
//
// 输入参数:
// 输出参数:size_t 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
size_t SystemParaComOvertime()
{
	return 2;
}

//////////////////////////////////////////////////////////////
// 函数名称:SystemParaComPort
//
// 功能描述:获取配置文件中的串口号
//
// 输入参数:
// 输出参数:size_t 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
size_t SystemParaComPort()
{
	int info_ = ::GetPrivateProfileInt("SerialParam", "Port", -1, GetConfigPath().c_str());
	return info_;
}

//////////////////////////////////////////////////////////////
// 函数名称:SystemParaComBaud
//
// 功能描述:获取串口波特率
//
// 输入参数:
// 输出参数:int 
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
int SystemParaComBaud()
{
	int info_ = ::GetPrivateProfileInt("SerialParam", "BaudRate", 9600, GetConfigPath().c_str());
	return info_;
}

bool SystemParaEnableTransferImage()
{
	static bool info_ = ::GetPrivateProfileInt("SMControl", "TransferImage", 0, GetConfigPath().c_str());
	return info_;
}
bool SystemParaForceImageSize()
{
	static bool info_ = ::GetPrivateProfileInt("SMControl", "CheckImageSize", 0, GetConfigPath().c_str());
	return info_;
}

int SystemParaImageMaxSize()
{
	static int info_ = ::GetPrivateProfileInt("SMControl", "MaxImageSize", 1024, GetConfigPath().c_str());
	return info_;
}

size_t SystemParaComInBufferSize()
{
	return 2048;
}
size_t SystemParaComOutBufferSize()
{
	return 2048;
}


std::string SystemParaDBPath()
{
	char modePath[300] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);
	std::string pathStr = modePath;
	size_t pos = pathStr.find_last_of('\\');
	if (pos == std::string::npos)
	{
		return "";
	}
	pathStr = pathStr.substr(0, pos) + "\\TWBack.db";
	return pathStr;
}