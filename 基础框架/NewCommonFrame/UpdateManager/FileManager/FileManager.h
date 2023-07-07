#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>

class FileManager
{
public:
  FileManager();
  static bool isFileExist(QString fileName);

  static bool readFlagFileName(QString &codeStr,int &result);

  static QString flagFileName; //更新标志位
};

#endif // FILEMANAGER_H
