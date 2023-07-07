#ifndef HTTWSDDEVICE_H
#define HTTWSDDEVICE_H

#include "httwsddevice_global.h"
#include <QObject>



//初始化硬件环境，接收上位机命令
bool IF_InitDev( int iOverTime, void (*fun)(int iType,int iSize,char* szData));
//清除申请的资源
extern "C" bool IF_Destroy(void);
//设置LED屏节目
bool IF_SetSingle(int iSingleType);
//显示扣款信息，同时启动支付凭证获取设备
bool IF_StartScan(int iMoney,int iType,int iClass,int iBalance,int iWeight,int iAxis,char* szEntryName,void (*fun)(int iType,int iSize,char* szData));
//向上位机发送终端状态信息(心跳，设备状态)
bool IF_SendState(int iFrameType,int iResult,int iState);
//给上位机回馈扣费结果状态信息
bool IF_SendDebitResult (int iSize,char* szData);
//给上位机回馈撤单结果状态信息
bool IF_SendDebitCancel(int iSize,char* szData);
//给上位机发特定的信息
bool IF_ResponseNormalDataToUpperMonitor(int iType,int iSize,char* szData);
//读设备
bool IF_ReadDevice(int iType,int &iLen,char* pBuf);
//写设备
bool IF_WriteDevice(int iType,int len,char* buf,int &iLen,char* pBuf);
//把从银联后台传来的数据写入到非接模块
bool IF_WriteDataToNFC(int iSize,char* szData);


class UpperQueueData
{
  public:
    char UpperData[1024];
    int len;
};
extern QList<UpperQueueData *> G_msgUpperList;

class HTTWSDDEVICESHARED_EXPORT HTTWSDDevice:public QObject
{
    Q_OBJECT
public:
    static HTTWSDDevice* instance();
    QString readJsonEepromData();
    bool writeJsonEepromData(QString json);

    // reture scaneCode to Payment Module
    void returnScanerCodeData(int iScanLen, char* cScanData);

private:
    HTTWSDDevice();
    ~HTTWSDDevice();

signals:
    void sigMessage2TMC(QString str, QString type);

public slots:
    void slMessageFromTMC(QByteArray str);
    void slMessageFromPOS(QString str, QString type);
    void slStartFirstSignIn();
    void slCorrectLastdeal();

private:
    static HTTWSDDevice *self;
};

#endif // HTTWSDDEVICE_H
