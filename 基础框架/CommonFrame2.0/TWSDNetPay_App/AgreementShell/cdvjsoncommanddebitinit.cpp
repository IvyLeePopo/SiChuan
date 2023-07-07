#include "cdvjsoncommanddebitinit.h"
#include "common/twsutils.h"

bool ResultsComing = false;
int  ResultsData = 0;

CDVJsonCommandDebitInit::CDVJsonCommandDebitInit()
{
    m_localNetwork = new TWSLocalnet;
    m_twsAppConfig = TWSConfig::instance();
}

CDVJsonCommandDebitInit::~CDVJsonCommandDebitInit()
{
    if (m_localNetwork)
        delete m_localNetwork;
}

bool CDVJsonCommandDebitInit::fnAnalysisStringForData(const QString &strResponseResult)
{
    qDebug() << "CDVJsonCommandDebitInit::fnAnalysisStringForData: " << strResponseResult;

    bool breturn = false;

    QString strlog;

    try
    {
        //进行数据格式转换
        Json::Reader reader;
        Json::Value  root;
        Json::Value  DeviceParamContext;
        Json::Value  Data;


        QString strResult(strResponseResult);
        if(!reader.parse(strResult.toLocal8Bit().data(),root))
        {
            qDebug() << "CDVJsonCommandDebitInit::fnAnalysisStringForData: json parse error";
            strResult.clear();
            throw -2;
        }

        if(Json::nullValue == root.type()) {
            qDebug() << "CDVJsonCommandDebitInit::fnAnalysisStringForData: root.type() == Json::nullValue";
            throw -3;
        }

        //获取交易数据区
        if(Json::nullValue == root["DeviceParamContext"].type()) {
            qDebug() << "CDVJsonCommandDebitInit::fnAnalysisStringForData: root[\"DeviceParamContext\"].type() == Json::nullValue";
            throw -4;
        }

        DeviceParamContext = root["DeviceParamContext"];

        //获取数据明细区
        if(Json::nullValue == DeviceParamContext["Data"].type())
            throw -5;

        Data = DeviceParamContext["Data"];

         //省份行政编码
        if(Json::intValue != Data["ProvinceID"].type())
            throw -7;
        m_ProvinceID = Data["ProvinceID"].asInt();

        //区域编码
        if(Json::intValue != Data["AreaID"].type())
            throw -8;
        m_AreaID = Data["AreaID"].asInt();

        //路段编码
        if(Json::intValue != Data["RoadID"].type())
            throw -9;
        m_RoadID = Data["RoadID"].asInt();

        //站点编码
        if (Json::stringValue == Data["StationID"].type())
            m_StationID = Data["StationID"].asCString();
        else if (Json::intValue == Data["StationID"].type()) {
            m_StationID = QString::number(Data["StationID"].asInt());

            if (m_StationID.isEmpty())
                m_StationID = "0";
        }
        else
            throw -10;

        //车道编码
        if(Json::intValue != Data["LaneID"].type())
            throw -11;
        m_LaneID = Data["LaneID"].asInt();

        //站点名称
        if(Json::stringValue != Data["StationName"].type())
            throw -12;
        m_StationName = Data["StationName"].asCString();

        //服务器类型
        if(Json::intValue != Data["ServerType"].type())
            throw -13;
        m_ServerType = Data["ServerType"].asInt();

        //服务器IP
        if(Json::stringValue != Data["ServerIP"].type())
            throw -14;
        m_ServerIP = Data["ServerIP"].asCString();

        //服务器端口号
        if(Json::intValue != Data["ServerPort"].type())
            throw -14;
        m_ServerPort = Data["ServerPort"].asInt();

        //升级服务器端口
        if(Json::intValue == Data["UpdatePort"].type())
            m_UpdatePort = Data["UpdatePort"].asInt();
        else
            m_UpdatePort = 8089;

        //设备运营模式
        if(Json::intValue != Data["DevicePayMode"].type())
            throw -15;
        m_DevicePayMode = Data["DevicePayMode"].asInt();

        //服务器IP
        if(Json::stringValue != Data["CurrentTime"].type())
            throw -16;

        // setting system time
        m_CurrentTime = Data["CurrentTime"].asCString();
        TWSUtils::setSystemTime(m_CurrentTime);

        // get local networks from json
        // local netcard 0
        string blankStr = "";
        if (Json::stringValue == Data["ConfigLocalIP0"].type())
            m_localNetwork->setLocalIP0(Data["ConfigLocalIP0"].asString());
        else
            m_localNetwork->setLocalIP0(blankStr);

        if (Json::stringValue == Data["ConfigLocalSub0"].type())
            m_localNetwork->setLocalNetmask0(Data["ConfigLocalSub0"].asString());
        else
            m_localNetwork->setLocalNetmask0(blankStr);

        if (Json::stringValue == Data["ConfigLocalGate0"].type())
            m_localNetwork->setLocalGW0(Data["ConfigLocalGate0"].asString());
        else
            m_localNetwork->setLocalGW0(blankStr);

        // local netcard 1
        if (Json::stringValue == Data["ConfigLocalIP1"].type())
            m_localNetwork->setLocalIP1(Data["ConfigLocalIP1"].asString());
        else
            m_localNetwork->setLocalIP1(blankStr);

        if (Json::stringValue == Data["ConfigLocalSub1"].type())
            m_localNetwork->setLocalNetmask1(Data["ConfigLocalSub1"].asString());
        else
            m_localNetwork->setLocalNetmask1(blankStr);

        if (Json::stringValue == Data["ConfigLocalGate1"].type())
            m_localNetwork->setLocalGW1(Data["ConfigLocalGate1"].asString());
        else
            m_localNetwork->setLocalGW1(blankStr);

        // local default gateway
        if (Json::stringValue == Data["ConfigLocalDefGate"].type())
            m_localNetwork->setLocalDefaultGW(Data["ConfigLocalDefGate"].asString());
        else
            m_localNetwork->setLocalDefaultGW(blankStr);

        // setting system ip
        m_localNetwork->updateNetworksParam();

        // for middleware dll version
        if (Json::stringValue == Data["DeviceDllVersion"].type())
            m_twsAppConfig->setHTTWSDllVersion(Data["DeviceDllVersion"].asCString());
        else
            m_twsAppConfig->setHTTWSDllVersion("2.0.0.1");

        if (Json::stringValue == Data["NetPayDllVersion"].type())
            m_twsAppConfig->setPayDllVersion(Data["NetPayDllVersion"].asCString());
        else
            m_twsAppConfig->setPayDllVersion("2.0.1.8");

        // set payStationId
        m_twsAppConfig->setPayStationId(m_StationID);

        //获取软件版本号
        GetGlobalApp()->m_strNetPayDllVersion = m_twsAppConfig->getPayDllVersion();
        GetGlobalApp()->m_strDeviceDllVersion = m_twsAppConfig->getHTTWSDllVersion();

        //数据正常，撤单退款
        qDebug()<<"CDVJsonCommandDebitInit()";
    }

    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case -1:            strlog = "[A5指令],数据解析,异常找不到： ";                     break;
        case -2:            strlog = "[A5指令],数据解析,异常找不到： ";                     break;
        case -3:            strlog = "[A5指令],数据解析,异常找不到： ";                     break;
        case -4:            strlog = "[A5指令],数据解析,异常找不到： ";                     break;
        case -5:            strlog = "[A5指令],数据解析,异常找不到： ";                     break;
        case -6:            strlog = "[A5指令],数据解析,异常找不到： ";                     break;


        }
        qDebug()<<"异常: "<<iErrorCode;
    }

    //sync info to ini
    updateInitInfo2Config();

    qDebug()<<"\n~~~~~~A500 INIT~~~~~m_StationName1 :" << m_StationName << ", StationID: " << m_StationID;

    return breturn;
}

void CDVJsonCommandDebitInit::updateInitInfo2Config()
{
//    int     m_ProvinceID;            //省份行政编码
//    int     m_AreaID;                //区域编码
//    int     m_RoadID;                //路段编码
//    QString m_StationID;             //站点编码
//    int     m_LaneID;                //车道编码
//    QString m_ServerIP;              //服务器IP

    //写入ini配置
    QSettings* config = TWSConfig::instance()->getAppConfig();
    config->setValue("Initial/ProvinceID", m_ProvinceID);
    config->setValue("Initial/AreaID", m_AreaID);
    config->setValue("Initial/RoadID", m_RoadID);
    config->setValue("Initial/StationID", m_StationID);
    config->setValue("Initial/LaneID", m_LaneID);

    //服务器配置
    config->setValue("Server/ServerIP", m_ServerIP);
    config->setValue("Server/UpdatePort", m_UpdatePort);
}

QString	CDVJsonCommandDebitInit::FormatParamData2Json()
{





}




/****************************************
 *函数名称：CommandResult()
 *函数描述：构造命令结果
 *函数参数：void
 *函数返回：QByteArray
*****************************************/
QByteArray CDVJsonCommandDebitInit::CommandResult(char t_NumericalOrderH,char t_NumericalOrderL)
{

   ushort t_VerifyCode;
   QByteArray  t_InitCommand;
   t_InitCommand.append(COMMAND_START);

   t_InitCommand.append((char)0X00);
   t_InitCommand.append((char)0X01);

   t_InitCommand.append((uchar)t_NumericalOrderH);
   t_InitCommand.append((uchar)t_NumericalOrderL);

   t_InitCommand.append(0XB5);
   t_InitCommand.append(0X05);
   t_InitCommand.append(0X01);

   t_VerifyCode = GetVerifyCode(t_InitCommand,t_InitCommand.length());


   t_InitCommand.append((uchar)(t_VerifyCode>>8));
   t_InitCommand.append((uchar)t_VerifyCode);


   t_InitCommand.append(COMMAND_END);


   return  t_InitCommand;
}




/****************************************
 *函数名称：CommandResult()
 *函数描述：构造命令结果
 *函数参数：void
 *函数返回：QByteArray
*****************************************/
 bool CDVJsonCommandDebitInit::CommandDebitModelInit()
 {

     bool breturn = false;

     int tLenth_Parameter = 200;

     QByteArray byteIP_Parameter;
     QByteArray byteAreaIDRoadID_Parameter;
     QByteArray byteStationName_Parameter;

     char  *pIP_Parameter = NULL;             //服务器地址
     char  *pAreaIDRoadID_Parameter = NULL;   //区域路段
     char  *pStationName_Parameter = NULL;    //站名ID

     pIP_Parameter = new char[tLenth_Parameter];
     pAreaIDRoadID_Parameter = new char[tLenth_Parameter];
     pStationName_Parameter = new char[tLenth_Parameter];

     memset(pIP_Parameter,0,tLenth_Parameter);
     memset(pAreaIDRoadID_Parameter,0,tLenth_Parameter);
     memset(pStationName_Parameter,0,tLenth_Parameter);


     byteIP_Parameter = m_ServerIP.toLocal8Bit();
     byteAreaIDRoadID_Parameter = (QString::number(m_AreaID)+"#"+QString::number(m_RoadID)).toLocal8Bit();

     QString stationParam = m_StationID.append("#").append(m_StationName);
     byteStationName_Parameter = stationParam.toLocal8Bit();

     qDebug() << "~~~~~~2CDVJsonCommandDebitInit::CommandDebitModelInit: " << m_StationID << "/" << m_StationName << "/" << stationParam;

     memcpy(pIP_Parameter,byteIP_Parameter.data(),byteIP_Parameter.length());
     memcpy(pAreaIDRoadID_Parameter,byteAreaIDRoadID_Parameter.data(),byteAreaIDRoadID_Parameter.length());
     memcpy(pStationName_Parameter,byteStationName_Parameter.data(),byteStationName_Parameter.length());


     breturn = IF_InitEnvironment(
                     "/dev/ttyUSB0",
                     9600,
                     pIP_Parameter,
                     m_ServerPort,
                     true,
                     true,
                     20,
                     pAreaIDRoadID_Parameter,
                     pStationName_Parameter,
                     QString::number(m_LaneID).toLocal8Bit(),
                     "127.0.0.1",
                     m_ProvinceID,
                     G_ChargeResults );


       byteIP_Parameter.clear();
       byteAreaIDRoadID_Parameter.clear();
       byteStationName_Parameter.clear();

       if(NULL != pIP_Parameter)
       {
           delete [] pIP_Parameter;
           pIP_Parameter = NULL;
       }

       if(NULL != pAreaIDRoadID_Parameter)
       {
           delete [] pAreaIDRoadID_Parameter;
           pAreaIDRoadID_Parameter = NULL;
       }


       if(NULL != pStationName_Parameter)
       {
           delete [] pStationName_Parameter;
           pStationName_Parameter = NULL;
       }

       return breturn;

 }


