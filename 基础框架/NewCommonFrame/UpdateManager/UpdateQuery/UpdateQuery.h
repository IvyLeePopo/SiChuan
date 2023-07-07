#ifndef UPDATEQUERY_H
#define UPDATEQUERY_H

#include "CommonBus.h"
#include "UpdateManagerGlobal.h"

#include <QTimer>
#include "FileManager/ReadVersionXml.h"
#include <UpdateCommon.h>

class UpdateQuery:public UpdateCommon,public CommonBus
{
    Q_OBJECT
public:
    static UpdateQuery* instance();
    static UpdateQuery*self;
    virtual ~UpdateQuery();
private:
  UpdateQuery();
public:


  int recvFromBus(int destKey,char *msg,int len, int srcKey=0);


  bool parseUpdateInfo(QString updateStr, QList<ST_UPDATE_INFO_FROM_HTTP> &result);


  QString generateQueryInfo();

  void sendQueryData2Http(QString json);

  QTimer *pUpdateTime;


   ReadVersionXml myReadVersionXml;


public slots:

    void updateQuery2Http();

};

#endif // UPDATEQUERY_H
