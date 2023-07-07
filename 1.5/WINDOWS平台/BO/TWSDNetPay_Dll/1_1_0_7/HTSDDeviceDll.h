/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：HTSDDeviceDll.h
 * 摘    要：智能硬件接口动态库加载类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年7月24日
 */
#pragma once

class CHTSDDeviceDll
{
//函数定义
public:
	//初始化硬件
	typedef BOOL	(WINAPI  *defIF_InitDev)(IN const UINT& nThreadID,IN const HWND& hWnd,IN const UINT& nNotifyMsgID,IN const int& iProvinceID,IN const int& iOverTime);
	//清理资源
	typedef BOOL	(WINAPI  *defIF_Destroy)(void);
	//获取上一次错误信息
	typedef CString (WINAPI  *defIF_GetLastErrorDesc)(void);
	//执行启动获取数据操作
	typedef BOOL	(WINAPI  *defIF_StartGetData)(IN const int& iOvertime);
	//执行停止获取数据操作
	typedef BOOL	(WINAPI  *defIF_StopGetData)(IN const int& iChangeMMI);
	//执行检测硬件状态操作
	typedef BOOL	(WINAPI  *defIF_CheckDeviceStatus)(IN OUT DWORD& dwState);
	//获取数据结果
	typedef BOOL	(WINAPI  *defIF_GetData)(IN OUT CString& strData,IN OUT int& iDataType,IN OUT int& iSrcType);
	//权限认证操作
	typedef	BOOL	(WINAPI  *defIF_Authority)(IN const CString& strData);
	//获取固件版本信息
	typedef	BOOL	(WINAPI  *defIF_GetDeviceFirewareVer)(IN OUT CString& strXML);
	//获取显示内容信息
	typedef	BOOL	(WINAPI  *defIF_GetDisplayContext)(IN OUT int&, CString&);
	//设置显示内容信息
	typedef	BOOL	(WINAPI  *defIF_SetDisplayContext)(IN const int&,IN const CString& strXML);
	//执行数据下发操作
	typedef BOOL	(WINAPI  *defIF_IssuedData)(IN const CString& strXML);
	//执行控制IO操作
	typedef BOOL	(WINAPI  *defIF_SetIOState)(IN const int& iIOState);


	//add 2017-12
	typedef BOOL	(WINAPI  *defIF_UpgradeFirware)(IN const CString& strJson);
	typedef BOOL	(WINAPI  *defIF_IsUpgrading)();
	typedef	BOOL	(WINAPI  *defIF_RealFirmwareVer)(IN OUT CString& strXML);
	typedef	BOOL	(WINAPI  *defIF_RealSlaveFirmwareVer)(IN OUT CString& strXML);

	typedef bool (WINAPI *defIF_TerminalSystemVersionStatus)(int& status);
	typedef bool (WINAPI *defIF_ExchangeTerminalSystem)(const int& targetSystemVersion);
public:
	CHTSDDeviceDll(void);
	~CHTSDDeviceDll(void);
public:
	//初始化动态库
	const BOOL Init(IN const CString& strDllFilePath);
	//获取动态库加载状态
	inline const BOOL GetDllInitState(void){return m_bInitDll;}
	//获取组件初始化状态
	inline const BOOL GetDevInitState(void){return m_bInitDev;}
//Operations
public:
	//组件初始化
	BOOL InitDev(IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const int& iProvinceID,IN const int& iOverTime);
	//清理资源
	BOOL Destroy(void);
	//获取上一次错误信息
	CString GetLastErrorDesc(void);
	//执行启动获取数据操作
	BOOL StartGetData(IN const int& iOvertime);
	//执行停止获取数据操作
	BOOL StopGetData(IN const int& iChangeMMI = 0);
	//执行检测硬件状态操作
	BOOL CheckDeviceStatus(IN OUT DWORD& dwState);
	//获取数据结果
	BOOL GetData(IN OUT CString& strData,IN OUT int& iDataType,IN OUT int& iSrcType);
	//权限认证操作
	BOOL Authority(IN const CString& strData);
	//获取固件版本信息
	BOOL GetDeviceFirewareVer(IN OUT CString& strXML);
	//获取显示内容信息
	BOOL GetDisplayContext(IN OUT int& iID, CString& strXML);
	//设置显示内容信息
	BOOL SetDisplayContext(IN const int& iID,IN const CString& strXML);
	//执行数据下发操作
	BOOL IssuedData(IN const CString& strXML);
	//执行控制IO操作
	BOOL SetIOState(IN const int& iIOState);


	BOOL UpgradeFirware(const CString& strJson);
	BOOL IsUpgrading();

	BOOL RealFirmwareVer(IN OUT CString& strXML);

	BOOL RealSlaveFirmwareVer(IN OUT CString& strXML);

	bool QueryTerminalSystemVersionStatus(int& status);
	bool ExchangeTerminalSystem(const int& targetSystemVersion);
//Attributes
private:
	//动态库加载句柄
	HMODULE	m_hModule;
	//动态库初始化标记
	BOOL	m_bInitDll;
	//组件初始化标记
	BOOL	m_bInitDev;
	//错误说明
	CString m_strLastError;
	//函数定义变量
	defIF_InitDev				fnIF_InitDev;
	defIF_Destroy				fnIF_Destroy;
	defIF_GetLastErrorDesc		fnIF_GetLastErrorDesc;
	defIF_StartGetData			fnIF_StartGetData;
	defIF_StopGetData			fnIF_StopGetData;
	defIF_CheckDeviceStatus		fnIF_CheckDeviceStatus;
	defIF_GetData				fnIF_GetData;
	defIF_Authority				fnIF_Authority;
	defIF_GetDeviceFirewareVer	fnIF_GetDeviceFirewareVer;
	defIF_GetDisplayContext		fnIF_GetDisplayContext;
	defIF_SetDisplayContext		fnIF_SetDisplayContext;
	defIF_IssuedData			fnIF_IssuedData;
	defIF_SetIOState			fnIF_SetIOState;

	defIF_UpgradeFirware        fnIF_UpgradeFirware;
	defIF_IsUpgrading           fnIF_IsUpgrading;

	defIF_RealFirmwareVer       fnIF_RealFirmwareVer;
	defIF_RealSlaveFirmwareVer  fnIF_RealSlaveFirmwareVer;

	defIF_TerminalSystemVersionStatus fnIF_TerminalSystemVersionStatus;
	defIF_ExchangeTerminalSystem fnIF_ExchangeTerminalSystem;
};
