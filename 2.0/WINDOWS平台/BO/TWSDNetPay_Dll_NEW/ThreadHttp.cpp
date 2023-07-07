/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 *
 * 文件名称：ThreadHttp.cpp
 * 摘    要：完成与收费中心WEB服务器的所有通讯工作，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月12日
 */
// ThreadHttp.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadHttp.h"

// CThreadHttp

IMPLEMENT_DYNCREATE(CThreadHttp, CThreadBase)

CThreadHttp::CThreadHttp()
	: m_wQueryEntryPort(80)
	, m_wQueryExitPort(80)
	, m_wDebitPort(80)
	, m_wDebitSecondPort(80)
	, m_wDebitCheckPort(80)
	, m_wDebitCancelPort(80)
	, m_wOpenLoopPort(80)
	, m_wCloseLoopPort(80)
	, m_wPassStationPort(80)
	, m_wSendRecordPort(80)
	, m_wHeartPort(80)

	, m_strQueryEntryURL(_T(""))
	, m_strQueryExitURL(_T(""))
	, m_strDebitURL(_T(""))
	, m_strDebitSecondURL(_T(""))
	, m_strDebitCheckURL(_T(""))
	, m_strDebitCancelURL(_T(""))
	, m_strOpenLoopURL(_T(""))
	, m_strCloseLoopURL(_T(""))
	, m_strPassStationURL(_T(""))
	, m_strHeartURL(_T(""))

	, m_strSendRecordURL(_T(""))

	, m_dwHeratFailForCenterReflectWeb(0)
	, m_dwHeratFailForOperationServer(0)
	, m_dwHeratFailWarningTimes(2)

	, m_bResendWorking(FALSE)
	, m_tResendDataSpanTime(30)
	, m_dwWaitThreadWorkTimeout(6000)
	, goDie(false)
	, dieNumber(0)
	, isAlive(false)
{
	::InitializeCriticalSection(&idleMutex);
	isFirstRun=true;

}

CThreadHttp::~CThreadHttp()
{
	//::DeleteCriticalSection(&idleMutex);
}


BOOL CThreadHttp::InitInstance()
{
	m_bLoopBreak = FALSE;

	//设置本地配置文件
	m_strPathLocalConfigFile.Format(_T("%s\\ReflectWeb.ini"), m_sTranstParam.PathCommDir);
	//读取配置文件，获得标准的配置信息，填充URL和指定端口
	fnReadSpecailConfig();

	return TRUE;
}

int CThreadHttp::ExitInstance()
{
	::DeleteCriticalSection(&idleMutex);
	fnReleaseResource();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadHttp, CWinThread)
	ON_THREAD_MESSAGE(WM_THREAD_HTTP_SEND_DATA_REQUEST, OnMsgSendQueryToReflectWeb)
	ON_THREAD_MESSAGE(WM_THREAD_HTTP_SEND_HEART, OnMsgSendHeart)
END_MESSAGE_MAP()

UINT CThreadHttp::GetRandNumber(IN const UINT& nMin, IN const UINT& nMax)
{
	UINT nReturn = 0;
	UINT nRealMax = (0 == nMax) ? 1 : nMax;
	srand( (unsigned)::GetTickCount());
	nReturn = rand() % (nMax - nMin + 1)  + nMin ;
	//返回数据
	return nReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::CheckUnProcessDebitCancelRecord
//
// 功能描述：检测处理是否有未递交的撤单数据
//
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::CheckUnProcessDebitCancelRecord()
{
	// return ;//debug
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::fnReleaseResource
//
// 功能描述：资源清理函数
//
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年11月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：重载函数
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::fnReleaseResource()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::fnReadSpecailConfig
//
// 功能描述：资源清理函数
//
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年11月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：重载函数
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::fnReadSpecailConfig()
{
	TCHAR szTemp[512] = {0};

	TCHAR szIP[32] = {0};
	WORD  wPort = 80;
	BOOL  bAddPort = FALSE;

	//获取公共配置信息
	CString strCommConfigFile;
	strCommConfigFile.Format(_T("%s\\%s"), m_sTranstParam.PathCommDir, FILE_NAME_APP_CONFIG);
	//1.0.3.4版本，调整公网测试环境IP，由60.205.140.198更改为：60.205.140.198
	::GetPrivateProfileString(_T("ReflectWeb"), _T("IP"), _T("60.205.140.198"), szIP, _countof(szIP), strCommConfigFile);
	wPort = (WORD)::GetPrivateProfileInt(_T("ReflectWeb"), _T("Port"), 8081, strCommConfigFile);
	bAddPort = (BOOL)::GetPrivateProfileInt(_T("ReflectWeb"), _T("AddPort"), 0, strCommConfigFile);
	//子线程等待时间,单位:毫秒，默认超时时间为 6 秒
	m_dwWaitThreadWorkTimeout = (DWORD)::GetPrivateProfileInt(_T("ReflectWeb"), _T("WaitThreadWorkTimeout"), 6000, strCommConfigFile);

	switch (m_sTranstParam.ProvinceID)
	{
	case 63:	//青海
	{
		if (::PathFileExists(m_strPathLocalConfigFile))
		{
			m_wDebitPort = (WORD)::GetPrivateProfileInt(_T("Debit"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Debit"), _T("URL"), _T("http://10.63.0.91/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitURL.Format(_T("%s"), szTemp);

			m_wDebitSecondPort = (WORD)::GetPrivateProfileInt(_T("DebitSecond"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitSecond"), _T("URL"), _T("http://10.63.0.91/repay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitSecondURL.Format(_T("%s"), szTemp);

			m_wDebitCheckPort = (WORD)::GetPrivateProfileInt(_T("DebitCheck"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCheck"), _T("URL"), _T("http://10.63.0.91/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCheckURL.Format(_T("%s"), szTemp);

			m_wDebitCancelPort = (WORD)::GetPrivateProfileInt(_T("DebitCancel"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCancel"), _T("URL"), _T("http://10.63.0.91/home1"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCancelURL.Format(_T("%s"), szTemp);

			m_wHeartPort = (WORD)::GetPrivateProfileInt(_T("Heart"), _T("Port"), 80, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Heart"), _T("URL"), _T("http://10.63.0.91/heartbeat"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strHeartURL.Format(_T("%s"), szTemp);
		}
		else
		{
			if (bAddPort)
			{
				//扣款URL
				m_wDebitPort		= wPort;
				m_strDebitURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitPort);

				//查询URL
				m_wDebitCheckPort	= wPort;
				m_strDebitCheckURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitCheckPort);

				//撤单URL
				m_wDebitCancelPort	= wPort;
				m_strDebitCancelURL.Format(_T("http://%s:%d/pay/reverse"), szIP, m_wDebitCancelPort);

				//心跳URL
				m_wHeartPort		= wPort;
				m_strHeartURL.Format(_T("http://%s:%d/heartbeat"), szIP, m_wHeartPort);

			}
			else
			{
				//扣款URL
				m_strDebitURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitPort		= wPort;
				//查询URL
				m_strDebitCheckURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitCheckPort	= wPort;
				//撤单URL
				m_strDebitCancelURL.Format(_T("http://%s/pay/reverse"), szIP);
				m_wDebitCancelPort	= wPort;
				//心跳URL
				m_strHeartURL.Format(_T("http://%s/heartbeat"), szIP);
				m_wHeartPort		= wPort;
			}
		}
	}
	break;
	case 44:	//广东
	{
		if (::PathFileExists(m_strPathLocalConfigFile))
		{
			switch (m_sTranstParam.LaneConfigInfo.RoadID)
			{
			case 30:
			case 31:
			{
				m_wDebitPort = (WORD)::GetPrivateProfileInt(_T("Debit"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("Debit"), _T("URL"), _T("http://10.44.125.241/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitURL.Format(_T("%s"), szTemp);

				m_wDebitSecondPort = (WORD)::GetPrivateProfileInt(_T("DebitSecond"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("DebitSecond"), _T("URL"), _T("http://10.44.125.241/repay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitSecondURL.Format(_T("%s"), szTemp);

				m_wDebitCheckPort = (WORD)::GetPrivateProfileInt(_T("DebitCheck"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("DebitCheck"), _T("URL"), _T("http://10.44.125.241/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitCheckURL.Format(_T("%s"), szTemp);

				m_wDebitCancelPort = (WORD)::GetPrivateProfileInt(_T("DebitCancel"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("DebitCancel"), _T("URL"), _T("http://10.44.125.241/pay/reverse"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strDebitCancelURL.Format(_T("%s"), szTemp);

				m_wHeartPort = (WORD)::GetPrivateProfileInt(_T("Heart"), _T("Port"), 80, m_strPathLocalConfigFile);
				::GetPrivateProfileString(_T("Heart"), _T("URL"), _T("http://10.44.125.241/heartbeat"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
				m_strHeartURL.Format(_T("%s"), szTemp);
			}
			break;
			}
		}
		else
		{
			if (bAddPort)
			{
				//扣款URL
				m_wDebitPort		= wPort;
				m_strDebitURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitPort);

				//查询URL
				m_wDebitCheckPort	= wPort;
				m_strDebitCheckURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitCheckPort);

				//撤单URL
				m_wDebitCancelPort	= wPort;
				m_strDebitCancelURL.Format(_T("http://%s:%d/pay/reverse"), szIP, m_wDebitCancelPort);

				//心跳URL
				m_wHeartPort		= wPort;
				m_strHeartURL.Format(_T("http://%s:%d/heartbeat"), szIP, m_wHeartPort);

			}
			else
			{
				//扣款URL
				m_strDebitURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitPort		= wPort;
				//查询URL
				m_strDebitCheckURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitCheckPort	= wPort;
				//撤单URL
				m_strDebitCancelURL.Format(_T("http://%s/pay/reverse"), szIP);
				m_wDebitCancelPort	= wPort;
				//心跳URL
				m_strHeartURL.Format(_T("http://%s/heartbeat"), szIP);
				m_wHeartPort		= wPort;
			}
		}
	}
	break;
	default: //其他省
	{
		if (::PathFileExists(m_strPathLocalConfigFile))
		{
			m_wDebitPort = (WORD)::GetPrivateProfileInt(_T("Debit"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Debit"), _T("URL"), _T("http://60.205.140.198:8081/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitURL.Format(_T("%s"), szTemp);

			m_wDebitSecondPort = (WORD)::GetPrivateProfileInt(_T("DebitSecond"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitSecond"), _T("URL"), _T("http://60.205.140.198:8081/repay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitSecondURL.Format(_T("%s"), szTemp);

			m_wDebitCheckPort = (WORD)::GetPrivateProfileInt(_T("DebitCheck"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCheck"), _T("URL"), _T("http://60.205.140.198:8081/pay"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCheckURL.Format(_T("%s"), szTemp);

			m_wDebitCancelPort = (WORD)::GetPrivateProfileInt(_T("DebitCancel"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("DebitCancel"), _T("URL"), _T("http://60.205.140.198:8081/pay/reverse"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strDebitCancelURL.Format(_T("%s"), szTemp);

			m_wHeartPort = (WORD)::GetPrivateProfileInt(_T("Heart"), _T("Port"), 8081, m_strPathLocalConfigFile);
			::GetPrivateProfileString(_T("Heart"), _T("URL"), _T("http://60.205.140.198:8081/heartbeat"), szTemp, _countof(szTemp), m_strPathLocalConfigFile);
			m_strHeartURL.Format(_T("%s"), szTemp);
		}
		else
		{
			if (bAddPort)
			{
				//扣款URL
				m_wDebitPort		= wPort;
				m_strDebitURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitPort);

				//查询URL
				m_wDebitCheckPort	= wPort;
				m_strDebitCheckURL.Format(_T("http://%s:%d/pay"), szIP, m_wDebitCheckPort);

				//撤单URL
				m_wDebitCancelPort	= wPort;
				m_strDebitCancelURL.Format(_T("http://%s:%d/pay/reverse"), szIP, m_wDebitCancelPort);

				//心跳URL
				m_wHeartPort		= wPort;
				m_strHeartURL.Format(_T("http://%s:%d/heartbeat"), szIP, m_wHeartPort);
 
			}
			else
			{
				//扣款URL
				m_strDebitURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitPort		= wPort;
				//查询URL
				m_strDebitCheckURL.Format(_T("http://%s/pay"), szIP);
				m_wDebitCheckPort	= wPort;
				//撤单URL
				m_strDebitCancelURL.Format(_T("http://%s/pay/reverse"), szIP);
				m_wDebitCancelPort	= wPort;
				//心跳URL
				m_strHeartURL.Format(_T("http://%s/heartbeat"), szIP);
				m_wHeartPort		= wPort;
			}
		}
	}
	break;
	}
	//心跳检测失败次数达到报警的阈值
	m_dwHeratFailWarningTimes	= (DWORD)::GetPrivateProfileInt(_T("Heart"), _T("FailWarningTims"), 2, m_strPathLocalConfigFile);
	m_m_dwHeartCheckMin			= (DWORD)::GetPrivateProfileInt(_T("Heart"), _T("CheckMin"), 3600, m_strPathLocalConfigFile);
	m_m_dwHeartCheckMax			= (DWORD)::GetPrivateProfileInt(_T("Heart"), _T("CheckMax"), 3720, m_strPathLocalConfigFile);

	m_tResendDataSpanTime = ::GetGlobalApp()->m_tResendDataSpanTime;
}

// CThreadHttp message handlers

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::OnIdle
//
// 功能描述：与收费中心WEB服务器通讯线程类，空闲处理函数
//
// 输入参数：LONG lCount
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2015年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadHttp::OnIdle(LONG lCount)
{
	CScopeLock tmpIdle(&idleMutex);
	if (m_bExit)
	{
		dieNumber = 2017;
		::PostQuitMessage(0);
		return FALSE;
	}
	

	CString strLog;
	//记录上次检测时间
	static time_t tLastHeartOpertateTime = 0;
	static INT64 iCheckHeartTimes = 0x00;
	static time_t tLastCheckResendTime = 0x00;

	static time_t tLastHeartSpanTime = 0;

	if ( (time(NULL) - tLastHeartOpertateTime) > tLastHeartSpanTime)
	{
		//MessageBox(0,"hearet","",0);
		//进行一轮心跳检测
		OnMsgSendHeart(3, 1000);
		//MessageBox(0, "afeter beat", "", 0);
		//重设检测时间
		tLastHeartOpertateTime	= time(NULL);
		//重设心跳检测间隔
		tLastHeartSpanTime		= GetRandNumber(m_m_dwHeartCheckMin, m_m_dwHeartCheckMax);
	}



	//检测是否需要处理本地未递交的撤单数据
	if ( (time(NULL) - tLastCheckResendTime) > m_tResendDataSpanTime)
	{
		CheckUnProcessDebitCancelRecord();
		//重设上次检测时间
		tLastCheckResendTime = time(NULL);
	}

	//线程存活日志记录检测
	static time_t tLastHttpLogTime = 0;

	if ( (time(NULL) - tLastHttpLogTime) > GetGlobalApp()->m_dwThreadCheckSpanTime)
	{
		strLog.Format(_T("[支付通讯线程],线程存活日志检测"));
		GetGlobalApp()->RecordLog(strLog);
		//重设检测时间
		tLastHttpLogTime = time(NULL);
	}

	::Sleep(100);



	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::OnMsgSendQueryToReflectWeb
//
// 功能描述：给指定的WEB服务器发送查询请求
//
// 输入参数：WPARAM wParam	--	发送的请求数据指针
//         ：LPARAM lParam	--	发送的请求类型(扣费、扣费状态查询、撤单)
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年11月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_HTTP_SEND_DATA_REQUEST
//			 回馈消息 WM_WND_CHECK_SERVER_RESPONSE_QUERY_RESULT
////////////////////////////////////////////////////////////////////////////////////////////////
/*
void CThreadHttp::OnMsgSendQueryToReflectWeb(WPARAM wParam,LPARAM lParam)
{
	//设置工作状态
	m_bWorking = TRUE;

	//锁定
//	Lock();

	CString strLog;
	DWORD	dwServerReturnResponseCode = 0x00;

	//解析数据
	WORD wSendDataType				= LOWORD(lParam);
	//是否需要回馈调用者
	BYTE byIsNeedResponseCaller		= LOBYTE(HIWORD(lParam));
	//是否需要检索本地的撤单数据
	BYTE byIsNeedRetrievalLocalData	= HIBYTE(HIWORD(lParam));;
	//回馈结果代码
	WORD wReturnCode = 0x00;
	//数据操作类指针
	CJsonOperationIF* pJsonIF = NULL;
	//转化成需要发送的数据格式载体
	CString strSendData;
	//获得需要传递的数据
	CDVJsonBase* pDVJsonReceive = (CDVJsonBase*)wParam;

	//获取当前使用的URL地址
	CString strURL;
	//获取当前使用的端口号
	INTERNET_PORT wPort = 80;
	//当前调用方式
	int	iCallMode = 1;//默认GET方式获取数据
	//调用者的窗体句柄
	HWND hWnd = NULL;

	//服务器返回的数据
	CString strResponseResult;
	//服务器返回的状态码
	DWORD dwResponseStateCode = 0x00;
	//服务器处理时间
	DWORD dwConsumeTime = 0x00;

	//构造参数
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
	try
	{
		if(NULL == pDVJsonReceive)
			throw -1;
		//根据发送过来的数据类型，进行数据格式和转化，并获得服务端链接信息
		if(!DataConvertFromJson2String(wSendDataType,pDVJsonReceive,strURL,wPort,strSendData,hWnd,iCallMode,&pJsonIF))
			throw -2;
		strLog.Format(_T("[支付通讯线程]:已接收到请求[类型:%s,%s]，准备发送数据"),GetSendDataTypeDesc(wSendDataType),pDVJsonReceive->PayIdentifier);
		GetGlobalApp()->RecordLog(strLog);
		strLog.Empty();

		//标记扣款流程接收到支付凭证(整合模式有效)
		if(GetGlobalApp()->m_bCurrentDebitMode)
		{
			if(JSON_DT_SEND_DIBIT == wSendDataType)
			{
				GetGlobalApp()->m_iDebitTaskNo = 0x03;
			}
		}

		DWORD	dwThreadID = 0;
		static tagHttpParam sParam = {0};

		//填充参数结构体
		sParam.URL					= _T("");
		sParam.ConsumeTime			= 0;
		sParam.SendData				= _T("");
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout= m_dwWaitThreadWorkTimeout - 2000;
		sParam.SessionControlTimeout= m_dwWaitThreadWorkTimeout - 1000;

		sParam.CallMode	= iCallMode;
		sParam.Port		= wPort;
		sParam.URL		= strURL;
		//转化成UTF-8格式
		if(strSendData.IsEmpty())
		{
			strSendData.Format(_T("Illegal Data Format"));
		}
		sParam.SendData	= StringConvertByCodePage(strSendData,CP_ACP,CP_UTF8);

		//发送数据，获取回馈
		if( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
			throw -3;
		//等待超时
		if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,m_dwWaitThreadWorkTimeout))
		{
			//终止线程
			sParam.NeedEndThread = TRUE;
		}
		//获取结果
		dwConsumeTime		= sParam.ConsumeTime;
		dwResponseStateCode	= sParam.ServerResponseCode;
		strResponseResult	= sParam.ServerResponseData;

		//如果是主动终止发送子线程，或获取到的结果回馈不是成功，判定超时
		if( ( TRUE == sParam.NeedEndThread ) || ( OP_SUCCESS != sParam.ExecuteCode ) )
		{
			if( OP_SUCCESS != sParam.ExecuteCode )
			{
				//标记需要进行心跳
				PostThreadMessage(WM_THREAD_HTTP_SEND_HEART,(WPARAM)0x03,(LPARAM)0);
			}
			//如果是撤单，则需要判断是否需要保存撤单数据
			if(JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
			{
				//保存撤单信息
				CDVDebitCancel cData;
				cData.PayIdentifier = ((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier;
				cData.ProvinceID    = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
				cData.RoadID		= ::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.RoadID;
				cData.StationID		= _ttoi(::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
				cData.SubTime		= pDVJsonReceive->SubmitTime;
				cData.CreateTime	= CTime(time(NULL));
				m_cTableDebitCancel.Save(cData);
			}
			//抛出异常，描述通讯超时
			throw -0xF0;
		}

		//解析获得的数据,得出格式化结果
		CDVJsonBase* pDVJsonBase = pJsonIF->FormatResponseResult2Class(strResponseResult,wSendDataType);
		if(NULL == pDVJsonBase)
			throw -4;
		pDVJsonBase->ProvinceID      = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
		pDVJsonBase->m_dwConsumeTime = dwConsumeTime;

#ifdef _DEBUG
		//记录日志
		strSendData.TrimRight();
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:发送数据到服务器(请求[类型:%s])，详细数据:%s;获取回馈数据:%s"),GetSendDataTypeDesc(wSendDataType),strSendData,strResponseResult));
#endif
		//判断服务器回馈结果
		dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;

		//重置服务器检测状态为正常
		m_dwHeratFailForCenterReflectWeb	= 0;
		m_dwHeratFailForOperationServer		= 0;

		//如果是扣费或扣费查询
		if(   (JSON_DT_SEND_DIBIT	    == wSendDataType)
			||(JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
			)
		{
			//返回代码不在以下列表（成功/输入密码/二维码超时/二维码非法/余额不足/密码错误），则认为是内部参数异常
			if(   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_SRC_ILLGAL				!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	!= pDVJsonBase->ServerResponseCode)
				&&(HTTP_RC_INPUT_PASSWORD_ERROR			!= pDVJsonBase->ServerResponseCode)
				)
			{
				//抛出异常
				throw -5;
			}
		}
		else if(JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
		{
			//检索是否需要处理本地的撤单数据
			if(byIsNeedRetrievalLocalData)
			{
				m_cTableDebitCancel.UpdateProcessMark(((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier);
				m_cTableDebitCancel.Delete();
			}
		}
		else
		{
			throw -7;
		}

		//通知调用者结果
		CString strNeedNotifyCaller(_T(",不需要通知调用者"));
		if(0x01 == byIsNeedResponseCaller)
		{
			if(0 != m_sTranstParam.CallerThreadID)
			{
				strNeedNotifyCaller.Format(_T(",已通知调用者"));
				::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT,(WPARAM)pDVJsonBase,MAKELONG(wSendDataType,HTTP_RC_DATA_LEGAL ));
			}
		}
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:已接收到服务器回馈[请求类型:%s,%s]，状态:处理成功%s"),GetSendDataTypeDesc(wSendDataType),pDVJsonBase->PayIdentifier,strNeedNotifyCaller));
		//如果通讯状态异常，则在此进行纠正
		if(   (FALSE != GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
			||(FALSE != GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist)
			)
		{
			//重置服务器检测状态为正常
			m_dwHeratFailForCenterReflectWeb = 0;
			m_dwHeratFailForOperationServer = 0;
			//设置服务器状态 -- 收费专网反射服务器异常
			GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
			//设置服务器状态 -- 运营平台服务器异常
			GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://传入参数指针为空
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,传入参数指针为空,回馈业务管理线程代码为:0x01(内部错误)"));
			break;
		case -2://数据格式转换出现异常
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,数据格式转换出现异常,回馈业务管理线程代码为:0x02(内部错误)"));
			break;
		case -3://无法启动数据发送线程
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,无法启动数据发送线程,回馈业务管理线程代码为:0x03(内部错误)"));
			break;
		case -4://回馈数据显示失败
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,回馈业务管理线程代码为:0x04(内部错误)，回馈数据异常，无法解析"));
			break;
		case -5://扣费或查询，回馈结果非成功或正在输入密码，判定内部错误
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据格式成功[请求类型:%s],回馈业务管理线程代码为:0x05(内部错误)，详细描述:%s"),GetSendDataTypeDesc(wSendDataType),strResponseResult);
			break;
		case -6://撤单失败
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据格式成功(撤单失败）,回馈业务管理线程代码为:0x06(内部错误)"));
			break;
		case -7://非法的请求回馈
			strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败（非法请求）,回馈业务管理线程代码为:0x07(内部错误)"));
			break;
		}
		wReturnCode = abs(iErrorCode);

		//回馈调用结果
		if(0x01 == byIsNeedResponseCaller)
		{
			if(0 != m_sTranstParam.CallerThreadID)
			{
				::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT,(WPARAM)0,MAKELONG(wSendDataType,wReturnCode));
			}
		}
		//记录日志
		if(strLog != m_strLastErrorDesc)
		{
			m_strLastErrorDesc = strLog;
			GetGlobalApp()->RecordLog(m_strLastErrorDesc);
		}
		strLog.Empty();
	}

	//清理资源
	if(NULL != pDVJsonReceive)
	{
		delete pDVJsonReceive;
		pDVJsonReceive = NULL;
	}

	if(NULL != pJsonIF)
	{
		delete pJsonIF;
		pJsonIF = NULL;
	}

	//关闭句柄
	::CloseHandle(hThreadHandle);

	//解锁
//	UnLock();

	//设置工作状态
	m_bWorking = FALSE;
}
*/

//新处理版本，增加版本判断，新版本使用首次扣费，多次查询处理
void CThreadHttp::OnMsgSendQueryToReflectWeb(WPARAM wParam, LPARAM lParam)
{
	// return ;//debug
	//设置工作状态
	m_bWorking = TRUE;
	CString strLog = _T("");

	//解析数据
	WORD wSendDataType				= LOWORD(lParam);
	BYTE byIsNeedResponseCaller		= LOBYTE(HIWORD(lParam));	//是否需要回馈调用者
	BYTE byIsNeedRetrievalLocalData	= HIBYTE(HIWORD(lParam));	//是否需要检索本地的撤单数据
	WORD	wReturnCode = 0x00;									//回馈结果代码
	DWORD	dwServerReturnResponseCode = 0x00;

	CJsonOperationIF* pJsonIF = NULL;	//数据操作类指针
	CString strSendData = _T("");		//转化成需要发送的数据格式载体
	CDVJsonBase* pDVJsonReceive = (CDVJsonBase*)wParam;	//获得需要传递的数据

	CString strURL = _T("");	//获取当前使用的URL地址
	INTERNET_PORT wPort = 80;	//获取当前使用的端口号
	int	iCallMode = 1;			//默认GET方式获取数据
	HWND hWnd = NULL;			//调用者的窗体句柄

	CString strResponseResult = _T("");	//服务器返回的数据
	DWORD dwResponseStateCode = 0x00;	//服务器返回的状态码
	DWORD dwConsumeTime = 0x00;			//服务器处理时间

	//构造参数
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;

	if ( 0 == theApp.m_iHTTPVersion )
	{
		try
		{
			if (NULL == pDVJsonReceive)
				throw - 1;
			int iPayChannelType = -1;
			//根据发送过来的数据类型，进行数据格式和转化，并获得服务端链接信息
			if (!DataConvertFromJson2String(wSendDataType, pDVJsonReceive, strURL, wPort, strSendData, hWnd, iCallMode, &pJsonIF,iPayChannelType))
				throw - 2;

			strLog.Format(_T("[支付通讯线程]:已接收到请求[类型:%s,%s]，准备发送数据"), GetSendDataTypeDesc(wSendDataType), pDVJsonReceive->PayIdentifier);
			GetGlobalApp()->RecordLog(strLog);
			strLog.Empty();

			//标记扣款流程接收到支付凭证(整合模式有效)
			if (GetGlobalApp()->m_bCurrentDebitMode)
			{
				if (JSON_DT_SEND_DIBIT == wSendDataType)
				{
					GetGlobalApp()->m_iDebitTaskNo = 0x03;
				}
			}

			DWORD	dwThreadID = 0;
			static tagHttpParam sParam = {0};

			//填充参数结构体
			sParam.URL					= _T("");
			sParam.ConsumeTime			= 0;
			sParam.SendData				= _T("");
			sParam.ExecuteCode			= 0xFF;
			sParam.ConsumeTime			= 0;
			sParam.ServerResponseData	= _T("");
			sParam.ServerResponseCode	= 0;
			sParam.NeedEndThread		= FALSE;

			//根据支付类型进行超时预设
			if(2 == iPayChannelType)
			{
				//银联支付，强制将时间调整为15秒
				sParam.SessionReceiveTimeout= 15*1000;
				sParam.SessionControlTimeout= 16*1000;
				sParam.SessionSendTimeout	= 30*1000;
			}
			else//非银联支付，保持配置的超时时间
			{
				sParam.SessionReceiveTimeout= m_dwWaitThreadWorkTimeout - 1500;
				sParam.SessionControlTimeout= m_dwWaitThreadWorkTimeout - 1000;
				sParam.SessionSendTimeout	= 2*1000;
			}


			sParam.CallMode	= iCallMode;
			sParam.Port		= wPort;
			sParam.URL		= strURL;
			//转化成UTF-8格式
			if (strSendData.IsEmpty())
			{
				strSendData.Format(_T("Illegal Data Format"));
			}
			sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

			//发送数据，获取回馈
			if ( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
				throw - 3;

			if(2 == iPayChannelType)
			{
				//如果是NFC支付,强制将超时时间设置为15秒
				if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,15*1000))
				{
					//终止线程
					sParam.NeedEndThread = TRUE;
				}
			}
			else
			{
				if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,m_dwWaitThreadWorkTimeout))
				{
					//终止线程
					sParam.NeedEndThread = TRUE;
				}
			}
			
			//获取结果
			dwConsumeTime		= sParam.ConsumeTime;
			dwResponseStateCode	= sParam.ServerResponseCode;
			strResponseResult	= sParam.ServerResponseData;

			//如果是主动终止发送子线程，或获取到的结果回馈不是成功，判定超时
			if ( ( TRUE == sParam.NeedEndThread ) || ( OP_SUCCESS != sParam.ExecuteCode ) )
			{
				if ( OP_SUCCESS != sParam.ExecuteCode )
				{
					//标记需要进行心跳
					PostThreadMessage(WM_THREAD_HTTP_SEND_HEART, (WPARAM)0x03, (LPARAM)0);
				}
				//如果是撤单，则需要判断是否需要保存撤单数据
				if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
				{
					//保存撤单信息
					CDVDebitCancel cData;
					cData.PayIdentifier = ((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier;
					cData.ProvinceID    = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
					cData.RoadID		= ::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.RoadID;
					cData.StationID		= _ttoi(::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
					cData.SubTime		= pDVJsonReceive->SubmitTime;
					cData.CreateTime	= CTime(time(NULL));
					m_cTableDebitCancel.Save(cData);
				}
				//抛出异常，描述通讯超时
				throw - 0xF0;
			}

			//解析获得的数据,得出格式化结果
			CDVJsonBase* pDVJsonBase = pJsonIF->FormatResponseResult2Class(strResponseResult, wSendDataType);
			if (NULL == pDVJsonBase)
				throw - 4;
			pDVJsonBase->ProvinceID      = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
			pDVJsonBase->m_dwConsumeTime = dwConsumeTime;

#ifdef _DEBUG
			//记录日志
			strSendData.TrimRight();
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:发送数据到服务器(请求[类型:%s])，详细数据:%s;获取回馈数据:%s"), GetSendDataTypeDesc(wSendDataType), strSendData, strResponseResult));
#endif
			//判断服务器回馈结果
			dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;
		

			//重置服务器检测状态为正常
			m_dwHeratFailForCenterReflectWeb	= 0;
			m_dwHeratFailForOperationServer		= 0;

			//如果是扣费或扣费查询
			if (   (JSON_DT_SEND_DIBIT	    == wSendDataType)
			        || (JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
			   )
			{
				//返回代码不在以下列表（成功/输入密码/二维码超时/二维码非法/余额不足/密码错误），则认为是内部参数异常
				if (   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_SRC_ILLGAL				!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_INPUT_PASSWORD_ERROR			!= pDVJsonBase->ServerResponseCode)
				        && (HTTP_RC_NFC_NO_SUPPORT				!= pDVJsonBase->ServerResponseCode) //NFC,卡片不支持闪付
				        && (HTTP_RC_NFC_NO_INIT					!= pDVJsonBase->ServerResponseCode) //NFC,卡片未初始化
				        && (HTTP_RC_NFC_NO_SERVICES				!= pDVJsonBase->ServerResponseCode) //NFC,银联闪付暂停服务
				   )
				{
					//抛出异常
					throw - 5;
				}
			}
			else if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
			{
				//检索是否需要处理本地的撤单数据
				if (byIsNeedRetrievalLocalData)
				{
					m_cTableDebitCancel.UpdateProcessMark(((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier);
					m_cTableDebitCancel.Delete();
				}
			}
			else
			{
				throw - 7;
			}

			//通知调用者结果
			CString strNeedNotifyCaller(_T(",不需要通知调用者"));
			if (0x01 == byIsNeedResponseCaller)
			{
				if (0 != m_sTranstParam.CallerThreadID)
				{
					strNeedNotifyCaller.Format(_T(",已通知调用者"));
					::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)pDVJsonBase, MAKELONG(wSendDataType, HTTP_RC_DATA_LEGAL ));
				}
			}

		

			//记录日志
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:已接收到服务器回馈[请求类型:%s,%s]，状态:处理成功%s"), GetSendDataTypeDesc(wSendDataType), pDVJsonBase->PayIdentifier, strNeedNotifyCaller));
			//如果通讯状态异常，则在此进行纠正
			if (   (FALSE != GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
			        || (FALSE != GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist)
			   )
			{
				//重置服务器检测状态为正常
				m_dwHeratFailForCenterReflectWeb = 0;
				m_dwHeratFailForOperationServer = 0;
				//设置服务器状态 -- 收费专网反射服务器异常
				GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
				//设置服务器状态 -- 运营平台服务器异常
				GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
			}
		}
		catch (int& iErrorCode)
		{
			switch (iErrorCode)
			{
			case -1://传入参数指针为空
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,传入参数指针为空,回馈业务管理线程代码为:0x01(内部错误)"));
				break;
			case -2://数据格式转换出现异常
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,数据格式转换出现异常,回馈业务管理线程代码为:0x02(内部错误)"));
				break;
			case -3://无法启动数据发送线程
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,无法启动数据发送线程,回馈业务管理线程代码为:0x03(内部错误)"));
				break;
			case -4://回馈数据显示失败
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,回馈业务管理线程代码为:0x04(内部错误)，回馈数据异常，无法解析"));
				break;
			case -5://扣费或查询，回馈结果非成功或正在输入密码，判定内部错误
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据格式成功[请求类型:%s],回馈业务管理线程代码为:0x05(内部错误)，详细描述:%s"), GetSendDataTypeDesc(wSendDataType), strResponseResult);
				break;
			case -6://撤单失败
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据格式成功(撤单失败）,回馈业务管理线程代码为:0x06(内部错误)"));
				break;
			case -7://非法的请求回馈
				strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败（非法请求）,回馈业务管理线程代码为:0x07(内部错误)"));
				break;
			}
			wReturnCode = abs(iErrorCode);

			//回馈调用结果
			if (0x01 == byIsNeedResponseCaller)
			{
				if (0 != m_sTranstParam.CallerThreadID)
				{
					::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)0, MAKELONG(wSendDataType, wReturnCode));
				}
			}
			//记录日志
			if (strLog != m_strLastErrorDesc)
			{
				m_strLastErrorDesc = strLog;
				GetGlobalApp()->RecordLog(m_strLastErrorDesc);
			}
			strLog.Empty();
		}

		//清理资源
		if (NULL != pDVJsonReceive)
		{
			delete pDVJsonReceive;
			pDVJsonReceive = NULL;
		}

		if (NULL != pJsonIF)
		{
			delete pJsonIF;
			pJsonIF = NULL;
		}

		//关闭句柄
		::CloseHandle(hThreadHandle);
	}
	else//版本非0，扣费处理为首次扣费，后续循环查询
	{
		int	iDealSN = 0;	//处理序列号，标记首次扣费还是后续查询
		int	iLoop = 1;		//循环处理的总次数
		int		iPayChannelType = -1;//如果是扣费请求，该项返回指明是那种扣费类型（-1/0，未知；1，第三方支付平台；2，银联支付）
		bool	bWaitSleep = false;
		m_bLoopBreak = FALSE;

		if ( (JSON_DT_SEND_DIBIT == wSendDataType) || (JSON_DT_SEND_DIBIT_CHECK == wSendDataType) )
		{
			if (JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
			{
				iDealSN = 1;
			}
			iLoop = 99;	//对于扣费（查询），可以多次循环，撤单只执行一次（有本地缓存）
		}

		for (int i = 0; i < iLoop; ++i)
		{
			if ( m_bLoopBreak )
			{
				break;
			}

			try
			{
				if (NULL == pDVJsonReceive)
				{
					m_bLoopBreak = TRUE;
					throw - 1;
				}

				//根据发送过来的数据类型，进行数据格式和转化，并获得服务端链接信息
				if (!DataConvertFromJson2String(wSendDataType, pDVJsonReceive, strURL, wPort, strSendData, hWnd, iCallMode, &pJsonIF,iPayChannelType, iDealSN))
				{
					m_bLoopBreak = TRUE;
					throw - 2;
				}
				iDealSN += 1;

				strLog.Format(_T("[支付通讯线程]:已接收到请求[类型:%s,%s]，准备发送数据"), GetSendDataTypeDesc(wSendDataType), pDVJsonReceive->PayIdentifier);
				GetGlobalApp()->RecordLog(strLog);
				strLog.Empty();

				//标记扣款流程接收到支付凭证(整合模式有效)
				if (GetGlobalApp()->m_bCurrentDebitMode)
				{
					if (JSON_DT_SEND_DIBIT == wSendDataType)
					{
						GetGlobalApp()->m_iDebitTaskNo = 0x03;
					}
				}

				DWORD	dwThreadID = 0;
				static tagHttpParam sParam = {0};

				//填充参数结构体
				sParam.ConsumeTime			= 0;
				sParam.SendData				= _T("");
				sParam.ExecuteCode			= 0xFF;
				sParam.ConsumeTime			= 0;
				sParam.ServerResponseData	= _T("");
				sParam.ServerResponseCode	= 0;
				sParam.NeedEndThread		= FALSE;

				//根据支付类型进行超时预设
				if(2 == iPayChannelType)
				{
					//NFC支付，强制将时间调整为15秒
					sParam.SessionReceiveTimeout= 15*1000;
					sParam.SessionControlTimeout= 16*1000;
					sParam.SessionSendTimeout	= 30*1000;
				}
				else
				{
					sParam.SessionReceiveTimeout= m_dwWaitThreadWorkTimeout - 1500;
					sParam.SessionControlTimeout= m_dwWaitThreadWorkTimeout - 1000;
					sParam.SessionSendTimeout	= 2*1000;
				}
				
				sParam.CallMode	= iCallMode;
				sParam.Port		= wPort;
				sParam.URL		= strURL;
				//转化成UTF-8格式
				if (strSendData.IsEmpty())
				{
					strSendData.Format(_T("Illegal Data Format"));
				}
				sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

				if ( m_bLoopBreak )	{	break;	}

				//发送数据，获取回馈
				if ( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
				{
					m_bLoopBreak = TRUE;
					throw - 3;
				}
				//等待超时
				if(2 == iPayChannelType)
				{
					//如果是NFC支付,强制将超时时间设置为15秒
					if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,15*1000))
					{
						//终止线程
						sParam.NeedEndThread = TRUE;
					}
				}
				else
				{
					if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,m_dwWaitThreadWorkTimeout))
					{
						//终止线程
						sParam.NeedEndThread = TRUE;
					}
				}
				//获取结果
				dwConsumeTime		= sParam.ConsumeTime;
				dwResponseStateCode	= sParam.ServerResponseCode;
				strResponseResult	= sParam.ServerResponseData;

				//如果是主动终止发送子线程，或获取到的结果回馈不是成功，判定超时
				if ( ( TRUE == sParam.NeedEndThread ) || ( OP_SUCCESS != sParam.ExecuteCode ) )
				{
					if ( OP_SUCCESS != sParam.ExecuteCode )
					{
						//标记需要进行心跳
						PostThreadMessage(WM_THREAD_HTTP_SEND_HEART, (WPARAM)0x03, (LPARAM)0);
					}
					//如果是撤单，则需要判断是否需要保存撤单数据
					if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
					{
						//保存撤单信息
						CDVDebitCancel cData;
						cData.PayIdentifier = ((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier;
						cData.ProvinceID    = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
						cData.RoadID		= ::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.RoadID;
						cData.StationID		= _ttoi(::GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
						cData.SubTime		= pDVJsonReceive->SubmitTime;
						cData.CreateTime	= CTime(time(NULL));
						m_cTableDebitCancel.Save(cData);
					}
					//延时3秒
					if( iDealSN >= 3)
					{
						for(int m=0; m < 60; m++)
						{
							if( m_bLoopBreak )
							{
								break;
							}
							::Sleep(50);
						}
					}
					else
					{
						::Sleep(50);
					}
					//抛出异常，描述通讯超时
					throw - 0xF0;
				}

				//解析获得的数据,得出格式化结果
				CDVJsonBase* pDVJsonBase = pJsonIF->FormatResponseResult2Class(strResponseResult, wSendDataType);
				if (NULL == pDVJsonBase)
				{
					m_bLoopBreak = TRUE;
					throw - 4;
				}
				pDVJsonBase->ProvinceID      = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
				pDVJsonBase->m_dwConsumeTime = dwConsumeTime;

#ifdef _DEBUG
				//记录日志
				strSendData.TrimRight();
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:发送数据到服务器(请求[类型:%s])，详细数据:%s;获取回馈数据:%s"), GetSendDataTypeDesc(wSendDataType), strSendData, strResponseResult));
#endif
				//判断服务器回馈结果
				dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;

				//重置服务器检测状态为正常
				m_dwHeratFailForCenterReflectWeb	= 0;
				m_dwHeratFailForOperationServer		= 0;

				//如果是扣费或扣费查询
				if (   (JSON_DT_SEND_DIBIT	    == wSendDataType)
				        || (JSON_DT_SEND_DIBIT_CHECK == wSendDataType)
				   )
				{
					//返回代码不在以下列表（成功/输入密码/二维码超时/二维码非法/余额不足/密码错误/查询失败），则认为是内部参数异常
					if (   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_SRC_ILLGAL				!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_INPUT_PASSWORD_ERROR			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_QUERY_ERROR			!= pDVJsonBase->ServerResponseCode)
					        && (HTTP_RC_DEBIT_FAIL					!= pDVJsonBase->ServerResponseCode) //调试过程中出现该返回
					        && (HTTP_RC_NFC_NO_SUPPORT				!= pDVJsonBase->ServerResponseCode) //NFC,卡片不支持闪付
					        && (HTTP_RC_NFC_NO_INIT					!= pDVJsonBase->ServerResponseCode) //NFC,卡片未初始化
					        && (HTTP_RC_NFC_NO_SERVICES				!= pDVJsonBase->ServerResponseCode) //NFC,银联闪付暂停服务
					   )
					{
						//抛出异常
						m_bLoopBreak = TRUE;
						throw - 5;
					}

					//对于明确结果返回的，不再继续循环（成功/二维码超时/二维码非法/余额不足/密码错误）
					//只有返回输入密码及查询失败（扣费进行中。。），才继续循环查询
					if (		(HTTP_RC_DEBIT_SUCCESS				== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_CODE_TIMEOUT			== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_SRC_ILLGAL			== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_INPUT_PASSWORD_ERROR		== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_DEBIT_FAIL					== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_NFC_NO_SUPPORT				== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_NFC_NO_INIT				== pDVJsonBase->ServerResponseCode)
					            ||	(HTTP_RC_NFC_NO_SERVICES			== pDVJsonBase->ServerResponseCode)
					   )
					{
						bWaitSleep = false;
						m_bLoopBreak = TRUE;
					}
					else if ((HTTP_RC_DEBIT_NEED_PASSWORD	== pDVJsonBase->ServerResponseCode) //输入密码状态继续循环
					         ||	(HTTP_RC_DEBIT_QUERY_ERROR		== pDVJsonBase->ServerResponseCode)//查询失败继续循环
					        )
					{
						if ((HTTP_RC_DEBIT_NEED_PASSWORD	== pDVJsonBase->ServerResponseCode))
						{
							//要求设备进行通知(至少间隔10秒)
							static time_t tLastSendInputPSWInfo = 0;
							if ( (time(NULL) - tLastSendInputPSWInfo) > 10)
							{
								::GetGlobalApp()->m_pThreadManage->m_pThreadDevice->PostThreadMessage(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI, (WPARAM)SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT, (LPARAM)0x00);
								tLastSendInputPSWInfo = time(NULL);
							}
						}
						//设置等待查询标记
						bWaitSleep = true;
					}
				}
				else if (JSON_DT_SEND_DIBIT_CANCEL == wSendDataType)
				{
					//检索是否需要处理本地的撤单数据
					if (byIsNeedRetrievalLocalData)
					{
						m_cTableDebitCancel.UpdateProcessMark(((CDVJsonDebitCancel*)pDVJsonReceive)->PayIdentifier);
						m_cTableDebitCancel.Delete();
					}
				}
				else
				{
					m_bLoopBreak = TRUE;
					throw - 7;
				}

				//通知调用者结果
				CString strNeedNotifyCaller(_T(",不需要通知调用者"));
				if (0x01 == byIsNeedResponseCaller)
				{
					if (0 != m_sTranstParam.CallerThreadID)
					{
						strNeedNotifyCaller.Format(_T(",已通知调用者"));


						::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)pDVJsonBase, MAKELONG(wSendDataType, HTTP_RC_DATA_LEGAL ));
					}
				}

				
				//记录日志
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:已接收到服务器回馈[请求类型:%s,%s]，状态:处理成功%s"), GetSendDataTypeDesc(wSendDataType), pDVJsonBase->PayIdentifier, strNeedNotifyCaller));
				//如果通讯状态异常，则在此进行纠正
				if (   (FALSE != GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist)
				        || (FALSE != GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist)
				   )
				{
					//重置服务器检测状态为正常
					m_dwHeratFailForCenterReflectWeb = 0;
					m_dwHeratFailForOperationServer = 0;
					//设置服务器状态 -- 收费专网反射服务器异常
					GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= FALSE;
					//设置服务器状态 -- 运营平台服务器异常
					GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= FALSE;
				}

				//循环等待处理，目前等待间隔 3000 毫秒
				if ( bWaitSleep )
				{
					for (int m = 0; m < 60; m++)
					{
						if ( m_bLoopBreak )
						{
							break;
						}
						::Sleep(50);
					}
				}
			}
			catch (int& iErrorCode)
			{
				switch (iErrorCode)
				{
				case -1://传入参数指针为空
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,传入参数指针为空,回馈业务管理线程代码为:0x01(内部错误)"));
					break;
				case -2://数据格式转换出现异常
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,数据格式转换出现异常,回馈业务管理线程代码为:0x02(内部错误)"));
					break;
				case -3://无法启动数据发送线程
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,无法启动数据发送线程,回馈业务管理线程代码为:0x03(内部错误)"));
					break;
				case -4://回馈数据显示失败
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败,回馈业务管理线程代码为:0x04(内部错误)，回馈数据异常，无法解析"));
					break;
				case -5://扣费或查询，回馈结果非成功或正在输入密码，判定内部错误
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据格式成功[请求类型:%s],回馈业务管理线程代码为:0x05(内部错误)，详细描述:%s"), GetSendDataTypeDesc(wSendDataType), strResponseResult);
					break;
				case -6://撤单失败
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据格式成功(撤单失败）,回馈业务管理线程代码为:0x06(内部错误)"));
					break;
				case -7://非法的请求回馈
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据失败（非法请求）,回馈业务管理线程代码为:0x07(内部错误)"));
					break;

				case -0xF0:
					strLog.Format(_T("[支付通讯线程]:服务器回馈数据超时, 回馈业务管理线程代码为:0xF0"));
					break;

				default:
					break;
				}
				wReturnCode = abs(iErrorCode);

				if ( -0xF0 != iErrorCode )
				{
					//回馈调用结果
					if (0x01 == byIsNeedResponseCaller)
					{
						if (0 != m_sTranstParam.CallerThreadID)
						{
							::PostThreadMessage(m_sTranstParam.CallerThreadID, WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT, (WPARAM)0, MAKELONG(wSendDataType, wReturnCode));
						}
					}
				}

				//记录日志
				if (strLog != m_strLastErrorDesc)
				{
					m_strLastErrorDesc = strLog;
					GetGlobalApp()->RecordLog(m_strLastErrorDesc);
				}
				strLog.Empty();
			}

			//由于pJsonIF，hThreadHandle均是在循环内部生成使用，故每次循环结束必须进行释放
			if (NULL != pJsonIF)
			{
				delete pJsonIF;
				pJsonIF = NULL;
			}

			//关闭句柄
			::CloseHandle(hThreadHandle);
			hThreadHandle = INVALID_HANDLE_VALUE;
		}

		//清理资源
		if (NULL != pDVJsonReceive)
		{
			delete pDVJsonReceive;
			pDVJsonReceive = NULL;
		}
	}

	//设置工作状态
	m_bWorking = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::OnMsgSendHeart
//
// 功能描述：需要发送一次心跳到检测状态
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年9月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::OnMsgSendHeart(WPARAM wParam, LPARAM lParam)
{

	int iCount = (int)wParam;
	int iSleep = (int)lParam;
	static time_t tLastCheckHeartTime = 0x00;
	//判断上一次检测心跳时间,少于10秒则不进行该次检测
	if ( (time(NULL) - tLastCheckHeartTime) < 10)
		return;
	//进行心跳检测
	for (int i = 0; i < iCount; ++i)
	{
		//更新日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:支付平台心跳验证,第 %d 次"), i + 1), LOG_LEVEL_ERROR);
		CheckHeartState();
		Sleep(iSleep);
	}
	tLastCheckHeartTime = time(NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::DataConvertFromJson2String
//
// 功能描述：数据格式转换,从调用者传过来的DVJson格式转换成字符串形式
//
// 输入参数：IN const WORD& wSendDataType			--	传递的数据类型
//         ：IN OUT CDVJsonBase* pDVJsonReceive		--	传递的参数指针
//         ：IN OUT CString& strUseURL				--	返回来的URL
//         ：IN OUT INTERNET_PORT& wUsePort			--	返回来的端口
//         ：IN OUT CString& strSendData			--	返回来需要传输的字符串
//         ：IN OUT HWND& hWnd						--	返回来调用者窗体句柄
//         ：IN OUT int& iCallMode					--	返回来的调用模式(1,Get;2,Post)
//         ：IN OUT CJsonOperationIF* pJsonIF		--	返回来的JSON操作类指针
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2015年12月14日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CThreadHttp::DataConvertFromJson2String(IN const WORD& wSendDataType, IN OUT CDVJsonBase* pDVJsonReceive, IN OUT CString& strURL, IN OUT INTERNET_PORT& wPort, IN OUT CString& strSendData, IN OUT HWND& hWnd, IN OUT int& iCallMode, IN OUT CJsonOperationIF** pJsonIF,int& iPayChannelType, int nSN )
{
	BOOL bReturn = TRUE;

	try
	{
		switch (wSendDataType)
		{
		case JSON_DT_SEND_DIBIT:		//支付请求(扣费)
		{
			//获得相应的数据类指针
			CDVJsonDebit* pDVJsonDebit = (CDVJsonDebit* )pDVJsonReceive;
			if (NULL == pDVJsonDebit)
			{
				throw - 1;
			}
			//获得调用者窗体句柄
			hWnd = pDVJsonDebit->m_hCallerWnd;

			//判断是那种支付类型(1.0.8.4增添，判断支付渠道是否银联）
			if(   (TRUE != pDVJsonDebit->NFCPayData.IsEmpty())																			//银联NFC支付
				||((0 == pDVJsonDebit->QRCodeData.Left(2).CompareNoCase(_T("62"))) && (19 == pDVJsonDebit->QRCodeData.GetLength()))		//银联二维码
				)
			{
				iPayChannelType = 2;
			}

			//申请数据操作类
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//转化成JSON数据格式
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebit, nSN);
			//获得服务端链接信息
			strURL = m_strDebitURL;
			wPort  = m_wDebitPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_DIBIT_CHECK:	//支付请求(查询)
		{
			//获得相应的数据类指针
			CDVJsonDebitCheck* pDVJsonDebitCheck = (CDVJsonDebitCheck* )pDVJsonReceive;
			if (NULL == pDVJsonDebitCheck)
			{
				throw - 1;
			}
			//获得调用者窗体句柄
			hWnd = pDVJsonDebitCheck->m_hCallerWnd;

			//申请数据操作类
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//转化成JSON数据格式
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitCheck, nSN);
			//获得服务端链接信息
			strURL = m_strDebitCheckURL;
			wPort  = m_wDebitCheckPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_DIBIT_CANCEL:	//支付请求(撤单)
		{
			//获得相应的数据类指针
			CDVJsonDebitCancel* pDVJsonDebitCancel = (CDVJsonDebitCancel* )pDVJsonReceive;
			if (NULL == pDVJsonDebitCancel)
			{
				throw - 1;
			}
			//获得调用者窗体句柄
			hWnd = pDVJsonDebitCancel->m_hCallerWnd;

			//申请数据操作类
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//转化成JSON数据格式
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitCancel);
			//获得服务端链接信息
			strURL = m_strDebitCancelURL;
			wPort  = m_wDebitCancelPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_DIBIT_SECOND:	//支付请求(二次处理)
		{
			//获得相应的数据类指针
			CDVJsonDebitSecond* pDVJsonDebitSecond = (CDVJsonDebitSecond* )pDVJsonReceive;
			if (NULL == pDVJsonDebitSecond)
			{
				throw - 1;
			}
			//获得调用者窗体句柄
			hWnd = pDVJsonDebitSecond->m_hCallerWnd;

			//申请数据操作类
			*pJsonIF = new CJsonDebitOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//转化成JSON数据格式
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitSecond);
			//获得服务端链接信息
			strURL = m_strDebitSecondURL;
			wPort  = m_wDebitSecondPort;
			iCallMode = 2;
		}
		break;
		case JSON_DT_SEND_HEART:	//心跳检测
		{
			//获得相应的数据类指针
			CDVJsonHeart* pDVJsonHeart = (CDVJsonHeart* )pDVJsonReceive;
			if (NULL == pDVJsonHeart)
			{
				throw - 1;
			}
			//获得调用者窗体句柄
			hWnd = pDVJsonHeart->m_hCallerWnd;

			//申请数据操作类
			*pJsonIF = new CJsonHeartOperate();
			if (NULL == *pJsonIF)
			{
				throw - 2;
			}
			//转化成JSON数据格式
			strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonHeart);
			//获得服务端链接信息
			strURL = m_strHeartURL;
			wPort  = m_wHeartPort;
			iCallMode = 2;
		}
		break;
		default:
			throw - 3;
		}
	}
	catch (int& iErrorCode)
	{
		bReturn = FALSE;

		switch (iErrorCode)
		{
		case -1://参数指针为空
			break;
		case -2://资源无法申请
			break;
		case -3://传输数据格式非法
			break;
		}
	}

	return bReturn;
}
UINT  WINAPI CThreadHttp::SendData_ByWinNet(LPVOID lpParam)
{return 0;}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::SendData
//
// 功能描述：数据递交 -- 线程工作函数
//
// 输入参数：LPVOID lpParam	--	参数结构体指针
// 输出参数：UINT
// 编写人员：ROCY
// 编写时间：2015年12月14日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
UINT WINAPI CThreadHttp::SendData(LPVOID lpParam)
{
	//return SendData_ByWinNet(lpParam);

	UINT dwReturn = 0x01;
	BOOL bIsHeart = FALSE;
	CString strErrorDesc;
	tagHttpParam* pParam = (tagHttpParam*)lpParam;

	//计时开始
	DWORD dwStart = ::GetTickCount();

	//创建网络连接对象
	CInternetSession session(_T("HttpClient"));


	CHttpConnection *pHtCon = NULL;
	CHttpFile *pHtFile		= NULL;

	int iTaskNo = 0x00;
	try
	{
		if (NULL == lpParam)
			throw 1;

		//接收超时
		session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, pParam->SessionReceiveTimeout);
		//响应超时
		session.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, pParam->SessionControlTimeout);

		/*

				session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 1000 * 6); //发送请求的超时时间
		        session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 2); //发送请求的超时时间
		        session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1); //发送请求的超时时间
		        session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 1000*2); //发送请求的超时时间
		        session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 1000*2); //发送请求的超时时间
		*/
		session.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, 2*1000);


		//解析URL的正确性
		INTERNET_PORT wGetPort;	//用于保存目标HTTP服务端口
		CString strServer;		//用于保存服务器地址
		CString	strObject;		//用于保存文件对象名称
		DWORD	dwServiceType;	//用于保存服务类型

		//解析URL，获取信息
		if (!AfxParseURL(pParam->URL, dwServiceType, strServer, strObject, wGetPort))
		{
			throw 2;
		}
		//判定是否心跳
		if ( -1 == strObject.Find(_T("heart")))
		{
			bIsHeart = FALSE;
		}
		else
		{
			bIsHeart = TRUE;
		}

		iTaskNo = 0x01;

		//如果需要强制结束，则结束线程
		if (pParam->NeedEndThread)
			goto END;

		//测试网络
		//BOOL bBreakPing = FALSE;
		//int iPingCount = 0x00;
		//BOOL bPing = FALSE;
		//do
		//{
		//	bPing = Ping(strServer,32);
		//	if(bPing || (++iPingCount > 4))
		//	{
		//		bBreakPing = TRUE;
		//	}
		//}while(!bBreakPing);
		//
		//if(!bPing)
		//{
		//	throw 3;
		//}


		iTaskNo = 0x02;

		//如果需要强制结束，则结束线程
		if (pParam->NeedEndThread)
			goto END;

		//建立网络连接

		pHtCon = session.GetHttpConnection(strServer, wGetPort);
		if ( NULL == pHtCon )
		{
			//建立网络连接失败
			session.Close();
			//抛出异常
			throw 4;
		}

		iTaskNo = 0x03;


		//如果需要强制结束，则结束线程
		if (pParam->NeedEndThread)
			goto END;


		//发起GET/POST请求
		pHtFile = pHtCon->OpenRequest((1 == pParam->CallMode) ? CHttpConnection::HTTP_VERB_GET : CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_EXISTING_CONNECT);


		if ( NULL == pHtFile )
		{
			//发起请求失败
			session.Close();
			delete pHtCon;
			pHtCon = NULL;
			//抛出异常
			throw 5;
		}


		iTaskNo = 0x04;

		//如果需要强制结束，则结束线程
		if (pParam->NeedEndThread)
			goto END;

		//MessageBox(0,pParam->SendData,"debug",0);
		//modify

		pHtFile->SendRequestEx(pParam->SendData.GetLength());

		//MessageBox(0, "end SendRequestEx", "", 0);

		iTaskNo = 0x05;

		pHtFile->WriteString(pParam->SendData);

		iTaskNo = 0x06;
	
		pHtFile->EndRequest();
		


		


		//获取服务器状态回馈
		if (pParam->NeedEndThread)
			goto END;

		iTaskNo = 0x07;
		pParam->ServerResponseCode = 0x00;
		pHtFile->QueryInfoStatusCode(pParam->ServerResponseCode);

		//如果是心跳请求回馈，不接收任何数据
		if (!bIsHeart)
		{
			//获取返回数据(统一当作字符串处理)
			if (pParam->NeedEndThread)
				goto END;
			pParam->ServerResponseData.Empty();

			if (pParam->NeedEndThread)
				goto END;

			//接收数据最长不允许超过2K
			DWORD dwReceiveSize = 2 * 1024;
			CString strSingleData;
			while (pHtFile->ReadString(strSingleData))
			{
				//如果需要强制结束，则结束线程
				if (pParam->NeedEndThread)
					goto END;

				pParam->ServerResponseData += strSingleData;
				::Sleep(5);

				//接收数据不允许超过指定值
				if (pParam->ServerResponseData.GetLength() > dwReceiveSize)
				{
					break;
				}
			}
			pParam->ServerResponseData.TrimRight();

			CString strTempData = pParam->ServerResponseData;

			pParam->ServerResponseData = StringConvertByCodePage(strTempData, CP_UTF8, CP_ACP);

#ifdef _DEBUG
			::OutputDebugString(pParam->ServerResponseData);
#endif
		}

		iTaskNo = 0x08;

#ifdef _DEBUG
		if (	  (HTTP_STATUS_OK != pParam->ServerResponseCode)
		          && (HTTP_STATUS_BAD_METHOD != pParam->ServerResponseCode)
		   )
		{
			//服务器不接受请求
			session.Close();
			delete pHtCon;
			pHtCon = NULL;

			delete pHtFile;
			pHtFile = NULL;
			//抛出异常
			throw 7;
		}
#else
		if (HTTP_STATUS_OK != pParam->ServerResponseCode)
		{
			//服务器不接受请求
			session.Close();
			delete pHtCon;
			pHtCon = NULL;

			delete pHtFile;
			pHtFile = NULL;
			//抛出异常
			throw 7;
		}
#endif

		iTaskNo = 0x09;

		//标记返回成功
		dwReturn = OP_SUCCESS;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case 1:
			strErrorDesc.Format(_T("%s,参数结构体指针为空"), bIsHeart ? _T("心跳检测") : _T("业务请求"));
			break;
		case 2:
			strErrorDesc.Format(_T("%s,解析URL[%s]发现异常,%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), pParam->URL, GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 3:
			strErrorDesc.Format(_T("%s,网络异常，无法检索到内网反射服务器"), bIsHeart ? _T("心跳检测") : _T("业务请求"));
			break;
		case 4:
			strErrorDesc.Format(_T("%s,建立网络连接出现异常,%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 5:
			strErrorDesc.Format(_T("%s,发起请求出现异常,%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 6:
			strErrorDesc.Format(_T("%s,检测服务端网络状态异常,%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 7:
			strErrorDesc.Format(_T("%s,获取服务器状态回馈出现异常,%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		case 8:
			strErrorDesc.Format(_T("%s,获取服务器状态回馈出现异常,%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), GetGlobalApp()->DisplayWinError(::GetLastError()));
			break;
		}
		dwReturn = iErrorCode;
	}
	catch (CInternetException* e)
	{
		DWORD dwErorCode = ::GetLastError();
		TCHAR info[1024] = {0};
		e->GetErrorMessage(info, 1024);
		strErrorDesc.Format(_T("服务器异常,详细描述:(%s,任务号 %d,错误代码:%d)%s"), bIsHeart ? _T("心跳检测") : _T("业务请求"), iTaskNo, dwErorCode, info);
		e->Delete();
		dwReturn = 9;

	}
	catch (...)
	{
		DWORD dwErrorCode = ::GetLastError();
	}
END:
	if (NULL != pParam)
	{
		pParam->ExecuteCode = dwReturn;
	}
	//关闭文件
	if (NULL != pHtFile)
	{
		pHtFile->Close();
		//释放资源
		delete pHtFile;
		pHtFile = NULL;
	}

	//关闭链接
	if (NULL != pHtCon)
	{
		pHtCon->Close();
		//释放资源
		delete pHtCon;
		pHtCon = NULL;
	}
	//关闭会话
	session.Close();

	strErrorDesc.TrimLeft();
	strErrorDesc.TrimRight();
	if (!strErrorDesc.IsEmpty())
	{
		static CString strLastError = _T("");
		if (strLastError != strErrorDesc)
		{
			strLastError = strErrorDesc;
			//更新日志
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[支付通讯线程]:数据交互过程产生错误,%s"), strErrorDesc), LOG_LEVEL_ERROR);
		}
	}
	//统计用时
	pParam->ConsumeTime = ::GetTickCount() - dwStart;
	//返回
	return dwReturn;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::CheckHeartState
//
// 功能描述：进行心跳检测
//
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年1月13日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadHttp::CheckHeartState()
{
	//解析数据
	WORD wSendDataType = JSON_DT_SEND_HEART;
	//回馈结果代码
	WORD wReturnCode = 0x00;
	//转化成需要发送的数据格式载体
	CString strSendData = _T("HeartCheck");

	CString strLog;
	//获取当前使用的URL地址
	CString strURL;
	//获取当前使用的端口号
	INTERNET_PORT wPort = 80;
	//当前调用方式
	int	iCallMode = 1;//默认GET方式获取数据
	//调用者的窗体句柄
	HWND hWnd = NULL;
	//子线程等待时间,单位:毫秒
	DWORD dwWaitThreadWorkTimeout = 5000;

	//服务器返回的数据
	CString strResponseResult;
	//服务器返回的状态码
	DWORD dwResponseStateCode = 0x00;
	//服务器处理时间
	DWORD dwConsumeTime = 0x00;
	//开始处理流程
	try
	{
		//构造参数
		HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
		DWORD	dwThreadID   = 0;
		tagHttpParam sParam;

		//填充参数结构体
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout = 4000;
		sParam.SessionControlTimeout = 5000;

		sParam.CallMode	= iCallMode;
		sParam.Port		= m_wHeartPort;
		sParam.URL		= m_strHeartURL;
		//转化成UTF-8格式
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		//发送数据，获取回馈
		// if( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread(&sParam)) )
		// 	throw -1;
		// //等待超时
		
		// if(WAIT_TIMEOUT == ::WaitForSingleObject(hThreadHandle,dwWaitThreadWorkTimeout))
		// {
		// 	//终止线程
		// 	sParam.NeedEndThread = TRUE;
		// 	//沉默300毫秒
		// 	Sleep(300);
		// 	dwResponseStateCode	= sParam.ServerResponseCode;
		// }
		// else
		// {
		// 	//获取结果
		// 	dwConsumeTime		= sParam.ConsumeTime;
		// 	dwResponseStateCode	= sParam.ServerResponseCode;
		// 	strResponseResult	= sParam.ServerResponseData;
		// }
		// //MessageBox(0,"wait over","",0);
		// GetGlobalApp()->RecordLog("wait over");
		// //关闭句柄
		// ::CloseHandle(hThreadHandle);
		
		

		DWORD dwTime1 = ::GetTickCount();
		SendData(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

		//判断服务器回馈结果
		if (OP_SUCCESS != sParam.ExecuteCode)
			throw - 1;

		//重置服务器检测状态为正常
		m_dwHeratFailForCenterReflectWeb = 0;
		m_dwHeratFailForOperationServer = 0;

	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1://无法启动发送信息的线程
			++m_dwHeratFailForCenterReflectWeb;
			break;
		case -2://HTTP服务器回馈失败
			++m_dwHeratFailForOperationServer;
			break;
		}
		wReturnCode = abs(iErrorCode);
	}
	//设置服务器状态 -- 收费专网反射服务器异常
	GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist		= (m_dwHeratFailForCenterReflectWeb >= m_dwHeratFailWarningTimes) ? TRUE : FALSE;
	//设置服务器状态 -- 运营平台服务器异常
	GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist	= (m_dwHeratFailForOperationServer >= m_dwHeratFailWarningTimes) ? TRUE : FALSE;
	//记录日志
	if (GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist || GetGlobalApp()->m_bNetErrorHttpServerTrawePlatformNotExist )
	{
		static time_t tLastCheckHeartLogTime = 0x00;
		if ( (time(NULL) - tLastCheckHeartLogTime) > 30)
		{
			strLog.Format(_T("[支付通讯线程]:心跳检测,服务器产生异常"));
			if (strLog != m_strLastErrorDesc)
			{
				m_strLastErrorDesc = strLog;
				GetGlobalApp()->RecordLog(m_strLastErrorDesc);
			}

			tLastCheckHeartLogTime = time(NULL);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::StartProcessThread
//
// 功能描述：启动Http处理线程
//
// 输入参数：IN const LPVOID lpParam	--	线程工作函数的参数结构指针
// 输出参数：HANDLE	--	线程句柄
// 编写人员：ROCY
// 编写时间：2015年12月14日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE CThreadHttp::StartProcessThread(IN const LPVOID lpParam)
{
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
	tagHttpParam* pParam = (tagHttpParam*)lpParam;
	if (NULL != pParam)
	{
		hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &SendData, lpParam, 0, NULL);
	}
	return hThreadHandle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::StringConvertByCodePage
//
// 功能描述：对于给定的字符串数据，通过指定的编码格式进行互相转换
//
// 输入参数：IN const CString& strConvertData	--	需要转换的字符串
//         ：IN const int& iSourceCodepage		--	需要转换的字符串的编码格式
//         ：IN const int& iTargetCodepage		--	转换出来的字符串的编码格式
// 输出参数：CString -- 返回转换的字符串数据(生成格式:iTargetCodepage)
// 编写人员：ROCY
// 编写时间：2015年12月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
CString CThreadHttp::StringConvertByCodePage(IN const CString& strConvertData, IN const int& iSourceCodepage, IN const int& iTargetCodepage)
{
	CString strReturn;
	CString strRealConvertData(strConvertData);
	int len = strRealConvertData.GetLength();
	int unicodeLen = MultiByteToWideChar(iSourceCodepage, 0, strRealConvertData, -1, NULL, 0);

	wchar_t *pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	if (NULL != pUnicode)
	{
		memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
		MultiByteToWideChar(iSourceCodepage, 0, strConvertData, -1, (LPWSTR)pUnicode, unicodeLen);

		BYTE *pTargetData = NULL ;
		int targetLen = WideCharToMultiByte(iTargetCodepage, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);

		pTargetData = new BYTE[targetLen + 1];
		if (NULL != pTargetData)
		{
			memset(pTargetData, 0, targetLen + 1);

			WideCharToMultiByte(iTargetCodepage, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, targetLen, NULL, NULL);

			strReturn.Format(_T("%s"), pTargetData);

			delete []pTargetData;
		}
		delete []pUnicode;
	}

	return strReturn ;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::GetSendDataTypeDesc
//
// 功能描述：获取发送数据类型描述
//
// 输入参数：IN const int& iDataType
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadHttp::GetSendDataTypeDesc(IN const int& iDataType)
{
	CString strReturn;
	switch (iDataType)
	{
	case JSON_DT_SEND_DIBIT:
		strReturn.Format(_T("扣款请求"));
		break;
	case JSON_DT_SEND_DIBIT_CHECK:
		strReturn.Format(_T("扣费查询"));
		break;
	case JSON_DT_SEND_DIBIT_CANCEL:
		strReturn.Format(_T("撤销订单"));
		break;
	case JSON_DT_SEND_NFC_SIGN_IN:
		strReturn.Format(_T("NFC签到"));
		break;
	}
	return strReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadHttp::GetServerResponseCodeDesc
//
// 功能描述：获取服务器回馈代码描述
//
// 输入参数：IN const int& iResponseType
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CThreadHttp::GetServerResponseCodeDesc(IN const int& iResponseType)
{
	CString strReturn;
	switch (iResponseType)
	{
	case HTTP_RC_DATA_LEGAL:
		strReturn.Format(_T("回馈数据符合规则"));
		break;
	case HTTP_RC_DATA_VALID:
		strReturn.Format(_T("数据正常"));
		break;
	case HTTP_RC_BLACKLIST:
		strReturn.Format(_T("二维码绑定支付用户在特微黑名单当中"));
		break;
	case HTTP_RC_NO_REGISTER:
		strReturn.Format(_T("二维码绑定支付用户未注册"));
		break;
	case HTTP_RC_NO_ENTRY_BY_ID:
		strReturn.Format(_T("二维码绑定支付用户无有效入口信息"));
		break;
	case HTTP_RC_ID_IS_RUNNING:
		strReturn.Format(_T("二维码绑定支付用户已经路网有入口"));
		break;
	case HTTP_RC_DEBIT_NEED_PASSWORD:
		strReturn.Format(_T("需要二维码绑定支付用户输入密码"));
		break;
	case HTTP_RC_DEBIT_CODE_TIMEOUT:
		strReturn.Format(_T("二维码有效期超时"));
		break;
	case HTTP_RC_DEBIT_SRC_ILLGAL:
		strReturn.Format(_T("二维码来源非法"));
		break;
	case HTTP_RC_DEBIT_INSUFFICIENT_BALANCE:
		strReturn.Format(_T("余额不足"));
		break;
	case HTTP_RC_DEBIT_FAIL:
		strReturn.Format(_T("扣费失败"));
		break;
	case HTTP_RC_DEBIT_SUCCESS:
		strReturn.Format(_T("扣费成功"));
		break;
	case HTTP_RC_DEBIT_NO_VOUCHER:
		strReturn.Format(_T("协议格式错误"));
		break;
	case HTTP_RC_PROTOCOL_ERROR:
		strReturn.Format(_T("车道产生的交易凭证关联的订单信息在平台没有找到"));
		break;
	case HTTP_RC_PAYPLATROMR_ERROR:
		strReturn.Format(_T("支付平台错误"));
		break;
	case HTTP_RC_NO_CANCEL_ORDER:
		strReturn.Format(_T("订单已过有效期，禁止撤销"));
		break;
	case HTTP_RC_NO_SERVICES:
		strReturn.Format(_T("平台暂停服务"));
		break;
	default:
		strReturn.Format(_T("平台产生无法解析的内部异常"));
		break;
	}
	return strReturn;
}

