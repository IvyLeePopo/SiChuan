/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 *
 * 文件名称：ThreadSMManager.cpp
 * 摘    要：扫码器管理线程类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月27日
 */
// ThreadSMManager.cpp : implementation file
//

#include "stdafx.h"
#include "ThreadSMManager2.h"
#include "ScanMachine.h"
extern CScanMachineApp theApp;

#define LockMedia()         ::WaitForSingleObject(hGGMedia, INFINITE)
#define FreeMedia()         ::SetEvent(hGGMedia)

// CThreadSMManager2

IMPLEMENT_DYNCREATE(CThreadSMManager2, CThreadBase)

CThreadSMManager2::CThreadSMManager2()
    : m_bOpen(FALSE)
    , m_bCOMLastOpenState(FALSE)
    , m_pCOM(NULL)
    , m_tLastRecordCOMStateTime(0)
    , m_hSyncComData(INVALID_HANDLE_VALUE)
    , m_lpReceiveDataBuff(NULL)
    , m_dwReceiveDataIndex(0)

    , m_pSMProtocol(NULL)
    , m_dwScanStartTime(0)
    , m_dwSpanDelayTimeForControlLight(250)
    , m_bAllowCloseScanTipLight(FALSE)
    , m_iCheckHeartErrorSpanTime(30)
    , m_bSerialCommError(FALSE)

    , m_tNoDataFromCOMLastTime(0)

    , m_iNoNewDataTime(20)
    , m_bPrintCommandDataInLog(FALSE)
    , m_tLastComReadDataTime(0)
    , m_tLastComWriteDataTime(0)
    , m_iResetCOMErrorSpanTime(0)
    , m_bAbandonEchoData(TRUE)
{

    ::InitializeCriticalSection(&m_ComRepondSyncMutex);

    memset(&m_sCOMState, 0, sizeof(tagSMInnerDeviceState));
    memset(&m_sHeartState, 0, sizeof(tagSMInnerDeviceState));

    m_sHeartState.StateType = 2;

    m_strPathLocalConfigFile.Format(_T("%s\\%s"), theApp.csAppDir, FILE_NAME_DEVICE_SM_CONFIG);

    m_dwSMState = 0;
    m_dwMessageID = 0;
    ::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
//  m_sSMResult.ScanResult = -9;
    m_sSMResult.ScanResult = 0;

    ::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));

    m_nSendCmdCount = 0;
    m_dwLastChecktick = 0;
    m_dwAnswerCheckSpan = 60000;

    hGGMedia = NULL;
    aryMedia.RemoveAll();
}

CThreadSMManager2::~CThreadSMManager2()
{
    ::DeleteCriticalSection(&m_ComRepondSyncMutex);

}

BOOL CThreadSMManager2::InitInstance()
{
    m_pSMProtocol = new CHTSMDeviceControl();

    ASSERT(m_pSMProtocol);

    m_pSMProtocol->SetParam(m_nThreadID);

    fnReadSpecailConfig();

    m_lpReceiveDataBuff = new BYTE[m_sLocalCOM.InBufferSize];

    ASSERT(m_lpReceiveDataBuff);

    memset(m_lpReceiveDataBuff, 0, m_sLocalCOM.InBufferSize);

    m_pCOM = new CSerialCom();
    if (NULL != m_pCOM)
    {
        m_pCOM->SetMain(m_nThreadID);
        m_pCOM->SetNotifyNum(1);

        ::Sleep(50);
        m_bOpen = OpenCom();
        if (!m_bOpen)
        {
            theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,串口[端口:%d]打开失败"), m_sLocalCOM.Port));
        }
    }

    CString strSyncComDataHandleName;
    strSyncComDataHandleName.Format(_T("TOLL_LANE_COM_SYNC_DLL_%d"), m_nThreadID);
    m_hSyncComData = ::CreateEvent(NULL, TRUE, FALSE, strSyncComDataHandleName);

    CString strLog;
    m_pSMProtocol->InitInnerDevice(strLog);

    m_tLastComReadDataTime = time(NULL);

    hGGMedia    = CreateEvent(NULL, FALSE, TRUE,    NULL);  //默认属性、自动重置、初始化
    if ( NULL == hGGMedia )
    {
        theApp.RecordMsgTemp( _T("创建广告媒体数据队列操作同步变量失败！") );
    }

    return TRUE;
}

int CThreadSMManager2::ExitInstance()
{
    fnReleaseResource();

    if ( NULL != hGGMedia )     ::CloseHandle(hGGMedia);

    return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CThreadSMManager2, CThreadBase)

    ON_THREAD_MESSAGE(WM_THREAD_SM_START_SCAN, OnMsgStartScan)
    ON_THREAD_MESSAGE(WM_THREAD_SM_STOP_SCAN, OnMsgStopScan)
    ON_THREAD_MESSAGE(WM_THREAD_SM_CONTROL_BOARD, OnMsgControlSerialBoard)
    ON_THREAD_MESSAGE(WM_THREAD_SM_SET_LIGHT_MODE, OnMsgSetLightMode)
    ON_THREAD_MESSAGE(WM_THREAD_SM_SET_DISPLAY, OnMsgSetDisplay)
    ON_THREAD_MESSAGE(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO, OnMsgDealResponseForProtocolProcessResult)
    ON_THREAD_MESSAGE(WM_THREAD_SM_SET_TRANSMISSION, OnMsgTransmission)
    ON_THREAD_MESSAGE(WM_THREAD_SM_TRANSMISSION_RESPONSE, OnMsgTransmissionResponse)

    ON_THREAD_MESSAGE(WM_THREAD_SM_TTS, OnMsgTTS)
    ON_THREAD_MESSAGE(WM_THREAD_REMOTE_UPGRADE_RESULT, OnMsgRemoteControlResult)


    ON_THREAD_MESSAGE(ON_COM_RECEIVE, OnMsgReceiveDataFromCOM)
    ON_THREAD_MESSAGE(ON_COM_TXEMPTY, OnMsgDataSendEnd)
    ON_THREAD_MESSAGE(ON_COM_ERROR, OnMsgComError)

END_MESSAGE_MAP()


DWORD CThreadSMManager2::GetSMState()
{
    return m_dwSMState;
}

void CThreadSMManager2::SetMessageID(DWORD dwMessage)
{
    m_dwMessageID = dwMessage;
}

void CThreadSMManager2::GetSMResult(tagSMInnerDeviceState& sState)
{
    ::memset(&sState, 0, sizeof(tagSMInnerDeviceState));
    ::memcpy(&sState, &m_sSMResult, sizeof(tagSMInnerDeviceState));
    //获取到数据以后，清除原来的数据
    ::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
}
void CThreadSMManager2::GetDeviceResponse(tagDeviceFeedback& feedback)
{
    ::memset(&feedback, 0, sizeof(tagTransmissionFeedback));
    tagSMInnerDeviceState tmp_cmp;
    ::memset(&tmp_cmp, 0, sizeof(tagSMInnerDeviceState));
    if ( memcmp(&tmp_cmp, &m_sSMResult, sizeof(tagSMInnerDeviceState)) != 0)
    {
        feedback.feed_type = 1; /*qr code*/
        ::memcpy(&feedback.inner_dev, &m_sSMResult, sizeof(tagSMInnerDeviceState));
    }
    else
    {
        feedback.feed_type = 2;
        ::memcpy(&feedback.trans_feedback, &mTransResponse, sizeof(tagTransmissionFeedback));
    }
    ::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
    ::memset(&mTransResponse, 0, sizeof(tagTransmissionFeedback));
}

void CThreadSMManager2::SetDisplay(tagScanDisplay sDisplay)
{
    ::memset(&sScanDisplay, 0, sizeof(tagScanDisplay));
    ::memcpy(&sScanDisplay, &sDisplay, sizeof(tagScanDisplay));
}

void CThreadSMManager2::SetTransData(const tagTransmissionData& trans_data)
{
    ::memset(&mTransData, 0, sizeof(tagTransmissionData));
    ::memcpy(&mTransData, &trans_data, sizeof(tagTransmissionData));
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnReleaseResource
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
void CThreadSMManager2::fnReleaseResource()
{
    aryMedia.RemoveAll();

    //关灯
    fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
    ::Sleep(10);
    //关闭串口
    CloseCom();

    if (NULL != m_pCOM)
    {
        delete m_pCOM;
        m_pCOM = NULL;
    }

    if (NULL != m_lpReceiveDataBuff)
    {
        delete[] m_lpReceiveDataBuff;
        m_lpReceiveDataBuff = NULL;
    }

    if (NULL != m_pSMProtocol)
    {
        delete m_pSMProtocol;
        m_pSMProtocol = NULL;
    }

    if (INVALID_HANDLE_VALUE != m_hSyncComData)
        ::CloseHandle(m_hSyncComData);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnReadSpecailConfig
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
void CThreadSMManager2::fnReadSpecailConfig(void)
{
    int iSMCount = ::GetPrivateProfileInt(_T("SMControl"), _T("Count"), 1, m_strPathLocalConfigFile);
    //获取检测心跳异常的间隔时间（单位：秒）
    m_iCheckHeartErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"), _T("CheckHeartErrorSpanTime"), 30, m_strPathLocalConfigFile);
    if (  (m_iCheckHeartErrorSpanTime <= 0) || (m_iCheckHeartErrorSpanTime >= 240) )
    {
        m_iCheckHeartErrorSpanTime = 60;
    }

    //是否将收发数据都显示在日志文件当中
    m_bPrintCommandDataInLog = (BOOL)::GetPrivateProfileInt(_T("SMControl"), _T("PrintCommandDataInLog "), 0, m_strPathLocalConfigFile);;
    //是否丢弃重复数据(相比上一条数据而言)
    m_bAbandonEchoData       = (BOOL)::GetPrivateProfileInt(_T("SMControl"), _T("AbandonEchoData "), 1, m_strPathLocalConfigFile);
    //获取串口异常后需要复位的间隔时间
    m_iResetCOMErrorSpanTime = ::GetPrivateProfileInt(_T("SMControl"), _T("ResetCOMErrorSpanTime"), 20, m_strPathLocalConfigFile);
    //读取两个字符之间的时间间隔（超过这个时间值则认为数据传输完毕）
    m_iNoNewDataTime         = ::GetPrivateProfileInt(_T("SMControl"), _T("NoNewDataTime"), 20, m_strPathLocalConfigFile);

    // 1017版本增加应答检测时间间隔设置
    m_dwAnswerCheckSpan = ::GetPrivateProfileInt(_T("SMControl"), _T("NoAnswerTick"), 60000, m_strPathLocalConfigFile);

    CString strSectionName(_T("SMControl"));

    memset(&m_sLocalCOM, 0, sizeof(tagSMInnerDeviceInit));

    strSectionName.Format(_T("SerialParam"));
    //输入缓冲区大小
    m_sLocalCOM.InBufferSize    = ::GetPrivateProfileInt(strSectionName, _T("InBufferSize"), 2048, m_strPathLocalConfigFile);
    //输出缓冲区大小
    m_sLocalCOM.OutBufferSize   = ::GetPrivateProfileInt(strSectionName, _T("OutBufferSize"), 2048, m_strPathLocalConfigFile);
    //串口号
    m_sLocalCOM.Port            = ::GetPrivateProfileInt(strSectionName, _T("Port"), 1, m_strPathLocalConfigFile);
    if (m_sLocalCOM.Port > 64)
        m_sLocalCOM.Port = 64;
    //波特率
    m_sLocalCOM.BaudRate        = ::GetPrivateProfileInt(strSectionName, _T("BaudRate"), 9600, m_strPathLocalConfigFile);
    //校验位
    m_sLocalCOM.ParityBit       = ::GetPrivateProfileInt(strSectionName, _T("ParityBit"), 0, m_strPathLocalConfigFile);
    //停止位
    m_sLocalCOM.StopBit         = ::GetPrivateProfileInt(strSectionName, _T("StopBit"), 1, m_strPathLocalConfigFile);
    //数据位
    m_sLocalCOM.DataBit         = ::GetPrivateProfileInt(strSectionName, _T("DataBit"), 8, m_strPathLocalConfigFile);

    m_sCOMState.StateType = 1;
    m_sCOMState.DeviceID  = m_sLocalCOM.Port;

    strSectionName.Format(_T("Device"));

    //扫码器设备类型
    m_pSMProtocol->m_cInnerDeviceInfo.SMType                    = ::GetPrivateProfileInt(strSectionName, _T("DeviceType"), 1, m_strPathLocalConfigFile);
    //有效数据长度
    m_pSMProtocol->m_cInnerDeviceInfo.EffectiveDataLen          = ::GetPrivateProfileInt(strSectionName, _T("EffectiveDataLen"), 12, m_strPathLocalConfigFile);
    //单次串口读取间隔时间，单位：毫秒
    m_pSMProtocol->m_cInnerDeviceInfo.SingleReadDataSpanTime    = ::GetPrivateProfileInt(strSectionName, _T("SingleReadDataSpanTime"), 12, m_strPathLocalConfigFile);
    //预设串口等待超时,单位：毫秒
    m_pSMProtocol->m_cInnerDeviceInfo.SMRoundTimeOutValue       = ::GetPrivateProfileInt(strSectionName, _T("RoundTimeOutValue"), 12, m_strPathLocalConfigFile);
    //扫码器默认读取等待超时，单位：毫秒
    m_pSMProtocol->m_cInnerDeviceInfo.SMDefaultScanTimeOutValue = ::GetPrivateProfileInt(strSectionName, _T("DefaultScanTimeOut"), 12, m_strPathLocalConfigFile);
    //扫码/停止指令与亮灯之间的间隔时间
    m_dwSpanDelayTimeForControlLight                            = ::GetPrivateProfileInt(strSectionName, _T("DelayTimeForControlLight"), 200, m_strPathLocalConfigFile);
    //获取扫码数据后是否关闭扫码提示灯
    m_bAllowCloseScanTipLight                                   = (BOOL)::GetPrivateProfileInt(strSectionName, _T("AllowCloseScanTipLight"), 0, m_strPathLocalConfigFile);

    for (int i = 0; i < iSMCount; ++i)
    {
        tagSMInnerDeviceInit sSMInit = {0};

        tagSMInnerDeviceState sState = {0};

        strSectionName.Format(_T("SMConfig%d"), i + 1);

        //设备ID编码
        sSMInit.DeviceID        = ::GetPrivateProfileInt(strSectionName, _T("DeviceID"), 1, m_strPathLocalConfigFile);
        //串口号
        sSMInit.COM.Port        = ::GetPrivateProfileInt(strSectionName, _T("Port"), 1, m_strPathLocalConfigFile);
        //波特率
        sSMInit.COM.BaudRate    = ::GetPrivateProfileInt(strSectionName, _T("BaudRate"), 9600, m_strPathLocalConfigFile);
        //校验位
        sSMInit.COM.ParityBit   = ::GetPrivateProfileInt(strSectionName, _T("ParityBit"), 0, m_strPathLocalConfigFile);
        //停止位
        sSMInit.COM.StopBit     = ::GetPrivateProfileInt(strSectionName, _T("StopBit"), 1, m_strPathLocalConfigFile);
        //数据位
        sSMInit.COM.DataBit     = ::GetPrivateProfileInt(strSectionName, _T("DataBit"), 8, m_strPathLocalConfigFile);

        sState.DeviceID = sSMInit.DeviceID;

        sState.LastInitTime = time(NULL) - 25;
        //增添一个内挂设备
        BOOL bExists = FALSE;
        for (INT_PTR j = 0; j < m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.GetSize(); ++j)
        {
            if (sState.DeviceID == m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.GetAt(j).DeviceID)
            {
                bExists = TRUE;
                break;
            }
        }

        if (!bExists)
        {
            m_pSMProtocol->m_cInnerDeviceInfo.SMInitList.Add(sSMInit);

            m_pSMProtocol->m_cInnerDeviceInfo.SMStateList.Add(sState);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OpenCom
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
BOOL CThreadSMManager2::OpenCom()
{
    if (NULL != m_pCOM)
    {
        m_pCOM->SetNotifyNum(1);
        m_bOpen = m_pCOM->Open(m_sLocalCOM.Port, m_sLocalCOM.BaudRate);

        CString strDesc = theApp.FmtStr(_T("本地串口[端口号:%d,波特率:%d,停止位:%d,校验位:%d,数据位:%d]打开%s"), m_sLocalCOM.Port, m_sLocalCOM.BaudRate, m_sLocalCOM.StopBit, m_sLocalCOM.ParityBit, m_sLocalCOM.DataBit, m_bOpen ? _T("成功") : _T("失败"));
        _tcscpy_s(m_sCOMState.StateDesc, _countof(m_sCOMState.StateDesc), strDesc.GetBuffer(0));
        strDesc.ReleaseBuffer();
        m_sCOMState.IsInit = m_bOpen;
        //保存日志
        //theApp.RecordMsgTemp(strDesc);
    }
    return m_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::CloseCom
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
BOOL CThreadSMManager2::CloseCom()
{
    BOOL bReturn = FALSE;
    if (NULL != m_pCOM)
    {
        m_pCOM->Close();
        m_bOpen = FALSE;
        bReturn = TRUE;
    }
    return bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::WriteDataToComm
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
DWORD CThreadSMManager2::WriteDataToComm(IN const LPBYTE byData, IN const DWORD& dwCommandDataSize)
{
    DWORD dwWriteDataLen = 0;
    if (NULL != m_pCOM)
    {
        dwWriteDataLen = m_pCOM->Write(byData, dwCommandDataSize);
        if (m_bPrintCommandDataInLog)
        {
            CString strTemp;
            CString strCmd;
            for (DWORD i = 0; i < dwCommandDataSize; ++i)
            {
                strTemp.Format(_T("%02X "), byData[i]);
                strCmd += strTemp;
            }
            theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送数据到设备:%d -- %s"), dwCommandDataSize, strCmd));
        }
    }
    return dwCommandDataSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::WriteDataToComm
//
// 功能描述：物理端口操作函数 -- 往串口写数据，字符串形式
//
// 输入参数：IN const CString& strWriteData    --  需要写到串口的数据内容
// 输出参数：BOOL
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CThreadSMManager2::WriteDataToComm(IN const CString& strWriteData)
{
    BOOL bReturn = FALSE;
    CString strRealData(strWriteData);
    strRealData.TrimRight();
    strRealData.TrimLeft();
    DWORD dwWriteDataLen = 0;
    BYTE byData[5120] = {0};
    DWORD dwCommandDataSize = 0;

    if (0 == strRealData.GetLength() % 2)
    {
        //转译成字节流
        theApp.Hex2Bin(strRealData.GetBuffer(0), byData, strRealData.GetLength());
        dwCommandDataSize = strRealData.GetLength() / 2;

        dwWriteDataLen = WriteDataToComm(byData, dwCommandDataSize);

        bReturn = (dwWriteDataLen == dwCommandDataSize) ? TRUE : FALSE;
    }
    else
        bReturn = FALSE;
    //重置重新发送指令的状态为假
    m_pSMProtocol->m_bNeedReSendLastData = FALSE;

    // 1017版本记录发送指令数目和时间
    if ( 0 == m_nSendCmdCount )
    {
        m_dwLastChecktick = ::GetTickCount(); //如果当前没有已发送指令，记录发送时间，否则等待第一个应答
    }
    m_nSendCmdCount += 1;//发送指令数目加1

    //返回提示
    return bReturn;
}
// CThreadSMManager2 message handlers

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnIdle
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
BOOL CThreadSMManager2::OnIdle(LONG lCount)
{
    if (m_bExit)
    {
        //关闭串口连接
        CloseCom();
        //发送退出消息
        ::PostQuitMessage(0);

        return FALSE;
    }

    static time_t tLastOpenCOMTime = 0;
    //判断是否需要重新打开串口（满足2个条件 1，串口超过指定时间没成功接收到任何数据）
    if (   ((time(NULL) - m_tLastComReadDataTime)  > m_iResetCOMErrorSpanTime)
//      &&((time(NULL) - m_tLastComWriteDataTime) > m_iResetCOMErrorSpanTime)
       )
    {
        //是否允许执行打开串口操作
        if ( (time(NULL) - tLastOpenCOMTime) > 60) // 1017版本调整判断间隔为 60秒
        {
            //记录日志
            theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,串口[端口:%d]状态出现异常（限定时间内未成功[收到/发送]任何数据），需要进行重置"), m_sLocalCOM.Port));
            //关闭串口连接
            CloseCom();
            //标记上次状态
            m_bCOMLastOpenState = FALSE;
            //沉默1秒
            Sleep(1000);
            //打开串口
            m_bOpen = OpenCom();
            if (!m_bOpen)
            {
                theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,串口[端口:%d]打开失败"), m_sLocalCOM.Port));
            }
            //记录操作时间
            tLastOpenCOMTime = time(NULL);

            //由于串口已经重置，对于应答检测队列，也应重置检测状态
            m_nSendCmdCount = 0;
            m_dwLastChecktick = ::GetTickCount();
            return TRUE;
        }
    }


    // 1017版本增加对串口成功收发的超时判断
    if ( m_nSendCmdCount > 0 ) //只判断发送指令数目大于0的情况
    {
        DWORD dwCurrent = ::GetTickCount();
        if ( dwCurrent > m_dwLastChecktick )
        {
            if ( dwCurrent - m_dwLastChecktick > m_dwAnswerCheckSpan ) //超过检查时间都没收到应答数据帧，重置串口（默认两分钟）
            {
                theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,串口[%d]异常（限定时间内未成功收到应答数据），重置"), m_sLocalCOM.Port));

                CloseCom();                     //关闭串口连接
                m_bCOMLastOpenState = FALSE;    //标记上次状态
                ::Sleep(500);
                m_bOpen = OpenCom();            //打开串口
                if (!m_bOpen)
                {
                    theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,串口[端口:%d]打开失败"), m_sLocalCOM.Port));
                }
                //记录操作时间
                tLastOpenCOMTime = time(NULL);

                //重置发送队列检测状态
                m_nSendCmdCount = 0;
                m_dwLastChecktick = ::GetTickCount();
                return TRUE;
            }
        }
        else
        {
            m_dwLastChecktick = dwCurrent;
        }
    }

    //串口状态发生变化
    if (m_bCOMLastOpenState != m_bOpen)
    {
        m_bCOMLastOpenState = m_bOpen;
        //检测串口是否能打开
        m_dwSMState &= 0xFFFFFFFD;
        m_dwSMState |= m_bOpen ? 0x00 : 0x02;

        //记录日志
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,打开本地串口[端口:%d,波特率:%d,数据位:%d,停止位:%d,校验位:%d] %s"), m_sLocalCOM.Port, m_sLocalCOM.BaudRate, m_sLocalCOM.DataBit, m_sLocalCOM.StopBit, m_sLocalCOM.ParityBit, m_bOpen ? _T("成功") : _T("失败")));
    }

    //如果串口打开成功,检测心跳信息
    if (m_bOpen)
    {
        if (NULL != m_pSMProtocol)
        {
            //指定时间没有心跳信息,认定串口通讯异常（默认30秒）
            if ( (time(NULL) - m_pSMProtocol->m_tLastHeartTimeForBoard) > m_iCheckHeartErrorSpanTime)
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
    if ( (time(NULL) - tLastCheckSerialCommTime) > 5)
    {
        m_dwSMState &= 0xFFFFFFFB;
        m_dwSMState |= m_bSerialCommError ? 0x04 : 0x00;
    }

    //线程检测状态,120秒检测一次
    static time_t tLastSMLogTime = 0x00;
    if ( (time(NULL) - tLastSMLogTime) > 120)
    {
        //  theApp.RecordMsgTemp(FmtStr(_T("扫码工作线程,线程存活检测日志记录，当前任务编码 %d"),::theApp.m_nTask));
        tLastSMLogTime = time(NULL);
    }

    //版本 1018新增广告播放处理
    GGDeal();

    FirmwareUpgradeProcess();


    ::Sleep(100);
    return TRUE;
}






void CThreadSMManager2::GGDeal()
{
    //1020版本 增加发送间隔处理
    static DWORD dwGGCheck = 0;

    DWORD   dwCurrent = ::GetTickCount();
    if ( dwCurrent >= dwGGCheck ) //正常计时，未到49.7天的溢出周期
    {
        if ( dwCurrent - dwGGCheck >= 1000 ) //默认 1000 ms 检测一次
        {
            ////////////////////////广告下发处理//////////////////////////////////////
            if ( m_bOpen ) //1019版本增加对串口状态的判断
            {
                if ( aryMedia.GetSize() > 0 )
                {
                    CString     strLog = _T("");
                    CString     strSend = _T("");
                    tagMMIMedia     sSend;
                    ::memset(&sSend, 0, sizeof(tagMMIMedia));

                    LockMedia();

                    try
                    {
                        if (NULL != m_pSMProtocol)
                        {
                            sSend = aryMedia.GetAt( 0 );
                            strSend = m_pSMProtocol->ConstructFrameA1( sSend );

                            if ( WriteDataToComm(strSend) ) //发送命令给设备
                            {
                                aryMedia.RemoveAt( 0 );//成功后删除当前媒体指令结构
                            }
                            else
                            {
                                m_bOpen = FALSE;        //版本1020增加处理：发送指令异常，认为串口异常，等待重置

                                aryMedia[0].Spare += 1; //媒体指令发送失败次数累加，超过5次删除此媒体指令
                                if ( aryMedia[0].Spare > 5 )
                                {
                                    strLog.Format( _T("媒体指令(Type:%d, Context:%s)发送异常5次，已删除！"),
                                                   aryMedia[0].Type, aryMedia[0].Content );
                                    theApp.RecordMsgTemp( strLog );
                                    aryMedia.RemoveAt( 0 );
                                }
                            }
                        }
                        else
                        {
                            strLog.Format( _T("协议解析类指针为空，退出广告发送") );
                            theApp.RecordMsgTemp( strLog );
                            aryMedia.RemoveAll();
                        }
                    }
                    catch (...)
                    {
                        strLog.Format( _T("广告发送异常，稍后重试！") );
                        theApp.RecordMsgTemp( strLog );
                    }

                    FreeMedia();
                }
            }

            ////////////////////////广告下发处理结束//////////////////////////////////////
            dwGGCheck = ::GetTickCount();
        }
    }
    else//当前获取的时间片小于上一次，已到溢出周期
    {
        dwGGCheck = dwCurrent;  //重置上次时间片，等待下次循环处理
    }
}




////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgStartScan
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
void CThreadSMManager2::OnMsgStartScan(WPARAM wParam, LPARAM lParam)
{
    WORD wDelayScanTime = LOWORD(lParam);
    //记录日志
    theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,接收到命令,开始进行扫码预处理操作")));

    //控制灯光， 版本1014去掉启动扫码灯光控制
//  fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SCAN);
    //延时控制
//  Sleep(m_dwSpanDelayTimeForControlLight);

    //执行扫码命令
    fnSMScanCode();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgStopScan
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
void CThreadSMManager2::OnMsgStopScan(WPARAM wParam, LPARAM lParam)
{
    int iChangeMMI = (int)lParam;
    //记录日志
    theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,接收到命令,停止扫码")));
    //默认需要控制灯光
    if (0 == iChangeMMI)
    {
        //控制灯光
        fnSMControlLight(CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF);
        //延时控制
        Sleep(m_dwSpanDelayTimeForControlLight);
    }
    //执行停止扫码命令
    fnSMStopScan();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgDataSendEnd
//
// 功能描述：通知缓冲区的数据已经发送完毕
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgDataSendEnd(WPARAM wParam, LPARAM lParam)
{
    m_tLastComWriteDataTime = time(NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgComError
//
// 功能描述：通知调用者串口发生错误
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年9月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：响应消息 ON_COM_ERROR
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgComError(WPARAM wParam, LPARAM lParam)
{
}



void CThreadSMManager2::HandleComDataEx(WPARAM wParam, LPARAM lParam)
{
    if (NULL == m_lpReceiveDataBuff)
        return;

    BOOL bAnalyseState = FALSE;
    BYTE byPort = (BYTE)wParam;
    CString strLog;
    CString strReceiveData;

    if (NULL != m_pCOM)
    {
        DWORD dwReadBytes = 0;
        DWORD dwSize = m_sLocalCOM.InBufferSize - m_dwReceiveDataIndex;
        if ((dwReadBytes = m_pCOM->Read(m_lpReceiveDataBuff + m_dwReceiveDataIndex, dwSize)) > 0)
        {
            //获得读取数据的最新时间
            m_tLastComReadDataTime = time(NULL);
            //进制转换处理
            CString strTemp(theApp.Bin2Hex(m_lpReceiveDataBuff + m_dwReceiveDataIndex, dwReadBytes));

            //特殊兼容，只为了读取终端序列号
            if (0 == strTemp.CompareNoCase(_T("DFB0000AA1053030303031363744077AFDDFC00003A10500ED43FD")))
            {
                strLog.Format(_T("扫码工作线程,底层处理:接收到特定数据(终端序号):%s"), strTemp);
                theApp.RecordMsgTemp(strLog);
                strLog.Empty();

                strTemp = strTemp.Left(34);
                dwReadBytes = 17;
            }

            for (int i = 0; i < strTemp.GetLength(); i = i + 2)
            {
                strReceiveData += (strTemp.Mid(i, 2) + _T(" "));
            }
            //strLog.Format(_T("扫码工作线程,底层处理:接收到串口%d数据(%d字节),具体数据:%s"), byPort, dwReadBytes, strReceiveData);
            //theApp.RecordMsgTemp(strLog);

            m_dwReceiveDataIndex += dwReadBytes;
            // 若缓冲区索引超出缓冲区大小,强制将缓冲区索引设置成缓冲区大小,
            if ( dwReadBytes > dwSize )
            {
                dwReadBytes = dwSize;
                return;
            }
            //要求处理类进行数据解析
            BYTE* pValue  = m_lpReceiveDataBuff;
            int DealSize = 0;
            int valueSize = m_dwReceiveDataIndex;
            if (!m_pSMProtocol->IsAllowAnalyseReceiveDataOnBytesNum(m_dwReceiveDataIndex))
                return;
            // m_pSMProtocol->AnalyseReciveData(pValue, valueSize, pBegin, pEnd);
            std::list<LPBYTE> pBList;
            std::list<LPBYTE> pEList;
            m_pSMProtocol->AnalyseReciveData_list(pValue, valueSize, pBList, pEList, DealSize);

            while (!pBList.empty() && !pEList.empty())
            {
                BYTE* pBegin  = pBList.front();
                BYTE* pEnd    = pEList.front();
                pBList.pop_front();
                pEList.pop_front();
                ASSERT(pBegin);
                ASSERT(pEnd);

                //接收到一个协议帧(帧头帧尾合法)
                BYTE* byTemp    = pBegin;
                DWORD dwFramLen = pEnd - pBegin + 1;

                CString strFrameData;
                strReceiveData.Empty();
                for (int i = 0; i < dwFramLen; ++i)
                {
                    strReceiveData += theApp.FmtStr(_T("%02X "), *pBegin++);
                }
                pBegin = byTemp;

                BOOL bNotifyCallerFail = FALSE;
                BYTE byResponseType = 0x00;
                int nFrameType = 0;
                if (m_pSMProtocol->ProcessReceiveFrameData(pBegin, dwFramLen, nFrameType))
                {
                    if (nFrameType == 0x11)
                    {
                        int value = MAKEINTBIG(pBegin[8],pBegin[9],pBegin[10],pBegin[11]);
                        NewSystemSwitchRespond(value);
                    }
                    else if (nFrameType == 0x12)
                    {
                        int value = MAKEINTBIG(pBegin[8],pBegin[9],pBegin[10],pBegin[11]);
                        NewSystemStatusRespond(value);
                    }
                    if (0xC0 == *(byTemp + 1))
                    {
                        strLog.Format(_T("扫码工作线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%02X 帧,%s"), m_sLocalCOM.Port, *(byTemp + 1), strReceiveData);
                        theApp.RecordMsgTemp(strLog);
                        strLog.Empty();
                    }
                    else
                    {
                        strLog.Format(_T("扫码工作线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%02X 帧"), m_sLocalCOM.Port, *(byTemp + 1));
                    }
                    //是否打印日志
                    if (m_bPrintCommandDataInLog)
                    {
                        if (0xC0 != *(byTemp + 1))
                        {
                            CString strCmdLog;
                            strCmdLog.Format(_T("扫码工作线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%s"), m_sLocalCOM.Port, strReceiveData);
                            theApp.RecordMsgTemp(strCmdLog);
                            strLog.Empty();
                        }
                    }
                }
                else
                {
                    byResponseType = 0x02;
                    //对于接收到的异常数据进行处理(有帧头)
                    if (dwFramLen > 6)
                    {
                        //仅仅处理指定帧数据的回馈
                        if (   ( CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5 == *(byTemp + 5) )
                                && ( ( SM_CMD_START == *(byTemp + 6) ) || ( 0x57 == *(byTemp + 6) ) )
                           )
                        {
                            static int iReceiveEchoCount = 0x00;
                            static CString strLastRecData = _T("");
                            if (strReceiveData != strLastRecData)
                            {
                                iReceiveEchoCount = 0;
                                strLastRecData = strReceiveData;
                            }
                            //判断是否需要通知调用者(连续接收到3次相同的异常数据，通知调用者获取扫码结果失败）
                            if (++iReceiveEchoCount > 3)
                            {
                                //通知调用者，扫码失败
                                tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
                                if (NULL != pSMRemoteState)
                                {
                                    memset(pSMRemoteState, 0, sizeof(tagSMInnerDeviceState));

                                    pSMRemoteState->ScanResult = 0x01;
                                    PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO, (WPARAM)MAKELONG(SM_CMD_START, 0x00), (LPARAM)pSMRemoteState);
                                }
                                strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(多次失败),设置结果[获取扫码信息失败]"), m_sLocalCOM.Port, strReceiveData);
                            }
                            else
                            {
                                //回馈C0帧
                                CString strC0String = m_pSMProtocol->SendCmdResponse(byResponseType, CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5);
                                BOOL bSendState = WriteDataToComm(strC0String);
                                strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败),具体内容:%s，次数已经 %d 次，发送C0帧通知下位机重发该数据[%s]"), m_sLocalCOM.Port, strReceiveData, iReceiveEchoCount, bSendState ? _T("成功") : _T("失败"));
                            }
                        }

                        else
                        {
                            strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败),具体内容:%s"), m_sLocalCOM.Port, strReceiveData);
                        }
                    }
                    else
                    {
                        strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败,数据长度小于6字节),具体内容:%s"), m_sLocalCOM.Port, strReceiveData);
                    }
                }

                // 1017版本增加对接收数据处理
                if ( nFrameType > 0 ) //只有大于0，取到帧类型，才认为有效数据帧
                {
                    if ( CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_B1 == nFrameType ) //心跳帧默认不处理
                    {
                    }
                    else//其它非心跳应答帧
                    {
                        if ( m_nSendCmdCount > 0 )
                        {
                            m_nSendCmdCount -= 1;               //发送命令计数减1
                        }
                        m_dwLastChecktick = ::GetTickCount();   //重置检查时间片
                    }
                }

                //记录日志
                theApp.RecordMsgTemp(strLog, TRUE);
                strLog.Empty();

                //新增处理，如果需要重新发送数据，则在此进行处理(仅仅当接收到回馈失败，并且回馈失败的数据指令类型为A5帧的时候，才进行该项处理）
                if (m_pSMProtocol->m_bNeedReSendLastData)
                {
                    if (!m_pSMProtocol->m_strLastSendFrameData.IsEmpty())
                    {
                        static int iReSendLastCmdCount = 0x00;
                        static CString strLastVerifyCmd = _T("");
                        if (m_pSMProtocol->m_strLastSendFrameData != strLastVerifyCmd)
                        {
                            strLastVerifyCmd = m_pSMProtocol->m_strLastSendFrameData;
                            iReSendLastCmdCount = 0;
                        }

                        //同一个指令只允许重发3次
                        if (++iReSendLastCmdCount < 4)
                        {
                            //往串口写数据
                            if (WriteDataToComm(m_pSMProtocol->m_strLastSendFrameData))
                            {
                                strLog.Format(_T("扫码管理线程,底层处理:接收到下位机回馈C0帧，标识处理上位机指令失败，重新进行发送上次的数据【成功】，次数 %d "), iReSendLastCmdCount);
                            }
                            else
                            {
                                strLog.Format(_T("扫码管理线程,底层处理:接收到下位机回馈C0帧，标识处理上位机指令失败，重新进行发送上次的数据【失败】,次数 %d "), iReSendLastCmdCount);
                            }
                        }
                        else
                        {
                            iReSendLastCmdCount = 0;
                            m_pSMProtocol->m_bNeedReSendLastData = FALSE;// 1016版本增加对重发标记的设置

                            //判断是否重发的是扫码操作
                            if ( (m_pSMProtocol->m_strLastSendFrameData.GetLength() > 12)
                                    && ( dwFramLen > 6 && 0xA5 == *(byTemp + 5) && SM_CMD_START == *(byTemp + 6) ) )
                            {
                                //重发次数已经超过3次，判定扫码失败，通知调用者
                                tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
                                if (NULL != pSMRemoteState)
                                {
                                    memset(pSMRemoteState, 0, sizeof(tagSMInnerDeviceState));

                                    pSMRemoteState->ScanResult = 0x01;
                                    PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO, (WPARAM)MAKELONG(SM_CMD_START, 0x00), (LPARAM)pSMRemoteState);
                                    strLog.Format(_T("扫码管理线程,底层处理:由于硬件回馈获取指令失败，组件重发同一帧数据已经超过3次，判定扫码操作失败 "));
                                }
                            }
                        }
                        //记录日志
                        theApp.RecordMsgTemp(strLog);
                        strLog.Empty();
                    }
                }

                //将缓冲区域置0
                // memset( m_lpReceiveDataBuff, 0x00, m_sLocalCOM.InBufferSize);
                // m_dwReceiveDataIndex = 0;
            }
            //清除数据
            // 要清除的字节数
            if (DealSize > m_dwReceiveDataIndex)
            {
                DealSize = m_dwReceiveDataIndex;
            }
            int iDel = DealSize;

            // 得到新的缓冲区有效字节
            m_dwReceiveDataIndex -= iDel;

            // 分配临时区域
            BYTE* pTemp = new BYTE[m_dwReceiveDataIndex];

            // 将有用的数据流拷贝到临时区域
            memcpy( pTemp, m_lpReceiveDataBuff + iDel, m_dwReceiveDataIndex );

            // 将缓冲区域置0
            memset( m_lpReceiveDataBuff, 0x00, m_sLocalCOM.InBufferSize);

            // 从临时区域将数据流拷回到缓冲区
            memcpy( m_lpReceiveDataBuff, pTemp, m_dwReceiveDataIndex );

            // 删除临时区域
            delete [] pTemp;


        }
    }
    else
    {
        strLog.Format(_T("扫码工作线程,底层处理:接收到串口%d数据!"), byPort);
    }
    //记录日志
    if (!strLog.IsEmpty())
    {
        theApp.RecordMsgTemp(strLog);
        strLog.Empty();
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgReceiveDataFromCOM
//
// 功能描述：内部被动消息触发处理函数 -- 获取到串口数据
//
// 输入参数：WPARAM wParam
//         ：LPARAM lParam
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年1月29日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgReceiveDataFromCOM(WPARAM wParam, LPARAM lParam)
{
    return HandleComDataEx(wParam, lParam);

    if (NULL == m_lpReceiveDataBuff)
        return;

    BOOL bAnalyseState = FALSE;
    BYTE byPort = (BYTE)wParam;
    CString strLog;
    CString strReceiveData;

    if (NULL != m_pCOM)
    {
        DWORD dwReadBytes = 0;
        DWORD dwSize = m_sLocalCOM.InBufferSize - m_dwReceiveDataIndex;
        if ((dwReadBytes = m_pCOM->Read(m_lpReceiveDataBuff + m_dwReceiveDataIndex, dwSize)) > 0)
        {
            //获得读取数据的最新时间
            m_tLastComReadDataTime = time(NULL);
            //进制转换处理
            CString strTemp(theApp.Bin2Hex(m_lpReceiveDataBuff + m_dwReceiveDataIndex, dwReadBytes));

            //特殊兼容，只为了读取终端序列号
            if (0 == strTemp.CompareNoCase(_T("DFB0000AA1053030303031363744077AFDDFC00003A10500ED43FD")))
            {
                strLog.Format(_T("扫码工作线程,底层处理:接收到特定数据(终端序号):%s"), strTemp);
                theApp.RecordMsgTemp(strLog);
                strLog.Empty();

                strTemp = strTemp.Left(34);
                dwReadBytes = 17;
            }

            for (int i = 0; i < strTemp.GetLength(); i = i + 2)
            {
                strReceiveData += (strTemp.Mid(i, 2) + _T(" "));
            }
            strLog.Format(_T("扫码工作线程,底层处理:接收到串口%d数据(%d字节),具体数据:%s"), byPort, dwReadBytes, strReceiveData);
            //theApp.RecordMsgTemp(strLog);

            m_dwReceiveDataIndex += dwReadBytes;
            // 若缓冲区索引超出缓冲区大小,强制将缓冲区索引设置成缓冲区大小,
            if ( dwReadBytes > dwSize )
            {
                dwReadBytes = dwSize;
                return;
            }
            //要求处理类进行数据解析
            BYTE* pBegin  = NULL;
            BYTE* pEnd    = NULL;
            BYTE* pValue  = m_lpReceiveDataBuff;
            int valueSize = m_dwReceiveDataIndex;
            if (!m_pSMProtocol->IsAllowAnalyseReceiveDataOnBytesNum(m_dwReceiveDataIndex))
                return;
            m_pSMProtocol->AnalyseReciveData(pValue, valueSize, pBegin, pEnd);


            // 若没有找到帧头,说明这些数据流无用,全丢弃,且返回0
            if ( NULL == pBegin )
            {
                memset( m_lpReceiveDataBuff, 0x00, m_sLocalCOM.InBufferSize);
                m_dwReceiveDataIndex = 0;
                strLog.Format(_T("扫码工作线程,底层处理:接收到串口%d数据(%d字节),具体数据:%s,由于无法找到帧头，数据被丢弃。"), byPort, dwReadBytes, strReceiveData);
                //theApp.RecordMsgTemp(strLog, TRUE);
                return;
            }

            // 若找到帧头,没有找到帧尾,则将帧头之前的数据清除
            if ( (NULL != pBegin) && (NULL == pEnd) )
            {
                if ( pBegin != pValue )
                {
                    // 要清除的字节数
                    int iDel = pBegin - m_lpReceiveDataBuff;

                    // 得到新的缓冲区有效字节
                    m_dwReceiveDataIndex -= iDel;

                    // 分配临时区域
                    BYTE* pTemp = new BYTE[m_dwReceiveDataIndex];

                    // 将有用的数据流拷贝到临时区域
                    memcpy( pTemp, pBegin, m_dwReceiveDataIndex );

                    // 将缓冲区域置0
                    memset( m_lpReceiveDataBuff, 0x00, m_sLocalCOM.InBufferSize);

                    // 从临时区域将数据流拷回到缓冲区
                    memcpy( m_lpReceiveDataBuff, pTemp, m_dwReceiveDataIndex );

                    // 删除临时区域
                    delete [] pTemp;
                }

                return;
            }
            ASSERT(pBegin);
            ASSERT(pEnd);

            //接收到一个协议帧(帧头帧尾合法)
            BYTE* byTemp    = pBegin;
            DWORD dwFramLen = pEnd - pBegin + 1;

            CString strFrameData;
            strReceiveData.Empty();
            for (int i = 0; i < dwFramLen; ++i)
            {
                strReceiveData += theApp.FmtStr(_T("%02X "), *pBegin++);
            }
            pBegin = byTemp;

            BOOL bNotifyCallerFail = FALSE;
            BYTE byResponseType = 0x00;
            int nFrameType = 0;
            if (m_pSMProtocol->ProcessReceiveFrameData(pBegin, dwFramLen, nFrameType))
            {
                if (0xC0 == *(byTemp + 1))
                {
                    strLog.Format(_T("扫码工作线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%02X 帧,%s"), m_sLocalCOM.Port, *(byTemp + 1), strReceiveData);
                    theApp.RecordMsgTemp(strLog);
                    strLog.Empty();
                }
                else
                {
                    strLog.Format(_T("扫码工作线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%02X 帧"), m_sLocalCOM.Port, *(byTemp + 1));
                }
                //是否打印日志
                if (m_bPrintCommandDataInLog)
                {
                    if (0xC0 != *(byTemp + 1))
                    {
                        CString strCmdLog;
                        strCmdLog.Format(_T("扫码工作线程,底层处理:接收到串口[%d]数据(完整数据帧),具体内容:%s"), m_sLocalCOM.Port, strReceiveData);
                        theApp.RecordMsgTemp(strCmdLog);
                        strLog.Empty();
                    }
                }
            }
            else
            {
                byResponseType = 0x02;
                //对于接收到的异常数据进行处理(有帧头)
                if (dwFramLen > 6)
                {
                    //仅仅处理指定帧数据的回馈
                    if (   ( CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5 == *(byTemp + 5) )
                            && ( ( SM_CMD_START == *(byTemp + 6) ) || ( 0x57 == *(byTemp + 6) ) )
                       )
                    {
                        static int iReceiveEchoCount = 0x00;
                        static CString strLastRecData = _T("");
                        if (strReceiveData != strLastRecData)
                        {
                            iReceiveEchoCount = 0;
                            strLastRecData = strReceiveData;
                        }
                        //判断是否需要通知调用者(连续接收到3次相同的异常数据，通知调用者获取扫码结果失败）
                        if (++iReceiveEchoCount > 3)
                        {
                            //通知调用者，扫码失败
                            tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
                            if (NULL != pSMRemoteState)
                            {
                                memset(pSMRemoteState, 0, sizeof(tagSMInnerDeviceState));

                                pSMRemoteState->ScanResult = 0x01;
                                PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO, (WPARAM)MAKELONG(SM_CMD_START, 0x00), (LPARAM)pSMRemoteState);
                            }
                            strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(多次失败),设置结果[获取扫码信息失败]"), m_sLocalCOM.Port, strReceiveData);
                        }
                        else
                        {
                            //回馈C0帧
                            CString strC0String = m_pSMProtocol->SendCmdResponse(byResponseType, CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5);
                            BOOL bSendState = WriteDataToComm(strC0String);
                            strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败),具体内容:%s，次数已经 %d 次，发送C0帧通知下位机重发该数据[%s]"), m_sLocalCOM.Port, strReceiveData, iReceiveEchoCount, bSendState ? _T("成功") : _T("失败"));
                        }
                    }
                    else
                    {
                        strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败),具体内容:%s"), m_sLocalCOM.Port, strReceiveData);
                    }
                }
                else
                {
                    strLog.Format(_T("扫码管理线程,底层处理:接收到串口[%d]数据(校验失败,数据长度小于6字节),具体内容:%s"), m_sLocalCOM.Port, strReceiveData);
                }
            }

            // 1017版本增加对接收数据处理
            if ( nFrameType > 0 ) //只有大于0，取到帧类型，才认为有效数据帧
            {
                if ( CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_B1 == nFrameType ) //心跳帧默认不处理
                {
                }
                else//其它非心跳应答帧
                {
                    if ( m_nSendCmdCount > 0 )
                    {
                        m_nSendCmdCount -= 1;               //发送命令计数减1
                    }
                    m_dwLastChecktick = ::GetTickCount();   //重置检查时间片
                }
            }

            //记录日志
            theApp.RecordMsgTemp(strLog, TRUE);
            strLog.Empty();

            //新增处理，如果需要重新发送数据，则在此进行处理(仅仅当接收到回馈失败，并且回馈失败的数据指令类型为A5帧的时候，才进行该项处理）
            if (m_pSMProtocol->m_bNeedReSendLastData)
            {
                if (!m_pSMProtocol->m_strLastSendFrameData.IsEmpty())
                {
                    static int iReSendLastCmdCount = 0x00;
                    static CString strLastVerifyCmd = _T("");
                    if (m_pSMProtocol->m_strLastSendFrameData != strLastVerifyCmd)
                    {
                        strLastVerifyCmd = m_pSMProtocol->m_strLastSendFrameData;
                        iReSendLastCmdCount = 0;
                    }

                    //同一个指令只允许重发3次
                    if (++iReSendLastCmdCount < 4)
                    {
                        //往串口写数据
                        if (WriteDataToComm(m_pSMProtocol->m_strLastSendFrameData))
                        {
                            strLog.Format(_T("扫码管理线程,底层处理:接收到下位机回馈C0帧，标识处理上位机指令失败，重新进行发送上次的数据【成功】，次数 %d "), iReSendLastCmdCount);
                        }
                        else
                        {
                            strLog.Format(_T("扫码管理线程,底层处理:接收到下位机回馈C0帧，标识处理上位机指令失败，重新进行发送上次的数据【失败】,次数 %d "), iReSendLastCmdCount);
                        }
                    }
                    else
                    {
                        iReSendLastCmdCount = 0;
                        m_pSMProtocol->m_bNeedReSendLastData = FALSE;// 1016版本增加对重发标记的设置

                        //判断是否重发的是扫码操作
                        if ( (m_pSMProtocol->m_strLastSendFrameData.GetLength() > 12)
                                && ( dwFramLen > 6 && 0xA5 == *(byTemp + 5) && SM_CMD_START == *(byTemp + 6) ) )
                        {
                            //重发次数已经超过3次，判定扫码失败，通知调用者
                            tagSMInnerDeviceState* pSMRemoteState = new tagSMInnerDeviceState();
                            if (NULL != pSMRemoteState)
                            {
                                memset(pSMRemoteState, 0, sizeof(tagSMInnerDeviceState));

                                pSMRemoteState->ScanResult = 0x01;
                                PostThreadMessage(WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO, (WPARAM)MAKELONG(SM_CMD_START, 0x00), (LPARAM)pSMRemoteState);
                                strLog.Format(_T("扫码管理线程,底层处理:由于硬件回馈获取指令失败，组件重发同一帧数据已经超过3次，判定扫码操作失败 "));
                            }
                        }
                    }
                    //记录日志
                    theApp.RecordMsgTemp(strLog);
                    strLog.Empty();
                }
            }

            //将缓冲区域置0
            memset( m_lpReceiveDataBuff, 0x00, m_sLocalCOM.InBufferSize);
            m_dwReceiveDataIndex = 0;
        }
    }
    else
    {
        strLog.Format(_T("扫码工作线程,底层处理:接收到串口%d数据!"), byPort);
    }
    //记录日志
    if (!strLog.IsEmpty())
    {
        theApp.RecordMsgTemp(strLog);
        strLog.Empty();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgDealResponseForProtocolProcessResult
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
void CThreadSMManager2::OnMsgDealResponseForProtocolProcessResult(WPARAM wParam, LPARAM lParam)
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
        if (NULL == pSMRemoteState)
            throw - 1;
        //其它管理操作
        switch (wCmdType)
        {
        case SM_CMD_INIT:
        case SM_CMD_VER:
        case SM_CMD_STOP:
        case SM_CMD_RESET_FACTORY:
        case SM_CMD_RESET_PERSION:
        {
            strLog = theApp.FmtStr(_T("扫码工作线程,%s"), pSMRemoteState->StateDesc);
        }
        break;
        case SM_CMD_START:
        {
            if (0 == pSMRemoteState->ScanResult)
            {
                strLog = theApp.FmtStr(_T("扫码工作线程,操作步骤3:扫码器[%d]获取数据:%s"), pSMRemoteState->DeviceID, pSMRemoteState->Code);
            }
            else
            {
                strLog = theApp.FmtStr(_T("扫码工作线程,操作步骤3:未获取任何有效数据"));
            }
            //记录日志
            theApp.RecordMsgTemp(strLog);

            pSMRemoteState->ScanConsumeTime = GetTickCount() - m_dwScanStartTime;
            _tcscpy_s(pSMRemoteState->StateDesc, _countof(pSMRemoteState->StateDesc), strLog.GetBuffer(0)); strLog.ReleaseBuffer();
            //扫码结果格式化处理
            fnProcessInnerDeviceResponseForStartScan(pSMRemoteState);
        }
        break;
        }
        //删除数据
        delete pSMRemoteState;
        pSMRemoteState = NULL;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog.Format(_T("扫码工作线程,该次扫码完毕,协议解析类回馈数据指针异常"));
            break;
        }
        //记录日志
        theApp.RecordMsgTemp(strLog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgSetLightMode
//
// 功能描述：接收调用者消息处理函数 -- 控制串口板灯亮模式
//
// 输入参数：WPARAM wParam  -- 延迟时间（毫秒）
//         ：LPARAM lParam -- 灯亮模式(0,扫码;1,成功;2,失败;3,全灭)
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgSetLightMode(WPARAM wParam, LPARAM lParam)
{
    int iLightMode = (int)lParam;
    int iDelayTime = (int)wParam;
    Sleep(iDelayTime);
    if (   (iLightMode >= CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_SUCCESS)
            && (iLightMode <= CHTSMDeviceControl::SM_PRPTOCOL_LIGHT_ALL_OFF
                || 1 /*modify 2017-11 允许所有的控制 */)
       )
    {
        fnSMControlLight(iLightMode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgControlSerialBoard
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
void CThreadSMManager2::OnMsgControlSerialBoard(WPARAM wParam, LPARAM lParam)
{
    CString strLog;
    tagSMMainBoardParam* pCMDParam = (tagSMMainBoardParam*)lParam;
    try
    {
        if (NULL == pCMDParam)
            throw - 1;
        if (NULL == m_pSMProtocol)
            throw - 2;

        CString strSendData;
        switch (pCMDParam->CMDFlag)
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
            switch (iLightMode)
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
            strLog.Format(_T("扫码工作线程,控制扫码提示灯，设置为:%s"), strLightMode);

        }
        break;
        case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A4:
            strSendData = m_pSMProtocol->GetLightMode();
            break;

        //新增加A5显示指令
        case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5:
            strSendData = m_pSMProtocol->SetScanDisplay(sScanDisplay);
            strLog.Format(_T("扫码工作线程,交互设置:(Type:%d, Money:%d, Weight:%d, VType:%d, VClass:%d, Time:%s, EntryST:%s)"),
                          sScanDisplay.Type, sScanDisplay.Money, sScanDisplay.Weight, sScanDisplay.VehicleType,
                          sScanDisplay.VehicleClass,    CTime(sScanDisplay.tShowtime).Format("%Y-%m-%d %H:%M:%S"), sScanDisplay.EntrySTName );
            break;

        case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A6:
            strSendData = m_pSMProtocol->ResetBoard();
            break;

        case CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A7:
            strSendData = m_pSMProtocol->SetTransData(mTransData);
            break;

        }

        if (!WriteDataToComm(strSendData))
            throw - 3;

        ::Sleep(30);// 版本1018调整，对成功发送的串口数据，等待30 ms 处理时间

    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            break;
        }
        strLog.Format(_T("扫码工作线程,控制IO失败"));
    }
    //记录日志
    if (!strLog.IsEmpty())
    {
        theApp.RecordMsgTemp(strLog);
    }

    if (NULL != pCMDParam)
    {
        delete pCMDParam;
        pCMDParam = NULL;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnProcessInnerDeviceResponseForStartScan
//
// 功能描述：处理内部挂接设备的执行扫码指令后的回馈信息
//
// 输入参数：IN tagSMInnerDeviceState* pSMState    --  扫码器状态
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnProcessInnerDeviceResponseForStartScan(IN tagSMInnerDeviceState* pSMState)
{
    BOOL bGetData = FALSE;
    CString strError;
    BOOL bNeedNotifyCaller = TRUE;
    try
    {
        if (NULL == pSMState)
            throw - 1;

        if (OP_SUCCESS != pSMState->ScanResult)
            throw - 2;

        //格式化数据
        static CString  strLastCode = _T("");
        static DWORD    dwLastTick = 0;

        CString strCurrentCode(pSMState->Code);
        if (strCurrentCode != strLastCode)
        {
            strLastCode = strCurrentCode;
            dwLastTick = ::GetTickCount();
        }
        else
        {
            // 1017版本增加对重复数据的时间判断处理
            if (m_bAbandonEchoData) //是否判断重复数据
            {
                if ( ::GetTickCount() - dwLastTick < 3000 )
                {
                    bNeedNotifyCaller = FALSE;
                    throw - 3;
                }
                else
                {
                    //重复数据但是超过限定时间，直接赋值
                    strLastCode = strCurrentCode;
                    dwLastTick = ::GetTickCount();
                }
            }
            else
            {
                //不判断重复数据，直接赋值
                strLastCode = strCurrentCode;
                dwLastTick = ::GetTickCount();
            }
        }

        //格式化数据
        ::memset(&m_sSMResult, 0, sizeof(tagSMInnerDeviceState));
        ::memcpy(&m_sSMResult, pSMState, sizeof(tagSMInnerDeviceState));
        m_sSMResult.ScanResult = 0;

        bGetData = TRUE;
        //记录日志
        strError.Format(_T("扫码工作线程,该次扫码完毕,已经获取数据，发送通知消息给调用者"));
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1://接收到的数据为空
        case -2://接收到的数据非法
            strError.Format(_T("扫码工作线程,该次扫码完毕，未能获取数据,发送通知消息给调用者"));
            break;
        case -3://当前接收到重复数据
            strError.Format(_T("扫码工作线程,该次扫码完毕，接收到重复的数据，内部丢弃处理，不推送消息给调用者"));
            break;
        }
    }
    //通知调用者
    if (bNeedNotifyCaller)
    {
        if (::IsWindow(m_hMainWnd))
        {
            ::PostMessage(m_hMainWnd, m_dwMessageID, (WPARAM)bGetData, (LPARAM)0);
        }
        else if (0 != m_nMainThreadID)
        {
            ::PostThreadMessage(m_nMainThreadID, m_dwMessageID, (WPARAM)bGetData, (LPARAM)0);
        }
    }
    //记录日志
    theApp.RecordMsgTemp(strError);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnSMScanCode
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
void CThreadSMManager2::fnSMScanCode(void)
{
    //开始计时
    GetLocalTime(&m_stBeginTime);
    m_dwScanStartTime = GetTickCount();

    //变量声明
    CString strSendData;
    CString strInnerDeviceCommand;
    DWORD   dwCommandDataSize = 0;
    //标记工作状态
    m_bWorking = TRUE;
    m_tNoDataFromCOMLastTime = time(NULL);

    CString strError;
    //开始执行扫描操作
    try
    {
        if (!m_bOpen || (NULL == m_pSMProtocol))
            throw - 1;
        //记录日志
        //theApp.RecordMsgTemp(FmtStr(_T("扫码工作线程,开始扫码流程")));

        //strError.Format(_T("操作步骤%d:正在准备扫描,计时[%s]开始"),0x01,GetCurrentTimeString(1));
        //theApp.RecordMsgTemp(FmtStr(_T("扫码工作线程,%s"),strError));

        //获取内挂设备的扫码命令内容
        strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"), SM_CMD_START, m_pSMProtocol->GetInnerDeviceProtocolClass()->Start());
        //获取格式化发送数据
        strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
        if (strSendData.IsEmpty())
            throw - 2;

        ////记录日志
        //strError.Format(_T("操作步骤%d:获取到规格化的二进制命令数据[%s]"),0x02,strSendData);
        //theApp.RecordMsgTemp(FmtStr(_T("扫码工作线程,%s"),strError));

        //往串口写数据
        if (!WriteDataToComm(strSendData))
            throw - 3;

        //记录日志
        strError.Format(_T("操作步骤%d:扫码命令已发送，正在扫描数据，请稍后"), 0x01);
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,%s"), strError));

    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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

        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,%s"), strError));
    }
    //标记工作状态
    m_bWorking = FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnSMStopScan
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
void CThreadSMManager2::fnSMStopScan(void)
{
    CString strSendData;
    CString strInnerDeviceCommand;
    DWORD   dwCommandDataSize = 0;
    BYTE    byCmdData[1024] = {0};
    try
    {
        if (!m_bOpen || (NULL == m_pSMProtocol))
            throw - 1;
        CHTSMDeviceBaseIF* pSMIF = m_pSMProtocol->GetInnerDeviceProtocolClass();
        if (NULL == pSMIF)
            throw - 2;
        //获取停止扫描命令
        strInnerDeviceCommand = theApp.FmtStr(_T("%02X%s"), SM_CMD_STOP, pSMIF->Stop());
        if (strInnerDeviceCommand.IsEmpty())
            throw - 2;
        //获取格式化发送数据
        strSendData = m_pSMProtocol->SendUserDefineCmd(strInnerDeviceCommand);
        if (strSendData.IsEmpty())
            throw - 3;
        //往串口写数据
        if (!WriteDataToComm(strSendData))
            throw - 4;
        //写日志
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送强制停止扫码指令[成功]")));
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送强制停止扫码指令[失败]")));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnSMControlLight
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
void CThreadSMManager2::fnSMControlLight(IN const int& iLightType)
{
    tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
    if (NULL != pParam)
    {
        memset(pParam, 0, sizeof(tagSMMainBoardParam));

        pParam->Type        = 1;
        pParam->CMDFlag     = CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A3;
        pParam->ParamData[0] = iLightType;
        pParam->ParamSize   = 0x01;

        OnMsgControlSerialBoard((WPARAM)0, (LPARAM)pParam);
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::fnSendGetDeviceSerialNoCMD
//
// 功能描述：发送获取终端序列号的指令
//
// 输入参数：void
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2017年10月13日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::fnSendGetDeviceSerialNoCMD(void)
{
    CString strSendData;
    CString strInnerDeviceCommand;
    DWORD   dwCommandDataSize = 0;
    BYTE    byCmdData[1024] = {0};
    try
    {
        if (!m_bOpen || (NULL == m_pSMProtocol))
            throw - 1;
        //获取格式化发送数据
        strSendData = m_pSMProtocol->ReadConfigInfoFromBoard(0x00);
        if (strSendData.IsEmpty())
            throw - 2;
        //往串口写数据
        if (!WriteDataToComm(strSendData))
            throw - 3;
        //写日志
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送获取硬件序列号指令[成功]")));
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送获取硬件序列号指令[失败]")));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadSMManager2::OnMsgSetDisplay
//
// 功能描述：接收调用者消息处理函数 -- 设置显示内容
//
// 输入参数：WPARAM wParam  -- 延迟时间（毫秒）
//         ：LPARAM lParam -- 灯亮模式(0,扫码;1,成功;2,失败;3,全灭)
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月21日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadSMManager2::OnMsgSetDisplay(WPARAM wParam, LPARAM lParam)
{
    int iSerial = (int)wParam;//获取消息序列号
    if ( iSerial == sScanDisplay.iSerial ) //抛送序列号等于当前序列号，执行显示操作
    {
        if ( 0 != sScanDisplay.DealInfoUse )
        {
            tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
            if (NULL != pParam)
            {
                memset(pParam, 0, sizeof(tagSMMainBoardParam));

                pParam->Type        = 1;
                pParam->CMDFlag     = CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A5;
                pParam->ParamData[0] = 0;
                pParam->ParamSize   = 0x01;

                OnMsgControlSerialBoard((WPARAM)0, (LPARAM)pParam);
            }
        }

        LockMedia();

        if ( 0 != sScanDisplay.GGMsgUse )
        {
            tagMMIMedia     sAdd;
            ::memset(&sAdd, 0, sizeof(tagMMIMedia));

            sAdd.Type = 102;
            ::memcpy(sAdd.Content, sScanDisplay.GGMsg, _tcslen(sScanDisplay.GGMsg) );
            aryMedia.Add( sAdd );
        }

        if ( 0 != sScanDisplay.GGVoiceUse )
        {
            tagMMIMedia     sAdd;
            ::memset(&sAdd, 0, sizeof(tagMMIMedia));

            sAdd.Type = 103;
            ::memcpy(sAdd.Content, sScanDisplay.GGVoice, _tcslen(sScanDisplay.GGVoice) );
            aryMedia.Add( sAdd );
        }

        if ( 0 != sScanDisplay.GGWelSetUse )
        {
            tagMMIMedia     sAdd;
            ::memset(&sAdd, 0, sizeof(tagMMIMedia));

            sAdd.Type = 104;
            ::memcpy(sAdd.Content, sScanDisplay.GGWelSet, _tcslen(sScanDisplay.GGWelSet) );
            aryMedia.Add( sAdd );
        }

        FreeMedia();
    }
}

//////////////////////////////////////////////////////////////
// 函数名称:OnMsgTransmission
//
// 功能描述:
//
// 输入参数:WPARAM wParam,LPARAM lParam,
// 输出参数:afx_msg void
// 编写人员:朱亚冰
// 编写时间:2017-09-13
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
afx_msg void CThreadSMManager2::OnMsgTransmission(WPARAM wParam, LPARAM lParam)
{
    int iSerial = (int)wParam;//获取消息序列号
    if (iSerial != mTransData.iSerial)
    {
        return ;
    }

    tagSMMainBoardParam* pParam = new tagSMMainBoardParam();
    if (NULL != pParam)
    {
        memset(pParam, 0, sizeof(tagSMMainBoardParam));

        pParam->Type        = 1;
        pParam->CMDFlag     = CHTSMDeviceControl::SM_PROTOCOL_FRAME_TYPE_A7;
        pParam->ParamData[0] = 0;
        pParam->ParamSize   = 0x01;

        OnMsgControlSerialBoard((WPARAM)0, (LPARAM)pParam);
    }
}

//收到回应，硬件数据
afx_msg void CThreadSMManager2::OnMsgTransmissionResponse(WPARAM wParam, LPARAM lParam)
{
    tagTransmissionFeedback* tmp_p = (tagTransmissionFeedback*)lParam;
    if (tmp_p == NULL)
    {
        return;
    }
    ::memcpy(&mTransResponse, tmp_p, sizeof(tagTransmissionFeedback));
    delete tmp_p;

    if (::IsWindow(m_hMainWnd))
    {
        ::PostMessage(m_hMainWnd, m_dwMessageID, (WPARAM)TRUE, (LPARAM)0);
    }
    else if (0 != m_nMainThreadID)
    {
        ::PostThreadMessage(m_nMainThreadID, m_dwMessageID, (WPARAM)TRUE, (LPARAM)0);
    }

}
afx_msg void CThreadSMManager2::OnMsgTTS(WPARAM wParam, LPARAM lParam)
{
    CString cmd = m_pSMProtocol->ConstructTTS(m_SoundMsg);
    WriteDataToComm(cmd);
}

void  CThreadSMManager2::SetTTSMessage(const std::string& msg)
{
    m_SoundMsg = msg;
}

///固件升级处理,包含样式和语音
//注意：这里只进行发送，不进行串口状态的检测
void CThreadSMManager2::FirmwareUpgradeProcess()
{
    FreeDisplayUpgrade();
    FIRMWARE_UPGRADE_STATE upgradeState = m_firmwareRemoteControl.UpgradeState();
    FirmwareUpgradeControl tmpControlCmd;//用于开启和关闭升级处理
    FirmwareUpgradeTransfer tmpUpgradeData;
    CString tmpComStr;
    switch (upgradeState )
    {
    case FIRMWARE_UPGRADE_STATE_IDLE://idle
        break;
    case FIRMWARE_UPGRADE_STATE_AVAILABLE://start
        tmpControlCmd = m_firmwareRemoteControl.UpgradeControlCMD(FIRMWARE_UPGRADE_CONTROL_STARTUP);
        tmpComStr = m_pSMProtocol->ConstructUpgradeControl(tmpControlCmd);
        theApp.RecordMsgTemp("firmware update start");
        m_firmwareRemoteControl.SetUpgradeState(FIRMWARE_UPGRADE_STATE_UPGRADING);
        WriteDataToComm(tmpComStr);
        break;
    case FIRMWARE_UPGRADE_STATE_UPGRADING://ing
        if (m_firmwareRemoteControl.QueryUpgradeData(tmpUpgradeData))
        {
            theApp.RecordMsgTemp("firmware updating");
            tmpComStr = m_pSMProtocol->ConstructUpgradeData(tmpUpgradeData);
            WriteDataToComm(tmpComStr);
            Sleep(500);//for debug instead of 500
        }
        else
        {
            //no data;
        }
        break;
    case FIRMWARE_UPGRADE_STATE_COMPLETE://complete
        tmpControlCmd = m_firmwareRemoteControl.UpgradeControlCMD(FIRMWARE_UPGRADE_CONTROL_COMPLETE);
        tmpComStr = m_pSMProtocol->ConstructUpgradeControl(tmpControlCmd);
        WriteDataToComm(tmpComStr);
        theApp.RecordMsgTemp("firmware update complete");
        m_firmwareRemoteControl.RemoveUpgradeFile();

        m_firmwareRemoteControl.KeepIdle();
        break;
    case FIRMWARE_UPGRADE_STATE_ERROR:
    case FIRMWARE_UPGRADE_STATE_UNDEFINED://error
        theApp.RecordMsgTemp("firmware update error");
        tmpControlCmd = m_firmwareRemoteControl.UpgradeControlCMD(FIRMWARE_UPGRADE_CONTROL_STOP);
        tmpComStr = m_pSMProtocol->ConstructUpgradeControl(tmpControlCmd);
        WriteDataToComm(tmpComStr);

        m_firmwareRemoteControl.KeepIdle();
        break;
    }
}

void CThreadSMManager2::FirmwareUpgradeInfo(const std::string& paraJson)
{
    m_firmwareRemoteControl.SetUpgradeInfo(paraJson);
}

bool CThreadSMManager2::IsUpgrading()
{
    FIRMWARE_UPGRADE_STATE upgradeState = m_firmwareRemoteControl.UpgradeState();
    if (FIRMWARE_UPGRADE_STATE_IDLE == upgradeState)
    {
        return false;
    }
    return true;

}

void   CThreadSMManager2::fnSendFirmwareVerCMD()
{

    //copy of fnSendGetDeviceSerialNoCMD
    CString strSendData;
    CString strInnerDeviceCommand;
    DWORD   dwCommandDataSize = 0;
    BYTE    byCmdData[1024] = {0};
    try
    {
        if (!m_bOpen || (NULL == m_pSMProtocol))
            throw - 1;
        //获取格式化发送数据
        strSendData = "DFA2000101CCE3FD";//读取固件版本号的指令
        if (strSendData.IsEmpty())
            throw - 2;
        //往串口写数据
        if (!WriteDataToComm(strSendData))
            throw - 3;
        //写日志
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送获取固件版本号指令[成功]")));
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送获取固件版本号指令[失败]")));
    }

}

void    CThreadSMManager2::fnSendSlaveFirmwareVerCMD()
{
    CString strSendData;
    CString strInnerDeviceCommand;
    DWORD   dwCommandDataSize = 0;
    BYTE    byCmdData[1024] = {0};
    try
    {
        if (!m_bOpen || (NULL == m_pSMProtocol))
            throw - 1;
        strSendData = "DFA20001050FE2FD";
        if (strSendData.IsEmpty())
            throw - 2;

        if (!WriteDataToComm(strSendData))
            throw - 3;
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送获取从机固件版本号指令[成功]")));
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
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
        theApp.RecordMsgTemp(theApp.FmtStr(_T("扫码工作线程,发送获取从机固件版本号指令[失败]")));
    }

}

void CThreadSMManager2::FreeDisplayUpgrade()
{
    std::list<FreeDisplayData>  fdd_list = m_firmwareRemoteControl.FreeDisplayDataList();

    for (std::list<FreeDisplayData>::iterator it = fdd_list.begin(); it != fdd_list.end(); ++it)
    {
        //theApp.RecordMsgTemp(it->HexData.c_str());
        CString tmpComStr = m_pSMProtocol->ConstructFreeDisplayData(*it);
        WriteDataToComm(tmpComStr);
    }
    std::list<FreeDisplayStyle>  fds_list = m_firmwareRemoteControl.FreeDisplayStyleList();
    for (std::list<FreeDisplayStyle>::iterator it = fds_list.begin(); it != fds_list.end(); ++it)
    {
        theApp.RecordMsgTemp(it->HexData.c_str());
        CString tmpComStr = m_pSMProtocol->ConstructFreeDisplayStyle(*it);
        WriteDataToComm(tmpComStr);
    }
}
//远程升级结果
afx_msg void CThreadSMManager2::OnMsgRemoteControlResult(WPARAM wParam, LPARAM lParam)
{
    //theApp.RecordMsgTemp("notify sdf sdf sdfsd fsd");
    if (0 != m_nMainThreadID)
    {
        //清除固件版本信息，以便重新获取
        theApp.m_strFirmwareVer = "";
        theApp.m_strSlaveFirmwareVer = "";
        theApp.RecordMsgTemp("get firmware versoin again,wait 120s");

        ::PostThreadMessage(m_nMainThreadID, WM_THREAD_REMOTE_UPGRADE_RESULT, wParam, lParam);
    }
}

bool CThreadSMManager2::QueryTerminalSystemStatus(int& status)
{
    ClearSystemStatusRespond();
    CString tmpComStr = m_pSMProtocol->ConstructTerminalStatusQuery();
    WriteDataToComm(tmpComStr);

    //查询状态
    int sleepUnitMillSec = 100;
    int sleepTotalSlice = 10;
    for (int i = 0; i < sleepTotalSlice; ++i)
    {
        if (TakeOneSystemStatusRespond(status))
        {
            return true;
        }
        Sleep(sleepUnitMillSec);

    }
    return false;
}
bool CThreadSMManager2::ExcuteSystemSwitch(const int& targetVer)
{
    ClearSystemSwitchRespond();
    int sleepUnitMillSec = 100;
    int sleepTotalSlice = 10;
    int switchStatus = 0;
    if (targetVer == 15) //切换到1.5
    {
        //send syn
        CString tmpComStr = m_pSMProtocol->ConstructSwitchSync(SWITCHTO_V1_5_SYN);
        WriteDataToComm(tmpComStr);

        //wait ack syn
        switchStatus = -99;
        for (int i = 0; i < sleepTotalSlice; ++i)
        {
            if (TakeOneSystemSwitchRespond(switchStatus))
            {
                break;
            }
            Sleep(sleepUnitMillSec);
        }
        if (switchStatus != SWITCHTO_V1_5_SYN_ACK)
        {
            return false;
        }

        // send ack
        tmpComStr = m_pSMProtocol->ConstructSwitchSync(SWITCHTO_V1_5_ACK);
        WriteDataToComm(tmpComStr);
        return true;

    }
    else if (targetVer == 20) //切换到2.0
    {
        //send syn
        CString tmpComStr = m_pSMProtocol->ConstructSwitchSync(SWITCHTO_V2_0_SYN);
        WriteDataToComm(tmpComStr);

        //wait ack syn
        switchStatus = -99;
        for (int i = 0; i < sleepTotalSlice; ++i)
        {
            if (TakeOneSystemSwitchRespond(switchStatus))
            {
                break;
            }
            Sleep(sleepUnitMillSec);
        }
        if (switchStatus != SWITCHTO_V2_0_SYN_ACK)
        {
            return false;
        }

        // send ack
        tmpComStr = m_pSMProtocol->ConstructSwitchSync(SWITCHTO_V2_0_ACK);
        WriteDataToComm(tmpComStr);
        return true;
    }

    return false;
}
bool CThreadSMManager2::NewSystemSwitchRespond(const int &val)
{
    CScopeLock comLock(&m_ComRepondSyncMutex);
    m_SystemSwitchRespondQueue.push_back(val);
    while (m_SystemSwitchRespondQueue.size() > 10)
    {
        m_SystemSwitchRespondQueue.pop_front();
    }
    return true;
}
bool CThreadSMManager2::TakeOneSystemSwitchRespond(int& val)
{
    CScopeLock comLock(&m_ComRepondSyncMutex);
    if (m_SystemSwitchRespondQueue.empty())
    {
        return false;
    }
    val = m_SystemSwitchRespondQueue.front();
    m_SystemSwitchRespondQueue.pop_front();
    return true;
}
bool CThreadSMManager2::NewSystemStatusRespond(const int& val)
{
    CScopeLock comLock(&m_ComRepondSyncMutex);
    m_SystemStatusRespondQueue.push_back(val);
    while (m_SystemStatusRespondQueue.size() > 10)
    {
        m_SystemStatusRespondQueue.pop_front();
    }
    return true;
}
bool CThreadSMManager2::TakeOneSystemStatusRespond(int& val)
{
    CScopeLock comLock(&m_ComRepondSyncMutex);
    if (m_SystemStatusRespondQueue.empty())
    {
        return false;
    }
    val = m_SystemStatusRespondQueue.front();
    m_SystemStatusRespondQueue.pop_front();
    return true;
}

bool CThreadSMManager2::ClearSystemSwitchRespond()
{
    CScopeLock comLock(&m_ComRepondSyncMutex);
    m_SystemSwitchRespondQueue.clear();
    return true;
}
bool CThreadSMManager2::ClearSystemStatusRespond()
{
    CScopeLock comLock(&m_ComRepondSyncMutex);
    m_SystemStatusRespondQueue.clear();
    return true;
}