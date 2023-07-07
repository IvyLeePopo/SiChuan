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
#include "NFCProtocol.h"
#include "DVXMLUnipay.h"
  
// CThreadDevice

class CThreadDevice : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadDevice)

public://protected --> public 由于未知的原因，ExitInstance在线程退出的时候并没有被执行，因此需要手动清理资源。
	CThreadDevice();           // protected constructor used by dynamic creation
	virtual ~CThreadDevice();
public:
	CUpdateData		c_update_device;//用于向底层控制发送的更新队列
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
//Operations
	inline const BOOL GetDeviceInitState(void){if(NULL != m_pHTSDDeviceDll) return m_pHTSDDeviceDll->GetDevInitState(); return FALSE;}
	//获取硬件设备状态
	DWORD	GetDeviceState(void);

	void	ReadWebConfig();
	void	ITMSMain();
	void	LocalDBCheck();
	void	ArrayCheck();
	void	GGPublish(int nPos);

	int		GetLocalDB2Array(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, int nType, CString& csError);
	int		SetLocalDBTransMark(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, CString& csError );

public://add zhuyabing
	void RestartNFCPay();
	void RemoteControlFirmware(const std::string&para);
	

private://add zhuyabing
	CString m_LastNFCPayData;


protected:
	//执行下发数据功能操作
	afx_msg void OnMsgIssueData(WPARAM wParam,LPARAM lParam);
	//执行启动获取数据的操作
	afx_msg void OnMsgStartGetData(WPARAM wParam,LPARAM lParam);
	//执行停止获取数据的操作
	afx_msg void OnMsgStopGetData(WPARAM wParam,LPARAM lParam);
	//接收到硬件推送的消息，进行数据获取
	afx_msg void OnMsgGetData(WPARAM wParam,LPARAM lParam);
	//接收到信息，要求执行费显功能
	afx_msg void OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam);
	//接收到消息，启动扣款流程
	afx_msg void OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam);
	//接收到消息，要求展示缴费结果交互界面
	afx_msg void OnMsgControlResultMMI(WPARAM wParam,LPARAM lParam);

	afx_msg void OnMsgRemoteUpgradeResult(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//往智能硬件更新时间
	BOOL UpdateDeviceTime(void);
	//控制显示支付异常的信息（占用广告位置）
	void SetPayErrorMsgInfo(IN const int& iErrorCode = 0,IN const bool& bForceDefault = false);
//Attribute:
private:
	//智能硬件版本(1,初始版本;2,1.5版本)
	int				m_iDeviceVersion;
	//硬件控制接口类
	CHTSDDeviceDll*	m_pHTSDDeviceDll;
	//硬件上次检测的状态
	DWORD			m_dwLastCheckState;

	CString			m_csUpdateLDB;	//本地更新存储文件
	CString			m_csLDBTable;	//本地更新存储文件表名
	CString			m_csDevLog;		//硬件日志记录

	int				nLocalDBSpan;	//本地数据库检测处理间隔
	int				nArraySpan;		//更新队列检测处理间隔
	DWORD			dwLocalDBCheck;	//本地数据库检测时间点
	DWORD			dwArrayCheck;	//更新队列检测时间点
	CString			m_strDefaultGGText;	//默认从云端拖下来的文字广告内容

	bool m_firmwareUpdateFlag ;//add 2018-01 表示是否有固件更新，
	time_t m_firmwareUpdateTime; //add 2018-01 固件升级的时间

private:
	void Write_dev_id();
	std::string mShareMemNameOfDevId;
	HANDLE mhShareMemOfDevId;
	char* mpShareMemOfDevId;

};


