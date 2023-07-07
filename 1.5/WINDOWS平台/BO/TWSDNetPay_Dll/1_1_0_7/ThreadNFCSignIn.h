/*
 * Copyright(C) 2017,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadNFCSignIn.h
 * 摘    要：NFC模块签到专用线程
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2017年9月20日
 */
#pragma once


#include "ThreadHttp.h"
#include "NFCProtocol.h"
#include "DVJsonNFCSignIn.h"
#include "JsonNFCOperate.h"


#include "HttpClient_LibCurl.h"
// CThreadNFCSignIn

class CThreadNFCSignIn : public CThreadHttp
{
	DECLARE_DYNCREATE(CThreadNFCSignIn)

protected:
	CThreadNFCSignIn();           // protected constructor used by dynamic creation
	virtual ~CThreadNFCSignIn();
private:
	static HttpClientLibCurl m_pHttp;
	HANDLE StartProcessThread_WinHttp(IN const LPVOID lpParam);

	static UINT  WINAPI SendData_WinHttp(LPVOID lpParam);
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
//Override
protected:
	//清除本类申请的资源
	void fnReleaseResource();
	//数据格式转换,从调用者传过来的DVJson格式转换成字符串形式
	const BOOL DataConvertFromJson2String(IN const WORD& wSendDataType,IN OUT CDVJsonBase* pDVJsonReceive,IN OUT CString& strURL,IN OUT INTERNET_PORT& wPort,IN OUT CString& strSendData,IN OUT HWND& hWnd,IN OUT int& iCallMode,IN OUT CJsonOperationIF** pJsonIF, int nSN );
private:
	//读取本类相关的配置信息(从指定配置文件)
	void fnReadSpecailConfig();
	//构造并发送NFC签到数据
	void ContructNFCSignInData(IN const CString& strSignData);
protected:
	DECLARE_MESSAGE_MAP()
	//传输NFC签到数据到云端
	afx_msg void OnMsgTransportSignInData(WPARAM wParam,LPARAM lParam);
	//接收从NFC模块上传的签到数据
	afx_msg void OnMsgRevSignDataFromPOS(WPARAM wParam,LPARAM lParam);

//Attributes
private:
	//签到接口路径
	CString			m_strHttpNFCSignInURL;
	//签到接口端口
	INTERNET_PORT	m_wHttpNFCSignInPort;
	//发送数据子线程超时时间限制
	DWORD  m_dwWaitThreadWorkTimeout;
};


