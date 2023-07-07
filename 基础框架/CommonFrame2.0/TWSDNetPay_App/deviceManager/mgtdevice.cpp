#include "mgtdevice.h"
#include "json/json.h"

#include <QDateTime>
#include <QDebug>

MGTDevice::MGTDevice(QObject *parent) : QObject(parent)
{

}

void MGTDevice::testMGTDevice()
{
    packHeartbeatMessage();
    packUpdateQueryMessage();
    packUpdateDownloadMessage();
    packUpdateConfirmMessage();
}

// pack json message
QString MGTDevice::packHeartbeatMessage()
{
//    {
//    aes:"12345dsdfasde",
//    md5:"XDDFGSERWWETD",
//    version:"1.0",
//    packageType:4，
//    time:'2017-03-12 10:01:01',
//    provinceId:44,
//    roadId:1001,
//    stationId:201,
//    laneId:6,
//    deviceNo:"",
//    deviceType:"",
//    state:0,
//    heartbeatTime:'2017-03-12 10:00:01'
//    }

    Json::FastWriter write;
    Json::Value root;

    root = getRootJsonNode();

    QString timestr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    root["heartbeatTime"] = timestr.toStdString();

    QString json = QString::fromStdString(write.write(root));
    qDebug() << "MGTDevice::packHeartbeatMessage: " << QString::fromStdString(root.toStyledString());

    return json;
}

QString MGTDevice::packUpdateQueryMessage()
{
    Json::FastWriter write;
    Json::Value root;

    root = getRootJsonNode();

    QString timestr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    root["infos"] = getQueryInfoList();
    root["queryTime"] = timestr.toStdString();

    qDebug() << "MGTDevice::packUpdateQueryMessage: " << QString::fromStdString(root.toStyledString());

    QString json = QString::fromStdString(write.write(root));
    return json;
}

QString MGTDevice::packUpdateDownloadMessage()
{
    qDebug() << "MGTDevice::packUpdateDownloadMessage";

    Json::FastWriter write;
    Json::Value root;

    root = getRootJsonNode();

    QString timestr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    root["infos"] = getDownloadInfoList();
    root["requstTime"] = timestr.toStdString();
    root["updateCode"] = "u123456789";

    qDebug() << "MGTDevice::packUpdateDownloadMessage: " << QString::fromStdString(root.toStyledString());

    QString json = QString::fromStdString(write.write(root));
    return json;
}

QString MGTDevice::packUpdateConfirmMessage()
{
    qDebug() << "MGTDevice::packUpdateConfirmMessage";

    Json::FastWriter write;
    Json::Value root;

    root = getRootJsonNode();
    root["updateCode"] = "u123456789";

    qDebug() << "MGTDevice::packUpdateConfirmMessage: " << QString::fromStdString(root.toStyledString());

    QString json = QString::fromStdString(write.write(root));
    return json;
}

Json::Value MGTDevice::getRootJsonNode()
{
//    {
//    aes:"12345dsdfasde",
//    md5:"XDDFGSERWWETD",
//    version:"1.0",
//    packageType:4，
//    time:'2017-03-12 10:01:01',
//    provinceId:44,
//    roadId:1001,
//    stationId:201,
//    laneId:6,
//    deviceNo:"",
//    deviceType:"",
//    state:0,
//    heartbeatTime:'2017-03-12 10:00:01'
//    }

    Json::Value root;

    QString timestr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    root["aes"] = "12345dsdfasde";
    root["md5"] = "XDDFGSERWWETD";
    root["version"] = "1.0";
    root["packageType"] = 4;
    root["provinceId"] = 44;
    root["roadId"] = 1001;
    root["stationId"] = 201;
    root["laneId"] = 6;
    root["deviceNo"] = "";
    root["deviceType"] = "";
    root["state"] = 0;
    root["time"] = timestr.toStdString();

    return root;
}

Json::Value MGTDevice::getQueryInfoList()
{
//queryTime:'2017-03-12 10:01:01',
//infos:[{
//    type:1,
//    infoNo:"",
//    infoFileName:"",
//    infoVersion:""},
//    {
//    type:2,
//    infoNo:"",
//    infoFileName:"",
//    infoVersion:""}
//]

    Json::Value root;
    Json::Value info;

    for (int i = 0; i < 3; i++) {
        info.clear();

        info["type"] = i;
        info["infoNo"] = "";
        info["infoFileName"] = "";
        info["infoVersion"] = "";

        root.append(info);
    }

    return root;
}

Json::Value MGTDevice::getDownloadInfoList()
{
    Json::Value root;
    Json::Value info;

    for (int i = 0; i < 2; i++) {
        info.clear();

        info["type"] = i;
        info["infoNo"] = "";
        info["infoFileName"] = "";
        info["infoVersion"] = "";

        root.append(info);
    }

    return root;
}
