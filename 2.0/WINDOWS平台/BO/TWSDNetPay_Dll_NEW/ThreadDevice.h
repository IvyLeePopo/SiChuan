/*
* Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
* 
* 文件名称：ThreadDevice.h
* 摘    要：硬件控制线程，声明文件
* 当前版本：1.0.0.1
* 作    者：ROCY
* 创建日期：2016年7月23日
*/
#pragma once
#include "ThreadBase.h"
#include "HTSDDeviceDll.h"
#include "HTSDDeviceDll2.h"
// CThreadDevice

class CThreadDevice : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadDevice)

public:
	CThreadDevice();           // protected constructor used by dynamic creation
	virtual ~CThreadDevice();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
//Operations
public:
	//获取硬件初始化状态
	inline const BOOL GetDeviceInitState(void){if(NULL != m_pHTSDDeviceDll) return m_pHTSDDeviceDll->GetDevInitState(); return FALSE;}
	//获取硬件设备状态
	DWORD GetDeviceState(void);
public:/*add zhuyabing 201706*/
	bool BreakDebit(const CString& payID);
	bool AgencyDebitQuery(const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);
	bool TransferImage(const std::string& szImageInfo);

	bool TransTickNo(const std::string& para);

	bool HardwareTestGreenChannel(int testType,const std::string& testPara,std::string& result);//add 2018-04 提供给硬件组测试数据的专用通道，
	bool ExcuteComCommond(const std::string& hexStr);
	bool TakeComFeedbackCommond(CString& jsonStr);

	bool QueryTerminalSystemVersionStatus(int& status);
	bool ExchangeTerminalSystem(const int& targetSystemVersion);
protected:
	//接收到信息，要求执行费显功能
	afx_msg void OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam);
	//接收到消息，启动扣款流程
	afx_msg void OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam);
	//接收到消息，启动撤单流程
	afx_msg void OnMsgDebitCancelFlow(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
private:
	//往硬件下发站点配置信息
	BOOL InitConfigDevice(void);
	//获取通讯模块主动通知函数
	static void fnGetNotifyFromCommModule(int iType,const std::string& strData);
//Attribute:
private:
	//智能硬件版本(1,初始版本;2,1.5版本)
	int				m_iDeviceVersion;
	//硬件控制接口类
	//CHTSDDeviceDll*	m_pHTSDDeviceDll;
	CHTSDDeviceDll2* m_pHTSDDeviceDll;
	//硬件上次检测的状态
	DWORD			m_dwLastCheckState;
	//是否下发初始化指令
	BOOL			m_bInitConfigDevice;
	//最近一次初始化指令下发时间
	time_t			m_tLastInit;

public:
	bool Init_Dev();
	bool Destroy_Dev();
};


