#include "cthreadhttp.h"
#include "ctwsdnetpay_dllapp.h"
#include "upay/upaytmc.h"
#include "http/qreplytimeout.h"

#define HTTP_DEFAULT_TIMEOUT    60*1000

CThreadHttp *CThreadHttp::self = NULL;

CThreadHttp::CThreadHttp(void)
{
    m_dwFailHearWarningTimes = 0;
    m_dwFailForCenterReflectWeb = 0;
    m_dwHeartCheckMax  = 3000;
    //20171012
    m_dwHeartCheckMax = 1000*60*6;
    m_reply = NULL;
    m_httpRequestAborted = false;
    m_isNetworkAccessible = true;
    m_isHeartbeatInProgress = false;

    self = this;
    m_CurrentOperation = 0;
}

CThreadHttp::~CThreadHttp(void)
{
    delete m_manager;
}

CThreadHttp* CThreadHttp::instance ()
{
    if (NULL == self) {
        self = new CThreadHttp();
    }

    return self;
}

void CThreadHttp::reInitNetwork()
{ 
    m_request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    if (m_manager) {
        disconnect(m_manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onFinished(QNetworkReply *)));
        m_manager->deleteLater();
    }

    m_manager = new QNetworkAccessManager;
    connect(m_manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onFinished(QNetworkReply *)));
}

void CThreadHttp::Init(void)
{
   m_manager = new QNetworkAccessManager;

   m_HearTimer = new QTimer;

   m_request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

   connect(m_manager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onFinished(QNetworkReply *)));
   connect(m_HearTimer,SIGNAL(timeout()),this,SLOT(onMsgGetHeartbeat()));

   //资源申请
   fnReadSpecailConfig();
   m_HearTimer->start(m_dwHeartCheckMax);
}

void CThreadHttp::dumpSpecailConfig()
{
    fnReadSpecailConfig();
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

void  CThreadHttp::onFinishedDeviceMgt(QNetworkReply *reply)
{
    qDebug() << "CThreadHttp::onFinishedDeviceMgt";
    emit WM_THREAD_HTTP_DEVICE_MGT(reply);
}


/***********************************************
* Name        :  onFinished
* Described   :  网络数据处理
* Parameters  :  reply  云端返回数据
* return value;  void   互斥锁
* *********************************************/
void CThreadHttp::onFinished(QNetworkReply *reply)//reply用于接收消息
{
    QString path = reply->url().path();
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int nStatusCode = variant.toInt();
    qDebug()<<"CThreadHttp::onFinished: 网络响应码:"<<nStatusCode<<",(非200说明网络异常)";
    qDebug()<<"后台回馈地址:"<<reply->url().toString()<<" "<<reply->error()<<" "<<reply->errorString();

    if (0 == path.compare("/signIn", Qt::CaseSensitive))
    {
        m_CurrentOperation = JSON_DT_SEND_QUICKPASS_SIGNIN;
        qDebug()<<"收到签到回馈";
    }
    else if (0 == path.compare("/pay", Qt::CaseSensitive))
    {
        m_CurrentOperation = JSON_DT_SEND_DIBIT;
        m_httpRequestAborted = false;
        qDebug()<<"收到扣款回馈";
    }
    else if (0 == path.compare("/pay/reverse", Qt::CaseSensitive))
    {
        m_CurrentOperation = JSON_DT_SEND_DIBIT_CANCEL;
        qDebug()<<"收到撤单回馈";
    }
    else if (0 == path.compare("/heartbeat", Qt::CaseSensitive))
    {
        m_CurrentOperation = JSON_DT_SEND_HEART;
        qDebug()<<"收到心跳回馈";
    }


//    if (m_CurrentOperation != JSON_DT_SEND_HEART)
//        qDebug() << "CThreadHttp::onFinished: 网络响应码:" << nStatusCode << "/" << m_CurrentOperation <<"/" << reply->url().toString() << "/" << reply->error() << "/" << reply->errorString();

    if (reply->error() != QNetworkReply::NoError)
    {
        emit WM_THREAD_HTTP_REPLY(reply, m_CurrentOperation);
        return;
    }

    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (m_CurrentOperation == JSON_DT_SEND_QUICKPASS_SIGNIN || \
        m_CurrentOperation == JSON_DT_SEND_QUICKPASS_CORRECT)
    {
        if (status_code.toInt() == 200) {
            QByteArray t_data = reply->readAll();
            if ((t_data != "success")&(t_data != "success\n"))
            {
                QString str = QString::fromLocal8Bit(t_data);
                emit WM_THREAD_HTTP_QUICKPASS_DATA(str, m_CurrentOperation);
            }
        }
        return;
    }

    if (status_code.toInt()==200)
    {
        QByteArray t_Data=reply->readAll();

        // quickpass support
        if (JSON_DT_SEND_DIBIT == m_CurrentOperation || \
            JSON_DT_SEND_DIBIT_CANCEL == m_CurrentOperation)
        {
            QString str = QString::fromLocal8Bit(t_Data);
            emit WM_THREAD_HTTP_QUICKPASS_DATA(str,m_CurrentOperation);
        }

        GetGlobalApp()->m_bNetErrorHttpServerInTollNetNotExist = false;

    //    if((t_Data != "success")&(t_Data != "success\n"))
        if(JSON_DT_SEND_HEART != m_CurrentOperation)
        {
            QString str = QString::fromLocal8Bit(t_Data);
            emit WM_THREAD_HTTP_ARRIVAL_DATA(str,m_CurrentOperation);
        }
        else
        {
            //  qDebug()<<"网络心跳";
            QString str = QString::fromLocal8Bit(t_Data);
            emit WM_THREAD_HTTP_ARRIVAL_HEARTBEAT(str);
        }
    }
    else
    {
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

            // filter invalid Operations
            if (m_CurrentOperation > 0)
            {
                // inform network errors
                emit WM_THREAD_HTTP_ARRIVAL_DATA(reply->errorString(), m_CurrentOperation);
            }
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
        m_wDebitCheckPort	=  GetGlobalApp()->m_CurrentServerPort;
        m_strDebitCheckURL  =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wDebitCheckPort)+"/pay";

        //撤单URL
        m_wDebitCancelPort	=  GetGlobalApp()->m_CurrentServerPort;
        m_strDebitCancelURL =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wDebitCancelPort)+"/pay/reverse";

        //心跳URL
        m_wHeartPort        =  GetGlobalApp()->m_CurrentServerPort;
        m_strHeartURL       =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_wHeartPort)+"/heartbeat";

        // quickpass support
        m_serverPort        =  GetGlobalApp()->m_CurrentServerPort;
        m_strQuickpassCorrectURL    =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_serverPort)+"/correct";
        m_strQuickpassSigninURL     =  "http://"+GetGlobalApp()->m_CurrentServerAddress+":"+QString::number(m_serverPort)+"/signIn";
    }
    else
    {
        m_strDebitURL       = QUERY_URL;
        m_strDebitCheckURL  = CHARG_URL;
        m_strDebitCancelURL = CANCEL_URL;
        m_strHeartURL       = HEART_URL;

        // quickpass support
        m_strQuickpassCorrectURL    = CORRECT_URL;
        m_strQuickpassSigninURL     = SIGNIN_URL;
    }

    qDebug()<<m_strDebitCheckURL;
    qDebug()<<m_strDebitCancelURL;
    qDebug()<<m_strDebitURL;
    qDebug()<<m_strHeartURL;
    qDebug() << m_strQuickpassCorrectURL;
    qDebug() << m_strQuickpassSigninURL;
}

/**************************************************************
 * 函数名称：onMsgPostQueryData
 * 函数描述：支付请求-查询
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgPostQueryData(QString* t_data)
{
     QString url = m_strDebitCheckURL;
     qDebug() << "\nCThreadHttp::onMsgPostQueryData: " << url << (*t_data);

     QNetworkAccessManager *pManger = new QNetworkAccessManager(this);
     QNetworkRequest request;
     request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
     request.setUrl(QUrl(url));

     QByteArray t_SendData = (*t_data).toLocal8Bit();

     QNetworkReply *pReply = pManger->post(request, t_SendData);
     connect(pManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

     QReplyTimeout *pTimeout = new QReplyTimeout(pReply, HTTP_DEFAULT_TIMEOUT);

     // 超时进一步处理
     connect(pTimeout, SIGNAL(timeout()), pTimeout, SLOT(onTimeout()));

      if (t_data)
          delete t_data;
}



/**************************************************************
 * 函数名称：onMsgPostCancelData
 * 函数描述：支付请求-撤单
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgPostCancelData(QString* t_data, bool beCancel)
{
    QString url = m_strDebitCancelURL;
    qDebug() << "\nCThreadHttp::onMsgPostCancelData: " << beCancel << url << (*t_data);

    // abort previous request
    if (m_httpRequestAborted && beCancel) {
        qDebug() << "\n~~~~~~m_reply->abort()~~~~~~";

        m_reply->abort();
        m_reply->deleteLater();

        m_httpRequestAborted = false;
    }

    QNetworkAccessManager *pManger = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setUrl(QUrl(url));

    QByteArray t_SendData = (*t_data).toLocal8Bit();

    QNetworkReply *pReply = pManger->post(request, t_SendData);
    connect(pManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QReplyTimeout *pTimeout = new QReplyTimeout(pReply, HTTP_DEFAULT_TIMEOUT);

    // 超时进一步处理
    connect(pTimeout, SIGNAL(timeout()), pTimeout, SLOT(onTimeout()));

     if (t_data)
         delete t_data;
}


/**************************************************************
 * 函数名称：onMsgPostChargingData
 * 函数描述：支付请求-扣费
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgPostChargingData(QString* t_data)
{
    QString url = m_strDebitURL;
    qDebug() << "\nCThreadHttp::onMsgPostChargingData: " << url << (*t_data);

    QNetworkAccessManager *pManger = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setUrl(QUrl(url));

    QByteArray t_SendData = (*t_data).toLocal8Bit();

    QNetworkReply *pReply = pManger->post(request, t_SendData);
    connect(pManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    m_reply = pReply;
    m_httpRequestAborted = true;

    QReplyTimeout *pTimeout = new QReplyTimeout(pReply, HTTP_DEFAULT_TIMEOUT);

    // 超时进一步处理
    connect(pTimeout, SIGNAL(timeout()), pTimeout, SLOT(onTimeout()));
    connect(pTimeout, SIGNAL(timeout()), this, SLOT(onMsgPostDebitTimeout()));

     if (t_data)
         delete t_data;
}

void CThreadHttp::onMsgPostDebitTimeout()
{
    qDebug() << "CThreadHttp::onMsgPostDebitTimeout";
    m_httpRequestAborted = false;
    m_reply->deleteLater();
}

/**************************************************************
 * 函数名称：onMsgSecondPostChargingData
 * 函数描述：支付请求-二次扣费
 * 函数参数：QString
 * 函数返回：void
 * ************************************************************/
void CThreadHttp::onMsgSecondPostChargingData(QString t_data)
{
    qDebug() << "CThreadHttp::onMsgSecondPostChargingData: " << t_data;

    //标示当前业务操作---二次扣费
     m_CurrentOperation = JSON_DT_SEND_DIBIT_SECOND;

    QByteArray t_SendData = t_data.toLocal8Bit();
    m_request.setUrl(QUrl(m_strDebitSecondURL));
    m_reply = m_manager->post(m_request,t_SendData);

    m_httpRequestAborted = true;

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
//    m_CurrentOperation = JSON_DT_SEND_HEART;

    static int timeCount = 0;

    if (m_isHeartbeatInProgress)
    {
        qDebug() << "CThreadHttp::onMsgGetHeartbeat m_isHeartbeatInProgress: " << m_isHeartbeatInProgress;
        return;
    }

    QTimer timer;
    timer.setInterval(2000);  // 设置超时时间 1 秒
    timer.setSingleShot(true);  // 单次触发

    QNetworkAccessManager manager;
    QNetworkRequest request;

    connect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onFinished(QNetworkReply *)));

    request.setUrl(QUrl(m_strHeartURL));

    QNetworkReply *pReply = manager.get(request);

    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));

    m_isHeartbeatInProgress = true;

    timer.start();
    loop.exec();  // 启动事件循环

    if (timer.isActive())
    {  // 处理响应
        timer.stop();

        if (pReply->error() != QNetworkReply::NoError)
        {
            // 错误处理
            qDebug() << "onMsgPost4DeviceMgt Error String : " << pReply->errorString();
            m_isNetworkAccessible = false;
        }
        else {
//            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();

//            qDebug() << "onMsgGetHeartbeat Status Code : " << nStatusCode;
            m_isNetworkAccessible = true;
        }

        timeCount = 0;

        disconnect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    }
    else
    {  // 处理超时
        disconnect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));

        pReply->abort();
        pReply->deleteLater();

        timeCount++;

        //若由扣费或撤单请求进行中，则不重新初始化
        //对间歇性的timeout情况进行过滤，不重新初始化，连续心跳达到5次timeout，则重新初始化
        if (!m_httpRequestAborted || timeCount > 5)
        {
            qDebug() << "CThreadHttp::onMsgGetHeartbeat: Timeout";

            timeCount = 0;
            m_isNetworkAccessible = false;
            reInitNetwork();
        }
    }

    m_isHeartbeatInProgress = false;
}

// for quickpass support
void CThreadHttp::onMsgPostSignInData(QString t_data)
{
    QString url = m_strQuickpassSigninURL;
    qDebug() << "\nCThreadHttp::onMsgPostSignInData: " << url << t_data;

    QNetworkAccessManager *pManger = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    request.setUrl(QUrl(url));

    QByteArray t_SendData = t_data.toLocal8Bit();

    QNetworkReply *pReply = pManger->post(request, t_SendData);
    connect(pManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));

    QReplyTimeout *pTimeout = new QReplyTimeout(pReply, HTTP_DEFAULT_TIMEOUT);

    // 超时进一步处理
    connect(pTimeout, SIGNAL(timeout()), pTimeout, SLOT(onTimeout()));
}

void CThreadHttp::onMsgPost4DeviceMgt(QString path, QString json)
{
    QString url;
    url = "http://101.201.155.190:8080/trawe-itms/interface"+path;

    qDebug() << "CThreadHttp::onMsgPost4DeviceMgt: " << path << "/" << url << "\n" << json;

    QNetworkAccessManager *pManger = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl(url));

    QNetworkReply *pReply = pManger->post(request, json.toLocal8Bit());
    connect(pManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinishedDeviceMgt(QNetworkReply*)));

    QReplyTimeout *pTimeout = new QReplyTimeout(pReply, 1000);

    // 超时进一步处理
    connect(pTimeout, SIGNAL(timeout()), pTimeout, SLOT(onTimeout()));
}
