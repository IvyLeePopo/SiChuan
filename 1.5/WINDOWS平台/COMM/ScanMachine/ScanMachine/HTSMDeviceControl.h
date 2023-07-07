/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：HTSMProtocol.h
 * 摘    要：扫码器内部通讯协议解析类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年2月22日
 */
#pragma once
#include "HTSMHoneywell3310g.h"

#include <list>
#include "ProtocolHelper.h"

#define MAKEINTBIG(fir,sec,thir,four) ( MAKELONG(MAKEWORD((four),(thir)),MAKEWORD((sec),(fir) )) )

#define SWITCHTO_V2_0_SYN  10
#define SWITCHTO_V2_0_SYN_ACK 11
#define SWITCHTO_V2_0_ACK  12
#define SWITCHTO_V1_5_SYN  20
#define SWITCHTO_V1_5_SYN_ACK  21
#define SWITCHTO_V1_5_ACK  22

class CHTSMDeviceControl
{
public:
//内部常量定义
	enum SM_PROTOCOL_CONST
	{
		SM_PROTOCOL_HEADER	= 0xDF,	//协议起始符
		SM_PROTOCOL_END		= 0xFD,	//协议休止符

		SM_PROTOCOL_CONVERT_CHAR		= 0x00,	//协议转译公共字符

		SM_PROTOCOL_HEAND_CONVERT_CHAR1	= 0xDE,	//协议起始符转译字符1
		SM_PROTOCOL_HEAND_CONVERT_CHAR2	= 0x01,	//协议起始符转译字符2

		SM_PROTOCOL_END_CONVERT_CHAR1	= 0xFC,	//协议休止符转译字符1
		SM_PROTOCOL_END_CONVERT_CHAR2	= 0x01,	//协议休止符转译字符2

		/**************************自定义协议帧类型*****************************/
		SM_PROTOCOL_FRAME_TYPE_UNKNOW	= 0x00,	//未知数据帧

		SM_PROTOCOL_FRAME_TYPE_A1		= 0xA1,	//发送写配置指令
		SM_PROTOCOL_FRAME_TYPE_A2		= 0xA2, //发送读配置指令
		SM_PROTOCOL_FRAME_TYPE_A3		= 0xA3,	//发送IO状态设置指令
		SM_PROTOCOL_FRAME_TYPE_A4		= 0xA4,	//发送IO状态读取指令
		SM_PROTOCOL_FRAME_TYPE_A5		= 0xA5, //发送自定义指令
		SM_PROTOCOL_FRAME_TYPE_A6		= 0xA6, //发送设备复位指令

		SM_PROTOCOL_FRAME_TYPE_A7		= 0xA7, //数据透传
		SM_PROTOCOL_FRAME_TYPE_A8		= 0xA8, //add 2017-12 
		SM_PROTOCOL_FRAME_TYPE_A9		= 0xA9, //add 2017-12
		SM_PROTOCOL_FRAME_TYPE_AD		= 0xAD, //add 2017-12
		SM_PROTOCOL_FRAME_TYPE_AE		= 0xAE, //add 2017-12


		SM_PROTOCOL_FRAME_TYPE_B0		= 0xB0,	//初始化信息帧
		SM_PROTOCOL_FRAME_TYPE_B1		= 0xB1, //心跳帧
		SM_PROTOCOL_FRAME_TYPE_B2		= 0xB2, //数据上传信息帧
		SM_PROTOCOL_FRAME_TYPE_B3		= 0xB3, //数据透传信息帧
		SM_PROTOCOL_FRAME_TYPE_B4		= 0xB4, //ref to doc
		SM_PROTOCOL_FRAME_TYPE_B5		= 0xB5, //ref to doc

		
		SM_PROTOCOL_FRAME_TYPE_C0		= 0xC0, //指令应答帧（标记上位机发送的A1~A8）指令的接收情况
	
		SM_PRPTOCOL_LIGHT_ALL_OFF		= 0x03, //灭灯
		SM_PRPTOCOL_LIGHT_SCAN			= 0x01, //提示正在扫码
		SM_PRPTOCOL_LIGHT_SUCCESS		= 0x00,	//提示扫码成功
		SM_PRPTOCOL_LIGHT_FAIL			= 0x02,	//提示扫码失败
	};

public:
	CHTSMDeviceControl(void);
	~CHTSMDeviceControl(void);
//Interfaces
public:
	//设置参数
	void SetParam(IN const UINT& nCallThreadID){m_nCallerThreadID = nCallThreadID;}
	//获取内挂设备协议解析类
	CHTSMDeviceBaseIF* GetInnerDeviceProtocolClass(void) {return m_pSMIF;}
	//初始化设备
	const BOOL InitInnerDevice(IN OUT CString& strLog,IN const int& iInnerDeviceType=1);
	//接收到的数据长度是否允许进行数据帧校验
	const BOOL IsAllowAnalyseReceiveDataOnBytesNum(IN const DWORD& dwHadReceiveDataSize) const ;
	//解析接收到的数据，判断是否符合协议
	void AnalyseReciveData(IN OUT LPBYTE pData,IN const int& iDataSize,IN OUT LPBYTE& pBegin,IN OUT LPBYTE& pEnd);
	//add 2017-10 处理多个数据帧的情况
	void AnalyseReciveData_list(IN OUT LPBYTE pData,IN const int& iDataSize,std::list<LPBYTE>& pBegin_list,std::list<LPBYTE>& pEnd,int& deal_size);
	//处理接收数据
	const BOOL ProcessReceiveFrameData(IN const LPBYTE lpData,IN const DWORD& dwSize, IN OUT int& iFrameType);

	//读取串口板配置
	const CString ReadConfigInfoFromBoard(IN const int& iType = 0x00);
	//更新串口板配置
	const CString UpdateConfigInfoFromBoard(IN const CString& strConfigInfo);
	//设置亮灯模式
	const CString SetLightMode(IN const int& iLightIndex);
	//查询亮灯状态
	const CString GetLightMode(void);

	//新增设置扫码器显示
	const CString SetScanDisplay(tagScanDisplay	sDisplay);
	const CString ConstructFrameA1(tagMMIMedia	sMedia);

	//新增发送回馈指令
	const CString SendCmdResponse(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame);

	const CString SetTransData(const tagTransmissionData& trans_data);

	//add 2017-11 zhuyabing
	const CString ConstructTTS(const std::string& msg);

	//add 2017-12 zhuyabing
	const CString ConstructUpgradeControl(const FirmwareUpgradeControl& upgradOption);
	const CString ConstructUpgradeData(const FirmwareUpgradeTransfer& upgradeData);
	const CString ConstructFreeDisplayData(const FreeDisplayData& data);
	const CString ConstructFreeDisplayStyle(const FreeDisplayStyle& style);
	
	const CString ConstructTerminalStatusQuery();
	const CString ConstructSwitchSync(const int& status);

	//复位串口板
	const CString ResetBoard(void);
	//发送自定义指令
	const CString SendUserDefineCmd(IN const CString& strCmdContent,IN const BYTE& byDeviceIndex=0);

	bool ParseSpecialCommond01(const LPBYTE lpData, const DWORD& dwSize,  int& result);
	bool ParseSpecialCommond02(const LPBYTE lpData, const DWORD& dwSize,  int& result);
//Inner Fuctions
private:
	//构造A1帧 -- 写主板配置信息
	const CString ConstructFrameA1(IN const CString& strParam);
	const CString ConstructFrameA1(IN const int& iType);

	//构造A2帧 -- 读主板配置信息
	const CString ConstructFrameA2(void);
	//构造A3帧 -- 亮灯状态控制
	const CString ConstructFrameA3(IN const int& iLightIndex);
	//构造A4帧 -- 亮灯状态读取
	const CString ConstructFrameA4(void);
	//构造A5帧 -- 发送扫码设备指令
	const CString ConstructFrameA5(IN const LPBYTE lpData,IN const DWORD& dwSize);

	//新增用于设置智能硬件显示A5指令
	const CString ConstructFrameA5(tagScanDisplay sDisplay);

	//构造A8帧 -- 复位主板
	const CString ConstructFrameA6(void);
	//构造C0帧 -- 指令应答帧
	const CString ContructFrameC0(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame);

	const CString ConstructFrameA7(const tagTransmissionData& trans_data);

	const CString ConstructFrameA8(const FirmwareUpgradeControl& upgradOption);
	const CString ConstructFrameA9(const FirmwareUpgradeTransfer& upgradeData);
	

	//根据接收到的数据判断数据帧
	const int GetDataFrameType(IN const LPBYTE lpFrame,IN const DWORD& dwSize,IN OUT CString& strReceiveFrameType);
	//数据帧校验
	const BOOL VerifyFrameData(IN const LPBYTE pData,IN const DWORD& dwSize);

	//数据转译
	void ConvertData(IN OUT LPBYTE lpData,IN OUT DWORD& dwSize,IN const BOOL& bSendState);
	//获取校验码
	const WORD GetVerifyCode(IN const BYTE& byFrameType,IN const WORD& wSize,IN const LPBYTE lpData) const;
	
	//处理B0帧
	void ProcessFrameB0(IN const LPBYTE lpData,IN const DWORD& dwSize);
	//处理B1帧
	void ProcessFrameB1(IN const LPBYTE lpData,IN const DWORD& dwSize);
	//处理B2帧
	void ProcessFrameB2(IN const LPBYTE lpData,IN const DWORD& dwSize);
	void ProcessFrameB3(IN const LPBYTE lpData,IN const DWORD& dwSize);
	void ProcessFrameB4(IN const LPBYTE lpData,IN const DWORD& dwSize);
	void ProcessFrameB5(IN const LPBYTE lpData,IN const DWORD& dwSize);
	//处理C0帧
	void ProcessFrameC0(IN const LPBYTE lpData,IN const DWORD& dwSize);

	//设备内部协议处理函数 -- 处理初始化指令返回数据
	void fnInnerDeviceProtocolProcessResultForInit(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//设备内部协议处理函数 -- 处理版本读取指令返回数据
	void fnInnerDeviceProtocolProcessResultForVer(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//设备内部协议处理函数 -- 处理开始扫码指令返回数据
	void fnInnerDeviceProtocolProcessResultForStartScan(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//设备内部协议处理函数 -- 处理停止扫码指令返回数据
	void fnInnerDeviceProtocolProcessResultForStopScan(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//设备内部协议处理函数 -- 处理设备心跳指令返回数据
	void fnInnerDeviceProtocolProcessResultForHeart(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//设备内部协议处理函数 -- 处理重设出厂默认环境参数指令返回数据
	void fnInnerDeviceProtocolProcessResultForDefaultFactory(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//设备内部协议处理函数 -- 处理重设用户自定义环境指令返回数据
	void fnInnerDeviceProtocolProcessResultForDefaultPersion(IN const BYTE& byDeviceID,IN const CString& strFormatData);

//Attributes
public:
	//扫码器状态描述类
	CSMInnerDeviceInfo	m_cInnerDeviceInfo;
	//串口板信息 -- 上次收到心跳的时间
	time_t	m_tLastHeartTimeForBoard;
	//上次发送的数据信息(仅仅在A5帧有效)
	CString m_strLastSendFrameData;
	//标记是否需要重发数据
	BOOL	m_bNeedReSendLastData;
private:
	//设备协议解析类
	CHTSMDeviceBaseIF* m_pSMIF;
	//调用者线程ID
	UINT	m_nCallerThreadID;
	
	//串口板信息 -- 是否已经检测到数据帧标记
	BOOL	m_bIsHaveFrameFlag;
	
	//内部设备信息 -- 扫码指令发送时间
	DWORD		m_dwBeginScanTime;
	SYSTEMTIME	m_stForBeginScan;
	//内部设备信息 -- 扫码指令获得回馈时间
	SYSTEMTIME	m_stForEndScan;
};
