#include "ReadVersionXml.h"

#include <QFile>
#include <QDebug>
#include "UpdateManagerGlobal.h"

ReadVersionXml::ReadVersionXml()
{
//  readXmlFile(XML_CONFIG_FILE_PATH);
  fileDirPath = XML_CONFIG_FILE_PATH;
}
bool  ReadVersionXml::openXmlFile(QString filePath)
{
     QFile file(filePath);
     if( !file.open(QFile::ReadWrite | QFile::Text))
     {
        qDebug()<<QObject::tr("error::ParserXML->OpenXmlFile->file.open->%s\n") << filePath;
        return false;
     }
     QString errorStr;
     int errorLine;
     int errorColumn;
     if (!m_doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        qDebug()<<QObject::tr("error::ParserXML->OpenXmlFile->doc.setContent\n") << filePath;
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool ReadVersionXml::openUpdateXmlFile(QString filePath)
{
  QFile file(filePath);
  if( !file.open(QFile::ReadOnly | QFile::Text))
  {
     qDebug()<<QObject::tr("error::ParserXML->OpenXmlFile->file.open->%s\n") << filePath;
     return false;
  }
  QString errorStr;
  int errorLine;
  int errorColumn;
  if (!m_UpdateDoc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
 {
     qDebug()<<QObject::tr("error::ParserXML->OpenXmlFile->doc.setContent\n") << filePath;
     file.close();
     return false;
 }
 file.close();
 return true;
}


QString ReadVersionXml::readXmlFile(QString findName)
{
  if(!openXmlFile(fileDirPath))
  {
        return false;
  }

  QDomElement root = m_doc.documentElement();

  QDomElement findNameNode = root.firstChildElement(findName);
  QString myVersion;
  myVersion.clear();
  if(!findNameNode.isNull())
  {
    myVersion = findNameNode.text();
    qDebug()<<"myVersion="<<myVersion;
  }
  return myVersion;
}

QString ReadVersionXml::writeXmlFile()
{
//  if(!openXmlFile(fileDirPath))
//  {
//        return false;
//  }
//  if(!openUpdateXmlFile(XML_UPDATE_CONFIG_FILE_PATH))
//  {
//        return false;
//  }
//  QDomElement rootUpdate = m_UpdateDoc.documentElement();
//  QDomElement updateNameNode = root.firstChildElement();
//  QString myUpdateVersion;
//  if(!updateNameNode.isNull())
//  {
//    myUpdateVersion = updateNameNode.text();
//    QDomElement root = m_doc.documentElement();
//    QDomElement findNameNode = root.firstChildElement(myUpdateVersion.nodeName());
//    QString myVersion;
//    myVersion.clear();
//    if(!findNameNode.isNull())
//    {
//      myVersion = findNameNode.text();
//      qDebug()<<"myVersion="<<myVersion;
//      findNameNode.re
//    }
//  }


//  return myVersion;
}

