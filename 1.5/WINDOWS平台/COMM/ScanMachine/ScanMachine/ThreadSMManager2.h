/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ThreadSMManager.h
 * ժ    Ҫ��ɨ���������߳��࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
#pragma once
#include "ThreadBase.h"
#include "HTSMDeviceControl.h"
#include "SerialCom.h"


#include "FirmwareUpgrade.h"
// CThreadSMManager2


class CThreadSMManager2 : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadSMManager2)

public:
	CThreadSMManager2();           // protected constructor used by dynamic creation
	virtual ~CThreadSMManager2();

	//��ȡ�豸״̬��һ����ָ����ͨѶ״̬��
	DWORD	GetSMState();
	void	SetMessageID(DWORD dwMessage);
	void	GetSMResult(tagSMInnerDeviceState& sState);
	void 	GetDeviceResponse(tagDeviceFeedback& feedback);

	void	SetDisplay(tagScanDisplay sDisplay);
	void  	SetTransData(const tagTransmissionData& trans_data);

	void    SetTTSMessage(const std::string& msg);
	void	GGDeal();

	//��ȡ����ͨѶ״̬
	inline const BOOL GetSerialCommState(void) const {return m_bSerialCommError;}
	//��ȡ�豸Ӳ��״̬
	inline const DWORD GetDeviceState(void) const {return 0;}

public://add zhuyabing 2017-12
	void FirmwareUpgradeInfo(const std::string& paraJson);
	bool IsUpgrading();
	bool QueryTerminalSystemStatus(int& status);
	bool ExcuteSystemSwitch(const int& targetVer);
private://add zhuyabing 2017-12
	FirmwareUpgrade m_firmwareRemoteControl;

	void FirmwareUpgradeProcess();
	void FreeDisplayUpgrade();
	
	bool NewSystemSwitchRespond(const int &val);
	bool TakeOneSystemSwitchRespond(int& val);
	bool ClearSystemSwitchRespond();
	bool NewSystemStatusRespond(const int& val);
	bool TakeOneSystemStatusRespond(int& val);
	bool ClearSystemStatusRespond();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
//���غ���
protected:
	//��������������Դ
	void fnReleaseResource();
	//��ȡ������ص�������Ϣ(��ָ�������ļ�)
	void fnReadSpecailConfig();
private://add 2017-10 ��������������֡������
	void HandleComDataEx(WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()

	/*******************************�ڲ�������Ϣ����������*******************************/
	//�ڲ�������Ϣ���������� -- ��ȡ����������
	afx_msg void OnMsgReceiveDataFromCOM(WPARAM wParam,LPARAM lParam);
	//֪ͨ�������������Ѿ��������
	afx_msg void OnMsgDataSendEnd(WPARAM wParam,LPARAM lParam);
	//֪ͨ�����ߴ��ڷ�������
	afx_msg void OnMsgComError(WPARAM wParam,LPARAM lParam);
	//�ڲ�������Ϣ���������� -- ��������Э�����ʽ���������
	afx_msg void OnMsgDealResponseForProtocolProcessResult(WPARAM wParam,LPARAM lParam);
	
	/*******************************���յ�������Ϣ������*******************************/
	//���յ�������Ϣ������ -- ǿ�ƿ�ʼɨ�蹤��
	afx_msg void OnMsgStartScan(WPARAM wParam,LPARAM lParam);
	//���յ�������Ϣ������ -- ǿ�ƽ���ɨ�蹤��
	afx_msg void OnMsgStopScan(WPARAM wParam,LPARAM lParam);
	//���ܵ�������Ϣ������ -- ���ƴ��ڰ����
	afx_msg void OnMsgControlSerialBoard(WPARAM wParam,LPARAM lParam);
	//���յ�������Ϣ������ -- ���ƴ��ڰ����ģʽ
	afx_msg void OnMsgSetLightMode(WPARAM wParam,LPARAM lParam);

	//���յ�������Ϣ������ -- ������ʾ����
	afx_msg void OnMsgSetDisplay(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMsgTransmission(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMsgTransmissionResponse(WPARAM wParam,LPARAM lParam);


	//����TTS������������
	afx_msg void OnMsgTTS(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMsgRemoteControlResult(WPARAM wParam,LPARAM lParam);


//Operations
private:
	/**************************************����˿ڲ�������**************************************/
	//����˿ڲ������� -- �򿪴�������
	BOOL	OpenCom(void);
	//����˿ڲ������� -- �رմ�������
	BOOL	CloseCom(void);
	//����˿ڲ������� -- ������д���ݣ��ֽ�����ʽ
	DWORD	WriteDataToComm(IN const LPBYTE byData,IN const DWORD& dwSize);
	//����˿ڲ������� -- ������д���ݣ��ַ�����ʽ
	BOOL	WriteDataToComm(IN const CString& strWriteData);
	
	/**************************************�ڲ����̲�������**************************************/
	//�ڲ����̲������� -- ִ��ɨ�����̲���
	void	fnSMScanCode(void);
	//�ڲ����̲������� -- ִ��ֹͣ���̲���
	void	fnSMStopScan(void);
public:
	//�ڲ����̿��ƺ��� -- ���Ƶƹ�
	void	fnSMControlLight(IN const int& iLightType);
	//���ͻ�ȡ�ն����кŵ�ָ��
	void	fnSendGetDeviceSerialNoCMD(void);


	//add 2017-12 �̼��汾����Ϣ
	void    fnSendFirmwareVerCMD(void);
	void    fnSendSlaveFirmwareVerCMD(void);//add 2018-01 ��ȡ�ӻ��汾��
private:	
	//����ɨ������������
	void	fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState);
//Attributes

	/**************************************����˿ڲ�������**************************************/
public:
	//���ڴ򿪱��
	BOOL	m_bOpen;
private:
	//�����ϴεı��
	BOOL	m_bCOMLastOpenState;
	//���ڿ�����
	CSerialCom*		m_pCOM;
	//�ϴμ�¼����״̬��ʱ��
	time_t	m_tLastRecordCOMStateTime;
	//��дͬ������
	HANDLE	m_hSyncComData;		
	//������ָ��
	LPBYTE	m_lpReceiveDataBuff;	
	//��������������ָ������
	DWORD	m_dwReceiveDataIndex;	
	//����ͨѶ���ڲ����ṹ
	tagCOMParam	m_sLocalCOM;
	//����ͨѶר��״̬��
	tagSMInnerDeviceState m_sCOMState;
	//����ר��״̬��
	tagSMInnerDeviceState m_sHeartState;

	//ɨ����Э�������
	CHTSMDeviceControl*	m_pSMProtocol;
	//ɨ������ʱ��
	SYSTEMTIME m_stBeginTime;
	DWORD	m_dwScanStartTime;
	//����ɨ��ָ��������֮��ĳ�Ĭ���
	DWORD	m_dwSpanDelayTimeForControlLight;
	//�Ƿ������ȡɨ����Ϣ��ر�ɨ����ʾ��
	BOOL	m_bAllowCloseScanTipLight;
	//����������ʱ��(�ж��д������),��λ����
	int		m_iCheckHeartErrorSpanTime;
	//��ʶ����ͨѶ�쳣
	BOOL	m_bSerialCommError;

	//���ڼ����������������
	time_t	m_tNoDataFromCOMLastTime;
	//�Ƿ�����־�д�ӡ���͵���������
	BOOL	m_bPrintCommandDataInLog;

	//����ɨ��״̬
	DWORD	m_dwSMState;
	DWORD	m_dwMessageID;
	tagSMInnerDeviceState	m_sSMResult;
	tagScanDisplay			sScanDisplay;

	tagTransmissionData mTransData;/*add 2017-09*/
	tagTransmissionFeedback mTransResponse;

	//�����ֽ�����ʱ��,��ʱʱ�����ã���λ��MS Ĭ��Ϊ10
	int			m_iNoNewDataTime;
	//�ϴδ��ڶ�ȡ�����ݵ�ʱ��
	time_t		m_tLastComReadDataTime;
	//�ϴδ��ڷ������ݳɹ���ʱ��
	time_t		m_tLastComWriteDataTime;
	//���ڳ����쳣��λ�ļ��ʱ��
	int			m_iResetCOMErrorSpanTime;
	//�Ƿ�����ͬ�������ݣ������һ�����յ������ݣ�
	BOOL		m_bAbandonEchoData;

	// 1017�汾�����������ѷ���ָ��ָ����������һ�μ��ʱ��
	int			m_nSendCmdCount;	//����ָ�����
	DWORD		m_dwLastChecktick;	//�����ʱ��Ƭ
	DWORD		m_dwAnswerCheckSpan;//Ӧ����ռ��ʱ����

	HANDLE		hGGMedia;
	//�汾 1018�������ڹ�沥�ŵĶ�ý�����
	CArray<tagMMIMedia, tagMMIMedia&> aryMedia;

	private:
		std::string m_SoundMsg;//add 2017-11 ��������
	CRITICAL_SECTION m_ComRepondSyncMutex;
	std::list<int> m_SystemSwitchRespondQueue;
	std::list<int> m_SystemStatusRespondQueue;
};


