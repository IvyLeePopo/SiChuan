#ifndef CDEVICEMESSAGE_H
#define CDEVICEMESSAGE_H

#include <QObject>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/time.h>
#include "basedefine.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>


class CDevicemessage : public QObject
{
    Q_OBJECT
public:
    explicit CDevicemessage(QObject *parent = 0);
    ~CDevicemessage();

    bool BuildUnderMS();
    void ReadUnderMS();

signals:
     void ON_MSSAGE_UNDER_TO_DEVICER(int);

public slots:
     void RepeatReadData();

private:
    bool   UnderIsAvailable;
    mqd_t  m_UnderMqd;           //消息队列句柄
    struct mq_attr m_UnderAtt;  //消息属性
    char   m_UnderMqdName[30];
    int    m_UnderMqMSMax;
    char   m_ReceData[QUEUE_MAX_SIZE+1];
    QTimer *m_ReceTimer;

};

#endif // CDEVICEMESSAGE_H
