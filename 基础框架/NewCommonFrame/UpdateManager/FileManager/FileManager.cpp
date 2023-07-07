#include "FileManager.h"

#include <QFile>
#include "json/json.h"
#include <QDebug>

QString FileManager::flagFileName;

FileManager::FileManager()
{
  flagFileName = "updateflag.txt";
}

bool FileManager::isFileExist(QString fileName)
{
  QFile mFile(fileName);
  if(mFile.exists())
  {
    return true;
  }
  return false;
}

bool FileManager::readFlagFileName(QString &codeStr,int &result)
{
  QFile mFile(flagFileName);
  if(!mFile.open(QIODevice::ReadOnly))
  {
      qDebug()<<"打开失败";
      return false;
  }
  QByteArray sendData = mFile.readAll();
  mFile.close();
  QFile::remove(flagFileName);//刪除文件

  Json::Value rootN;
  Json::Reader  reader;
  if (!reader.parse(sendData.data(), rootN))
  {
      return 0;
  }
  if ( rootN["code"].type() != Json::stringValue)
  {
      return 0;
  }
  codeStr = rootN["code"].asCString();
  if ( rootN["result"].type() != Json::intValue)
  {
      return 0;
  }
  result = rootN["result"].asInt();

  return true;
}



