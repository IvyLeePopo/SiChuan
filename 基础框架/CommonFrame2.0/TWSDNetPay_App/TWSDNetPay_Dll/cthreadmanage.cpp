#include "cthreadmanage.h"
#include "ctwsdnetpay_dllapp.h"
#include "paydbinf.h"
#include "common/twsconfig.h"
#include "common/twsutils.h"
#include "upay/upaytmc.h"

CThreadManage::CThreadManage(void)
{

    //  m_pDVJsonDebitCheck  = new CDVJsonDebitCheck();
    m_pDVJsonDebitCheck  = NULL;
    m_AES = NULL;
    m_debitCancelPayIdFromDB.clear();

    connect(this,SIGNAL(WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString)),this,SLOT(OnMsgShowInfo(QString)));
    connect(this,SIGNAL(WM_THREAD_MANAGE_START_DEBIT_FLOW(QString)),this,SLOT(OnMsgManageStartDebitFlow(QString)));
    connect(this,SIGNAL(WM_THREAD_MANAGE_ONLY_DEBIT_MONEY(void)),this,SLOT(OnMsgOnlyDebitMoney(void)));
    connect(this,SIGNAL(WM_THREAD_DEVICE_STOP_GET_DATA(void)),this,SLOT(OnMsgStopGetAccountInfo(void)));
    connect(this,SIGNAL(WM_THREAD_DEVICE_START_GET_DATA(int,int)),this,SLOT(OnMsgStartGetAccountInfo(int,int)));
    connect(this,SIGNAL(WM_THREAD_MANAGE_DEBIT_CANCEL(QString)),this,SLOT(OnMsgDebitCancel(QString)));

    connect(this,SIGNAL(WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE(void)),this,SLOT(OnMsgCheckDebitPasswordState()));

    //定时器连接
    connect(&m_nTimerIDForScanCode,SIGNAL(timeout()),this,SLOT(TimerIDForScanCodeProc()));
    connect(&m_nTimerIDForDebitMoney,SIGNAL(timeout()),this,SLOT(TimerIDForDebitMoneyProc()));
    connect(&m_nTimerIDForCheckDebitState,SIGNAL(timeout()),this,SLOT(TimerIDForCheckDebitProc()));

    // m_bNeedPWD = true;
}

/***************************************************
 * 函数名称：StartTimer
 * 函数功能：设置定时器
 * 参   数：nEventID  dwDelayTime
 * 返   回：void
 * *************************************************/
void CThreadManage::StartTimer( const quint32& nEventID, const quint64& dwDelayTime)
{
    switch(nEventID)
    {
    case TIMER_ID_DEBIT_MONEY://扣款流程定时器
        m_nTimerIDForDebitMoney.start(dwDelayTime);

        break;
    case TIMER_ID_SCAN_CODE://获取用户信息定时器
        m_nTimerIDForScanCode.start(dwDelayTime);
        break;
    case TIMER_ID_DEBIT_CHECK://扣款状态查询定时器
        m_nTimerIDForCheckDebitState.start(dwDelayTime);
        break;
    }
}


/***************************************************
 * 函数名称：StopTimer
 * 函数功能：停止定时器
 * 参   数：nEventID
 * 返   回：void
 * *************************************************/
void CThreadManage::StopTimer( const quint32& nEventID)
{
    switch (nEventID)
    {
    //扣款流程定时器
    case TIMER_ID_DEBIT_MONEY:
        m_nTimerIDForDebitMoney.stop();
        break;

        //获取用户信息定时器
    case TIMER_ID_SCAN_CODE:
        m_nTimerIDForScanCode.stop();
        break;

        //扣款状态查询定时器
    case TIMER_ID_DEBIT_CHECK:
        m_nTimerIDForCheckDebitState.stop();
        break;
    }
}



/***************************************************
 * 函数名称：StartWorkThreads
 * 函数功能：启动工作线程
 * 参   数：tagTranstParam
 * 返   回：void
 * *************************************************/
const bool CThreadManage::StartWorkThreads( const tagTranstParam& sTranstParam)
{
    QString strLog;
    bool bReturn = false;
    try
    {
        qDebug()<<strLog;
        theApp.RecordLog(strLog);
        /*******************启动相应线程*******************/
        //启动支付子平台通讯线程
        QString strConfigFilePath;
        m_pThreadHttp = new CThreadHttp();
        if(NULL == m_pThreadHttp)
            throw -1;

        tagTranstParam sTranstParam = {0};
        memcpy(&sTranstParam,&m_sTranstParam,sizeof(tagTranstParam));
        strConfigFilePath = GetGlobalApp()->m_strWorkDir;
        m_pThreadHttp->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadHttp->SetParam(&sTranstParam);
        m_pThreadHttp->Init();

        // start upay tmc thread
        m_pUpayThread = UpayTMC::instance();

        // start deviceMgtThread
        m_deviceMgtThread = DeviceMgtThread::instance();
        m_deviceMgtThread->setHttpContext(m_pThreadHttp);
        m_deviceMgtThread->start();

        strLog = "m_pThreadHttp->Init()";
        qDebug()<<strLog;

        connect(this,SIGNAL(WM_THREAD_HTTP_SEND_PostQuery(QString *)),m_pThreadHttp,SLOT(onMsgPostQueryData(QString *)));
        connect(this,SIGNAL(WM_THREAD_HTTP_SEND_PostCancel(QString *, bool)),m_pThreadHttp,SLOT(onMsgPostCancelData(QString *, bool)));
        connect(this,SIGNAL(WM_THREAD_HTTP_SEND_PostCharging(QString *)),m_pThreadHttp,SLOT(onMsgPostChargingData(QString *)));
        connect(this,SIGNAL(WM_THREAD_HTTP_SEND_SecondPostCharging(QString)),m_pThreadHttp,SLOT(onMsgSecondPostChargingData(QString)));
        connect(this,SIGNAL(WM_THREAD_HTTP_SEND_GetHeartbeat(void)),m_pThreadHttp,SLOT(onMsgGetHeartbeat(void)));

        connect(m_pThreadHttp,SIGNAL(WM_THREAD_HTTP_ARRIVAL_DATA(QString,int)),this,SLOT(HttpDataReception(QString,int)));
        connect(m_pThreadHttp,SIGNAL(WM_THREAD_HTTP_REPLY(QNetworkReply*, int)),this,SLOT(slHttpReplyReception(QNetworkReply*, int)));

        // message to/from http thread
        connect(m_pThreadHttp, SIGNAL(WM_THREAD_HTTP_QUICKPASS_DATA(QString, int)), m_pUpayThread, SLOT(slMessageFromHttpThread(QString,int)));
        connect(m_pUpayThread, SIGNAL(sigSignIn2HttpThread(QString)), m_pThreadHttp, SLOT(onMsgPostSignInData(QString)));
     //   connect(m_pUpayThread, SIGNAL(sigCorrect2HttpThread(QString)), m_pThreadHttp, SLOT(onMsgPostCorrectData(QString)));

        // inform pos trigger correct lastDeal
        // quickpass support
        connect(this, SIGNAL(WM_THREAD_QUICKPASS_CORRECT(QString)), m_pUpayThread, SLOT(slRequestCorrectOperation(QString)));
        connect(m_pUpayThread, SIGNAL(sigCorrectLastdeal2MGR(QString, QString)), this, SLOT(slCorrectLastdealFromPOS(QString, QString)));

        //链接数据库
        PayDBInf::instance();

        connect(m_pThreadHttp, SIGNAL(WM_THREAD_HTTP_ARRIVAL_HEARTBEAT(QString)), this, SLOT(slHttpDataHeartBeat(QString)));

        strLog = "m_pThreadDevice->Init()";
        qDebug()<<strLog;

        //启动硬件控制线程
        m_pThreadDevice = new CThreadDevice();
        if(NULL == m_pThreadDevice)
            throw -2;
        strConfigFilePath = GetGlobalApp()->m_strWorkDir;
        m_pThreadDevice->SetLocalConfigFilePath(strConfigFilePath);
        m_pThreadDevice->SetParam(&sTranstParam);
        if(!m_pThreadDevice->Init())
            throw -3;

        connect(this,SIGNAL(WM_THREAD_DEVICE_START_GET_DATA_TODEVICE(int)),m_pThreadDevice,SLOT(OnMsgStartGetData(int)));
        connect(this,SIGNAL(WM_THREAD_DEVICE_STOP_GET_DATA_TODEVICE(void)),m_pThreadDevice,SLOT(OnMsgStopGetData(void)));
        connect(this,SIGNAL(WM_THREAD_DEVICE_SHOW_DEAL_INFO_TODEVICE(QString)),m_pThreadDevice,SLOT(OnMsgShowDealInfo(QString)));
        connect(this,SIGNAL(WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(int,QString)),m_pThreadDevice,SLOT(OnMsgControlResultMMI(int,QString)));
        connect(this,SIGNAL(WM_THREAD_DEVICE_START_DEBIT_FLOW_TODEVICE(QString)),m_pThreadDevice,SLOT(OnMsgStartDebitFlow(QString)));
        connect(this,SIGNAL(WM_THREAD_DEVICE_CLEAR_TODEVICE(int, QString)),m_pThreadDevice,SLOT(OnMsgControlResultMMI(int, QString)));

        //沉默200毫秒
        QThread::msleep(100);
        ::qRegisterMetaType<tagAccountInfo>("tagAccountInfo");
        connect(m_pThreadDevice,SIGNAL( WM_THREAD_DEVICE_MS_GETDATA_NOTIFY(int,tagAccountInfo*)),this,SLOT(OnMsgReceiveAccountInfo(int,tagAccountInfo*)));

        //消息回调函数启动
        m_callbackThread.start();

        strLog = "业务管理线程初始化完毕";
        qDebug()<<strLog;

        //沉默200毫秒
        QThread::msleep(200);

        //日志检查定时器
        m_LogCheckDeleteTimer = new QTimer();
        connect(m_LogCheckDeleteTimer,SIGNAL(timeout()),this,SLOT(CheckAndDeleteLogFile()));
        CheckAndDeleteLogFile();
        m_LogCheckDeleteTimer->start(LOGOUTCHECKTIMER);

        strLog = "业务管理日志初始化完毕";
        qDebug()<<strLog;

        m_pUpayThread->triggerFirstSignIn();
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "业务管理线程初始化，异常错误为：HTTP线程初始化错误";
            break;
        case -2:
            strLog = "业务管理线程初始化，异常错误为：硬件控制线程初始化错误";
            break;
        case -3:
            strLog = "业务管理线程初始化，异常错误为：与底层通信消息队列初始化错误";
            break;
        case -4:
            strLog = "业务管理线程初始化，异常错误为：与上层通信消息队列初始化错误";
            break;
        }
        qDebug()<<strLog;
    }
    return bReturn;
}


// update TransParam
void CThreadManage::updateTransParam(const tagTranstParam& sTranstParam)
{
    m_pThreadHttp->SetParam(&sTranstParam);
    m_pThreadHttp->dumpSpecailConfig();
    m_pThreadDevice->SetParam(&sTranstParam);
}


/***************************************************
 * 函数名称：SetAccountDataReadyState
 * 函数功能：设置用户数据是否准备就绪
 * 参   数：nEventID
 * 返   回：void
 * *************************************************/
void CThreadManage::SetAccountDataReadyState( const bool& bState)
{
    m_bAccountInfoIsReady = bState;
    if(!bState)
    {
        memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
    }
}


/***************************************************
 * 函数名称：ContructDebitMoneyInfo
 * 函数功能：构造发送到支付子平台的数据
 * 参   数：const tagAccountInfo* pAccountInfo -- 智能硬件获取到的用户信息
 * 返   回：void
 * *************************************************/
bool CThreadManage::ContructDebitMoneyInfo( const tagAccountInfo* pAccountInfo)
{
    bool bReturn = false;

    CDVJsonDebit* pDVJsonDebit = NULL;

    try
    {
        if(NULL == pAccountInfo)
            throw -1;
        //填充扣款信息结构体
        pDVJsonDebit = new CDVJsonDebit();
        if(NULL == pDVJsonDebit)
            throw -2;
        // QString strPayId = GetGlobalApp()->GetGuidString(1);
        QString strTemp;

        //标识是否测试数据
        pDVJsonDebit->TestingData			= GetGlobalApp()->m_bIsTestData;
        //获取软件版本号
        pDVJsonDebit->NetPayDllVersion		        = GetGlobalApp()->m_strNetPayDllVersion;
        pDVJsonDebit->DeviceDllVersion		        = GetGlobalApp()->m_strDeviceDllVersion;

        //填充报文头信息
        pDVJsonDebit->Version				= "1.0";
        pDVJsonDebit->PackageType			= JSON_DT_SEND_DIBIT;
        pDVJsonDebit->Token					= "";
        pDVJsonDebit->ProvinceID			= m_sTranstParam.ProvinceID;
        pDVJsonDebit->SubmitCount		= 0x01;
        pDVJsonDebit->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);;
        pDVJsonDebit->RecordMatchCode		        = GetNetPayRecordMatchCode();
        pDVJsonDebit->SMVirtualCardID       =    GetGlobalApp()->m_sCurrentDebitInfo.CardID;
        pDVJsonDebit->TradeSecurityCode		        = "";
        pDVJsonDebit->PayIdentifier			= GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;

        //填充基本信息
        pDVJsonDebit->QRCodeData				= pAccountInfo->DataContext;
        pDVJsonDebit->QRCodeType				= 0x00;
        pDVJsonDebit->IDBindInPayPlatform		        = "";
        pDVJsonDebit->IDBindInPayPlatformType	                = pAccountInfo->SrcType;
        pDVJsonDebit->DebitType					= JSON_CMD_TYPE_DEBIT_ONLY_PAY;

        pDVJsonDebit->DebitMoney                                = GetGlobalApp()->m_sCurrentDebitInfo.Money;

        pDVJsonDebit->DebitTime					= GetGlobalApp()->m_sCurrentDebitInfo.SubTime;
        pDVJsonDebit->DebitTollDate				= GetGlobalApp()->m_sCurrentDebitInfo.TollDate;
        pDVJsonDebit->DebitTollShift			= GetGlobalApp()->m_sCurrentDebitInfo.ShiftID;
        pDVJsonDebit->AccountInfoSrcType		= uchar(pAccountInfo->DataType);
        pDVJsonDebit->AccountInfoDeviceID		= uchar(pAccountInfo->DataType>>8);
        pDVJsonDebit->AccountInfoDataType		= uchar(pAccountInfo->DataType);

        //填充附加信息
        pDVJsonDebit->EntryProvinceID		        = m_sTranstParam.ProvinceID;
        pDVJsonDebit->EntryAreaID			= GetGlobalApp()->m_sCurrentDebitInfo.EntryAreaID;
        pDVJsonDebit->EntryRoadID			= GetGlobalApp()->m_sCurrentDebitInfo.EntryRoadID;
        pDVJsonDebit->EntryStationID		        = GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID;
        pDVJsonDebit->EntryStationName		        = GetGlobalApp()->m_sCurrentDebitInfo.EntryStationName;
        pDVJsonDebit->EntryLane				= GetGlobalApp()->m_sCurrentDebitInfo.EntryLane;
        pDVJsonDebit->EntryTime				= GetGlobalApp()->m_sCurrentDebitInfo.EntryTime;
        if(pDVJsonDebit->EntryStationName.isEmpty())
        {
            pDVJsonDebit->EntryStationName.sprintf("%07s",pDVJsonDebit->EntryStationID.toStdString().data());
        }

        //weight
        pDVJsonDebit->TotalWeight           = GetGlobalApp()->m_sCurrentDebitInfo.TotalWeight;
        pDVJsonDebit->LimitWeight           = GetGlobalApp()->m_sCurrentDebitInfo.LimitWeight;

        pDVJsonDebit->PayProvinceID			= m_sTranstParam.ProvinceID;
        //        pDVJsonDebit->PayStationID			= m_sTranstParam.LaneConfigInfo.StationID;
        pDVJsonDebit->PayStationID          = TWSConfig::instance()->getPayStationId();
        pDVJsonDebit->PayStationName        = QString(m_sTranstParam.LaneConfigInfo.StationTab);
        pDVJsonDebit->PayLane				= m_sTranstParam.LaneConfigInfo.LaneID;
        pDVJsonDebit->PayRoadID				= m_sTranstParam.LaneConfigInfo.RoadID;

        //OperatorId/OperatorName
        pDVJsonDebit->PayOperatorID         = GetGlobalApp()->m_sCurrentDebitInfo.OperatorID;
        pDVJsonDebit->PayOperatorName       = GetGlobalApp()->m_sCurrentDebitInfo.OperatorName;

        pDVJsonDebit->VehicleClass			= GetGlobalApp()->m_sCurrentDebitInfo.VehicleClass;
        pDVJsonDebit->VehicleType			= GetGlobalApp()->m_sCurrentDebitInfo.VehicleType;
        pDVJsonDebit->VehicleLicense		        = GetGlobalApp()->m_sCurrentDebitInfo.VehicleLicense;
        pDVJsonDebit->VehicleColor			= GetGlobalApp()->m_sCurrentDebitInfo.VLColor;
        pDVJsonDebit->AxisCount				= GetGlobalApp()->m_sCurrentDebitInfo.AxisCount;

        theApp.m_sCurrentDebitInfo.m_payPlatformType = pDVJsonDebit->AccountInfoDataType;
        theApp.m_sCurrentDebitInfo.m_qrCodeData = pDVJsonDebit->QRCodeData;
        theApp.m_sCurrentDebitInfo.m_consDebitTime = QDateTime::currentDateTime().toTime_t();

        //发送扣款请求
        QString* t_strSendJsonData = NULL;

        t_strSendJsonData =  new QString();

        if(t_strSendJsonData != NULL)
        {

            *t_strSendJsonData = m_pJsonIF.FormatSendData2Json(pDVJsonDebit);
            emit WM_THREAD_HTTP_SEND_PostCharging(t_strSendJsonData);
            //标识处理超时
            m_dwCheckDebitAccumulateTime = 0x00;
            //标记成功
            bReturn = true;
        }
    }
    catch(int& iErrorCode)
    {

    }

    if(pDVJsonDebit != NULL)
    {
        delete pDVJsonDebit;
        pDVJsonDebit = NULL;
    }

    return bReturn;
}




/***************************************************
 * 函数名称：CThreadManage::ContructDebitMoneyInfo
 * 函数功能：构造发送到支付子平台的数据
 * 参   数：void
 * 返   回：void
 * *************************************************/
bool CThreadManage::ContructDebitMoneyInfo(void)
{
    bool bReturn = false;
    try
    {
        //填充扣款信息结构体
        CDVJsonDebit* pDVJsonDebit = new CDVJsonDebit();
        if(NULL == pDVJsonDebit)
            throw -1;
        // QString strPayId = GetGlobalApp()->GetGuidString(1);
        QString strTemp;

        //标识是否测试数据
        pDVJsonDebit->TestingData			= ::GetGlobalApp()->m_bIsTestData;
        //获取软件版本号
        pDVJsonDebit->NetPayDllVersion		= ::GetGlobalApp()->m_strNetPayDllVersion;
        pDVJsonDebit->DeviceDllVersion		= ::GetGlobalApp()->m_strDeviceDllVersion;

        //填充报文头信息
        pDVJsonDebit->Version				= "1.0";
        pDVJsonDebit->PackageType			= JSON_DT_SEND_DIBIT;
        pDVJsonDebit->Token					= "";
        pDVJsonDebit->ProvinceID			= m_sTranstParam.ProvinceID;
        pDVJsonDebit->SubmitCount			= 0x01;
        pDVJsonDebit->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
        pDVJsonDebit->RecordMatchCode		= GetNetPayRecordMatchCode();
        pDVJsonDebit->TradeSecurityCode		= "";
        pDVJsonDebit->PayIdentifier			= GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;

        //填充基本信息
        pDVJsonDebit->QRCodeData				= GetGlobalApp()->m_sCurrentDebitInfo.PayCertificateCode;
        pDVJsonDebit->QRCodeType				= 0x00;
        pDVJsonDebit->IDBindInPayPlatform		= "";
        pDVJsonDebit->IDBindInPayPlatformType	= GetGlobalApp()->m_sCurrentDebitInfo.m_payPlatformType;
        pDVJsonDebit->DebitType					= JSON_CMD_TYPE_DEBIT_ONLY_PAY;

        pDVJsonDebit->DebitMoney                = GetGlobalApp()->m_sCurrentDebitInfo.Money;


        pDVJsonDebit->DebitTime					= GetGlobalApp()->m_sCurrentDebitInfo.SubTime;
        pDVJsonDebit->DebitTollDate				= GetGlobalApp()->m_sCurrentDebitInfo.TollDate;
        pDVJsonDebit->DebitTollShift			= GetGlobalApp()->m_sCurrentDebitInfo.ShiftID;
        pDVJsonDebit->AccountInfoSrcType		= uchar(GetGlobalApp()->m_sCurrentDebitInfo.PayCertificateType);
        pDVJsonDebit->AccountInfoDeviceID		= uchar(GetGlobalApp()->m_sCurrentDebitInfo.PayCertificateType>>8);
        pDVJsonDebit->AccountInfoDataType		= 0x01;

        //填充附加信息
        pDVJsonDebit->EntryProvinceID		= m_sTranstParam.ProvinceID;
        pDVJsonDebit->EntryAreaID			= GetGlobalApp()->m_sCurrentDebitInfo.EntryAreaID;
        pDVJsonDebit->EntryRoadID			= GetGlobalApp()->m_sCurrentDebitInfo.EntryRoadID;
        pDVJsonDebit->EntryStationID		= GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID;
        pDVJsonDebit->EntryStationName		= GetGlobalApp()->m_sCurrentDebitInfo.EntryStationName;
        pDVJsonDebit->EntryLane				= GetGlobalApp()->m_sCurrentDebitInfo.EntryLane;
        pDVJsonDebit->EntryTime				= GetGlobalApp()->m_sCurrentDebitInfo.EntryTime;
        if(pDVJsonDebit->EntryStationName.isEmpty())
        {
            //  pDVJsonDebit->EntryStationName.Format(_T("%07d"),pDVJsonDebit->EntryStationID);
        }
        pDVJsonDebit->EntryRoadID			= GetGlobalApp()->m_sCurrentDebitInfo.EntryRoadID;


        pDVJsonDebit->PayProvinceID			= m_sTranstParam.ProvinceID;
        //        pDVJsonDebit->PayStationID			= m_sTranstParam.LaneConfigInfo.StationID;
        pDVJsonDebit->PayStationID          = TWSConfig::instance()->getPayStationId();
        pDVJsonDebit->PayStationName		= QString(m_sTranstParam.LaneConfigInfo.StationTab);
        pDVJsonDebit->PayLane				= m_sTranstParam.LaneConfigInfo.LaneID;
        pDVJsonDebit->PayRoadID				= m_sTranstParam.LaneConfigInfo.RoadID;

        pDVJsonDebit->VehicleClass			= GetGlobalApp()->m_sCurrentDebitInfo.VehicleClass;
        pDVJsonDebit->VehicleType			= GetGlobalApp()->m_sCurrentDebitInfo.VehicleType;
        pDVJsonDebit->VehicleLicense		= GetGlobalApp()->m_sCurrentDebitInfo.VehicleLicense;
        pDVJsonDebit->VehicleColor			= GetGlobalApp()->m_sCurrentDebitInfo.VLColor;
        pDVJsonDebit->AxisCount				= GetGlobalApp()->m_sCurrentDebitInfo.AxisCount;
        pDVJsonDebit->TotalWeight			= GetGlobalApp()->m_sCurrentDebitInfo.TotalWeight;
        pDVJsonDebit->LimitWeight			= GetGlobalApp()->m_sCurrentDebitInfo.LimitWeight;

        //发送扣款请求

        QString* t_strSendJsonData = NULL;

        t_strSendJsonData =  new QString();

        if(t_strSendJsonData != NULL)
        {
            *t_strSendJsonData = m_pJsonIF.FormatSendData2Json(pDVJsonDebit);
            emit WM_THREAD_HTTP_SEND_PostCharging(t_strSendJsonData);
            //标识处理超时
            m_dwCheckDebitAccumulateTime = 0x00;
            //标记成功
            bReturn = true;
        }

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        }
    }

    return bReturn;
}


/***************************************************
 * 函数名称：ContructDebitCancelInfo
 * 函数功能：构造撤单数据发送到支付子平台
 * 参   数：void
 * 返   回：void
 * *************************************************/
bool CThreadManage::ContructDebitCancelInfo( const QString& strPayIdentifier)
{
    bool bReturn = false;
    try
    {
        //填充扣款信息结构体
        CDVJsonDebitCancel* pDVJsonDebitCancel = new CDVJsonDebitCancel();
        if(NULL == pDVJsonDebitCancel)
            throw -1;
        //  QString strPayId = GetGlobalApp()->GetGuidString(1);
        QString strTemp;
        //填充报文头信息
        pDVJsonDebitCancel->Version				= "1.0";
        pDVJsonDebitCancel->PackageType			= JSON_DT_SEND_DIBIT_CANCEL;
        pDVJsonDebitCancel->Token				= "";
        pDVJsonDebitCancel->ProvinceID			= m_sTranstParam.ProvinceID;
        pDVJsonDebitCancel->SubmitCount			= 0x01;
        pDVJsonDebitCancel->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
        pDVJsonDebitCancel->PayIdentifier		= strPayIdentifier;
        //发送撤单请求

        //通过判断交易识别码与当前扣费是否一样，一样则取消当前扣费请求
        bool beCancel = false;
        if (strPayIdentifier == theApp.m_payIdentifier)
            beCancel = true;

        QString* t_strSendJsonData = NULL;
        t_strSendJsonData =  new QString();
        if(t_strSendJsonData != NULL)
        {
            *t_strSendJsonData = m_pJsonIF.FormatSendData2Json(pDVJsonDebitCancel);
            emit WM_THREAD_HTTP_SEND_PostCancel(t_strSendJsonData, beCancel);

            //标记成功
            bReturn = true;
        }

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            break;
        }
    }

    m_lastPayId4Cancel = strPayIdentifier;

    return bReturn;
}

bool CThreadManage::ContructDebitCancelInfo(const QString& strPayIdentifier, QString contdata)
{
    bool bReturn = false;

    //填充扣款信息结构体
    CDVJsonDebitCancel* pDVJsonDebitCancel = new CDVJsonDebitCancel();

    //填充报文头信息
    pDVJsonDebitCancel->Version				= "1.0";
    pDVJsonDebitCancel->PackageType			= JSON_DT_SEND_DIBIT_CANCEL;
    pDVJsonDebitCancel->Token				= "";
    pDVJsonDebitCancel->ProvinceID			= m_sTranstParam.ProvinceID;
    pDVJsonDebitCancel->SubmitCount			= 0x01;
    pDVJsonDebitCancel->SubmitTime			= GetGlobalApp()->GetCurrentTimeString(1);
    pDVJsonDebitCancel->PayIdentifier		= strPayIdentifier;
    //发送撤单请求

    //通过判断交易识别码与当前扣费是否一样，一样则取消当前扣费请求
    bool beCancel = false;
    if (strPayIdentifier == theApp.m_payIdentifier)
        beCancel = true;

    pDVJsonDebitCancel->m_payPlatformType   = 0x07;     // quickpass platform
    pDVJsonDebitCancel->m_contdata          = contdata;

    QString* t_strSendJsonData = NULL;
    t_strSendJsonData =  new QString();
    if(t_strSendJsonData != NULL)
    {
        *t_strSendJsonData = m_pJsonIF.FormatSendData2Json(pDVJsonDebitCancel);
        emit WM_THREAD_HTTP_SEND_PostCancel(t_strSendJsonData, beCancel);

        //标记成功
        bReturn = true;
    }

    m_lastPayId4Cancel = strPayIdentifier;

    return bReturn;
}


/***************************************************
 * 函数名称：FinishTrade
 * 函数功能：交易结束的处理
 * 参   数：void
 * 返   回：void
 * *************************************************/
QString CThreadManage::FinishTrade( const bool& bNormal)
{
    QString strReturn = GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;

    //停止接收处理支付子平台回馈的数据
    GetGlobalApp()->m_bIsTrading		= false;
    GetGlobalApp()->m_bCurrentDebitMode = false;

    //关闭扣款超时定时器
    StopTimer(TIMER_ID_DEBIT_MONEY);
    //关闭检查扣款状态定时器
    StopTimer(TIMER_ID_DEBIT_CHECK);
    //清除扣款状态查询结构体
    if(NULL == m_pDVJsonDebitCheck)
    {
        delete m_pDVJsonDebitCheck;
        m_pDVJsonDebitCheck = NULL;
    }

    m_sDVJsonDebit.Reset();

    //超时截至导致的交易结束，需要自动发起撤单
    if(!bNormal)
    {
        //清理扣款相关资源
        GetGlobalApp()->m_sCurrentDebitInfo.Valid	= false;
        GetGlobalApp()->m_sCurrentDebitResult.Valid = false;
        //构造撤单数据
        CDVJsonDebitCancel* pDVJsonDebitCancle = new CDVJsonDebitCancel(&m_sDVJsonDebit);
        if(NULL != pDVJsonDebitCancle)
        {
            pDVJsonDebitCancle->ProvinceID	  = ::GetGlobalApp()->m_sTranstParam.ProvinceID;
            pDVJsonDebitCancle->Version		  = "1.0";
            pDVJsonDebitCancle->SubmitTime	  = GetGlobalApp()->GetCurrentTimeString(1);
            //设置撤单报文
            pDVJsonDebitCancle->PackageType   = JSON_DT_SEND_DIBIT_CANCEL;
            //获取交易识别码
            pDVJsonDebitCancle->PayIdentifier = GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;
            //发送撤单请求
            //   m_pThreadHttp->PostThreadMessage(WM_THREAD_HTTP_SEND_DATA_REQUEST,(WPARAM)pDVJsonDebitCancle,(LPARAM)MAKELONG(JSON_DT_SEND_DIBIT_CANCEL,MAKEWORD(0x00,0x00)));
        }
    }
    //标识扣款流程结束
    if(GetGlobalApp()->m_bCurrentDebitMode)
    {
        GetGlobalApp()->m_iDebitTaskNo = 0x00;
    }

    return strReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CThreadManage::GetNetPayRecordMatchCode
//
// 功能描述：获取互联网支付的交易匹配码
//
// 输入参数：IN const CString& strEncKey -- AES算法密钥
// 输出参数：const qsTRING

// 备注说明：
// 匹配使用到的因子
// 1.入口站号	（7位字符)
// 2.入口时间	（14位字符)
// 3.出口站号	（7位字符)
// 4.出口时间	（14位字符)
// 5.出口车道号	（3位字符）
// 6.虚拟卡号	（20位字符）
// 7.交易金额	（10位字符）
// 使用原则
// 根据序号拼接成一个字符串（长度:75）S，对S进行AES加密（使用密钥K）得到密文D（16进制字符串），
// 对密文D进行MD5运算得到匹配码M
// 校验原则：根据上述方式获得新的匹配码M1，M与M1相等匹配正常
////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************
 * 函数名称：GetNetPayRecordMatchCode
 * 函数功能：获取互联网支付的交易匹配码
 * 参   数：
            // 1.入口站号	（7位字符)
            // 2.入口时间	（14位字符)
            // 3.出口站号	（7位字符)
            // 4.出口时间	（14位字符)
            // 5.出口车道号	（3位字符）
            // 6.虚拟卡号	（20位字符）
            // 7.交易金额	（10位字符）
 * 返   回：void
 * *************************************************/
const QString CThreadManage::GetNetPayRecordMatchCode(const QString& strEncKey)
{

    //校验密钥
    QString strRealKey(strEncKey);
    if(strRealKey.isEmpty())
    {
        switch(this->m_sTranstParam.ProvinceID)
        {
        case 11:
            strRealKey = AES_ENC_KEY_DATA_11;
            break;
        case 12:
            strRealKey = AES_ENC_KEY_DATA_12;
            break;
        case 13:
            strRealKey = AES_ENC_KEY_DATA_13;
            break;
        case 14:
            strRealKey = AES_ENC_KEY_DATA_14;
            break;
        case 15:
            strRealKey = AES_ENC_KEY_DATA_15;
            break;

        case 31:
            strRealKey = AES_ENC_KEY_DATA_31;
            break;
        case 32:
            strRealKey = AES_ENC_KEY_DATA_32;
            break;
        case 33:
            strRealKey = AES_ENC_KEY_DATA_33;
            break;
        case 34:
            strRealKey = AES_ENC_KEY_DATA_34;
            break;
        case 35:
            strRealKey = AES_ENC_KEY_DATA_35;
            break;
        case 36:
            strRealKey = AES_ENC_KEY_DATA_36;
            break;
        case 37:
            strRealKey = AES_ENC_KEY_DATA_37;
            break;

        case 41:
            strRealKey = AES_ENC_KEY_DATA_41;
            break;
        case 42:
            strRealKey = AES_ENC_KEY_DATA_42;
            break;
        case 43:
            strRealKey = AES_ENC_KEY_DATA_43;
            break;
        case 44:
            strRealKey = AES_ENC_KEY_DATA_44;
            break;
        case 45:
            strRealKey = AES_ENC_KEY_DATA_45;
            break;

        case 50:
            strRealKey = AES_ENC_KEY_DATA_50;
            break;
        case 51:
            strRealKey = AES_ENC_KEY_DATA_51;
            break;
        case 52:
            strRealKey = AES_ENC_KEY_DATA_52;
            break;
        case 53:
            strRealKey = AES_ENC_KEY_DATA_53;
            break;
        case 54:
            strRealKey = AES_ENC_KEY_DATA_54;
            break;

        case 61:
            strRealKey = AES_ENC_KEY_DATA_61;
            break;
        case 62:
            strRealKey = AES_ENC_KEY_DATA_62;
            break;
        case 63:
            strRealKey = AES_ENC_KEY_DATA_63;
            break;
        case 64:
            strRealKey = AES_ENC_KEY_DATA_64;
            break;
        case 65:
            strRealKey = AES_ENC_KEY_DATA_65;
            break;
        default:
            //默认密钥
            strRealKey = AES_ENC_KEY_DATA;
            break;
        }
    }
    qDebug()<<"下发省份ID:  "<<this->m_sTranstParam.ProvinceID;
    qDebug()<<"使用秘钥:  "<<strRealKey;
    //构造AES对象
    //    if (m_AES == NULL)
    //    {
    //        m_AES = new CAesECB(strRealKey);
    //    }

    /**************************构造加密明文**************************/
    QString strData;

    //入口站点
    QString strEntryST = TWSUtils::getFormatStrEntryST();
    qDebug()<<"计算后入口站点: "<<strEntryST;

    //入口时间
    QString strEntryTime;
    strEntryTime = GetGlobalApp()->m_sCurrentDebitInfo.EntryTime;
    //  strEntryTime = "2017-08-25 23:32:30";

    qDebug()<<"下发入口时间: "<<strEntryTime;
    strEntryTime.replace(QString("-"),  QString(""));
    strEntryTime.replace(QString("\r"), QString(""));
    strEntryTime.replace(QString("\n"), QString(""));
    strEntryTime.replace(QString(":"),  QString(""));
    strEntryTime.replace(QString(" "),  QString(""));
    qDebug()<<"计算后入口时间: "<<strEntryTime;

    //出口站点
    QString strExitST = TWSUtils::getFormatStrExitST();
    qDebug()<<"计算后出口站点: "<<strExitST;

    //出口时间
    QString strExitTime;
    strExitTime = GetGlobalApp()->m_sCurrentDebitInfo.SubTime;
    //   strExitTime = "2017-08-25 23:32:30";
    qDebug()<<"下发出口时间: "<<strExitTime;
    strExitTime.replace(QString("-"),  QString(""));
    strExitTime.replace(QString("\r"), QString(""));
    strExitTime.replace(QString("\n"), QString(""));
    strExitTime.replace(QString(":"),  QString(""));
    strExitTime.replace(QString(" "),  QString(""));
    qDebug()<<"计算后出口时间: "<<strExitTime;

    //出口车道
    QString strExitLane;
    qDebug()<<"下发出口车道: "<<GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID;
    if(   (44 == GetGlobalApp()->m_sTranstParam.ProvinceID)
          ||(61 == GetGlobalApp()->m_sTranstParam.ProvinceID)
          ||(62 == GetGlobalApp()->m_sTranstParam.ProvinceID)
          ||(63 == GetGlobalApp()->m_sTranstParam.ProvinceID)
          )
    {
        strExitLane.sprintf("%03d",GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
    }
    else
    {
        strExitLane.sprintf("%08d",GetGlobalApp()->m_sTranstParam.LaneConfigInfo.LaneID);
    }
    qDebug()<<"计算后出口车道: "<<strExitLane;

    //虚拟卡号
    QString strVirtualCardID;
    strVirtualCardID = "00000000000000000000";

    if(   (44 == GetGlobalApp()->m_sTranstParam.ProvinceID)
          ||(62 == GetGlobalApp()->m_sTranstParam.ProvinceID)
          )
    {
        strVirtualCardID = "00000000000000000000";
        qDebug()<<" 省份编码为44或62，虚拟卡号默认取值: 00000000000000000000";
    }
    else
    {
        if(GetGlobalApp()->m_sCurrentDebitInfo.CardID.isEmpty())
        {
            strVirtualCardID = "00000000000000000000";
            qDebug()<<"下发卡号为空，虚拟卡号默认取值: 00000000000000000000";
        }
        else
        {
            qDebug()<<"下发虚拟卡号: "<<GetGlobalApp()->m_sCurrentDebitInfo.CardID;
            QString strCardTemp;
            strCardTemp = "00000000000000000000";
            if(GetGlobalApp()->m_sCurrentDebitInfo.CardID.length()<20)
            {

                int iRemainCount = 20 - GetGlobalApp()->m_sCurrentDebitInfo.CardID.length();
                QString leftCardTemp = strCardTemp.left(iRemainCount);
                strVirtualCardID = QString("%1%2").arg(leftCardTemp).arg(GetGlobalApp()->m_sCurrentDebitInfo.CardID);
            }
            else
            {
                QString strVT = GetGlobalApp()->m_sCurrentDebitInfo.CardID;
                strVT = strVT.trimmed();
                strVirtualCardID = strVT;
            }
            qDebug()<<"计算后虚拟卡号: "<<strVirtualCardID;
        }
    }

    //交易金额
    QString strTradeMoney;
    qDebug()<<"下发交易金额: "<<GetGlobalApp()->m_sCurrentDebitInfo.Money;
    strTradeMoney.sprintf("%010d",GetGlobalApp()->m_sCurrentDebitInfo.Money);
    qDebug()<<"计算后交易金额: "<<strTradeMoney;


    strData = strEntryST+strEntryTime+strExitST+strExitTime+strExitLane+strVirtualCardID+strTradeMoney;

    qDebug()<<"整个加密字符串: "<<strData;

    /**************************加密处理**************************/
    //  QString strEncData = m_AES->EncryptData(strData);

    CAesECB  cAes(strRealKey);
    QString strEncData = cAes.EncryptData(strData);

    qDebug()<<"AES计算结果: "<<strEncData;

    QByteArray bytemiwen = QByteArray(strEncData.toLatin1());

    QByteArray byteMatchCode = QCryptographicHash::hash(bytemiwen , QCryptographicHash::Md5 );

    qDebug()<<"HASH计算结果: "<<byteMatchCode.toHex();

    QString strMatchCode(byteMatchCode.toHex());
    byteMatchCode.clear();

    //    qDebug() << "--------------------GetNetPayRecordMatchCode--------------------";
    //    qDebug() << "EntryStationID : " << GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID;
    //    qDebug() << "StationID      : " << QString::fromLocal8Bit(GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);

    //    qDebug() << "--------------------" << this->m_sTranstParam.ProvinceID << "--------------------";
    //    qDebug() << "strEntryST     : " << strEntryST;
    //    qDebug() << "strEntryTime   : " << strEntryTime;
    //    qDebug() << "strExitST      : " << strExitST;
    //    qDebug() << "strExitTime    : " << strExitTime;
    //    qDebug() << "strExitLane    : " << strExitLane;
    //    qDebug() << "strVirtualCardID   : " << strVirtualCardID;
    //    qDebug() << "strTradeMoney  : " << strTradeMoney;

    //    qDebug() << "\n";
    //    qDebug() << "strData        : " << strData;
    //    qDebug() << "strMatchCode   : " << strMatchCode;

    //返回匹配码
    return strMatchCode;
}



/***************************************************
 * 函数名称：OnMsgStartGetAccountInfo
 * 函数功能：接收要求执行启动从硬件获取用户信息的操作
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgStartGetAccountInfo(int wParam,int lParam)
{
    int iOverTime = (int)wParam;
    if(NULL != m_pThreadDevice)
    {
        emit WM_THREAD_DEVICE_START_GET_DATA_TODEVICE(wParam);
    }
}



/***************************************************
 * 函数名称：OnMsgStopGetAccountInfo
 * 函数功能：接收要求执行停止从硬件获取用户信息的操作
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgStopGetAccountInfo(void)
{
    if(NULL != m_pThreadDevice)
    {
        emit WM_THREAD_DEVICE_STOP_GET_DATA_TODEVICE();

        //清除定时器
        StopTimer(TIMER_ID_DEBIT_CHECK);
        StopTimer(TIMER_ID_DEBIT_MONEY);
        //停止接收处理支付子平台回馈的数据
        GetGlobalApp()->m_bIsTrading = false;
        //清理扣款相关资源
        GetGlobalApp()->m_sCurrentDebitInfo.Valid	= false;
        GetGlobalApp()->m_sCurrentDebitResult.Valid = false;
    }
}


/***************************************************
 * 函数名称：OnMsgReceiveAccountInfo
 * 函数功能：获取到用户信息后的处理过程
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgReceiveAccountInfo(int wParam, tagAccountInfo* lParam)
{
    int bGetData	= wParam;
    quint32 dwParam	= 0x00;
    tagAccountInfo* pAccountInfo =NULL;
    QString strLog;
    try
    {
        pAccountInfo = lParam;

        if (pAccountInfo == NULL)
            throw -1;

        //获取用户信息
        if (0 == pAccountInfo->DataSize )
            throw -2;

        //判断当前流程是否允许进行扣款业务处理
        if(!GetGlobalApp()->m_bIsTrading)
            throw -3;

        //标记扣款流程接收到支付凭证
        GetGlobalApp()->m_iDebitTaskNo = 0x02;

        //构造消费数据并通知支付子平台通讯线程开始工作
        ContructDebitMoneyInfo(pAccountInfo);
    }

    catch (int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
        case -2:
            strLog = "获取到用户信息后的处理过程，用户数据长度为异常";
            break;

        case -3:
            strLog = "获取到用户信息后的处理过程，扣款业务正忙";
            break;
        }

        //通知调用者用户数据获取失败
        dwParam = quint32((0x03<<24)+(0x05<<16)+(0x00<<8)+0x00);

        //记录日志
        GetGlobalApp()->RecordLog("[业务管理线程],响应智能硬件控制线程回馈处理完成(获取用户信息)，格式化结果(功能代码:"+QString::number(0x03)+",执行状况:"+QString::number(0x05));
    }
    //清理资源

    //    //通知调用者
    //      char t_ResultsCode[4];
    //      t_ResultsCode[0]=0x00;
    //      t_ResultsCode[1]=0x00;
    //      t_ResultsCode[2]=0x05;
    //      t_ResultsCode[3]=0x03;
    //      m_UpperMange.SendUpperMS(t_ResultsCode,sizeof(t_ResultsCode));

    m_callbackThread.setResults(0x00,0x00);

    if (pAccountInfo != NULL) {
        delete pAccountInfo;
        pAccountInfo = NULL;
    }
}


/***************************************************
 * 函数名称：OnMsgManageStartDebitFlow
 * 函数功能：接收到消息，启动扣款流程（整合操作模式）
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgManageStartDebitFlow(const QString &wParam)
{
    QString strLog;

    try
    {
        //标记扣款流程开始
        GetGlobalApp()->m_iDebitTaskNo = 0x01;

        if(wParam == NULL)
            throw -1;

        if(NULL == m_pThreadDevice)
            throw -2;
        //清除上一次的用户信息
        memset(&m_sAccountInfo,0,sizeof(tagAccountInfo));
        //通知硬件控制线程
        emit WM_THREAD_DEVICE_START_DEBIT_FLOW_TODEVICE(wParam);
        //记录日志
        GetGlobalApp()->RecordLog("[业务管理线程]:外部主动调用扣款操作(整合操作)，交易标识:"+GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier);
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[业务管理线程]: pstrXML is  Empty() ";
            break;
        case -2:
            strLog = "[业务管理线程]: m_pThreadDevice is  NULL ";
            break;
        }
        GetGlobalApp()->RecordLog(strLog);
    }
}

void CThreadManage::slHttpReplyReception(QNetworkReply* reply, int op)
{
    //    qDebug() << "\nCThreadManage::slHttpReplyReception: error: " << reply->error() << "/" << op << "/" << reply->errorString() << "\n";
}

/**************************************************************
 * 函数名称：HttpDataReception
 * 函数描述：HTTP数据接收
 * 函数参数：t_httpData HTTP数据
 *         t_modeType 业务操作类型
 * 函数返回：void
 * ************************************************************/
void CThreadManage::HttpDataReception(QString t_httpData,int t_modeType)
{
    qDebug()<<"\ninto CThreadManage::HttpDataReception:" << t_modeType << "/" << t_httpData;

    int iReturnCode = 0x00;
    QString strLog;
    CJsonOperationIF *pJsonIF = NULL;
    CDVJsonBase* pDVJsonBase = NULL;

    QString payId;
    int errorNo = 0;

    try
    {
        switch(t_modeType)
        {
        case JSON_DT_SEND_DIBIT:
        case JSON_DT_SEND_DIBIT_CHECK:
        case JSON_DT_SEND_DIBIT_CANCEL:
        case JSON_DT_SEND_DIBIT_SECOND:
            pJsonIF = new CJsonDebitOperate();
            break;
        case JSON_DT_SEND_HEART:
            pJsonIF = new CJsonHeartOperate();
            break;
        }

        if(pJsonIF == NULL)
            throw -3;

        // add network Reply error handle
        if (JSON_DT_SEND_DIBIT == t_modeType && GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist) {
            qDebug() << "CThreadManage::HttpDataReception Network error";
            throw -8;
        }

        //解析接收到的JSON数据
        pDVJsonBase = pJsonIF->FormatResponseResult2Class(t_httpData,t_modeType);
        if(pDVJsonBase == NULL)
            throw -4;

        pDVJsonBase->ProvinceID = ::GetGlobalApp()->m_sTranstParam.ProvinceID;

        if(JSON_DT_SEND_DIBIT_CANCEL == t_modeType) {
            payId = ((CDVJsonDebitCancel*)pDVJsonBase)->PayIdentifier;
        }

        //如果是扣费或者查询
        if(   (JSON_DT_SEND_DIBIT	    == t_modeType)
              ||(JSON_DT_SEND_DIBIT_CHECK == t_modeType)
              )
        {
            //返回的代码不在一下列表，则认为内部参数异常
            if(   (HTTP_RC_DEBIT_SUCCESS				!= pDVJsonBase->ServerResponseCode)
                  &&(HTTP_RC_DEBIT_NEED_PASSWORD			!= pDVJsonBase->ServerResponseCode)
                  &&(HTTP_RC_DEBIT_CODE_TIMEOUT			!= pDVJsonBase->ServerResponseCode)
                  &&(HTTP_RC_DEBIT_SRC_ILLGAL		        != pDVJsonBase->ServerResponseCode)
                  &&(HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	        != pDVJsonBase->ServerResponseCode)
                  &&(HTTP_RC_INPUT_PASSWORD_ERROR		!= pDVJsonBase->ServerResponseCode)
                  &&(HTTP_RC_SERVER_ERROR               != pDVJsonBase->ServerResponseCode)
                  )
            {
                //抛出异常
                throw -5;
            }
        }
        else if(JSON_DT_SEND_DIBIT_CANCEL == t_modeType)
        {
            if(HTTP_RC_DATA_VALID != pDVJsonBase->ServerResponseCode)
            {
                //保存撤单信息
                throw -6;
            }
        }
        else
        {
            throw -7;
        }

        int cancelType = PayDBInf::instance()->getCancelTypeById(payId.toStdString());

        // handle cancelType, 类型1为需要回馈
        // 判断是否为缓存发起的撤单，若是则不回馈
        if ((1 == cancelType && JSON_DT_SEND_DIBIT_CANCEL == t_modeType) || \
                JSON_DT_SEND_DIBIT_CANCEL != t_modeType ||                      \
                0 != m_debitCancelPayIdFromDB.compare(payId)) {

            OnMsgDealSubPayPlatformResponse(pDVJsonBase,t_modeType,HTTP_RC_DATA_LEGAL);
        }

        // update DebitCancel status in DB
        if(JSON_DT_SEND_DIBIT_CANCEL == t_modeType) {
            updateDebitCancelById(payId);
        }
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -3://回馈数据失败
            strLog = "[支付通讯线程]:服务器回馈数据失败,回馈业务管理线程代码为:0x03(内部错误)，回馈数据异常，无法解析";
            break;
        case -4://回馈数据显示失败
            strLog = "[支付通讯线程]:服务器回馈数据失败,回馈业务管理线程代码为:0x04(内部错误)，回馈数据异常，无法解析";
            break;
        case -5://扣费或查询，回馈结果非成功或正在输入密码，判定内部错误
            strLog = "[支付通讯线程]:服务器回馈数据格式成功[请求类型:],回馈业务管理线程代码为:0x05(内部错误)，详细描述:";
            break;
        case -6://撤单失败
            strLog = "[支付通讯线程]:服务器回馈数据格式成功(撤单失败）,回馈业务管理线程代码为:0x06(内部错误)";

            break;
        case -7://非法的请求回馈
            strLog = "[支付通讯线程]:服务器回馈数据失败（非法请求）,回馈业务管理线程代码为:0x07(内部错误)";
            break;

        case -8://network timeout
            strLog = "[支付通讯线程]:服务器回馈数据失败（网络异常）,回馈业务管理线程代码为:0x08(网络异常)";
            break;
        }

        int cancelType = PayDBInf::instance()->getCancelTypeById(payId.toStdString());

        // update DebitCancel status in DB
        if(JSON_DT_SEND_DIBIT_CANCEL == t_modeType) {
            int statusHttp = pDVJsonBase->ServerResponseCode;

            // 没找到交易凭证 可能已经超过撤单有效期
            if (13 == statusHttp) {
                if (m_lastPayId4Cancel == theApp.m_payIdentifier) {
                    qDebug() << "~~~~~~~~~~FinishTrade1 HttpDataReception~~~~~~~~~~";
                    FinishTrade(true);

                    IF_SetSingle(4);
                }

                updateDebitCancelById(payId);
            }
        }

        iReturnCode = qAbs(iErrorCode);
        errorNo = iErrorCode;

        //记录日志
        //        GetGlobalApp()->RecordLog(strLog);

        strLog.clear();

        // handle cancelType, 类型1为需要回馈
        // 判断是否为缓存发起的撤单，若是则不回馈
        if (JSON_DT_SEND_DIBIT_CANCEL == t_modeType) {
            if (2 == cancelType || 0 == m_debitCancelPayIdFromDB.compare(payId)) {
                return;
            }
        }

        OnMsgDealSubPayPlatformResponse(pDVJsonBase, t_modeType, iReturnCode);
    }


    if(pJsonIF != NULL)
    {
        delete pJsonIF;
        pJsonIF =NULL;
    }
}

/***************************************************
 * 函数名称：OnMsgDealSubPayPlatformResponse
 * 函数功能：处理支付子平台的回馈信息
 * 参   数：
 *        wOpType:  获得请求数据的类型
 *        wOpState: 获得服务器回馈的结果
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgDealSubPayPlatformResponse(CDVJsonBase* wParam,int wOpType,int wOpState)
{
    qDebug()<<"into CThreadManage::OnMsgDealSubPayPlatformResponse: wOpType=" << wOpType << ", wOpState=" << wOpState;

    QString strLog;
    QString strResponseType;

    //未处于交易当中，不接收回馈
    //    if(   (JSON_DT_SEND_DIBIT	    == wOpType) ||(JSON_DT_SEND_DIBIT_CHECK == wOpType)
    //            )
    {
        if (!GetGlobalApp()->m_bIsTrading)
        {
            qDebug() << "CThreadManage::OnMsgDealSubPayPlatformResponse: 未处于交易当中，不接收回馈";
            return;
        }
    }

    //临时交易状态
    bool bIsInputPassword	= false;
    bool bDebitSuccess		= false;


    CDVJsonBase* pDVJsonBase = wParam;

    CDVJsonDebit* pDVLocalDebit = new CDVJsonDebit();


    uchar byFunCode		= 0x01;
    uchar byExecuteState = RCC_INNER_ERROR;

    try
    {
        //获取业务处理类型
        switch(wOpType)
        {
        case JSON_DT_SEND_DIBIT:
            strResponseType = "扣费请求";
            break;
        case JSON_DT_SEND_DIBIT_CHECK:
            strResponseType = "扣费查询";
            break;
        case JSON_DT_SEND_DIBIT_CANCEL:
            strResponseType = "撤销订单";
            break;
        }

        //获得合法的回馈信息
        if(HTTP_RC_DATA_LEGAL != wOpState)
            throw -1;
        //判断消息回馈类型
        if(    (JSON_DT_SEND_DIBIT			!= wOpType)
               && (JSON_DT_SEND_DIBIT_CHECK	!= wOpType)
               && (JSON_DT_SEND_DIBIT_CANCEL	!= wOpType)
               )
        {
            throw -2;
        }

        //校核指针是否非法
        if( (NULL == pDVJsonBase) || (NULL == pDVLocalDebit) )
            throw -3;


        if(JSON_DT_SEND_DIBIT == wOpType)
        {
            *pDVLocalDebit = *((CDVJsonDebit*)pDVJsonBase);
            m_sDVJsonDebit = *pDVLocalDebit;
        }
        else
        {

            // *pDVLocalDebit = *((CDVJsonBase*)pDVJsonBase);
            *((CDVJsonBase*)pDVLocalDebit) = *pDVJsonBase;
        }


        //只处理扣款/查询回馈（撤单回馈后续处理）
        if (JSON_DT_SEND_DIBIT_CANCEL != wOpType)
        {
            switch(pDVJsonBase->ServerResponseCode)
            {
            case HTTP_RC_DEBIT_NEED_PASSWORD:			//需要用户输入密码
                //标记正在输入密码
                bIsInputPassword	= true;
                byExecuteState		= RCC_INPUT_PASSWORD;

                if(theApp.m_bNeedPWD)
                {
                    IF_SetSingle(7);
                }

                theApp.m_bNeedPWD = false;

                break;
            case HTTP_RC_DEBIT_INSUFFICIENT_BALANCE:	//余额不足
                byExecuteState		= RCC_NO_MONEY;
                break;
            case HTTP_RC_DEBIT_SRC_ILLGAL:				//二维码非法
                byExecuteState		= RCC_QRCODE_ILLEGAL;
                break;
            case HTTP_RC_DEBIT_CODE_TIMEOUT:			//二维码有效期超时
                byExecuteState		= RCC_QRCODE_OT;
                break;
            case HTTP_RC_INPUT_PASSWORD_ERROR:			//用户输入密码错误
                byExecuteState		= RCC_PSW_ERROR;
                break;
            case HTTP_RC_DEBIT_SUCCESS:					//扣费成功
                //标记扣款成功
                bDebitSuccess		= true;
                byExecuteState		= RCC_OK;
                break;
            default:									//未知异常
            {
                //其它异常
                byExecuteState = RCC_INNER_ERROR;
            }
                break;
            }
        }
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            if(RCC_PAY_PLATFORM_ERROR == wOpState)
            {
                byExecuteState		= wOpState;
                strLog  = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:服务器发生故障";
            }
            else if(0xF0 == wOpState)
            {
                byExecuteState		= RCC_DEBIT_OT;
                strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:通讯超时";
            }

            else
            {
                strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:返回数据校验非法";
                byExecuteState		= RCC_PARAM_ERROR;
            }
            break;
        case -2:
            strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:无效的消息类型";
            if(RCC_PAY_PLATFORM_ERROR == wOpState)
            {
                byExecuteState     = wOpState;
            }
            else
            {
                byExecuteState		= RCC_PARAM_ERROR;
            }
            break;
        case -3:
            strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:内部资源申请有异常";
            byExecuteState     = RCC_INNER_ERROR;
            break;
        }

        GetGlobalApp()->RecordLog(strLog);
        strLog.clear();
    }

    //撤单回馈后续逻辑处理(撤单回馈在此截至)
    if (JSON_DT_SEND_DIBIT_CANCEL == wOpType)
    {
        byFunCode		= 0x02;

        //撤单成功
        if ((NULL != pDVJsonBase) && (HTTP_RC_DATA_VALID == pDVJsonBase->ServerResponseCode))
        {
            byExecuteState	= RCC_OK;
        }
        //车道产生的交易凭证关联的订单信息在平台没有找到
        else if (pDVJsonBase && HTTP_RC_PROTOCOL_ERROR == pDVJsonBase->ServerResponseCode)
            byExecuteState	= RCC_OK;

        //保存日志
        QString strlog = "[业务管理线程],响应支付子平台回馈处理完成(撤销订单)，将结果通知调用者(功能代码:"+QString::number(byFunCode)+",执行状况:"+QString::number(byExecuteState);
        GetGlobalApp()->RecordLog(strlog);
    }
    else {
        /*******************************扣款回馈后续逻辑处理*******************************/
        qDebug()<<"扣款回馈后续逻辑处理";
        if(bIsInputPassword)//扣款结果尚未明确
        {
            qDebug()<<"扣款结果尚未明确";
            //用户正在输入密码的处理
            if(NULL != pDVLocalDebit)
            {
                qDebug()<<"用户正在输入密码的处理";
                if(NULL == m_pDVJsonDebitCheck)
                {
                    qDebug()<<"初始化m_pDVJsonDebitCheck";
                    m_pDVJsonDebitCheck			= new CDVJsonDebitCheck(pDVLocalDebit);
                    m_dwCheckDebitSpanTime		= 0;
                    m_dwInputPasswordDelayTime	        = GetGlobalApp()->m_sCurrentDebitInfo.OverTime*1000 - (QDateTime::currentDateTime().toTime_t() - ::GetGlobalApp()->m_dwTradeBeginOvertime);

                    if(m_dwInputPasswordDelayTime<10000)
                    {
                        m_dwInputPasswordDelayTime					 = 30000;
                    }
                }

                //记录日志
                strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"用户正在输入密码，进行扣款结果查询";
                qDebug() << strLog;

                //发起扣款结果快速查询
                emit WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE();
            }
        }
        else				//扣款结果已经出来(失败或成功)
        {
            theApp.m_bNeedPWD = true;
            //如果扣款成功，则进行保存扣款结果操作
            if (bDebitSuccess)
            {
                GetGlobalApp()->m_sCurrentDebitResult.Valid				= true;

                GetGlobalApp()->m_sCurrentDebitResult.ResultType		= 0x00;
                GetGlobalApp()->m_sCurrentDebitResult.ExecuteCode		= 0x00;
                GetGlobalApp()->m_sCurrentDebitResult.PayPlatformType	        = m_sDVJsonDebit.QRCodeType;
                GetGlobalApp()->m_sCurrentDebitResult.DebitTime			= pDVLocalDebit->ServerDebitTime;

                GetGlobalApp()->m_sCurrentDebitResult.ConsumeTime		= QDateTime::currentDateTime().toTime_t() - ::GetGlobalApp()->m_dwTradeBeginOvertime;
                //                memcpy(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc,m_sDVJsonDebit.DebitStatusDesc.toLatin1().data(),sizeof(GetGlobalApp()->m_sCurrentDebitResult.ExecuteDesc));


                QString strTempCode = GetNetPayRecordMatchCode();

                tagDebitResultItems debitResult = GetGlobalApp()->m_sCurrentDebitResult;
                int tscLen = min((uint)strTempCode.toLatin1().length(), sizeof(debitResult.TradeSecurityCode));
                int payIdLen = min((uint)m_sDVJsonDebit.PayIdentifier.toLatin1().length(), sizeof(debitResult.PayIdentifier));
                int orderLen = min((uint)m_sDVJsonDebit.DebitOrderNumber.toLatin1().length(), sizeof(debitResult.DebitOrder));

                memcpy(GetGlobalApp()->m_sCurrentDebitResult.TradeSecurityCode, strTempCode.toLatin1().data(), tscLen);
                memcpy(GetGlobalApp()->m_sCurrentDebitResult.PayIdentifier, m_sDVJsonDebit.PayIdentifier.toLatin1().data(), payIdLen);
                memcpy(GetGlobalApp()->m_sCurrentDebitResult.DebitOrder, m_sDVJsonDebit.DebitOrderNumber.toLatin1().data(), orderLen);

                //控制显示缴费成功交互
                emit WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(0x01,NULL);
                strTempCode.clear();

                //记录日志
                strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:扣费成功,已向调用者发送通知";
            }
            else
            {
                //控制显示缴费成功交互
                emit WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(0x02,NULL);
                //标识处理超时
                m_dwCheckDebitAccumulateTime = 0x0FFF;
                //记录日志
                strLog = "[业务管理线程],响应支付子平台回馈消息"+strResponseType+"处理状态:扣费失败,已向调用者发送通知";
            }
            //正常结束交易（非超时）
            qDebug() << "~~~~~~~~~~FinishTrade2 OnMsgDealSubPayPlatformResponse~~~~~~~~~~";
            FinishTrade(true);
            //保存日志
            GetGlobalApp()->RecordLog("[业务管理线程],响应支付子平台回馈处理完成(扣款请求/查询)，格式化结果(功能代码:"+QString::number(byFunCode)+",执行状况:"+QString::number(byExecuteState));
        }

        //保存日志
        qDebug() << strLog;
    }

    if (JSON_DT_SEND_DIBIT_CANCEL == wOpType) {
        CDVJsonDebitCancel* pdvJsonCancel = (CDVJsonDebitCancel*)pDVJsonBase;
        QString payIdFromServer = pdvJsonCancel->PayIdentifier;

        if (payIdFromServer == theApp.m_payIdentifier) {
            theApp.m_payIdentifier.clear();

            // finished trade
            qDebug() << "~~~~~~~~~~FinishTrade5 JSON_DT_SEND_DIBIT_CANCEL~~~~~~~~~~";
            FinishTrade(true);
        }
    }

    m_callbackThread.setResults(byFunCode, byExecuteState);

    //清理资源
    if (NULL != pDVJsonBase)
    {
        delete pDVJsonBase;
        pDVJsonBase = NULL;
    }

    if (NULL != pDVLocalDebit)
    {
        delete pDVLocalDebit;
        pDVLocalDebit = NULL;
    }
}




/***************************************************
 * 函数名称：OnMsgCheckDebitPasswordState
 * 函数功能：查询用户输入密码的扣款结果状态
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgCheckDebitPasswordState(void)
{

    //进行扣费查询准备(5秒,4秒,3秒,2秒）查询，最长不超过30秒
    switch(m_dwCheckDebitSpanTime)
    {
    case 0:
        m_dwCheckDebitSpanTime = 5000;
        break;
    case 5000:
        m_dwCheckDebitSpanTime = 4000;
        break;
    case 4000:
        m_dwCheckDebitSpanTime = 3000;
        break;
    case 3000:
        m_dwCheckDebitSpanTime = 2000;
        break;
    case 2000:
        m_dwCheckDebitSpanTime = 2000;
        break;
    }
    //设置定时器
    StartTimer(TIMER_ID_DEBIT_CHECK,m_dwCheckDebitSpanTime);

}



/***************************************************
 * 函数名称：OnMsgOnlyDebitMoney
 * 函数功能：执行扣款操作
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgOnlyDebitMoney(void)
{
    QString strLog;
    try
    {
        //构造消费数据并通知支付子平台通讯线程开始工作
        if(!ContructDebitMoneyInfo())
            throw -1;
        //记录日志
        GetGlobalApp()->RecordLog("[业务管理线程]:外部主动调用扣款操作(单体操作)，交易标识:"+GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier);
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:
            strLog = "[业务管理线程]:外部主动调用扣款操作(单体操作),构造并发送扣款数据操作失败";
            break;
        }
        //缓存日志
        if(strLog != m_strLastErrorDesc)
        {
            m_strLastErrorDesc = strLog;
            GetGlobalApp()->RecordLog(m_strLastErrorDesc);
        }
    }
}

/***************************************************
 * 函数名称：OnMsgDebitCancel
 * 函数功能：执行撤单操作
 * 参   数：void
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgDebitCancel(QString wParam)
{
    QString strLog;
    QString pstrPayIdentifier = wParam;

    if (pstrPayIdentifier.isEmpty()) {
        strLog = "[业务管理线程]:外部主动调用撤单接口,传入参数(交易识别码)指针为空";
        qDebug() << strLog;
    }

    //关闭检查扣款状态定时器
    StopTimer(TIMER_ID_DEBIT_MONEY);

    //关闭扣款超时检测定时器
    StopTimer(TIMER_ID_DEBIT_CHECK);

    uint timeNow = QDateTime::currentDateTime().toTime_t();
    uint debitStartTime = theApp.m_sCurrentDebitInfo.m_consDebitTime;
    int overTime = timeNow - debitStartTime;
    qDebug() << "~~~~~~~~m_consDebitTime~~~~~~~~    : " << debitStartTime << "/" << timeNow << "/" << overTime;

    // quickpass platform
    // inform NAK to POS module
    if (pstrPayIdentifier == theApp.m_payIdentifier || !theApp.m_bIsTrading) {
        emit WM_THREAD_QUICKPASS_CORRECT(pstrPayIdentifier);
    }

    QString contdata;
    contdata.clear();

    // handle debitCancel data
    handleDebitCancel(pstrPayIdentifier, contdata);
    //    }

    //清理资源
    pstrPayIdentifier.clear();
}

void CThreadManage::slCorrectLastdealFromPOS(QString payId, QString contdata)
{
    qDebug() << "CThreadManage::slCorrectLastdealFromPOS: " << payId << "/" << contdata;
    handleDebitCancel(payId, contdata);
}

void CThreadManage::handleDebitCancel(QString pstrPayIdentifier, QString contdata)
{
    QString strLog;

    if (pstrPayIdentifier == theApp.m_payIdentifier) {
        //关闭扫码器
        IF_SetSingle(6);

        // finish trade
        qDebug() << "~~~~~~~~~~FinishTrade3 OnMsgDebitCancel~~~~~~~~~~";
        FinishTrade(true);

        if (0x01 == GetGlobalApp()->m_iDebitTaskNo)
        {
            if (NULL != m_pThreadDevice)
            {
                emit  WM_THREAD_DEVICE_STOP_GET_DATA_TODEVICE();
            }
        }

        GetGlobalApp()->m_iDebitTaskNo = 0x00;

        if (contdata.isEmpty())
            ContructDebitCancelInfo(pstrPayIdentifier);
        else
            ContructDebitCancelInfo(pstrPayIdentifier, contdata);
    }

    //记录日志
    qDebug() << "[业务管理线程]:外部主动调用撤单操作，交易标识:" << pstrPayIdentifier;

    // move payId to DB
    addDebitCancelId2DB(pstrPayIdentifier);
}


/***************************************************
 * 函数名称：OnMsgShowInf
 * 函数功能：执行显示操作
 * 参   数：QString
 * 返   回：void
 * *************************************************/
void CThreadManage::OnMsgShowInfo(QString wParam)
{
    emit WM_THREAD_DEVICE_SHOW_DEAL_INFO_TODEVICE(wParam);
}

/***************************************************
 * 函数名称：TimerIDForScanCodeProc
 * 函数描述：定时器超时处理--扫码
 * 函数参数：void
 * 函数返回：void
 * *************************************************/
void CThreadManage::TimerIDForScanCodeProc()
{
    //关闭扫码超时计时器
    StopTimer(TIMER_ID_SCAN_CODE);

}


/***************************************************
 * 函数名称：TimerIDForDebitMoneyProc
 * 函数描述：定时器超时处理--扣费
 * 函数参数：void
 * 函数返回：void
 * *************************************************/
void CThreadManage::TimerIDForDebitMoneyProc()
{
    //交易因为超时截至而完毕
    qDebug() << "~~~~~~~~~~FinishTrade4 TimerIDForDebitMoneyProc~~~~~~~~~~";
    QString strTradeID = FinishTrade(false);

    //设置交互界面
    emit WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(0x02,NULL);

    //记录日志
    GetGlobalApp()->RecordLog("[业务管理线程]:扣款操作超时截至（预定超时,"+QString::number(::GetGlobalApp()->m_sCurrentDebitInfo.OverTime/1000)+" 秒），自动调用撤单操作，交易标识:"+strTradeID);

    m_callbackThread.setResults(0x01,0x06);
}



/***************************************************
 * 函数名称：TimerIDForCheckDebitProc
 * 函数描述：定时器超时处理--查询
 * 函数参数：void
 * 函数返回：void
 * *************************************************/
void CThreadManage::TimerIDForCheckDebitProc()
{

    qDebug()<<"into CThreadManage::TimerIDForCheckDebitProc";
    //关闭检查扣款状态定时器
    StopTimer(TIMER_ID_DEBIT_CHECK);

    //获取查询已经使用的时间
    m_dwCheckDebitAccumulateTime += m_dwCheckDebitSpanTime;


    //判断是否超时
    if(m_dwCheckDebitAccumulateTime >= m_dwInputPasswordDelayTime)
    {
        //交易因为超时截至而完毕
        qDebug() << "~~~~~~~~~~FinishTrade5 TimerIDForCheckDebitProc~~~~~~~~~~";
        QString strTradeID = FinishTrade(false);

        //设置交互界面
        emit WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(0x02,NULL);
        //记录日志
        GetGlobalApp()->RecordLog("[业务管理线程]:扣款操作超时截至（检测累加,"+QString::number(m_dwCheckDebitAccumulateTime)+" ），自动调用撤单操作，交易标识:"+strTradeID);

        m_callbackThread.setResults(0x01,0x06);
    }
    else
    {

        CDVJsonDebitCheck* pDVJsonDebitCheck = new CDVJsonDebitCheck(m_pDVJsonDebitCheck);
        if(NULL != pDVJsonDebitCheck)
        {
            pDVJsonDebitCheck->Version      = GetGlobalApp()->m_JsonVersion;
            pDVJsonDebitCheck->ProvinceID   = GetGlobalApp()->m_sTranstParam.ProvinceID;
            pDVJsonDebitCheck->PayIdentifier= GetGlobalApp()->m_sCurrentDebitInfo.PayIdentifier;
            pDVJsonDebitCheck->PackageType  = JSON_DT_SEND_DIBIT_CHECK;
            pDVJsonDebitCheck->SubmitTime   = GetGlobalApp()->GetCurrentTimeString(1);
            //记录日志
            GetGlobalApp()->RecordLog("[业务管理线程]:扣款操作超时未截至（已累加 "+QString::number(m_dwCheckDebitAccumulateTime)+" 毫秒），根据规则，自动进行下一次的扣款状态检测，交易标识:"+pDVJsonDebitCheck->PayIdentifier);

            QString * t_strSendJsonData = NULL;
            t_strSendJsonData =  new QString();

            if(t_strSendJsonData != NULL)
            {
                *t_strSendJsonData = m_pJsonIF.FormatSendData2Json(pDVJsonDebitCheck);
                emit WM_THREAD_HTTP_SEND_PostQuery(t_strSendJsonData);
            }
        }
        //通知下一次查询
        emit  WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE();
    }
}





/***************************************************
 * 函数名称：ResourceDestroy
 * 函数描述：资源销毁
 * 函数参数：void
 * 函数返回：void
 * *************************************************/
void CThreadManage::ResourceDestroy(void)
{

    QString strLog;
    try
    {

        /**********************************************清理各种线程资源**********************************************/

        StopTimer(TIMER_ID_DEBIT_MONEY);
        StopTimer(TIMER_ID_DEBIT_CHECK);
        StopTimer(TIMER_ID_SCAN_CODE);


        //清除扣款状态查询结构体
        if(NULL == m_pDVJsonDebitCheck)
        {
            delete m_pDVJsonDebitCheck;
            m_pDVJsonDebitCheck = NULL;
        }
        m_sDVJsonDebit.Reset();

        strLog = "[业务管理线程],正在开始退出工作线程...";
        //记录日志
        GetGlobalApp()->RecordLog(strLog);

        /*********硬件线程注销*********/
        if(NULL != m_pThreadDevice)
        {
            //设置该线程推出标记为真
            m_pThreadDevice->m_bExit = true;

            m_pThreadDevice->ResourceDestroy();
            //沉默50毫秒
            msleep(50);
        }


        /*****停止支付子平台通讯线程*******/
        if(NULL != m_pThreadHttp)
        {
            //设置该线程推出标记为真
            m_pThreadHttp->m_bExit = true;
            //沉默50毫秒
            msleep(50);
        }


        //关闭消息队列
        m_UpperMange.CloseUpperMS();



        //记录日志
        GetGlobalApp()->RecordLog(strLog);
    }
    catch(int& iErrorCode)
    {

    }


}




/****************************QT4.7*****************************/

void  CThreadManage::SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString sParm)
{
    emit  WM_THREAD_DEVICE_SHOW_DEAL_INFO(sParm);
}

void  CThreadManage::SEND_WM_THREAD_MANAGE_START_DEBIT_FLOW(const QString &sParm)
{

    emit  WM_THREAD_MANAGE_START_DEBIT_FLOW(sParm);
}

void  CThreadManage::SEND_WM_THREAD_MANAGE_ONLY_DEBIT_MONEY(void)
{
    emit WM_THREAD_MANAGE_ONLY_DEBIT_MONEY();
}

void  CThreadManage::SEND_WM_THREAD_DEVICE_STOP_GET_DATA(void)
{
    emit  WM_THREAD_DEVICE_STOP_GET_DATA();
}

void  CThreadManage::SEND_WM_THREAD_DEVICE_START_GET_DATA(int lParm,int wParm)
{
    emit  WM_THREAD_DEVICE_START_GET_DATA(lParm,wParm);
}

void  CThreadManage::SEND_WM_THREAD_MANAGE_DEBIT_CANCEL(QString sParm)
{
    emit  WM_THREAD_MANAGE_DEBIT_CANCEL(sParm);
}

void  CThreadManage::SEND_WM_THREAD_DEVICE_CLEAR(int lParm,QString sParm)
{
    emit  WM_THREAD_DEVICE_CLEAR_TODEVICE(lParm,sParm);
}




/***************************************************
  * 函数名称：CheckAndDeleteLogFile
  * 函数描述：日志检查定时器处理--删除
  * 函数参数：void
  * 函数返回：void
  * *************************************************/
void CThreadManage::CheckAndDeleteLogFile()
{
    static QMutex mutex;
    mutex.lock();

    GetGlobalApp()->RecordLog("日志检查定时器处理--删除");
    QDir dir(LOGOUTPATH);
    //若目录不存在则返回退出
    if (!dir.exists())
    {
        qDebug()<<"/var/log not exist";
        mutex.unlock();
        return;
    }

    //设置过滤器(目录，文件或非上级目录)
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    //当前时间
    time_t  t_CurrentTime = QDateTime::currentDateTime().toTime_t();
    //取得目录中文件列表(包含目录)
    QFileInfoList list = dir.entryInfoList();
    int i=0;
    do
    {
        QFileInfo fileInfo = list.at(i);
        //判断是否为目录，如果是目录则遍历，否则当前处理文件
        bool bisDir=fileInfo.isDir();

        if(!bisDir)
        {
            //取得文件类型后缀
            QString t_StrSuffix = fileInfo.suffix().toLower();
            bool t_BoolSuffix = (t_StrSuffix =="log");

            //取得文件名称
            QString t_FileName=fileInfo.fileName();
            QString t_strFileName=t_FileName.mid(0,13);
            bool    t_BoolFileName = (t_strFileName=="LogTWSDNetPay");

            time_t  t_FileTime = fileInfo.created().toTime_t();

            if(t_BoolSuffix&&t_BoolFileName)
            {
                if((t_CurrentTime-t_FileTime)>LOGOUTTIME)
                    // if((t_CurrentTime-t_FileTime)>(60*60))
                {
                    //记录日志
                    GetGlobalApp()->RecordLog("删除日志路径："+fileInfo.filePath());
                    //刪除文件
                    QFile::remove(fileInfo.filePath());
                }
            }
        }
        i++;
    }while(i<list.size());

    mutex.unlock();
}

// 增加撤单相关处理
void CThreadManage::updateDebitCancelById(QString id)
{
    //    qDebug() << "CThreadManage::updateDebitCancelById: " << id;

    if (id.isEmpty()) {
        if (m_lastPayId4Cancel.isEmpty())
            return;

        id = m_lastPayId4Cancel;
    }

    PayDBInf::instance()->updateCancelDataById(id.toStdString());
}

void CThreadManage::slTimeoutClearDisplay()
{
    //    qDebug() << "CThreadManage::timeoutClearDisplay";
    //    emit WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(0x03, NULL);
}

void CThreadManage::addDebitCancelId2DB(QString id)
{
    PayDBInf::instance()->addCancelData2DB(id.toStdString());

    //    QString strLog = "[主进程响应], PayDBInf::addCancelData2DB: "+id;
    //    qDebug() << strLog;
}

QString CThreadManage::getDebitCancelIdFromDB()
{
    string payId;
    payId = PayDBInf::instance()->getCancelDataPayId();

    return QString::fromStdString(payId);
}

void CThreadManage::slHttpDataHeartBeat(QString t_httpData)
{
    string id = PayDBInf::instance()->getCancelDataPayId();
    QString payId = QString::fromStdString(id);
    if (!payId.isEmpty()) {
        ContructDebitCancelInfo(payId);
        m_debitCancelPayIdFromDB = payId;
    }
}

void CThreadManage::refreshLCDDisplay()
{
    emit WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(0x03, NULL);
}
