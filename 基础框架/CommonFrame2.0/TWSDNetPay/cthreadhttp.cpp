#include "cthreadhttp.h"
#include "twsdnetpay.h"

CThreadHttp::CThreadHttp(void)
{
    m_dwFailHearWarningTimes = 0;
    m_dwFailForCenterReflectWeb = 0;
    m_dwHeartCheckMax  = 3000;

}

CThreadHttp::~CThreadHttp(void)
{
    delete m_manager;
}

void CThreadHttp::Init(void)
{
    //业务
    m_manager = new QNetworkAccessManager;
    m_request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    connect(m_manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onFinished(QNetworkReply *)));


    //心跳
    m_managerHear = new QNetworkAccessManager;
    m_requestHear.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    connect(m_managerHear,SIGNAL(finished(QNetworkReply *)),this,SLOT(onFinishedHear(QNetworkReply *)));


    m_HearTimer = new QTimer;
    connect(m_HearTimer,SIGNAL(timeout()),this,SLOT(onMsgGetHeartbeat()));
    //资源申请
    fnReadSpecailConfig();
    m_HearTimer->start(30*1000);
}

/**************************************************
 * 函数名称：fnReleaseResource
 * 函数描述：资源清理函数
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
void CThreadHttp::fnReleaseResource()
{
}



/***********************************************
* Name        :  onFinished
* Described   :  网络数据处理
* Parameters  :  reply  云端返回数据
* return value;  void   互斥锁
* *********************************************/
void CThreadHttp::onFinished(QNetworkReply *reply)//reply用于接收消息
{

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt()==200)
    {
        QByteArray t_Data=reply->readAll();

        GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = false;

        if((t_Data != "success")&(t_Data != "success\n"))
        {
            theApp.RecordLog("[网络接收数据：]"+QString::fromLocal8Bit(t_Data));
            emit WM_THREAD_HTTP_ARRIVAL_DATA(QString::fromLocal8Bit(t_Data),m_CurrentOperation);
        }
        else
        {
            //测试使用

            //20170510
         //   emit WM_THREAD_HTTP_ARRIVAL_HEARTBEAT(QString::fromLocal8Bit(t_Data));
        }
    }
    else
    {
        qDebug()<<"支付网络 Error:"<<status_code.toInt();
        theApp.RecordLog(QString("支付网络 Error: ") + QString::number(status_code.toInt()) );

        //网络心跳
        if(m_CurrentOperation == JSON_DT_SEND_HEART)
        {
            ++m_dwFailHearWarningTimes;
            if(m_dwFailHearWarningTimes>m_dwHeartCheckMax)
            {
                m_dwFailHearWarningTimes = 0;
                GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = true;
            }
        }
        else
        {
            GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = true;
        }

    }
    reply->deleteLater();
}

/***********************************************
* Name        :  onFinishedHear
* Described   :  网络心跳数据处理
* Parameters  :  reply  云端返回数据
* return value;  void   互斥锁
* *********************************************/
void CThreadHttp::onFinishedHear(QNetworkReply *reply)
{

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt()==200)
    {
        GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = false;

     //   theApp.RecordLog("网络心跳正常");
        emit WM_THREAD_HTTP_ARRIVAL_HEARTBEAT(QString("1"));
    }
    else
    {
        qDebug()<<"心跳网络 Error:"<<status_code.toInt();
        theApp.RecordLog(QString("心跳网络 Error: ") + QString::number(status_code.toInt()) );


        //网络心跳
        if(m_CurrentOperation == JSON_DT_SEND_HEART)
        {
            ++m_dwFailHearWarningTimes;
            if(m_dwFailHearWarningTimes>m_dwHeartCheckMax)
            {
                m_dwFailHearWarningTimes = 0;
                GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = true;
            }
        }
        else
        {
            GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = true;
        }
    }
    reply->deleteLater();
}



/**************************************************
 * 函数名称：fnReadSpecailConfig
 * 函数描述：资源设置
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
void CThreadHttp::fnReadSpecailConfig()
{

    if(!GetGlobalApp()->m_CurrentServerAddress.isEmpty())
    {
        //扣款URL
        m_wDebitPort        =  GetGlobalApp()->m_CurrentServerPort;
        m_strDebitURL       = "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wDebitPort)+"/pay";

        //查询URL
        m_wDebitCheckPort   =  GetGlobalApp()->m_CurrentServerPort;
        m_strDebitCheckURL  =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wDebitCheckPort)+"/pay";

        //撤单URL
        m_wDebitCancelPort  =  GetGlobalApp()->m_CurrentServerPort;
        m_strDebitCancelURL =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wDebitCancelPort)+"/pay/reverse";

        //心跳URL
        m_wHeartPort        =  GetGlobalApp()->m_CurrentServerPort;
        m_strHeartURL       =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wHeartPort)+"/heartbeat";

        theApp.RecordLog("使用车道下发的IP");
    }
    else
    {
        m_strDebitURL       = QUERY_URL;
        m_strDebitCheckURL  = CHARG_URL;
        m_strDebitCancelURL = CANCEL_URL;
        m_strHeartURL       = HEART_URL;

        theApp.RecordLog("使用备用IP");
    }

    qDebug()<<m_strDebitCheckURL;
    qDebug()<<m_strDebitCancelURL;
    qDebug()<<m_strDebitURL;
    qDebug()<<m_strHeartURL;

    theApp.RecordLog("扣款请求接口地址:     " + m_strDebitURL);
    theApp.RecordLog("扣款查询接口地址:     " + m_strDebitCheckURL);
    theApp.RecordLog("扣款撤单接口地址:     " + m_strDebitCancelURL);
    theApp.RecordLog("心跳查询接口地址:     " + m_strHeartURL);

}


/**************************************************
 * 函数名称：GetSendDataTypeDesc
 * 函数描述：获取发送数据类型描述
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
const QString CThreadHttp::GetSendDataTypeDesc( const int& iDataType)
{
    QString strReturn;
    switch(iDataType)
    {
    case JSON_DT_SEND_DIBIT:
        strReturn = "扣款请求";
        break;
    case JSON_DT_SEND_DIBIT_CHECK:
        strReturn = "扣费查询";
        break;
    case JSON_DT_SEND_DIBIT_CANCEL:
        strReturn = "撤销订单";
        break;
    }
    return strReturn;
}


/**************************************************
 * 函数名称：GetServerResponseCodeDesc
 * 函数描述：获取服务器回馈代码描述
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
const QString CThreadHttp::GetServerResponseCodeDesc( const int& iResponseType)
{
    QString strReturn;
    switch(iResponseType)
    {
    case HTTP_RC_DATA_LEGAL:
        strReturn = "回馈数据符合规则";
        break;
    case HTTP_RC_DATA_VALID:
        strReturn = "数据正常";
        break;
    case HTTP_RC_BLACKLIST:
        strReturn = "二维码绑定支付用户在特微黑名单当中";
        break;
    case HTTP_RC_NO_REGISTER:
        strReturn = "二维码绑定支付用户未注册";
        break;
    case HTTP_RC_NO_ENTRY_BY_ID:
        strReturn = "二维码绑定支付用户无有效入口信息";
        break;
    case HTTP_RC_ID_IS_RUNNING:
        strReturn = "二维码绑定支付用户已经路网有入口";
        break;
    case HTTP_RC_DEBIT_NEED_PASSWORD:
        strReturn = "需要二维码绑定支付用户输入密码";
        break;
    case HTTP_RC_DEBIT_CODE_TIMEOUT:
        strReturn = "二维码有效期超时";
        break;
    case HTTP_RC_DEBIT_SRC_ILLGAL:
        strReturn = "二维码来源非法";
        break;
    case HTTP_RC_DEBIT_INSUFFICIENT_BALANCE:
        strReturn ="余额不足";
        break;
    case HTTP_RC_DEBIT_FAIL:
        strReturn = "扣费失败";
        break;
    case HTTP_RC_DEBIT_SUCCESS:
        strReturn = "扣费成功";
        break;
    case HTTP_RC_DEBIT_NO_VOUCHER:
        strReturn = "协议格式错误";
        break;
    case HTTP_RC_PROTOCOL_ERROR:
        strReturn = "车道产生的交易凭证关联的订单信息在平台没有找到";
        break;
    case HTTP_RC_PAYPLATROMR_ERROR:
        strReturn = "支付平台错误";
        break;
    case HTTP_RC_NO_CANCEL_ORDER:
        strReturn = "订单已过有效期，禁止撤销";
        break;
    case HTTP_RC_NO_SERVICES:
        strReturn = "平台暂停服务";
        break;
    default:
        strReturn = "平台产生无法解析的内部异常";
        break;
    }
    return strReturn;
}


//数据递交 -- 线程工作函数
quint32  CThreadHttp::SendData(tagHttpParam lpParam)
{
    quint32 dwReturn = 0x01;
    QString strErrorDesc;
    tagHttpParam pParam = lpParam;

    //计时开始
    //  quint64 dwStart = QTime::elapsed();
    quint64 dwStart;

    try
    {

        //如果需要强制结束，则结束线程
        if(pParam.NeedEndThread)
            goto END;

    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case 1:
            strErrorDesc = "参数结构体指针为空";
            break;
        }
        dwReturn = 0;
    }
    catch(...)
    {
        strErrorDesc = "数据提交异常";
    }

END:

    return dwReturn;

}

/**************************************************
 * 函数名称：DataConvertFromJson2String
 * 函数描述：数据格式转换,从调用者传过来的DVJson格式转换成字符串形式
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
const bool CThreadHttp::DataConvertFromJson2String(const quint32& wSendDataType,
                                                   CDVJsonBase* pDVJsonReceive, QString& strURL,
                                                   quint32& wPort, QString& strSendData, int& iCallMode,
                                                   CJsonOperationIF** pJsonIF)
{
    bool bReturn = true;

    try
    {
        switch(wSendDataType)
        {
        case JSON_DT_SEND_DIBIT:		//支付请求(扣费)
        {
            //获得相应的数据类指针
            CDVJsonDebit* pDVJsonDebit = (CDVJsonDebit* )pDVJsonReceive;
            if(NULL == pDVJsonDebit)
            {
                throw -1;
            }

            //申请数据操作类
            *pJsonIF = new CJsonDebitOperate();
            if(NULL == *pJsonIF)
            {
                throw -2;
            }
            //转化成JSON数据格式
            strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebit);
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
            if(NULL == pDVJsonDebitCheck)
            {
                throw -1;
            }

            //申请数据操作类
            *pJsonIF = new CJsonDebitOperate();
            if(NULL == *pJsonIF)
            {
                throw -2;
            }
            //转化成JSON数据格式
            strSendData = (*pJsonIF)->FormatSendData2Json(pDVJsonDebitCheck);
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
            if(NULL == pDVJsonDebitCancel)
            {
                throw -1;
            }

            //申请数据操作类
            *pJsonIF = new CJsonDebitOperate();
            if(NULL == *pJsonIF)
            {
                throw -2;
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
            if(NULL == pDVJsonDebitSecond)
            {
                throw -1;
            }

            //申请数据操作类
            *pJsonIF = new CJsonDebitOperate();
            if(NULL == *pJsonIF)
            {
                throw -2;
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
            if(NULL == pDVJsonHeart)
            {
                throw -1;
            }

            //申请数据操作类
            *pJsonIF = new CJsonHeartOperate();
            if(NULL == *pJsonIF)
            {
                throw -2;
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
            throw -3;
        }
    }
    catch(int& iErrorCode)
    {
        bReturn = false;

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



/**************************************************
 * 函数名称：GetRandNumber
 * 函数描述：获取随机数，在给定的范围内
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
quint32  CThreadHttp::GetRandNumber( const quint32& nMin, const quint32& nMax)
{
    quint32 nReturn = 0;
    quint32 nRealMax = (0 == nMax)?1:nMax;
    // srand( (unsigned)( QTime::elapsed ());
    //   srand( (unsigned)(1);//临时调用
    nReturn = rand() % (nMax - nMin + 1)  + nMin ;
    //返回数据
    return nReturn;
}



/**************************************************
 * 函数名称：CheckHeartState
 * 函数描述：进行心跳检测
 * 参   数：CDVJsonDebit
 * 返   回：QString
 * ************************************************/
void CThreadHttp::CheckHeartState()
{
    //解析数据
    quint32 wSendDataType = JSON_DT_SEND_HEART;
    //回馈结果代码
    quint32 wReturnCode = 0x00;
    //转化成需要发送的数据格式载体
    QString strSendData = "HeartCheck";

    QString strLog;
    //获取当前使用的URL地址
    QString strURL;
    //获取当前使用的端口号
    quint32 wPort = 80;
    //当前调用方式
    int	iCallMode = 1;//默认GET方式获取数据
    //子线程等待时间,单位:毫秒
    quint32 dwWaitThreadWorkTimeout = 5000;

    //服务器返回的数据
    QString strResponseResult;
    //服务器返回的状态码
    quint32 dwResponseStateCode = 0x00;
    //服务器处理时间
    quint32 dwConsumeTime = 0x00;
    //开始处理流程
    try
    {

        tagHttpParam sParam;

        //填充参数结构体
        sParam.ExecuteCode			= 0xFF;
        sParam.ConsumeTime			= 0;
        sParam.ServerResponseData	= "";
        sParam.ServerResponseCode	= 0;
        sParam.NeedEndThread		= false;
        sParam.SessionReceiveTimeout    = 4000;
        sParam.SessionControlTimeout    = 5000;

        sParam.CallMode	= iCallMode;
        sParam.Port		= m_wHeartPort;
        sParam.URL		= m_strHeartURL;
        //转化成UTF-8格式
        //  sParam.SendData	= StringConvertByCodePage(strSendData,CP_ACP,CP_UTF8);

        //  quint64 dwTime1 = QTime::elapsed() ;
        SendData(sParam);
        //  quint64 dwTime2 = QTime::elapsed() - dwTime1;


    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1://无法启动发送信息的线程

            break;
        case -2://HTTP服务器回馈失败

            break;
        }
        wReturnCode = abs(iErrorCode);
    }

}



/**************************************************************
 * 函数名称：onMsgPostQueryData
 * 函数描述：支付请求-查询
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgPostQueryData(QString* t_data)
{
    m_bWorking = true;
    //    m_HearTimer->stop();  //关闭Hear扫码器


    //标示当前业务操作---查询
    m_CurrentOperation = JSON_DT_SEND_DIBIT_CHECK;

    //set URL

    QByteArray t_SendData = (*t_data).toLocal8Bit();
    m_request.setUrl(QUrl(m_strDebitCheckURL));
    m_manager->post(m_request,t_SendData);

    theApp.RecordLog("[网络请求],启动支付请求-查询:"+t_SendData);

    m_bWorking = false;

    if(t_data != NULL )
    {
        delete t_data;
        t_data = NULL;
    }

    //启动Hear扫码器
    //     m_HearTimer->start(m_dwHeartCheckMax);

}



/**************************************************************
 * 函数名称：onMsgPostCancelData
 * 函数描述：支付请求-撤单
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgPostCancelData(QString* t_data)
{
    m_bWorking = true;
    //    m_HearTimer->stop();  //关闭Hear扫码器

    //标示当前业务操作---撤单
    m_CurrentOperation = JSON_DT_SEND_DIBIT_CANCEL;
    qDebug()<<"CThreadHttp::onMsgPostCancelData";

    QByteArray t_SendData = (*t_data).toLocal8Bit();

    m_request.setUrl(QUrl(m_strDebitCancelURL));
    m_manager->post(m_request,t_SendData);

    theApp.RecordLog("[网络请求],启动支付请求-撤单:"+t_SendData);

    m_bWorking = false;

    if(t_data != NULL )
    {
        delete t_data;
        t_data = NULL;
    }

    //启动Heart
    //    m_HearTimer->start(m_dwHeartCheckMax);

}


/**************************************************************
 * 函数名称：onMsgPostChargingData
 * 函数描述：支付请求-扣费
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgPostChargingData(QString* t_data)
{
    m_bWorking = true;
    //  m_HearTimer->stop();  //关闭Heart
    //  标示当前业务操作---扣费
    m_CurrentOperation = JSON_DT_SEND_DIBIT;

    QByteArray t_SendData = (*t_data).toLocal8Bit();
    m_request.setUrl(QUrl(m_strDebitURL));
    m_manager->post(m_request,t_SendData);

    theApp.RecordLog("[网络请求],启动支付请求-扣费:"+t_SendData);


    m_bWorking = false;

    if(t_data != NULL)
    {
        delete t_data;
        t_data = NULL;
    }


    //启动Heart
    //   m_HearTimer->start(m_dwHeartCheckMax);

}


/**************************************************************
 * 函数名称：onMsgSecondPostChargingData
 * 函数描述：支付请求-二次扣费
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgSecondPostChargingData(QString t_data)
{

    //标示当前业务操作---二次扣费
    m_CurrentOperation = JSON_DT_SEND_DIBIT_SECOND;

    QByteArray t_SendData = t_data.toLocal8Bit();
    m_request.setUrl(QUrl(m_strDebitSecondURL));
    m_manager->post(m_request,t_SendData);
    theApp.RecordLog("[主进程响应],启动支付请求-二次扣费:"+t_data);
}


/**************************************************************
 * 函数名称：onMsgGetHeartbeat
 * 函数描述：常规查询-心跳检测
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgGetHeartbeat(void)
{            
    //标示当前业务操作---心跳检测
    //m_CurrentOperation = JSON_DT_SEND_HEART;
    m_requestHear.setUrl(QUrl(m_strHeartURL));
    m_managerHear->get(m_requestHear);

}






