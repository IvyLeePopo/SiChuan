#include "devicemgtthread.h"

DeviceMgtThread *DeviceMgtThread::self = NULL;

DeviceMgtThread::DeviceMgtThread(QThread *parent) : QThread(parent)
{

}

// single instance
DeviceMgtThread* DeviceMgtThread::instance()
{
    if (NULL == self) {
        self = new DeviceMgtThread();
    }

    return self;
}

// set httpThread context
void DeviceMgtThread::setHttpContext(CThreadHttp *http)
{
    m_httpThread = http;
    m_mgtDevice = new MGTDevice(this);

    initConnect();

    m_heartbeatTimer = new QTimer;
    connect(m_heartbeatTimer, SIGNAL(timeout()), this, SLOT(slHeartbeatTimerTimeout()));

    m_heartbeatTimer->start(1000*18);
}

// connect signals/slots
void DeviceMgtThread::initConnect()
{
    connect(m_httpThread, SIGNAL(WM_THREAD_HTTP_DEVICE_MGT(QNetworkReply*)), this, SLOT(slMsgDeviceMgtArrial(QNetworkReply*)));
    connect(this, SIGNAL(sigMgtMessage2Http(QString,QString)), m_httpThread, SLOT(onMsgPost4DeviceMgt(QString,QString)));
}

// mian loop
void DeviceMgtThread::run()
{
    qDebug() << "enter DeviceMgtThread main loop";
}

// handle message from http server
void DeviceMgtThread::slMsgDeviceMgtArrial(QNetworkReply* pReply)
{
    qDebug() << "DeviceMgtThread::slMsgDeviceMgtArrial: " << pReply->url().toString();

    if (pReply->error() != QNetworkReply::NoError) {
        // 错误处理
        qDebug() << "Error String : " << pReply->errorString();
    }
    else {
        QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        int nStatusCode = variant.toInt();

        // 根据状态码做进一步数据处理
        QByteArray bytes = pReply->readAll();

        qDebug() << "slMsgDeviceMgtArrial Status Code : " << nStatusCode << "/" << bytes;
    }

    pReply->deleteLater();
}

// post heartbeat request
void DeviceMgtThread::slHeartbeatTimerTimeout()
{
    return;

    qDebug() << "DeviceMgtThread::slHeartbeatTimerTimeout";

    QString path = "/t_heartbeat";
    QString json = m_mgtDevice->packHeartbeatMessage();

    emit sigMgtMessage2Http(path, json);
}
