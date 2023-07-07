/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadNFCSignIn.h
 * ժ    Ҫ��NFCģ��ǩ��ר���߳�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��20��
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
	//��������������Դ
	void fnReleaseResource();
	//���ݸ�ʽת��,�ӵ����ߴ�������DVJson��ʽת�����ַ�����ʽ
	const BOOL DataConvertFromJson2String(IN const WORD& wSendDataType,IN OUT CDVJsonBase* pDVJsonReceive,IN OUT CString& strURL,IN OUT INTERNET_PORT& wPort,IN OUT CString& strSendData,IN OUT HWND& hWnd,IN OUT int& iCallMode,IN OUT CJsonOperationIF** pJsonIF, int nSN );
private:
	//��ȡ������ص�������Ϣ(��ָ�������ļ�)
	void fnReadSpecailConfig();
	//���첢����NFCǩ������
	void ContructNFCSignInData(IN const CString& strSignData);
protected:
	DECLARE_MESSAGE_MAP()
	//����NFCǩ�����ݵ��ƶ�
	afx_msg void OnMsgTransportSignInData(WPARAM wParam,LPARAM lParam);
	//���մ�NFCģ���ϴ���ǩ������
	afx_msg void OnMsgRevSignDataFromPOS(WPARAM wParam,LPARAM lParam);

//Attributes
private:
	//ǩ���ӿ�·��
	CString			m_strHttpNFCSignInURL;
	//ǩ���ӿڶ˿�
	INTERNET_PORT	m_wHttpNFCSignInPort;
	//�����������̳߳�ʱʱ������
	DWORD  m_dwWaitThreadWorkTimeout;
};


