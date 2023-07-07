/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: WorkingPara.cpp
* ժ   Ҫ:  ����ϵͳ����ʱ��ʹ�õĲ���
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-07
* �޸ļ�¼:
*/
#include "stdafx.h"
#include "fs_funcs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////
// ��������:GetConfigPath
//
// ��������:��ȡ�����ļ��ĵ�ַ����DLL��ͬ·��
//
// �������:
// �������:std::string 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
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
// ��������:SystemParaComRetryCount
//
// ��������:�����ط�����
//
// �������:
// �������:size_t 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
size_t SystemParaComRetryCount()
{
	return 3;
}
//////////////////////////////////////////////////////////////
// ��������:SystemParaComOvertime
//
// ��������:����Ĭ�ϳ�ʱ���ã���λ����
//
// �������:
// �������:size_t 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
size_t SystemParaComOvertime()
{
	return 2;
}

//////////////////////////////////////////////////////////////
// ��������:SystemParaComPort
//
// ��������:��ȡ�����ļ��еĴ��ں�
//
// �������:
// �������:size_t 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
size_t SystemParaComPort()
{
	int info_ = ::GetPrivateProfileInt("SerialParam", "Port", -1, GetConfigPath().c_str());
	return info_;
}

//////////////////////////////////////////////////////////////
// ��������:SystemParaComBaud
//
// ��������:��ȡ���ڲ�����
//
// �������:
// �������:int 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
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