#ifndef UPDATEFROMHTTPMANAGER_H
#define UPDATEFROMHTTPMANAGER_H

#include <CommonBus.h>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
class UpdateFromHttpManager:public QObject,public CommonBus
{
    Q_OBJECT
public:
        static UpdateFromHttpManager* instance();
        static UpdateFromHttpManager*self;
        virtual ~UpdateFromHttpManager();
private:
    UpdateFromHttpManager();
public:
    int recvFromBus(int destKey,char *msg,int len, int srcKey);
    void postUpdateRequest(QString url, QString json);

    void getServerIpInfo();

    QString m_serverIp;
    int     m_port;


    QNetworkAccessManager *pManger;


public slots:
    void recvFromHttp(QNetworkReply *reply);


public:

    void testHttp(int type);

};

#endif // UPDATEFROMHTTPMANAGER_H
