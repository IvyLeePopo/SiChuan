#include "cthreaduppermange.h"

CThreadUpperMange::CThreadUpperMange(QObject *parent) : QObject(parent)
{

    UpperIsAvailable = false;

}


CThreadUpperMange::~CThreadUpperMange()
{
   mq_unlink(m_UpperMqdName);
}


/*********************************************
 * 函数名称: InitMS
 * 函数描述: 建立消息队列
 * 函数参数: char*
 * 函数返回: bool
 * *******************************************/
bool CThreadUpperMange::OpenUpperMS(char* t_mqnane)
{

   //m_UpperMqdName = t_mqnane;

   memcpy(m_UpperMqdName,t_mqnane,sizeof(m_UpperMqdName));

   m_UpperMqd = mq_open(m_UpperMqdName, O_RDWR);

  if(m_UpperMqd == -1)
    {
      qDebug()<<"DLL Upper mq open error";

      UpperIsAvailable = false;

  }else
   {
        qDebug()<<"DLL Upper mq open OK";
        UpperIsAvailable = true;

        mq_getattr(m_UpperMqd,&m_UpperAtt);
        m_UpperMqMSMax = m_UpperAtt.mq_msgsize;
  }
      return UpperIsAvailable;
}




/*********************************************
 * 函数名称: SendUpperMS
 * 函数描述: 发送数据
 * 函数参数: char* int
 * 函数返回: bool
 * *******************************************/
bool CThreadUpperMange::SendUpperMS(char *t_buf,unsigned int t_size)
{

    int t_res;

    if(!UpperIsAvailable)  return false ;

    if(t_size >m_UpperAtt.mq_msgsize) return false;

    struct timespec t_readtime;
    t_readtime.tv_sec = QDateTime::currentDateTime().toTime_t();
    t_readtime.tv_nsec = 100;

   // t_res =  mq_send(m_mqd, t_buf, t_size, 0);
    t_res =  mq_timedsend(m_UpperMqd, t_buf, t_size, 0,&t_readtime);

    if(t_res == -1)
      {qDebug()<<"Upper mq sending false";  return false;   }
    else
      {qDebug()<<"Upper mq sending ok";     return true;}

}




/*********************************************
 * 函数名称: CloseUpperMS
 * 函数描述: 关闭消息队列
 * 函数参数: void
 * 函数返回: bool
 * *******************************************/
bool CThreadUpperMange::CloseUpperMS(void)
{


    if(UpperIsAvailable)
      {
        mq_close(m_UpperMqd);
    }


}
