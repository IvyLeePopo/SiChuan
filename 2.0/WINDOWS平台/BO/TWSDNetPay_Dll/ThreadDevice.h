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
	inline const BOOL GetDeviceInitState(void){if(NULL != m_pHTSDDeviceDll) return m_pHTSDDeviceDll->GetDevInitState(); return FALSE;}
	//获取硬件设备状态
	DWORD GetDeviceState(void);

public:
	//获取C0帧数据的接收情况
	BOOL GetC0SuccFailedCount(OUT int& dwSucce, OUT int& dwFailed, OUT int& nReSendCount, OUT CString& strA2Frame, OUT CString& strC0Frame);

protected:

	//响应硬件推送的消息
	afx_msg void OnMsgGetData(WPARAM wParam,LPARAM lParam);

	//执行启动获取数据的操作
	afx_msg void OnMsgStartGetData(WPARAM wParam,LPARAM lParam);

	//执行停止获取数据的操作
	afx_msg void OnMsgStopGetData(WPARAM wParam,LPARAM lParam);

	//接收到信息，要求执行费显功能
	afx_msg void OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam);

	//接收到消息，启动扣款流程
	afx_msg void OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam);

	//接收到消息，启动撤单流程
	afx_msg void OnMsgDebitCancelFlow(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	//往硬件下发站点配置信息，一定要保证成功
	const CString InitConfigDevice(int nFormatType = 1);

	//主动查询心跳时间
	time_t m_tLastCheckBoard; 

	//Attribute:
private:
	//智能硬件版本(1,初始版本;2,1.5版本)
	int				m_iDeviceVersion;
	//硬件控制接口类
	CHTSDDeviceDll*	m_pHTSDDeviceDll;
	//硬件上次检测的状态
	DWORD			m_dwLastCheckState;
	//是否下发初始化指令
	BOOL m_bInitConfigDevice;



};


