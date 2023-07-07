#ifndef FRAMECONFIGFILEOPERATE_H
#define FRAMECONFIGFILEOPERATE_H
//#include <QObject>
#include "json/json.h"
#define FILE_PATH_CONFIG_PATH  "filePathConfig.txt"
#define  MAX_BUF_LEN 2048


typedef struct ST_FRAME_CONFIG_INFO_T
{//modify 128 to 300
    char  downloadPath[300];
    char loadLibPath[300];
    char libPath[300];
    char logPath[300];
    char netPayPath[300];
	char netPayPath_V1[300];
	char netPayPath_V2[300];
	char updateCorePath[300];
	char configFilePath[300];
	char itc_path[300];
}ST_FRAME_CONFIG_INFO;




class FrameConfigFileOperate
{
public:
    FrameConfigFileOperate();

	bool parseFilePathConfig(ST_FRAME_CONFIG_INFO *result ,std::string& module_path);
    bool getJsonData(Json::Value &rootN,char *msgType,char *result);

};

#endif // FRAMECONFIGFILEOPERATE_H
