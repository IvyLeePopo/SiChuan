#ifndef READVERSIONXML_H
#define READVERSIONXML_H
#include <QDomElement>

class ReadVersionXml
{
public:
  ReadVersionXml();

  bool  openXmlFile(QString filePath);
  QDomDocument m_doc;
  QString fileDirPath;
  QString readXmlFile(QString findName);

  QString writeXmlFile();
  bool  openUpdateXmlFile(QString filePath);
  QDomDocument m_UpdateDoc;
};

#endif // READVERSIONXML_H
