#ifndef UPLOAD_H
#define UPLOAD_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QNetworkAccessManager>
#include <QtNetwork/QUrlInfo>
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTime>
#include <QMutex>

#include "jsoncpp/json.h"


class UpLoad : public QObject
{
    Q_OBJECT
public:
    explicit UpLoad(QObject *parent = 0);
    ~UpLoad();

       //设置文件片传输长度，每次长度由服务端下发
    void setPostLength(long len) { m_postLength = len; }
    //设置文件上传地址
    void setServerAddr(QString addr){  m_strServerAddr = addr;}
    //设置文件标识
    void setSessionID(const QString strSeesionID) { m_SeesionId = strSeesionID; }
    //设置要上传的文件路径
    void setPostFilePath(QString filePath);
    //开始上传文件片数据
    bool startUpLoad();

signals:
    //从服务器返回的数据，retCode=0 表示成功，那么msg就是存储位置；retCode非0 msg就是错误信息
    void sigReplyMessage(int retCode,QString msg);

private slots:
    void replyFinished( QNetworkReply* reply);

    void upLoadError(QNetworkReply::NetworkError errorCode);
    void OnUploadProgress( qint64 bytesSent, qint64 bytesTotal );
    void slotUploadFinished();


private:
    bool UpLoadFile();
  QString getSessionID();
  ushort GetVerifyCode(QByteArray t_lpData, uchar t_wLength);

private:
    QNetworkAccessManager    *m_uploadManager;
    QNetworkReply            *m_reply;

    QString m_strServerAddr ;       //文件上传目标地址
    QString m_strFilePath  ;        //待上传文件


    QByteArray                m_FileBuffer;  //文件数据缓冲区
    int m_StartOffset;               //上传起始位置   下标从0开始
    int m_postLength;          //待上传文件片大小，每次从服务端读取
    int m_CompleteLenth;              //已完成的长度
    int m_AllLenth;                 //总长度

    long m_MaxTimeout;          //超时设置
    long m_StartTime;                  //起始时间
    bool m_isRun;                   //是否在上传
    QString m_SeesionId;               //同一文件标识

private:
    QMutex m_mutex;
    bool m_isUpLoading;

};

#endif // UPLOAD_H







