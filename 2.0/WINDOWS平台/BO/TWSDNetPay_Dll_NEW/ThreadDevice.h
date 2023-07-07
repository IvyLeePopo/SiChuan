/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
* 
* �ļ����ƣ�ThreadDevice.h
* ժ    Ҫ��Ӳ�������̣߳������ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��7��23��
*/
#pragma once
#include "ThreadBase.h"
#include "HTSDDeviceDll.h"
#include "HTSDDeviceDll2.h"
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
public:
	//��ȡӲ����ʼ��״̬
	inline const BOOL GetDeviceInitState(void){if(NULL != m_pHTSDDeviceDll) return m_pHTSDDeviceDll->GetDevInitState(); return FALSE;}
	//��ȡӲ���豸״̬
	DWORD GetDeviceState(void);
public:/*add zhuyabing 201706*/
	bool BreakDebit(const CString& payID);
	bool AgencyDebitQuery(const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult);
	bool TransferImage(const std::string& szImageInfo);

	bool TransTickNo(const std::string& para);

	bool HardwareTestGreenChannel(int testType,const std::string& testPara,std::string& result);//add 2018-04 �ṩ��Ӳ����������ݵ�ר��ͨ����
	bool ExcuteComCommond(const std::string& hexStr);
	bool TakeComFeedbackCommond(CString& jsonStr);

	bool QueryTerminalSystemVersionStatus(int& status);
	bool ExchangeTerminalSystem(const int& targetSystemVersion);
protected:
	//���յ���Ϣ��Ҫ��ִ�з��Թ���
	afx_msg void OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam);
	//���յ���Ϣ�������ۿ�����
	afx_msg void OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam);
	//���յ���Ϣ��������������
	afx_msg void OnMsgDebitCancelFlow(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
private:
	//��Ӳ���·�վ��������Ϣ
	BOOL InitConfigDevice(void);
	//��ȡͨѶģ������֪ͨ����
	static void fnGetNotifyFromCommModule(int iType,const std::string& strData);
//Attribute:
private:
	//����Ӳ���汾(1,��ʼ�汾;2,1.5�汾)
	int				m_iDeviceVersion;
	//Ӳ�����ƽӿ���
	//CHTSDDeviceDll*	m_pHTSDDeviceDll;
	CHTSDDeviceDll2* m_pHTSDDeviceDll;
	//Ӳ���ϴμ���״̬
	DWORD			m_dwLastCheckState;
	//�Ƿ��·���ʼ��ָ��
	BOOL			m_bInitConfigDevice;
	//���һ�γ�ʼ��ָ���·�ʱ��
	time_t			m_tLastInit;

public:
	bool Init_Dev();
	bool Destroy_Dev();
};


