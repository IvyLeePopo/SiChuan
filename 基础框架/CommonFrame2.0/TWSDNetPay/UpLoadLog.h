#ifndef UPLOADLOG_H
#define UPLOADLOG_H
#include "jsoncpp/json.h"
#include <QString>
#include<QDate>

typedef struct   tagPostLogInfo
{
    QString               aes;
    QString            md5;
    QString           version;
    int                      packageType;
    QString           time;
    int                     provinceId;
    int                   roadId;
    int                     stationId;
    int                  laneId;
    QString         deviceNo;
    QString         deviceType;
    int             state;
    QString           heartbeatTime;

}PostLogInfo, *pPostLogInfo;










class UpLoadLog
{

public:
    explicit  UpLoadLog();
    ~UpLoadLog();

    public:
    //初始化日志上传参数
    void setParam(const PostLogInfo& postloginfo) { memset(&m_PostLogInfo, 0x00, sizeof(PostLogInfo));  memcpy(&m_PostLogInfo, &postloginfo, sizeof(PostLogInfo)); }



    public:
    //组织心跳包的json数据
    QString  custructBeating();
    //组织获取执行命令的json数据
    QString custructGetCmd();
    //组织上传日志文件的json数据
    QString custructUpLoadLog();
    //组织上传日志文件片的json数据
    QString custructUpLoadPicfile();
    //组织命令确认的json数据
    QString custructCopyCmd();

private:
    //解析心跳回馈包的json数据
    void parseBeating(QString& beatingResult);
    //解析执行命令回馈的json数据
    void parseCmd(QString& cmdResult);
    //解析上传日志文件回馈的json数据
    void parseUpLoadLog(QString& uploadResult);
    //解析上传日志文件片回馈的json数据
    void parseUpLoadPicfile(QString& picfileResult);
    //解析命令确认回馈的json数据
    void parseCopyCmd(QString& cpCmdResult);

private:

    //属性
    PostLogInfo         m_PostLogInfo;

};

#endif // UPLOADLOG_H
