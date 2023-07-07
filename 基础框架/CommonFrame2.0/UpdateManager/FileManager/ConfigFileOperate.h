#ifndef CONFIGFILEOPERATE_H
#define CONFIGFILEOPERATE_H
#include <QObject>
#include "json/json.h"
class ConfigFileOperate
{
public:
    ConfigFileOperate();

    QString parseVersionInfo(QString fileName);

    QString parseFilePathConfig();
    bool getJsonData(Json::Value &rootN,QString msgType,QString &result);
};

#endif // CONFIGFILEOPERATE_H
