#ifndef TWSDNETPAY_H
#define TWSDNETPAY_H

#include "twsdnetpay_global.h"

#include <QObject>
#include <QtCore/qglobal.h>
#include<QSettings>
#include<QDir>
#include <QThread>
#include<QTextCodec>
#include "cthreadmanage.h"
#include "basedefine.h"
#include "cdvxmldebitcancel.h"
#include "cdvjsonparamdebitcancel.h"
#include "cjsonparamoperate.h"
#include "cdvxmldebitresult.h"
#include "cdvxmldebitparamcontext.h"
#include "c_logtxt.h"
#include "cdvjsonparamaccount.h"
#include "cdvxmlmmi.h"
#include "cdvjsonparammmi.h"
#include "cdvxmlaccountcontext.h"
#include "cthreaddevice.h"


//通用回调函数
typedef void (*NotifyCallerFun)(int iOption,int iResult);
//广西专用回调函数
typedef void (*NotifyCallerFun45)(int iOption,int iResult,void* pIndPtr);


class TWSDNetPay : public QThread
{
    Q_OBJECT
public:
    explicit TWSDNetPay(void);
    ~TWSDNetPay();
    void run();

    //写日志
    static void  WriteLogFile(QString   strLog);

    //内部辅助函数
    //将时间的字符串形式转化为time_t格式
    static time_t QString2time_t(QString lpstime);
    //获取当前时间的字符串形式
    static const QString GetCurrentTimeString(const int& iTimeType);

    //重构扣款xml/json
    QString ConstructDebitInfo(int nFormateType, tagPayKeyItems &pkItems );
    //重构费显josn/xml
    QString ConstructMMIInfo(int nFormateType, const QString& strOrgString , int& nMMItype);
    //重构撤单JSON/xml
    QString ConstructCancleInfo(int nFormatType,QString strPayID);

    //构造清屏指令
    QString GetClearScreenJson();

    //获取指定文件的版本号
    QString  GetVersionOfFile(const char* lpsPath);
    //创建指定的多级目录
    bool MakeDirectory(const char* p_dir);
    //通知外部调用者，异步处理结果
    static void NotifyCaller( const int& iOption, const int& iResult);

protected:
    //权限验证
    static  bool  IF_Authority( const char* szAuthKey, const int& iAuthKeySize);
    //读取配置文件信息
    static void   ReadConfigInfo(void);

    //Attributes
public:

    //业务管理线程类指针
    static CThreadManage*       m_pThreadManage;
    //标识组建环境是否正常初始化
    static bool                     m_bIsInitEnvironment;
    //标识调用是否经过合法授权
    static bool                     m_bAuthorizedLegal;

    //标识是否需要保存日志
    static bool                     m_bSaveLog;
    //线程存活检测时间间隔(单位:秒),默认5分钟检测一次
    static quint32                  m_dwThreadCheckSpanTime;
    //存储主动态库工作目录
    static QString                  m_strWorkDir;
    //存储上次错误信息指针
    static QString                  m_strLastErrorDesc;
    //存储虚拟卡号前10位
    static QString                  m_strVirtualCardPrefix;
    //存储线程配置信息
    static tagTranstParam      m_sTranstParam;
    //当前的扣款操作是整合操作还是单体操作
    static bool                     m_bCurrentDebitMode;  // true: 整合操作, false: 单体操作
  // true: 正在交易 , false:  已完成交易
    static bool                     m_bIsTrading;
    //存储当前扣款操作的关键信息
    static tagPayKeyItems           m_sCurrentDebitInfo;
    //存储当前扣款结果的关键信息
    static tagDebitResultItems	m_sCurrentDebitResult;
    //往智能硬件更新时间的间隔
    static quint32                   m_dwUpdateDevieTimeSpanTime;
    //标识测试数据
    static bool                     m_bIsTestData;
    //业务动态库版本信息
    static QString                  m_strNetPayDllVersion;
    //设备控制动态库版本
    static QString                  m_strDeviceDllVersion;
    //日志删除操作指定时间段起点
    static int                      m_iDeleteLogStartHour;
    //日志删除操作指定时间段终点
    static int                      m_iDeleteLogEndHour;
    //日志删除操作轮询时间
    static int                      m_iDeleteLogSpanTime;
    //日志保存天数
    static int                      m_iDeleteLogSaveDays;
    //标识扣款流程编码
    static int                      m_iDebitTaskNo;
    //服务器通信协议版本
    static QString                  m_JsonVersion;
    //服务器通信协议版本
    static QString                  m_XMLVersion;
    //日志對象
    static C_logtxt                 m_Logtxt;
    //串口名字
    static char                      m_PortName[20];
    //串口波特率
    static int                         m_BaudRate;

    //当前服务器地址
    static QString                  m_CurrentServerAddress;
    //当前服务器端口
    static int                      m_CurrentServerPort;


    //标识组件通知外部调用者的方式(1,消息回抛；2，使用回调函数)
    static int	m_iNotifyMode;
    //标识组件通知外部使用的回调函数指针
    static NotifyCallerFun m_fnCallbackNotifyFun;
    //标识组件通知外部使用的回掉函数指针（广西专用）
    static NotifyCallerFun45 m_fnCallbackNotifyFun45;
    //专门为广西定做的初始化接口3，用于保存一个个性化指针，在回掉通知的时候传递出去
    static void* m_pIndPtrFor45;

};


extern TWSDNetPay theApp;

TWSDNetPay* GetGlobalApp();

///////////////////////////API    Interface////////////////////////////////////////////////////////

////初始化组件环境		    -- 接口1
///******************************************
// *参数列表
//        1.串口号
//        2.串口波特率
//        3.代理服务器地址，可留空
//        4.服务器链接端口，默认80（公网测试为8081）
//        5.服务器是否开放端口，默认0（公网测试为1）
//        6.是否产生日志标记（0或1），默认为1
//        7.日志保存天数，整型，默认20
//        8.区域信息
//        9.本地站点信息
//        10.本地车道号信息
//        11.本地服务器信息
//        12.省份编码
//        13.回调函数声明
//* ***************************************/
//extern "C" TWSDNETPAYSHARED_EXPORT bool IF_InitEnvironment(
//        const char *tty,
//        int        baudRate,
//        const char *serverIP,
//        int        serverPort,
//        bool       isOpenServerPort,
//        bool       logFlag,
//        int        logSaveTime,
//        const char *szAreaInfo,
//        const char *szLoaclStation,
//        const char *szLoaclLaneID,
//        const char *szServerInfo,
//        const int  &iProvinceID,
//        void (*fun)(int option, int result)
//        );


////清理资源			-- 接口2
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_Destroy(void);
////获取上一次错误信息		-- 接口3
//extern "C" TWSDNETPAYSHARED_EXPORT const char*  IF_GetLastErrorDesc(void);
////启动获取用户信息操作		-- 接口4
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_StartGetAccountInfo( const int& iOvertime);
////停止获取用户信息操作		-- 接口5
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_StopGetAccountInfo(void);
////获取用户信息				-- 接口6
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType);
////执行扣款操作				-- 接口7
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType);
////执行撤单操作				-- 接口8
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType);
////获取扣款结果				-- 接口9
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType);
////设置交互界面				-- 接口10
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType);
////获取硬件参数				-- 接口11
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
////检测组件状态				-- 接口12
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_GetComponentStatus( unsigned int& uStatus);
////执行传输关键信息数据操作                  -- 接口13
//extern "C" TWSDNETPAYSHARED_EXPORT bool  IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType);

////////////////////////////////////////////////////////////////////////////////////////////////////



//改成普通的全局方法-- 20171115

///////////////////////////API    Interface////////////////////////////////////////////////////////

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
 bool Inner_IF_InitEnvironment(
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
 bool  Inner_IF_Destroy(void);
//获取上一次错误信息		-- 接口3
const char*  Inner_IF_GetLastErrorDesc(void);
//启动获取用户信息操作		-- 接口4
bool  Inner_IF_StartGetAccountInfo( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
 bool  Inner_IF_StopGetAccountInfo(void);
//获取用户信息				-- 接口6
 bool  Inner_IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
 bool  Inner_IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
 bool  Inner_IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
 bool  Inner_IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
 bool Inner_IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
 bool  Inner_IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
 bool  Inner_IF_GetComponentStatus( unsigned int& uStatus);
//执行传输关键信息数据操作                  -- 接口13
 bool  Inner_IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType);




#endif // TWSDNETPAY_H
