
#ifndef COMMANDMANAGEMENT_H
#define COMMANDMANAGEMENT_H

#include "cdvjsoncommanddebitmoney.h"
#include "cdvjsoncommanddebitcencel.h"
#include "cdvjsoncommanddebitinit.h"
#include "cdvjsoncommandwrite.h"
#include "cdvjsoncommanddebitresult.h"
#include "deviceinfomanagement.h"
#include "HWPerformance/hwperformance.h"
#include "deviceinfomanagement.h"
#include "cdvjsoncommandsetmmi.h"
#include "TWSDNetPay_Dll/ctwsdnetpay_dllapp.h"
#include "common/twsconfig.h"

#include <QTime>
#include <QObject>

#define HeartbeatInstruction_COM   0xA1
#define MutualInformation_COM      0xA2
#define DebitFlowchart_COM         0xA3
#define CencelFlowchart_COM        0xA4
#define PLCTransmission_COM        0xA5
#define Configuration_COM          0xA6
#define AuthorityControl_COM       0xA7

#define MaxCommandDataLenth        1024

#define LEGAL_USE_COM               1            //判定终端是否合法使用
#define START_DEBIT_COM             2            //发起扣款
#define START_CENCEL_COM            3            //发起撤单
#define CHARGE_FLOW_KEY_COM         4            //下发收费流水关键信息
#define LAST_ERROR_COM              5            //要求提供上次错误信息

#define HEARTBEAT_INQUIRY_COM      101           //心跳查询
#define HARDWARE_VERSION_COM       102           //读硬件版本
#define READ_CANID_COM             103           //读CANID
#define READ_MAX_COM               104           //读MAX码
#define FIRMWARE_REVISION_COM      105           //读固件版本
#define SCANNING_TIME_COM          106           //读扫码时间限制设置
#define SERIAL_NUMBER_COM          107           //唯一序号
#define READ_BOOT_INTERFACE_COM    108           //读开机界面
#define READ_IP_ADDERSS0_COM       109           //读网口0(eth0) IP 地址
#define READ_IP_ADDERSS1_COM       110           //读网口1(eth1) IP 地址
#define READ_TEMPERATURE_COM       111           //读硬件温度
#define READ_CURRENT_TIME_COM      112           //读硬件当前时间

// ADD A6/B6 subcommand
#define READ_ETH0_NETMASK           113
#define READ_ETH0_GATEWAY           114
#define READ_ETH1_NETMASK           115
#define READ_ETH1_GATEWAY           116
#define READ_DEFAULT_GATEWAY        117
#define READ_SCANER_VERSION         118

#define SET_ETH0_NETMASK            209
#define SET_ETH0_GATEWAY            210
#define SET_ETH1_NETMASK            211
#define SET_ETH1_GATEWAY            212
#define SET_DEFAULT_GATEWAY         213
#define SET_HW_VERSION              220
#define SET_LCD_PARAMS              200

#define SET_HEARTBEAT_SEPTUM_COM   201           //设置终端心跳间隔
#define SET_BOOT_INTERFACE_COM     202           //设置开机界面

#define DEBIT_FLOW_CHART_COM       203           //控制交互显示信息(费显功能)
#define DISPLAY_SEE_AGAIN_COM      204           //控制交互（再见）
#define DISPALY_ADVERTISING_COM    205           //控制交互（广告）

#define ISSUED_TIME_COM            206           //下发车道时间
#define SET_IP_ADDERSS0_COM        207           //设置网口0(eth0) IP 地址
#define SET_IP_ADDERSS1_COM        208           //设置网口1(eth1) IP 地址

#define SET_CANID_COM              302           //写CANID
#define SET_MAX_COM                301           //写MAX码
#define SET_SERIAL_NUMBER_COM      303           //唯一序号
#define SET_SCANNING_TIME_COM      304           //读扫码时间限制设置
#define POLL_TEST_COM              305           //节目测试轮询测试
#define DATA_TO_PORT_COM           306           //透传数据到指定串口
#define SYS_REBOOT_ST              307           //reboot smartTerminal

//上位机命令數據
class UpperCommandData
{
  public:
    char m_UpperData[1024];
    int  m_len;
    int  m_CommandType;
};

extern QList<UpperCommandData *> G_msgCommandDataList;

class CommandManagement : public QObject
{
    Q_OBJECT
public:
    explicit CommandManagement(QObject *parent = 0);
    void CommandManagementInit();


    void CommandInterpretation(int com_type,int lent, char* byteResponseCommand);

    ~CommandManagement();

    void OnMsgNetPayNotify(int wParam);

    bool DebitFlowChartHandle(int  lent, const char* byteResponseCommand);
    bool CencelFlowChartHandle(int lent, const char* byteResponseCommand);
    bool PLCTransmissionHandle(int lent, const char* byteResponseCommand);

    //读數據
    bool HearTbeatInouiryComHandle(int com_type);
    bool HardwareVersionComHandle(int com_type);
    bool ReadCanidComHandle(int com_type);
    bool ReadMaxComHandle(int com_type);
    bool FirmWareRevisionComHandle(int com_type);
    bool ScanningTimeComHandle(int com_type);
    bool SerialNumberComHandle(int com_type);
    bool ReadBootInterfaceComHandle(int com_type);
    bool ReadIpAdderss0ComHandle(int com_type);
    bool ReadIpAdderss1ComHandle(int com_type);
    bool ReadTemperatureComHandle(int com_type);
    bool ReadCurrentTimeCom(int com_type);

    QString ReadAppVersion();

    //设置节目
    bool DispalyAdvertisingCom();
    bool DisplaySeeAgainComHandle();

    //设置设备数据间隔
    bool SetHeartBeatSeptumCom(int com_type, int len, char *buf);
    bool SetBootInTerfaceComHandle(int com_type,int len,char* buf);
    bool SetIpAdderss0ComHandle(int com_type,int len,char* buf);
    bool SetIpAdderss1ComHandle(int com_type,int len,char* buf);
    bool SetCanIdComHandle(int com_type,int len,char* buf);
    bool SetMaxComHandle(int com_type,int len,char* buf);
    bool SetSerialNumberCom(int com_type,int len,char* buf);
    bool SetPollTestCom(int com_type,int len,char* buf);
    bool SetScanningTimeCom(int com_type,int len,char* buf);
    bool DebitFlowChartCom(int com_type,int len,char* buf);

    bool setNetworkEnv2EEprom(int comType, int len, char *buf);
    bool readNetWorkEvnFromEEprom(int comType);

    // setting for common command testing
    bool ackCommonCommandResp(int com_type);

signals:

public slots:
    void ReadUpperData();
    void OnTimeOutCheckResult();
    void SendHeartData(int iResult, int iState);
    void SendSelfLoginData();

public:
    char *m_ByteInterfaceData;          //接口数据
    int   m_ByteInterfaceDataLenth;     //接口数据长度

    char *m_ByteSendData;          //接口发送数据
    int   m_ByteSendDataLenth;     //接口发送数据长度

    char m_NumericalOrderH ;       //当前命令流水号
    char m_NumericalOrderL ;

private:
    bool m_IsInitFlag;
    QByteArray                m_byteCommandContent;
    CDVJsonCommandDebitResult m_CDVJsonCommandDebitResult;     //交易结果获取
    DeviceInfoManagement      m_DeviceInfoManagement;          //设备信息管理


    char m_CommandData[MaxCommandDataLenth];
 // char m_ByteCommandData[MaxCommandDataLenth];

    QTimer  *m_ReadResultTimer;     //读取数据定时器

    //写命令线程
    CDVJsonCommandWrite     m_CDVJsonCommandWrite;
    //错误描述
    QString                 m_strLastErrorDesc;
    //字符编码
//  QTextCodec              *m_codecGBK2312;

    //TWS config
    TWSConfig* m_twsConfig;
    //20170918
    QTimer *m_SendD2Timer;

    QString m_D2Json;
private slots:
    void ReTranD2Fun();
};

#endif // COMMANDMANAGEMENT_H
