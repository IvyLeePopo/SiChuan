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
#define XML_CONFIG_FILE_PATH "../etc/version.xml"
#define XML_UPDATE_CONFIG_FILE_PATH "../etc/updateVersion.xml"

#define DLL_PATH  "../loadLib/"



//文件的更新信息
typedef struct ST_UPDATE_INFO_FROM_HTTP_T
{
    QString infoFileName;
    QString infoFileMD5;
    QString infoNo;
    QString infoVersion;
    int type;
    int fileSize;
    QString updateCode;
}ST_UPDATE_INFO_FROM_HTTP;


#endif // UPDATEMANAGERGLOBAL_H
