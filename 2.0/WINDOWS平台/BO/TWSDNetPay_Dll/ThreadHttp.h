/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadHttp.h
 * 摘    要：完成与收费中心WEB服务器的所有通讯工作，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
#pragma once
#include "BaseDefine.h"
#include "ThreadBase.h"
#include <afxinet.h>
#include <process.h> 
#include "JsonOperationIF.h"
#include "JsonDebitOperate.h"
#include "JsonHeartOperate.h"
#include "TableDebitCancel.h"

//#include <Ipexport.h>
//#include <Icmpapi.h>
//#pragma  comment(lib, "Iphlpapi.lib") 

class CThreadHttp : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadHttp)
//参数结构体定义
	typedef struct _tagHttpParam
	{
		CString			SendData;			//发送的数据信息
		CString			URL;				//URL地址
		INTERNET_PORT	Port;				//链接端口号
		int				CallMode;			//数据递交类型(POST 或 GET)
		DWORD			ConsumeTime;		//线程处理消耗时间(毫秒)
		DWORD			ExecuteCode;		//线程处理结果代码
		CString			ServerResponseData;	//服务端返回数据
		DWORD			ServerResponseCode;	//服务端返回代码
		BOOL			NeedEndThread;		//是否强制终止线程

		DWORD			SessionReceiveTimeout;	//接收数据超时
		DWORD			SessionSendTimeout;		//发送数据超时
		DWORD			SessionControlTimeout;	//控制超时
	}tagHttpParam;
public:
	CThreadHttp();           // protected constructor used by dynamic creation
	virtual ~CThreadHttp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
protected:
	//给与外网联通的反射WEB服务器发送请求
	afx_msg void OnMsgSendQueryToReflectWeb(WPARAM wParam,LPARAM lParam);
	//需要发送一次心跳到检测状态
	afx_msg void OnMsgSnedHeart(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
//重载函数
protected:
	//清除本类申请的资源
	void fnReleaseResource();
	//读取本类相关的配置信息(从指定配置文件)
	void fnReadSpecailConfig();
	//获取发送数据类型描述
	const CString GetSendDataTypeDesc(IN const int& iDataType);
	//获取服务器回馈代码描述
	const CString GetServerResponseCodeDesc(IN const int& iResponseType);
//内部辅助处理函数
private:
	//对于不同的编码格式的字符串进行转换
	static CString StringConvertByCodePage(IN const CString& strConvertData,IN const int& iSourceCodepage, IN const int& iTargetCodepage);
	//启动Http处理线程
	HANDLE StartProcessThread(IN const LPVOID lpParam);
	//进行心跳检测
	void CheckHeartState();
	//数据递交 -- 线程工作函数
	static UINT  WINAPI SendData(LPVOID lpParam);
	//数据格式转换,从调用者传过来的DVJson格式转换成字符串形式
	const BOOL DataConvertFromJson2String(IN const WORD& wSendDataType,IN OUT CDVJsonBase* pDVJsonReceive,IN OUT CString& strURL,IN OUT INTERNET_PORT& wPort,IN OUT CString& strSendData,IN OUT HWND& hWnd,IN OUT int& iCallMode,IN OUT CJsonOperationIF** pJsonIF);
	//测试指定IP计算机是否在同一个网络内
	//static BOOL Ping(LPCTSTR lpHostName,UCHAR nTTL=10, DWORD dwTimeout=500,UCHAR nPacketSize=32);
	//获取随机数，在给定的范围内
	UINT GetRandNumber(IN const UINT& nMin,IN const UINT& nMax);
	//检测处理是否有未递交的撤单数据
	//void CheckUnProcessDebitCancelRecord();

//属性
private:
	//入口发起的查询所使用的端口
	INTERNET_PORT	m_wQueryEntryPort;
	//出口发起的查询所使用的端口
	INTERNET_PORT	m_wQueryExitPort;
	//扣款指令所使用的端口
	INTERNET_PORT	m_wDebitPort;
	//扣款二次处理指令所使用的端口
	INTERNET_PORT	m_wDebitSecondPort;
	//扣款查询所用端口
	INTERNET_PORT	m_wDebitCheckPort;
	//扣款撤单所用端口
	INTERNET_PORT	m_wDebitCancelPort;
	//开环借口所用端口
	INTERNET_PORT	m_wOpenLoopPort;
	//闭环接口所用端口
	INTERNET_PORT	m_wCloseLoopPort;
	//开放站过车接口所用端口
	INTERNET_PORT	m_wPassStationPort;
	//流水记录推送接口所用的端口
	INTERNET_PORT	m_wSendRecordPort;
	//心跳查询接口所用的端口
	INTERNET_PORT	m_wHeartPort;

	//入口查询接口地址
	CString	m_strQueryEntryURL;
	//出口查询接口地址
	CString	m_strQueryExitURL;
	//扣款请求接口地址
	CString	m_strDebitURL;
	//扣款二次处理接口地址
	CString m_strDebitSecondURL;
	//扣款查询接口地址
	CString m_strDebitCheckURL;
	//扣款撤单接口地址
	CString m_strDebitCancelURL;
	//入口开环接口地址
	CString m_strOpenLoopURL;
	//出口闭环接口地址
	CString m_strCloseLoopURL;
	//开放过站接口地址
	CString m_strPassStationURL;
	//心跳查询接口使用的URL
	CString	m_strHeartURL;
	
	//记录推送URL地址
	CString m_strSendRecordURL;

	//中心反射服务器心跳检测失败累加值
	DWORD	m_dwHeratFailForCenterReflectWeb;
	//运营平台服务器心跳检测失败累加值
	DWORD	m_dwHeratFailForOperationServer;
	
	//判定心跳检测失败多少次报警
	DWORD	m_dwHeratFailWarningTimes;
	//心跳检测时间间隔最小阈值
	DWORD	m_dwHeartCheckMin;
	//心跳检测时间间隔最大阈值
	DWORD	m_dwHeartCheckMax;

	//是否正在重发数据
	BOOL m_bResendWorking;
	//重发数据的时间间隔
	time_t m_tResendDataSpanTime;
	//发送数据子线程超时时间限制
	DWORD  m_dwWaitThreadWorkTimeout;
	//撤单本地缓存数据处理类
	//CTableDebitCancel	m_cTableDebitCancel;
};


