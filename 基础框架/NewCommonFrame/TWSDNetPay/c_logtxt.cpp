#include "c_logtxt.h"


QString                 C_logtxt::m_curFileName              = "";
QString                 C_logtxt::m_curDir                        = "";
bool                      C_logtxt::m_bFirstCreate              = true;
C_logtxt::C_logtxt(void)
{

}

C_logtxt::~C_logtxt()
{
}


//void C_logtxt::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
void C_logtxt::outputMessage(QtMsgType type, const char* msg)

{


    if(m_bFirstCreate)
    {
        m_bFirstCreate = false;
        //QString dirpath = GetCurrentPath();
        //江苏版指定日志路径
        QString dirpath = LOGOUTPATH;
        QString logDir = QString("%1").arg(dirpath);
        m_curDir = logDir;
        MakeDirectory(logDir.toStdString().c_str());
        m_curFileName = MakeLogFileName();
    }

    //  static QMutex mutex;
    //  mutex.lock();

    QString text;

    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    //  QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    //  QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    // QString current_date = QString("(%1)").arg(current_date_time);
    //    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);


    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString message = QString("%1 %2").arg(current_date_time).arg(msg);


    QFile file(MakeLogFileName());
    if(file.open(QIODevice::ReadWrite | QIODevice::Append) )
    {
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();
    }

    //   mutex.unlock();
}



int C_logtxt::MakeDirectory(const char* p_dir)
{
    int i = 0;
    int iRet;
    int iLen;
    char* pszDir;

    if(NULL == p_dir)
    {
        return 0;
    }
    pszDir = STRDUP(p_dir);
    iLen = strlen(pszDir);

    //逐级创建，不管是否创建成功
    for (i = 0;i < iLen; i++)
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/')
        {
            pszDir[i] = '\0';
            iRet = MKDIR(pszDir);
            //支持linux,将所有\换成/
            pszDir[i] = '/';
        }
    }

    iRet = MKDIR(pszDir);
    free(pszDir);
    return iRet;

}

QString C_logtxt::GetCurrentPath()
{
    QDir dir;
    QString dirpath;
    dirpath =  dir.currentPath();
    return dirpath;
}

QString  C_logtxt::MakeLogFileName()
{
    QString current_date_time = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString logFileName = QString("%1/LogTWSDNetPay-%2%3").arg(m_curDir).arg(current_date_time).arg(".log");
    if(m_curFileName.compare(logFileName))
    {
        m_curFileName = logFileName;
    }
    return m_curFileName;
}



void C_logtxt::WriteLogFile(QString fileContext)
{
    QMutexLocker  lock(&m_Mutex);

 //   static  QMutex lMutex;
    //lMutex.lock();
    QDateTime dateTime = QDateTime::currentDateTime();
    QString time = dateTime.toString("yyyyMMdd");
    QString fileName = QObject::tr("/home/xgssoft/log/LogTWSDNetPay-%1.log").arg(time); //
    QFileInfo info(fileName);
    if(!info.exists())  //不存在就创建一个，对应1天1个日志
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadWrite|QIODevice::Append))
            qDebug()<< file.errorString();
        //   QTextStream Stream(&file);
        //    QString str =  dateTime.toString("yyyyMMdd") + ":    业务组件日志" + "\r\n\r\n\r\n";
        //    Stream << str;
        file.close();
    }
    qint64 fileSize = info.size();
    if(fileSize > 10*1024*1024)    //超过指定大小删除当前日志,再创建1个
    {
        QFile::remove(fileName);

        QFile file(fileName);
        if (!file.open(QIODevice::ReadWrite|QIODevice::Append))
            qDebug() << file.errorString();
        QTextStream Stream(&file);
        Stream << "今天最开始的1份超过指定大小，已删除\n";
        QString str =  dateTime.toString("yyyyMMdd") + ":   通信组件日志" + "\r\n\r\n\r\n";
        Stream << str;
        file.close();
    }
    QString totalTime = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString allStr;

    allStr = QObject::tr("[%1]:%2").arg(totalTime).arg(fileContext) + "\r\n";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite|QIODevice::Append))
        qDebug()<< file.errorString();
    QTextStream Stream(&file);
    Stream << allStr;
    file.close();

 //   lMutex.unlock();
}

















