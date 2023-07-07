#ifndef DEVICEMGTTHREAD_H
#define DEVICEMGTTHREAD_H

#include <QThread>
#include <QNetworkReply>
#include <QTimer>

#include "cthreadhttp.h"
#include "mgtdevice.h"

class DeviceMgtThread : public QThread
{
    Q_OBJECT
public:
    static DeviceMgtThread* instance();
    void setHttpContext(CThreadHttp* http);

    virtual void run();

private:
    explicit DeviceMgtThread(QThread *parent = 0);
    void initConnect();

signals:
    void sigMgtMessage2Http(QString path, QString json);

public slots:
    void slMsgDeviceMgtArrial(QNetworkReply* reply);
    void slHeartbeatTimerTimeout();

private:
    static DeviceMgtThread *self;

    CThreadHttp* m_httpThread;
    QTimer* m_heartbeatTimer;

    MGTDevice* m_mgtDevice;
};

#endif // DEVICEMGTTHREAD_H
