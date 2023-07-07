#ifndef COMMONBUS_H
#define COMMONBUS_H


#define COMMONBUS_LIBRARY


#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
#if defined(COMMONBUS_LIBRARY)
#define COMMONBUSSHARED_EXPORT __declspec(dllexport)
#else
#define COMMONBUSSHARED_EXPORT __declspec(dllimport)
#endif
#else
#define COMMONBUSSHARED_EXPORT
#endif

enum COMMPENTS_KEY_TYPE
{
    KEY_COMMON_FRAME=1,

    KEY_COM_MANAGER=10,
    //业务类
    KEY_BUSINESS=20,
    KEY_BUSINESS_DEBIT=21,
    KEY_BUSINESS_DEBIT_CANCEL=22,
    KEY_BUSINESS_DEBIT_INIT=23,



    KEY_DB_MANAGER=40, //数据库
    KEY_AUTO_UPDATE_MANAGER=60,//自动更新
    KEY_UPDATE_QUERY,
    KEY_UPDATE_DOWNLOAD,
    KEY_UPDATE_CONFIRM,
    KEY_UPDATE_HTTP_MANAGER,

    KEY_HTTP_MANAGER=70,//HTTP网络类
    //
    //操作硬件类
    KEY_HEARTBEAT_INQUIRY_COM=101,//心跳查询
    KEY_HARDWARE_VERSION=102,//读硬件版本
    KEY_READ_CANID=103,//读CANID
    KEY_READ_MAX=104,//读MAX码
    KEY_FIRMWARE_REVISION=105,//读固件版本
    KEY_SCANNING_TIME=106, //读扫码时间限制设置
    KEY_SERIAL_NUMBER=107,//唯一序号
    KEY_READ_BOOT_INTERFAC=108,//读开机界面
    KEY_READ_IP_ADDERSS0=109,//读网口0(eth0) IP 地址
    KEY_READ_IP_ADDERSS1=110,//读网口1(eth1) IP 地址
    KEY_READ_TEMPERATURE=111,//读硬件温度
    KEY_READ_CURRENT_TIME=112,//读硬件当前时间
    KEY_READ_ETH0_NETMASK=113,
    KEY_READ_ETH0_GATEWAY=114,
    KEY_READ_ETH1_NETMASK=115,
    KEY_READ_ETH1_GATEWAY=116,
    KEY_READ_DEFAULT_GATEWAY=117,
    KEY_READ_SCANER_VERSION=118,

    KEY_SET_LCD_PARAMS=200,
    KEY_SET_HEARTBEAT_SEPTUM=201,//设置终端心跳间隔
    KEY_SET_BOOT_INTERFACE=202,//设置开机界面
    KEY_DEBIT_FLOW_CHART_COM=203,//控制交互显示信息(费显功能
    KEY_DISPLAY_SEE_AGAIN_COM=204,//控制交互（再见）
    KEY_DISPALY_ADVERTISING_COM=205, //控制交互（广告）
    KEY_ISSUED_TIME_COM=206,//下发车道时间
    KEY_SET_IP_ADDERSS0_COM=207,//设置网口0(eth0) IP 地址
    KEY_SET_IP_ADDERSS1_COM=208, //设置网口1(eth1) IP 地址
    KEY_SET_ETH0_NETMASK=209,
    KEY_SET_ETH0_GATEWAY=210,
    KEY_SET_ETH1_NETMASK=211,
    KEY_SET_ETH1_GATEWAY=212,
    KEY_SET_DEFAULT_GATEWAY=213,
    KEY_SET_HW_VERSION214=220,

    KEY_SET_MAX_COM=301,//写CANID
    KEY_SET_CANID_COM=302, //写MAX码
    KEY_SET_SERIAL_NUMBER_COM=303,//唯一序号
    KEY_SET_SCANNING_TIME_COM=304,//读扫码时间限制设置
    KEY_POLL_TEST_COM=305, //节目测试轮询测试
    KEY_DATA_TO_PORT_COM=306,//透传数据到指定串口
    KEY_SYS_REBOOT_ST=307//reboot smartTerminal
};



class  COMMONBUSSHARED_EXPORT  CommonBus
{

public:
    CommonBus();
    virtual ~CommonBus();
    //注册  key:代表组件在全局定义的唯一键值
    virtual void register2Bus(int key);
    //注销  key:代表组件在全局定义的唯一键值
    virtual void unRegister2Bus(int key);
    /*
    发布
    destKey:代表目标组件在全局定义的唯一键值
    msg:需要发送的消息内容
    len:需要发送的消息内容的长度
    srcKey：本组件的全局定义的唯一键值。默认为0。
*/
    virtual int send2Bus(int destKey,char *msg,int len,int srcKey=0);

    /*
        订阅
        destKey:代表本组件的全局定义的唯一键值
        msg:需要发送的消息内容
        len:需要发送的消息内容的长度
        srcKey：代表源组件在全局定义的唯一键值
    */
    virtual int recvFromBus(int destKey,char *msg,int len, int srcKey=0);

public:

    int commpentsKey;
};




#endif // COMMONBUS_H
