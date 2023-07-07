#ifndef UPDATEMANAGERGLOBAL_H
#define UPDATEMANAGERGLOBAL_H

#include <QObject>

typedef struct ST_BUS_MSG
{
    int  type;
    char *pMsg;
}BUS_MSG;



typedef struct ST_UPDATE_INIT_INFO_T
{
    int  mProvinceId;
    char mRoadId[32];
    char mStationId[32];
    char  mLanId[32];
}ST_UPDATE_INIT_INFO;

#define INIT_INFO_ERROR   0
#define TERMINAL_INI_FILE    "../etc/apptwsd.ini"
#define XML_CONFIG_FILE_PATH "../etc/version_info.txt"
//#define XML_UPDATE_CONFIG_FILE_PATH "../etc/updateVersion.xml"
#define VERSION_CONFIG_FILE_OPERATE_PATH  "../etc/version.txt"

#define FILE_PATH_CONFIG_PATH  "../etc/filePathConfig.txt"

//#define DLL_PATH  "../loadLib/"



//文件的更新信息
typedef struct ST_UPDATE_INFO_FROM_HTTP_T
{
    char infoFileName[128];
    char infoFileMD5[256];
    char infoNo[32];
    char infoVersion[32];
    int type;
    int fileSize;
    char updateCode[32];
}ST_UPDATE_INFO_FROM_HTTP;


//
typedef struct ST_UPDATE_CONFIG_INFO_T
{
    QString  downloadPath;
    QString loadLibPath;
    QString libPath;
    QString logPath;
}ST_UPDATE_CONFIG_INFO;

extern ST_UPDATE_CONFIG_INFO g_ST_UPDATE_CONFIG_INFO;

#endif // UPDATEMANAGERGLOBAL_H
