#include "UpdateQuery.h"
#include "json/json.h"
#include <QThread>
#include <QDateTime>
#include <QSettings>
#include "QDebug"
#include <QTime>


UpdateQuery *UpdateQuery::self=NULL;

UpdateQuery *UpdateQuery::instance()
{
  if(self==NULL)
    self = new UpdateQuery();
  return self;
}

UpdateQuery::~UpdateQuery()
{
    unRegister2Bus(KEY_UPDATE_QUERY);
    delete pUpdateTime;
}

UpdateQuery::UpdateQuery()
{
  qDebug()<<"register UpdateQuery";
  register2Bus(KEY_UPDATE_QUERY);
  pUpdateTime = new QTimer();
  connect(pUpdateTime,SIGNAL(timeout()),this,SLOT(updateQuery2Http()));
  qDebug()<<"register UpdateQuery 2";
  pUpdateTime->start(1000*2);
  qDebug()<<"register UpdateQuery 3";

}
//定时发送查询信息
void UpdateQuery::updateQuery2Http()
{
  pUpdateTime->stop(); //测试加上

  QString queryData = generateQueryInfo();
  sendQueryData2Http(queryData);
}
int UpdateQuery::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
  QList<ST_UPDATE_INFO_FROM_HTTP> result;
  if(parseUpdateInfo(msg,result)) //如果有更新包
  {
    qDebug()<<"[#UpdateQuery::recvFromBuse#]有更新包可下载!!";
    send2Bus(KEY_UPDATE_DOWNLOAD,(char *)&result,0,KEY_UPDATE_QUERY);  //开始下载
  }

}
//解析云端返回数据，有更新包的情况，如果返回true,则说明有更新 ,如何传送版本号？？？？
bool UpdateQuery::parseUpdateInfo(QString updateStr, QList<ST_UPDATE_INFO_FROM_HTTP> &result)
{
  Json::Value rootN;
  Json::Reader  reader;
  qDebug()<<"[#UpdateQuery::parseUpdateInfo#]接收HTTP的查询版本信息:"<<updateStr;
  if (!reader.parse(updateStr.toLocal8Bit().data() , rootN))
  {
    qDebug()<<"ERROR:[#UpdateQuery::parseUpdateInfo#]解析失败";
    return false;
  }
  Json::Value errorCode = rootN["errorCode"];
  if ( errorCode.type() != Json::stringValue )
  {
    qDebug()<<"ERROR:[#UpdateQuery::parseUpdateInfo#]：errorCode error";
    return false;
  }
  QString errorCodeStr = errorCode.asCString();
  if(0!=errorCodeStr.compare("0000"))
  {
    qDebug()<<"ERROR:[#UpdateQuery::parseUpdateInfo#]errorCode error";
    return false;
  }
  Json::Value infosVal = rootN["infos"];
  if (infosVal.type() != Json::arrayValue)
  {
    qDebug()<<"ERROR:[#UpdateQuery::parseUpdateInfo#]infosVal error";
    return false;
  }
  qDebug()<<"infosVal.size() ="<<infosVal.size()  ;

  if (infosVal.size() == 0)
  {
    return false;
  }
  result.clear();
  for (int i = 0; i < infosVal.size(); ++i)
  {
    Json::Value oneInfo = infosVal[i];
    ST_UPDATE_INFO_FROM_HTTP ui;
    ui.type = 2;
    if (oneInfo["infoNo"].type() == Json::stringValue )
      ui.infoNo = oneInfo["infoNo"].asCString();
    else
      return false;
    if (oneInfo["infoFileName"].type() == Json::stringValue )
      ui.infoFileName = oneInfo["infoFileName"].asCString();
    else
      return false;
    if (oneInfo["infoVersion"].type() == Json::stringValue )
      ui.infoVersion = oneInfo["infoVersion"].asCString();
    else
      return false;
    if (oneInfo["fileSize"].type() == Json::intValue )
      ui.fileSize = oneInfo["fileSize"].asInt();
    else
      return false;
    if (oneInfo["md5"].type() == Json::stringValue )
      ui.infoFileMD5 = oneInfo["md5"].asCString();
    else
      return false;
    if(rootN["updateCode"].type()== Json::stringValue )
      ui.updateCode = rootN["updateCode"].asCString();
    else
      return false;


    result.push_back(ui);

    qDebug()<<"[#UpdateQuery::parseUpdateInfo#]ui.infoNo ="<<ui.infoNo ;
    qDebug()<<"[#UpdateQuery::parseUpdateInfo#]ui.infoFileName ="<<ui.infoFileName ;
    qDebug()<<"[#UpdateQuery::parseUpdateInfo#]ui.infoVersion ="<<ui.infoVersion ;
    qDebug()<<"[#UpdateQuery::parseUpdateInfo#]ui.fileSize ="<<ui.fileSize ;
    qDebug()<<"[#UpdateQuery::parseUpdateInfo#]ui.infoFileMD5 ="<<ui.infoFileMD5 ;

  }
  return true;
}

//私有函数：构造更新数据，用于云端通信
QString UpdateQuery::generateQueryInfo()
{
  ST_UPDATE_INIT_INFO myUpdateInitInfo;
  memset(&myUpdateInitInfo,0,sizeof(ST_UPDATE_INIT_INFO));
  QSettings *configIniRead = new QSettings(TERMINAL_INI_FILE, QSettings::IniFormat);

  QString mLanId = configIniRead->value("/Initial/LaneID").toString();
  memcpy(myUpdateInitInfo.mLanId ,mLanId.toLocal8Bit().data(),mLanId.toLocal8Bit().length());
  qDebug()<<"[#UpdateQuery::generateQueryInfo#]mLanId="<<mLanId;

  myUpdateInitInfo.mProvinceId= configIniRead->value("/Initial/ProvinceID").toInt();
  qDebug()<<"[#UpdateQuery::generateQueryInfo#]mProvinceId="<<  myUpdateInitInfo.mProvinceId;

  QString mRoadId = configIniRead->value("/Initial/RoadID").toString();
  memcpy(myUpdateInitInfo.mRoadId ,mRoadId.toLocal8Bit().data(),mRoadId.toLocal8Bit().length());
  qDebug()<<"[#UpdateQuery::generateQueryInfo#]mRoadId="<<mRoadId;

  QString stationId = configIniRead->value("/Initial/StationID").toString();
  memcpy(myUpdateInitInfo.mStationId ,stationId.toLocal8Bit().data(),stationId.toLocal8Bit().length());
  qDebug()<<"[#UpdateQuery::generateQueryInfo#]stationId="<<stationId;

  Json::Value rootN;
  rootN["aes"] = Json::Value("");
  rootN["md5"] = Json::Value("123fsdfsd");
  rootN["version"] = Json::Value("1.0");
  rootN["packageType"] = Json::Value(4);
  QString currentTime = datetime2Str (time(NULL)).c_str();
  rootN["time"] = Json::Value(currentTime.toLocal8Bit().data());
  rootN["provinceId"] = Json::Value(myUpdateInitInfo.mProvinceId);
  rootN["roadId"] = Json::Value(myUpdateInitInfo.mRoadId);
  rootN["stationId"] = Json::Value(myUpdateInitInfo.mStationId);
  rootN["laneId"] = Json::Value(myUpdateInitInfo.mLanId);
  rootN["deviceNo"] = Json::Value("11");
  rootN["queryTime"] = Json::Value(currentTime.toLocal8Bit().data());

  QList<QString>::iterator it = NULL;
  for(it=dllNameList.begin(); it != dllNameList.end(); ++it)
  {
    Json::Value tmpV;
    tmpV["type"] = Json::Value(2);
    //从文本当中获取版本号
    QString fileNameTemp = *it;
    QString dllVersion = myReadVersionXml.readXmlFile(fileNameTemp);
    tmpV["infoNo"] = Json::Value(dllVersion.toLocal8Bit().data());

    tmpV["infoFileName"] = Json::Value(fileNameTemp.toLocal8Bit().data());
    tmpV["infoVersion"] = Json::Value("");
    rootN["infos"].append(tmpV);
    if(dllVersion.contains("2",Qt::CaseSensitive)) //返回true;
    {
      rootN["deviceType"] = Json::Value("TR300");
    }
    else
    {
      rootN["deviceType"] = Json::Value("TR200");
    }
  }
  Json::FastWriter jfw;
  QString result = QString(jfw.write(rootN).c_str());
  return result;
}

//私有函数：
void UpdateQuery::sendQueryData2Http(QString queryData)
{
  send2Bus(KEY_UPDATE_HTTP_MANAGER,queryData.toLocal8Bit().data(),
           queryData.toLocal8Bit().length(),KEY_UPDATE_QUERY);
}
