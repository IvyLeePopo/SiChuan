/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadSMManager.h
 * 摘    要：扫码器管理线程类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
#pragma once
#include "ThreadBase.h"
#include "HTSMDeviceControl.h"
#include "SerialCom.h"

// CThreadSMManager2

class CThreadSMManager2 : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadSMManager2)

public:
	CThreadSMManager2();           // protected constructor used by dynamic creation
	virtual ~CThreadSMManager2();

	//获取设备状态（一般是指串口通讯状态）
	DWORD	GetSMState();
	void	SetMessageID(DWORD dwMessage);
	void	GetSMResult(tagSMInnerDeviceState& sState);
	void	SetDisplay(tagScanDisplay sDisplay);

	//获取串口通讯状态
	inline const BOOL GetSerialCommState(void) const {return m_bSerialCommError;}
	//获取设备硬件状态
	inline const DWORD GetDeviceState(void) const {return 0;}

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
//重载函数
protected:
	//清除本类申请的资源
	void fnReleaseResource();
	//读取本类相关的配置信息(从指定配置文件)
	void fnReadSpecailConfig();
protected:
	DECLARE_MESSAGE_MAP()

	/*******************************内部被动消息触发处理函数*******************************/
	//内部被动消息触发处理函数 -- 获取到串口数据
	afx_msg void OnMsgReceiveDataFromCOM(WPARAM wParam,LPARAM lParam);
	//通知缓冲区的数据已经发送完毕
	afx_msg void OnMsgDataSendEnd(WPARAM wParam,LPARAM lParam);
	//内部被动消息触发处理函数 -- 处理主板协议类格式化后的数据
	afx_msg void OnMsgDealResponseForProtocolProcessResult(WPARAM wParam,LPARAM lParam);
	
	/*******************************接收调用者消息处理函数*******************************/
	//接收调用者消息处理函数 -- 强制开始扫描工作
	afx_msg void OnMsgStartScan(WPARAM wParam,LPARAM lParam);
	//接收调用者消息处理函数 -- 强制结束扫描工作
	afx_msg void OnMsgStopScan(WPARAM wParam,LPARAM lParam);
	//接受调用者消息处理函数 -- 控制串口板参数
	afx_msg void OnMsgControlSerialBoard(WPARAM wParam,LPARAM lParam);
	//接收调用者消息处理函数 -- 控制串口板灯亮模式
	afx_msg void OnMsgSetLightMode(WPARAM wParam,LPARAM lParam);

	//接收调用者消息处理函数 -- 设置显示内容
	afx_msg void OnMsgSetDisplay(WPARAM wParam,LPARAM lParam);

//Operations
private:
	/**************************************物理端口操作函数**************************************/
	//物理端口操作函数 -- 打开串口连接
	BOOL	OpenCom(void);
	//物理端口操作函数 -- 关闭串口连接
	BOOL	CloseCom(void);
	//物理端口操作函数 -- 往串口写数据，字节流形式
	DWORD	WriteDataToComm(IN const LPBYTE byData,IN const DWORD& dwSize);
	//物理端口操作函数 -- 往串口写数据，字符串形式
	BOOL	WriteDataToComm(IN const CString& strWriteData);
	
	/**************************************内部流程操作函数**************************************/
	//内部流程操作函数 -- 执行扫码流程操作
	void	fnSMScanCode(void);
	//内部流程操作函数 -- 执行停止流程操作
	void	fnSMStopScan(void);
public:
	//内部流程控制函数 -- 控制灯光
	void	fnSMControlLight(IN const int& iLightType);
private:	
	//启动扫码结果回馈处理
	void	fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState);
//Attributes
private:
	/**************************************物理端口参数声明**************************************/
	//串口打开标记
	BOOL	m_bOpen;
	//串口上次的标记
	BOOL	m_bCOMLastOpenState;
	//串口控制类
	CSerialCom*		m_pCOM;
	//上次记录串口状态的时间
	time_t	m_tLastRecordCOMStateTime;
	//读写同步变量
	HANDLE	m_hSyncComData;		
	//缓冲区指针
	LPBYTE	m_lpReceiveDataBuff;	
	//缓冲区接收数据指针索引
	DWORD	m_dwReceiveDataIndex;	
	//本地通讯串口参数结构
	tagCOMParam	m_sLocalCOM;
	//串口通讯专用状态类
	tagSMInnerDeviceState m_sCOMState;
	//心跳专用状态类
	tagSMInnerDeviceState m_sHeartState;

	//扫码器协议解析类
	CHTSMDeviceControl*	m_pSMProtocol;
	//扫码启动时间
	SYSTEMTIME m_stBeginTime;
	DWORD	m_dwScanStartTime;
	//发送扫码指令与亮灯之间的沉默间隔
	DWORD	m_dwSpanDelayTimeForControlLight;
	//是否允许获取扫码信息后关闭扫码提示灯
	BOOL	m_bAllowCloseScanTipLight;
	//检测心跳间隔时间(判定有错误产生),单位：秒
	int		m_iCheckHeartErrorSpanTime;
	//标识串口通讯异常
	BOOL	m_bSerialCommError;

	//串口检索无数据输入次数
	time_t	m_tNoDataFromCOMLastTime;
	//是否在日志中打印发送的命令数据
	BOOL	m_bPrintCommandDataInLog;

	//新增扫码状态
	DWORD	m_dwSMState;
	DWORD	m_dwMessageID;
	tagSMInnerDeviceState	sSMResult;
	tagScanDisplay			sScanDisplay;

	//接收字节数的时候,超时时间配置，单位：MS 默认为10
	int			m_iNoNewDataTime;
	//上次串口读取到数据的时间
	time_t		m_tLastComReadDataTime;
	//上次串口发送数据成功的时间
	time_t		m_tLastComWriteDataTime;
	//串口出现异常后复位的间隔时间
	int			m_iResetCOMErrorSpanTime;
	//是否抛弃同样的数据（相比上一条接收到的数据）
	BOOL		m_bAbandonEchoData;
};


