/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�HTSDDeviceDll2.h
 * ժ    Ҫ��TR300ͨѶģ�������
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��5��3��
 */
#pragma once
#include <iostream>
using namespace std;

class CHTSDDeviceDll2
{
public:
	CHTSDDeviceDll2(void);
	~CHTSDDeviceDll2(void);
	//��������
public:
	/*******************************��������************************************/
	//��ʼ��Ӳ��
	typedef bool	(WINAPI  *defIF_InitDev)(void (*NotifyFun)(int type,const std::string& sendStr));
	//������Դ
	typedef bool	(WINAPI  *defIF_Destroy)(void);
	//��ȡ��һ�δ�����Ϣ
	typedef bool	(WINAPI  *defIF_GetLastErrorDesc)(CString& errDesc);
	//ִ�в�ѯ����
	typedef bool	(WINAPI  *defIF_QueryDevice)(const std::string& paraStr,int paraType);
	//ִ�����ò���
	typedef bool	(WINAPI  *defIF_SetDevice)(const std::string& paraStr,int paraType);
	//ִ�пۿ����
	typedef bool    (WINAPI  *defIF_StartDebit)(const std::string& paraStr,int paraType,CString& strResult);
	//ִ�г�������
	typedef bool    (WINAPI  *defIF_StartDebitCancle)(IN const std::string& paraStr,int paraType);
	//ִ���豸��ʼ���ò���
	typedef bool    (WINAPI  *defIF_InitPara)(IN const std::string& paraStr,IN int paraType);


	typedef bool    (WINAPI  *defIF_BreakDebit)(IN const std::string& payID);
	typedef bool	(WINAPI  *defIF_CheckDeviceStatus)(IN OUT DWORD& dwState);
	typedef bool	(WINAPI  *defIF_AgencyDebitQuery)(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);
	typedef bool    (WINAPI  *defIF_TransferImage)(IN const std::string& image_para);

	typedef bool    (WINAPI  *defIF_TransTickNo)(IN const std::string& para);

	typedef bool    (WINAPI  *def_GreenChannelTest)(int testType,const std::string& testPara,char* result);

	typedef bool  (WINAPI* defIF_ExcuteComCommond)(const std::string& hexStr);
	typedef bool  (WINAPI* defIF_TakeComFeedbackCommond)(CString& jsonStr);
	typedef bool (WINAPI *defIF_TerminalSystemVersionStatus)(int& status);
	typedef bool (WINAPI *defIF_ExchangeTerminalSystem)(const int& targetSystemVersion);
public:
	//��ʼ����̬��
	const BOOL Init(IN const CString& strDllFilePath);
	//��ȡ��̬�����״̬
	inline const BOOL GetDllInitState(void){return m_bInitDll;}
	//��ȡ�����ʼ��״̬
	inline const BOOL GetDevInitState(void){return m_bInitDev;}
//Operations
public:	//�����ʼ��
	/*******************************��������************************************/
	BOOL InitDev(void (*NotifyFun)(int type,const std::string& sendStr));
	//������Դ
	BOOL Destroy(void);
	//��ȡ��һ�δ�����Ϣ
	BOOL GetLastErrorDesc(IN OUT CString& strLastDesc);
	//ִ�м��Ӳ��״̬����
	BOOL CheckDeviceStatus(IN OUT DWORD& dwState);
	/*******************************TR300����************************************/
	//�ۿ����
	BOOL DebitMoney(IN const CString& strDebitInfo,IN OUT CString& strResult,IN const int& iTyp=0);
	//��������
	BOOL DebitCancel(IN const CString& strCancelInfo,IN const int& iType=0);
	//�豸��ʼ�����ò���
	BOOL InitConfigDevice(IN const CString& strInitInfo,IN const int& iType=0);
	//==============================================
	BOOL SetDevice(IN const CString& para);

	bool AgencyDebitQuery(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);
	bool TransferImage(const std::string& szImageInfo);

	bool TransTickNo(const std::string& para);

	bool GreenChannelTest(int testType,const std::string& testPara,std::string& result);  

	bool ExcuteComCommond(const std::string& hexStr);
	bool TakeComFeedbackCommond(CString& jsonStr); 

	bool QueryTerminalSystemVersionStatus(int& status);
	bool ExchangeTerminalSystem(const int& targetSystemVersion);

//Attributes
public:/*add zhuyabing 201706*/
	bool BreakDebit(const CString& payID);
private:
	//��̬����ؾ��
	HMODULE	m_hModule;
	//��̬���ʼ�����
	BOOL	m_bInitDll;
	//�����ʼ�����
	BOOL	m_bInitDev;
	//����˵��
	CString m_strLastError;
	//�����������
	defIF_InitDev				fnIF_InitDev;
	defIF_Destroy				fnIF_Destroy;
	defIF_GetLastErrorDesc		fnIF_GetLastErrorDesc;
	defIF_QueryDevice			fnIF_QueryDevice;
	defIF_SetDevice				fnIF_SetDevice;
	defIF_StartDebit			fnIF_DebitMoney;
	defIF_StartDebitCancle      fnIF_DebitCancel;
	defIF_InitPara				fnIF_InitDeviecCMD;

	defIF_BreakDebit            fnIF_BreakDebit;

	defIF_CheckDeviceStatus		fnIF_CheckDeviceStatus;

	defIF_AgencyDebitQuery      fnIF_AgencyDebitQuery;
	defIF_TransferImage         fnIF_TransferImage;

	defIF_TransTickNo			fnIF_TransTickNo;

	def_GreenChannelTest        fn_GreenChannelTest;

	defIF_ExcuteComCommond fnIF_ExcuteComCommond;
	defIF_TakeComFeedbackCommond fnIF_TakeComFeedbackCommond;
	
	defIF_TerminalSystemVersionStatus fnIF_TerminalSystemVersionStatus;
	defIF_ExchangeTerminalSystem fnIF_ExchangeTerminalSystem;
};
