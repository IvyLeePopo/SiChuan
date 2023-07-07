#include <QCoreApplication>

#include "CommonFrameMain.h"

#include <QTextCodec>
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include "UpdateManager.h"
#include <UpdateHttp/UpdateFromHttpManager.h>

//说明：运行程序测试工程

void outputMessage(QtMsgType type, const char* data)
{
    static QMutex mutex;
    mutex.lock();

    QString msg(data);
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

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1 %2 %3").arg(current_date).arg(text).arg(msg);

    QString file_time = QDate::currentDate().toString("yyyyMMdd");
    QString file_name = QObject::tr("../log/%1.log").arg(file_time);
    QFile file(file_name);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}
void  MessageOutput( QtMsgType type, const QMessageLogContext& Context, const QString &sMsg )
{
    static QMutex mutex;
    mutex.lock();

    QString msg(sMsg);
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

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("[%1]").arg(current_date_time);
    QString message = QString("%1 %2 %3").arg(current_date).arg(text).arg(msg);

    QString file_time = QDate::currentDate().toString("yyyyMMdd");
    QString file_name = QObject::tr("../log/%1.log").arg(file_time);
    QFile file(file_name);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

   //qInstallMsgHandler(outputMessage);
  qInstallMessageHandler(MessageOutput);
    qDebug()<<"程序发布时间:"<<QString( __DATE__ )<< QString( __TIME__);
    qDebug()<<"程序发布时间:"<<QString( __DATE__ )<< QString( __TIME__);


    system("rm -rf ../download/*");
    system("rm -rf ../log/*");

  CommonFrameMain::instance();

    return a.exec();
}
