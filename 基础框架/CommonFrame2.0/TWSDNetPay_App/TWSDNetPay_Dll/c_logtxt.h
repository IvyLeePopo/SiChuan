#ifndef C_LOGTXT_H
#define C_LOGTXT_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QThread>
#include "cthreadbase.h"


#define  DEBUG      1
#define  WARNING    2
#define  CRITICAL   3
#define  FATAL      4


class C_logtxt : public CThreadBase
{
    Q_OBJECT
public:
    explicit C_logtxt(void);
    ~C_logtxt();
    //void outputMessage(int type, const QString &msg);
    //void DebugMessage(const QString &msg);
    void outputMessage(int type, QString msg);
    void DebugMessage(QString msg);
    bool Open(QString t_FileName);
    bool LogInit(QString t_FilePath);



signals:

public slots:



private :
    QFile *m_LogFile;
    bool IsAvailable;
    QString m_FileName;


};

#endif // C_LOGTXT_H
