#pragma once

#include <afxinet.h>
#include <process.h> 


#include "RemoteAccessCore.h"//远程控制放在此线程实现
#include "LogUploader.h"

// CThreadUpdate

class CThreadUpdate : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadUpdate)

	typedef struct _tagHttpParam
	{
		CString			SendData;			//发送的数据信息
		CString			URL;				//URL地址
		INTERNET_PORT	Port;				//链接端口号
		int				CallMode;			//数据递交类型(POST 或 GET)
		DWORD			ConsumeTime;		//线程处理消耗时间(毫秒)
		DWORD			ExecuteCode;		//线程处理结果代码
		CString			ServerResponseData;	//服务端返回数据
		DWORD			ServerResponseCode;	//服务端返回代码
		BOOL			NeedEndThread;		//是否强制终止线程

		DWORD			SessionReceiveTimeout;	//接收数据超时
		DWORD			SessionSendTimeout;		//发送数据超时
		DWORD			SessionControlTimeout;	//控制超时
	}tagHttpParam;

protected:
	CThreadUpdate();           // protected constructor used by dynamic creation
	virtual ~CThreadUpdate();
private://add zhuyabing 2017-12
	RemoteAccessCore m_RAC;
	LogUploader m_LogUnit;
public:
	void RemoteUpgradeResult(WPARAM wParam);
private:
	void RemoteControl();

public:
	bool	m_bExit;
	bool	m_bDownLoad;//下载数据标记

	CString	m_csWebIP;		//WEB端IP
	int		m_nWebPort;		//WEB端端口
	int		m_nHeartSpan;		//心跳发送处理间隔
	int		m_nUpdateSpan;	//更新检测处理间隔
	int		m_nDownLoadSpan;	//数据下载间隔
	int		m_nCommport;	//设备串口号

	DWORD	m_dwHeartCheck;	//心跳检测时间点
	DWORD	m_dwUpdateCheck;	//更新检测时间点
	DWORD	m_dwDwonLoadCheck;//下载检测时间点

	CString	m_csVersion;	//
	CString	m_csHeartURL;	//心跳URL
	CString	m_csQueryURL;	//查询URL
	CString	m_csObtainURL;	//获取URL
	CString	m_csConfirmURL;	//确认URL
	CString	m_csLogMsg;		//日志记录信息

	CString	m_csUpdateLDB;	//本地更新存储文件
	CString	m_csLDBTable;	//本地更新存储文件表名

	CUpdateData	c_update_data_http;	//更新信息类，记录当前的更新信息


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


private:
	
};


