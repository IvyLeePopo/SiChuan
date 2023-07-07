#include "UpLoadLog.h"

UpLoadLog::UpLoadLog()
{
}

UpLoadLog::~UpLoadLog()
{

}

QString UpLoadLog::custructBeating()
{
    Json::Value root;

    root["aes"]                           = Json::Value(m_PostLogInfo.aes.toStdString());
    root["md5"]                         = Json::Value(m_PostLogInfo.md5.toStdString());
    root["version"]                    = Json::Value(m_PostLogInfo.version.toStdString());
    root["packageType"]         =  Json::Value(m_PostLogInfo.packageType);
    root["time"]                       =    Json::Value(m_PostLogInfo.time.toStdString());
    root["provinceId"]                     = Json::Value(m_PostLogInfo.provinceId);
    root["roadId"]                    = Json::Value(m_PostLogInfo.roadId);
    root["stationId"]               = Json::Value(m_PostLogInfo.stationId);
    root["laneId"]              = Json::Value(m_PostLogInfo.laneId);
    root["deviceNo"]                = Json::Value(m_PostLogInfo.deviceNo.toStdString());
    root["deviceType"]              = Json::Value(m_PostLogInfo.deviceType.toStdString());
    root["state"]                   = Json::Value(m_PostLogInfo.state);
    root["heartbeatTime"]               = Json::Value(m_PostLogInfo.heartbeatTime.toStdString());

    Json::FastWriter  writer;

    QString strData = QString::fromStdString(writer.write(root));

    return strData;

}

QString UpLoadLog::custructGetCmd()
{
    Json::Value root;

    root["aes"]                           = Json::Value(m_PostLogInfo.aes.toStdString());
    root["md5"]                         = Json::Value(m_PostLogInfo.md5.toStdString());
    root["version"]                    = Json::Value(m_PostLogInfo.version.toStdString());
    root["packageType"]         =  Json::Value(m_PostLogInfo.packageType);
    root["time"]                       =    Json::Value(m_PostLogInfo.time.toStdString());
    root["provinceId"]                     = Json::Value(m_PostLogInfo.provinceId);
    root["roadId"]                    = Json::Value(m_PostLogInfo.roadId);
    root["stationId"]               = Json::Value(m_PostLogInfo.stationId);
    root["laneId"]              = Json::Value(m_PostLogInfo.laneId);
    root["deviceNo"]                = Json::Value(m_PostLogInfo.deviceNo.toStdString());
    root["deviceType"]              = Json::Value(m_PostLogInfo.deviceType.toStdString());
    root["state"]                   = Json::Value(m_PostLogInfo.state);

    root["cmdId"] =         "String";

    Json::FastWriter  writer;

    QString strData = QString::fromStdString(writer.write(root));

    return strData;
}

QString UpLoadLog::custructUpLoadLog()
{
    Json::Value root;

    root["aes"]                           = Json::Value(m_PostLogInfo.aes.toStdString());
    root["md5"]                         = Json::Value(m_PostLogInfo.md5.toStdString());
    root["version"]                    = Json::Value(m_PostLogInfo.version.toStdString());
    root["packageType"]         =  Json::Value(m_PostLogInfo.packageType);
    root["time"]                       =    Json::Value(m_PostLogInfo.time.toStdString());
    root["provinceId"]                     = Json::Value(m_PostLogInfo.provinceId);
    root["roadId"]                    = Json::Value(m_PostLogInfo.roadId);
    root["stationId"]               = Json::Value(m_PostLogInfo.stationId);
    root["laneId"]              = Json::Value(m_PostLogInfo.laneId);
    root["deviceNo"]                = Json::Value(m_PostLogInfo.deviceNo.toStdString());
    root["deviceType"]              = Json::Value(m_PostLogInfo.deviceType.toStdString());

    root["cmdId"]               = "string";
    root["fileType"]            =1;
    root["fileName"]            = "string";
    root["fileSize"]                = 1;

    Json::FastWriter  writer;

    QString strData = QString::fromStdString(writer.write(root));

    return strData;
}

QString UpLoadLog::custructUpLoadPicfile()
{
    Json::Value root;

    root["aes"]                           = Json::Value(m_PostLogInfo.aes.toStdString());
    root["md5"]                         = Json::Value(m_PostLogInfo.md5.toStdString());
    root["version"]                    = Json::Value(m_PostLogInfo.version.toStdString());
    root["packageType"]         =  Json::Value(m_PostLogInfo.packageType);
    root["time"]                       =    Json::Value(m_PostLogInfo.time.toStdString());
    root["provinceId"]                     = Json::Value(m_PostLogInfo.provinceId);
    root["roadId"]                    = Json::Value(m_PostLogInfo.roadId);
    root["stationId"]               = Json::Value(m_PostLogInfo.stationId);
    root["laneId"]              = Json::Value(m_PostLogInfo.laneId);
    root["deviceNo"]                = Json::Value(m_PostLogInfo.deviceNo.toStdString());
    root["deviceType"]              = Json::Value(m_PostLogInfo.deviceType.toStdString());

    root["cmdId"]               = "string";

    root["fileSeq"]             =1;
    root["md5"]                ="string";
    root["fileContent"]       = "String";

    Json::FastWriter  writer;

    QString strData = QString::fromStdString(writer.write(root));

    return strData;
}

QString UpLoadLog::custructCopyCmd()
{
    Json::Value root;

    root["aes"]                           = Json::Value(m_PostLogInfo.aes.toStdString());
    root["md5"]                         = Json::Value(m_PostLogInfo.md5.toStdString());
    root["version"]                    = Json::Value(m_PostLogInfo.version.toStdString());
    root["packageType"]         =  Json::Value(m_PostLogInfo.packageType);
    root["time"]                       =    Json::Value(m_PostLogInfo.time.toStdString());
    root["provinceId"]                     = Json::Value(m_PostLogInfo.provinceId);
    root["roadId"]                    = Json::Value(m_PostLogInfo.roadId);
    root["stationId"]               = Json::Value(m_PostLogInfo.stationId);
    root["laneId"]              = Json::Value(m_PostLogInfo.laneId);
    root["deviceNo"]                = Json::Value(m_PostLogInfo.deviceNo.toStdString());
    root["deviceType"]              = Json::Value(m_PostLogInfo.deviceType.toStdString());

    root["cmdId"]               = "string";

    root["cmdResp"]        = "String";
    root["updateState"]	 = "String";

    Json::FastWriter  writer;

    QString strData = QString::fromStdString(writer.write(root));

    return strData;
}




void UpLoadLog::parseBeating(QString& beatingResult)
{
    Json::Value root;
    Json::Reader reader;
    QString strError;
    QString strResult(beatingResult);

    try
    {
        if(!reader.parse(strResult.toLocal8Bit().data(), root))
        {
            throw -1;
        }

        if( root["version"].type() != Json::stringValue)
            throw -2;
        QString strVer = root["version"].asCString();

        if(root["packageType"].type() != Json::intValue)
            throw -3;
        int pkType = root["packageType"].asInt();

        if(root["recvTime"].type() != Json::stringValue)//time  yyyy-MM-dd HH:mm:ss.SSS
            throw -4;
        QString strRecvTime = root["recvTime"].asCString();

        if(root["responseTime"].type() != Json::stringValue)//time yyyy-MM-dd HH:mm:ss.SSS
            throw -5;
        QString strResponseTime = root["responseTime"].asCString();

        if(root["cmdList"].type() != Json::stringValue)
            throw -6;
        QString strCmdList = root["cmdList"].asCString();

        if(root["errorCode"].type() != Json::stringValue)
            throw -7;
        QString strErrorCode = root["errorCode"].asCString();

        if(root["errorMsg"].type() != Json::stringValue)
            throw -8;
        QString strErrorMsg = root["errorMsg"].asCString();

    }
    catch(int& error)
    {
        switch(error)
        {
        case -1:
            strError = QString("解析心跳包数据失败，json数据解析失败");

            break;

        case -2:
            strError = QString("解析心跳包数据失败，无法解析 version");
            break;
        case -3:
            strError = QString("解析心跳包数据失败，无法解析 packageType");
            break;
        case -4:
            strError = QString("解析心跳包数据失败，无法解析 recvTime");
            break;
        case -5:
            strError = QString("解析心跳包数据失败，无法解析 responseTime");
            break;
        case -6:
            strError = QString("解析心跳包数据失败，无法解析 cmdList");
            break;
        case -7:
            strError = QString("解析心跳包数据失败，无法解析 errorCode");
            break;
        case -8:
            strError = QString("解析心跳包数据失败，无法解析 errorMsg");
            break;
        default:
            break;
        }
    }

}

void UpLoadLog::parseCmd(QString& cmdResult)
{
    Json::Value root;
    Json::Reader reader;
    QString strError;
    QString strResult(cmdResult);

    try
    {
        if(!reader.parse(strResult.toLocal8Bit().data(), root))
        {
            throw -1;
        }

        if( root["version"].type() != Json::stringValue)
            throw -2;
        QString strVer = root["version"].asCString();

        if(root["packageType"].type() != Json::intValue)
            throw -3;
        int pkType = root["packageType"].asInt();

        if(root["recvTime"].type() != Json::stringValue)//time  yyyy-MM-dd HH:mm:ss.SSS
            throw -4;
        QString strRecvTime = root["recvTime"].asCString();

        if(root["responseTime"].type() != Json::stringValue)//time yyyy-MM-dd HH:mm:ss.SSS
            throw -5;
        QString strResponseTime = root["responseTime"].asCString();

        if(root["cmdType"].type() != Json::stringValue)
            throw -6;
        QString strCmdType = root["cmdType"].asCString();

        if(root["cmdParam"].type() != Json::stringValue)
            throw -7;
        QString strCmdParam = root["cmdParam"].asCString();

        if(root["errorCode"].type() != Json::stringValue)
            throw -8;
        QString strErrorCode = root["errorCode"].asCString();

        if(root["errorMsg"].type() != Json::stringValue)
            throw -9;
        QString strErrorMsg = root["errorMsg"].asCString();


    }
    catch(int& error)
    {
        switch(error)
        {
        case -1:
            strError = QString("解析执行命令包数据失败，json 数据解析失败 ");

            break;

        case -2:
            strError = QString("解析执行命令包数据失败，无法解析 version");
            break;
        case -3:
            strError = QString("解析执行命令包数据失败，无法解析 packageType");
            break;
        case -4:
            strError = QString("解析执行命令包数据失败，无法解析 recvTime");
            break;
        case -5:
            strError = QString("解析执行命令包数据失败，无法解析 responseTime");
            break;
        case -6:
            strError = QString("解析执行命令包数据失败，无法解析 cmdType");
            break;
        case -7:
            strError = QString("解析执行命令包数据失败，无法解析 cmdParam");
            break;
        case -8:
            strError = QString("解析执行命令包数据失败，无法解析 errorCode");
            break;
        case -9:
            strError = QString("解析执行命令包数据失败，无法解析 errorMsg");
            break;
        default:
            break;
        }
    }



}

void UpLoadLog::parseUpLoadLog(QString& uploadResult)
{
    Json::Value root;
    Json::Reader reader;
    QString strError;
    QString strResult(uploadResult);

    try
    {
        if(!reader.parse(strResult.toLocal8Bit().data(), root))
        {
            throw -1;
        }

        if( root["version"].type() != Json::stringValue)
            throw -2;
        QString strVer = root["version"].asCString();

        if(root["packageType"].type() != Json::intValue)
            throw -3;
        int pkType = root["packageType"].asInt();


        if(root["piece"].type() != Json::intValue)
            throw -4;
          int nPiece = root["piece"].asInt();


        if(root["recvTime"].type() != Json::stringValue)//time  yyyy-MM-dd HH:mm:ss.SSS
            throw -5;
        QString strRecvTime = root["recvTime"].asCString();


        if(root["responseTime"].type() != Json::stringValue)//time yyyy-MM-dd HH:mm:ss.SSS
            throw -6;
        QString strResponseTime = root["responseTime"].asCString();


        if(root["errorCode"].type() != Json::stringValue)
            throw -7;
        QString strErrorCode = root["errorCode"].asCString();

        if(root["errorMsg"].type() != Json::stringValue)
            throw -8;
        QString strErrorMsg = root["errorMsg"].asCString();


    }
    catch(int& error)
    {
        switch(error)
        {
        case -1:
            strError = QString("解析日志上传包数据失败，json 数据解析失败 ");

            break;

        case -2:
            strError = QString("解析日志上传包数据失败，无法解析 version");
            break;
        case -3:
            strError = QString("解析日志上传包数据失败，无法解析 packageType");
            break;
        case -4:
            strError = QString("解析日志上传包数据失败，无法解析 piece");
            break;
        case -5:
            strError = QString("解析日志上传包数据失败，无法解析 recvTime");
            break;
        case -6:
            strError = QString("解析日志上传包数据失败，无法解析 responseTime");
            break;
        case -7:
            strError = QString("解析日志上传包数据失败，无法解析 errorCode");
            break;
        case -8:
            strError = QString("解析日志上传包数据失败，无法解析 errorMsg");
            break;
        default:
            break;
        }
    }


}

void UpLoadLog::parseUpLoadPicfile(QString& picfileResult)
{
    Json::Value root;
    Json::Reader reader;
    QString strError;
    QString strResult(picfileResult);

    try
    {
        if(!reader.parse(strResult.toLocal8Bit().data(), root))
        {
            throw -1;
        }

        if( root["version"].type() != Json::stringValue)
            throw -2;
        QString strVer = root["version"].asCString();

        if(root["packageType"].type() != Json::intValue)
            throw -3;
        int pkType = root["packageType"].asInt();


        if(root["piece"].type() != Json::intValue)
            throw -4;
          int nPiece = root["piece"].asInt();


        if(root["recvTime"].type() != Json::stringValue)//time  yyyy-MM-dd HH:mm:ss.SSS
            throw -5;
        QString strRecvTime = root["recvTime"].asCString();


        if(root["responseTime"].type() != Json::stringValue)//time yyyy-MM-dd HH:mm:ss.SSS
            throw -6;
        QString strResponseTime = root["responseTime"].asCString();


        if(root["errorCode"].type() != Json::stringValue)
            throw -7;
        QString strErrorCode = root["errorCode"].asCString();

        if(root["errorMsg"].type() != Json::stringValue)
            throw -8;
        QString strErrorMsg = root["errorMsg"].asCString();


    }
    catch(int& error)
    {
        switch(error)
        {
        case -1:
            strError = QString("解析日志文件片上传包数据失败，json 数据解析失败 ");

            break;

        case -2:
            strError = QString("解析日志文件片上传包数据失败，无法解析 version");
            break;
        case -3:
            strError = QString("解析日志文件片上传包数据失败，无法解析 packageType");
            break;
        case -4:
            strError = QString("解析日志文件片上传包数据失败，无法解析 piece");
            break;
        case -5:
            strError = QString("解析日志文件片上传包数据失败，无法解析 recvTime");
            break;
        case -6:
            strError = QString("解析日志文件片上传包数据失败，无法解析 responseTime");
            break;
        case -7:
            strError = QString("解析日志文件片上传包数据失败，无法解析 errorCode");
            break;
        case -8:
            strError = QString("解析日志文件片上传包数据失败，无法解析 errorMsg");
            break;
        default:
            break;
        }
    }


}

void UpLoadLog::parseCopyCmd(QString& cpCmdResult)
{
    Json::Value root;
    Json::Reader reader;
    QString strError;
    QString strResult(cpCmdResult);

    try
    {
        if(!reader.parse(strResult.toLocal8Bit().data(), root))
        {
            throw -1;
        }

        if( root["version"].type() != Json::stringValue)
            throw -2;
        QString strVer = root["version"].asCString();

        if(root["packageType"].type() != Json::intValue)
            throw -3;
        int pkType = root["packageType"].asInt();



        if(root["recvTime"].type() != Json::stringValue)//time  yyyy-MM-dd HH:mm:ss.SSS
            throw -4;
        QString strRecvTime = root["recvTime"].asCString();


        if(root["responseTime"].type() != Json::stringValue)//time yyyy-MM-dd HH:mm:ss.SSS
            throw -5;
        QString strResponseTime = root["responseTime"].asCString();


        if(root["errorCode"].type() != Json::stringValue)
            throw -6;
        QString strErrorCode = root["errorCode"].asCString();

        if(root["errorMsg"].type() != Json::stringValue)
            throw -7;
        QString strErrorMsg = root["errorMsg"].asCString();


    }
    catch(int& error)
    {
        switch(error)
        {
        case -1:
            strError = QString("解析确认命令包数据失败，json 数据解析失败 ");

            break;

        case -2:
            strError = QString("解析确认命令包数据失败，无法解析 version");
            break;
        case -3:
            strError = QString("解析确认命令包数据失败，无法解析 packageType");
            break;
        case -4:
            strError = QString("解析确认命令包数据失败，无法解析 recvTime");
            break;
        case -5:
            strError = QString("解析确认命令包数据失败，无法解析 responseTime");
            break;
        case -6:
            strError = QString("解析确认命令包数据失败，无法解析 errorCode");
            break;
        case -7:
            strError = QString("解析确认命令包数据失败，无法解析 errorMsg");
            break;
        default:
            break;
        }
    }



}
