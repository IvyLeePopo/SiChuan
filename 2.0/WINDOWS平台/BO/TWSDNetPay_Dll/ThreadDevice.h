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
	inline const BOOL GetDeviceInitState(void){if(NULL != m_pHTSDDeviceDll) return m_pHTSDDeviceDll->GetDevInitState(); return FALSE;}
	//��ȡӲ���豸״̬
	DWORD GetDeviceState(void);

public:
	//��ȡC0֡���ݵĽ������
	BOOL GetC0SuccFailedCount(OUT int& dwSucce, OUT int& dwFailed, OUT int& nReSendCount, OUT CString& strA2Frame, OUT CString& strC0Frame);

protected:

	//��ӦӲ�����͵���Ϣ
	afx_msg void OnMsgGetData(WPARAM wParam,LPARAM lParam);

	//ִ��������ȡ���ݵĲ���
	afx_msg void OnMsgStartGetData(WPARAM wParam,LPARAM lParam);

	//ִ��ֹͣ��ȡ���ݵĲ���
	afx_msg void OnMsgStopGetData(WPARAM wParam,LPARAM lParam);

	//���յ���Ϣ��Ҫ��ִ�з��Թ���
	afx_msg void OnMsgShowDealInfo(WPARAM wParam,LPARAM lParam);

	//���յ���Ϣ�������ۿ�����
	afx_msg void OnMsgStartDebitFlow(WPARAM wParam,LPARAM lParam);

	//���յ���Ϣ��������������
	afx_msg void OnMsgDebitCancelFlow(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	//��Ӳ���·�վ��������Ϣ��һ��Ҫ��֤�ɹ�
	const CString InitConfigDevice(int nFormatType = 1);

	//������ѯ����ʱ��
	time_t m_tLastCheckBoard; 

	//Attribute:
private:
	//����Ӳ���汾(1,��ʼ�汾;2,1.5�汾)
	int				m_iDeviceVersion;
	//Ӳ�����ƽӿ���
	CHTSDDeviceDll*	m_pHTSDDeviceDll;
	//Ӳ���ϴμ���״̬
	DWORD			m_dwLastCheckState;
	//�Ƿ��·���ʼ��ָ��
	BOOL m_bInitConfigDevice;



};


