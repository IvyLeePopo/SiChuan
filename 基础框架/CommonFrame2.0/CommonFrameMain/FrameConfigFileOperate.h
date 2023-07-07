#ifndef FRAMECONFIGFILEOPERATE_H
#define FRAMECONFIGFILEOPERATE_H
#include <QObject>
#include "json/json.h"
#define FILE_PATH_CONFIG_PATH  "../etc/filePathConfig.txt"
#define  MAX_BUF_LEN 2048
#define  MAX_TXT_LEN 256
typedef struct ST_FRAME_CONFIG_INFO_T
{
    char  downloadPath[128];
    char loadLibPath[128];
    char libPath[128];
    char logPath[128];
    char etcPath[128];
    char netPayPath[128];
}ST_FRAME_CONFIG_INFO;

class FrameConfigFileOperate
{
public:
    FrameConfigFileOperate();

    bool parseFilePathConfig(ST_FRAME_CONFIG_INFO *result );
    bool getJsonData(Json::Value &rootN,char *msgType,char *result);

};

#endif // FRAMECONFIGFILEOPERATE_H
