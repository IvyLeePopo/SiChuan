/***********
 * A5
************/
#ifndef CDVJSONCOMMANDDEBITINIT_H
#define CDVJSONCOMMANDDEBITINIT_H

#include <QObject>
#include <QTimer>

#include "cdvjsoncommanddebit.h"
#include "../TWSDNetPay_Dll/ctwsdnetpay_dllapp.h"
#include "miscs/twslocalnet.h"
#include "common/twsconfig.h"

extern bool ResultsComing ;
extern int  ResultsData ;

//static QMutex  G_Mutex;

static void  G_ChargeResults(int option,int result)
{
//    G_Mutex.lock();

    qDebug()<<"G_ChargeResults->T_option:"<<option;
    qDebug()<<"G_ChargeResults->T_result:"<<result;

    ResultsData = (option<<8)+result;
    ResultsComing = true;

//    G_Mutex.unlock();
}


class CDVJsonCommandDebitInit : public CDVJsonCommandDebit
{
    Q_OBJECT
public:
    explicit CDVJsonCommandDebitInit();
    ~CDVJsonCommandDebitInit();
    bool fnAnalysisStringForData(const QString &strResponseResult);
    QString	FormatParamData2Json();
    bool CommandDebitModelInit();

    QByteArray CommandResult(char t_NumericalOrderH,char t_NumericalOrderL);

private:
    void updateInitInfo2Config();

signals:

public slots:

public:
    int     m_ProvinceID;            //省份行政编码
    int     m_AreaID;                //区域编码
    int     m_RoadID;                //路段编码
    QString m_StationID;             //站点编码
    int     m_LaneID;                //车道编码
    QString m_StationName;           //站点名称
    int     m_ServerType;            //服务器类型
    QString m_ServerIP;              //服务器IP
    int     m_ServerPort;            //服务器端口号
    int     m_UpdatePort;            //升级服务器端口
    int     m_DevicePayMode;         //设备运营模式
    QString m_CurrentTime;           //车道当前时间

private:
    TWSLocalnet* m_localNetwork;

    // APP configs instance
    TWSConfig* m_twsAppConfig;
};

#endif // CDVJSONCOMMANDDEBITINIT_H
