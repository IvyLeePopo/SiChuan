#pragma once
#include "Threadbase.h"
#include "HTSMDeviceControl.h"

class CThreadSMManager :
	public CThreadBase
{
	DECLARE_DYNCREATE(CThreadSMManager)

public:
	CThreadSMManager(void);
	~CThreadSMManager(void);
	//��ȡ�豸״̬��һ����ָ����ͨѶ״̬��
	DWORD	GetSMState();
	void	SetMessageID(DWORD dwMessage);
	void	GetSMResult(tagSMInnerDeviceState& sState);
	void	SetDisplay(tagScanDisplay sDisplay);
	//��ȡ����ͨѶ״̬
	inline const BOOL GetSerialCommState(void) const {return m_bSerialCommError;}
	//��ȡ�豸Ӳ��״̬
	inline const DWORD GetDeviceState(void) const {return 0;}

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
protected:
	DECLARE_MESSAGE_MAP()

	/*******************************�ڲ�������Ϣ����������*******************************/
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
private:	
	//����ɨ������������
	void	fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState);
//Attributes
private:
	/**************************************����˿ڲ�������**************************************/
	//���ڴ򿪱��
	BOOL	m_bOpen;
	//�����ϴεı��
	BOOL	m_bCOMLastOpenState;
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

	//�Ƿ�����־�д�ӡ���͵���������
	BOOL	m_bPrintCommandDataInLog;

	//����ɨ��״̬
	DWORD	m_dwSMState;
	DWORD	m_dwMessageID;
	tagSMInnerDeviceState	sSMResult;
	tagScanDisplay			sScanDisplay;

	//���ھ��
	HANDLE		m_hCom;	
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
};
