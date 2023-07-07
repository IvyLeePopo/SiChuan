#include "commandmanagement.h"
#include "HWPerformance/hwperformance.h"
#include "httwsddevice.h"
#include "paydbinf.h"

QList<UpperCommandData *> G_msgCommandDataList;

QMutex        G_UpperReadMutex;


//APP版本号
QString G_AppVersion;

/****************************************
*回调函数--获取上位机传来的数据，加入队列
*參數： int iType:  命令類型
*      int iLenth: 命令長度
*      char *msg : 命令數據
*****************************************/
static void HTProcessUpperData(int iType,int iLenth, char *msg)
{
    UpperCommandData *t_UpperQueueData = new UpperCommandData;

    t_UpperQueueData->m_CommandType = iType;
    t_UpperQueueData->m_len = iLenth;

    memcpy(t_UpperQueueData->m_UpperData,msg,t_UpperQueueData->m_len);

    G_UpperReadMutex.lock();
    G_msgCommandDataList.append(t_UpperQueueData);
    G_UpperReadMutex.unlock();
}


CommandManagement::CommandManagement(QObject *parent) : QObject(parent)
{
    m_ReadResultTimer = NULL;

    //初始化标志
    m_IsInitFlag = false;

    m_ByteInterfaceData = new char[MaxCommandDataLenth];
    m_ByteSendData = new char[MaxCommandDataLenth];

    //TWS Config
    m_twsConfig = TWSConfig::instance();
}

CommandManagement::~CommandManagement()
{
    m_ReadResultTimer->stop();
    delete m_ReadResultTimer;
    delete m_ByteInterfaceData;
    delete m_ByteSendData;

    IF_Destroy();
}



/****************************************
 *函数名称：PLCTransmissionHandle
 *函数描述：下发收费流水关键信息
 *函数参数：char *
 *函数返回：bool
*****************************************/
bool CommandManagement::PLCTransmissionHandle(int lent,const char* byteResponseCommand)
{
    char  *t_szData = NULL;
    t_szData = new char[4];

    m_SendD2Timer->stop();
    qDebug()<<"收到A500";

    CDVJsonCommandDebitInit   *t_pCDVJsonCommandDebitInit ;
    t_pCDVJsonCommandDebitInit = NULL;
    t_pCDVJsonCommandDebitInit = new CDVJsonCommandDebitInit;

    try{
        //提取命令
        memset(m_CommandData,0,1024);
        memcpy(m_CommandData,byteResponseCommand,lent);

        QString t_strJsonBuffer(m_CommandData);

        t_strJsonBuffer.replace(QString("\n"), QString(""));
        t_strJsonBuffer.replace(QString("\r"), QString(""));

        t_pCDVJsonCommandDebitInit->fnAnalysisStringForData(t_strJsonBuffer);
        m_IsInitFlag = t_pCDVJsonCommandDebitInit->CommandDebitModelInit();

        qDebug() << "***********[串口服務响应],接收到初始化信息，初始化接口***********";

        // trigger first signIn after booting
        HTTWSDDevice::instance()->slStartFirstSignIn();

        //成功
        memset(t_szData,0,4);

        IF_ResponseNormalDataToUpperMonitor(4,4,t_szData);
    }catch(...)
    {
        qDebug()<<"[串口服務响应],接收到初始化信息，執行異常";
    }

    if( NULL != t_pCDVJsonCommandDebitInit)
    {
        delete t_pCDVJsonCommandDebitInit;
        t_pCDVJsonCommandDebitInit = NULL;
    }

    if(NULL != t_szData)
    {
        delete  t_szData;
        t_szData = NULL;
    }
}




/****************************************
 *函数名称：DebitFlowChartHandle
 *函数描述：扣款命令執行
 *函数参数：char *
 *函数返回：void
*****************************************/
bool CommandManagement::DebitFlowChartHandle(int lent,const char* byteResponseCommand)
{
    bool bretrun = false;

    if(!m_IsInitFlag) {
        qWarning() << "CommandManagement::DebitFlowChartHandle m_IsInitFlag: " << m_IsInitFlag;
        return bretrun;
    }

    CDVJsonCommandDebitMoney   *t_pCDVJsonCommandDebitMoney;
    t_pCDVJsonCommandDebitMoney = NULL;
    t_pCDVJsonCommandDebitMoney = new CDVJsonCommandDebitMoney;

    try{
        //接口数据
        QString t_strInterface;

        //提取命令
        memset(m_CommandData,0,1024);
        memcpy(m_CommandData,byteResponseCommand,lent);

        QString t_strJsonBuffer(m_CommandData);

        t_strJsonBuffer.replace(QString("\n"), QString(""));
        t_strJsonBuffer.replace(QString("\r"), QString(""));

        t_pCDVJsonCommandDebitMoney->fnAnalysisStringForData(t_strJsonBuffer);
        t_strInterface = t_pCDVJsonCommandDebitMoney->FormatParamData2Json();

        qDebug() << "***********[串口服務响应-交易命令處理],接收到交易信息，交易接口***********";

        if (t_strInterface.length() < MaxCommandDataLenth)
        {
            memset(m_ByteInterfaceData,0,MaxCommandDataLenth);
            memcpy(m_ByteInterfaceData, t_strInterface.toLocal8Bit().data(), t_strInterface.toLocal8Bit().length());
            m_ByteInterfaceDataLenth = t_strInterface.toLocal8Bit().length();

            IF_DebitMoney(m_ByteInterfaceData, m_ByteInterfaceDataLenth, 2);
        }

        bretrun = true;

    }catch(...)
    {
        qDebug()<<"[串口服務响应],接收到交易信息，執行異常";

        bretrun = false;
    }


    if( NULL != t_pCDVJsonCommandDebitMoney)
    {
        delete t_pCDVJsonCommandDebitMoney;
        t_pCDVJsonCommandDebitMoney = NULL;
    }

    return bretrun;
}




/****************************************
 *函数名称：CencelFlowChartHandle
 *函数描述：撤單命令
 *函数参数：void
 *函数返回：void
*****************************************/
bool CommandManagement::CencelFlowChartHandle(int lent, const char* byteResponseCommand)
{
    bool bretrun = false;

    if(!m_IsInitFlag) return bretrun;

    CDVJsonCommandDebitCencel   *t_pCDVJsonCommandDebitCencel;
    t_pCDVJsonCommandDebitCencel = NULL;
    t_pCDVJsonCommandDebitCencel = new CDVJsonCommandDebitCencel;

    try{

        //接口数据
        QString t_strInterface;

        //提取命令
        memset(m_CommandData,0,1024);
        memcpy(m_CommandData,byteResponseCommand,lent);

        QString t_strJsonBuffer(m_CommandData);

        t_strJsonBuffer.replace(QString("\n"), QString(""));
        t_strJsonBuffer.replace(QString("\r"), QString(""));

        t_pCDVJsonCommandDebitCencel->fnAnalysisStringForData(t_strJsonBuffer);
        t_strInterface = t_pCDVJsonCommandDebitCencel->FormatParamData2Json();

        qDebug() << "***********[串口服務响应--撤單命令處理],接收到撤單信息，撤單接口***********";

        if(t_strInterface.length()<MaxCommandDataLenth)
        {
            memset(m_ByteInterfaceData,0,MaxCommandDataLenth);
            memcpy(m_ByteInterfaceData,t_strInterface.toLocal8Bit().data(),t_strInterface.toLocal8Bit().length());
            m_ByteInterfaceDataLenth = t_strInterface.toLocal8Bit().length();
            IF_DebitCancel(m_ByteInterfaceData,m_ByteInterfaceDataLenth, 2);

        }

        bretrun = true;

    }catch(...)
    {
        qDebug()<<"[串口服務响应],接收到撤單信息，執行異常";
        theApp.RecordLog("[串口服務响应],接收到撤單信息，執行異常");

        bretrun = false;
    }

    if( NULL != t_pCDVJsonCommandDebitCencel)
    {
        delete t_pCDVJsonCommandDebitCencel;
        t_pCDVJsonCommandDebitCencel = NULL;
    }


    return bretrun;


}



/****************************************
 *函数名称：HearTbeatInouiryComHandle
 *函数描述：心跳查询
 *函数参数：void
 *函数返回：void
*****************************************/
bool CommandManagement::HearTbeatInouiryComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;


}



/****************************************
 *函数名称：HardwareVersionComHandle
 *函数描述：读硬件版本
 *函数参数：void
 *函数返回：void
*****************************************/
bool CommandManagement::HardwareVersionComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;
}

QString CommandManagement::ReadAppVersion()
{
    QString tReturn;
    tReturn.clear();
    int fd;
    fd = open(APPVERSION,O_RDWR);
    if(fd < 0)
    {
        QString str  = QObject::tr("open %1 error!\n").arg(APPVERSION);
        qDebug()<<str;
        return tReturn;
    }
    char buf[256];
    memset(buf,0,256);
    int i;
    i = read(fd,buf,256);
    if(i < 0)
    {
        QString str = QObject::tr("read %1 error!\n").arg(APPVERSION);
        qDebug()<<str;
        return tReturn;
    }
    close(fd);
    tReturn = QString(QLatin1String(buf));
    qDebug()<<"APP版本号: "<<tReturn;
    return tReturn;
}
/****************************************
 *函数名称：ReadCanidComHandle
 *函数描述：读CANID
 *函数参数：void
 *函数返回：void
*****************************************/
bool CommandManagement::ReadCanidComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}



/****************************************
 *函数名称：ReadMaxComHandle
 *函数描述：读MAX码
 *函数参数：void
 *函数返回：void
*****************************************/
bool CommandManagement::ReadMaxComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}



/****************************************
 *函数名称：FirmWareRevisionComHandle
 *函数描述：读固件版本
 *函数参数：void
 *函数返回：void
*****************************************/
bool CommandManagement::FirmWareRevisionComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;


}



/****************************************
 *函数名称：ScanningTimeComHandle
 *函数描述：读扫码时间限制设置
 *函数参数：void
 *函数返回：void
 *****************************************/
bool CommandManagement::ScanningTimeComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}



/****************************************
 *函数名称：SerialNumberComHandle
 *函数描述：唯一序号
 *函数参数：void
 *函数返回：void
 *****************************************/
bool CommandManagement::SerialNumberComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);   }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：ReadBootInterfaceComHandle
 *函数描述：读开机界面
 *函数参数：void
 *函数返回：void
 *****************************************/
bool CommandManagement::ReadBootInterfaceComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}



/****************************************
 *函数名称：ReadIpAdderss0ComHandle
 *函数描述：读取网卡地址
 *函数参数：int
 *函数返回：void
 *****************************************/
bool CommandManagement::ReadIpAdderss0ComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


bool CommandManagement::setNetworkEnv2EEprom(int comType, int len, char *buf)
{
    qDebug() << "CommandManagement::setNetworkEnv2EEprom: " << comType << ", len: " << len << ", buf: " << QString::fromLocal8Bit(buf);

    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(comType, len, buf, iLen, pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(comType,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型: " << comType; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;
}

bool CommandManagement::readNetWorkEvnFromEEprom(int comType)
{
    qDebug() << "CommandManagement::readNetWorkEvnFromEEprom: " << comType;

    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(comType, iLen, pBuf);

    if (bReturn)
        IF_ResponseNormalDataToUpperMonitor(comType,iLen,pBuf);
    else
        qDebug()<<"读数据错误:命令类型: " << comType;


    if (pBuf) {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;
}


/****************************************
 *函数名称：ReadIpAdderss1ComHandle
 *函数描述：读取网卡地址
 *函数参数：int
 *函数返回：void
 *****************************************/
bool CommandManagement::ReadIpAdderss1ComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：ReadTemperatureComHandle
 *函数描述：读取温度数据
 *函数参数：int
 *函数返回：void
 *****************************************/
bool CommandManagement::ReadTemperatureComHandle(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;
}



/****************************************
 *函数名称：ReadCurrentTimeCom
 *函数描述：读取温度数据
 *函数参数：int
 *函数返回：void
 *****************************************/
bool CommandManagement::ReadCurrentTimeCom(int com_type)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_ReadDevice(com_type,iLen,pBuf);

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"读数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}



/****************************************
 *函数名称：DispalyAdvertisingCom
 *函数描述：控制交互（广告）
 *函数参数：void
 *函数返回：bool
 * 1  欢迎界面
   2  支付成功
   3  支付失败
   4  清屏
 *****************************************/
bool CommandManagement::DispalyAdvertisingCom()
{

    qDebug()<<"into  CommandManagement DispalyAdvertisingCom";

    bool bretrun = false;

    try{
        
        IF_SetSingle(1);

        bretrun = true;

    }catch(...)
    {
        qDebug()<<"[串口服務响应],接收到交互信息，執行異常";
    }



    return bretrun;

}


/****************************************
 *函数名称：DispalyAdvertisingCom
 *函数描述：控制交互（再见）
 *函数参数：void
 *函数返回：bool
 * 1  欢迎界面
   2  支付成功
   3  支付失败
   4  清屏
   5  再见
 *****************************************/
bool  CommandManagement::DisplaySeeAgainComHandle()
{
    bool bretrun = false;

    try{

        // clear LCD display
        IF_SetSingle(4);

        bretrun = true;

    }catch(...)
    {
        qDebug()<<"[串口服務响应],接收到交互信息，執行異常";
    }


    return bretrun;


}


/****************************************
 *函数名称：SetHeartBeatSeptumCom
 *函数描述：设置心跳间隔
 *函数参数：int
 *函数返回：void
 *****************************************/
bool CommandManagement::SetHeartBeatSeptumCom(int com_type,int len,char* buf)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;


}



/****************************************
 *函数名称：SetBootInTerfaceComHandle
 *函数描述：设置开机界面
 *函数参数：int int char
 *函数返回：void
 *****************************************/
bool CommandManagement::SetBootInTerfaceComHandle(int com_type,int len,char* buf)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;
}

/****************************************
 *函数名称：SetIpAdderss0ComHandle
 *函数描述：设置网卡IP
 *函数参数：int int char
 *函数返回：void
 *****************************************/
bool CommandManagement::SetIpAdderss0ComHandle(int com_type,int len,char* buf)
{

    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}

/****************************************
 *函数名称：SetIpAdderss1ComHandle
 *函数描述：设置网卡IP
 *函数参数：int int char
 *函数返回：void
 *****************************************/
bool CommandManagement::SetIpAdderss1ComHandle(int com_type,int len,char* buf)
{

    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：SetCanIdComHandle
 *函数描述：设置CAN的IP
 *函数参数：int int char
 *函数返回：bool
 *****************************************/
bool CommandManagement::SetCanIdComHandle(int com_type,int len,char* buf)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：SetPollTestCom
 *函数描述：设置节目测试轮询测试
 *函数参数：int int char
 *函数返回：bool
 *****************************************/
bool CommandManagement::SetPollTestCom(int com_type,int len,char* buf)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}

bool CommandManagement::ackCommonCommandResp(int com_type)
{
    int iLen = 4;
    char *pBuf = NULL;
    pBuf = new char[4];
    memset(pBuf,0,4);

    IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf);

    return true;
}

/****************************************
 *函数名称：SetMaxComHandle
 *函数描述：设置Max
 *函数参数：int int char
 *函数返回：bool
 *****************************************/
bool CommandManagement::SetMaxComHandle(int com_type,int len,char* buf)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：SetSerialNumberCom
 *函数描述：设置唯一序号
 *函数参数：int int char
 *函数返回：bool
 *****************************************/
bool CommandManagement::SetSerialNumberCom(int com_type,int len,char* buf)
{

    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：DebitFlowChartCom
 *函数描述：设置费显
 *函数参数：int int char
 *函数返回：bool
 *****************************************/
bool CommandManagement::DebitFlowChartCom(int com_type,int len,char* buf)
{
    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;


}

/****************************************
 *函数名称：SetScanningTimeCom
 *函数描述：设置唯一序号
 *函数参数：int int char
 *函数返回：bool
 *****************************************/
bool CommandManagement::SetScanningTimeCom(int com_type,int len,char* buf)
{

    bool bReturn = false;
    int iLen = 0;
    char *pBuf = NULL;
    pBuf = new char[1024];
    memset(pBuf,0,1024);

    bReturn = IF_WriteDevice(com_type,len,buf,iLen,pBuf);  //写设备

    if(bReturn)
    {  IF_ResponseNormalDataToUpperMonitor(com_type,iLen,pBuf); }
    else
    { qDebug()<<"设置数据错误:命令类型"<<com_type; }


    if(pBuf != NULL)
    {
        delete pBuf;
        pBuf = NULL;
    }

    return bReturn;

}


/****************************************
 *函数名称：CommandInterpretation
 *函数描述：命令解释
 *函数参数：QByteArray
 *函数返回：void
*****************************************/
void CommandManagement::CommandInterpretation(int com_type,int lent, char* byteResponseCommand)
{

    //    qDebug() << "into CommandManagement::CommandInterpretation: com_type: " << com_type << "/" << lent;

    try{
        switch(com_type)
        {
        case LEGAL_USE_COM:               //判定终端是否合法使用
            break;
        case START_DEBIT_COM:             //发起扣款
            DebitFlowChartHandle(lent,byteResponseCommand);
            break;
        case START_CENCEL_COM:            //发起撤单
            CencelFlowChartHandle(lent,byteResponseCommand);
            break;
        case CHARGE_FLOW_KEY_COM:         //下发收费流水关键信息
            PLCTransmissionHandle(lent, byteResponseCommand);
            break;
        case LAST_ERROR_COM:              //要求提供上次错误信息
            break;

            /***************读取设备信息**********************/
        case HEARTBEAT_INQUIRY_COM:      //心跳查询
            IF_SendState(1,0,0);
            break;
        case HARDWARE_VERSION_COM :      //读硬件版本
            HardwareVersionComHandle(com_type);
            break;
        case READ_CANID_COM:             //读CANID
            ReadCanidComHandle(com_type);
            break;
        case READ_MAX_COM:               //读MAX码
            ReadMaxComHandle(com_type);
            break;
        case FIRMWARE_REVISION_COM:      //读固件版本
            FirmWareRevisionComHandle(com_type);
            break;
        case SCANNING_TIME_COM:          //读扫码时间限制设置
            ScanningTimeComHandle(com_type);
            break;
        case SERIAL_NUMBER_COM:          //唯一序号
            SerialNumberComHandle(com_type);
            break;
        case READ_BOOT_INTERFACE_COM:    //读开机界面
            ReadBootInterfaceComHandle(com_type);
            break;
        case READ_IP_ADDERSS0_COM:       //读网口0(eth0) IP 地址
            ReadIpAdderss0ComHandle(com_type);
            break;
        case READ_IP_ADDERSS1_COM:       //读网口1(eth1) IP 地址
            ReadIpAdderss1ComHandle(com_type);
            break;

            // ADD A6/B6 subcommand
        case READ_ETH0_NETMASK:
        case READ_ETH0_GATEWAY:
        case READ_ETH1_NETMASK:
        case READ_ETH1_GATEWAY:
        case READ_DEFAULT_GATEWAY:
        case READ_SCANER_VERSION:
            readNetWorkEvnFromEEprom(com_type);
            break;

        case SET_ETH0_NETMASK:
        case SET_ETH0_GATEWAY:
        case SET_ETH1_NETMASK:
        case SET_ETH1_GATEWAY:
        case SET_DEFAULT_GATEWAY:
        case SET_LCD_PARAMS:
        case SET_HW_VERSION:
            setNetworkEnv2EEprom(com_type, lent, byteResponseCommand);
            break;

        case READ_TEMPERATURE_COM:       //读硬件温度
            ReadTemperatureComHandle(com_type);
            break;
        case READ_CURRENT_TIME_COM:      //读硬件当前时间
            ReadCurrentTimeCom(com_type);
            break;
            // reboot smartTermianl
        case SYS_REBOOT_ST:
            ackCommonCommandResp(com_type); //ack
            qDebug() << "rebooting smart termial";
            sleep(2);
            system("reboot");
            break;

            /***************设置设备信息**********************/
        case SET_HEARTBEAT_SEPTUM_COM:   //设置终端心跳间隔
            SetHeartBeatSeptumCom(com_type,lent,byteResponseCommand);
            break;
        case SET_BOOT_INTERFACE_COM:     //设置开机界面
            SetBootInTerfaceComHandle(com_type,lent,byteResponseCommand);
            break;
        case DEBIT_FLOW_CHART_COM:       //控制交互显示信息(费显功能)
            DebitFlowChartCom(com_type,lent,byteResponseCommand);
            break;
        case DISPLAY_SEE_AGAIN_COM:      //控制交互（再见）
            ackCommonCommandResp(com_type); //ack
            DisplaySeeAgainComHandle();
            break;

        case DISPALY_ADVERTISING_COM:    //控制交互（广告）
            ackCommonCommandResp(com_type); //ack
            DispalyAdvertisingCom();
            break;
        case ISSUED_TIME_COM:            //下发车道时间
            break;

        case SET_IP_ADDERSS0_COM:        //设置网口0(eth0) IP 地址
            SetIpAdderss0ComHandle(com_type,lent,byteResponseCommand);
            break;

        case SET_IP_ADDERSS1_COM:        //设置网口1(eth1) IP 地址
            SetIpAdderss1ComHandle(com_type,lent,byteResponseCommand);
            break;

        case SET_CANID_COM:              //写CANID
            SetCanIdComHandle(com_type,lent,byteResponseCommand);
            break;
        case SET_MAX_COM:                //写MAX码
            SetMaxComHandle(com_type,lent,byteResponseCommand);
            break;
        case SET_SERIAL_NUMBER_COM:      //唯一序号
            SetSerialNumberCom(com_type,lent,byteResponseCommand);
            break;
        case SET_SCANNING_TIME_COM:      //设置扫码时间限制设置
            SetScanningTimeCom(com_type,lent,byteResponseCommand);
            break;
        case POLL_TEST_COM:              //节目测试轮询测试
            SetPollTestCom(com_type,lent,byteResponseCommand);
            break;
        case DATA_TO_PORT_COM:           //透传数据到指定串口
            break;

        }

    }

    catch (int& iErrorCode)
    {
        switch (iErrorCode)
        {
        case -1:
            m_strLastErrorDesc = "命令数据解析业务，心跳反饋流程失败，没有初始化";
            break;
        }

        qDebug()<<m_strLastErrorDesc;
        m_strLastErrorDesc.clear();
    }
}


/****************************************
 *函数名称：ReadUpperData
 *函数描述：命令解释
 *函数参数：QByteArray
 *函数返回：void
*****************************************/
void CommandManagement::ReadUpperData()
{
    if (!G_msgCommandDataList.isEmpty())
    {
        static QMutex t_mutex;
        t_mutex.lock();

        int  t_lent;
        int  t_ComType;
        char t_ByteCommandData[1024];

        // lock
    //    G_UpperReadMutex.lock();

        UpperCommandData *t_UpperCommandData = NULL;
        t_UpperCommandData = G_msgCommandDataList.at(0);
        t_lent = t_UpperCommandData->m_len;
        t_ComType = t_UpperCommandData->m_CommandType;      //命令類型

        memset(t_ByteCommandData,0,1024);
        memcpy(t_ByteCommandData,t_UpperCommandData->m_UpperData,t_lent);
        G_msgCommandDataList.removeAt(0);

        if( t_UpperCommandData != NULL)
        {
            delete  t_UpperCommandData;
            t_UpperCommandData = NULL;
        }

        // unlock
     //   G_UpperReadMutex.unlock();

        CommandInterpretation(t_ComType,t_lent,t_ByteCommandData);

        t_mutex.unlock();
    }
}

/****************************************
  *函数名称：CommandManagementInit
  *函数描述：初始化串口解析服务
  *函数参数：void
  *函数返回：void
 *****************************************/
void CommandManagement::CommandManagementInit()
{
    qDebug() << "CommandManagement::CommandManagementInit";

    G_AppVersion = ReadAppVersion();
    G_AppVersion.replace(QString("\r"),QString(""));
    G_AppVersion.replace(QString("\n"),QString(""));
    //看门狗设置
    IF_InitDev(30, HTProcessUpperData);

    m_ReadResultTimer = new  QTimer(this);
    connect(m_ReadResultTimer,SIGNAL(timeout()),this,SLOT(ReadUpperData()));
    connect(m_ReadResultTimer,SIGNAL(timeout()),this,SLOT(OnTimeOutCheckResult()));

    //主動上傳心跳命令
    connect(&m_DeviceInfoManagement,SIGNAL(sigSendHeartCommand(int,int)),this,SLOT(SendHeartData(int,int)));

    m_DeviceInfoManagement.DeviceInfoManagementInit();

    m_ReadResultTimer->start(20);

    qDebug()<<"****串口服務模塊初始化完成*****";

    // 上传自检命令
    SendSelfLoginData();
}

/****************************************
  *函数名称：OnMsgNetPayNotify
  *函数描述：获取消息
  *函数参数：int  , int
  *函数返回：void
 *****************************************/
void CommandManagement::OnMsgNetPayNotify(int wParam)
{
    char byFunCode      = char(wParam>>8);
    char byExecuteState = char(wParam);

    //获取结果
    int  iSize = MaxCommandDataLenth*2;
    char* pszResult = NULL;

    //接口数据
    QString t_strInterface;

    //命令数据
    QByteArray t_byteCommandData;

    switch (byFunCode)
    {
    case 1:		//扣款回馈
        if (1 == byExecuteState)
            break;//输入密码不上传

        if (0 == byExecuteState) {
            pszResult = new char[MaxCommandDataLenth*2];
            if (NULL != pszResult) {
                memset(pszResult,0,MaxCommandDataLenth*2);

                //获取扣款结果
                IF_GetDebitResult(pszResult, iSize,2);

                //解析扣费结果
                m_CDVJsonCommandDebitResult.fnAnalysisStringForData(QString(pszResult));

                // success
                IF_SetSingle(2);

                //构造上传结果
                t_strInterface = m_CDVJsonCommandDebitResult.DebitResultSuccess();
            }
        }
        else {
            // failed
            IF_SetSingle(3);

            //失败结果取不到，所以构造出来
            t_strInterface = m_CDVJsonCommandDebitResult.DebitResultfail(byFunCode, byExecuteState);
        }

        //数据长度
        m_ByteSendDataLenth = t_strInterface.toLocal8Bit().length();

        //构造上传上位机命令
        if (m_ByteSendDataLenth < MaxCommandDataLenth) {
            memset(m_ByteSendData, 0, MaxCommandDataLenth);
            memcpy(m_ByteSendData, t_strInterface.toLocal8Bit().data(), m_ByteSendDataLenth);

            //            qDebug() << "CommandManagement::OnMsgNetPayNotify: IF_SendDebitResult: " << t_strInterface;

            // shutdown scanner
            IF_SetSingle(6);

            // sent result to uppper
            IF_SendDebitResult(m_ByteSendDataLenth, m_ByteSendData);
        }

        break;


    case 2:     //撤单回馈
    {
        // filter empty payId
        string payId = PayDBInf::instance()->getCurrentOfCancelPayId().toStdString();
        if (!payId.empty()) {
            t_strInterface = m_CDVJsonCommandDebitResult.CencelResultCommand(byExecuteState, QString::fromStdString(payId));

            //数据长度
            m_ByteSendDataLenth = t_strInterface.toLocal8Bit().length();

            //构造上传上位机命令
            if (m_ByteSendDataLenth<MaxCommandDataLenth)
            {
                memset(m_ByteSendData,0,MaxCommandDataLenth);
                memcpy(m_ByteSendData,t_strInterface.toLocal8Bit().data(),m_ByteSendDataLenth);

                //                 qDebug() << "CommandManagement::OnMsgNetPayNotify: IF_SendDebitCancel: " << t_strInterface;
                IF_SendDebitCancel(m_ByteSendDataLenth, m_ByteSendData);
            }
        }

        break;
    }

    case 3:     //获取扫码数据
        if (0 == byExecuteState)
        {

        }
        break;
    }


    if(NULL != pszResult)
    {
        delete [] pszResult;
        pszResult = NULL;
    }
}

/****************************************
  *函数名称：OnMsgTimeOutResult()
  *函数描述：获取支付后数据
  *函数参数：void
  *函数返回：QByteArray
 *****************************************/
void CommandManagement::OnTimeOutCheckResult()
{
    if (ResultsComing)
    {
        OnMsgNetPayNotify(ResultsData);
        ResultsComing = false;
    }
}

/****************************************
  *函数名称：SendHeartData()
  *函数描述：获取支付后数据
  *函数参数：void
  *函数返回：void
 *****************************************/
void CommandManagement::SendHeartData(int iResult,int iState)
{
    IF_SendState(0, iResult, iState);
}

void CommandManagement::SendSelfLoginData()
{

    m_twsConfig->updateConfigFromHW();
    QString jsonStr = m_twsConfig->getSelfTestJsonData();

    m_D2Json = jsonStr;

    string strData = jsonStr.toStdString();
    int len = strData.length();

    qDebug() << "CommandManagement::SendSelfLoginData: " << len << "/" << strData.size() << "/" << jsonStr;

    m_SendD2Timer = new QTimer();
    connect(m_SendD2Timer,SIGNAL(timeout()),this,SLOT(ReTranD2Fun()));
    IF_ResponseNormalDataToUpperMonitor(0, len, (char*)strData.c_str());
    m_SendD2Timer->start(1000*5);
}
void CommandManagement::ReTranD2Fun()
{
    static int i = 0;
    i++;
    qDebug()<<tr("未收到A500，重发%1次").arg(i);
    IF_ResponseNormalDataToUpperMonitor(0,m_D2Json.toLocal8Bit().length(),m_D2Json.toLocal8Bit().data());
}
