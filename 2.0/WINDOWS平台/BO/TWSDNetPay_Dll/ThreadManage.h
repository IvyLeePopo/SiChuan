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
#include "ThreadDevice.h"
#include "DVXMLDebitMoney.h"
#include "JsonDebitOperate.h"
#include <map>
using namespace std;

typedef map<CString,CString>	DEBITCANCELMAP;//保存撤单信息




typedef struct tagHKArray
{
	CString		strC0ResultData;
	CString		strB1ResultData;
	CString		strB2ResultData;
	CString		strB3ResultData;
	CString		strB4ResultData;
	CString		strB5ResultData;
	CString		strB6ResultData;
	CString		strB7ResultData;

}HKArray;

class CThreadManage : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadManage)
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
	//Override
protected:
	//清除本类申请的资源
	void fnReleaseResource();
	//Inner Functions
private:


	//获取交易匹配码
	const CString GetNetPayRecordMatchCode(IN const CString& strEncKey=_T(""));
	//获取交易安全码
	const CString GetNetPaySecurityCode(void);
	//检索并删除日志
	void CheckAndDeleteLogFile(void);
	//交易结束的处理
	CString FinishTrade(IN const BOOL& bNormal);
	//定时器响应函数
	static void CALLBACK OneMilliSecondProc(UINT nTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2);
protected:
	//处理启动获取用户信息操作的消息
	afx_msg void OnMsgStartGetAccountInfo(WPARAM wParam,LPARAM lParam);
	//处理停止获取用户信息操作的消息
	afx_msg void OnMsgStopGetAccountInfo(WPARAM wParam,LPARAM lParam);
	//处理获取用户信息的消息
	afx_msg void OnMsgReceiveNorify(WPARAM wParam,LPARAM lParam);

	//查询用户输入密码的结果状态
	afx_msg void OnMsgCheckDebitPasswordState(WPARAM wParam,LPARAM lParam);
	//执行扣款操作(整合模式)
	afx_msg void OnMsgManageStartDebitFlow(WPARAM wParam,LPARAM lParam);
	//执行撤单操作
	afx_msg void OnMsgDebitCancel(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	//Attribute
public:
	//硬件控制类指针
	CThreadDevice*		m_pThreadDevice;
	//扣款数据结构体
	CDVJsonDebit		m_sDVJsonDebit;
	//默认输入密码的超时时间
	DWORD				m_dwInputPasswordDelayTime;
private:
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
	//扣款状态查询时间间隔
	DWORD				m_dwCheckDebitSpanTime;
	//扣款状态查询时间总计
	DWORD				m_dwCheckDebitAccumulateTime;
	//上次获取的用户数据
	tagAccountInfo		m_sAccountInfo;

	//定时器ID(撤单超时)
	UINT				m_nTimerIDForDebitCancel;

	//回馈数据结果集
	HKArray		m_HKResult;

	//撤单信息列表
	DEBITCANCELMAP m_DebitCancelMap;
public:
	BOOL GetHKResult(IN WORD nCmdType,OUT CString& sResult);

};


