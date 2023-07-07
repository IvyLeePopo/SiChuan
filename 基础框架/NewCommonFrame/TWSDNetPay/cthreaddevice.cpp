#include "cthreaddevice.h"
#include "twsdnetpay.h"
#include <string>
#include <QDir>

CThreadDevice*     CThreadDevice::pThis    = NULL;

CThreadDevice::CThreadDevice(void)
    : m_pHTSDDeviceDll(NULL),
      m_bInitConfigDevice(false),
      m_bInitDebviceSuccessed(false)
{
    //扣款
    connect(this,SIGNAL(WM_THREAD_DEVICE_START_DEBIT_FLOW(QString , int)),this,SLOT(OnMsgStartDebitFlow(QString , int)));
    //撤单
    connect(this,SIGNAL(WM_THREAD_DEVICE_DEBIT_CANCEL(QString, int)),this,SLOT(OnMsgDebitCancelFlow(QString, int)));
    //清屏 费显
    connect(this,SIGNAL(WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString , int)),this,SLOT(OnMsgShowDealInfo(QString , int)));

    //消息回馈通知
    connect(this, SIGNAL(SENDCALLBACKRESULT(int, QString)), this, SLOT(OnMsgCallbackResult(int,QString)));

    connect(&m_timerID_InitConfigDevice, SIGNAL(timeout()), this, SLOT(OnTimerInitConfigDevice()));
    connect(&m_timerID_ActiveThread, SIGNAL(timeout()), this, SLOT(OnTimerActiveThread()));
    connect(&m_timerID_InitDevice, SIGNAL(timeout()), this, SLOT(OnTimerInitDevice()));

    StartTimer(TIMER_ID_INIT_DEVICE, 8*1000);//通讯动态库初始化接口，传递回调地址下去，必须首先调用
    StartTimer(TIMER_ID_INITCONFIG_DEVICE, 10*1000);//通讯动态库初始化参数下发
    StartTimer(TIMER_ID_THREAD_DEVICE, GetGlobalApp()->m_dwThreadCheckSpanTime * 1000);

}


CThreadDevice::~CThreadDevice()
{
    if(NULL != m_pHTSDDeviceDll)
    {
        delete m_pHTSDDeviceDll;
        m_pHTSDDeviceDll = NULL;
    }
}

void CThreadDevice::run()
{
    if(NULL == m_pHTSDDeviceDll)
    {
        m_pHTSDDeviceDll = new CHTSDDeviceDll();
    }

    Init();
    OnTimerInitDevice();
    OnTimerInitConfigDevice();

    exec();
}

void CThreadDevice::StartTimer(const quint32& nEventID, const quint64& dwDelayTime)
{
    switch(nEventID)
    {
    case TIMER_ID_INITCONFIG_DEVICE:
        m_timerID_InitConfigDevice.start(dwDelayTime);
        break;
    case TIMER_ID_THREAD_DEVICE:
        m_timerID_ActiveThread.start(dwDelayTime);
        break;
    case TIMER_ID_INIT_DEVICE:
        m_timerID_InitDevice.start(dwDelayTime);
    default:
        break;
    }
}


void CThreadDevice::StopTimer(const quint32& nEventID)
{
    switch(nEventID)
    {
    case TIMER_ID_INITCONFIG_DEVICE:
        if(m_timerID_InitConfigDevice.isActive())
        {
            m_timerID_InitConfigDevice.stop();
        }
        break;
    case TIMER_ID_THREAD_DEVICE:
        if(m_timerID_ActiveThread.isActive())
        {
            m_timerID_ActiveThread.stop();
        }
        break;
    case TIMER_ID_INIT_DEVICE:
        if(m_timerID_InitDevice.isActive())
        {
            m_timerID_InitDevice.stop();
        }
        break;
    default:
        break;
    }
}


void CThreadDevice::OnTimerInitDevice()
{
    if(NULL == m_pHTSDDeviceDll )
    {
        return ;
    }

    std::string strError;
    std::string strPortName = theApp.m_PortName;
    int nRate = theApp.m_BaudRate;
    if(  !m_pHTSDDeviceDll->GetDevInitState()  )
    {
        if (!m_pHTSDDeviceDll->InitDev(strPortName, nRate ,fnGetNotifyFromCommModule))
        {
            m_pHTSDDeviceDll->GetLastErrorDesc(strError);
            GetGlobalApp()->WriteLogFile(QString("[设备控制线程], 异常错误:%1").arg(strError.c_str()));
        }

    }
}

void CThreadDevice::OnTimerActiveThread()
{
    static unsigned int nPrintLog = 0;
    if(1 == nPrintLog%10)
    {
        ++nPrintLog;
        GetGlobalApp()->WriteLogFile("设备控制线程],线程存活日志检测");
    }
}

void CThreadDevice::OnTimerInitConfigDevice()
{
    //往串口下发设备初始化指令
    if (!m_bInitConfigDevice )
    {
        m_bInitConfigDevice = InitConfigDevice();
    }
}


bool CThreadDevice::Init()
{
    bool breturn = false;
    QString t_StrDir;
    QDir t_mydir;
    if(NULL == m_pHTSDDeviceDll )
    {
        return breturn;
    }

    //在构造函数完成之后，保存this指针，方便回调使用
    pThis = this;


    t_StrDir = t_mydir.currentPath();
     int npos = t_StrDir.lastIndexOf('/');
     QString newpath = t_StrDir.mid(0,npos) + FILE_NAME_DEVICE_DLL;

    qDebug()<<"CThreadDevice::Init()"<<newpath;
    if(m_pHTSDDeviceDll->Init(newpath))
    {
        breturn = true;
    }

    return breturn;
}

/***************************************************
 * 函数名称：GetDeviceState
 * 函数描述：获取硬件设备状态
 * 函数参数：void
 * 函数返回: quint32，按位进行标记，定义如下：0位,驱动加载失败;1,串口打开失败;2,无心跳(通讯故障)
 * ************************************************/
quint32 CThreadDevice::GetDeviceState(void)
{
    quint32 dwState = 0x00;
    quint32 dwReturnState = 0x00;
    try
    {
        if(NULL == m_pHTSDDeviceDll)
            throw -1;
        //获取硬件当前状态
        if(!m_pHTSDDeviceDll->CheckDeviceStatus(dwState))
            throw -2;
        //获取驱动加载状态
        if(!GetDeviceInitState())
        {
            dwReturnState |= 0x80;
        }
        //获取硬件串口状态
        dwReturnState |= dwState;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        case -2:
            break;
        }
    }
    return dwReturnState;
}



/***************************************************
 * 函数名称：OnMsgShowDealInfo
 * 函数描述：接收到信息，要求执行费显功能
 * 函数参数：VOID
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgShowDealInfo(QString wParam, int lParam)
{
    theApp.WriteLogFile("into CThreadDevice::OnMsgShowDealInfo");

    QString strLog;

    try
    {
        if(wParam.isEmpty())
            throw -1;

        if(NULL == m_pHTSDDeviceDll)
            throw -2;

        if(!m_pHTSDDeviceDll->GetDevInitState())
            throw -3;

        if(!m_bInitDebviceSuccessed)
        {
            throw -4;
        }

        //设置显示交互信息
        QString strData = wParam;
        int nType = lParam;
        m_pHTSDDeviceDll->SetDevice(strData,nType);


    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = (("[设备控制线程],接收到的参数异常,指针为空"));
            break;
        case -2:
            strLog = (("[设备控制线程],通讯模块状态异常,指针为空"));
            break;
        case -3:
            strLog = (("[设备控制线程],通讯模块未能成功初始化"));
            break;
        case -4:
            strLog = (("[设备控制线程--显示交互],设备参数未能成功初始化"));
            break;

        }

        GetGlobalApp()->WriteLogFile(strLog);
    }

}




/***************************************************
 * 函数名称：OnMsgStartDebitFlow
 * 函数描述：功能描述：接收到消息，启动扣款流程（整合操作模式）
 * 函数参数：VOID
 * 函数返回: VOID
 * ************************************************/
void CThreadDevice::OnMsgStartDebitFlow(QString wParam, int lParam)
{

    GetGlobalApp()->WriteLogFile("into CThreadDevice::OnMsgStartDebitFlow");

    unsigned char byCmdType		= RDM_DEBITE_START;
    unsigned char byExecuteCode	= OP_FAILED;
    QString strLog;
    try
    {
        if(wParam.isEmpty())
            throw -1;
        if(NULL == m_pHTSDDeviceDll)
            throw -2;
        if(!m_pHTSDDeviceDll->GetDevInitState())
            throw -3;
        if(!m_bInitDebviceSuccessed)
            throw -4;


        QString strData = wParam;
        int nType = lParam;

        //启动扣款流程
        QString strResult;

        if(!m_pHTSDDeviceDll->DebitMoney(strData,strResult, nType))
            throw -5;

        strLog = ("[设备控制线程],扣费流程执行完毕");
        byExecuteCode = OP_SUCCESS;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = (("[设备控制线程],接收到的参数异常,指针为空"));
            break;
        case -2:
            strLog = (("[设备控制线程],通讯模块状态异常,指针为空"));
            break;
        case -3:
            strLog = (("[设备控制线程],通讯模块未能成功初始化"));
            break;
        case -4:
            strLog = (("[设备控制线程--扣款流程],设备参数未能成功初始化"));
            break;
        case -5:

            std::string strError;
            m_pHTSDDeviceDll->GetLastErrorDesc(strError);
            byExecuteCode = 0xFF;
            strLog = QString("[设备控制线程],执行扣费操作失败，错误原因描述：%1").arg(strError.c_str());
            if(!strError.compare("取消"))
            {
                byExecuteCode = OP_CANCLE;
            }
            else if (!strError.compare("账户余额不足") )
            {
                byExecuteCode = OP_NO_MONEY;
            }
            else if (!strError.compare("二维码非法") )
            {
                byExecuteCode = OP_QRCODE_ILLEGAL;
            }
            else if (!strError.compare("二维码超出有效期") )
            {
                byExecuteCode = OP_QRCODE_OT;
            }
            else if (!strError.compare("支付凭证异常") )
            {
                byExecuteCode = OP_NO_ACCOUNT_INFO;
            }
            else if (!strError.compare("扣款超时") )
            {
                byExecuteCode = OP_DEBIT_OT;
            }
            else if (!strError.compare("密码错误") )
            {
                byExecuteCode = OP_PSW_ERR;
            }
            else if (!strError.compare("参数错误") )
            {
                byExecuteCode = OP_PARAM_ERR;
            }
            else if (!strError.compare("上次交易未结束") )
            {
                byExecuteCode = OP_LAST_TRADE_NOT_END;
            }
            else if (!strError.compare("组件未初始化") )
            {
                byExecuteCode = OP_COM_NOT_INIT;
            }
            else if (!strError.compare("组件校验非法") )
            {
                byExecuteCode = OP_COM_ILLEGAL;
            }
            else if (!strError.compare("支付平台网络故障") )
            {
                //byExecuteCode = OP_PAY_PLATFORM_ERR;
            }
            else if( !strError.compare("与下位机通讯异常") )
            {
                //byExecuteCode = OP_DISCONNECT_TERMINAL;
            }

            break;
        }

        GetGlobalApp()->NotifyCaller(byCmdType,byExecuteCode);
    }

    if(!strLog.isEmpty() )
    {
        GetGlobalApp()->WriteLogFile(strLog);
    }

}

void CThreadDevice::OnMsgDebitCancelFlow(QString wParam, int lParam)
{
    QString strLog;
    bool bSaveError = false;
    unsigned char  byCmdType		= RCC_FUN_CANCEL;
    unsigned char  byExecuteCode	= OP_FAILED;
    try
    {
        if(wParam.isEmpty())
            throw -1;

        if (NULL == m_pHTSDDeviceDll)
            throw - 2;
        if (!m_pHTSDDeviceDll->GetDevInitState())
            throw - 3;
        if(!m_bInitDebviceSuccessed)
            throw -4;

        QString strParam = wParam;
        int iType =  lParam;


        strLog = ("[设备控制线程],接收到指令，开始执行撤单操作");
        //保存日志
        GetGlobalApp()->WriteLogFile(strLog);
        //执行撤单操作
        iType = 0x00;
        if (!m_pHTSDDeviceDll->DebitCancel(strParam, iType))
            throw - 5;

        strLog = (("[设备控制线程],撤单操作执行完毕"));
        //标识执行成功
        byExecuteCode	= OP_SUCCESS;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog = "[设备控制线程],接收到的参数异常,指针为空";
            break;
        case -2:
            strLog = "[设备控制线程],通讯模块动态库异常,指针为空";
            break;
        case -3:
            strLog = "[设备控制线程],获取通讯模块动态库状态异常";
            break;
        case -4:
            strLog = "[设备控制线程--撤单操作],设备参数未能初始化成功";
            break;
        case -5:
        {
            byExecuteCode = RCC_INNER_ERROR;//add 2017-07-04
            std::string strError;
            m_pHTSDDeviceDll->GetLastErrorDesc(strError);
            strLog = QString("[设备控制线程],执行撤单操作失败，错误原因描述：%1").arg(strError.c_str());
            if(!strError.compare("取消"))
            {
                byExecuteCode = OP_CANCLE;
            }
            else if (!strError.compare("账户余额不足") )
            {
                byExecuteCode = OP_NO_MONEY;
            }
            else if (!strError.compare("二维码非法") )
            {
                byExecuteCode = OP_QRCODE_ILLEGAL;
            }
            else if (!strError.compare("二维码超出有效期") )
            {
                byExecuteCode = OP_QRCODE_OT;
            }
            else if (!strError.compare("支付凭证异常") )
            {
                byExecuteCode = OP_NO_ACCOUNT_INFO;
            }
            else if (!strError.compare("扣款超时") )
            {
                byExecuteCode = OP_DEBIT_OT;
            }
            else if (!strError.compare("密码错误") )
            {
                byExecuteCode = OP_PSW_ERR;
            }
            else if (!strError.compare("参数错误") )
            {
                byExecuteCode = OP_PARAM_ERR;
            }
            else if (!strError.compare("上次交易未结束") )
            {
                byExecuteCode = OP_LAST_TRADE_NOT_END;
            }
            else if (!strError.compare("组件未初始化") )
            {
                byExecuteCode = OP_COM_NOT_INIT;
            }
            else if (!strError.compare("组件校验非法") )
            {
                byExecuteCode = OP_COM_ILLEGAL;
            }
            else if (!strError.compare("支付平台网络故障") )
            {
                byExecuteCode = OP_PAY_PLATFORM_ERR;
            }
            else if( !strError.compare("与下位机通讯异常") )
            {
                byExecuteCode = OP_DISCONNECT_TERMINAL;
            }
        }
            break;
        }

        bSaveError = true;

    }

    //保存日志
    if (!strLog.isEmpty() && bSaveError)
    {
        GetGlobalApp()->WriteLogFile(strLog);
    }

    //通知业务管理线程已经获取硬件回馈业务数据
    QThread::usleep(800);
    GetGlobalApp()->NotifyCaller(byCmdType,byExecuteCode);

}


/***************************************************
 * 函数名称：ResourceDestroy
 * 函数描述：资源销毁
 * 函数参数：void
 * 函数返回: void
 * ************************************************/
void CThreadDevice::ResourceDestroy(void)
{
    QString strLog;

    try
    {
        StopTimer(TIMER_ID_INITCONFIG_DEVICE);
        StopTimer(TIMER_ID_THREAD_DEVICE);
        StopTimer(TIMER_ID_INIT_DEVICE);

        if(NULL == m_pHTSDDeviceDll)
            throw -1;
        if(!m_pHTSDDeviceDll->Destroy())
            throw -2;

        strLog  = "[设备控制线程],成功注销资源";
        //保存日志
        GetGlobalApp()->WriteLogFile(strLog);

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog  = "[设备控制线程],异常硬件设备没有初始化";
            break;
        case -2:
            strLog  = "[设备控制线程],注销资源异常";
            break;
        }
        GetGlobalApp()->WriteLogFile(strLog);
    }

}

bool CThreadDevice::TmpFillDebitResult(QString &ptr)
{
    if(ptr.isEmpty())
    {
        return false;
    }
    Json::Reader reader;
    Json::Value root, context, data, tradeitem;

    QString strResult(ptr);
    if (!reader.parse(strResult.toStdString().c_str(), root))
    {
        return false;
    }
    context = root["DebitResultContext"];
    data = context["Data"];
    tradeitem = data["TradeKeyItem"];

    if (tradeitem["DebitTime"].type() != Json::stringValue)
        return false;
    GetGlobalApp()->m_sCurrentDebitResult.DebitTime =  tradeitem["DebitTime"].asCString();

    if ( tradeitem["PayPlatformType"].type() != Json::intValue)
        return false;
    GetGlobalApp()->m_sCurrentDebitResult.PayPlatformType = ( tradeitem["PayPlatformType"].asInt());


    if (tradeitem["TradeSecurityCode"].type() != Json::stringValue)
        return false;
    strncpy(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode, tradeitem["TradeSecurityCode"].asCString(), 40);


    if ( tradeitem["DebitOrder"].type() != Json::stringValue)
        return false;
    strncpy(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder,  tradeitem["DebitOrder"].asCString(), 40);


    if (tradeitem["PayIdentifier"].type() != Json::stringValue)
        return false;
    strncpy(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier, tradeitem["PayIdentifier"].asCString(), 40);


    if (tradeitem["ConsumeTime"].type() != Json::intValue)
        return false;
    GetGlobalApp()->m_sCurrentDebitResult.ConsumeTime = (tradeitem["ConsumeTime"].asInt());

    if(Json::stringValue != data["ExecuteDesc"].type())
        return false;
    strncpy(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc, data["ExecuteDesc"].asCString(), 64);

    if(Json::intValue  !=  data["ExecuteCode"].type())
        return false;
    GetGlobalApp()->m_sCurrentDebitResult.ExecuteCode = data["ExecuteCode"].asInt();

    if(Json::intValue != data["ResultType"].type())
        return false;
    GetGlobalApp()->m_sCurrentDebitResult.ResultType = data["ResultType"].asInt();

    return true;
}



bool CThreadDevice::InitConfigDevice(void)
{
    bool bReturn	= false;
    bool bSaveError = false;

    unsigned char  byCmdType		= RDM_DEVICE_INIT;
    unsigned char  byExecuteCode	= OP_FAILED;
    QString strLog;

    try
    {
        QString strDataString;
        Json::Value root;
        Json::Value DeviceParamContext;
        Json::Value Data;

        Data["ProvinceID"]			= Json::Value(m_sTranstParam.ProvinceID);
        Data["AreaID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.AreaID);
        Data["RoadID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.RoadID);
        Data["StationID"]			= Json::Value(m_sTranstParam.LaneConfigInfo.str_StationID);
        Data["LaneID"]				= Json::Value(m_sTranstParam.LaneConfigInfo.LaneID);
        Data["StationName"]			= Json::Value(m_sTranstParam.LaneConfigInfo.StationTab);
        Data["ServerType"]			= Json::Value(m_sTranstParam.LaneConfigInfo.STServerTpye);
        Data["ServerIP"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerIP);
        Data["ServerPort"]			= Json::Value(m_sTranstParam.LaneConfigInfo.WebServerPort);

        Data["ConfigLocalIPMode"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIPMode);
        Data["ConfigLocalIP0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP0);
        Data["ConfigLocalSub0"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub0);
        Data["ConfigLocalGate0"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate0);
        Data["ConfigLocalIP1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalIP1);
        Data["ConfigLocalSub1"]		= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalSub1);
        Data["ConfigLocalGate1"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalGate1);
        Data["ConfigLocalDefGate"]	= Json::Value(m_sTranstParam.LaneConfigInfo.ConfigLocalDefGate);
        Data["DevicePayMode"]		= Json::Value(3);
        QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        Data["CurrentTime"]			= Json::Value(strTime.toStdString().c_str());
        Data["UpdatePort"]                      = Json::Value(m_sTranstParam.LaneConfigInfo.AutoUpdatePort);

        Data["DeviceDllVersion"]			= Json::Value( GetGlobalApp()->m_strDeviceDllVersion.toStdString().c_str());
        Data["NetPayDllVersion"]			= Json::Value(GetGlobalApp()->m_strNetPayDllVersion.toStdString().c_str());

        DeviceParamContext["Data"]		= Data;
        DeviceParamContext["Version"]	= Json::Value(("1.0"));
        root["DeviceParamContext"]		= DeviceParamContext;

        Json::FastWriter fast_writer;
        strDataString = QString(fast_writer.write(root).c_str());

        if (NULL == m_pHTSDDeviceDll)
            throw - 1;

        strLog = QString("[设备控制线程],设备初始化操作,初始化参数: %1").arg( strDataString);
        theApp.WriteLogFile(strLog);
        if (!m_pHTSDDeviceDll->InitConfigDevice(strDataString))
            throw - 2;

        strLog = (("[设备控制线程],设备初始化操作完成"));

        bReturn = true;
        //标识执行成功
        byExecuteCode	= OP_SUCCESS;
    }
    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            strLog = (("[设备控制线程],通讯模块动态库异常,指针为空"));
            break;
        case -2:
        {
            std::string strError("未知");
            m_pHTSDDeviceDll->GetLastErrorDesc(strError);
            strLog = QString("[设备控制线程],执行设备初始化操作失败，错误原因描述：%1").arg(strError.c_str());
        }
            break;
        }

        bSaveError = true;
    }
    //保存日志
    if (!strLog.isEmpty() && bSaveError)
    {
        GetGlobalApp()->WriteLogFile(strLog);
    }

    //通知业务管理线程已经获取硬件回馈业务数据
    GetGlobalApp()->NotifyCaller(byCmdType, byExecuteCode);

    return bReturn;


}



////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::fnGetNotifyFromCommModule
//
// 功能描述：用于获取通讯模块主动回馈的回调函数
//
// 输入参数：int iType
//         ：const std::string& strData
// 输出参数：void
// 编写人员：FFFF
// 编写时间：2017年5月7日
// 修改人员：FFFF
// 修改时间：2017年9月12日
// 函数版本：1.0.0.1
// 备注说明：
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::fnGetNotifyFromCommModule(int iType, const QByteArray& strData)
{
    QString tmpData;
    if(NULL != pThis)
    {
        tmpData = pThis->custructDebitResult2Json(iType, strData);

        pThis->JUSTFORSENDCALLBACK(iType, tmpData);
    }

}

bool CThreadDevice::BreakDebit(const QString& payID)
{
    if (NULL == m_pHTSDDeviceDll)
    {
        GetGlobalApp()->WriteLogFile("句柄无效");
        return false;
    }
    return m_pHTSDDeviceDll->BreakDebit(payID);
}

bool CThreadDevice::Init_Dev()
{
    if (NULL == m_pHTSDDeviceDll)
    {
        GetGlobalApp()->WriteLogFile("句柄无效");
        return false;
    }
    std::string strPortName = theApp.m_PortName;
    int nRate = theApp.m_BaudRate;
    return m_pHTSDDeviceDll->InitDev(strPortName, nRate, fnGetNotifyFromCommModule);
}
bool CThreadDevice::Destroy_Dev()
{
    if (NULL == m_pHTSDDeviceDll)
    {
        GetGlobalApp()->WriteLogFile("句柄无效");
        return false;
    }
    return m_pHTSDDeviceDll->Destroy();
}

void CThreadDevice::JUSTFORSENDCALLBACK(int nType, QString strData)
{
    emit SENDCALLBACKRESULT(nType, strData);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadDevice::OnMsgCallbackResult(int nType, QString dataResult)
//
// 功能描述：处理回调函数返回的消息
//
// 输入参数：int nType
//         ：QString dataResult
// 输出参数：void
// 编写人员：FFFF
// 编写时间：2017年9月12日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：信号源 SENDCALLBACKRESULT
//
////////////////////////////////////////////////////////////////////////////////////////////////
void CThreadDevice::OnMsgCallbackResult(int nType, QString dataResult)
{
    unsigned char  byCmdType = RDM_NONE;
    unsigned char  byExecuteCode = OP_FAILED;
    switch(nType)
    {
    case RCC_FUN_DEBIT: //扣款结果
        //关闭扣款超时定时器
        if(NULL != GetGlobalApp()->m_pThreadManage)
        {
            GetGlobalApp()->m_pThreadManage->StopTimer(TIMER_ID_DEBIT_MONEY);
        }

        GetGlobalApp()->m_bIsTrading = false;
        byCmdType = RCC_FUN_DEBIT;
        byExecuteCode = OP_SUCCESS;
        TmpFillDebitResult(dataResult);
        GetGlobalApp()->m_sCurrentDebitResult.Valid = true;

        break;

    case RCC_FUN_CANCEL://撤单结果
        byCmdType = RCC_FUN_CANCEL;
        byExecuteCode = OP_SUCCESS;
        break;

    case RCC_FUN_DEVICE_INIT: //初始化成功回馈
        byCmdType = RCC_NONE;
        byExecuteCode = OP_FAILED;
        m_bInitDebviceSuccessed = true; //标识初始化成功

        break;

    default:
        break;
    }

    //通知业务管理线程已经获取硬件回馈业务数据
    GetGlobalApp()->NotifyCaller(byCmdType, byExecuteCode);

}

void CThreadDevice::SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString wParam, int lParam)
{
    emit WM_THREAD_DEVICE_SHOW_DEAL_INFO(wParam, lParam);
}


void CThreadDevice::SEND_WM_THREAD_DEVICE_START_DEBIT_FLOW(QString wParam, int lParam)
{
    emit WM_THREAD_DEVICE_START_DEBIT_FLOW( wParam,  lParam);
}


void CThreadDevice::SEND_WM_THREAD_DEVICE_DEBIT_CANCEL(QString wParam, int lParam)
{
    emit WM_THREAD_DEVICE_DEBIT_CANCEL( wParam,  lParam);
}

QString CThreadDevice::custructDebitResult2Json(int iType, const QByteArray &strData)
{
    QString strReturn = "";
    if(iType == RCC_FUN_DEBIT)
    {
 //       GetGlobalApp()->WriteLogFile("扣款结果数据块转Json B301");

        QByteArray array = strData;

        QByteArray ResultArray;
        ResultArray = array.mid(0,4);

        QByteArray TradeSecurityCodeArray;
        TradeSecurityCodeArray = array.mid(4,40);

        QByteArray PayIdentifierArray;
        PayIdentifierArray = array.mid(44,40);

        QByteArray DebitOrderArray;
        DebitOrderArray = array.mid(84,32);

        QByteArray DebitTimeArray;
        DebitTimeArray = array.mid(116,4);
        bool ok;
        int iDebitTime = DebitTimeArray.toHex().toInt(&ok,16);
        QString strDebitTime = QDateTime::fromTime_t(iDebitTime).toString("yyyy-MM-dd hh:mm:ss");

        QByteArray PayPlatformTypeArray;
        PayPlatformTypeArray = array.mid(120,2);

        QByteArray ConsumeTimeArray;
        ConsumeTimeArray = array.mid(122,4);

    //    GetGlobalApp()->WriteLogFile("构造扣费结果Json");

        Json::FastWriter write;
        Json::Value root;
        Json::Value DebitResultContext;
        Json::Value Data;
        Json::Value TradeKeyItem;


        Data["ExecuteCode"] = Json::Value(ResultArray.toHex().toInt(&ok,16));
        Data["ExecuteDesc"] = Json::Value("");
        Data["ResultType"] = Json::Value(0);


        TradeKeyItem["ConsumeTime"] = Json::Value(ConsumeTimeArray.toHex().toInt(&ok,16));
        TradeKeyItem["DebitOrder"] = Json::Value(std::string(DebitOrderArray));
        TradeKeyItem["DebitTime"] = Json::Value(strDebitTime.toStdString());
        TradeKeyItem["PayIdentifier"] = Json::Value(std::string(PayIdentifierArray));
        TradeKeyItem["PayPlatformType"] = Json::Value(PayPlatformTypeArray.toHex().toInt(&ok,16));
        TradeKeyItem["TradeSecurityCode"] = Json::Value(std::string(TradeSecurityCodeArray));


        Data["TradeKeyItem"] = TradeKeyItem;
        Data["Version"] = "1.0";
        DebitResultContext["Data"] = Data;
        root["DebitResultContext"] = DebitResultContext;

        strReturn = QString::fromStdString(write.write(root));

        GetGlobalApp()->WriteLogFile("DebitResultJson: " + strReturn);
    }

    return strReturn;
}

