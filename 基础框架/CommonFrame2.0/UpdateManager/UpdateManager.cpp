#include "FileManager/FileManager.h"
#include "UpdateManager.h"

#include <QDateTime>
#include <QFile>
#include <QCryptographicHash>

#include <json/json.h>
#include "UpdateConfirm/UpdateConfirm.h"
#include "UpdateDownLoad/UpdateDownLoad.h"
#include "UpdateQuery/UpdateQuery.h"
#include <QDebug>

#include <UpdateHttp/UpdateFromHttpManager.h>

ST_UPDATE_CONFIG_INFO g_ST_UPDATE_CONFIG_INFO;

UpdateManager *UpdateManager::self=NULL;

CommonBus* getInstance()
{
    return UpdateManager::instance();
}

CommonBus* deleteInstance()
{
    if(UpdateManager::self!=NULL)
        delete UpdateManager::self;
    return NULL;
}

UpdateManager *UpdateManager::instance()
{
    if(self==NULL)
    {
        self = new UpdateManager();
    }
    return self;
}

UpdateManager::UpdateManager()
{
    printf("!!!!!!!!!!UpdateManager.\n");;
    printf("!!!!!!!!!!UpdateManager.\n");;
    printf("!!!!!!!!!!UpdateManager.\n");;
    printf("!!!!!!!!!!UpdateManager.\n");;


    commpentsKey = KEY_AUTO_UPDATE_MANAGER;
       register2Bus(KEY_AUTO_UPDATE_MANAGER);
       printf("123123.\n");;

       myConfigFileOperate.parseFilePathConfig();
       printf("!!!!!!!!!!11.\n");;

       UpdateConfirm::instance();
       printf("!!!!!!!!!!22.\n");;

       UpdateDownLoad::instance();
       printf("!!!!!!!!!!33.\n");;

       UpdateQuery::instance();
       printf("!!!!!!!!!!44.\n");;

       UpdateFromHttpManager::instance();
}

UpdateManager::~UpdateManager()
{
    unRegister2Bus(KEY_AUTO_UPDATE_MANAGER);
    qDebug()<<"unRegister UpdateManager";
  delete UpdateConfirm::self;
    printf("2.\n");
   delete UpdateDownLoad::self;
    printf("3.\n");
  delete UpdateQuery::self;
   printf("4.\n");

   delete UpdateFromHttpManager::self;
    printf("5.\n");
   self = NULL;
}


int UpdateManager::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
    if(srcKey==KEY_COMMON_FRAME)
    {
        return 1;
    }
}


