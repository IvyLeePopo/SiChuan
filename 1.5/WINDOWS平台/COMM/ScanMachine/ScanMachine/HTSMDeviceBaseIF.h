/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ScanMachineIF.h
 * 摘    要：扫码器功能定义基本类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
#pragma once

class CHTSMDeviceBaseIF
{
//内部枚举定义
public:
	enum SM_FUN_TYPE
	{
		SM_FUN_SUPPORT_LIGHT_ALLOW	= 1,	//扫码过程允许补光
		SM_FUN_SUPPORT_LIGHT_FORBID = 2,	//扫码过程不允许补光

		SM_FUN_FOCUS_LIGHT			= 1,	//对焦有灯光
		SM_FUN_FOCUS_FLICKER		= 2,	//对焦灯光闪烁
		SM_FUN_FOCUS_NONE			= 3,	//对焦无灯光

		SM_FUN_VOICLE_NORMAL		= 1,	//正常发声
		SM_FUN_VOICE_BIG			= 2,	//较大音量

		SM_FUN_SCAN_CMD				= 1,	//命令触发
		SM_FUN_SCAN_AUTO			= 2,	//自动触发
		SM_FUN_SCAN_MANUAL			= 3,	//手动触发

		SM_FUN_DELAY_WAIT			= 1,	//扫码堵塞等待
		SM_FUN_DELAY_NO_WAIT		= 2,	//扫码不堵塞等待

		SM_FUN_SENSITIVITY_LOW		= 1,	//灵敏度低
		SM_FUN_SENSITIVITY_MID		= 2,	//灵敏度正常
		SM_FUN_SENSITIVITY_HIGH		= 3,	//灵敏度高

		SM_DEFAULT_WAIT_SCAN_TIME   = 30000,//扫码等待返回时间,默认30秒
	};

public:
	CHTSMDeviceBaseIF();
	CHTSMDeviceBaseIF(IN const int& iSMType);
	~CHTSMDeviceBaseIF(void);
//Interface
public:
	/****************************系统基本设置*******************************/

	//重置为出厂参数
	virtual const CString ResetDefaultParam(void)=0;
	//重置为用户默认参数
	virtual const CString	ResetPrivateParam(IN OUT BYTE& byDataConvertType)=0;
	//开启用户设置操作流程
	virtual const CString	BeginSetPrivateParam(void)=0;
	//递交用户设置参数(保存)
	virtual const CString	SubmitPrivateParam(void)=0;
	//获取扫码器系统信息
	virtual const CString	GetSysInfo(void)=0;
	//心跳命令(发送一个查询)
	virtual const CString GetHeartCmd(void)=0;
	//串口通讯设置
	virtual const CString	SetRS232CommMode(IN const int& iRate = 9600,IN const int& iParityBit = 0,IN const int& iStopBit = 1,IN const int& iDataBit = 8)=0;
	//检测设备状态
	virtual const int	CheckDeviceStatus(IN const CString& strState)=0;
	//获取扫码器类型
	virtual const int	GetSMType(void) const {return m_iSMType;}
	//解析版本信息字符串
	virtual const BOOL	AnalysisVersionString(IN const CString& strVersionString)=0;
	//是否需要读取版本信息
	const BOOL IsNeedReadVersionInfo(void);

	/****************************硬件设置*******************************/
	//设置扫描手机模式
	virtual const CString	SetScanPhoneReadMode(void) = 0;
	//设置扫码补光模式
	virtual const CString	SetSupplementalLightMode(IN const int& iLightMode = SM_FUN_SUPPORT_LIGHT_FORBID)=0;
	//设置对焦模式
	virtual const CString	SetFocusingMode(IN const int& iMFocusode = SM_FUN_FOCUS_NONE)=0;
	//设置是否允许解码完毕后发声
	virtual const CString	SetAllowDecodeVoice(IN const BOOL& bAllowVoice = TRUE)=0;
	//设置解码完毕后的声音种类
	virtual const CString	SetDecodeVoiceType(IN const int& iVoiceType = SM_FUN_VOICLE_NORMAL)=0;

	/****************************扫码模式设置*******************************/
	//设置扫码超时时间
	virtual const CString	SetDelayTime(IN const int& iDealyTime = SM_DEFAULT_WAIT_SCAN_TIME)=0;
	//设置延时模式(完全延时：无论是否读取到数据都延时指定时间；非完全延时：有数据则返回)
	virtual const CString	SetDelayTimeMode(IN const int& iDelayMode = SM_FUN_DELAY_WAIT)=0;
	//设置扫描模式
	virtual const CString	SetScanMode(IN const int& iScanMode = SM_FUN_SCAN_CMD)=0;
	//设置灵敏度
	virtual const CString	SetSensitivityRate(IN const int& iSensitivityRate = SM_FUN_SENSITIVITY_MID)=0;
	//设置灵敏度数值
	virtual const CString	SetSensitivityValue(IN const int& iSensitivityValue)=0;

	/****************************数据筛选设置*******************************/
	
	//只允许扫描QR码
	virtual const CString	AllowOnlyScanQRCode(void)=0;
	//重设QR码的参数为默认
	virtual const CString	ResetQRCodeParam(void)=0;
	
	//格式化读取回来的数据
	virtual CString FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)=0;

	//检查命令执行结果是否正确
	virtual BOOL CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)=0;

	//获取上次执行的命令类型
	const int GetLastCommandType(void) const {return m_iLastCommandType;}

	//重置上次的命令类型
	void ResetLastCommandType(void) {m_iLastCommandType = 0;}
	
	//增加前缀
	const CString AddPrefix(IN const CString& strSuffix);
	//增加后缀
	const CString AddSuffix(IN const CString& strSuffix);

	/****************************实时操作*******************************/
	//开启扫码操作
	virtual const CString	Start(void) = 0;
	//停止扫码操作
	virtual const CString	Stop(void) = 0;
	//发出声音
	virtual const CString	MakeVoice(void) = 0;
//Inner Function
protected:
	//开启或关闭设置码功能
	virtual const CString	AllowFunctionCode(IN const BOOL& bAllow) = 0;
//Attributes
protected:
	int		m_iSMType;				//扫码器类型(1,新大陆NLS-FM420)
	DWORD	m_dwCodeLen;			//扫码器读取回来规格化的数据的位数
	DWORD	m_iLastCommandType;		//1,初始化;2,读版本;3,扫码触发;4,停止扫码;5,心跳;6,重置出厂;7,重置用户默认
	CString m_strProductName;		//内部产品全称
	CString m_strBootRevision;		//启动版本信息
	CString	m_strSerialNumber;		//产品序列号
	CString m_strFirmwareVersion;	//固件版本类型
};
