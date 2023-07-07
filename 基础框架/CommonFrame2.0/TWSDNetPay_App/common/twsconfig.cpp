#include "twsconfig.h"
#include <QDebug>
#include "json/json.h"

using namespace std;

TWSConfig *TWSConfig::self = NULL;

#define TERMINAL_INI_FILE   "/opt/scaner/etc/apptwsd.ini"

TWSConfig::TWSConfig()
{
    m_appConfig = new QSettings(TERMINAL_INI_FILE, QSettings::IniFormat);
}

// single instance
TWSConfig* TWSConfig::instance ()
{
    if (NULL == self) {
        self = new TWSConfig();
    }

    return self;
}

QSettings* TWSConfig::getAppConfig()
{
    return m_appConfig;
}

// read EEPROM and parse json data
bool TWSConfig::updateConfigFromHW()
{
    qDebug() << "TWSConfig::updateConfigFromHW";

    HTTWSDDevice* twsDeviceInf = HTTWSDDevice::instance();

//    twsDeviceInf->writeJsonEepromData("testing data form TWSConfig");
    QString json = twsDeviceInf->readJsonEepromData();

    // parse json
    parseJsonData(json);

    return true;
}

// parse json data from EEPROM
bool TWSConfig::parseJsonData(QString jsonstr)
{
    qDebug() << "TWSConfig::parseJsonData";

    //进行数据格式转换
    Json::Reader reader;
    Json::Value  root;
    Json::Value  eepromReadContext;
    Json::Value  Data;

    //    read eeprom acJosnBuf[364] = [{"EepromReadContext":{"Data":{"CanId":1,"AdvertisementId":4,"Scantime":30, \
    //            "HardwareVer":"V02.01.0000","MaxId":"2016101020161102171538","SnId":"87654321","DefaultGw":"eth0",\
    //            "Eth0IpAddr":"192.168.6.114","Eth0NetMask":"255.255.255.0","Eth0Gateway":"192.168.6.1",
    //            "Eth1IpAddr":"192.168.1.136","Eth1NetMask":"255.255.255.0","Eth1Gateway":"192.168.1.1"},"Version":"1.0"}}]


    QString strResult(jsonstr);
    if (!reader.parse(strResult.toLocal8Bit().data(), root)) {
        strResult.clear();
        qDebug() << "TWSConfig::parseJsonData: json parser error";
        return false;
    }

    if (Json::nullValue == root.type()) {
        qDebug() << "root.type() is Json::nullValue";
        return false;
    }

    //获取交易数据区
    if(Json::nullValue == root["EepromReadContext"].type()) {
        return false;
    }

    eepromReadContext = root["EepromReadContext"];

    //获取数据明细区
    if(Json::nullValue == eepromReadContext["Data"].type())
        return false;

    Data = eepromReadContext["Data"];

    m_hwVer = Data["HardwareVer"].asCString();
    m_maxId = Data["MaxId"].asCString();
    m_snId = Data["SnId"].asCString();
    m_defaultGW = Data["DefaultGw"].asCString();
    m_eth0Ip = Data["Eth0IpAddr"].asCString();
    m_eth1Ip = Data["Eth1IpAddr"].asCString();
    m_eth0Gateway = Data["Eth0Gateway"].asCString();
    m_eth1Gateway = Data["Eth1Gateway"].asCString();

    qDebug() << "hardware Version: " << m_hwVer;
    qDebug() << "Max ID: " << m_maxId;
    qDebug() << "SN ID : " << m_snId;
    qDebug() << "default GW: " << m_defaultGW;
    qDebug() << "eth0: " << m_eth0Ip << "/" << m_eth0Gateway;
    qDebug() << "eth1: " << m_eth1Ip << "/" << m_eth1Gateway;

    //写入ini配置
    QSettings* config = m_appConfig;
    config->setValue("Terminal/HardwareVer", m_hwVer);
    config->setValue("Terminal/MaxId", m_maxId);
    config->setValue("Terminal/DeviceNo", m_snId);
    config->setValue("Terminal/DeviceType", "TR300");

    return true;
}

void TWSConfig::setPayDllVersion(QString ver)
{
    m_payDllVersion = ver;
}

void TWSConfig::setHTTWSDllVersion(QString ver)
{
    m_httwsdDllVersion = ver;
}

QString TWSConfig::getPayDllVersion()
{
    return m_payDllVersion;
}

QString TWSConfig::getHTTWSDllVersion()
{
    return m_httwsdDllVersion;
}

void TWSConfig::setPayStationId(QString id)
{
    m_payStationId = id;
}

QString TWSConfig::getPayStationId()
{
    return m_payStationId;
}

QString TWSConfig::getSelfTestJsonData()
{
//    {
//    "DeviceUploadContext":{
//      "Data":{
//        "vendor": “123456”,
//        "model": “123456”,
//        "deviceNo": “123456”,
//        "hwVer": “123456”,
//        "swVer": “123456”,
//        "msgVer": “123456”,
//        "status": “1”,
//        "isConcatenation”: “0”,
//        "isNFC”: “0”,
//        "Eth0IP":"192.168.5.255",
//        "Eth1IP":"192.168.5.255",
//        "Wireless":"4G",
//        "Wirelesstate": “1”,
//       },

//       "Version":"1.0"
//        }
//    }

    // 表示整个 json 对象
    Json::Value root;
    // 标识用户参数
    Json::Value DeviceUploadContext;

    // 标识详细数据项
    Json::Value Data;
    Json::FastWriter t_RootDocument;

    // DeviceUploadContext
    DeviceUploadContext["Version"] = "1.0";

    // Data
    Data["vendor"] = m_maxId.toStdString();
    Data["model"] = "123456";
    Data["deviceNo"] = m_snId.toStdString();
    Data["hwVer"] = m_hwVer.toStdString();
    Data["swVer"] = "123456";
    Data["msgVer"] = "123456";

    Data["status"] = "1";
    Data["isConcatenation”"] = "0";
    Data["isNFC”"] = "0";
    Data["Eth0IP"] = m_eth0Ip.toStdString();
    Data["Eth1IP"] = m_eth1Ip.toStdString();
    Data["Wireless"] = "4G";
    Data["Wirelesstate"] = "0";

    DeviceUploadContext["Data"] = Data;
    root["DeviceUploadContext"] = DeviceUploadContext;

    QString strReturn = QString::fromStdString(t_RootDocument.write(root));

    return strReturn;
}
