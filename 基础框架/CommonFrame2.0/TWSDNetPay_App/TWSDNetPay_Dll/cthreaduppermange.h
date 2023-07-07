/********************************************
 *
 * 类   名：CThreadUpperMange
 * 摘   要：与上层消息通信，声明文件
 * 作   者：CWX
 * 创建日期：2016-12-01
 * 修改日期：
 *
*********************************************/
#ifndef CTHREADUPPERMANGE_H
#define CTHREADUPPERMANGE_H

#include <QObject>
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
#include <QDateTime>
#include <QtDebug>


class CThreadUpperMange : public QObject
{
    Q_OBJECT
public:
    explicit CThreadUpperMange(QObject *parent = 0);
    ~CThreadUpperMange();

    bool OpenUpperMS(char* t_mqnane);
    bool SendUpperMS(char *t_buf,unsigned int t_size);
    bool CloseUpperMS(void);

signals:

public slots:


private:

    bool  UpperIsAvailable;
    mqd_t m_UpperMqd;           //消息队列句柄
    struct mq_attr m_UpperAtt;  //消息属性
    char m_UpperMqdName[30];
    int m_UpperMqMSMax;
};

#endif // CTHREADUPPERMANGE_H
