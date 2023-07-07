/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadManage.h
 * 摘    要：业务后台管理线程，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年7月24日
 */
#pragma once
#include "ThreadBase.h"
#include "ThreadHttp.h"
#include "ThreadDevice.h"
#include "ThreadUpdate.h"
#include "ThreadNFCSignIn.h"
#include "DVXMLDebitMoney.h"
#include "NFCProtocol.h"

#include "ThreadDevice_ForDebug.h"

class CThreadManage : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadManage)

private://add 2017-11
	bool goDie;
	int dieNumber;
	CRITICAL_SECTION idleMutex;

protected:
	CThreadManage();           // protected constructor used by dynamic creation
	virtual ~CThreadManage();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
//Operations
public:
	//启动工作线程
	const BOOL StartWorkThreads(IN const tagTranstParam& sTranstParam);
	//判断用户数据是否已经就绪（硬件提供）
	const BOOL GetAccountDataReadyState(void) {return m_bAccountInfoIsReady;}
	//设置用户数据是否准备就绪
	void SetAccountDataReadyState(IN const BOOL& bState);
	//获取上次检索到的用户数据
	const tagAccountInfo GetLastAccountInfo(void){return m_sAccountInfo;}
	//设置定时器
	void StartTimer(IN const UINT& nEventID,IN const DWORD& dwDelayTime);
	//停止定时器
	void StopTimer(IN const UINT& nEventID);
	//获取硬件驱动加载状况
	const BOOL GetDeviceLoadStae(void) const {return m_bDeviceLoadState;}
	//线程退出
	void	ThreadQuit();

//Override
protected:
	//清除本类申请的资源
	void fnReleaseResource();
//Inner Functions
private:
	//构造发送到支付子平台的数据
	BOOL ContructDebitMoneyInfo(IN const tagAccountInfo* pAccountInfo);
	//构造发送到支付子平台的数据
	BOOL ContructDebitMoneyInfo(void);
	//构造发送到支付子平台的数据
	BOOL ContructDebitCancelInfo(IN const CString& strPayIdentifier);
	//获取交易匹配码
	const CString GetNetPayRecordMatchCode(IN const CString& strEncKey=_T(""));
	//获取交易安全码
	const CString GetNetPaySecurityCode(void);
	//检索并删除日志
	void CheckAndDeleteLogFile(void);
	//交易结束的处理
	CString FinishTrade(IN const BOOL& bNormal);
	//结束已取消的交易并进行撤单操作
	void FinishHadCancelTrade(IN const CDVJsonBase* pDVJsonBase);
	//定时器响应函数
	static void CALLBACK OneMilliSecondProc(UINT nTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2);


	//NFC测试连接回馈处理函数
	void NFCLinkTestDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol);
	//NFC消费数据处理函数
	void NFCConsumeDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol);
	//NFC冲正数据处理函数
	void NFCCorrectDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol);
	//NFC模块回馈数据帧异常处理
	void NFCProcessErrorFrame(IN CNFCProtocol& cNFCProtocol);
	//NFC签到数据处理函数（数据从POS非接模块回传）
	void NFCSignInDataProcessFromPOS(IN CNFCProtocol& cNFCProtocol);
	//NFC签到数据处理函数（数据从HTTP回传）
	void NFCSignInDataProcessFromHTTP(IN const CString& strData);
	//NFC交易失败处理（数据未递交云端）
	void NFCLocalTradeFailProcess(IN const int& iExecuteCode = 0xFF);
	//NFC交易流程，发送默认的指令（1，测试链路/复位；2，发起签到；3，停止执行上一次流程）
	void NFCSendDefaultCmdToPos(IN const int& iCmdType);
public:
	void NFCParaQuery(const CString& strData);
protected:
	//处理启动获取用户信息操作的消息
	afx_msg void OnMsgStartGetAccountInfo(WPARAM wParam,LPARAM lParam);
	//处理停止获取用户信息操作的消息
	afx_msg void OnMsgStopGetAccountInfo(WPARAM wParam,LPARAM lParam);
	//处理获取用户信息的消息
	afx_msg void OnMsgReceiveAccountInfo(WPARAM wParam,LPARAM lParam);
	//处理支付子平台回馈的消息
	afx_msg void OnMsgDealSubPayPlatformResponse(WPARAM wParam,LPARAM lParam);
	//查询用户输入密码的结果状态
	afx_msg void OnMsgCheckDebitPasswordState(WPARAM wParam,LPARAM lParam);
	//执行扣款操作(整合模式)
	afx_msg void OnMsgManageStartDebitFlow(WPARAM wParam,LPARAM lParam);
	//执行扣款操作(单体模式)
	afx_msg void OnMsgOnlyDebitMoney(WPARAM wParam,LPARAM lParam);
	//执行撤单操作
	afx_msg void OnMsgDebitCancel(WPARAM wParam,LPARAM lParam);
	//处理从HTTP端返回的NFC签到数据
	afx_msg void OnMsgRevNFCSignDataFromHTTP(WPARAM wParam,LPARAM lParam);
	//获取通知，发起签到流程
	afx_msg void OnMsgNFCStartSignInFlow(WPARAM wParam,LPARAM lParam);

	//add 2017-12
	afx_msg void OnMsgRemoteControlFirmware(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMsgRemoteUpgradeResult(WPARAM wParam,LPARAM lParam);

	afx_msg void OnMsgQueryLPRImageId(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
//Attribute
public:
	//硬件控制类指针
	CThreadDevice*		m_pThreadDevice;

	//更新线程类指针
	CThreadUpdate*		m_pThreadUpdate;

	//银联模块签到线程类指针
	CThreadNFCSignIn*	m_pThreadNFCSignIn;

	//扣款状态查询
	CDVJsonDebitCheck*  m_pDVJsonDebitCheck;
	//扣款数据结构体
	CDVJsonDebit		m_sDVJsonDebit;
	//默认输入密码的超时时间
	DWORD				m_dwInputPasswordDelayTime;
private:

	CThreadDevice_ForDebug* bing_Debug;
	//支付子平台通讯线程类指针
	CThreadHttp*		m_pThreadHttp;
	//用户信息是否准备就绪(数据被取走后，需要重置状态)
	BOOL				m_bAccountInfoIsReady;
	//硬件驱动加载情况
	BOOL				m_bDeviceLoadState;
	//定时器分辨率
	UINT				m_nAccuracy;

	//定时器ID(扫码超时)
	UINT				m_nTimerIDForScanCode;
	//定时器ID(扣款超时)
	UINT				m_nTimerIDForDebitMoney;
	//定时器ID(扣款状态查询)
	UINT				m_nTimerIDForCheckDebitState;
	//定时器ID(NFC冲正流水获取超时)
	UINT				m_nTimerIDForNFCGetCorrentData;
	//定时器ID(NFC普通数据获取超时)
	UINT				m_nTimerIDForNFCGetNormalData;
	//扣款状态查询时间间隔
	DWORD				m_dwCheckDebitSpanTime;
	//扣款状态查询时间总计
	DWORD				m_dwCheckDebitAccumulateTime;
	//上次获取的用户数据
	tagAccountInfo		m_sAccountInfo;
};


