// ThreadUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "TWSDNetPay_Dll.h"
#include "ThreadUpdate.h"


// CThreadUpdate

IMPLEMENT_DYNCREATE(CThreadUpdate, CWinThread)

CThreadUpdate::CThreadUpdate()
{
	m_bExit = false;
	m_bDownLoad = false;

	m_nCommport = 3;

	nHeartSpan = 60000;	
	nUpdateSpan = 300000;
	nDownLoadSpan = 30000;

	dwHeartCheck = ::GetTickCount();
	dwUpdateCheck = ::GetTickCount();
	dwDwonLoadCheck = ::GetTickCount();

	m_csVersion		= _T("1.0.1");
	m_csHeartURL	= _T("");
	m_csQueryURL	= _T("");
	m_csObtainURL	= _T("");
	m_csConfirmURL	= _T("");

	m_csLogMsg		= _T("");

	m_csUpdateLDB	= _T("");
	m_csLDBTable	= _T("");
}

CThreadUpdate::~CThreadUpdate()
{
}

BOOL CThreadUpdate::InitInstance()
{
	m_csLogMsg.Format( _T("[播报更新线程]: 线程已启动，正在读取本地配置信息") );
	GetGlobalApp()->RecordLog( m_csLogMsg );

	ReadWebConfig();


	m_csLogMsg.Format( _T("[播报更新线程]: 正在读取本地已有播报数据") );
	GetGlobalApp()->RecordLog( m_csLogMsg );

	//初始化先读取本地的缓存更新信息
	CString	csErr = _T("");
	LockFileUpdate();
	CreateLocalDB3(m_csUpdateLDB, m_csLDBTable, csErr );

	c_update_data_http.m_aryUpdateInfo.RemoveAll();
	GetLocalDB2Array(m_csUpdateLDB, m_csLDBTable, c_update_data_http, 0, csErr);
	FreeFileUpdate();

	return TRUE;
}

int CThreadUpdate::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadUpdate, CWinThread)
	ON_THREAD_MESSAGE(WM_THREAD_UPDATE_CONFIRM,	OnUpdateConfirm)
END_MESSAGE_MAP()


// CThreadUpdate message handlers

BOOL CThreadUpdate::OnIdle(LONG lCount)
{
	if( m_bExit )
	{
		m_csLogMsg.Format( _T("[播报更新线程]: 收到退出指令，即将退出线程执行") );
		GetGlobalApp()->RecordLog( m_csLogMsg );

		c_update_data_http.m_aryUpdateInfo.RemoveAll();

		::PostQuitMessage(0);//要求退出线程
		return FALSE;
	}
	
	MainHeart();


	if( m_bDownLoad )
	{
		MainDownLoad();
	}
	else
	{
		MainUpdate();
	}

	::Sleep(20);
	return TRUE;
}

void CThreadUpdate::ThrQuit()
{
	m_bExit = true;
}

void CThreadUpdate::ReadWebConfig()
{
	CString m_csCfgFile = _T("");
	m_csCfgFile.Format(_T("%s\\%s"),m_sTranstParam.PathCommDir,FILE_NAME_APP_CONFIG);

	m_csUpdateLDB.Format(_T("%s\\TWSDNetPayLog\\TWITMS.db3"), m_sTranstParam.PathCommDir);
	
	m_csLDBTable.Format( _T("ITMSUpdate") );

	char szbuff[64] = {0};
	GetPrivateProfileString(_T("ReflectWeb"), _T("IP"), _T("101.201.155.190"),	szbuff, (DWORD)64, m_csCfgFile );
	m_csWebIP.Format( _T("%s"), szbuff);	m_csWebIP.Trim();

	m_nWebPort	=	::GetPrivateProfileInt(_T("IMTS"),	_T("Port"),		8080,		m_csCfgFile );

	nHeartSpan		=	::GetPrivateProfileInt(_T("IMTS"),	_T("HeartSpan"),		60000,		m_csCfgFile );
	nUpdateSpan		=	::GetPrivateProfileInt(_T("IMTS"),	_T("UpdateSpan"),		300000,		m_csCfgFile );
	nDownLoadSpan	=	::GetPrivateProfileInt(_T("IMTS"),	_T("DownLoadSpan"),		30000,		m_csCfgFile );

//#ifdef _DEBUG
//	nUpdateSpan = 10000;
//	nDownLoadSpan = 20000;
//	m_csWebIP.Format(_T("192.168.102.31"));
//#endif

	CString m_csDevCfg = _T("");
	m_csDevCfg.Format( _T("%s\\HTSMConfig.ini"), m_sTranstParam.PathCommDir);
	m_nCommport		=	::GetPrivateProfileInt(_T("SerialParam"),	_T("Port"),		3,			m_csDevCfg);

	/*
	if( nHeartSpan < 30000)//心跳间隔不能小于 30s
	{
		nHeartSpan = 30000;
	}
	if( nUpdateSpan < 180000)//更新检测间隔不能小于 180s
	{
		nUpdateSpan = 180000;
	}
	if( nDownLoadSpan < 30000 )//更新下载间隔不能小于 30s
	{
		nDownLoadSpan = 30000;
	}
	*/

	//http://101.201.155.190:8080/trawe-itms/
	//心跳URL
	m_csHeartURL.Format(_T("http://%s:%d/trawe-itms/interface/t_heartbeat"), m_csWebIP, m_nWebPort);

	//更新查询URL
	m_csQueryURL.Format(_T("http://%s:%d/trawe-itms/interface/u_update_query"), m_csWebIP, m_nWebPort);

	//执行包获取URL
	m_csObtainURL.Format(_T("http://%s:%d/trawe-itms/interface/u_update_download"), m_csWebIP, m_nWebPort);

	//确认URL
	m_csConfirmURL.Format(_T("http://%s:%d/trawe-itms/interface/u_update_confirm"), m_csWebIP, m_nWebPort);

}

CString CThreadUpdate::StringConvertByCodePage(CString strConvertData, int iSourceCodepage, int iTargetCodepage)
{
	CString strReturn;
	CString strRealConvertData(strConvertData);
	int len = strRealConvertData.GetLength();
	int unicodeLen = MultiByteToWideChar(iSourceCodepage,0,strRealConvertData,-1,NULL,0);

	wchar_t *pUnicode;  
	pUnicode = new wchar_t[unicodeLen+1];
	if(NULL != pUnicode)
	{
		memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));
		MultiByteToWideChar(iSourceCodepage,0,strConvertData,-1,(LPWSTR)pUnicode,unicodeLen);

		BYTE *pTargetData = NULL ;
		int targetLen = WideCharToMultiByte(iTargetCodepage,0,(LPWSTR)pUnicode,-1,(char*)pTargetData,0,NULL,NULL);

		pTargetData = new BYTE[targetLen+1];
		if(NULL != pTargetData)
		{
			memset(pTargetData,0,targetLen+1);

			WideCharToMultiByte(iTargetCodepage,0,(LPWSTR)pUnicode,-1,(char*)pTargetData,targetLen,NULL,NULL);

			strReturn.Format(_T("%s"),pTargetData);

			delete []pTargetData;
		}
		delete []pUnicode;
	}

	return strReturn ;
}




void CThreadUpdate::HTTPDeal(LPVOID lpParam)
{
	UINT dwReturn = 0x01;
	CString strErrorDesc = _T("");
	if(NULL == lpParam)
	{
		strErrorDesc.Format( _T("[播报更新线程]: HTTP处理失败,参数结构体指针为空") );
		GetGlobalApp()->RecordLog( strErrorDesc );
		return;
	}
	tagHttpParam* pParam = (tagHttpParam*)lpParam;

	//计时开始
	DWORD dwStart = ::GetTickCount();

	//创建网络连接对象
	CInternetSession session(_T("HttpClient"));
	CHttpConnection	*pHtCon	= NULL;
	CHttpFile		*pHtFile= NULL;

	int iTaskNo = 0x00;
	try
	{
		do
		{
			iTaskNo = 0x01;
			if( m_bExit ) break;//程序退出则强制结束

			BOOL b = InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);

			session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,			pParam->SessionReceiveTimeout);	//接收超时
			session.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT,	pParam->SessionControlTimeout);	//响应超时

			//解析URL的正确性	
			WORD		wGetPort;		//用于保存目标HTTP服务端口   
			CString		strServer;		//用于保存服务器地址
			CString		strObject;		//用于保存文件对象名称   
			DWORD		dwServiceType;	//用于保存服务类型
			if( !AfxParseURL(pParam->URL, dwServiceType, strServer, strObject, wGetPort) )
			{
				dwReturn = 2;
				strErrorDesc.Format( _T("[播报更新线程]: HTTP处理失败,解析URL[%s]发现异常,%s"), pParam->URL, theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}


			iTaskNo = 0x02;
			if( m_bExit ) break;//程序退出则强制结束

			//建立网络连接   
			pHtCon = session.GetHttpConnection(strServer, wGetPort);
			if( NULL == pHtCon )  
			{
				dwReturn = 4;
				//建立网络连接失败   
				session.Close();

				strErrorDesc.Format(_T("[播报更新线程]: HTTP处理失败,建立网络连接出现异常,%s"), theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}


			iTaskNo = 0x03;
			if( m_bExit ) break;//程序退出则强制结束

			//发起GET/POST请求，默认使用POST
		//	pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_EXISTING_CONNECT);
			pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
			if( NULL == pHtFile )  
			{
				dwReturn = 5;
				//发起请求失败
				session.Close();
				delete pHtCon;
				pHtCon = NULL;

				strErrorDesc.Format(_T("[播报更新线程]: HTTP处理失败,发起请求出现异常,%s"), theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}

			if( m_bExit ) break;//程序退出则强制结束

			CString strHeaders = _T("Content-Type: application/json;charset=UTF-8"); // 请求头
			pHtFile->AddRequestHeaders(strHeaders);

			iTaskNo = 0x04;
			pHtFile->SendRequestEx(pParam->SendData.GetLength());

			iTaskNo = 0x05;
			pHtFile->WriteString(pParam->SendData);

			iTaskNo = 0x06;
			pHtFile->EndRequest();

			iTaskNo = 0x07;
			pParam->ServerResponseCode = 0x00;
			pHtFile->QueryInfoStatusCode(pParam->ServerResponseCode);



			if( m_bExit ) break;//程序退出则强制结束
			iTaskNo = 0x08;
			pParam->ServerResponseData.Empty();

			//接收数据最长不允许超过2K
			DWORD dwReceiveSize = 2048;
			CString strSingleData;
			while(pHtFile->ReadString(strSingleData))
			{
				if( m_bExit ) break;//程序退出则强制结束

				pParam->ServerResponseData += strSingleData;
				::Sleep(10);

				//接收数据不允许超过指定值
				if(pParam->ServerResponseData.GetLength() > dwReceiveSize)
				{
					break;
				}
			}
			pParam->ServerResponseData.TrimRight();
			CString strTempData = pParam->ServerResponseData;
			pParam->ServerResponseData = StringConvertByCodePage(strTempData, CP_UTF8, CP_ACP);

		//	GetGlobalApp()->RecordLog( pParam->ServerResponseData, LOG_LEVEL_ERROR );

			iTaskNo = 0x09;
			if(HTTP_STATUS_OK != pParam->ServerResponseCode )
			{
				dwReturn = 7;
				//服务器不接受请求
				session.Close();
				delete pHtCon;
				pHtCon = NULL;

				delete pHtFile;
				pHtFile = NULL;

				strErrorDesc.Format(_T("[播报更新线程]: HTTP处理失败,获取服务器状态回馈出现异常,%s"), theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}

			iTaskNo = 0x0a;
			//标记返回成功
			dwReturn = OP_SUCCESS;

		}
		while( false );

	}
	catch(CInternetException* e)
	{
		dwReturn = 9;
		DWORD dwErorCode = ::GetLastError();
		TCHAR info[1024] = {0};
		e->GetErrorMessage(info,1024);
		strErrorDesc.Format(_T("[播报更新线程]: HTTP处理失败,服务器异常,详细描述:(%s,任务号 %d,错误代码:%d)"), info, iTaskNo, dwErorCode );
		GetGlobalApp()->RecordLog( strErrorDesc );
		e->Delete();
	}
	catch(...)
	{
	}



	if(NULL != pParam)
	{
		pParam->ExecuteCode = dwReturn;
	}

	//关闭文件
	if(NULL != pHtFile)
	{
		pHtFile->Close();
		delete pHtFile;
		pHtFile = NULL;
	}
	//关闭链接
	if(NULL != pHtCon)
	{
		pHtCon->Close();
		delete pHtCon;
		pHtCon = NULL;
	}
	//关闭会话
	session.Close();

	//统计用时
	pParam->ConsumeTime = ::GetTickCount() - dwStart;

//	return dwReturn;
}











void CThreadUpdate::MainHeart()
{
	DWORD	dwCurrent = ::GetTickCount();
	if( dwCurrent >= dwHeartCheck )//正常计时，未到49.7天的溢出周期
	{
		if( dwCurrent - dwHeartCheck >= nHeartSpan)//默认 60 S检测一次
		{
			m_csLogMsg.Format( _T("[播报更新线程]: 心跳处理") );
			GetGlobalApp()->RecordLog( m_csLogMsg );

			ReportHeartState();

			dwHeartCheck = ::GetTickCount();
		}
	}
	else//当前获取的时间片小于上一次，已到溢出周期
	{
		dwHeartCheck = dwCurrent;	//重置上次时间片，等待下次循环处理
	}
}




void CThreadUpdate::ReportHeartState()
{
	CString strSendData = _T("HeartCheck");
	CString csError = _T("");
	try
	{
		//构造参数
		tagHttpParam sParam;

		//填充参数结构体
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout= 4000;
		sParam.SessionControlTimeout= 5000;

		sParam.CallMode	= 1;
		sParam.Port		= m_nWebPort;
		sParam.URL		= m_csHeartURL;
		//转化成UTF-8格式

		strSendData.Format( _T("%s"), GetHeartJson( 0 ) );	strSendData.Trim();
		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[播报更新线程]: 构造HTTP心跳json串(%s)异常: 长度过短"), strSendData );
			GetGlobalApp()->RecordLog( csError );
			return;
		}
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		DWORD dwTime1 = ::GetTickCount();
		HTTPDeal(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 发送HTTP心跳未知异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
	}
}




int CThreadUpdate::ComputeAES_MD5(int nProvince, CString strVer, CString strDevno, CString strDevType, time_t tTime, CString& strAes, CString& strMD5)
{
	CString csValue = _T("");
	CString csTemp = _T("");
	CString csZero = _T("000000000000000000000000000000");
	CString strRealKey = _T("");

	try
	{
		//依次构造计算字符串
		//5位省份编码
		csTemp.Format( _T("%05d"), nProvince );
		csValue += csTemp;

		//10位版本号
		csTemp.Format( _T("%s%s"), csZero, strVer );
		csValue += csTemp.Right(10);

		//23位设备编码
		csTemp.Format( _T("%s%s"), csZero, strDevno );
		csValue += csTemp.Right(23);

		//10位设备类型
		csTemp.Format( _T("%s%s"), csZero, strDevType );
		csValue += csTemp.Right(10);

		//时间
		CTime	tt(tTime);
		csTemp.Format(_T("%04d%02d%02d%02d%02d%02d"), tt.GetYear(),tt.GetMonth(),tt.GetDay(),tt.GetHour(),tt.GetMinute(),tt.GetSecond());
		csValue += csTemp;

		//MD5校验
		CMD5 md5((LPCSTR)csValue);		//使用原始字串结算MD5
		strMD5 = md5.GetMD5();
	}
	catch(...)
	{
		csTemp.Format( _T("[播报更新线程]: 计算数据(%s, %s, %s)加密未知异常: %d"), strDevno, strDevType, CTime(tTime).Format("%Y-%m-%d %H:%M:%S"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csTemp );

		return -1;
	}
	return 0;
}




CString CThreadUpdate::GetDeviceNo(int nProvince, int nRoad, CString strSTID, int nLane)
{
	CString	csNo = _T("");
	CString	csTemp = _T("");

	csTemp.Format( _T("000000000000000000000000000000%s"), strSTID );
	csNo.Format( _T("%02d%s%03d%s"), nProvince, csTemp.Right(10), nLane,theApp.m_strTWProductSerialNo.Left(8));
	return csNo;
}


CString CThreadUpdate::GetHeartJson(int nState)
{
	CString csJson = _T("");
	CString csError = _T("");
	try
	{
		int		nProvinceID	= m_sTranstParam.ProvinceID;
		int		nRoadID		= m_sTranstParam.LaneConfigInfo.RoadID;
		int		nLaneID		= _ttoi(m_sTranstParam.LaneConfigInfo.LaneID);
		CString csStationID = _T("");
		csStationID.Format( _T("%s"), m_sTranstParam.LaneConfigInfo.StationID );	csStationID.Trim();

		CString	csDeviceNo = GetDeviceNo(nProvinceID, nRoadID, csStationID, nLaneID);
		CString csDeviceType = _T("TR200");

		time_t	tHeartTime = ::time(NULL);
		CString csAES = _T("");
		CString csMD5 = _T("");

		if( 0 == ComputeAES_MD5(nProvinceID, m_csVersion, csDeviceNo, csDeviceType, tHeartTime, csAES, csMD5) )
		{
			Json::Value root;

			root["aes"]				= Json::Value( _T("no aes") );
			root["md5"]				= Json::Value( csMD5 );
			root["version"]			= Json::Value( m_csVersion );
			root["packageType"]		= Json::Value( 4 );
			root["time"]			= Json::Value( CTime(tHeartTime).Format("%Y-%m-%d %H:%M:%S.000") );
			root["provinceId"]		= Json::Value( nProvinceID );
			root["roadId"]			= Json::Value( nRoadID );
			root["stationId"]		= Json::Value( csStationID );
			root["laneId"]			= Json::Value( nLaneID );
			root["deviceNo"]		= Json::Value( csDeviceNo );
			root["deviceType"]		= Json::Value( csDeviceType );

			root["state"]			= Json::Value( nState );
			root["heartbeatTime"]	= Json::Value( CTime(tHeartTime).Format("%Y-%m-%d %H:%M:%S.000") );


			//转化为字符串
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[播报更新线程]: 计算HTTP心跳串MD5异常: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 构造HTTP心跳json串异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );

		csJson = _T("");
	}

	return csJson;
}


void CThreadUpdate::MainUpdate()
{
	DWORD	dwCurrent = ::GetTickCount();
	if( dwCurrent >= dwUpdateCheck )//正常计时，未到49.7天的溢出周期
	{
		if( dwCurrent - dwUpdateCheck >= nUpdateSpan)//默认 300 S检测一次
		{
			m_csLogMsg.Format( _T("[播报更新线程]: 更新检测处理") );
			GetGlobalApp()->RecordLog( m_csLogMsg );

			CheckHTTPUpdate();

			dwUpdateCheck = ::GetTickCount();
		}
	}
	else//当前获取的时间片小于上一次，已到溢出周期
	{
		dwUpdateCheck = dwCurrent;	//重置上次时间片，等待下次循环处理
	}
}



void CThreadUpdate::CheckHTTPUpdate()
{
	CString strSendData = _T("UpdateCheck");
	CString csError = _T("");
	try
	{
		//构造参数
		tagHttpParam sParam;

		//填充参数结构体
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout= 4000;
		sParam.SessionControlTimeout= 5000;

		sParam.CallMode	= 1;
		sParam.Port		= m_nWebPort;
		sParam.URL		= m_csQueryURL;
		//转化成UTF-8格式

		strSendData.Format( _T("%s"), GetQueryJson( c_update_data_http ) );	strSendData.Trim();

		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[播报更新线程]: 构造HTTP查询json串(%s)异常: 长度过短"), strSendData );
			GetGlobalApp()->RecordLog( csError );
			return;
		}
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		DWORD dwTime1 = ::GetTickCount();
		HTTPDeal(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

		//判断服务器回馈结果
		if(OP_SUCCESS == sParam.ExecuteCode)
		{
			CString	csReceiveData = _T("");
			csReceiveData = sParam.ServerResponseData;
			
			if( CheckQueryResult( csReceiveData ) )//检查结果是否存在更新
			{
				m_csLogMsg.Format( _T("[播报更新线程]: 检测到更新，进入下载处理") );
				GetGlobalApp()->RecordLog( m_csLogMsg );

				m_bDownLoad = true;
				dwDwonLoadCheck = ::GetTickCount();
			}
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 发送HTTP更新检测未知异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
	}
}



CString CThreadUpdate::GetQueryJson(CUpdateData& cUpdate)
{
	CString csJson = _T("");
	CString csError = _T("");
	try
	{
		int		nProvinceID	= m_sTranstParam.ProvinceID;
		int		nRoadID		= m_sTranstParam.LaneConfigInfo.RoadID;
		int		nLaneID		= _ttoi(m_sTranstParam.LaneConfigInfo.LaneID);
		CString csStationID = _T("");
		csStationID.Format( _T("%s"), m_sTranstParam.LaneConfigInfo.StationID );	csStationID.Trim();

		CString	csDeviceNo = GetDeviceNo(nProvinceID, nRoadID, csStationID, nLaneID);
		CString csDeviceType = _T("TR200");

		time_t	tQueryTime = ::time(NULL);
		CString csAES = _T("");
		CString csMD5 = _T("");

		if( 0 == ComputeAES_MD5(nProvinceID, m_csVersion, csDeviceNo, csDeviceType, tQueryTime, csAES, csMD5) )
		{
			Json::Value root;
			Json::Value infos;
			Json::Value json_null;

			root["aes"]				= Json::Value( _T("no aes") );
			root["md5"]				= Json::Value( csMD5 );
			root["version"]			= Json::Value( m_csVersion );
			root["packageType"]		= Json::Value( 4 );
			root["time"]			= Json::Value( CTime(tQueryTime).Format("%Y-%m-%d %H:%M:%S.000") );
			root["provinceId"]		= Json::Value( nProvinceID );
			root["roadId"]			= Json::Value( nRoadID );
			root["stationId"]		= Json::Value( csStationID );
			root["laneId"]			= Json::Value( nLaneID );
			root["deviceNo"]		= Json::Value( csDeviceNo );
			root["deviceType"]		= Json::Value( csDeviceType );

			root["queryTime"]		= Json::Value( CTime(tQueryTime).Format("%Y-%m-%d %H:%M:%S.000") );


			bool bFind = false;
			int	iPos = 0;

			Json::Value info1;
			info1["type"]			= Json::Value( 1 );
			if( cUpdate.m_aryUpdateInfo.GetSize() > 0 )
			{
				bFind = false;
				iPos = 0;
				for(int i=0; i<cUpdate.m_aryUpdateInfo.GetSize(); ++i)
				{
					if( 1 == cUpdate.m_aryUpdateInfo[i].Type )
					{
						bFind = true;
						iPos = i;
						break;
					}
				}

				if( bFind )
				{
					info1["infoNo"]			= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoNo );
					info1["infoFileName"]	= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoName );
					info1["infoVersion"]	= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoVersion );
				}
				else
				{
					info1["infoNo"]			= json_null;
					info1["infoFileName"]	= json_null;
					info1["infoVersion"]	= json_null;
				}
			}
			else
			{
				info1["infoNo"]			= json_null;
				info1["infoFileName"]	= json_null;
				info1["infoVersion"]	= json_null;
			}
			infos.append( info1 );


			Json::Value info2;
			info2["type"]			= Json::Value( 2 );
			if( cUpdate.m_aryUpdateInfo.GetSize() > 0 )
			{
				bFind = false;
				iPos = 0;
				for(int i=0; i<cUpdate.m_aryUpdateInfo.GetSize(); ++i)
				{
					if( 2 == cUpdate.m_aryUpdateInfo[i].Type )
					{
						bFind = true;
						iPos = i;
						break;
					}
				}

				if( bFind )
				{
					info2["infoNo"]			= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoNo );
					info2["infoFileName"]	= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoName );
					info2["infoVersion"]	= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoVersion );
				}
				else
				{
					info2["infoNo"]			= json_null;
					info2["infoFileName"]	= json_null;
					info2["infoVersion"]	= json_null;
				}
			}
			else
			{
				info2["infoNo"]			= json_null;
				info2["infoFileName"]	= json_null;
				info2["infoVersion"]	= json_null;
			}
			infos.append( info2 );

			Json::Value info3;
			info3["type"]			= Json::Value( 3 );
			if( cUpdate.m_aryUpdateInfo.GetSize() > 0 )
			{
				bFind = false;
				iPos = 0;
				for(int i=0; i<cUpdate.m_aryUpdateInfo.GetSize(); ++i)
				{
					if( 3 == cUpdate.m_aryUpdateInfo[i].Type )
					{
						bFind = true;
						iPos = i;
						break;
					}
				}

				if( bFind )
				{
					info3["infoNo"]			= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoNo );
					info3["infoFileName"]	= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoName );
					info3["infoVersion"]	= Json::Value( cUpdate.m_aryUpdateInfo[iPos].InfoVersion );
				}
				else
				{
					info3["infoNo"]			= json_null;
					info3["infoFileName"]	= json_null;
					info3["infoVersion"]	= json_null;
				}
			}
			else
			{
				info3["infoNo"]			= json_null;
				info3["infoFileName"]	= json_null;
				info3["infoVersion"]	= json_null;
			}
			infos.append( info3 );

			root["infos"] = infos;

			//转化为字符串
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[播报更新线程]: 计算HTTP更新查询串MD5异常: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 构造HTTP更新查询json串异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );

		csJson = _T("");
	}

	return csJson;
}



CString CThreadUpdate::GetSafeJsonString(Json::Value j_value, int nLen, bool bTime)
{
	CString	csRet = _T("");
	if( bTime )
	{
		csRet.Format( _T("1970-01-01 08:00:00") );
	}

	if(Json::stringValue == j_value.type())
	{
		csRet.Format( _T("%s"), j_value.asCString() );

		if( csRet.GetLength() > nLen )
		{
			csRet = csRet.Left( nLen );
		}
	}
	return csRet;
}


bool CThreadUpdate::CheckQueryResult(CString csResult)
{
	CString csError = _T("");

	try
	{
		Json::Reader reader;
		Json::Value  root;
		CString csJSON = _T("");
		
		csJSON.Format( _T("%s"), csResult );

		if( reader.parse( csJSON.GetBuffer(0), root) )
		{
			csJSON.ReleaseBuffer();

			int		nValue = 0;
			CString csValue = _T("");
			//updateFlag
			if(Json::intValue == root["updateFlag"].type())
			{
				nValue = root["updateFlag"].asInt();
			}
			else
			{
				csError.Format( _T("[播报更新线程]: 获取查询请求返回 updateFlag 数值异常：数据类型不符") );
				GetGlobalApp()->RecordLog( csError );
				return false;
			}

			if( 1 == nValue )//0：无更新项；1：有更新项
			{
				//updateCode
				if(Json::stringValue == root["updateCode"].type())
				{
					c_update_data_http.UpdateCode = root["updateCode"].asCString();
				}
				else
				{
					csError.Format( _T("[播报更新线程]: 获取查询请求返回 updateCode 数值异常：数据类型不符") );
					GetGlobalApp()->RecordLog( csError );
					return false;
				}

				Json::Value  infos;
				infos = root["infos"];
				for(int i=0; i<infos.size(); ++i)
				{
					nValue = infos[i]["type"].asInt();

					//根据更新类型查找队列中是否存在，有则更新，无则新增
					bool	bFind = false;
					int		iPos = 0;
					for(int j=0; j<c_update_data_http.m_aryUpdateInfo.GetSize(); ++j)
					{
						if( nValue == c_update_data_http.m_aryUpdateInfo[j].Type )
						{
							bFind = true;
							iPos = j;
							break;
						}
					}

					if( bFind )//当前已存在
					{
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoNo, _T("%s"),		GetSafeJsonString(infos[i]["infoNo"], 30) );
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoName, _T("%s"),	GetSafeJsonString(infos[i]["infoFileName"], 62) );
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoVersion, _T("%s"),	GetSafeJsonString(infos[i]["infoVersion"], 30) );
					}
					else//不存在，新增
					{
						tagUpdateInfo	sAdd;
						::memset(&sAdd, 0, sizeof(tagUpdateInfo));

						sAdd.Type = nValue;
						::_stprintf( sAdd.InfoNo, _T("%s"),			GetSafeJsonString(infos[i]["infoNo"], 30) );
						::_stprintf( sAdd.InfoName, _T("%s"),		GetSafeJsonString(infos[i]["infoFileName"], 62) );
						::_stprintf( sAdd.InfoVersion, _T("%s"),	GetSafeJsonString(infos[i]["infoVersion"], 30) );

						c_update_data_http.m_aryUpdateInfo.Add( sAdd );
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			csError.Format( _T("[播报更新线程]: 解析HTTP更新查询返回json串(%s)异常: %d"), csJSON, ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			return false;
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 解析HTTP更新查询返回json串异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
		return false;
	}
}










void CThreadUpdate::MainDownLoad()
{
	DWORD	dwCurrent = ::GetTickCount();
	if( dwCurrent >= dwDwonLoadCheck )//正常计时，未到49.7天的溢出周期
	{
		if( dwCurrent - dwDwonLoadCheck >= nDownLoadSpan)//默认 30 S检测一次
		{
			m_csLogMsg.Format( _T("[播报更新线程]: 更新下载处理") );
			GetGlobalApp()->RecordLog( m_csLogMsg );

			DownLoadHTTPUpdate();

			dwDwonLoadCheck = ::GetTickCount();
		}
	}
	else//当前获取的时间片小于上一次，已到溢出周期
	{
		dwDwonLoadCheck = dwCurrent;	//重置上次时间片，等待下次循环处理
	}
}



void CThreadUpdate::DownLoadHTTPUpdate()
{
	CString strSendData = _T("DownLoadCheck");
	CString csError = _T("");
	try
	{
		//构造参数
		tagHttpParam sParam;

		//填充参数结构体
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout= 4000;
		sParam.SessionControlTimeout= 5000;

		sParam.CallMode	= 1;
		sParam.Port		= m_nWebPort;
		sParam.URL		= m_csObtainURL;
		//转化成UTF-8格式

		strSendData.Format( _T("%s"), GetDownLoadJson( c_update_data_http ) );	strSendData.Trim();

		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[播报更新线程]: 构造HTTP下载json串(%s)异常: 长度过短"), strSendData );
			GetGlobalApp()->RecordLog( csError );
			return;
		}
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		DWORD dwTime1 = ::GetTickCount();
		HTTPDeal(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

		//判断服务器回馈结果
		if(OP_SUCCESS == sParam.ExecuteCode)
		{
			CString	csReceiveData = _T("");
			csReceiveData = sParam.ServerResponseData;
			
			if( CheckDownLoadResult( csReceiveData ) )
			{
				m_csLogMsg.Format( _T("[播报更新线程]: 成功获得更新数据，进行本地保存处理") );
				GetGlobalApp()->RecordLog( m_csLogMsg );

				//成功获取更新内容，存储到本地
				LockFileUpdate();
				int iRet = SaveData2LocalDB3(m_csUpdateLDB, m_csLDBTable, c_update_data_http, csError);
				FreeFileUpdate();

				if( 0 != iRet )//对保存出错的情况，清除当前内存缓存，后续重新获取
				{
					m_csLogMsg.Format( _T("[播报更新线程]: 保存更新数据到本地异常(%s)"), csError );
					GetGlobalApp()->RecordLog( m_csLogMsg );

					c_update_data_http.ErrorCount = 0;
					c_update_data_http.UpdateCode.Empty();
					c_update_data_http.m_aryUpdateInfo.RemoveAll();
				}
				
				m_bDownLoad = false;
				dwDwonLoadCheck = ::GetTickCount();
			}
			else//未成功获取到下载内容
			{
				c_update_data_http.ErrorCount += 1;
			}
		}
		else
		{
			//服务器反馈异常
			c_update_data_http.ErrorCount += 1;
		}

		if( c_update_data_http.ErrorCount > 3 )//3次异常后，重置错误并取消本次下载
		{
			csError.Format( _T("[播报更新线程]: 下载HTTP更新(%s)多次异常，已取消本次更新下载"), c_update_data_http.UpdateCode );
			GetGlobalApp()->RecordLog( csError );

			c_update_data_http.ErrorCount = 0;
			c_update_data_http.UpdateCode.Empty();
			m_bDownLoad = false;
			dwDwonLoadCheck = ::GetTickCount();
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 发送HTTP更新检测未知异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
	}
}



CString CThreadUpdate::GetDownLoadJson(CUpdateData& cUpdate)
{
	CString csJson = _T("");
	CString csError = _T("");
	try
	{
		int		nProvinceID	= m_sTranstParam.ProvinceID;
		int		nRoadID		= m_sTranstParam.LaneConfigInfo.RoadID;
		int		nLaneID		= _ttoi(m_sTranstParam.LaneConfigInfo.LaneID);
		CString csStationID = _T("");
		csStationID.Format( _T("%s"), m_sTranstParam.LaneConfigInfo.StationID );	csStationID.Trim();

		CString	csDeviceNo = GetDeviceNo(nProvinceID, nRoadID, csStationID, nLaneID);
		CString csDeviceType = _T("TR200");

		time_t	tDownTime = ::time(NULL);
		CString csAES = _T("");
		CString csMD5 = _T("");

		if( 0 == ComputeAES_MD5(nProvinceID, m_csVersion, csDeviceNo, csDeviceType, tDownTime, csAES, csMD5) )
		{
			Json::Value root;
			Json::Value infos;

			root["aes"]				= Json::Value( _T("no aes") );
			root["md5"]				= Json::Value( csMD5 );
			root["version"]			= Json::Value( m_csVersion );
			root["packageType"]		= Json::Value( 4 );
			root["time"]			= Json::Value( CTime(tDownTime).Format("%Y-%m-%d %H:%M:%S.000") );
			root["provinceId"]		= Json::Value( nProvinceID );
			root["roadId"]			= Json::Value( nRoadID );
			root["stationId"]		= Json::Value( csStationID );
			root["laneId"]			= Json::Value( nLaneID );
			root["deviceNo"]		= Json::Value( csDeviceNo );
			root["deviceType"]		= Json::Value( csDeviceType );

			root["requstTime"]		= Json::Value( CTime(tDownTime).Format("%Y-%m-%d %H:%M:%S.000") );
			root["updateCode"]		= Json::Value( cUpdate.UpdateCode );
			if( cUpdate.m_aryUpdateInfo.GetSize() > 0 )
			{
				for(int i=0; i<cUpdate.m_aryUpdateInfo.GetSize(); ++i)
				{
					Json::Value info1;

					info1["type"]			= Json::Value( cUpdate.m_aryUpdateInfo[i].Type );
					info1["infoNo"]			= Json::Value( cUpdate.m_aryUpdateInfo[i].InfoNo );
					info1["infoFileName"]	= Json::Value( cUpdate.m_aryUpdateInfo[i].InfoName );
					info1["infoVersion"]	= Json::Value( cUpdate.m_aryUpdateInfo[i].InfoVersion );
					infos.append( info1 );
				}
			}
			root["infos"] = infos;

			//转化为字符串
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[播报更新线程]: 计算HTTP更新下载串MD5异常: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 构造HTTP更新下载json串异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );

		csJson = _T("");
	}

	return csJson;
}



bool CThreadUpdate::CheckDownLoadResult(CString csResult)
{
	CString csError = _T("");

	try
	{
		Json::Reader reader;
		Json::Value  root;
		CString csJSON = _T("");
		
		csJSON.Format( _T("%s"), csResult );

		if( reader.parse( csJSON.GetBuffer(0), root) )
		{
			csJSON.ReleaseBuffer();

			int		nValue = 0;
			//errorCode
			if(Json::stringValue == root["errorCode"].type())
			{
				nValue = _ttoi(root["errorCode"].asCString());
			}
			else
			{
				csError.Format( _T("[播报更新线程]: 获取下载请求返回 errorCode 数值异常：数据类型不符") );
				GetGlobalApp()->RecordLog( csError );
				return false;
			}

			if( 0 == nValue )//0000	下载成功
			{
				Json::Value  datas;
				datas = root["dataTypes"];
				for(int i=0; i<datas.size(); ++i)
				{
					nValue = datas[i]["type"].asInt();

					//根据更新类型查找队列中是否存在，有则更新，无则新增
					bool	bFind = false;
					int		iPos = 0;
					for(int j=0; j<c_update_data_http.m_aryUpdateInfo.GetSize(); ++j)
					{
						if( nValue == c_update_data_http.m_aryUpdateInfo[j].Type )
						{
							bFind = true;
							iPos = j;
							break;
						}
					}

					if( bFind )//当前已存在
					{
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoVersion, _T("%s"),	GetSafeJsonString(datas[i]["version"], 30) );
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].MD5, _T("%s"),			GetSafeJsonString(datas[i]["md5"], 36) );

						c_update_data_http.m_aryUpdateInfo[iPos].UpdateTime	= theApp.CString2time_t( GetSafeJsonString(datas[i]["updateTime"], 24, true) );
						c_update_data_http.m_aryUpdateInfo[iPos].LoadTime	= theApp.CString2time_t( GetSafeJsonString(datas[i]["loadTime"], 24, true) );
						c_update_data_http.m_aryUpdateInfo[iPos].BeginDate	= theApp.CString2time_t( GetSafeJsonString(datas[i]["beginDate"], 24, true) );
						c_update_data_http.m_aryUpdateInfo[iPos].EndDate		= theApp.CString2time_t( GetSafeJsonString(datas[i]["endDate"], 24, true) );

						c_update_data_http.m_aryUpdateInfo[iPos].IsCondense	= ((Json::intValue == datas[i]["isCondense"].type()) ? datas[i]["isCondense"].asInt() : 0);
						c_update_data_http.m_aryUpdateInfo[iPos].FileType	= ((Json::intValue == datas[i]["fileType"].type()) ? datas[i]["fileType"].asInt() : 0);

						if( 1 == nValue )//1文字广告，限定64字节
						{
							::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].Content, _T("%s"),		GetSafeJsonString(datas[i]["fileContent"], 64) );
						}
						else if( 2 == nValue )//2语音广告，限定128字节
						{
							::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].Content, _T("%s"),		GetSafeJsonString(datas[i]["fileContent"], 128) );
						}
						else if( 3 == nValue )//3更新终端欢迎界面，限定8字节
						{
							::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].Content, _T("%s"),		GetSafeJsonString(datas[i]["fileContent"], 8) );
						}
					}
					else//不存在，新增
					{
						tagUpdateInfo	sAdd;
						::memset(&sAdd, 0, sizeof(tagUpdateInfo));

						sAdd.Type = nValue;

						::_stprintf( sAdd.InfoVersion, _T("%s"),	GetSafeJsonString(datas[i]["version"], 30) );
						::_stprintf( sAdd.MD5, _T("%s"),			GetSafeJsonString(datas[i]["md5"], 36) );

						sAdd.UpdateTime	= theApp.CString2time_t( GetSafeJsonString(datas[i]["updateTime"], 24, true) );
						sAdd.LoadTime	= theApp.CString2time_t( GetSafeJsonString(datas[i]["loadTime"], 24, true) );
						sAdd.BeginDate	= theApp.CString2time_t( GetSafeJsonString(datas[i]["beginDate"], 24, true) );
						sAdd.EndDate	= theApp.CString2time_t( GetSafeJsonString(datas[i]["endDate"], 24, true) );

						sAdd.IsCondense	= ((Json::intValue == datas[i]["isCondense"].type()) ? datas[i]["isCondense"].asInt() : 0);
						sAdd.FileType	= ((Json::intValue == datas[i]["fileType"].type()) ? datas[i]["fileType"].asInt() : 0);

						if( 1 == nValue )//1文字广告，限定64字节
						{
							::_stprintf( sAdd.Content, _T("%s"),	GetSafeJsonString(datas[i]["fileContent"], 64) );
						}
						else if( 2 == nValue )//2语音广告，限定128字节
						{
							::_stprintf( sAdd.Content, _T("%s"),	GetSafeJsonString(datas[i]["fileContent"], 128) );
						}
						else if( 3 == nValue )//3更新终端欢迎界面，限定8字节
						{
							::_stprintf( sAdd.Content, _T("%s"),	GetSafeJsonString(datas[i]["fileContent"], 8) );
						}

						c_update_data_http.m_aryUpdateInfo.Add( sAdd );
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			csError.Format( _T("[播报更新线程]: 解析HTTP更新下载返回json串(%s)异常: %d"), csJSON, ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			return false;
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 解析HTTP更新下载返回json串异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
		return false;
	}
}
 
void CThreadUpdate::OnUpdateConfirm(WPARAM wParam, LPARAM lParam)
{
	CString strSendData = _T("UpdateConfirm");
	CString csError = _T("");
	try
	{
		//构造参数
		tagHttpParam sParam;

		//填充参数结构体
		sParam.ExecuteCode			= 0xFF;
		sParam.ConsumeTime			= 0;
		sParam.ServerResponseData	= _T("");
		sParam.ServerResponseCode	= 0;
		sParam.NeedEndThread		= FALSE;
		sParam.SessionReceiveTimeout= 4000;
		sParam.SessionControlTimeout= 5000;

		sParam.CallMode	= 1;
		sParam.Port		= m_nWebPort;
		sParam.URL		= m_csConfirmURL;


		m_csLogMsg.Format( _T("[播报更新线程]: 更新确认处理") );
		GetGlobalApp()->RecordLog( m_csLogMsg );


		CString csConfirmCode = _T("");
		int		nConfirmState = 0;
		csConfirmCode.Format( _T("%s"), GetGlobalApp()->m_pThreadManage->m_pThreadDevice->c_update_device.UpdateCode );

		strSendData.Format( _T("%s"), GetConfirmJson( csConfirmCode, nConfirmState ) );	strSendData.Trim();

		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[播报更新线程]: 构造HTTP更新确认json串(%s)异常: 长度过短"), strSendData );
			GetGlobalApp()->RecordLog( csError );
			return;
		}
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		DWORD dwTime1 = ::GetTickCount();
		HTTPDeal(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 发送HTTP更新确认未知异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
	}
}



CString CThreadUpdate::GetConfirmJson(CString strCode, int nState)
{
	CString csJson = _T("");
	CString csError = _T("");
	try
	{
		int		nProvinceID	= m_sTranstParam.ProvinceID;
		int		nRoadID		= m_sTranstParam.LaneConfigInfo.RoadID;
		int		nLaneID		= _ttoi(m_sTranstParam.LaneConfigInfo.LaneID);
		CString csStationID = _T("");
		csStationID.Format( _T("%s"), m_sTranstParam.LaneConfigInfo.StationID );	csStationID.Trim();

		CString	csDeviceNo = GetDeviceNo(nProvinceID, nRoadID, csStationID, nLaneID);
		CString csDeviceType = _T("TR200");

		time_t	tConfirmTime = ::time(NULL);
		CString csAES = _T("");
		CString csMD5 = _T("");

		if( 0 == ComputeAES_MD5(nProvinceID, m_csVersion, csDeviceNo, csDeviceType, tConfirmTime, csAES, csMD5) )
		{
			Json::Value root;
			Json::Value infos;

			root["aes"]				= Json::Value( _T("no aes") );
			root["md5"]				= Json::Value( csMD5 );
			root["version"]			= Json::Value( m_csVersion );
			root["packageType"]		= Json::Value( 4 );
			root["time"]			= Json::Value( CTime(tConfirmTime).Format("%Y-%m-%d %H:%M:%S.000") );
			root["provinceId"]		= Json::Value( nProvinceID );
			root["roadId"]			= Json::Value( nRoadID );
			root["stationId"]		= Json::Value( csStationID );
			root["laneId"]			= Json::Value( nLaneID );
			root["deviceNo"]		= Json::Value( csDeviceNo );
			root["deviceType"]		= Json::Value( csDeviceType );

			root["updateCode"]		= Json::Value( strCode );
			root["updateState"]		= Json::Value( nState );

			//转化为字符串
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[播报更新线程]: 计算HTTP更新确认串MD5异常: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[播报更新线程]: 构造HTTP更新确认json串异常: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );

		csJson = _T("");
	}

	return csJson;
}


//创建本地保存的Sqlite数据库
int CThreadUpdate::CreateLocalDB3(LPCTSTR strDBFile, LPCTSTR strTableName, CString& csError)
{
	sqlite3 * sq3db;
	int rc = 0, ncols=0;
	sqlite3_stmt *stmt;
	const char * tail;
	char * zErr = NULL;
	CString csSQL = _T("");

	CString csCreateTB = _T("");
	csCreateTB.Format( _T(" CREATE TABLE %s(UpdateCode varchar(40) NOT NULL, InfoType int NOT NULL, InfoNo varchar(16), \
						  InfoName varchar(32), InfoVersion varchar(16), UpdateTime datetime, LoadTime datetime, \
						  BeginDate datetime, EndDate datetime, IsCondense int, FileType int, Content varchar(128), \
						  MD5 varchar(40), Spare int, SpareString varchar(128), TransferMark int, PRIMARY KEY(InfoType)) "), strTableName );

	errno_t err = 0;
	if((err = _access_s( strDBFile, 0 )) == 0 )
	{
		::SetFileAttributes(strDBFile, FILE_ATTRIBUTE_NORMAL);

		rc = sqlite3_open( strDBFile, &sq3db);	//SQLITE_OK
		if(rc != SQLITE_OK)
		{
			csError.Format( _T("Can't open DB(%s) database: %s\n"), strDBFile, sqlite3_errmsg(sq3db) );
			sqlite3_close(sq3db);
			return -11;
		}
		else
		{
			csSQL.Format( _T("select * from sqlite_master where name='%s';"), strTableName );
			rc = sqlite3_prepare(sq3db, csSQL.GetBuffer(0), csSQL.GetLength(), &stmt, &tail);	//SQLITE_OK
			csSQL.ReleaseBuffer();
			if(rc != SQLITE_OK)
			{
				csError.Format( _T("check DB(%s) table(%s) prepare error: %s\n"), strDBFile, strTableName, sqlite3_errmsg(sq3db) );
				sqlite3_close(sq3db);
				return -12;
			}
			else
			{
				rc = sqlite3_step(stmt);			//SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW, SQLITE_ERROR, SQLITE_MISUSE
				ncols = sqlite3_column_count(stmt);
				sqlite3_finalize(stmt);	//提前这里结束，是为了后面可能的建表语句
				
				int nTable = -1;
				if( rc == SQLITE_ROW)	//至少一条数据
				{
					nTable = 1;
				}
				else if( rc == SQLITE_DONE)//检索成功，没有数据
				{
					nTable = 0;

					rc = sqlite3_exec(sq3db, csCreateTB.GetBuffer(0), NULL, NULL, &zErr);
					csCreateTB.ReleaseBuffer();
					if(rc != SQLITE_OK) 
					{
						if (zErr != NULL) 
						{
							csError.Format( _T(" DB(%s) reCreate table(%s) SQL error: %s"), strDBFile, strTableName, zErr );
							sqlite3_free(zErr);
						}
					}
					else nTable = 1;
				}
				else	// SQLITE_BUSY, SQLITE_ERROR	SQLite处理 忙、或者错误
				{
					csError.Format( _T("check DB(%s) table(%s) step error: %s\n"), strDBFile, strTableName, sqlite3_errmsg(sq3db) );
				}

				sqlite3_close(sq3db);		//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY

				if( 1 == nTable )	return 0;
				else				return -13;
			}
		}
	}
	else//当前数据库不存在，创建新库
	{
		rc = sqlite3_open( strDBFile, &sq3db);
		if( rc != SQLITE_OK )
		{
			csError.Format( _T("Can't open (%s) database: %s\n"), strDBFile, sqlite3_errmsg(sq3db) );
			sqlite3_close(sq3db);
			return -1;
		}

		rc = sqlite3_exec(sq3db, csCreateTB.GetBuffer(0), NULL, NULL, &zErr);
		csCreateTB.ReleaseBuffer();
		if(rc != SQLITE_OK) 
		{
			if (zErr != NULL) 
			{
				csError.Format( _T("Create DB(%s) table(%s) SQL error: %s"), strDBFile, strTableName, zErr );
				sqlite3_free(zErr);
			}
			sqlite3_close(sq3db);

			::DeleteFile( strDBFile );
			return -2;
		}

		sqlite3_close(sq3db);
		return 0;
	}
}


//保存数据到本地数据库
int CThreadUpdate::SaveData2LocalDB3(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, CString& csError)
{
	CString csCreate = _T("");
	CString csInsert = _T("");
	CString csTemp = _T("");

	//获取建表语句
	csCreate.Format( _T(" CREATE TABLE %s(UpdateCode varchar(40) NOT NULL, InfoType int NOT NULL, InfoNo varchar(16), \
						InfoName varchar(32), InfoVersion varchar(16), UpdateTime datetime, LoadTime datetime, \
						BeginDate datetime, EndDate datetime, IsCondense int, FileType int, Content varchar(128), \
						MD5 varchar(40), Spare int, SpareString varchar(128), TransferMark int, PRIMARY KEY(InfoType)) "), strTBName );

	int rc;
	sqlite3 *db;
	char *zErr;
	rc = sqlite3_open( strDBFile, &db);	//SQLITE_OK
	if(rc != SQLITE_OK)
	{
		csError.Format( _T("Can't open (%s) database: %s\n"), strDBFile, sqlite3_errmsg(db) );
		sqlite3_close(db);
		return -1;
	}

	for(int i=0; i<cUpdate.m_aryUpdateInfo.GetSize(); ++i)
	{
		//获取插值语句
		csInsert.Format( _T(" DELETE FROM %s where InfoType=%d; \
							INSERT INTO %s(UpdateCode, InfoType, InfoNo, InfoName, InfoVersion, UpdateTime, LoadTime, \
							BeginDate, EndDate, IsCondense, FileType, Content, MD5, Spare, SpareString, TransferMark) \
							VALUES('%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', %d, '%s', %d) "),\
							strTBName, cUpdate.m_aryUpdateInfo[i].Type, \
							strTBName, cUpdate.UpdateCode, cUpdate.m_aryUpdateInfo[i].Type, cUpdate.m_aryUpdateInfo[i].InfoNo, \
							cUpdate.m_aryUpdateInfo[i].InfoName, cUpdate.m_aryUpdateInfo[i].InfoVersion, \
							CTime( cUpdate.m_aryUpdateInfo[i].UpdateTime ).Format("%Y-%m-%d %H:%M:%S"), \
							CTime( cUpdate.m_aryUpdateInfo[i].LoadTime ).Format("%Y-%m-%d %H:%M:%S"), \
							CTime( cUpdate.m_aryUpdateInfo[i].BeginDate ).Format("%Y-%m-%d %H:%M:%S"), \
							CTime( cUpdate.m_aryUpdateInfo[i].EndDate ).Format("%Y-%m-%d %H:%M:%S"), \
							cUpdate.m_aryUpdateInfo[i].IsCondense, cUpdate.m_aryUpdateInfo[i].FileType, \
							cUpdate.m_aryUpdateInfo[i].Content, cUpdate.m_aryUpdateInfo[i].MD5, \
							cUpdate.m_aryUpdateInfo[i].Spare, _T(""), 0 );

		rc = sqlite3_exec(db, csInsert.GetBuffer(0), NULL, NULL, &zErr);
		csInsert.ReleaseBuffer();

		if(rc != SQLITE_OK)
		{
			if (zErr != NULL) 
			{
				csError.Format( _T(" DB(%s) Table(%s) insert SQL error: %s"), strDBFile, strTBName, zErr );
				sqlite3_free(zErr);
			}

			if(1 == rc)	//insert no table
			{
				rc = sqlite3_exec(db, csCreate.GetBuffer(0), NULL, NULL, &zErr);
				csCreate.ReleaseBuffer();
				if(rc != SQLITE_OK) 
				{
					if (zErr != NULL) 
					{
						csError.Format( _T(" DB(%s) Table(%s) ReCreate table SQL error: %s"), strDBFile, strTBName, zErr );
						sqlite3_free(zErr);
					}
					sqlite3_close(db);
					return -3;
				}
				else
				{
					rc = sqlite3_exec(db, csInsert.GetBuffer(0), NULL, NULL, &zErr);
					csInsert.ReleaseBuffer();

					if(rc != SQLITE_OK) 
					{
						if (zErr != NULL) 
						{
							csError.Format( _T(" DB(%s) Table(%s) ReInsert data SQL error: %s"), strDBFile, strTBName, zErr );
							sqlite3_free(zErr);
						}
						sqlite3_close(db);
						return -4;
					}
					else//重新建表后，插入成功
					{
						continue;	//对于可能存在两个的情况，继续处理
					}
				}
			}
			else
			{
				sqlite3_close(db);
				return -2;
			}
		}
		else
		{
			continue;	//对于可能存在两个的情况，继续处理
		}
	}

	//中间过程无错误返回，则认为成功
	sqlite3_close(db);	//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY
	return 0;
}






int CThreadUpdate::GetLocalDB2Array(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, int nType, CString& csError)
{
	int	nReturn = 0;
	int rc, i, ncols;
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *tail;
	CString csSQL = _T("");

	errno_t err = 0;
    if((err = _access_s( strDBFile, 0 )) == 0 )
	{
		rc = sqlite3_open( strDBFile, &db);	//SQLITE_OK
		if(rc != SQLITE_OK)
		{
			csError.Format( _T("Can't open DB(%s) database: %s\n"), strDBFile, sqlite3_errmsg(db) );
			sqlite3_close(db);
			return -1;
		}
		
		if( 0 == nType )//不增加条件选取
		{
			csSQL.Format( _T(" select * from %s limit 9; "), strTBName);//目前最多只会有两条数据
		}
		else//条件选取
		{
			csSQL.Format( _T(" select * from %s where (TransferMark&1)<>1 limit 9; "), strTBName);//目前最多只会有两条数据
		}
		rc = sqlite3_prepare(db, csSQL.GetBuffer(0), csSQL.GetLength(), &stmt, &tail);	//SQLITE_OK
		csSQL.ReleaseBuffer();
		if(rc != SQLITE_OK)
		{
			csError.Format( _T(" DB(%s) table(%s) select SQL prepare error: %s\n"), strDBFile, strTBName, sqlite3_errmsg(db) );
			sqlite3_close(db);
			return -2;
		}

		rc = sqlite3_step(stmt);			//SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW, SQLITE_ERROR, SQLITE_MISUSE
		ncols = sqlite3_column_count(stmt);
		
		if( rc == SQLITE_ROW)	//至少一条数据
		{
			do
			{
				CString csDateName = _T("");
				tagUpdateInfo	sData;
				::memset(&sData, 0, sizeof(tagUpdateInfo));

				for(i=0; i < ncols; ++i) 
				{
					//获取列原始名称，用 sqlite3_column_origin_name，否则用 sqlite3_column_name 获取临时名称
					csDateName.Format( _T("%s"), sqlite3_column_origin_name(stmt, i) );

					//InfoType int NOT NULL,	InfoNo varchar(16),	InfoName varchar(32),	InfoVersion varchar(16),
					if( 0 == csDateName.CompareNoCase("InfoType") )			{	sData.Type = sqlite3_column_int(stmt, i);}
					else if( 0 == csDateName.CompareNoCase("InfoNo") )		{::sprintf(sData.InfoNo, "%s", sqlite3_column_text(stmt, i));}
					else if( 0 == csDateName.CompareNoCase("InfoName") )	{::sprintf(sData.InfoName, "%s", sqlite3_column_text(stmt, i));}
					else if( 0 == csDateName.CompareNoCase("InfoVersion") )	{::sprintf(sData.InfoVersion, "%s", sqlite3_column_text(stmt, i));}

					//UpdateTime datetime,	LoadTime datetime,	BeginDate datetime,	EndDate datetime, 
					else if( 0 == csDateName.CompareNoCase("UpdateTime") )	{	sData.UpdateTime	= theApp.CString2time_t( (LPCTSTR)sqlite3_column_text(stmt, i) );	}
					else if( 0 == csDateName.CompareNoCase("LoadTime") )	{	sData.LoadTime		= theApp.CString2time_t( (LPCTSTR)sqlite3_column_text(stmt, i) );	}
					else if( 0 == csDateName.CompareNoCase("BeginDate") )	{	sData.BeginDate		= theApp.CString2time_t( (LPCTSTR)sqlite3_column_text(stmt, i) );	}
					else if( 0 == csDateName.CompareNoCase("EndDate") )		{	sData.EndDate		= theApp.CString2time_t( (LPCTSTR)sqlite3_column_text(stmt, i) );	}

					//IsCondense int,	FileType int,	Content varchar(128),	MD5 varchar(40), 
					else if( 0 == csDateName.CompareNoCase("IsCondense") )	{	sData.IsCondense = sqlite3_column_int(stmt, i);}
					else if( 0 == csDateName.CompareNoCase("FileType") )	{	sData.FileType = sqlite3_column_int(stmt, i);}
					else if( 0 == csDateName.CompareNoCase("Content") )		{::sprintf(sData.Content, "%s", sqlite3_column_text(stmt, i));}
					else if( 0 == csDateName.CompareNoCase("MD5") )			{::sprintf(sData.MD5, "%s", sqlite3_column_text(stmt, i));}

					//Spare int,	SpareString varchar(128),	TransferMark int,
					else if( 0 == csDateName.CompareNoCase("Spare") )		{	sData.Spare	= sqlite3_column_int(stmt, i);}
				//	else if( 0 == csDateName.CompareNoCase("SpareString") )	{::sprintf(sData.sp, "%s", sqlite3_column_text(stmt, i));}
					else if( 0 == csDateName.CompareNoCase("TransferMark") ){	sData.TransferMark= sqlite3_column_int(stmt, i);}
					else
					{
						// default:	in this case , do nothing!
					}
				}
				cUpdate.m_aryUpdateInfo.Add( sData );

				nReturn += 1;
				rc = sqlite3_step(stmt);
			}
			while(rc == SQLITE_ROW);
		}
		else //SQLITE_DONE, SQLITE_BUSY, SQLITE_ERROR	SQLite处理 无数据、忙、或者错误
		{
			if( rc == SQLITE_DONE )	
			{
				nReturn = 0;	// no data to transfer
			}
			else
			{
				csError.Format( _T(" DB(%s) table(%s) SQL step error: %s\n"), strDBFile, strTBName, sqlite3_errmsg(db) );
				nReturn = -3;	// error in the select
			}
		}

		sqlite3_finalize(stmt);	//SQLITE_OK	SQLITE_ABORT	SQLITE_BUSY
		sqlite3_close(db);		//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY
	}
	else
	{
		nReturn = -9;
	}

	return nReturn;
}



