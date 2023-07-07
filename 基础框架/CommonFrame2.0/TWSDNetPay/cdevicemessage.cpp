#include "cdevicemessage.h"

CDevicemessage::CDevicemessage(QObject *parent) : QObject(parent)
{
    m_ReceTimer = new  QTimer();
    connect(m_ReceTimer,SIGNAL(timeout()),this,SLOT(RepeatReadData()));
}



CDevicemessage::~CDevicemessage()
{
    //  mq_unlink(m_UnderMqd);
}


bool CDevicemessage::BuildUnderMS()
{
    bool breturn = false;
    struct mq_attr t_attr;

    t_attr.mq_flags = 0;
    t_attr.mq_maxmsg = 10;
    t_attr.mq_msgsize = QUEUE_MAX_SIZE;
    t_attr.mq_curmsgs = 0;

    m_UnderMqd = mq_open(UNDER_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &t_attr);

    if( m_UnderMqd < 0 )
    {
        qDebug()<<"Net pay readMS build error ";
    }
    else
    {
        UnderIsAvailable = true;
        breturn = true;
        m_UnderAtt = t_attr;
        qDebug()<<"Net pay readMS ";
        m_ReceTimer->start(150); // start read Data
    }
    return breturn;
}




void CDevicemessage::ReadUnderMS()
{
    int t_res;
    if(!UnderIsAvailable) return ;

    struct timespec t_readtime;
    t_readtime.tv_sec = QDateTime::currentDateTime().toTime_t();
    t_readtime.tv_nsec = 100;

    memset(m_ReceData, 0, QUEUE_MAX_SIZE+1);
    t_res =  mq_timedreceive(m_UnderMqd,  m_ReceData, QUEUE_MAX_SIZE, 0, &t_readtime);

    if(t_res>0)
    {
        qDebug()<<"read data ";
     //   emit  ON_MSSAGE_UNDER_TO_DEVICER(SEND_QRCODE_NOTIFY);
    }
}




void CDevicemessage::RepeatReadData()
{
    ReadUnderMS();
    m_ReceTimer->start(200);
}
