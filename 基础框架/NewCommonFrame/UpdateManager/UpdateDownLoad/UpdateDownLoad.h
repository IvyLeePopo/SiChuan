#ifndef UPDATEDOWNLOAD_H
#define UPDATEDOWNLOAD_H


#include "UpdateManagerGlobal.h"

#include <UpdateCommon.h>
#include <CommonBus.h>
#include <QTimer>
#include <FileManager/ReadVersionXml.h>

class UpdateDownLoad:public UpdateCommon,public CommonBus
{
  Q_OBJECT
public:
    static UpdateDownLoad* instance();
    static UpdateDownLoad*self;
    virtual ~UpdateDownLoad();
private:
  UpdateDownLoad();
public:
  int recvFromBus(int destKey,char *msg,int len, int srcKey=0);
  bool downloadFile(QList<ST_UPDATE_INFO_FROM_HTTP> *result);
  bool parseDownloadInfo(char  *downStr);
  QString  generateDownloadInfo(const ST_UPDATE_INFO_FROM_HTTP&ui, int &offset);
  void tarFile(QString fileName);
  QString filePath;
  QString filePathName;
  void moveFile();
   long long  fileOffset; //文件写入的位置，写入的位置与http接收 的 数据的长度需要转换，base64与正常的流


   void readFileName();
   QList<QString> currentUpdateDLLName;

   ST_UPDATE_INFO_FROM_HTTP currentUpdateInfo;

   void sendDownLoadRequest(ST_UPDATE_INFO_FROM_HTTP &updateInfo,int fileOffSet);

   bool writeDownloadData2File(QString fileName,QByteArray &rawdata);

   QString getFileMd5(QString filePathName);


public slots:

   void updateCmdTimerProc();
   void downloadOverTimerProc();


private:
    QTimer updateCmdTimer;
    QTimer downloadOverTimer;

    ReadVersionXml myReadVersionXml;

};

#endif // UPDATEDOWNLOAD_H
