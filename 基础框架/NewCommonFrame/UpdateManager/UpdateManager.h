#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#ifdef WIN32
    #if defined(UPDATEMANAGER_LIBRARY)
    #  define UPDATEMANAGERSHARED_EXPORT __declspec(dllexport)
    #else
    #  define UPDATEMANAGERSHARED_EXPORT __declspec(dllimport)
    #endif
#else
    #  define UPDATEMANAGERSHARED_EXPORT
#endif

#include <CommonBus.h>
#include <QTimer>
#include <QObject>

#include "FileManager/ReadVersionXml.h"
#include "UpdateManagerGlobal.h"

#define LIB_NUM 50   //动态库最大加载的个数
#define LIB_NAME_LENTH 128//动态库名称最大长度

class UPDATEMANAGERSHARED_EXPORT UpdateManager:public QObject,public CommonBus
{
Q_OBJECT
public:
    static UpdateManager* instance();
    static UpdateManager*self;
    virtual ~UpdateManager();
    int recvFromBus(int destKey,char *msg,int len, int srcKey=0);

public:
    UpdateManager();

   
    ST_UPDATE_INIT_INFO myUpdateInitInfo;
    int myUpdateInitInfoFlag;

};

extern "C" UPDATEMANAGERSHARED_EXPORT CommonBus* getInstance(); //获取类TestDll的对象
extern "C" UPDATEMANAGERSHARED_EXPORT CommonBus* deleteInstance(); //获取类TestDll的对象


#endif // UPDATEMANAGER_H
