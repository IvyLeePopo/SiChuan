#ifndef NETPAYDLL_H
#define NETPAYDLL_H

#include "twsdnetpay_global.h"

#include <CommonBus.h>

class TWSDNETPAYSHARED_EXPORT   TWSDNetPay_Dll :public CommonBus
{
public:
    TWSDNetPay_Dll();
    ~TWSDNetPay_Dll();
    static TWSDNetPay_Dll* instance();
    static TWSDNetPay_Dll*self;
//    struct
//    {
//        int type;
//        char *msg;
//    } BUS_MSG;


 void recvFromBus(int destKey, char *msg, int len, int srcKey);



public:



};

extern "C" TWSDNETPAYSHARED_EXPORT CommonBus* getInstance(); //获取类TestDll的对象
extern "C" TWSDNETPAYSHARED_EXPORT CommonBus* deleteInstance(); //获取类TestDll的对象


/////////////////////////API    Interface////////////////////////////////////////////////////////

//初始化组件环境		    -- 接口1
/******************************************
 *参数列表
        1.串口号
        2.串口波特率
        3.代理服务器地址，可留空
        4.服务器链接端口，默认80（公网测试为8081）
        5.服务器是否开放端口，默认0（公网测试为1）
        6.是否产生日志标记（0或1），默认为1
        7.日志保存天数，整型，默认20
        8.区域信息
        9.本地站点信息
        10.本地车道号信息
        11.本地服务器信息
        12.省份编码
        13.回调函数声明
* ***************************************/
extern "C" TWSDNETPAYSHARED_EXPORT bool NetPay_IF_InitEnvironment(
        const char *tty,
        int        baudRate,
        const char *serverIP,
        int        serverPort,
        bool       isOpenServerPort,
        bool       logFlag,
        int        logSaveTime,
        const char *szAreaInfo,
        const char *szLoaclStation,
        const char *szLoaclLaneID,
        const char *szServerInfo,
        const int  &iProvinceID,
        void (*fun)(int option, int result)
        );


//清理资源			-- 接口2
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_Destroy(void);
//获取上一次错误信息		-- 接口3
extern "C" TWSDNETPAYSHARED_EXPORT const char*  NetPay_IF_GetLastErrorDesc(void);
//启动获取用户信息操作		-- 接口4
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_StartGetAccountInfo( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_StopGetAccountInfo(void);
//获取用户信息				-- 接口6
extern "C" TWSDNETPAYSHARED_EXPORT bool NetPay_IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
 extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_GetComponentStatus( unsigned int& uStatus);
//执行传输关键信息数据操作                  -- 接口13
extern "C" TWSDNETPAYSHARED_EXPORT bool  NetPay_IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType);

//////////////////////////////////////////////////////////////////////////////////////////////////









#endif // NETPAYDLL_H
