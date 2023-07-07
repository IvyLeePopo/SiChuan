#include "UpdateConfirm.h"
#include "json/json.h"
#include <QDateTime>

#include <QDebug>

#define STATE_UPGRADE_OK        0x02
#define STATE_UPGRADE_FAILED    0x04

UpdateConfirm *UpdateConfirm::self=NULL;

UpdateConfirm *UpdateConfirm::instance()
{
    if(self==NULL)
    {
        qDebug()<<"UpdateConfirm init3";
        self = new UpdateConfirm();
    }
    return self;
}

UpdateConfirm::~UpdateConfirm()
{
    unRegister2Bus(KEY_UPDATE_CONFIRM);
}
UpdateConfirm::UpdateConfirm()
{
    qDebug()<<"[#UpdateConfirm"; //输出文件名
    register2Bus(KEY_UPDATE_CONFIRM);
    readFileName();
}

int UpdateConfirm::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
  if(srcKey==KEY_UPDATE_DOWNLOAD)
  {
    QString confirmData = generateVerifyInfo(msg,STATE_UPGRADE_OK);
    send2Bus(KEY_UPDATE_HTTP_MANAGER,confirmData.toLocal8Bit().data(),confirmData.toLocal8Bit().length(),KEY_UPDATE_CONFIRM);
  }
  else if(srcKey==KEY_AUTO_UPDATE_MANAGER)
  {
    QString confirmData = generateVerifyInfo(msg,STATE_UPGRADE_FAILED);
    send2Bus(KEY_UPDATE_HTTP_MANAGER,confirmData.toLocal8Bit().data(),confirmData.toLocal8Bit().length(),KEY_UPDATE_CONFIRM);
  }
}

QString UpdateConfirm::generateVerifyInfo(QString updCode, int ver_code)
{
  Json::Value rootN;
  rootN["aes"] = Json::Value("");
  rootN["md5"] = Json::Value("123fsdfsd");
  rootN["version"] = Json::Value("1.0");
  rootN["packageType"] = Json::Value(4);
  QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd");

  rootN["time"] = Json::Value(currentTime.toLocal8Bit().data());

  rootN["updateCode"] = Json::Value(updCode.toLocal8Bit().data());
  rootN["type"] = Json::Value(2);
  rootN["updateState"] = Json::Value(ver_code);

  Json::FastWriter jfw;
  QString result = QString(jfw.write(rootN).c_str());
  return result;
}
