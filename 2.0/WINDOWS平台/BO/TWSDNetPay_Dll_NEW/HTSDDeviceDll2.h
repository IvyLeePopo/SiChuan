/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：HTSDDeviceDll2.h
 * 摘    要：TR300通讯模块加载类
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年5月3日
 */
#pragma once
#include <iostream>
using namespace std;

class CHTSDDeviceDll2
{
public:
	CHTSDDeviceDll2(void);
	~CHTSDDeviceDll2(void);
	//函数定义
public:
	/*******************************公共操作************************************/
	//初始化硬件
	typedef bool	(WINAPI  *defIF_InitDev)(void (*NotifyFun)(int type,const std::string& sendStr));
	//清理资源
	typedef bool	(WINAPI  *defIF_Destroy)(void);
	//获取上一次错误信息
	typedef bool	(WINAPI  *defIF_GetLastErrorDesc)(CString& errDesc);
	//执行查询操作
	typedef bool	(WINAPI  *defIF_QueryDevice)(const std::string& paraStr,int paraType);
	//执行设置操作
	typedef bool	(WINAPI  *defIF_SetDevice)(const std::string& paraStr,int paraType);
	//执行扣款操作
	typedef bool    (WINAPI  *defIF_StartDebit)(const std::string& paraStr,int paraType,CString& strResult);
	//执行撤单操作
	typedef bool    (WINAPI  *defIF_StartDebitCancle)(IN const std::string& paraStr,int paraType);
	//执行设备初始配置操作
	typedef bool    (WINAPI  *defIF_InitPara)(IN const std::string& paraStr,IN int paraType);


	typedef bool    (WINAPI  *defIF_BreakDebit)(IN const std::string& payID);
	typedef bool	(WINAPI  *defIF_CheckDeviceStatus)(IN OUT DWORD& dwState);
	typedef bool	(WINAPI  *defIF_AgencyDebitQuery)(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);
	typedef bool    (WINAPI  *defIF_TransferImage)(IN const std::string& image_para);

	typedef bool    (WINAPI  *defIF_TransTickNo)(IN const std::string& para);

	typedef bool    (WINAPI  *def_GreenChannelTest)(int testType,const std::string& testPara,char* result);

	typedef bool  (WINAPI* defIF_ExcuteComCommond)(const std::string& hexStr);
	typedef bool  (WINAPI* defIF_TakeComFeedbackCommond)(CString& jsonStr);
	typedef bool (WINAPI *defIF_TerminalSystemVersionStatus)(int& status);
	typedef bool (WINAPI *defIF_ExchangeTerminalSystem)(const int& targetSystemVersion);
public:
	//初始化动态库
	const BOOL Init(IN const CString& strDllFilePath);
	//获取动态库加载状态
	inline const BOOL GetDllInitState(void){return m_bInitDll;}
	//获取组件初始化状态
	inline const BOOL GetDevInitState(void){return m_bInitDev;}
//Operations
public:	//组件初始化
	/*******************************公共操作************************************/
	BOOL InitDev(void (*NotifyFun)(int type,const std::string& sendStr));
	//清理资源
	BOOL Destroy(void);
	//获取上一次错误信息
	BOOL GetLastErrorDesc(IN OUT CString& strLastDesc);
	//执行检测硬件状态操作
	BOOL CheckDeviceStatus(IN OUT DWORD& dwState);
	/*******************************TR300操作************************************/
	//扣款操作
	BOOL DebitMoney(IN const CString& strDebitInfo,IN OUT CString& strResult,IN const int& iTyp=0);
	//撤单操作
	BOOL DebitCancel(IN const CString& strCancelInfo,IN const int& iType=0);
	//设备初始化配置操作
	BOOL InitConfigDevice(IN const CString& strInitInfo,IN const int& iType=0);
	//==============================================
	BOOL SetDevice(IN const CString& para);

	bool AgencyDebitQuery(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);
	bool TransferImage(const std::string& szImageInfo);

	bool TransTickNo(const std::string& para);

	bool GreenChannelTest(int testType,const std::string& testPara,std::string& result);  

	bool ExcuteComCommond(const std::string& hexStr);
	bool TakeComFeedbackCommond(CString& jsonStr); 

	bool QueryTerminalSystemVersionStatus(int& status);
	bool ExchangeTerminalSystem(const int& targetSystemVersion);

//Attributes
public:/*add zhuyabing 201706*/
	bool BreakDebit(const CString& payID);
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
	defIF_QueryDevice			fnIF_QueryDevice;
	defIF_SetDevice				fnIF_SetDevice;
	defIF_StartDebit			fnIF_DebitMoney;
	defIF_StartDebitCancle      fnIF_DebitCancel;
	defIF_InitPara				fnIF_InitDeviecCMD;

	defIF_BreakDebit            fnIF_BreakDebit;

	defIF_CheckDeviceStatus		fnIF_CheckDeviceStatus;

	defIF_AgencyDebitQuery      fnIF_AgencyDebitQuery;
	defIF_TransferImage         fnIF_TransferImage;

	defIF_TransTickNo			fnIF_TransTickNo;

	def_GreenChannelTest        fn_GreenChannelTest;

	defIF_ExcuteComCommond fnIF_ExcuteComCommond;
	defIF_TakeComFeedbackCommond fnIF_TakeComFeedbackCommond;
	
	defIF_TerminalSystemVersionStatus fnIF_TerminalSystemVersionStatus;
	defIF_ExchangeTerminalSystem fnIF_ExchangeTerminalSystem;
};
