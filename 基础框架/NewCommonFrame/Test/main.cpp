#include <QCoreApplication>

#include "CommonFrameMain.h"

#include <QTextCodec>
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>

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
    QString file_name = QObject::tr("/var/log/LogTWSDNetPay%1.log").arg(file_time);
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
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    qInstallMsgHandler(outputMessage);

    qDebug()<<"程序发布时间:"<<QString( __DATE__ )<< QString( __TIME__);
    qDebug()<<"程序发布时间:"<<QString( __DATE__ )<< QString( __TIME__);


    CommonFrameMain::instance();




    return a.exec();
}
