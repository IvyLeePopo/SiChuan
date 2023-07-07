#include "StdAfx.h"
#include "ThreadSMManager.h"
#include "ScanMachine.h"
extern CScanMachineApp theApp;

IMPLEMENT_DYNCREATE(CThreadSMManager, CThreadBase)


CThreadSMManager::CThreadSMManager(void)
	:m_bOpen(FALSE)
	,m_bCOMLastOpenState(FALSE)
	,m_hCom(INVALID_HANDLE_VALUE)
	,m_tLastRecordCOMStateTime(0)
	,m_hSyncComData(INVALID_HANDLE_VALUE)
	,m_lpReceiveDataBuff(NULL)
	,m_dwReceiveDataIndex(0)

	,m_pSMProtocol(NULL)
	,m_dwScanStartTime(0)
	,m_dwSpanDelayTimeForControlLight(250)
	,m_bAllowCloseScanTipLight(FALSE)
	,m_iCheckHeartErrorSpanTime(30)
	,m_bSerialCommError(FALSE)

	,m_iNoNewDataTime(20)
	,m_bPrintCommandDataInLog(FALSE)
	,m_tLastComReadDataTime(0)
	,m_tLastComWriteDataTime(0)
	,m_iResetCOMErrorSpanTime(0)
	,m_bAbandonEchoData(TRUE)
	
{
	memset(&m_sCOMState,0,sizeof(tagSMInnerDeviceState));
	memset(&m_sHeartState,0,sizeof(tagSMInnerDeviceState));

	m_sHeartState.StateType = 2;

	m_strPathLocalConfigFile.Format(_T("%s\\%s"), theApp.csAppDir, FILE_NAME_DEVICE_SM_CONFIG);

	m_dwSMState = 0;
	m_dwMessageID = 0;
	::memset(&sSMResult, 0, sizeof(tagSMInnerDeviceState));
	sSMResult.ScanResult = -9;

	::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
}


CThreadSMManager::~CThreadSMManager(void)
{
}

BOOL CThreadSMManager::InitInstance()
{
	m_pSMProtocol = new CHTSMDeviceControl();

	ASSERT(m_pSMProtocol);

	m_pSMProtocol->SetParam(m_nThreadID);

	fnReadSpecailConfig();

	CString strSyncComDataHandleName;
	strSyncComDataHandleName.Format(_T("TOLL_LANE_COM_SYNC_DLL_%d"),m_nThreadID);
	m_hSyncComData = ::CreateEvent(NULL,TRUE,FALSE,strSyncComDataHandleName);

	CString strLog;
	m_pSMProtocol->InitInnerDevice(strLog);

	return TRUE;
}

int CThreadSMManager::ExitInstance()
{
	fnReleaseResource();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadSMManager, CThreadBase)

	ON_THREAD_MESSAGE(WM_THREAD_SM_START_SCAN,OnMsgStartScan)
	ON_THREAD_MESSAGE(WM_THREAD_SM_STOP_SCAN,OnMsgStopScan)
	ON_THREAD_MESSAGE(WM_THREAD_SM_CONTROL_BOARD,OnMsgControlSerialBoard)
	ON_THREAD_MESSAGE(WM_THREAD_SM_SET_LIGHT_MODE,OnMsgSetLightMode)
	ON_THREAD_MESSAGE(WM_THREAD_SM_SET_DISPLAY,OnMsgSetDisplay)
	ON_THREAD_MESSAGE(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,OnMsgDealResponseForProtocolProcessResult)

END_MESSAGE_MAP()


DWORD CThreadSMManager::GetSMState()
{
	return m_dwSMState;
}

void CThreadSMManager::SetMessageID(DWORD dwMessage)
{
	m_dwMessageID = dwMessage;
}

void CThreadSMManager::GetSMResult(tagSMInnerDeviceState& sState)
{
	::memset(&sState, 0, sizeof(tagSMInnerDeviceState));
	::memcpy(&sState, &sSMResult, sizeof(tagSMInnerDeviceState));
}

void CThreadSMManager::SetDisplay(tagScanDisplay sDisplay)
{
	::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
	::memcpy(&sScanDisplay, &sDisplay, sizeof(tagScanDisplay));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::fnReleaseResource
//     
// 功能描述：清除本类申请的资源
//     
// 输入参数：无参数
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年12月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::fnReleaseResource()
{
	//关灯
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
	::Sleep(10);
	//关闭串口
	CloseCom();

	if(NULL != m_pSMProtocol)
	{
		delete m_pSMProtocol;
		m_pSMProtocol = NULL;
	}

	if(INVALID_HANDLE_VALUE != m_hSyncComData)
		::CloseHandle(m_hSyncComData);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::fnReadSpecailConfig
//     
// 功能描述：读取扫码器基本配置信息
//     
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年12月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::fnReadSpecailConfig(void)
{
	int iSMCount = ::GetPrivateProfileInt(_T("SMControl"),_T("Count"),1,m_strPathLocalConfigFile);
	//获取检测心跳异常的间隔时间（单位：秒）
	m_iCheckHeartErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"),_T("CheckHeartErrorSpanTime"),30,m_strPathLocalConfigFile);
	if(  (m_iCheckHeartErrorSpanTime<=0) || (m_iCheckHeartErrorSpanTime>=240) )
	{
		m_iCheckHeartErrorSpanTime = 60;
	}

	//是否将收发数据都显示在日志文件当中
	m_bPrintCommandDataInLog = (BOOL)::GetPrivateProfileInt(_T("SMControl"),_T("PrintCommandDataInLog "),0,m_strPathLocalConfigFile);
	//是否丢弃重复数据(相比上一条数据而言)
	m_bAbandonEchoData       = (BOOL)::GetPrivateProfileInt(_T("SMControl"),_T("AbandonEchoData "),1,m_strPathLocalConfigFile);
	//获取串口异常后需要复位的间隔时间
	m_iResetCOMErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"),_T("ResetCOMErrorSpanTime"),20,m_strPathLocalConfigFile);
	//读取两个字符之间的时间间隔（超过这个时间值则认为数据传输完毕）
	m_iNoNewDataTime		 = ::GetPrivateProfileInt(_T("SMControl"),_T("NoNewDataTime"),50,m_strPathLocalConfigFile);
	
	CString strSectionName(_T("SMControl"));

	memset(&m_sLocalCOM,0,sizeof(tagSMInnerDeviceInit));

	strSectionName.Format(_T("SerialParam"));
	//输入缓冲区大小
	m_sLocalCOM.InBufferSize	= ::GetPrivateProfileInt(strSectionName,_T("InBufferSize"),1024,m_strPathLocalConfigFile);
	//输出缓冲区大小
	m_sLocalCOM.OutBufferSize	= ::GetPrivateProfileInt(strSectionName,_T("OutBufferSize"),1024,m_strPathLocalConfigFile);
	//串口号
	m_sLocalCOM.Port			= ::GetPrivateProfileInt(strSectionName,_T("Port"),1,m_strPathLocalConfigFile);
	if(m_sLocalCOM.Port>64)
		m_sLocalCOM.Port = 64;
	//波特率
	m_sLocalCOM.BaudRate		= ::GetPrivateProfileInt(strSectionName,_T("BaudRate"),9600,m_strPathLocalConfigFile);
	//校验位
	m_sLocalCOM.ParityBit		= ::GetPrivateProfileInt(strSectionName,_T("ParityBit"),0,m_strPathLocalConfigFile);
	//停止位
	m_sLocalCOM.StopBit			= ::GetPrivateProfileInt(strSectionName,_T("StopBit"),1,m_strPathLocalConfigFile);
	//数据位
	m_sLocalCOM.DataBit			= ::GetPrivateProfileInt(strSectionName,_T("DataBit"),8,m_strPathLocalConfigFile);

	m_sCOMState.StateType = 1;
	m_sCOMState.DeviceID  = m_sLocalCOM.Port;

	strSectionName.Format(_T("Device"));

	//扫码器设备类型
	m_pSMProtocol->m_cInnerDeviceInfo.SMType					= ::GetPrivateProfileInt(strSectionName,_T("DeviceType"),1,m_strPathLocalConfigFile);
	//有效数据长度	
	m_pSMProtocol->m_cInnerDeviceInfo.EffectiveDataLen			= ::GetPrivateProfileInt(strSectionName,_T("EffectiveDataLen"),12,m_strPathLocalConfigFile);
	//单次串口读取间隔时间，单位：毫秒
	m_pSMProtocol->m_cInnerDeviceInfo.SingleReadDataSpanTime	= ::GetPrivateProfileInt(strSectionName,_T("SingleReadDataSpanTime"),12,m_strPathLocalConfigFile);
	//预设串口等待超时,单位：毫秒
	m_pSMProtocol->m_cInnerDeviceInfo.SMRoundTimeOutValue		= ::GetPrivateProfileInt(strSectionName,_T("RoundTimeOutValue"),12,m_strPathLocalConfigFile);
	//扫码器默认读取等待超时，单位：毫秒
	m_pSMProtocol->m_cInnerDeviceInfo.SMDefaultScanTimeOutValue = ::GetPrivateProfileInt(strSectionName,_T("DefaultScanTimeOut"),12,m_strPathLocalConfigFile);
	//扫码/停止指令与亮灯之间的间隔时间
	m_dwSpanDelayTimeForControlLight							= ::GetPrivateProfileInt(strSectionName,_T("DelayTimeForControlLight"),200,m_strPathLocalConfigFile);
	//获取扫码数据后是否关闭扫码提示灯
	m_bAllowCloseScanTipLight									= (BOOL)::GetPrivateProfileInt(strSectionName,_T("AllowCloseScanTipLight"),0,m_strPathLocalConfigFile);
	
	for(int i = 0;i<iSMCount;++i)
	{
		tagSMInnerDeviceInit sSMInit = {0};

		tagSMInnerDeviceState sState={0};

		strSectionName.Format(_T("SMConfig%d"),i+1);
		
		//设备ID编码
		sSMInit.DeviceID		= ::GetPrivateProfileInt(strSectionName,_T("DeviceID"),1,m_strPathLocalConfigFile);
		//串口号
		sSMInit.COM.Port		= ::GetPrivateProfileInt(strSectionName,_T("Port"),1,m_strPathLocalConfigFile);
		//波特率
		sSMInit.COM.BaudRate	= ::GetPrivateProfileInt(strSectionName,_T("BaudRate"),9600,m_strPathLocalConfigFile);
		//校验位
		sSMInit.COM.ParityBit	= ::GetPrivateProfileInt(strSectionName,_T("ParityBit"),0,m_strPathLocalConfigFile);
		//停止位
		sSMInit.COM.StopBit		= ::GetPrivateProfileInt(strSectionName,_T("StopBit"),1,m_strPathLocalConfigFile);
		//数据位
		sSMInit.COM.DataBit		= ::GetPrivateProfileInt(strSectionName,_T("DataBit"),8,m_strPathLocalConfigFile);
	
		sState.DeviceID = sSMInit.DeviceID;

		sState.LastInitTime = time(NULL)-25;
		//增添一个内挂设备
		BOOL bExists = FALSE;
		for(INT_PTR j = 0;j<m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.GetSize();++j)
		{
			if(sState.DeviceID == m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.GetAt(j).DeviceID)
			{
				bExists = TRUE;
				break;
			}
		}

		if(!bExists)
		{
			m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.Add(sSMInit);
			
			m_pSMProtocol->m_cInnerDeviceInfo.SMStateList.Add(sState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OpenCom
//     
// 功能描述：初始化串口连接
//     
// 输入参数：无
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2015年12月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager::OpenCom()
{	
	DCB          dpb;
	COMMTIMEOUTS comtimeout;
	CString      strTemp;
	CString		 strDesc;

	memset(&comtimeout,0,sizeof(comtimeout));
	comtimeout.ReadIntervalTimeout=MAXDWORD;
	comtimeout.ReadTotalTimeoutMultiplier=0;
	comtimeout.ReadTotalTimeoutConstant=0;
	comtimeout.WriteTotalTimeoutMultiplier=50;
	comtimeout.WriteTotalTimeoutConstant=2000;

	strTemp.Format("\\\\.\\COM%d",m_sLocalCOM.Port );
	if(INVALID_HANDLE_VALUE!=(m_hCom=CreateFile(strTemp,
												GENERIC_READ|GENERIC_WRITE,
												0,
												NULL,
												OPEN_EXISTING,
												FILE_ATTRIBUTE_NORMAL,
												NULL)))
	{
		GetCommState(m_hCom, &dpb);
		dpb.BaudRate	= m_sLocalCOM.BaudRate;
		dpb.ByteSize	= 8;
		dpb.Parity		= NOPARITY;
		dpb.StopBits	= ONESTOPBIT;
		SetCommState(m_hCom,&dpb);
		SetCommTimeouts(m_hCom,&comtimeout);
		m_bOpen = TRUE;
	}
	else 
	{
		m_bOpen = FALSE;
		m_hCom  = NULL;
	}

	m_sCOMState.IsInit = m_bOpen;
	//保存日志
	strDesc.Format(_T("本地串口[端口号:%d,波特率:%d,停止位:%d,校验位:%d,数据位:%d]打开%s"),m_sLocalCOM.Port,m_sLocalCOM.BaudRate,m_sLocalCOM.StopBit,m_sLocalCOM.ParityBit,m_sLocalCOM.DataBit,m_bOpen?_T("成功"):_T("失败"));
	_tcscpy_s(m_sCOMState.StateDesc,_countof(m_sCOMState.StateDesc),strDesc.GetBuffer(0));
	strDesc.ReleaseBuffer();
	//返回串口打开标记
	return m_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::CloseCom
//     
// 功能描述：关闭串口连接
//     
// 输入参数：无参数
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2015年12月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager::CloseCom()
{
	BOOL bReturn = FALSE;
	if( INVALID_HANDLE_VALUE != m_hCom ) 
	{
		bReturn = ::CloseHandle(m_hCom);
		m_bOpen = !bReturn;
		//如果返回关闭句柄成功，则对串口句柄重置为非法值
		if(bReturn)
		{
			m_hCom = INVALID_HANDLE_VALUE;
		}

		CString strDesc;
		strDesc.Format(_T("扫码管理线程,串口[端口:%d]关闭%s"),m_sLocalCOM.Port,bReturn?_T("成功"):_T("失败"));
		theApp.RecordMsgTemp(strDesc);
	}
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::WriteDataToComm
//     
// 功能描述：往串口写数据，字节流形式
//     
// 输入参数：IN const LPBYTE byData
//         ：IN const DWORD& dwCommandDataSize
// 输出参数：DWORD
// 编写人员：ROCY
// 编写时间：2015年12月1日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CThreadSMManager::WriteDataToComm(IN const LPBYTE byData,IN const DWORD& dwCommandDataSize)
{
	DWORD dwWriteDataLen = 0;

	CString strCmd;
	CString strComErrDesc;

	if(m_bPrintCommandDataInLog)
	{
		CString strTemp;
		for(DWORD i = 0;i<dwCommandDataSize;++i)
		{
			strTemp.Format(_T("%02X "),byData[i]);
			strCmd += strTemp;
		}
	}

	if( INVALID_HANDLE_VALUE != m_hCom )
	{
		if(!(::WriteFile(m_hCom,byData,dwCommandDataSize,&dwWriteDataLen,NULL)))
		{
			//产生错误
			if( (1 == dwCommandDataSize) && (strCmd == _T("FF")) )
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,检测串口下发数据状态[故障]")));
			}
			else
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,发送数据到设备失败，错误代码:%d"),GetLastError()));
			}
		}
		else
		{
			//记录数据成功发送时间
			m_tLastComWriteDataTime = time(NULL);
			//发送正常
			if( (1 == dwCommandDataSize) && (strCmd == _T("FF")) )
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,检测串口下发数据状态[正常]")));
			}
			else
			{
				theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,发送数据到设备:%d -- %s"),dwCommandDataSize,strCmd));
			}
		}
	}
	else 
	{
		//串口句柄异常
		theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,接收到发送数据的请求，内部错误:串口句柄异常(%d -- %s)"),dwCommandDataSize,strCmd));
	}
	//返回实际发出去的字节数
	return dwWriteDataLen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::WriteDataToComm
//     
// 功能描述：物理端口操作函数 -- 往串口写数据，字符串形式
//     
// 输入参数：IN const CString& strWriteData	--	需要写到串口的数据内容
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager::WriteDataToComm(IN const CString& strWriteData)
{
	BOOL bReturn = FALSE;
	CString strRealData(strWriteData);
	strRealData.TrimRight();
	strRealData.TrimLeft();
	DWORD dwWriteDataLen = 0;
	BYTE byData[2048]={0};
	DWORD dwCommandDataSize = 0;

	if(0 == strRealData.GetLength()%2)
	{
		//转译成字节流
		theApp.Hex2Bin(strRealData.GetBuffer(0),byData,strRealData.GetLength());
		dwCommandDataSize = strRealData.GetLength()/2;

		int iLastCmdTyp = m_pSMProtocol->GetInnerDeviceProtocolClass()->GetLastCommandType();
		dwWriteDataLen = WriteDataToComm(byData,dwCommandDataSize);

		bReturn = (dwWriteDataLen == dwCommandDataSize)?TRUE:FALSE;
	}
	else
		bReturn = FALSE;
	//重置重新发送指令的状态为假
	m_pSMProtocol->m_bNeedReSendLastData = FALSE;
	//返回提示
	return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnIdle
//     
// 功能描述：扫码器线程管理类，空闲处理函数
//     
// 输入参数：LONG lCount
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2015年11月27日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager::OnIdle(LONG lCount)
{
	if(m_bExit)
	{
		//关闭串口连接
		CloseCom();
		//发送退出消息
		::PostQuitMessage(0);

		return FALSE;
	}

	//判断是否需要重新打开串口（满足2个条件 1，串口超过指定时间没成功接收到任何数据;2,串口超过指定时间没发送成功任何数据)
	if(   ((time(NULL) - m_tLastComReadDataTime)  > m_iResetCOMErrorSpanTime)
		&&((time(NULL) - m_tLastComWriteDataTime) > m_iResetCOMErrorSpanTime)
		)
	{
		static time_t tLastSendTestCharTime = 0;
		if( (time(NULL) - tLastSendTestCharTime) > 10)
		{
			//尝试发送一个字符，检测是否成功
			if(!WriteDataToComm(_T("FF")))
			{
				//是否允许执行打开串口操作
				static time_t tLastOpenCOMTime = 0;
				if( (time(NULL) - tLastOpenCOMTime) > 20)
				{
					//记录日志
					theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,串口[端口:%d]状态出现异常（限定时间内未成功[收到/发送]任何数据），需要进行重置"),m_sLocalCOM.Port));
					//关闭串口连接
					CloseCom();
					//标记上次状态
					m_bCOMLastOpenState = FALSE;
					//沉默1秒
					Sleep(1000);
					//打开串口
					m_bOpen = OpenCom();
					if(!m_bOpen)
					{
						theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,串口[端口:%d]打开失败"),m_sLocalCOM.Port));
					}
					//记录操作时间
					tLastOpenCOMTime = time(NULL);
				}
			}
			//记录发送测试字符时间
			tLastSendTestCharTime = time(NULL);
		}
	}

	//串口状态发生变化
	if(m_bCOMLastOpenState != m_bOpen)
	{
		m_bCOMLastOpenState = m_bOpen;
		//检测串口是否能打开
		m_dwSMState &= 0xFFFFFFFD;
		m_dwSMState |= m_bOpen?0x00:0x02;
		//记录日志
		theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,打开本地串口[端口:%d,波特率:%d,数据位:%d,停止位:%d,校验位:%d] %s"),m_sLocalCOM.Port,m_sLocalCOM.BaudRate,m_sLocalCOM.DataBit,m_sLocalCOM.StopBit,m_sLocalCOM.ParityBit,m_bOpen?_T("成功"):_T("失败")));
	}

	//如果串口打开成功,检测心跳信息
	if(m_bOpen)
	{
		if(NULL != m_pSMProtocol)
		{
			//指定时间没有心跳信息,认定串口通讯异常（默认30秒）
			if( (time(NULL) - m_pSMProtocol->m_tLastHeartTimeForBoard) > m_iCheckHeartErrorSpanTime)
			{
				m_bSerialCommError = TRUE;
			}
			else
			{
				m_bSerialCommError = FALSE;
			}
		}
	}
	else
	{
		m_dwSMState = 0x02;
	}

	//心跳状态，5秒检测一次
	static time_t tLastCheckSerialCommTime = 0x00;
	if( (time(NULL) - tLastCheckSerialCommTime) > 5)
	{
		m_dwSMState &= 0xFFFFFFFB;
		m_dwSMState |= m_bSerialCommError?0x04:0x00;
	}

	CString strTemp;
	CString strLog;
	//开始读取数据
	if(NULL != m_hCom)
	{
		DWORD	dwError=0;
		COMSTAT	comStat;
		//检测端口状态
		ClearCommError(m_hCom,&dwError,&comStat);
		if(comStat.cbInQue>0)//接收缓冲区中有数据
		{
			/******************************重写串口接收模块,减少ReadFile操作****************************************/
			DWORD	dwStart			= 0;				//数据帧在缓冲区的起始位置
			DWORD	dwEnd			= 0;				//数据帧在缓冲区的结尾位置+1
			DWORD	dwRWLen			= 0;				//接收数据帧长度
			DWORD	dwOrgDataLen	= 0;				//收到一个完整的数据帧(未经CRC校验，仅是长度符合)
			DWORD	dwReadNum		= 0;				//当前读取的字节数
			DWORD	dwNewTime		= 0;				//当前读取数据时间
			BYTE	byRBuff[2048]   = {0};					//接收数据缓冲区(定义2048字节)
			BOOL	bHaveFram = FALSE;

			CString strReceiveData;
			while(!bHaveFram)
			{
				//读取1个字节
				ReadFile(m_hCom,&byRBuff[dwEnd],1,&dwReadNum,NULL); 
				if(dwReadNum>0)
				{
					//更新当前读取数据时间
					dwNewTime = ::timeGetTime();
					m_tLastComReadDataTime = time(NULL);
					//获得缓冲区的末尾计数
					dwEnd += dwReadNum;
					//分析是否有合法包
					BOOL bFramStart = FALSE;
					for(register DWORD dwRecCount= 0;dwRecCount<dwEnd;++dwRecCount)
					{
						if(!bFramStart && (0xDF == byRBuff[dwRecCount]))//截获包头，记下在缓冲区的位置
						{
							bFramStart = TRUE;
							dwStart = dwRecCount++;
							continue;
						}
						if(bFramStart && 0xFD == byRBuff[dwRecCount])//截获包尾，得到完整包
						{
							dwOrgDataLen = dwRecCount - dwStart + 1;
							//获取完成数据帧,跳出循环
							bHaveFram = TRUE;
							//跳出内层循环
							break;
						}
					}
				}
				else 
				{ 
					if((::timeGetTime() - dwNewTime) > m_iNoNewDataTime)//连续指定毫秒(默认 50)没有读到数据，函数返回
					{
						//保存已收到的数据包
						if(dwEnd>=6)
						{
							//记录下接收到的数据
							for(int i = 0;i<dwEnd;++i)
							{
								strTemp.Format(_T("%02X "),byRBuff[dwStart+i]);
								strReceiveData += strTemp;
							}
							strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(不完整),具体内容:%s"),m_sLocalCOM.Port,strReceiveData);
							theApp.RecordMsgTemp(strLog);
							
							//仅仅当接收到的数据是响应A5指令回馈，才进行通知调用者处理
							if(    (CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5 == byRBuff[dwStart+5])
								&& ((SM_CMD_START == byRBuff[dwStart+6] ) || (0x57 == byRBuff[dwStart+6]))
								)
							{
								//收到大于6个字节的无效数据，判定扫码失败，通知调用者
								tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
								if(NULL != pSMRemoteState)
								{
									memset(pSMRemoteState,0,sizeof(tagSMInnerDeviceState));

									pSMRemoteState->ScanResult = 0x01;
									PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,0x00),(LPARAM)pSMRemoteState);
									strLog.Format(_T("扫码管理线程,底层处理:接收到串口无效数据（大于指定字节数），判定扫码操作失败 "));
								}
								theApp.RecordMsgTemp(strLog);
							}
						}
						//直接返回
						return TRUE;
					}
				}
				//沉默1毫秒
				Sleep(1);
			}

			//获取到完整数据帧
			for(int i = 0;i<dwOrgDataLen;++i)
			{
				strTemp.Format(_T("%02X "),byRBuff[dwStart+i]);
				strReceiveData += strTemp;
			}

			//数据帧解析以及处理
			BOOL bNotifyCallerFail = FALSE;
			BYTE byResponseType = 0x00;
			int nFrameType = 0;
			if(m_pSMProtocol->ProcessReceiveFrameData(&byRBuff[dwStart],dwOrgDataLen, nFrameType))
			{
				if(0xC0 == byRBuff[dwStart + 1])
				{
					strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%02X 帧,%s"),m_sLocalCOM.Port,byRBuff[dwStart + 1],strReceiveData);
					theApp.RecordMsgTemp(strLog);
					strLog.Empty();
				}
				else
				{
					strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%02X 帧"),m_sLocalCOM.Port,byRBuff[dwStart + 1]);
				}
				//是否打印日志
				if(m_bPrintCommandDataInLog)
				{
					if(0xC0 != byRBuff[dwStart + 1])
					{
						CString strCmdLog;
						strCmdLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%s"),m_sLocalCOM.Port,strReceiveData);
						theApp.RecordMsgTemp(strCmdLog);
						strLog.Empty();
					}
				}
			}
			else
			{
				byResponseType = 0x02;
				//对于接收到的异常数据进行处理
				if(dwOrgDataLen>6)
				{
					//仅仅处理指定帧数据的回馈
					if(   (CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5== byRBuff[dwStart + 5])
						||((SM_CMD_START == byRBuff[dwStart + 6]) || (0x57 == byRBuff[dwStart + 6]))
						)
					{
						static int iReceiveEchoCount = 0x00;
						static CString strLastRecData = _T("");
						if(strReceiveData != strLastRecData)
						{
							iReceiveEchoCount = 0;
							strLastRecData = strReceiveData;
						}
						//判断是否需要通知调用者(连续接收到3次相同的数据，通知调用者获取扫码结果失败）
						if(++iReceiveEchoCount>3)
						{
							//通知调用者，扫码失败
							tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
							if(NULL != pSMRemoteState)
							{
								memset(pSMRemoteState,0,sizeof(tagSMInnerDeviceState));

								pSMRemoteState->ScanResult = 0x01;
								PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,0x00),(LPARAM)pSMRemoteState);
							}
							strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(多次失败),设置结果[获取扫码信息失败]"),m_sLocalCOM.Port,strReceiveData);
						}
						else
						{
							//回馈C0帧
							CString strC0String = m_pSMProtocol->SendCmdResponse(byResponseType,CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5);
							BOOL bSendState = WriteDataToComm(strC0String);
							strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败),具体内容:%s，次数已经 %d 次，发送C0帧通知下位机重发该数据[%s]"),m_sLocalCOM.Port,strReceiveData,iReceiveEchoCount,bSendState?_T("成功"):_T("失败"));
						}
					}
					
					else
					{
						strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败),具体内容:%s"),m_sLocalCOM.Port,strReceiveData);
					}
				}
			}

			//记录日志
			if(!strLog.IsEmpty())
			{
				if(m_strLastErrorDesc != strLog)
				{
					m_strLastErrorDesc = strLog;
					theApp.RecordMsgTemp(strLog);
					strLog.Empty();
				}
			}

			//判断是否需要重新发送数据，则在此进行处理(仅仅当接收到回馈失败，并且回馈失败的数据指令类型为A5帧的时候，才进行该项处理）
			if(m_pSMProtocol->m_bNeedReSendLastData)
			{
				if(!m_pSMProtocol->m_strLastSendFrameData.IsEmpty())
				{
					static int iReSendLastCmdCount = 0x00;
					static CString strLastVerifyCmd = _T("");
					if(m_pSMProtocol->m_strLastSendFrameData != strLastVerifyCmd)
					{
						strLastVerifyCmd = m_pSMProtocol->m_strLastSendFrameData;
						iReSendLastCmdCount = 0;
					}

					//同一个指令只允许重发3次
					if(++iReSendLastCmdCount<4)
					{
						//往串口写数据
						if(WriteDataToComm(m_pSMProtocol->m_strLastSendFrameData))
						{
							strLog.Format(_T("扫码管理线程,底层处理:接收到下位机回馈C0帧，标识处理上位机指令失败，重新进行发送上次的数据【成功】，次数 %d "),iReSendLastCmdCount);
						}
						else
						{
							strLog.Format(_T("扫码管理线程,底层处理:接收到下位机回馈C0帧，标识处理上位机指令失败，重新进行发送上次的数据【失败】,次数 %d "),iReSendLastCmdCount);
						}
					}
					else
					{
						//重发次数已经超过3次，判定扫码失败，通知调用者
						tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
						if(NULL != pSMRemoteState)
						{
							memset(pSMRemoteState,0,sizeof(tagSMInnerDeviceState));

							pSMRemoteState->ScanResult = 0x01;
							PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,0x00),(LPARAM)pSMRemoteState);
							strLog.Format(_T("扫码管理线程,底层处理:由于硬件回馈获取指令失败，组件重发同一帧数据已经超过3次，判定扫码操作失败 "));
						}
					}
					//记录日志
					if(!strLog.IsEmpty())
					{
						if(m_strLastErrorDesc != strLog)
						{
							m_strLastErrorDesc = strLog;
							theApp.RecordMsgTemp(strLog);
						}
					}
				}
			}
		}
	}
	else
	{
		//尝试重新打开串口
		m_bOpen = OpenCom();
	}

	//线程检测状态,120秒检测一次
	static time_t tLastSMLogTime = 0x00;
	if( (time(NULL) - tLastSMLogTime) > 120)
	{
		tLastSMLogTime = time(NULL);
	}

	Sleep(5);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnMsgStartScan
//     
// 功能描述：开始扫码
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2015年12月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_SM_START_SCAN
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::OnMsgStartScan(WPARAM wParam,LPARAM lParam)
{
	WORD wDelayScanTime = LOWORD(lParam);
	//记录日志
	theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,接收到命令,开始进行扫码预处理操作")));
	//控制灯光
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SCAN);
	//延时控制
	Sleep(m_dwSpanDelayTimeForControlLight);
	//执行扫码命令
	fnSMScanCode();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnMsgStopScan
//     
// 功能描述：停止扫码
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_SM_STOP_SCAN
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::OnMsgStopScan(WPARAM wParam,LPARAM lParam)
{
	//记录日志
	theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,接收到命令,通知工作线程停止扫码")));
	//控制灯光
	fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
	//延时控制
	Sleep(m_dwSpanDelayTimeForControlLight);
	//执行停止扫码命令
	fnSMStopScan();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnMsgDealResponseForProtocolProcessResult
//     
// 功能描述：内部被动消息触发处理函数 -- 处理主板协议类格式化后的数据
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::OnMsgDealResponseForProtocolProcessResult(WPARAM wParam,LPARAM lParam)
{
	//获取执行的命令类型
	WORD wCmdType  = LOWORD(wParam);
	//获取设备ID编码
	WORD wDeviceID = HIWORD(wParam);
	//日志信息
	CString strLog;

	tagSMInnerDeviceState* pSMRemoteState = (tagSMInnerDeviceState*)lParam;

	//进行处理
	try
	{	
		if(NULL == pSMRemoteState)
			throw -1;
		//其它管理操作
		switch(wCmdType)
		{
		case SM_CMD_INIT:
		case SM_CMD_VER:
		case SM_CMD_STOP:
		case SM_CMD_RESET_FACTORY:
		case SM_CMD_RESET_PERSION:
			{
				strLog = theApp.FmtStr(_T("扫码管理线程,%s"),pSMRemoteState->StateDesc);
			}
			break;
		case SM_CMD_START:
			{
				if(0 == pSMRemoteState->ScanResult)
				{
					strLog = theApp.FmtStr(_T("扫码工作线程,操作步骤3:该次扫码完毕,扫码器[%d]获取数据:%s"),pSMRemoteState->DeviceID,pSMRemoteState->Code);
				}
				else
				{
					strLog = theApp.FmtStr(_T("扫码工作线程,操作步骤3:该次扫码完毕,未获取任何有效数据"));
				}
				pSMRemoteState->ScanConsumeTime = GetTickCount() - m_dwScanStartTime;
				_tcscpy_s(pSMRemoteState->StateDesc,_countof(pSMRemoteState->StateDesc),strLog.GetBuffer(0));strLog.ReleaseBuffer();
				//扫码结果格式化处理
				fnProcessInnerDeviceResponseForStartScan(pSMRemoteState);
			}
			break;
		}
		//删除数据
		delete pSMRemoteState;
		pSMRemoteState = NULL;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			strLog.Format(_T("扫码管理线程,该次扫码完毕,协议解析类回馈数据指针异常"));
			break;
		}
	}
	//记录日志
	theApp.RecordMsgTemp(strLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnMsgSetLightMode
//     
// 功能描述：接收调用者消息处理函数 -- 控制串口板灯亮模式
//     
// 输入参数：WPARAM wParam  -- 延迟时间（毫秒）
//         ：LPARAM lParam	-- 灯亮模式(0,扫码;1,成功;2,失败;3,全灭)
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::OnMsgSetLightMode(WPARAM wParam,LPARAM lParam)
{
	int iLightMode = (int)lParam;
	int iDelayTime = (int)wParam;
	Sleep(iDelayTime);
	if(   (iLightMode >= CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SUCCESS)
		&&(iLightMode <= CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF)
		)
	{
		fnSMControlLight(iLightMode);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnMsgControlSerialBoard
//     
// 功能描述：接受调用者消息处理函数 -- 控制串口板参数
//     
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月8日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 WM_THREAD_SM_CONTROL_BOARD
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::OnMsgControlSerialBoard(WPARAM wParam,LPARAM lParam)
{
	CString strLog;
	tagSMMainBoardParam* pCMDParam = (tagSMMainBoardParam*)lParam;
	try
	{
		if(NULL == pCMDParam)
			throw -1;
		if(NULL == m_pSMProtocol)
			throw -2;

		CString strSendData;
		switch(pCMDParam->CMDFlag)
		{
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A1:
			strSendData = m_pSMProtocol->UpdateConfigInfoFromBoard(_T(""));
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A2:
			strSendData = m_pSMProtocol->ReadConfigInfoFromBoard();
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A3:
			{
				int iLightMode = pCMDParam->ParamData[0];
				strSendData = m_pSMProtocol->SetLightMode(iLightMode);

				CString strLightMode;
				switch(iLightMode)
				{
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF:
					strLightMode.Format(_T("全熄灭"));
					break;
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SCAN:
					strLightMode.Format(_T("提示扫码"));
					break;
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SUCCESS:
					strLightMode.Format(_T("扫码通行成功"));
					break;
				case CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_FAIL:
					strLightMode.Format(_T("扫码通行失败"));
					break;
				}
				strLog.Format(_T("扫码管理线程,控制扫码提示灯，设置为:%s"),strLightMode);
				
			}
			break;
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A4:
			strSendData = m_pSMProtocol->GetLightMode();
			break;

			//新增加A5显示指令
		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5:
			strSendData = m_pSMProtocol->SetScanDisplay(sScanDisplay);
			if(strSendData.IsEmpty())
			{
				strLog.Format(_T("扫码管理线程,交互设置:Type %d 的数据暂时不允许发送"),sScanDisplay.Type);
			}
			else
			{
				strLog.Format(_T("扫码管理线程,交互设置:(Type:%d, Money:%d, Weight:%d, VType:%d, VClass:%d, Time:%s, EntryST:%s)"),
					sScanDisplay.Type, sScanDisplay.Money, sScanDisplay.Weight, sScanDisplay.VehicleType,
					sScanDisplay.VehicleClass,	CTime(sScanDisplay.tShowtime).Format("%Y-%m-%d %H:%M:%S"),sScanDisplay.EntrySTName );
			}
			break;

		case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A6:
			strSendData = m_pSMProtocol->ResetBoard();
			break;
		}

		if(!strSendData.IsEmpty())
		{
			if(!WriteDataToComm(strSendData))
				throw -3;
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
		strLog.Format(_T("扫码管理线程,控制IO失败"));
	}
	//记录日志
	if(!strLog.IsEmpty())
	{
		theApp.RecordMsgTemp(strLog);
	}

	if(NULL != pCMDParam)
	{
		delete pCMDParam;
		pCMDParam = NULL;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::fnProcessInnerDeviceResponseForStartScan
//     
// 功能描述：处理内部挂接设备的执行扫码指令后的回馈信息
//     
// 输入参数：IN tagSMInnerDeviceState* pSMState	--  扫码器状态
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState)
{
	BOOL bGetData = FALSE;
	CString strError;
	BOOL bNeedNotifyCaller = TRUE;
	try
	{
		if(NULL == pSMState)
			throw -1;

		if(OP_SUCCESS != pSMState->ScanResult)
			throw -2;

		//格式化数据
		static CString strLastCode = _T("");
		CString strCurrentCode(pSMState->Code);
		if(strCurrentCode != strLastCode)
		{
			strLastCode = strCurrentCode;
		}
		else
		{
			if(m_bAbandonEchoData)
			{
				bNeedNotifyCaller = FALSE;
				throw -3;
			}
		}

		::memset(&sSMResult, 0, sizeof(tagSMInnerDeviceState));
		::memcpy(&sSMResult, pSMState, sizeof(tagSMInnerDeviceState));
		sSMResult.ScanResult = 0;

		bGetData = TRUE;
		//记录日志
		strError.Format(_T("扫码管理线程,该次扫码完毕,已经获取数据，发送通知消息给调用者"));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://接收到的数据为空
		case -2://接收到的数据非法
			strError.Format(_T("扫码管理线程,该次扫码完毕，未能获取数据,发送通知消息给调用者"));
			break;
		case -3://当前接收到重复数据
			strError.Format(_T("扫码管理线程,该次扫码完毕，接收到重复的数据，内部丢弃处理，不推送消息给调用者"));
			break;
		}
	}
	
	if(bNeedNotifyCaller)
	{
		//通知调用者
		if(::IsWindow(m_hMainWnd))
		{
			::PostMessage(m_hMainWnd, m_dwMessageID, (WPARAM)bGetData,(LPARAM)0);
		}
		else if(0 != m_nMainThreadID)
		{
			::PostThreadMessage(m_nMainThreadID, m_dwMessageID, (WPARAM)bGetData,(LPARAM)0);
		}
	}
	//记录日志
	theApp.RecordMsgTemp(strError);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::fnSMScanCode
//     
// 功能描述：内部流程操作函数 -- 执行扫码流程操作
//     
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::fnSMScanCode(void)
{
	//开始计时
	GetLocalTime(&m_stBeginTime);
	m_dwScanStartTime = GetTickCount();

	//变量声明
	CString strSendData;
	CString strInnerDeviceCommand;
	DWORD	dwCommandDataSize = 0;
	//标记工作状态
	m_bWorking = TRUE;

	CString strError;
	//开始执行扫描操作
	try
	{
		if(!m_bOpen || (NULL == m_pSMProtocol))
			throw -1;

		//获取内挂设备的扫码命令内容
		strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"),SM_CMD_START,m_pSMProtocol->GetInnerDeviceProtocolClass()->Start());
		//获取格式化发送数据
		strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
		if(strSendData.IsEmpty())
			throw -2;

		//往串口写数据
		if(!WriteDataToComm(strSendData))
			throw -3;

		//记录日志
		strError.Format(_T("操作步骤%d:扫码命令已发送，正在扫描数据，请稍后"),0x01);
		theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,%s"),strError));	

	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://串口未打开或解析类不存在
			strError.Format(_T("扫码失败,内部异常:串口未打开或解析类不存在"));
			break;
		case -2://无法获取有效的扫码命令
			strError.Format(_T("扫码失败,内部异常:无法获取有效的扫码命令"));
			break;
		case -3://往串口写扫码命令数据失败
			strError.Format(_T("扫码失败,内部异常:往串口写扫码命令数据失败"));
			break;
		}
		
		theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,%s"),strError));	
	}
	//标记工作状态
	m_bWorking = FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::fnSMStopScan
//     
// 功能描述：内部流程操作函数 -- 执行停止流程操作
//     
// 输入参数：void
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::fnSMStopScan(void)
{
	CString strSendData;
	CString strInnerDeviceCommand;
	DWORD	dwCommandDataSize = 0;
	BYTE	byCmdData[1024]={0};
	try
	{
		if(!m_bOpen || (NULL == m_pSMProtocol))
			throw -1;
		CHTSMDeviceBaseIF* pSMIF = m_pSMProtocol->GetInnerDeviceProtocolClass();
		if(NULL == pSMIF)
			throw -2;
		//获取停止扫描命令
		strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"),SM_CMD_STOP,pSMIF->Stop());
		if(strInnerDeviceCommand.IsEmpty())
			throw -2;
		//获取格式化发送数据
		strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
		if(strSendData.IsEmpty())
			throw -3;
		//往串口写数据
		if(!WriteDataToComm(strSendData))
			throw -4;
		//写日志
		theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,发送强制停止扫码指令[成功]")));
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		case -4:
			break;
		}
		//写日志
		theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码管理线程,发送强制停止扫码指令[失败]")));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::fnSMControlLight
//     
// 功能描述：内部流程控制函数 -- 控制灯光
//     
// 输入参数：IN const int& iLightType
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::fnSMControlLight(IN const int& iLightType)
{
	tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
	if(NULL != pParam)
	{
		memset(pParam,0,sizeof(tagSMMainBoardParam));

		pParam->Type		= 1;
		pParam->CMDFlag		= CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A3;
		pParam->ParamData[0]= iLightType;
		pParam->ParamSize	= 0x01;

		OnMsgControlSerialBoard((WPARAM)0,(LPARAM)pParam);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager::OnMsgSetDisplay
//     
// 功能描述：接收调用者消息处理函数 -- 设置显示内容
//     
// 输入参数：WPARAM wParam  -- 延迟时间（毫秒）
//         ：LPARAM lParam	-- 灯亮模式(0,扫码;1,成功;2,失败;3,全灭)
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager::OnMsgSetDisplay(WPARAM wParam,LPARAM lParam)
{
	int iSerial = (int)wParam;//获取消息序列号
	if( iSerial == sScanDisplay.iSerial )//抛送序列号等于当前序列号，执行显示操作
	{
		tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
		if(NULL != pParam)
		{
			memset(pParam,0,sizeof(tagSMMainBoardParam));

			pParam->Type		= 1;
			pParam->CMDFlag		= CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5;
			pParam->ParamData[0]= 0;
			pParam->ParamSize	= 0x01;

			OnMsgControlSerialBoard((WPARAM)0,(LPARAM)pParam);
		}
	}
}
