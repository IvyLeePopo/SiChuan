/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadHttp.h
 * ժ    Ҫ��������շ�����WEB������������ͨѶ�����������ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��12��
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
//�����ṹ�嶨��
	typedef struct _tagHttpParam
	{
		CString			SendData;			//���͵�������Ϣ
		CString			URL;				//URL��ַ
		INTERNET_PORT	Port;				//���Ӷ˿ں�
		int				CallMode;			//���ݵݽ�����(POST �� GET)
		DWORD			ConsumeTime;		//�̴߳�������ʱ��(����)
		DWORD			ExecuteCode;		//�̴߳���������
		CString			ServerResponseData;	//����˷�������
		DWORD			ServerResponseCode;	//����˷��ش���
		BOOL			NeedEndThread;		//�Ƿ�ǿ����ֹ�߳�

		DWORD			SessionReceiveTimeout;	//�������ݳ�ʱ
		DWORD			SessionSendTimeout;		//�������ݳ�ʱ
		DWORD			SessionControlTimeout;	//���Ƴ�ʱ
	}tagHttpParam;
public:
	CThreadHttp();           // protected constructor used by dynamic creation
	virtual ~CThreadHttp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
protected:
	//����������ͨ�ķ���WEB��������������
	afx_msg void OnMsgSendQueryToReflectWeb(WPARAM wParam,LPARAM lParam);
	//��Ҫ����һ�����������״̬
	afx_msg void OnMsgSnedHeart(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
//���غ���
protected:
	//��������������Դ
	void fnReleaseResource();
	//��ȡ������ص�������Ϣ(��ָ�������ļ�)
	void fnReadSpecailConfig();
	//��ȡ����������������
	const CString GetSendDataTypeDesc(IN const int& iDataType);
	//��ȡ������������������
	const CString GetServerResponseCodeDesc(IN const int& iResponseType);
//�ڲ�����������
private:
	//���ڲ�ͬ�ı����ʽ���ַ�������ת��
	static CString StringConvertByCodePage(IN const CString& strConvertData,IN const int& iSourceCodepage, IN const int& iTargetCodepage);
	//����Http�����߳�
	HANDLE StartProcessThread(IN const LPVOID lpParam);
	//�����������
	void CheckHeartState();
	//���ݵݽ� -- �̹߳�������
	static UINT  WINAPI SendData(LPVOID lpParam);
	//���ݸ�ʽת��,�ӵ����ߴ�������DVJson��ʽת�����ַ�����ʽ
	const BOOL DataConvertFromJson2String(IN const WORD& wSendDataType,IN OUT CDVJsonBase* pDVJsonReceive,IN OUT CString& strURL,IN OUT INTERNET_PORT& wPort,IN OUT CString& strSendData,IN OUT HWND& hWnd,IN OUT int& iCallMode,IN OUT CJsonOperationIF** pJsonIF);
	//����ָ��IP������Ƿ���ͬһ��������
	//static BOOL Ping(LPCTSTR lpHostName,UCHAR nTTL=10, DWORD dwTimeout=500,UCHAR nPacketSize=32);
	//��ȡ��������ڸ����ķ�Χ��
	UINT GetRandNumber(IN const UINT& nMin,IN const UINT& nMax);
	//��⴦���Ƿ���δ�ݽ��ĳ�������
	//void CheckUnProcessDebitCancelRecord();

//����
private:
	//��ڷ���Ĳ�ѯ��ʹ�õĶ˿�
	INTERNET_PORT	m_wQueryEntryPort;
	//���ڷ���Ĳ�ѯ��ʹ�õĶ˿�
	INTERNET_PORT	m_wQueryExitPort;
	//�ۿ�ָ����ʹ�õĶ˿�
	INTERNET_PORT	m_wDebitPort;
	//�ۿ���δ���ָ����ʹ�õĶ˿�
	INTERNET_PORT	m_wDebitSecondPort;
	//�ۿ��ѯ���ö˿�
	INTERNET_PORT	m_wDebitCheckPort;
	//�ۿ�����ö˿�
	INTERNET_PORT	m_wDebitCancelPort;
	//����������ö˿�
	INTERNET_PORT	m_wOpenLoopPort;
	//�ջ��ӿ����ö˿�
	INTERNET_PORT	m_wCloseLoopPort;
	//����վ�����ӿ����ö˿�
	INTERNET_PORT	m_wPassStationPort;
	//��ˮ��¼���ͽӿ����õĶ˿�
	INTERNET_PORT	m_wSendRecordPort;
	//������ѯ�ӿ����õĶ˿�
	INTERNET_PORT	m_wHeartPort;

	//��ڲ�ѯ�ӿڵ�ַ
	CString	m_strQueryEntryURL;
	//���ڲ�ѯ�ӿڵ�ַ
	CString	m_strQueryExitURL;
	//�ۿ�����ӿڵ�ַ
	CString	m_strDebitURL;
	//�ۿ���δ���ӿڵ�ַ
	CString m_strDebitSecondURL;
	//�ۿ��ѯ�ӿڵ�ַ
	CString m_strDebitCheckURL;
	//�ۿ���ӿڵ�ַ
	CString m_strDebitCancelURL;
	//��ڿ����ӿڵ�ַ
	CString m_strOpenLoopURL;
	//���ڱջ��ӿڵ�ַ
	CString m_strCloseLoopURL;
	//���Ź�վ�ӿڵ�ַ
	CString m_strPassStationURL;
	//������ѯ�ӿ�ʹ�õ�URL
	CString	m_strHeartURL;
	
	//��¼����URL��ַ
	CString m_strSendRecordURL;

	//���ķ���������������ʧ���ۼ�ֵ
	DWORD	m_dwHeratFailForCenterReflectWeb;
	//��Ӫƽ̨�������������ʧ���ۼ�ֵ
	DWORD	m_dwHeratFailForOperationServer;
	
	//�ж��������ʧ�ܶ��ٴα���
	DWORD	m_dwHeratFailWarningTimes;
	//�������ʱ������С��ֵ
	DWORD	m_dwHeartCheckMin;
	//�������ʱ���������ֵ
	DWORD	m_dwHeartCheckMax;

	//�Ƿ������ط�����
	BOOL m_bResendWorking;
	//�ط����ݵ�ʱ����
	time_t m_tResendDataSpanTime;
	//�����������̳߳�ʱʱ������
	DWORD  m_dwWaitThreadWorkTimeout;
	//�������ػ������ݴ�����
	//CTableDebitCancel	m_cTableDebitCancel;
};


