#pragma once

#include <afxinet.h>
#include <process.h> 


// CThreadUpdate

class CThreadUpdate : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadUpdate)

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

protected:
	CThreadUpdate();           // protected constructor used by dynamic creation
	virtual ~CThreadUpdate();

public:
	bool	m_bExit;
	bool	m_bDownLoad;//�������ݱ��

	CString	m_csWebIP;		//WEB��IP
	int		m_nWebPort;		//WEB�˶˿�
	int		m_nHeartSpan;		//�������ʹ�����
	int		m_nUpdateSpan;	//���¼�⴦����
	int		m_nDownLoadSpan;	//�������ؼ��
	int		m_nCommport;	//�豸���ں�

	DWORD	m_dwHeartCheck;	//�������ʱ���
	DWORD	m_dwUpdateCheck;	//���¼��ʱ���
	DWORD	m_dwDwonLoadCheck;//���ؼ��ʱ���

	CString	m_csVersion;	//
	CString	m_csHeartURL;	//����URL
	CString	m_csQueryURL;	//��ѯURL
	CString	m_csObtainURL;	//��ȡURL
	CString	m_csConfirmURL;	//ȷ��URL
	CString	m_csLogMsg;		//��־��¼��Ϣ

	CString	m_csUpdateLDB;	//���ظ��´洢�ļ�
	CString	m_csLDBTable;	//���ظ��´洢�ļ�����

	CUpdateData	c_update_data_http;	//������Ϣ�࣬��¼��ǰ�ĸ�����Ϣ


	void	ThrQuit();
	void	ReadWebConfig();

	CString	StringConvertByCodePage(CString strConvertData, int iSourceCodepage, int iTargetCodepage);
	void	HTTPDeal(LPVOID lpParam);

	void	MainHeart();
	void	ReportHeartState();
	int		ComputeAES_MD5(int nProvince, CString strVer, CString strDevno, CString strDevType, time_t tTime, CString& strAes, CString& strMD5);
	CString GetDeviceNo(int nProvince, int nRoad, CString strSTID, int nLane);
	CString GetHeartJson(int nState);

	void	MainUpdate();
	void	CheckHTTPUpdate();
	CString GetQueryJson(CUpdateData& cUpdate);
	CString GetSafeJsonString(Json::Value j_value, int nLen, bool bTime=false);
	bool	CheckQueryResult(CString csResult);

	void	MainDownLoad();
	void	DownLoadHTTPUpdate();
	CString GetDownLoadJson(CUpdateData& cUpdate);
	bool	CheckDownLoadResult(CString csResult);


	CString GetConfirmJson(CString strCode, int nState);


	int		CreateLocalDB3(LPCTSTR strDBFile, LPCTSTR strTableName, CString& csError);
	int		SaveData2LocalDB3(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, CString& csError);
	int		GetLocalDB2Array(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, int nType, CString& csError);

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnUpdateConfirm(WPARAM wParam,LPARAM lParam);

};


