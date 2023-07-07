#include "UpLoad.h"

UpLoad::UpLoad(QObject *parent) :
    QObject(parent),
    m_uploadManager(NULL),
    m_reply(NULL),
    m_strServerAddr(""),     //文件上传目标地址
    m_strFilePath(""),        //待上传文件
    m_StartOffset(-1),               //上传起始位置   下标从0开始
    m_postLength(0),          //待上传文件片大小，每次从服务端读取
    m_CompleteLenth(0),              //已完成的长度
    m_AllLenth(0),                 //总长度

    m_MaxTimeout(0),          //超时设置
    m_StartTime(0),                  //起始时间
    m_isRun(false),                 //是否在上传
    m_SeesionId("")   ,           //同一文件标识
    m_isUpLoading(false)

{
    m_uploadManager = new QNetworkAccessManager(this);
    connect(m_uploadManager,SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));


}

UpLoad::~UpLoad()
{
    if(m_uploadManager)
    {
        m_uploadManager->deleteLater();
        m_uploadManager = NULL;
    }

}


void UpLoad::setPostFilePath(QString filePath)
{
    if(m_strFilePath != filePath)
    {
        m_strFilePath = filePath;
        QFile file(m_strFilePath);
        if(!file.open(QIODevice::ReadOnly) || file.size() ==0)
        {
            qDebug()<<"open file failed...";
            file.close();
            return;
        }


        m_AllLenth = file.size();
        m_FileBuffer.clear();
        m_FileBuffer = file.readAll();
        m_StartOffset = 0;
        file.close();

        qDebug()<<"open file success...m_AllLenth=" << m_AllLenth ;
    }

}


bool UpLoad::startUpLoad()
{
    return UpLoadFile();
}


bool UpLoad::UpLoadFile()
{
    {
        QMutexLocker locker(&m_mutex);
        if(m_isUpLoading)
        {
            return false;
        }
    }

    qDebug()<<"start upload file...";

    if(m_StartOffset >= m_AllLenth-1)
    {
        qDebug()<<"upload all finish...";
        //上传完成
        return false;
    }

    if(m_StartOffset+m_postLength >= m_AllLenth)
    {
        m_postLength = m_AllLenth - m_StartOffset;
    }

    QNetworkRequest request;
    //准备头信息
    request.setUrl(QUrl(m_strServerAddr));

    //准备数据
    QByteArray postData = m_FileBuffer.mid( m_StartOffset, m_postLength );
    qDebug()<<"m_StartOffset="<<m_StartOffset<<" , m_postLength="<<m_postLength;

    //这句很重要，不然传二进制文件，服务器会说安全问题
    //   request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    //    request.setRawHeader("filename", filename.toUtf8() );
    //      m_pNetWorkManager->post(request, params.toString().toUtf8());

    request.setRawHeader("filename", m_strFilePath.toLocal8Bit() );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=utf-8");
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_postLength);

    m_isUpLoading = true;
    // 这里是把数据转成base64的，好处是，防止文件传的过程因为符号，空格的原因，导致文件数据失真。
    m_reply = m_uploadManager->post( request , postData.toBase64() );


    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotUploadFinished()));

    return true;

}

void UpLoad::slotUploadFinished()
{

    {
        QMutexLocker locker(&m_mutex);
        m_isUpLoading = false;
    }
    qDebug()<<"upload picfile finish...";
    m_reply->deleteLater();
    m_StartOffset += m_postLength;


}

void UpLoad::upLoadError(QNetworkReply::NetworkError errorCode)
{
    if(errorCode == QNetworkReply::NoError)
    {
        qDebug()<<"upload success...";
        //post success
    }
    else
    {
        qDebug()<<"upload error...";
        //post error
    }
}



void UpLoad::replyFinished(QNetworkReply *reply)
{

    try
    {
        QByteArray ba = reply->readAll();
        Json::Reader reader;
        Json::Value root;

        if(!reader.parse(ba.data(), root))
        {
            throw -1;
        }
        if(Json::nullValue == root.type())
        {
            throw -2;
        }

        if(Json::nullValue != root["error_code"].type())
        {
            if(root["error_code"].asInt() == 0){//成功
                if(Json::nullValue != root["url"].type()){
                    qDebug() << "root[url] = " << root["url"].asCString();
                    emit sigReplyMessage(0,root["url"].asCString());
                }
            }
        }
        else
        {
            if(Json::nullValue != root["message"].type())
            {
                qDebug() << "root[message] = " << root["message"].asCString();
                emit sigReplyMessage(1,root["message"].asCString());
            }
        }


    }
    catch(int & nError)
    {
        switch(nError)
        {
        case -1:
            qDebug() << "error: json parse failed"  ;
            break;

        case -2:
            qDebug() << "error: shoud json object "  ;
            break;

        default:
            break;
        }
    }

    if(reply->isFinished())
    {
        reply->deleteLater();
    }

}


QString UpLoad::getSessionID()
{
    QString strID;
    strID.clear();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0; i<10; i++)
    {
        int test =qrand()%10;
        qDebug()<<test;
        strID.append(QString::number(test));
    }

    return strID;
}


ushort UpLoad::GetVerifyCode(QByteArray t_lpData, uchar t_wLength)
{
    uchar   CRC16Hi = 0xFF;
    uchar   CRC16Lo = 0xFF;
    uchar   CH = 0xA0;
    uchar   CL = 0x01;
    uchar   SaveHi = 0;
    uchar   SaveLo = 0;
    uchar   Flag;
    ushort  CRC_REC=0;

    for(int i = 1;i < t_wLength;i++)
    {
        CRC16Lo = CRC16Lo ^ t_lpData[i];
        for(Flag = 0; Flag < 8; Flag++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi >> 1;
            CRC16Lo = CRC16Lo >> 1;
            if((SaveHi & 1) == 1)
                CRC16Lo |= 0x80;
            if((SaveLo & 1) == 1)
            {
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }
    }
    CRC_REC = (CRC16Hi<<8);
    CRC_REC |= CRC16Lo;

    return CRC_REC;
}

void UpLoad::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    QString str = QString("upload total: %1, now upload:%2").arg(bytesTotal).arg(bytesSent);
    qDebug()<<str;
}








