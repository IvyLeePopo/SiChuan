#ifndef MGTDEVICE_H
#define MGTDEVICE_H

#include <QObject>
#include "json/json.h"

class MGTDevice : public QObject
{
    Q_OBJECT
public:
    explicit MGTDevice(QObject *parent = 0);

    void testMGTDevice();

    QString packHeartbeatMessage();
    QString packUpdateQueryMessage();
    QString packUpdateDownloadMessage();
    QString packUpdateConfirmMessage();

private:
    Json::Value getQueryInfoList();
    Json::Value getRootJsonNode();
    Json::Value getDownloadInfoList();

signals:

public slots:
};

#endif // MGTDEVICE_H
