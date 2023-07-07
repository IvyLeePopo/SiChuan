#ifndef TWSCONFIG_H
#define TWSCONFIG_H

#include "httwsddevice.h"
#include <QString>
#include <string>
#include <QSettings>

using namespace std;

class TWSConfig
{
private:
    TWSConfig();

public:
    static TWSConfig* instance();

public:
    bool updateConfigFromHW();
    QString getSelfTestJsonData();

    void setPayDllVersion(QString ver);
    void setHTTWSDllVersion(QString ver);
    void setPayStationId(QString id);

    QString getPayDllVersion();
    QString getHTTWSDllVersion();
    QString getPayStationId();

    //QSetting inf
    QSettings* getAppConfig();

private:
    bool parseJsonData(QString str);

private:
    QString m_hwVer;
    QString m_maxId;
    QString m_snId;
    QString m_defaultGW;
    QString m_eth0Ip;
    QString m_eth1Ip;
    QString m_eth0Gateway;
    QString m_eth1Gateway;

    static TWSConfig *self;

    // dll version
    QString m_payDllVersion;
    QString m_httwsdDllVersion;

    // pay station info
    QString m_payStationId;

    //
    QSettings* m_appConfig;
};

#endif // TWSCONFIG_H
