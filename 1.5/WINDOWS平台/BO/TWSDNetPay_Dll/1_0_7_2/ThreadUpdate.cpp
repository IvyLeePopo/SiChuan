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
	m_csLogMsg.Format( _T("[���������߳�]: �߳������������ڶ�ȡ����������Ϣ") );
	GetGlobalApp()->RecordLog( m_csLogMsg );

	ReadWebConfig();


	m_csLogMsg.Format( _T("[���������߳�]: ���ڶ�ȡ�������в�������") );
	GetGlobalApp()->RecordLog( m_csLogMsg );

	//��ʼ���ȶ�ȡ���صĻ��������Ϣ
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
		m_csLogMsg.Format( _T("[���������߳�]: �յ��˳�ָ������˳��߳�ִ��") );
		GetGlobalApp()->RecordLog( m_csLogMsg );

		c_update_data_http.m_aryUpdateInfo.RemoveAll();

		::PostQuitMessage(0);//Ҫ���˳��߳�
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
	if( nHeartSpan < 30000)//�����������С�� 30s
	{
		nHeartSpan = 30000;
	}
	if( nUpdateSpan < 180000)//���¼��������С�� 180s
	{
		nUpdateSpan = 180000;
	}
	if( nDownLoadSpan < 30000 )//�������ؼ������С�� 30s
	{
		nDownLoadSpan = 30000;
	}
	*/

	//http://101.201.155.190:8080/trawe-itms/
	//����URL
	m_csHeartURL.Format(_T("http://%s:%d/trawe-itms/interface/t_heartbeat"), m_csWebIP, m_nWebPort);

	//���²�ѯURL
	m_csQueryURL.Format(_T("http://%s:%d/trawe-itms/interface/u_update_query"), m_csWebIP, m_nWebPort);

	//ִ�а���ȡURL
	m_csObtainURL.Format(_T("http://%s:%d/trawe-itms/interface/u_update_download"), m_csWebIP, m_nWebPort);

	//ȷ��URL
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
		strErrorDesc.Format( _T("[���������߳�]: HTTP����ʧ��,�����ṹ��ָ��Ϊ��") );
		GetGlobalApp()->RecordLog( strErrorDesc );
		return;
	}
	tagHttpParam* pParam = (tagHttpParam*)lpParam;

	//��ʱ��ʼ
	DWORD dwStart = ::GetTickCount();

	//�����������Ӷ���
	CInternetSession session(_T("HttpClient"));
	CHttpConnection	*pHtCon	= NULL;
	CHttpFile		*pHtFile= NULL;

	int iTaskNo = 0x00;
	try
	{
		do
		{
			iTaskNo = 0x01;
			if( m_bExit ) break;//�����˳���ǿ�ƽ���

			BOOL b = InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);

			session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,			pParam->SessionReceiveTimeout);	//���ճ�ʱ
			session.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT,	pParam->SessionControlTimeout);	//��Ӧ��ʱ

			//����URL����ȷ��	
			WORD		wGetPort;		//���ڱ���Ŀ��HTTP����˿�   
			CString		strServer;		//���ڱ����������ַ
			CString		strObject;		//���ڱ����ļ���������   
			DWORD		dwServiceType;	//���ڱ����������
			if( !AfxParseURL(pParam->URL, dwServiceType, strServer, strObject, wGetPort) )
			{
				dwReturn = 2;
				strErrorDesc.Format( _T("[���������߳�]: HTTP����ʧ��,����URL[%s]�����쳣,%s"), pParam->URL, theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}


			iTaskNo = 0x02;
			if( m_bExit ) break;//�����˳���ǿ�ƽ���

			//������������   
			pHtCon = session.GetHttpConnection(strServer, wGetPort);
			if( NULL == pHtCon )  
			{
				dwReturn = 4;
				//������������ʧ��   
				session.Close();

				strErrorDesc.Format(_T("[���������߳�]: HTTP����ʧ��,�����������ӳ����쳣,%s"), theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}


			iTaskNo = 0x03;
			if( m_bExit ) break;//�����˳���ǿ�ƽ���

			//����GET/POST����Ĭ��ʹ��POST
		//	pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_EXISTING_CONNECT);
			pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject);
			if( NULL == pHtFile )  
			{
				dwReturn = 5;
				//��������ʧ��
				session.Close();
				delete pHtCon;
				pHtCon = NULL;

				strErrorDesc.Format(_T("[���������߳�]: HTTP����ʧ��,������������쳣,%s"), theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}

			if( m_bExit ) break;//�����˳���ǿ�ƽ���

			CString strHeaders = _T("Content-Type: application/json;charset=UTF-8"); // ����ͷ
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



			if( m_bExit ) break;//�����˳���ǿ�ƽ���
			iTaskNo = 0x08;
			pParam->ServerResponseData.Empty();

			//�����������������2K
			DWORD dwReceiveSize = 2048;
			CString strSingleData;
			while(pHtFile->ReadString(strSingleData))
			{
				if( m_bExit ) break;//�����˳���ǿ�ƽ���

				pParam->ServerResponseData += strSingleData;
				::Sleep(10);

				//�������ݲ�������ָ��ֵ
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
				//����������������
				session.Close();
				delete pHtCon;
				pHtCon = NULL;

				delete pHtFile;
				pHtFile = NULL;

				strErrorDesc.Format(_T("[���������߳�]: HTTP����ʧ��,��ȡ������״̬���������쳣,%s"), theApp.DisplayWinError(::GetLastError()) );
				GetGlobalApp()->RecordLog( strErrorDesc );
				break;
			}

			iTaskNo = 0x0a;
			//��Ƿ��سɹ�
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
		strErrorDesc.Format(_T("[���������߳�]: HTTP����ʧ��,�������쳣,��ϸ����:(%s,����� %d,�������:%d)"), info, iTaskNo, dwErorCode );
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

	//�ر��ļ�
	if(NULL != pHtFile)
	{
		pHtFile->Close();
		delete pHtFile;
		pHtFile = NULL;
	}
	//�ر�����
	if(NULL != pHtCon)
	{
		pHtCon->Close();
		delete pHtCon;
		pHtCon = NULL;
	}
	//�رջỰ
	session.Close();

	//ͳ����ʱ
	pParam->ConsumeTime = ::GetTickCount() - dwStart;

//	return dwReturn;
}











void CThreadUpdate::MainHeart()
{
	DWORD	dwCurrent = ::GetTickCount();
	if( dwCurrent >= dwHeartCheck )//������ʱ��δ��49.7����������
	{
		if( dwCurrent - dwHeartCheck >= nHeartSpan)//Ĭ�� 60 S���һ��
		{
			m_csLogMsg.Format( _T("[���������߳�]: ��������") );
			GetGlobalApp()->RecordLog( m_csLogMsg );

			ReportHeartState();

			dwHeartCheck = ::GetTickCount();
		}
	}
	else//��ǰ��ȡ��ʱ��ƬС����һ�Σ��ѵ��������
	{
		dwHeartCheck = dwCurrent;	//�����ϴ�ʱ��Ƭ���ȴ��´�ѭ������
	}
}




void CThreadUpdate::ReportHeartState()
{
	CString strSendData = _T("HeartCheck");
	CString csError = _T("");
	try
	{
		//�������
		tagHttpParam sParam;

		//�������ṹ��
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
		//ת����UTF-8��ʽ

		strSendData.Format( _T("%s"), GetHeartJson( 0 ) );	strSendData.Trim();
		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[���������߳�]: ����HTTP����json��(%s)�쳣: ���ȹ���"), strSendData );
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
		csError.Format( _T("[���������߳�]: ����HTTP����δ֪�쳣: %d"), ::GetLastError() );
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
		//���ι�������ַ���
		//5λʡ�ݱ���
		csTemp.Format( _T("%05d"), nProvince );
		csValue += csTemp;

		//10λ�汾��
		csTemp.Format( _T("%s%s"), csZero, strVer );
		csValue += csTemp.Right(10);

		//23λ�豸����
		csTemp.Format( _T("%s%s"), csZero, strDevno );
		csValue += csTemp.Right(23);

		//10λ�豸����
		csTemp.Format( _T("%s%s"), csZero, strDevType );
		csValue += csTemp.Right(10);

		//ʱ��
		CTime	tt(tTime);
		csTemp.Format(_T("%04d%02d%02d%02d%02d%02d"), tt.GetYear(),tt.GetMonth(),tt.GetDay(),tt.GetHour(),tt.GetMinute(),tt.GetSecond());
		csValue += csTemp;

		//MD5У��
		CMD5 md5((LPCSTR)csValue);		//ʹ��ԭʼ�ִ�����MD5
		strMD5 = md5.GetMD5();
	}
	catch(...)
	{
		csTemp.Format( _T("[���������߳�]: ��������(%s, %s, %s)����δ֪�쳣: %d"), strDevno, strDevType, CTime(tTime).Format("%Y-%m-%d %H:%M:%S"), ::GetLastError() );
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


			//ת��Ϊ�ַ���
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[���������߳�]: ����HTTP������MD5�쳣: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP����json���쳣: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );

		csJson = _T("");
	}

	return csJson;
}


void CThreadUpdate::MainUpdate()
{
	DWORD	dwCurrent = ::GetTickCount();
	if( dwCurrent >= dwUpdateCheck )//������ʱ��δ��49.7����������
	{
		if( dwCurrent - dwUpdateCheck >= nUpdateSpan)//Ĭ�� 300 S���һ��
		{
			m_csLogMsg.Format( _T("[���������߳�]: ���¼�⴦��") );
			GetGlobalApp()->RecordLog( m_csLogMsg );

			CheckHTTPUpdate();

			dwUpdateCheck = ::GetTickCount();
		}
	}
	else//��ǰ��ȡ��ʱ��ƬС����һ�Σ��ѵ��������
	{
		dwUpdateCheck = dwCurrent;	//�����ϴ�ʱ��Ƭ���ȴ��´�ѭ������
	}
}



void CThreadUpdate::CheckHTTPUpdate()
{
	CString strSendData = _T("UpdateCheck");
	CString csError = _T("");
	try
	{
		//�������
		tagHttpParam sParam;

		//�������ṹ��
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
		//ת����UTF-8��ʽ

		strSendData.Format( _T("%s"), GetQueryJson( c_update_data_http ) );	strSendData.Trim();

		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[���������߳�]: ����HTTP��ѯjson��(%s)�쳣: ���ȹ���"), strSendData );
			GetGlobalApp()->RecordLog( csError );
			return;
		}
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		DWORD dwTime1 = ::GetTickCount();
		HTTPDeal(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

		//�жϷ������������
		if(OP_SUCCESS == sParam.ExecuteCode)
		{
			CString	csReceiveData = _T("");
			csReceiveData = sParam.ServerResponseData;
			
			if( CheckQueryResult( csReceiveData ) )//������Ƿ���ڸ���
			{
				m_csLogMsg.Format( _T("[���������߳�]: ��⵽���£��������ش���") );
				GetGlobalApp()->RecordLog( m_csLogMsg );

				m_bDownLoad = true;
				dwDwonLoadCheck = ::GetTickCount();
			}
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP���¼��δ֪�쳣: %d"), ::GetLastError() );
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

			//ת��Ϊ�ַ���
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[���������߳�]: ����HTTP���²�ѯ��MD5�쳣: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP���²�ѯjson���쳣: %d"), ::GetLastError() );
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
				csError.Format( _T("[���������߳�]: ��ȡ��ѯ���󷵻� updateFlag ��ֵ�쳣���������Ͳ���") );
				GetGlobalApp()->RecordLog( csError );
				return false;
			}

			if( 1 == nValue )//0���޸����1���и�����
			{
				//updateCode
				if(Json::stringValue == root["updateCode"].type())
				{
					c_update_data_http.UpdateCode = root["updateCode"].asCString();
				}
				else
				{
					csError.Format( _T("[���������߳�]: ��ȡ��ѯ���󷵻� updateCode ��ֵ�쳣���������Ͳ���") );
					GetGlobalApp()->RecordLog( csError );
					return false;
				}

				Json::Value  infos;
				infos = root["infos"];
				for(int i=0; i<infos.size(); ++i)
				{
					nValue = infos[i]["type"].asInt();

					//���ݸ������Ͳ��Ҷ������Ƿ���ڣ�������£���������
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

					if( bFind )//��ǰ�Ѵ���
					{
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoNo, _T("%s"),		GetSafeJsonString(infos[i]["infoNo"], 30) );
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoName, _T("%s"),	GetSafeJsonString(infos[i]["infoFileName"], 62) );
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoVersion, _T("%s"),	GetSafeJsonString(infos[i]["infoVersion"], 30) );
					}
					else//�����ڣ�����
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
			csError.Format( _T("[���������߳�]: ����HTTP���²�ѯ����json��(%s)�쳣: %d"), csJSON, ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			return false;
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP���²�ѯ����json���쳣: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );
		return false;
	}
}










void CThreadUpdate::MainDownLoad()
{
	DWORD	dwCurrent = ::GetTickCount();
	if( dwCurrent >= dwDwonLoadCheck )//������ʱ��δ��49.7����������
	{
		if( dwCurrent - dwDwonLoadCheck >= nDownLoadSpan)//Ĭ�� 30 S���һ��
		{
			m_csLogMsg.Format( _T("[���������߳�]: �������ش���") );
			GetGlobalApp()->RecordLog( m_csLogMsg );

			DownLoadHTTPUpdate();

			dwDwonLoadCheck = ::GetTickCount();
		}
	}
	else//��ǰ��ȡ��ʱ��ƬС����һ�Σ��ѵ��������
	{
		dwDwonLoadCheck = dwCurrent;	//�����ϴ�ʱ��Ƭ���ȴ��´�ѭ������
	}
}



void CThreadUpdate::DownLoadHTTPUpdate()
{
	CString strSendData = _T("DownLoadCheck");
	CString csError = _T("");
	try
	{
		//�������
		tagHttpParam sParam;

		//�������ṹ��
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
		//ת����UTF-8��ʽ

		strSendData.Format( _T("%s"), GetDownLoadJson( c_update_data_http ) );	strSendData.Trim();

		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[���������߳�]: ����HTTP����json��(%s)�쳣: ���ȹ���"), strSendData );
			GetGlobalApp()->RecordLog( csError );
			return;
		}
		sParam.SendData	= StringConvertByCodePage(strSendData, CP_ACP, CP_UTF8);

		DWORD dwTime1 = ::GetTickCount();
		HTTPDeal(&sParam);
		DWORD dwTime2 = ::GetTickCount() - dwTime1;

		//�жϷ������������
		if(OP_SUCCESS == sParam.ExecuteCode)
		{
			CString	csReceiveData = _T("");
			csReceiveData = sParam.ServerResponseData;
			
			if( CheckDownLoadResult( csReceiveData ) )
			{
				m_csLogMsg.Format( _T("[���������߳�]: �ɹ���ø������ݣ����б��ر��洦��") );
				GetGlobalApp()->RecordLog( m_csLogMsg );

				//�ɹ���ȡ�������ݣ��洢������
				LockFileUpdate();
				int iRet = SaveData2LocalDB3(m_csUpdateLDB, m_csLDBTable, c_update_data_http, csError);
				FreeFileUpdate();

				if( 0 != iRet )//�Ա�����������������ǰ�ڴ滺�棬�������»�ȡ
				{
					m_csLogMsg.Format( _T("[���������߳�]: ����������ݵ������쳣(%s)"), csError );
					GetGlobalApp()->RecordLog( m_csLogMsg );

					c_update_data_http.ErrorCount = 0;
					c_update_data_http.UpdateCode.Empty();
					c_update_data_http.m_aryUpdateInfo.RemoveAll();
				}
				
				m_bDownLoad = false;
				dwDwonLoadCheck = ::GetTickCount();
			}
			else//δ�ɹ���ȡ����������
			{
				c_update_data_http.ErrorCount += 1;
			}
		}
		else
		{
			//�����������쳣
			c_update_data_http.ErrorCount += 1;
		}

		if( c_update_data_http.ErrorCount > 3 )//3���쳣�����ô���ȡ����������
		{
			csError.Format( _T("[���������߳�]: ����HTTP����(%s)����쳣����ȡ�����θ�������"), c_update_data_http.UpdateCode );
			GetGlobalApp()->RecordLog( csError );

			c_update_data_http.ErrorCount = 0;
			c_update_data_http.UpdateCode.Empty();
			m_bDownLoad = false;
			dwDwonLoadCheck = ::GetTickCount();
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP���¼��δ֪�쳣: %d"), ::GetLastError() );
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

			//ת��Ϊ�ַ���
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[���������߳�]: ����HTTP�������ش�MD5�쳣: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP��������json���쳣: %d"), ::GetLastError() );
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
				csError.Format( _T("[���������߳�]: ��ȡ�������󷵻� errorCode ��ֵ�쳣���������Ͳ���") );
				GetGlobalApp()->RecordLog( csError );
				return false;
			}

			if( 0 == nValue )//0000	���سɹ�
			{
				Json::Value  datas;
				datas = root["dataTypes"];
				for(int i=0; i<datas.size(); ++i)
				{
					nValue = datas[i]["type"].asInt();

					//���ݸ������Ͳ��Ҷ������Ƿ���ڣ�������£���������
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

					if( bFind )//��ǰ�Ѵ���
					{
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].InfoVersion, _T("%s"),	GetSafeJsonString(datas[i]["version"], 30) );
						::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].MD5, _T("%s"),			GetSafeJsonString(datas[i]["md5"], 36) );

						c_update_data_http.m_aryUpdateInfo[iPos].UpdateTime	= theApp.CString2time_t( GetSafeJsonString(datas[i]["updateTime"], 24, true) );
						c_update_data_http.m_aryUpdateInfo[iPos].LoadTime	= theApp.CString2time_t( GetSafeJsonString(datas[i]["loadTime"], 24, true) );
						c_update_data_http.m_aryUpdateInfo[iPos].BeginDate	= theApp.CString2time_t( GetSafeJsonString(datas[i]["beginDate"], 24, true) );
						c_update_data_http.m_aryUpdateInfo[iPos].EndDate		= theApp.CString2time_t( GetSafeJsonString(datas[i]["endDate"], 24, true) );

						c_update_data_http.m_aryUpdateInfo[iPos].IsCondense	= ((Json::intValue == datas[i]["isCondense"].type()) ? datas[i]["isCondense"].asInt() : 0);
						c_update_data_http.m_aryUpdateInfo[iPos].FileType	= ((Json::intValue == datas[i]["fileType"].type()) ? datas[i]["fileType"].asInt() : 0);

						if( 1 == nValue )//1���ֹ�棬�޶�64�ֽ�
						{
							::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].Content, _T("%s"),		GetSafeJsonString(datas[i]["fileContent"], 64) );
						}
						else if( 2 == nValue )//2������棬�޶�128�ֽ�
						{
							::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].Content, _T("%s"),		GetSafeJsonString(datas[i]["fileContent"], 128) );
						}
						else if( 3 == nValue )//3�����ն˻�ӭ���棬�޶�8�ֽ�
						{
							::_stprintf( c_update_data_http.m_aryUpdateInfo[iPos].Content, _T("%s"),		GetSafeJsonString(datas[i]["fileContent"], 8) );
						}
					}
					else//�����ڣ�����
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

						if( 1 == nValue )//1���ֹ�棬�޶�64�ֽ�
						{
							::_stprintf( sAdd.Content, _T("%s"),	GetSafeJsonString(datas[i]["fileContent"], 64) );
						}
						else if( 2 == nValue )//2������棬�޶�128�ֽ�
						{
							::_stprintf( sAdd.Content, _T("%s"),	GetSafeJsonString(datas[i]["fileContent"], 128) );
						}
						else if( 3 == nValue )//3�����ն˻�ӭ���棬�޶�8�ֽ�
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
			csError.Format( _T("[���������߳�]: ����HTTP�������ط���json��(%s)�쳣: %d"), csJSON, ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			return false;
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP�������ط���json���쳣: %d"), ::GetLastError() );
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
		//�������
		tagHttpParam sParam;

		//�������ṹ��
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


		m_csLogMsg.Format( _T("[���������߳�]: ����ȷ�ϴ���") );
		GetGlobalApp()->RecordLog( m_csLogMsg );


		CString csConfirmCode = _T("");
		int		nConfirmState = 0;
		csConfirmCode.Format( _T("%s"), GetGlobalApp()->m_pThreadManage->m_pThreadDevice->c_update_device.UpdateCode );

		strSendData.Format( _T("%s"), GetConfirmJson( csConfirmCode, nConfirmState ) );	strSendData.Trim();

		if( strSendData.GetLength() < 10 )
		{
			csError.Format( _T("[���������߳�]: ����HTTP����ȷ��json��(%s)�쳣: ���ȹ���"), strSendData );
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
		csError.Format( _T("[���������߳�]: ����HTTP����ȷ��δ֪�쳣: %d"), ::GetLastError() );
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

			//ת��Ϊ�ַ���
			Json::FastWriter fast_writer;
			std::string content = fast_writer.write(root);
			csJson.Format( _T("%s"), content.c_str());
		}
		else
		{
			csError.Format( _T("[���������߳�]: ����HTTP����ȷ�ϴ�MD5�쳣: %d"), ::GetLastError() );
			GetGlobalApp()->RecordLog( csError );
			csJson = _T("");
		}
	}
	catch(...)
	{
		csError.Format( _T("[���������߳�]: ����HTTP����ȷ��json���쳣: %d"), ::GetLastError() );
		GetGlobalApp()->RecordLog( csError );

		csJson = _T("");
	}

	return csJson;
}


//�������ر����Sqlite���ݿ�
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
				sqlite3_finalize(stmt);	//��ǰ�����������Ϊ�˺�����ܵĽ������
				
				int nTable = -1;
				if( rc == SQLITE_ROW)	//����һ������
				{
					nTable = 1;
				}
				else if( rc == SQLITE_DONE)//�����ɹ���û������
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
				else	// SQLITE_BUSY, SQLITE_ERROR	SQLite���� æ�����ߴ���
				{
					csError.Format( _T("check DB(%s) table(%s) step error: %s\n"), strDBFile, strTableName, sqlite3_errmsg(sq3db) );
				}

				sqlite3_close(sq3db);		//SQLITE_OK	SQLITE_ERROR	SQLITE_BUSY

				if( 1 == nTable )	return 0;
				else				return -13;
			}
		}
	}
	else//��ǰ���ݿⲻ���ڣ������¿�
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


//�������ݵ��������ݿ�
int CThreadUpdate::SaveData2LocalDB3(LPCTSTR strDBFile, LPCTSTR strTBName, CUpdateData& cUpdate, CString& csError)
{
	CString csCreate = _T("");
	CString csInsert = _T("");
	CString csTemp = _T("");

	//��ȡ�������
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
		//��ȡ��ֵ���
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
					else//���½���󣬲���ɹ�
					{
						continue;	//���ڿ��ܴ����������������������
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
			continue;	//���ڿ��ܴ����������������������
		}
	}

	//�м�����޴��󷵻أ�����Ϊ�ɹ�
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
		
		if( 0 == nType )//����������ѡȡ
		{
			csSQL.Format( _T(" select * from %s limit 9; "), strTBName);//Ŀǰ���ֻ������������
		}
		else//����ѡȡ
		{
			csSQL.Format( _T(" select * from %s where (TransferMark&1)<>1 limit 9; "), strTBName);//Ŀǰ���ֻ������������
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
		
		if( rc == SQLITE_ROW)	//����һ������
		{
			do
			{
				CString csDateName = _T("");
				tagUpdateInfo	sData;
				::memset(&sData, 0, sizeof(tagUpdateInfo));

				for(i=0; i < ncols; ++i) 
				{
					//��ȡ��ԭʼ���ƣ��� sqlite3_column_origin_name�������� sqlite3_column_name ��ȡ��ʱ����
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
		else //SQLITE_DONE, SQLITE_BUSY, SQLITE_ERROR	SQLite���� �����ݡ�æ�����ߴ���
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



