#include "TestResponse.h"
#include <QTime>
#include <QFile>
#include <QDebug>
#include <QCryptographicHash>
#include <QDataStream>

#define  TEST_FILE_PATH_NAME  "1.zip"

 long long  TestResponse::offset=0;
TestResponse::TestResponse()
{
}

QString TestResponse::getFileMd5(QString fileName)
{
  QFile localFile(fileName);
  if (!localFile.open(QFile::ReadOnly))
  {
    qDebug()<<"ERROR:[#TestResponse::getFileMd5#]读取文件时，打开失败";
    return QString("");
  }
  QByteArray fileContent = localFile.readAll();
  QByteArray myMd5 = QCryptographicHash::hash(fileContent,QCryptographicHash::Md5);
  QByteArray myMd5Str = myMd5.toHex();
  localFile.close();
  return myMd5Str;
}

//私有函数：构造更新数据，用于云端通信
QString TestResponse::generateQueryResponse()
{
  Json::Value rootN;
  rootN["errorCode"] = Json::Value("0000");
  rootN["errorMsg"] = Json::Value("");
  rootN["packageType"] = Json::Value(4);
  QString currentTime = datetime2Str (time(NULL)).c_str();
  rootN["recvTime"] = Json::Value(currentTime.toLocal8Bit().data());
  rootN["responseTime"] = Json::Value(currentTime.toLocal8Bit().data());
  rootN["updateCode"] = Json::Value("123456");
  rootN["updateFlag"] = Json::Value(1);
  rootN["version"] = Json::Value("");

  Json::Value tmpV;
  tmpV["infoNo"] = Json::Value("V.2.0.0.1");
  tmpV["infoFileName"] = Json::Value(TEST_FILE_PATH_NAME);
  tmpV["infoVersion"] = Json::Value("");
  tmpV["type"] = Json::Value(1);
  QFile myfile(TEST_FILE_PATH_NAME);
  int fileSize = myfile.size();

  tmpV["fileSize"] = Json::Value(fileSize);
  QString fileMd5 = getFileMd5(TEST_FILE_PATH_NAME);
  tmpV["md5"] = Json::Value(fileMd5.toLocal8Bit().data());


  rootN["infos"].append(tmpV);
  Json::FastWriter jfw;
  QString result = QString(jfw.write(rootN).c_str());
  return result;
}

//私有函数：构造更新数据，用于云端通信
QString TestResponse::generateDownLoadResponse()
{
  Json::Value rootN;
  rootN["errorCode"] = Json::Value("0000");
  rootN["errorMsg"] = Json::Value("");
  rootN["packageType"] = Json::Value(4);
  QString currentTime = datetime2Str (time(NULL)).c_str();
  rootN["recvTime"] = Json::Value(currentTime.toLocal8Bit().data());
  rootN["responseTime"] = Json::Value(currentTime.toLocal8Bit().data());
  rootN["updateCode"] = Json::Value("123456");
  rootN["version"] = Json::Value("");

  Json::Value tmpV;
  QFile myFile(TEST_FILE_PATH_NAME);
  if(!myFile.open(QIODevice::ReadOnly))
  {
    qDebug()<<"ERROR:[#TestResponse::generateDownLoadResponse#]读取文件时，打开失败";
    return false;
  }
  myFile.seek(offset);
  QByteArray fileContent = myFile.read(1024);
  myFile.close();
  QByteArray fileContentBase64 = fileContent.toBase64();
  tmpV["fileContent"] = Json::Value(fileContentBase64.data());
  tmpV["fileType"] = Json::Value(0);
  tmpV["isCondense"] = Json::Value(0);


  QByteArray myMd5 = QCryptographicHash::hash(fileContentBase64,QCryptographicHash::Md5);
  QByteArray myMd5Str = myMd5.toHex();
  tmpV["md5"] = Json::Value(myMd5Str.data());
  tmpV["type"] = Json::Value(1);
  tmpV["version"] = Json::Value("");

  rootN["dataTypes"].append(tmpV);
  Json::FastWriter jfw;
  QString result = QString(jfw.write(rootN).c_str());
  offset = offset+fileContent.length();

  return result;
}
