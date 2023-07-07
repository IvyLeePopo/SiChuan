#include "cdvjsoncommandwrite.h"
#include <QDebug>



CDVJsonCommandWrite::CDVJsonCommandWrite(QObject *parent) : QThread(parent)
{

    m_length = 0;
    m_sendCompletionFlag = false;

}



void CDVJsonCommandWrite::run()
{
//    while(1)
//    {
//     //   if(stop)break;

//        if( m_sendCompletionFlag)
//        {
//           m_sendResultsFlag = HW_writeUpperpcSerial(m_sendData, m_length);
//           m_length = 0;
//           m_sendCompletionFlag = false;
//        }
//        else
//          {  QThread::msleep(150); }
//    }
}




/****************************************
 *函数名称：WriteCommand
 *函数描述：写命令
 *函数参数：char  int
 *函数返回：bool
*****************************************/
bool CDVJsonCommandWrite::WriteCommand(char *CommandData,int lenth)
{
    if(m_sendCompletionFlag) return false;

    m_sendCompletionFlag = true;

    memset(m_sendData,0,2048);
    memcpy(m_sendData,CommandData,lenth);
    m_length = lenth;

//    //20170418
//     HW_writeUpperpcSerial(m_sendData, m_length);

    return true;
}



/****************************************
 *函数名称：getWriteResult
 *函数描述：获取结果
 *函数参数：void
 *函数返回：bool
*****************************************/
bool CDVJsonCommandWrite::getWriteResult()
{

    //发送结果
    return m_sendResultsFlag;


}




