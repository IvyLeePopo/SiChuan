/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
* 
* �ļ����ƣ�ThreadManage.h
* ժ    Ҫ��ҵ���̨�����̣߳������ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��7��24��
*/
#pragma once
#include "ThreadBase.h"
#include "ThreadDevice.h"
#include "DVXMLDebitMoney.h"
#include "JsonDebitOperate.h"
#include <map>
using namespace std;

typedef map<CString,CString>	DEBITCANCELMAP;//���泷����Ϣ




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
	//���������߳�
	const BOOL StartWorkThreads(IN const tagTranstParam& sTranstParam);
	//�ж��û������Ƿ��Ѿ�������Ӳ���ṩ��
	const BOOL GetAccountDataReadyState(void) {return m_bAccountInfoIsReady;}
	//�����û������Ƿ�׼������
	void SetAccountDataReadyState(IN const BOOL& bState);
	//��ȡ�ϴμ��������û�����
	const tagAccountInfo GetLastAccountInfo(void){return m_sAccountInfo;}
	//���ö�ʱ��
	void StartTimer(IN const UINT& nEventID,IN const DWORD& dwDelayTime);
	//ֹͣ��ʱ��
	void StopTimer(IN const UINT& nEventID);
	//��ȡӲ����������״��
	const BOOL GetDeviceLoadStae(void) const {return m_bDeviceLoadState;}
	//Override
protected:
	//��������������Դ
	void fnReleaseResource();
	//Inner Functions
private:


	//��ȡ����ƥ����
	const CString GetNetPayRecordMatchCode(IN const CString& strEncKey=_T(""));
	//��ȡ���װ�ȫ��
	const CString GetNetPaySecurityCode(void);
	//������ɾ����־
	void CheckAndDeleteLogFile(void);
	//���׽����Ĵ���
	CString FinishTrade(IN const BOOL& bNormal);
	//��ʱ����Ӧ����
	static void CALLBACK OneMilliSecondProc(UINT nTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2);
protected:
	//����������ȡ�û���Ϣ��������Ϣ
	afx_msg void OnMsgStartGetAccountInfo(WPARAM wParam,LPARAM lParam);
	//����ֹͣ��ȡ�û���Ϣ��������Ϣ
	afx_msg void OnMsgStopGetAccountInfo(WPARAM wParam,LPARAM lParam);
	//�����ȡ�û���Ϣ����Ϣ
	afx_msg void OnMsgReceiveNorify(WPARAM wParam,LPARAM lParam);

	//��ѯ�û���������Ľ��״̬
	afx_msg void OnMsgCheckDebitPasswordState(WPARAM wParam,LPARAM lParam);
	//ִ�пۿ����(����ģʽ)
	afx_msg void OnMsgManageStartDebitFlow(WPARAM wParam,LPARAM lParam);
	//ִ�г�������
	afx_msg void OnMsgDebitCancel(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	//Attribute
public:
	//Ӳ��������ָ��
	CThreadDevice*		m_pThreadDevice;
	//�ۿ����ݽṹ��
	CDVJsonDebit		m_sDVJsonDebit;
	//Ĭ����������ĳ�ʱʱ��
	DWORD				m_dwInputPasswordDelayTime;
private:
	//�û���Ϣ�Ƿ�׼������(���ݱ�ȡ�ߺ���Ҫ����״̬)
	BOOL				m_bAccountInfoIsReady;
	//Ӳ�������������
	BOOL				m_bDeviceLoadState;
	//��ʱ���ֱ���
	UINT				m_nAccuracy;

	//��ʱ��ID(ɨ�볬ʱ)
	UINT				m_nTimerIDForScanCode;
	//��ʱ��ID(�ۿʱ)
	UINT				m_nTimerIDForDebitMoney;
	//��ʱ��ID(�ۿ�״̬��ѯ)
	UINT				m_nTimerIDForCheckDebitState;
	//�ۿ�״̬��ѯʱ����
	DWORD				m_dwCheckDebitSpanTime;
	//�ۿ�״̬��ѯʱ���ܼ�
	DWORD				m_dwCheckDebitAccumulateTime;
	//�ϴλ�ȡ���û�����
	tagAccountInfo		m_sAccountInfo;

	//��ʱ��ID(������ʱ)
	UINT				m_nTimerIDForDebitCancel;

	//�������ݽ����
	HKArray		m_HKResult;

	//������Ϣ�б�
	DEBITCANCELMAP m_DebitCancelMap;
public:
	BOOL GetHKResult(IN WORD nCmdType,OUT CString& sResult);

};


