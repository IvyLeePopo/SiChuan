#include "UpdateDownLoad.h"

#include <QFile>
#include <QDebug>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include<dlfcn.h>

#include "json/json.h"
#include <QCryptographicHash>
#include <QDateTime>

#include "ZipManager/zip.h"
#include "ZipManager/unzip.h"
#include <QDir>

#define DOWNLOAD_FILE_PATH  "../download/"
#define LOAD_LIB_PATH       "../loadLib/"
#define COMMON_LIB_PATH     "../lib/"
#define BEI_FEN_LOG_PATH    "/opt/log/"


UpdateDownLoad *UpdateDownLoad::self=NULL;

UpdateDownLoad *UpdateDownLoad::instance()
{
  if(self==NULL)
    self = new UpdateDownLoad();
  return self;
}

UpdateDownLoad::~UpdateDownLoad()
{
    unRegister2Bus(KEY_UPDATE_DOWNLOAD);
}


UpdateDownLoad::UpdateDownLoad()
{
    qDebug()<<"register UpdateDownLoad";
    register2Bus(KEY_UPDATE_DOWNLOAD);
  connect(&updateCmdTimer,SIGNAL(timeout()),this,SLOT(updateCmdTimerProc()));
  connect(&downloadOverTimer,SIGNAL(timeout()),this,SLOT(downloadOverTimerProc()));
  filePath = DOWNLOAD_FILE_PATH;
  QDir *temp = new QDir;
  bool exist = temp->exists(filePath);
  if(exist==false)
  {
    temp->mkdir(filePath); //创建文件夹
  }
  exist = temp->exists(LOAD_LIB_PATH);
  if(exist==false)
  {
    temp->mkdir(LOAD_LIB_PATH); //创建文件夹
  }
  exist = temp->exists(COMMON_LIB_PATH);
  if(exist==false)
  {
    temp->mkdir(COMMON_LIB_PATH); //创建文件夹
  }
  exist = temp->exists(BEI_FEN_LOG_PATH);
  if(exist==false)
  {
    temp->mkdir(BEI_FEN_LOG_PATH); //创建文件夹
  }
  delete temp;
}

int UpdateDownLoad::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
  if(srcKey==KEY_UPDATE_QUERY)
  {
    QList<ST_UPDATE_INFO_FROM_HTTP> *reslut = (QList<ST_UPDATE_INFO_FROM_HTTP> *)msg;
    downloadFile(reslut);
  }
  if(srcKey==KEY_UPDATE_HTTP_MANAGER)
  {
    parseDownloadInfo(msg);
  }
}
//进行对应包的下载
bool UpdateDownLoad::downloadFile(QList<ST_UPDATE_INFO_FROM_HTTP> *result)
{
  QList<ST_UPDATE_INFO_FROM_HTTP>::iterator it = NULL; //这里到时http只给出一个包，不会分包
  for(it=result->begin(); it != result->end(); ++it)
  {
    QString  upd_code=it->updateCode;
    fileOffset = 0;
    qDebug()<<"[#UpdateDownLoad::downloadFile#]获取本次升级包总共大小为:"<<it->fileSize;
    filePathName.clear();
    filePathName = filePath;
    filePathName.append(it->infoFileName);
    memset(&currentUpdateInfo,0,sizeof(ST_UPDATE_INFO_FROM_HTTP));
    ST_UPDATE_INFO_FROM_HTTP myST_UPDATE_INFO_FROM_HTTP = *it;
    memcpy((void *)&currentUpdateInfo,&myST_UPDATE_INFO_FROM_HTTP,sizeof(ST_UPDATE_INFO_FROM_HTTP));
    sendDownLoadRequest(*it,fileOffset);
  }
  return true;
}

//解析下载的数据
void UpdateDownLoad::sendDownLoadRequest(ST_UPDATE_INFO_FROM_HTTP &updateInfo,int fileOffSet)
{
  downloadOverTimer.start(10*1000);
  QString postData = generateDownloadInfo(updateInfo,fileOffSet);
  send2Bus(KEY_UPDATE_HTTP_MANAGER,postData.toLocal8Bit().data(),postData.toLocal8Bit().length(),KEY_UPDATE_DOWNLOAD);
}

bool UpdateDownLoad::writeDownloadData2File(QString fileName,QByteArray &rawdata)
{
    qDebug()<<"fileName"<<fileName;
  QFile myFile(fileName);
  if(!myFile.open(QIODevice::ReadWrite|QIODevice::Append))
  {
    qDebug()<<"[#UpdateDownLoad::writeDownloadData2File#]写入文件时，打开失败";
    return false;
  }
  myFile.seek(fileOffset);
  myFile.write(rawdata,rawdata.length());
  myFile.close();
  myFile.flush();
  fileOffset += rawdata.length();
  qDebug()<<"fileOffset="<<fileOffset;
  return true;
}

QString UpdateDownLoad::getFileMd5(QString fileName)
{
  QFile localFile(fileName);
  if (!localFile.open(QFile::ReadOnly))
  {
    qDebug()<<"[#UpdateDownLoad::getFileMd5#]读取文件时，打开失败";
    return QString("");
  }
  QByteArray fileContent = localFile.readAll();
  QByteArray myMd5 = QCryptographicHash::hash(fileContent,QCryptographicHash::Md5);
  QByteArray myMd5Str = myMd5.toHex();
  localFile.close();
  return myMd5Str;
}

bool UpdateDownLoad::parseDownloadInfo(char  *downStr)
{
  qDebug()<<"[#UpdateDownLoad::parseDownloadInfo#]从http下载数据为:"<<downStr;
  Json::Value rootN;
  Json::Reader  reader;
  if (!reader.parse(downStr, rootN))
  {
    return false;
  }
  if ( rootN["errorCode"].type() != Json::stringValue
       ||rootN["errorCode"].asString() != "0000")
  {
    return false;
  }
  Json::Value infosVal = rootN["dataTypes"];
  if (infosVal.type() != Json::arrayValue)
  {
    return false;
  }
  if (infosVal.size() == 0)
  {
    return false;
  }
  downloadOverTimer.stop();
  for (int i = 0; i < infosVal.size(); ++i)
  {
    Json::Value oneInfo = infosVal[i];
    QString fileContent = oneInfo["fileContent"].asCString();
    QString md5 = oneInfo["md5"].asCString();
    QByteArray fileContentByteArray = QByteArray(fileContent.toLocal8Bit());
    QByteArray fileContentMd5 = QCryptographicHash::hash(fileContentByteArray, QCryptographicHash::Md5);
    QString fileContentMd5Hex = QString(fileContentMd5.toHex());
    qDebug()<<"[#UpdateDownLoad::parseDownloadInfo#]:HTTP发送的MD5="<<md5;
    qDebug()<<"[#UpdateDownLoad::parseDownloadInfo#]按照数据生成的MD5="<<fileContentMd5Hex;
    if (md5.compare(fileContentMd5Hex) != 0)
    {
      qDebug()<<"Error!!#UpdateDownLoad::parseDownloadInfo#]MD5对比不匹配，网络传输数据时出错";
      sendDownLoadRequest(currentUpdateInfo,fileOffset);
      return false;
    }
    QByteArray rawdata = QByteArray::fromBase64(fileContent.toLocal8Bit());
    qDebug()<<"[#UpdateDownLoad::parseDownloadInfo#]本次获取的下载数据长度为:"<<rawdata.length();
    writeDownloadData2File(filePathName,rawdata);
  }
  if(fileOffset<currentUpdateInfo.fileSize)
  {
    sendDownLoadRequest(currentUpdateInfo,fileOffset);
  }
  else
  {
    if(fileOffset==currentUpdateInfo.fileSize &&currentUpdateInfo.fileSize!=0)
    {
      QString fileAllMd5  = getFileMd5(filePathName);
      qDebug()<<"fileAllMd5"<<fileAllMd5;
      qDebug()<<"currentUpdateInfo.infoFileMD5"<<currentUpdateInfo.infoFileMD5;

      if(currentUpdateInfo.infoFileMD5.compare(fileAllMd5)==0)
      {
        send2Bus(KEY_UPDATE_CONFIRM,currentUpdateInfo.updateCode.toLocal8Bit().data(),currentUpdateInfo.updateCode.toLocal8Bit().length(),KEY_UPDATE_DOWNLOAD);
        tarFile(filePathName);//解压包，并移动位置，通知基础框架更新
        updateCmdTimer.start(1000);//开启定时器，定时更新
      }
      else
      {
          qDebug()<<"ERROR!!![##UpdateDownLoad::parseDownloadInfo##],文件MD5匹配不成功，升级失败！";
     //   remove(filePathName.toLocal8Bit().data()); // 失败，先删除zip文件
        //借用KEY_AUTO_UPDATE_MANAGER来区分不同的处理逻辑
        send2Bus(KEY_UPDATE_CONFIRM,currentUpdateInfo.updateCode.toLocal8Bit().data(),currentUpdateInfo.updateCode.toLocal8Bit().length(),KEY_AUTO_UPDATE_MANAGER);
      }
    }
    fileOffset = 0;
    filePathName.clear();
  }
  return true;
}
//构造下载具体库信息
QString UpdateDownLoad::generateDownloadInfo(const ST_UPDATE_INFO_FROM_HTTP &ui, int &offset)
{
  Json::Value rootN;
  rootN["aes"] = Json::Value("");
  rootN["md5"] = Json::Value("123123fsdfsd");
  rootN["version"] = Json::Value("1.0");
  rootN["packageType"] = Json::Value(4);
  QString currentTime = QDateTime::currentDateTime().toString("yyyyMMdd");
  rootN["time"] = Json::Value(currentTime.toLocal8Bit().data());
  rootN["updateCode"] = Json::Value(ui.updateCode.toLocal8Bit().data());
  rootN["requestTime"] = Json::Value(currentTime.toLocal8Bit().data());
  Json::Value tmpV;
  tmpV["type"] = Json::Value(2);
  tmpV["infoNo"] = Json::Value(ui.infoNo.toLocal8Bit().data());
  tmpV["infoFileName"] = Json::Value(ui.infoFileName.toLocal8Bit().data());
  tmpV["infoVersion"] = Json::Value(ui.infoVersion.toLocal8Bit().data());
  tmpV["offset"] = Json::Value(offset);

  rootN["infos"].append(tmpV);

  Json::FastWriter jfw;
  QString result =QString( jfw.write(rootN).c_str());
  return result;
}

void UpdateDownLoad::updateCmdTimerProc()
{
  updateCmdTimer.stop(); //先要确认是否有业务在处理，如果没有才能更新
  QList<QString>::iterator it = NULL;
  for(it=currentUpdateDLLName.begin(); it != currentUpdateDLLName.end(); ++it)
  {
    qDebug()<<"currentUpdateDLLName="<<*it;
    QString msg = *it;
    send2Bus(KEY_COMMON_FRAME,msg.toLocal8Bit().data(),msg.toLocal8Bit().length(),KEY_UPDATE_DOWNLOAD);
    //QString dllVersion = myReadVersionXml.readXmlFile(msg);
  }
}

void UpdateDownLoad::downloadOverTimerProc()
{
  sendDownLoadRequest(currentUpdateInfo,fileOffset);
}

void UpdateDownLoad::tarFile(QString fileName)
{    
  HZIP hz = OpenZip(fileName.toLocal8Bit().data(), 0);
  QDir *temp = new QDir;
  bool exist = temp->exists(filePath);
  if(exist==false)
  {
    bool isOk = temp->mkdir(filePath); //创建文件夹
    if(isOk==false)
    {
      qDebug()<<"[#UpdateDownLoad::tarFile#]:无法创建文件夹"<<filePath;
      delete temp;
      return;
    }
  }
  delete temp;
  SetUnzipBaseDir(hz,filePath.toLocal8Bit().data());
  ZIPENTRY ze;
  GetZipItem(hz, -1, &ze);
  int numitems = ze.index;
  for (int zi = 0; zi < numitems; zi++)
  {
    GetZipItem(hz, zi, &ze);
    UnzipItem(hz, zi, ze.name); //解压到当前目录了。已经，每个动态库解压之后的文件名不一样的。
  }
  CloseZip(hz);
 //remove(fileName.toLocal8Bit().data());
//  moveFile();
}


void UpdateDownLoad::moveFile()
{
//  QString szPath=filePath;
//  DIR * dir = NULL;
//  struct dirent * ptr;
//  int i=0;
//  dir = opendir(szPath.toLocal8Bit().data()); //打开一个目录
//  if(dir==NULL)
//  {
//    printf("dir is NULL.\n");
//    return;
//  }
//  while((ptr = readdir(dir)) != NULL) //循环读取目录数据
//  {
//    if(strcmp( ptr->d_name,".")==0)  //文件 "." 固定的格式，但是不需要
//      continue;
//    if(strcmp( ptr->d_name,"..")==0)//文件 ".." 固定的格式，但是不需要
//      continue;
//    QString dllName;
//    char *pTemp = strstr( ptr->d_name,".so.");
//    if(pTemp!=NULL)//文件 ".." 固定的格式，但是不需要
//    {
//        char name[256]={0};
//        memcpy(name,ptr->d_name,strlen(ptr->d_name)-strlen(pTemp));
//        dllName.append(name);
//        dllName.append(".so");
//    }
//    else
//    {
//        dllName = ptr->d_name;
//    }
//    qDebug()<<"dllName"<<dllName;
//    qDebug()<<"dllNameList.size"<<dllNameList.size();

//    if(dllNameList.contains(dllName))//组件库，放入loadLib,//ptr->d_name  .so.1
//    {
//      qDebug()<<"[#UpdateDownLoad::moveFile#]移动动态库"<<ptr->d_name<<"到"<<LOAD_LIB_PATH;
//      if(strstr(ptr->d_name,".so.")==NULL)
//      {
//          currentUpdateDLLName.push_back(ptr->d_name); //ji lu  dang qian de xu yao  shengji  de ku
//      }
//      QString cmd = "mv -f ";
//      cmd.append(szPath);
//      cmd.append(ptr->d_name);
//      cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
//      cmd.append(LOAD_LIB_PATH);
//      system(cmd.toLocal8Bit().data());
//    }
//    else
//    {
//      qDebug()<<"[#UpdateDownLoad::moveFile#]移动"<<ptr->d_name<<"到"<<COMMON_LIB_PATH;
//      QString cmd = "mv -f ";
//      cmd.append(szPath);
//      cmd.append(ptr->d_name);
//      cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
//      cmd.append(COMMON_LIB_PATH); //组件依赖库，放入lib
//      system(cmd.toLocal8Bit().data());
//    }
//  }
//  closedir(dir);//关闭目录指针
}
