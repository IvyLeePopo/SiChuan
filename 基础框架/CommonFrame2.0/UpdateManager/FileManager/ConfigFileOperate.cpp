#include "ConfigFileOperate.h"

#include "UpdateManagerGlobal.h"
#include <QFile>
#include <QDebug>

#define DOWNLOAD_STRING  "download"
#define LOADLIB_STRING  "loadLib"
#define LIB_STRING  "lib"
#define LOG_STRING  "log"


ConfigFileOperate::ConfigFileOperate()
{

}

QString ConfigFileOperate::parseVersionInfo(QString fileName)
{
    QFile file(VERSION_CONFIG_FILE_OPERATE_PATH);
    if( !file.open(QFile::ReadOnly | QFile::Text))
    {
       qDebug()<<QObject::tr("ConfigFileOperate::parseVersionInfo->%s\n") << VERSION_CONFIG_FILE_OPERATE_PATH;
       return false;
    }
    QByteArray readData = file.readAll();
    file.close();

    Json::Value rootN;
    Json::Reader  reader;
    if (!reader.parse(readData.data() , rootN))
    {
      qDebug()<<"ERROR:[#ConfigFileOperate::parseVersionInfo#]解析失败";
      return false;
    }
    Json::Value libUpdateManagerCode = rootN[fileName.toLocal8Bit().data()];
    if ( libUpdateManagerCode.type() != Json::stringValue )
    {
      qDebug()<<"ERROR:[#ConfigFileOperate::parseVersionInfo#]libUpdateManagerCode error";
      return "";
    }
    else
    {
        QString updateManagerCode = rootN[fileName.toLocal8Bit().data()].asCString();
        return updateManagerCode;
    }
}

QString ConfigFileOperate::parseFilePathConfig()
{
    printf("1.\n");
    QFile file(FILE_PATH_CONFIG_PATH);
    if( !file.open(QFile::ReadOnly | QFile::Text))
    {
       qDebug()<<QObject::tr("ConfigFileOperate::parseFilePathConfig->%s\n") << FILE_PATH_CONFIG_PATH;
       return "";
    }
    printf("2.\n");

    QByteArray readData = file.readAll();
    file.close();

    Json::Value rootN;
    Json::Reader  reader;
    if (!reader.parse(readData.data() , rootN))
    {
      qDebug()<<"ERROR:[#ConfigFileOperate::parseFilePathConfig#]解析失败";
      return "";
    }
   getJsonData(rootN,DOWNLOAD_STRING,g_ST_UPDATE_CONFIG_INFO.downloadPath);
   getJsonData(rootN,LOADLIB_STRING,g_ST_UPDATE_CONFIG_INFO.loadLibPath);
   getJsonData(rootN,LIB_STRING,g_ST_UPDATE_CONFIG_INFO.libPath);
   getJsonData(rootN,LOG_STRING,g_ST_UPDATE_CONFIG_INFO.logPath);
   printf("DOWNLOAD_STRING=%s.\n",g_ST_UPDATE_CONFIG_INFO.downloadPath.toLocal8Bit().data());
   printf("LOADLIB_STRING=%s.\n",g_ST_UPDATE_CONFIG_INFO.loadLibPath.toLocal8Bit().data());
   printf("LIB_STRING=%s.\n",g_ST_UPDATE_CONFIG_INFO.libPath.toLocal8Bit().data());
   printf("LOG_STRING=%s.\n",g_ST_UPDATE_CONFIG_INFO.logPath.toLocal8Bit().data());
   return "";
}

bool ConfigFileOperate::getJsonData(Json::Value &rootN,QString msgType, QString &result)
{

    Json::Value loadLibCode = rootN[msgType.toLocal8Bit().data()];
    if ( loadLibCode.type() != Json::stringValue )
    {
      qDebug()<<"ERROR:[#ConfigFileOperate::parseFilePathConfig#]loadLibCode error"<<msgType;
      return false;
    }
    else
    {
       result = rootN[msgType.toLocal8Bit().data()].asCString();
       return true;
    }
}
