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

const char *LIB_INFO =   "VERSION:V2.0.0.0";


UpdateDownLoad *UpdateDownLoad::self=NULL;

UpdateDownLoad *UpdateDownLoad::instance()
{
    if(self==NULL)
        self = new UpdateDownLoad();
    return self;
}

UpdateDownLoad::~UpdateDownLoad()
{
    qDebug()<<"[#~UpdateDownLoad1#]"; //输出文件名
    self = NULL;
    if(pDownloadOverTimer)
    {
        pDownloadOverTimer->stop();
        delete pDownloadOverTimer;
        pDownloadOverTimer = NULL;
    }
    unRegister2Bus(KEY_UPDATE_DOWNLOAD);
}


UpdateDownLoad::UpdateDownLoad()
{
    register2Bus(KEY_UPDATE_DOWNLOAD);
    pDownloadOverTimer = new QTimer();
    connect(pDownloadOverTimer,SIGNAL(timeout()),this,SLOT(downloadOverTimerProc()));
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

        currentUpdateInfo.fileSize = it->fileSize;
        memcpy(currentUpdateInfo.infoFileMD5,it->infoFileMD5,strlen(it->infoFileMD5));
        memcpy(currentUpdateInfo.infoFileName,it->infoFileName,strlen(it->infoFileName));
        memcpy(currentUpdateInfo.infoNo,it->infoNo,strlen(it->infoNo));
        memcpy(currentUpdateInfo.infoVersion,it->infoVersion,strlen(it->infoVersion));
        currentUpdateInfo.type = it->type;
        memcpy(currentUpdateInfo.updateCode,it->updateCode,strlen(it->updateCode));
        sendDownLoadRequest(*it,fileOffset);
    }
    return true;
}

//解析下载的数据
void UpdateDownLoad::sendDownLoadRequest(ST_UPDATE_INFO_FROM_HTTP &updateInfo,int fileOffSet)
{
    pDownloadOverTimer->stop();
    pDownloadOverTimer->start(10*1000);
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
    if(downStr==NULL)
    {

    }
    Json::Value rootN;
    Json::Reader  reader;
    if (!reader.parse(downStr, rootN))
    {
        qDebug()<<"[#UpdateDownLoad::parseDownloadInfo#]jiexi cuowu";
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
    pDownloadOverTimer->stop();
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

            if(fileAllMd5.compare(currentUpdateInfo.infoFileMD5)==0)
            {
                send2Bus(KEY_UPDATE_CONFIRM,currentUpdateInfo.updateCode,strlen(currentUpdateInfo.updateCode),KEY_UPDATE_DOWNLOAD);
                tarFile(filePathName);//解压包，并移动位置，通知基础框架更新
                send2Bus(KEY_COMMON_FRAME,currentUpdateInfo.updateCode,strlen(currentUpdateInfo.updateCode),KEY_UPDATE_DOWNLOAD);

            }
            else
            {
                qDebug()<<"ERROR!!![##UpdateDownLoad::parseDownloadInfo##],文件MD5匹配不成功，升级失败！";
                remove(filePathName.toLocal8Bit().data()); // 失败，先删除zip文件
                //借用KEY_AUTO_UPDATE_MANAGER来区分不同的处理逻辑
                send2Bus(KEY_UPDATE_CONFIRM,currentUpdateInfo.updateCode,strlen(currentUpdateInfo.updateCode),KEY_AUTO_UPDATE_MANAGER);
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
    rootN["updateCode"] = Json::Value(ui.updateCode);
    rootN["requestTime"] = Json::Value(currentTime.toLocal8Bit().data());
    Json::Value tmpV;
    tmpV["type"] = Json::Value(2);
    tmpV["infoNo"] = Json::Value(ui.infoNo);
    tmpV["infoFileName"] = Json::Value(ui.infoFileName);
    tmpV["infoVersion"] = Json::Value(ui.infoVersion);
    tmpV["offset"] = Json::Value(offset);

    rootN["infos"].append(tmpV);

    Json::FastWriter jfw;
    QString result =QString( jfw.write(rootN).c_str());
    return result;
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
    remove(fileName.toLocal8Bit().data());
}

