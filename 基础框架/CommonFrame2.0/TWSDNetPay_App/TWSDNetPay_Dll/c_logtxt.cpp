#include "c_logtxt.h"

#include <QTextCodec>



C_logtxt::C_logtxt(void)
{

   IsAvailable = false;
   //m_LogFile = new QFile();
}



/********************************************
 * 函数名称：Open
 * 函数说明：打开日志文件
 * 参数说明：QString 日志名称
 * 返 回 值：void
 * ******************************************/
bool C_logtxt::LogInit(QString t_FilePath)
{
    IsAvailable = false;
    QString t_FileName = "LogTWSDNetPay-";
    QString t_FilePathName;
    QString Current_date_time = QDateTime::currentDateTime().toString("yyyyMMdd");

    if(QDir(t_FilePath).exists())
    {
        t_FilePathName = t_FilePath +t_FileName+Current_date_time+".log";
    }else
    {
        t_FilePathName = QDir::currentPath()+t_FileName+Current_date_time+".log";
    }


    m_LogFile = new QFile(t_FilePathName);

    IsAvailable =  m_LogFile->open(QIODevice::ReadWrite | QIODevice::Append);


    if(!IsAvailable)
    {
        m_LogFile->close();
        qDebug()<<"m_LogFile open error";
    }else
    {
        qDebug()<<"m_LogFile open ok";

    }


    return IsAvailable;
}

/********************************************
 * 函数名称：Open
 * 函数说明：打开日志文件
 * 参数说明：QString 日志名称
 * 返 回 值：void
 * ******************************************/
bool C_logtxt::Open(QString t_FileName)
{

    bool breturn = false;
    m_LogFile->setFileName(t_FileName);
    breturn =  m_LogFile->open(QIODevice::ReadWrite | QIODevice::Append);

    return breturn;
}




C_logtxt::~C_logtxt()
{
    m_LogFile->close();
}



/********************************************
 * 函数名称：DebugMessage
 * 函数说明：Debug类型记录
 * 参数说明：msg 记录类型
 * 返 回 值：void
 * ******************************************/
//void C_logtxt::DebugMessage(const QString &msg)
void C_logtxt::DebugMessage(QString msg)
{

    if(IsAvailable)
     {
      //  qDebug()<<"into outputMessage ok ";
        outputMessage(DEBUG , msg);
    }else
     {
       qDebug()<<"into outputMessage error ";
    }

}


//void C_logtxt::outputMessage(int type, const QString &msg)
void C_logtxt::outputMessage(int type,  QString msg)
{

    //QMutex mutex;
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case 1:
        text = QString("Debug:");
        break;

    case 2:
        text = QString("Warning:");
        break;

    case 3:
        text = QString("Critical:");
        break;

    case 4:
        text = QString("Fatal:");
    }

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3").arg(current_date).arg(text).arg(msg);

 // m_LogFile->write(pCodec01.toUnicode());
    m_LogFile->write(message.toLocal8Bit());
    m_LogFile->write("\r\n");
    m_LogFile->flush();
//  QTextStream text_stream(m_LogFile);
//  text_stream<< message << "\r\n";


    mutex.unlock();
}



///********************************************
// * 函数名称：CheckAndDeleteLogFile
// * 函数说明：定时检查日志删除
// * 参数说明：void
// * 返 回 值：void
// * ******************************************/
//void C_logtxt::CheckAndDeleteLogFile()
//{

//    qDebug()<<"日志定时检查--删除";

//    m_LogCheckDeleteTimer->start(1000);

//}







