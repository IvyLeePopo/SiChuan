#include "FrameConfigFileOperate.h"


#include <string>
#include <QFile>
#include <stdio.h>

#define DOWNLOAD_STRING  "download"
#define LOADLIB_STRING  "loadLib"
#define LIB_STRING  "lib"
#define LOG_STRING  "log"
#define NET_PAY_STRING  "netPayPath"
#define ETC_STRING  "etc"
using namespace std;

FrameConfigFileOperate::FrameConfigFileOperate()
{
}

bool FrameConfigFileOperate::parseFilePathConfig(ST_FRAME_CONFIG_INFO *result )
{
    FILE *fp = fopen(FILE_PATH_CONFIG_PATH,"r");
    if( fp==NULL)
    {
        printf("[#FrameConfigFileOperate::parseFilePathConfig#]error:can not open file %s\n",FILE_PATH_CONFIG_PATH);
        return false;
    }
    char buf[MAX_BUF_LEN] ={0};
    char txt[MAX_TXT_LEN]={0};
    fseek(fp,0L,SEEK_SET);
    while (!feof(fp) && fgets(txt, MAX_TXT_LEN, fp))
    {
        strcat(buf,txt);
        memset(txt,0,MAX_TXT_LEN);
    }
    fclose(fp);

    Json::Value rootN;
    Json::Reader  reader;
    if (!reader.parse(buf , rootN))
    {
        return false;
    }
    bool  getResultFlag = false;
    if(true==getJsonData(rootN,DOWNLOAD_STRING,result->downloadPath))
        getResultFlag = true;
    else
        getResultFlag =false;

    if(true==getJsonData(rootN,LOADLIB_STRING,result->loadLibPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,LIB_STRING,result->libPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,LOG_STRING,result->logPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,NET_PAY_STRING,result->netPayPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    if(true==getJsonData(rootN,ETC_STRING,result->etcPath))
        getResultFlag = true;
    else
        getResultFlag =false;
    return getResultFlag;
}

bool FrameConfigFileOperate::getJsonData( Json::Value &rootN,char *msgType, char *result)
{
    Json::Value loadLibCode = rootN[msgType];
    if ( loadLibCode.type() != Json::stringValue )
    {
        printf("ERROR:[#ConfigFileOperate::parseFilePathConfig#]loadLibCode error %s",msgType);
        return false;
    }
    else
    {
        string msg = rootN[msgType].asCString();
        memcpy(result,msg.c_str(),msg.length());
        return true;
    }
}
