#include "deviceinfomanagement.h"
#include <QDebug>
#include "upay/upaytmc.h"

DeviceInfoManagement::DeviceInfoManagement()
{

    m_ReadStateTimer = new  QTimer(this);
    connect(m_ReadStateTimer,SIGNAL(timeout()),this,SLOT(ReadStateData()));

}




/****************************************
 *函数名称：DeviceInfoManagementInit
 *函数描述：读状态数据初始化
 *函数参数：void
 *函数返回：bool
*****************************************/
bool DeviceInfoManagement::DeviceInfoManagementInit()
{
   bool breturn = false;


    //心跳間隔10秒
    m_ReadStateTimer->start(1000*10);

    breturn = true;

    return breturn;

}


/****************************************
 *函数名称：ReadStateData
 *函数描述：读状态数据
 *函数参数：void
 *函数返回：void
*****************************************/
void DeviceInfoManagement::ReadStateData()
{
    uint status = 0;

    UpayTMC* tmc = UpayTMC::instance();

    if (!tmc->getServerNetworkStatus())
        status |= (uint)(1 << 0);
    if (!tmc->getPosSignInStatus())
        status |= (uint)(1 << 5);

//    qDebug() << "\n~~~~~~DeviceInfoManagement~~~~~~::ReadStateData: " << status;

    emit sigSendHeartCommand(0, status);
}


/****************************************
 *函数名称：SendHeartbeatCommand
 *函数描述：发送心跳数据
 *函数参数：void
 *函数返回：QByteArray
*****************************************/
QByteArray DeviceInfoManagement::SendHeartbeatCommand()
{
    ushort t_VerifyCode;
    QByteArray  t_InitCommand;
    t_InitCommand.append(COMMAND_START);

    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X04);

    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);


    t_InitCommand.append((char)0XB1);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);

    t_VerifyCode = GetVerifyCode(t_InitCommand,t_InitCommand.length());

    t_InitCommand.append((uchar)(t_VerifyCode>>8));
    t_InitCommand.append((uchar)t_VerifyCode);
    t_InitCommand.append(COMMAND_END);


    return  t_InitCommand;
 }






/****************************************
 *函数名称：FeedBackHeartbeatCommand
 *函数描述：回饋心跳数据
 *函数参数：void
 *函数返回：QByteArray
*****************************************/
QByteArray DeviceInfoManagement::FeedBackHeartbeatCommand(char t_NumericalOrderH,char t_NumericalOrderL)
{
    ushort t_VerifyCode;
    QByteArray  t_InitCommand;
    t_InitCommand.append(COMMAND_START);

    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X04);

    t_InitCommand.append((uchar)t_NumericalOrderH);
    t_InitCommand.append((uchar)t_NumericalOrderL);

    t_InitCommand.append((char)0XB1);
    t_InitCommand.append((char)0X01);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);
    t_InitCommand.append((char)0X00);

    t_VerifyCode = GetVerifyCode(t_InitCommand,t_InitCommand.length());

    t_InitCommand.append((uchar)(t_VerifyCode>>8));
    t_InitCommand.append((uchar)t_VerifyCode);
    t_InitCommand.append(COMMAND_END);


    return  t_InitCommand;





}




