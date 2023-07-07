/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：SMHoneywell3301g.h
 * 摘    要：霍尔韦尔3310g扫码器协议描述类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
#pragma once
#include "HTSMDeviceBaseIF.h"

class CHTSMDeviceHW3310g :
	public CHTSMDeviceBaseIF
{
public:
	CHTSMDeviceHW3310g(IN const int& iSMType);
	~CHTSMDeviceHW3310g(void);

	//重置为出厂参数
	const CString	ResetDefaultParam(void);
	//重置为用户默认参数
	const CString	ResetPrivateParam(IN OUT BYTE& byDataConvertType);
	//开启用户设置操作流程
	const CString	BeginSetPrivateParam(void);
	//递交用户设置参数(保存)
	const CString	SubmitPrivateParam(void);
	//获取扫码器系统信息
	const CString	GetSysInfo(void);
	//串口通讯设置
	const CString	SetRS232CommMode(IN const int& iRate = 9600,IN const int& iParityBit = 0,IN const int& iStopBit = 1,IN const int& iDataBit = 8);
	//检测设备状态
	const int	CheckDeviceStatus(IN const CString& strState);
	//发送心跳指令(实际就是一个简单的查询指令)
	const CString GetHeartCmd(void);
	//解析版本信息字符串
	const BOOL	AnalysisVersionString(IN const CString& strVersionString);
	/****************************硬件设置*******************************/
	//设置扫描手机模式
	virtual const CString	SetScanPhoneReadMode(void);
	//设置扫码补光模式
	const CString	SetSupplementalLightMode(IN const int& iLightMode = SM_FUN_SUPPORT_LIGHT_FORBID);
	//设置对焦模式
	const CString	SetFocusingMode(IN const int& iMFocusode = SM_FUN_FOCUS_NONE);
	//设置是否允许解码完毕后发声
	const CString	SetAllowDecodeVoice(IN const BOOL& bAllowVoice = TRUE);
	//设置解码完毕后的声音种类
	const CString	SetDecodeVoiceType(IN const int& iVoiceType = SM_FUN_VOICLE_NORMAL);

	/****************************扫码模式设置*******************************/
	//设置扫码超时时间
	const CString	SetDelayTime(IN const int& iDealyTime = SM_DEFAULT_WAIT_SCAN_TIME);
	//设置延时模式(完全延时：无论是否读取到数据都延时指定时间；非完全延时：有数据则返回)
	const CString	SetDelayTimeMode(IN const int& iDelayMode = SM_FUN_DELAY_WAIT);
	//设置扫描模式
	const CString	SetScanMode(IN const int& iScanMode = SM_FUN_SCAN_CMD);
	//设置灵敏度
	const CString	SetSensitivityRate(IN const int& iSensitivityRate = SM_FUN_SENSITIVITY_MID);
	//设置灵敏度数值
	const CString	SetSensitivityValue(IN const int& iSensitivityValue);

	/****************************数据筛选设置*******************************/

	//只允许扫描QR码
	const CString	AllowOnlyScanQRCode(void);
	//重设QR码的参数为默认
	const CString	ResetQRCodeParam(void);
	//格式化读取回来的数据
	CString	FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize);
	//检查命令执行结果是否正确
	BOOL CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize);

	//增加前缀
	const CString AddPrefix(IN const CString& strSuffix);
	//增加后缀
	const CString AddSuffix(IN const CString& strSuffix);

	/****************************实时操作*******************************/
	//开启扫码操作
	const CString	Start(void);
	//停止扫码操作
	const CString	Stop(void);
	//发出声音
	const CString	MakeVoice(void);
	
//Inner Function
protected:
	//开启或关闭设置码功能
	const CString	AllowFunctionCode(IN const BOOL& bAllow);
	//将命令数据（可视字符串）转化成相应ASCII码的16进制格式
	const CString	FormatCommandData(IN const CString& strCmd);
	//3310G命令头
	CString	m_strCommandHead;

	int m_iScanCodeType;
};
