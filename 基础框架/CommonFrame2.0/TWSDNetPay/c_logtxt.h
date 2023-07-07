#ifndef C_LOGTXT_H
#define C_LOGTXT_H

#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QString>
#include <QObject>
#include <QThread>
#include<QDir>
#include "cthreadbase.h"


#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <ctime>
#include <cstdio>
#else
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#endif

#ifdef WIN32
#define STRDUP(a) _strdup((a))
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define STRDUP(a) strdup((a))
#define ACCESS access
#define MKDIR(a) mkdir((a), 0755)
#endif

class C_logtxt// : public CThreadBase
{
    //   Q_OBJECT
public:
    explicit C_logtxt(void);
    ~C_logtxt();
  //  static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void outputMessage(QtMsgType type, const char* msg);


   void  WriteLogFile(QString fileContext);

protected:

    static int MakeDirectory(const char* pszDir);
    static QString  MakeLogFileName();

    static  QString  GetCurrentPath();

private:
    static QString m_curFileName;
    static QString m_curDir;
    static bool m_bFirstCreate;

    QMutex m_Mutex;

};

#endif // C_LOGTXT_H
