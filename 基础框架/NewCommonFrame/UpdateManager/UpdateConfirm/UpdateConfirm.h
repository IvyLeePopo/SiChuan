#ifndef UPDATECONFIRM_H
#define UPDATECONFIRM_H



#include "UpdateManagerGlobal.h"


#include <UpdateCommon.h>
#include <CommonBus.h>

class UpdateConfirm:public UpdateCommon,public CommonBus
{
public:
    static UpdateConfirm* instance();
    static UpdateConfirm*self;
    virtual ~ UpdateConfirm();
private:
  UpdateConfirm();
public:
  int recvFromBus(int destKey,char *msg,int len, int srcKey=0);
  QString generateVerifyInfo(QString updCode, int ver_code);
};

#endif // UPDATECONFIRM_H
