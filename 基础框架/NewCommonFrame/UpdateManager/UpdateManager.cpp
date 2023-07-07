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

UpdateManager *UpdateManager::self=NULL;

CommonBus* getInstance()
{
    return UpdateManager::instance();
}

CommonBus* deleteInstance()
{
    delete UpdateManager::instance();
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
        qDebug()<<"UpdateManager init";
        register2Bus(KEY_AUTO_UPDATE_MANAGER);
        qDebug()<<"UpdateManager init2";

       UpdateConfirm::instance();
        qDebug()<<"UpdateManager init3";

        UpdateDownLoad::instance();
        qDebug()<<"UpdateManager init4";

        UpdateQuery::instance();
        qDebug()<<"UpdateManager init5";

        UpdateFromHttpManager::instance();
        qDebug()<<"UpdateManager init6";

}

UpdateManager::~UpdateManager()
{
    unRegister2Bus(KEY_AUTO_UPDATE_MANAGER);
    qDebug()<<"unRegister UpdateManager";
    delete UpdateConfirm::instance();
    delete UpdateDownLoad::instance();
    delete UpdateQuery::instance();
    delete UpdateFromHttpManager::instance();
    self = NULL;
}


int UpdateManager::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
    if(srcKey==KEY_COMMON_FRAME)
    {
        return 1;
    }
}


