// ThreadNFCSignIn.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadNFCSignIn.h"


HttpClientLibCurl CThreadNFCSignIn::m_pHttp;
// CThreadNFCSignIn

IMPLEMENT_DYNCREATE(CThreadNFCSignIn, CThreadHttp)

CThreadNFCSignIn::CThreadNFCSignIn()
{
}

CThreadNFCSignIn::~CThreadNFCSignIn()
{
}

BOOL CThreadNFCSignIn::InitInstance()
{
	fnReadSpecailConfig();
	return TRUE;
}

int CThreadNFCSignIn::ExitInstance()
{
	//清理资源
	fnReleaseResource();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadNFCSignIn, CThreadHttp)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_ISSUE_DATA,OnMsgTransportSignInData)
	ON_THREAD_MESSAGE(WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_POS,OnMsgRevSignDataFromPOS)
END_MESSAGE_MAP()


// CThreadNFCSignIn message handlers
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadNFCSignIn::OnIdle
//     
// 功能描述：空闲处理函数
//     
// 输入参数：LONG lCount
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadNFCSignIn::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		//对本线程抛出退出消息
		::PostQuitMessage(0);

		return FALSE;
	}

	//检查银联非接模块
	if(::GetGlobalApp()->m_bNFCIsUse)
	{
		BOOL bNeedReSignIn = FALSE;
		CString strReSignText;
		//重签条件1：如果非接模块当前处于签到失败状态，并且非接模块不在签到进行中，则进行签到操作
		if(!bNeedReSignIn)
		{
			if(    !GetGlobalApp()->m_bNFCSignInSuccess
				&& (UMS_SIGNING != GetGlobalApp()->GetUnipayWorkFlowState()))
			{
				static time_t tLastCheckUnipaySignTime = time(NULL);
				if( (time(NULL)-tLastCheckUnipaySignTime) >  GetGlobalApp()->m_iNFCCheckSignSpanTime)
				{
					bNeedReSignIn = TRUE;
					strReSignText.Format(_T("上次重签失败或初始化签到"));
					tLastCheckUnipaySignTime = time(NULL);
				}
			}
		}

		//重签条件2：如果非接支付失败(连续3次),但是已经签到成功，则进行重新签到
		if(    !bNeedReSignIn
			&& (3 == GetGlobalApp()->m_iNFCPayFailCount)
			)
		{
			bNeedReSignIn = TRUE;
			strReSignText.Format(_T("交易失败重签(连续3次)"));
		}

		


		//重签条件3：每天固定时间进行签到
		if(!bNeedReSignIn)
		{
			CTime cCurrentTime = CTime::GetCurrentTime();
			if(    (cCurrentTime.GetHour() >= ::GetGlobalApp()->m_iNFCDefaultStartReSignHour ) 
				&& (cCurrentTime.GetHour() <= ::GetGlobalApp()->m_iNFCDefaultEndReSignHour))
			{
				if(!GetGlobalApp()->m_bNFCAutoSignIsExecute)
				{
					//modify zhuyabing 2017-10
					static time_t tLastCheckAutoSignTime = time(NULL);
					if(    ((time(NULL) - tLastCheckAutoSignTime) > 10)
						&& (UMS_SIGNING != GetGlobalApp()->GetUnipayWorkFlowState())
						)
					{
						bNeedReSignIn = TRUE;
						strReSignText.Format(_T("每天定时重签"));
						tLastCheckAutoSignTime= time(NULL);
					}
					
				}
				else
				{
					bNeedReSignIn = FALSE;
				}
			}
            else
            {
                GetGlobalApp()->m_bNFCAutoSignIsExecute= false;
            }
		}

		//执行重签操作（进行判断）
		if(bNeedReSignIn)
		{
			static time_t tLastCheckSignInOpTime = 0;
			if((time(NULL) - tLastCheckSignInOpTime)>30)
			{
				CString strLog;
				strLog.Format(_T("[NFC签到线程]:系统判断NFC非接模块需要进行签到流程(%s)，准备发起签到操作"),strReSignText);
				GetGlobalApp()->RecordLog(strLog);
				//通知业务管理线程，需要进行签到操作
				::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_NFC_START_SIGN,(WPARAM)0,(LPARAM)0);
				::GetGlobalApp()->m_tNFCStartSignInTime = time(NULL);
				//重设时间
				tLastCheckSignInOpTime = time(NULL);
			}
		}
	}

	::Sleep(100);

	return CThreadBase::OnIdle(lCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadNFCSignIn::OnMsgTransportSignInData
//     
// 功能描述：发送签到数据给云端
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_NFC_ISSUE_DATA
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::OnMsgTransportSignInData(WPARAM wParam,LPARAM lParam)
{
	//设置工作状态
	m_bWorking = TRUE;

	CString strLog;
	DWORD	dwServerReturnResponseCode = 0x00;

	//解析数据
	WORD wSendDataType				= LOWORD(lParam);
	//是否需要回馈调用者
	BYTE byIsNeedResponseCaller		= LOBYTE(HIWORD(lParam));
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
		if(!DataConvertFromJson2String(wSendDataType,pDVJsonReceive,strURL,wPort,strSendData,hWnd,iCallMode,&pJsonIF,0))
			throw -2;
		strLog.Format(_T("[NFC签到线程]:已接收到请求[类型:%s,%s]，准备发送数据"),GetSendDataTypeDesc(wSendDataType),pDVJsonReceive->PayIdentifier);
		GetGlobalApp()->RecordLog(strLog);
		strLog.Empty();

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
		if( INVALID_HANDLE_VALUE == (hThreadHandle = StartProcessThread_WinHttp(&sParam)) )
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
			//抛出异常，描述通讯超时
			strResponseResult.Format(_T("服务超时无响应"));
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
//		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[NFC签到线程]:发送数据到服务器(请求[类型:%s])，详细数据:%s;获取回馈数据:%s"),GetSendDataTypeDesc(wSendDataType),strSendData,strResponseResult));
#endif
		//判断服务器回馈结果
		dwServerReturnResponseCode = pDVJsonBase->ServerResponseCode;

		//通知调用者结果
		CString strNeedNotifyCaller;
		if(0 != m_sTranstParam.CallerThreadID)
		{
			strNeedNotifyCaller.Format(_T(",已通知调用者"));
			::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP,(WPARAM)pDVJsonBase,MAKELONG(wSendDataType,HTTP_RC_DATA_LEGAL ));
		}
		//记录日志
		GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[NFC签到线程]:已接收到服务器回馈[请求类型:%s]，状态:处理成功%s"),GetSendDataTypeDesc(wSendDataType),strNeedNotifyCaller));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://传入参数指针为空
			strLog.Format(_T("[NFC签到线程]:服务器回馈数据失败,传入参数指针为空,回馈业务管理线程代码为:0x01(内部错误)"));
			break;
		case -2://数据格式转换出现异常
			strLog.Format(_T("[NFC签到线程]:服务器回馈数据失败,数据格式转换出现异常,回馈业务管理线程代码为:0x02(内部错误)"));
			break;
		case -3://无法启动数据发送线程
			strLog.Format(_T("[NFC签到线程]:服务器回馈数据失败,无法启动数据发送线程,回馈业务管理线程代码为:0x03(内部错误)"));
			break;
		case -4://回馈数据显示失败
			strLog.Format(_T("[NFC签到线程]:服务器回馈数据失败,回馈业务管理线程代码为:0x04(内部错误)，回馈数据异常，无法解析"));
			break;
		case -0xF0://扣费或查询，回馈结果非成功或正在输入密码，判定内部错误
			strLog.Format(_T("[NFC签到线程]:通讯异常,回馈业务管理线程代码为:0x05(内部错误)，详细描述:%s"),strResponseResult);
			break;
		}
		wReturnCode = abs(iErrorCode);
	
		//回馈调用结果
		if(0 != m_sTranstParam.CallerThreadID)
		{
			::PostThreadMessage(m_sTranstParam.CallerThreadID,WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP,(WPARAM)0,MAKELONG(wSendDataType,wReturnCode));
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

	//设置工作状态
	m_bWorking = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadNFCSignIn::OnMsgRevSignDataFromPOS
//     
// 功能描述：接收从NFC模块上传的签到数据
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_POS
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::OnMsgRevSignDataFromPOS(WPARAM wParam,LPARAM lParam)
{
	CString strSignInData;
	try
	{
		CString* pstrData = (CString*)wParam;
		if(NULL == pstrData)
			throw -1;
		strSignInData.Format(_T("%s"),*pstrData);
		ContructNFCSignInData(strSignInData);

		delete pstrData;
		pstrData = NULL;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadNFCSignIn::fnReleaseResource
//     
// 功能描述：清除本类申请的资源
//     
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::fnReleaseResource()
{
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
const BOOL CThreadNFCSignIn::DataConvertFromJson2String(IN const WORD& wSendDataType,IN OUT CDVJsonBase* pDVJsonReceive,IN OUT CString& strURL,IN OUT INTERNET_PORT& wPort,IN OUT CString& strSendData,IN OUT HWND& hWnd,IN OUT int& iCallMode,IN OUT CJsonOperationIF** pJsonIF, int nSN )
{
	BOOL bReturn = TRUE;

	try
	{
		switch(wSendDataType)
		{
		case JSON_DT_SEND_NFC_SIGN_IN:		//NFC签到
			{
				//获得相应的数据类指针
				CDVJsonNFCSignIn* pDVJsonNFCSignIn = (CDVJsonNFCSignIn* )pDVJsonReceive;
				if(NULL == pDVJsonNFCSignIn)
				{
					throw -1;
				}
				//获得调用者窗体句柄
				hWnd = pDVJsonNFCSignIn->m_hCallerWnd;

				//申请数据操作类
				*pJsonIF = new CJsonNFCOperate();
				if(NULL == *pJsonIF)
				{
					throw -2;
				}
				//转化成JSON数据格式
				strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonNFCSignIn);
				//获得服务端链接信息
				strURL = m_strHttpNFCSignInURL;
				wPort  = m_wHttpNFCSignInPort;
				iCallMode = 2;
			}
			break;
		default:
			throw -3;
		}
	}
	catch(int& iErrorCode)
	{
		bReturn = FALSE;

		switch(iErrorCode)
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

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadNFCSignIn::fnReadSpecailConfig
//     
// 功能描述：读取本类相关的配置信息(从指定配置文件)
//     
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::fnReadSpecailConfig()
{
	TCHAR szTemp[512]={0};
	
	TCHAR szIP[32]={0};
	WORD  wPort = 80;
	BOOL  bAddPort = FALSE;

	//获取公共配置信息
	CString strCommConfigFile;
	strCommConfigFile.Format(_T("%s\\%s"),m_sTranstParam.PathCommDir,FILE_NAME_APP_CONFIG);
	//1.0.3.4版本，调整公网测试环境IP，由60.205.140.198更改为：60.205.140.198
	::GetPrivateProfileString(_T("ReflectWeb"),_T("IP"),_T("60.205.140.198"),szIP,_countof(szIP),strCommConfigFile);
	wPort = (WORD)::GetPrivateProfileInt(_T("ReflectWeb"),_T("Port"),8081,strCommConfigFile);
	bAddPort = (BOOL)::GetPrivateProfileInt(_T("ReflectWeb"),_T("AddPort"),0,strCommConfigFile);
	//子线程等待时间,单位:毫秒，默认超时时间为 8 秒
	m_dwWaitThreadWorkTimeout = (DWORD)::GetPrivateProfileInt(_T("ReflectWeb"),_T("WaitThreadWorkTimeout"),8000,strCommConfigFile);

	if(::PathFileExists(m_strPathLocalConfigFile))
	{
		if(bAddPort)
		{
			//签到URL
			m_wHttpNFCSignInPort		= wPort;
			m_strHttpNFCSignInURL.Format(_T("http://%s:%d/signIn"),szIP,m_wHttpNFCSignInPort);
		}
		else
		{
			//签到URL
			m_wHttpNFCSignInPort		= wPort;
			m_strHttpNFCSignInURL.Format(_T("http://%s/signIn"),szIP);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadNFCSignIn::ContructNFCSignInData
//     
// 功能描述：构造并发送签到数据给云端
//     
// 输入参数：IN const CString& strSignData
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadNFCSignIn::ContructNFCSignInData(IN const CString& strSignData)
{
	try
	{
		//填充扣款信息结构体
		CDVJsonNFCSignIn* pDVJsonNFCSignIn = new CDVJsonNFCSignIn();
		if(NULL == pDVJsonNFCSignIn)
			throw -1;
		CString strPayId = GetGlobalApp()->GetGuidString(1);
		CString strTemp;

		//标识是否测试数据
		pDVJsonNFCSignIn->TestingData			= ::GetGlobalApp()->m_bIsTestData;
		//获取软件版本号
		pDVJsonNFCSignIn->NetPayDllVersion		= ::GetGlobalApp()->m_strNetPayDllVersion;
		pDVJsonNFCSignIn->DeviceDllVersion		= ::GetGlobalApp()->m_strDeviceDllVersion;
		pDVJsonNFCSignIn->TWProductSerialNo		= ::GetGlobalApp()->m_strTWProductSerialNo;

		//填充报文头信息
		pDVJsonNFCSignIn->Version				= _T("1.5");
		pDVJsonNFCSignIn->PackageType			= JSON_DT_SEND_NFC_SIGN_IN;
		pDVJsonNFCSignIn->Token					= _T("");
		pDVJsonNFCSignIn->ProvinceID			= m_sTranstParam.ProvinceID;
		pDVJsonNFCSignIn->SubmitCount			= 0x01;
		pDVJsonNFCSignIn->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
		pDVJsonNFCSignIn->m_hCallerWnd			= NULL;
		
		GetGlobalApp()->HexToBase64(GetGlobalApp()->m_strNFCTerminal,pDVJsonNFCSignIn->NFCTerminal);
		GetGlobalApp()->HexToBase64(GetGlobalApp()->m_strNFCSerialNo,pDVJsonNFCSignIn->NFCSerialNo);
		GetGlobalApp()->HexToBase64(strSignData,pDVJsonNFCSignIn->NFCSignData);

		//发送签到请求
		PostThreadMessage(WM_THREAD_NFC_ISSUE_DATA,(WPARAM)pDVJsonNFCSignIn,(LPARAM)MAKELONG(JSON_DT_SEND_NFC_SIGN_IN,MAKEWORD(0x01,0x00)));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
}




HANDLE CThreadNFCSignIn::StartProcessThread_WinHttp(IN const LPVOID lpParam)
{
	HANDLE hThreadHandle = INVALID_HANDLE_VALUE;
	tagHttpParam* pParam = (tagHttpParam*)lpParam;
	if(NULL != pParam)
	{
		hThreadHandle = (HANDLE)_beginthreadex(NULL,0,&SendData_WinHttp,lpParam,0,NULL);
	}
	
	return hThreadHandle;
}



UINT WINAPI CThreadNFCSignIn::SendData_WinHttp(LPVOID lpParam)
{
	UINT dwReturn = 0x01;
	BOOL bIsHeart = FALSE;
	CString strErrorDesc;
	tagHttpParam* pParam = (tagHttpParam*)lpParam;
	//计时开始
	DWORD dwStart = ::GetTickCount();
	//HttpClientLibCurl* pHttp = NULL;
	if(  1 )
	{
		
		std::string stResponse = "";
		try
		{
			int iRet = m_pHttp.Post(pParam->URL.GetBuffer(), pParam->SendData.GetBuffer(), stResponse, pParam->SessionControlTimeout);
			if(0 ==  iRet)
			{
				pParam->ServerResponseData = CThreadHttp::StringConvertByCodePage(stResponse.c_str(),CP_UTF8,CP_ACP);
				pParam->ExecuteCode = OP_SUCCESS;
			}
			else
			{
				GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[signin]:数据交互过程产生错误,错误代码：%d"), iRet));
				pParam->ExecuteCode = iRet;
			}
			pParam->URL.ReleaseBuffer();pParam->SendData.ReleaseBuffer();
		}
		catch(...)
		{
			GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("[singin]:catch,错误代码：%d"), GetLastError()));
		}
		
		
		//pHttp->FreeInstance();
	}

	//统计用时
	pParam->ConsumeTime = ::GetTickCount() - dwStart;
	//GetGlobalApp()->RecordLog(GetGlobalApp()->FmtStr(_T("In CThreadHttp::SendData(libcurl), 请求URL:%s,cost time: %d"), pParam->URL, pParam->ConsumeTime));
	return 0;
}

