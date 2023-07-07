#include "UpdateFromHttpManager.h"
#include "UpdateManagerGlobal.h"
#include <QSettings>
#include <QObject>
#include "UpdateHttpTimeOut.h"
#include "TestResponse.h"



#define   QUERY_URL     "/trawe-itms/middleware/u_update_query"
#define   DOWNDLOAD_URL  "/trawe-itms/middleware/u_update_download"
#define   CONFIRM_URL    "/trawe-itms/middleware/u_update_confirm"

UpdateFromHttpManager *UpdateFromHttpManager::self=NULL;

UpdateFromHttpManager *UpdateFromHttpManager::instance()
{
    if(self==NULL)
        self = new UpdateFromHttpManager();
    return self;
}


UpdateFromHttpManager::UpdateFromHttpManager()
{
    qDebug()<<"UpdateFromHttpManager init6";
    register2Bus(KEY_UPDATE_HTTP_MANAGER);
    pManger = new QNetworkAccessManager();
}

UpdateFromHttpManager::~UpdateFromHttpManager()
{
    unRegister2Bus(KEY_UPDATE_HTTP_MANAGER);
    delete pManger;
}

int UpdateFromHttpManager::recvFromBus(int destKey,char *msg,int len, int srcKey)
{
    getServerIpInfo();
    QString url = "http://"+m_serverIp+":"+QString::number(m_port);
    if(srcKey ==KEY_UPDATE_QUERY)
        url.append(QUERY_URL);
    else if (srcKey ==KEY_UPDATE_DOWNLOAD)
        url.append(DOWNDLOAD_URL);
    else if (srcKey ==KEY_UPDATE_CONFIRM)
        url.append(CONFIRM_URL);
    else
        url.append(QUERY_URL);

    //postUpdateRequest(url,msg);
    // test
    testHttp(srcKey);
}

void UpdateFromHttpManager::testHttp(int type)
{
    TestResponse myTestResponse;
    if(type==KEY_UPDATE_QUERY)
    {
        QString qeuryData = myTestResponse.generateQueryResponse();
        send2Bus(KEY_UPDATE_QUERY,qeuryData.toLocal8Bit().data(),qeuryData.toLocal8Bit().length(),KEY_UPDATE_HTTP_MANAGER);
    }
    if(type==KEY_UPDATE_DOWNLOAD)
    {
        QString downloadData = myTestResponse.generateDownLoadResponse();
        send2Bus(KEY_UPDATE_DOWNLOAD,downloadData.toLocal8Bit().data(),downloadData.toLocal8Bit().length(),KEY_UPDATE_HTTP_MANAGER);

    }
}

void UpdateFromHttpManager::recvFromHttp(QNetworkReply *reply)
{
    QString path = reply->url().path();
    QVariant variant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int nStatusCode = variant.toInt();

    int key;

    if (0 == path.compare(QUERY_URL, Qt::CaseSensitive))
        key = KEY_UPDATE_QUERY;
    else if (0 == path.compare(DOWNDLOAD_URL, Qt::CaseSensitive))
        key = KEY_UPDATE_DOWNLOAD;
    else if (0 == path.compare(CONFIRM_URL, Qt::CaseSensitive))
        key = KEY_UPDATE_CONFIRM;
    else
        key = KEY_UPDATE_QUERY;

    qDebug()<<"收到后台回馈";
    qDebug()<<"类型: "<<key<<",状态码: "<<nStatusCode;

    if(nStatusCode == 200)
    {
        QByteArray t_Data=reply->readAll();
        send2Bus(key,t_Data.data(),t_Data.length(),KEY_UPDATE_HTTP_MANAGER);
    }

}



void UpdateFromHttpManager::postUpdateRequest(QString url, QString json)
{
    qDebug()<<"[#UpdateFromHttpManager::postUpdateRequest#]发送url地址:"<<url;
    qDebug()<<"[#UpdateFromHttpManager::postUpdateRequest#]HTTP发送数据:"<<json;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl(url));

    QNetworkReply *pReply = pManger->post(request, json.toLocal8Bit());
    QObject::connect(pManger, SIGNAL(finished(QNetworkReply*)), this, SLOT(recvFromHttp(QNetworkReply*)));

    //超时时间设置为３０Ｓ
    UpdateHttpTimeOut *pTimeout = new UpdateHttpTimeOut(pReply, 30000);
    // 超时进一步处理
    connect(pTimeout, SIGNAL(timeout()), pTimeout, SLOT(onTimeout()));
}

void UpdateFromHttpManager::getServerIpInfo()
{
    QSettings *configIniRead = new QSettings(TERMINAL_INI_FILE, QSettings::IniFormat);
    if (configIniRead)
    {
        m_serverIp = configIniRead->value("/Server/ServerIP").toString();
        m_port = configIniRead->value("/Server/UpdatePort").toInt();
        delete configIniRead;
    }

    if (m_serverIp.isEmpty())
    {
        qDebug() << "未收到车道初始化指令,无升级服务器配置.等待初始化指令(服务器配置)!";
    }

}
