#include "hwperformance.h"

#include <QDebug>
#include "../TWSDNetPay_Dll/json/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QVariant>
#include <QList>

#include "httwsddevice.h"
#include "basedefine.h"

int    G_msgScanDataLent = 0;
char    G_msgScanData[1024];
char    G_typeScan;

#define TYPE_PAY_MOBILE     0x01
#define TYPE_PAY_QUICKPASS  0x02

HWPerformance *HWPerformance::self = NULL;

//回调函数--获取扫码数据
static void HTProcessScanData(int type,int iLenth,char *msg)
{
    qDebug() << "HTProcessScanData: recv scan data lenth: " << iLenth << ", type: " << type << ", scanData: " << QByteArray(msg, iLenth);

    QByteArray array = QByteArray(msg, iLenth);
    if (array.isEmpty())
        return;

    if (iLenth > 1024)
        return ;

    memset(G_msgScanData,0,1024);
    memcpy(G_msgScanData,msg,iLenth);

    G_msgScanDataLent = iLenth;
    G_typeScan = type;

    HWPerformance::instance()->onMessageUnderToDevice(SEND_QRCODE_NOTIFY);
}

HWPerformance* HWPerformance::instance ()
{
    if (NULL == self) {
        self = new HWPerformance();
    }

    return self;
}

//inform that get scaner data
void HWPerformance::onMessageUnderToDevice(int type)
{
    qDebug() << "onMessageUnderToDevice read data X:" << G_msgScanDataLent;
    emit ON_MSSAGE_UNDER_TO_DEVICER(type);
}

HWPerformance::HWPerformance(QObject *parent) : QObject(parent)
{

}

/*************************************************************************
    *
    * 类 名 称： InitDev
    * 摘    要： 主动资源申请
    * 参    数： int
    * 返    回： bool
    *
 ***************************************************************************/
bool HWPerformance::InitDev(int iOverTime)
{
    bool breturn = false;

    //  HW_RegisteScanCodeDataCallback(HTProcessScanData);

    breturn = true;

    return breturn;
}

/*************************************************************************
    *
    * 类 名 称： Destroy
    * 摘    要： 主动资源注销
    * 参    数： void
    * 返    回： bool
    *
 ***************************************************************************/
bool HWPerformance::Destroy(void)
{
    bool breturn = false;
    return breturn;
}


/*************************************************************************
    *
    * 类 名 称： GetData
    * 摘    要： 当调用者接收到硬件主动发送的消息，通知数据已经准备好，调用该接口获取硬件回传信息
    * 参    数： IN OUT CString& strData		--	回传的数据内容
    *			IN OUT int& iDataType		--	回传的数据类型	1-QR支付码（纯数字）	其它-未定义
    *			IN OUT int& iSrcType		--	回传数据所属内挂外设类型	1-扫码器	其它-未定义
    *
    * 返    回：bool
    *
 ***************************************************************************/
bool HWPerformance::GetData(char* strData, int& iDataType, int& iSrcType)
{
    qDebug() << "HWPerformance::GetData from scaner or QuickPass Module";

    bool bReturn = false;
    QString strLOG ;

    try
    {
        iSrcType =  G_typeScan;
        iDataType = G_typeScan;

        memcpy(strData, G_msgScanData, G_msgScanDataLent);

        qDebug() << "GetData: " << G_msgScanDataLent << "/" << QString(strData);

        // clear G_msgScanData
        memset(G_msgScanData, 0x0, 1024);

        G_msgScanDataLent = 0;
        G_typeScan = 0;

        bReturn = true;
    }
    catch(int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case 1:
            strLOG = " ";

            break;
        }
        qDebug()<<strLOG;
    }

    return bReturn;

}



/*************************************************************************
    *
    * 类 名 称： SetDisplayContext
    * 摘    要： 获取智能硬件显示部分的内容,暂不实现
    * 参    数：  int& iID			    --	显示设备具体ID
    *            int  iType             --  消息类型
    *			 std::string& strXML	--	返回来的显示内容描述
    * 返    回：bool
    * 备    注：1、整合操作，2费显操作
 ***************************************************************************/
bool HWPerformance::SetDisplayContext(const int iID,const int iType, QString istrData)
{
    /*
     <?xml version="1.0" encoding="UTF-8"?>
     <DisplayContext>
     <Version>1.0</Version>
     <ProvinceID>63</ProvinceID>
     <VerifyCode>1</VerifyCode>
     <Data>
     <Type>1</Type>
     <Money>122</Money>
     <Weight>122</Weight>
     <VehicleType>1</VehicleType>
     <VehicleClass>1</VehicleClass>
     <Balance>12</Balance>
     <CurrentTime>2016-07-22 12:23:11</CurrentTime>
     </Data>
     </DisplayContext>
    */

    bool bReturn = false;
    QString strLOG ;

    try
    {
        tagScanDisplay	sScanTemp;
        memset(&sScanTemp,0,sizeof(tagScanDisplay));

//        QString t_strXML = QString(istrData);
//        CXmlDisplayContext xml;
//        if(!xml.Init(t_strXML))
//            throw 1;

//        xml.GetTagScanDisplay(sScanTemp);

//        int vType = sScanTemp.VehicleType;

        Json::Reader reader;
        Json::Value root;
        Json::Value DisplayContext;
        Json::Value Data;

        if(!reader.parse(istrData.toLocal8Bit().data(),root))
            throw 1;
        DisplayContext = root["DisplayContext"];
        Data = DisplayContext["Data"];

        int iMoney         = Data["Money"].asInt();
        int iVehicleType = Data["VehicleType"].asInt();
        int iVehicleClass = Data["VehicleClass"].asInt();
        int iBalance        = Data["Balance"].asInt();
        int iWeight         = Data["Weight"].asInt();
        int iAxis              = Data["Axis"].asInt();
        QString strEntry = Data["EntrySTName"].asCString();

        IF_StartScan(iMoney, iVehicleType, iVehicleClass, iBalance,
                     iWeight, iAxis, strEntry.toLocal8Bit().data(), HTProcessScanData);

        bReturn = true;
    }

    catch (int& iErrorCode)
    {
        switch(iErrorCode)
        {
        case 1:
            strLOG = "HWPerformance::SetDisplayContext接口, XML解析错误";
            break;
        case 2:
            strLOG = "HWPerformance::SetDisplayContext接口, 播放声音出示支付码失败!";
            break;
        case 3:
            strLOG = "HWPerformance::SetDisplayContext接口, 打开扫码头失败!";
            break;
        case 4:
            strLOG = "HWPerformance::SetDisplayContext接口, LED显示出示支付码失败!";
            break;
        }

        qDebug()<<strLOG;
    }

    return bReturn;
}


/*************************************************************************
    *
    * 类 名 称： SetDisplayPayResults
    * 摘    要：设置支付结果
    * 参    数：int iID 设置类型 //1、支付成功 2、支付失败 3、清屏
    *          strJSON 附带信息
    * 返    回：bool
    *
 ***************************************************************************/
bool HWPerformance::SetDisplayPayResults(const int iID, QString strData)
{
    switch (iID) {
    //    case 1:  //1、支付成功
    //        IF_SetSingle(2);
    //        break;
    //    case 2:  //2、支付失败
    //        IF_SetSingle(3);
    //          break;
    case 3:  //3、清屏
        IF_SetSingle(4);
        break;
    default:
        break;
    }

}

