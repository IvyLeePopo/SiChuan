#ifndef CTWSDNETPAY_DLLAPP_H
#define CTWSDNETPAY_DLLAPP_H

#include <QObject>
#include <QtCore/qglobal.h>
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
#include <QThread>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDebug>

class CTWSDNetPay_DllApp : public QThread
{
    Q_OBJECT
public:
    explicit CTWSDNetPay_DllApp(void);

    //内部辅助函数
    //将时间的字符串形式转化为time_t格式
    static time_t QString2time_t(QString lpstime);
    //获取当前时间的字符串形式
    static const QString GetCurrentTimeString(const int& iTimeType);

    //static void RecordLog( const QString& strLog);
    static void RecordLog(const QString& strLog);

   // QString ReadAppVersion();

    void run();

public slots:


protected:
    //权限验证
    static bool  IF_Authority( const char* szAuthKey, const int& iAuthKeySize);



public:

    bool m_bNeedPWD;
    //Attributes
        //业务管理线程类指针
        static CThreadManage*           m_pThreadManage;
        //标识组建环境是否正常初始化
        static bool                     m_bIsInitEnvironment;
        //标识调用是否经过合法授权
        static bool                     m_bAuthorizedLegal;
        //标识支付子平台网络是否正常
        static bool                     m_bNetErrorHttpServerInTollNetNotExist;
        //标识支付子平台服务器是否正常
        static bool                     m_bNetErrorHttpServerTrawePlatformNotExist;
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
        static tagTranstParam           m_sTranstParam;
        //当前的扣款操作是整合操作还是单体操作
        static bool                     m_bCurrentDebitMode;
        //标记当前是否正在交易(由业务管理线程进行重置:1.交易完毕;2,超时重置)
        static bool                     m_bIsTrading;
        //存储当前扣款操作的关键信息
        static tagPayKeyItems           m_sCurrentDebitInfo;
        //存储当前扣款结果的关键信息
        static tagDebitResultItems	m_sCurrentDebitResult;
        //当前扣款操作的超时起始时间
        static time_t                   m_dwTradeBeginOvertime;
        //往智能硬件更新时间的间隔
        static time_t                   m_dwUpdateDevieTimeSpanTime;
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
        //上层消息队列名称
        static char                     m_UpperMSName[30];
        //服务器通信协议版本
        static QString                  m_JsonVersion;
        //服务器通信协议版本
        static QString                  m_XMLVersion;
        //日志對象
        static C_logtxt                 m_Logtxt;
        //串口名字
        static char                     m_PortName[20];
        //串口波特率
        static int                      m_BaudRate;
        //消息回调函数
        static Fun_ChargeResults        m_Fun_ChargeResults;
        //当前服务器地址
        static QString                  m_CurrentServerAddress;
        //当前服务器端口
        static int                      m_CurrentServerPort;

        //PayId
        static QString                  m_payIdentifier;

};


extern CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp();


//初始化组件环境		    -- 接口1
//extern "C" Q_DECL_EXPORT bool  IF_InitEnvironment ( const quint32& nThreadID, const unsigned int& nNotifyMsgID, const char* szAreaInfo, const char* szLoaclStationID, const char* szLoaclLaneID, const char* szServerInfo, const int& iProvinceID,const char* MSName);
//JS
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
extern "C" Q_DECL_EXPORT bool IF_InitEnvironment(
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
//清理资源			    -- 接口2
extern "C" Q_DECL_EXPORT bool  IF_Destroy(void);
//获取上一次错误信息		-- 接口3
extern "C" Q_DECL_EXPORT const char*  IF_GetLastErrorDesc(void);
//启动获取用户信息操作		-- 接口4
extern "C" Q_DECL_EXPORT bool  IF_StartGetAccountInfo( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
extern "C" Q_DECL_EXPORT bool  IF_StopGetAccountInfo(void);
//获取用户信息				-- 接口6
extern "C" Q_DECL_EXPORT bool  IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
extern "C" Q_DECL_EXPORT bool  IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
extern "C" Q_DECL_EXPORT bool  IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
extern "C" Q_DECL_EXPORT bool  IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
extern "C" Q_DECL_EXPORT bool  IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
extern "C" Q_DECL_EXPORT bool  IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
extern "C" Q_DECL_EXPORT bool  IF_GetComponentStatus( unsigned int& uStatus);
//执行传输关键信息数据操作  -- 接口13
extern "C" Q_DECL_EXPORT bool  IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType);


#endif // CTWSDNETPAY_DLLAPP_H
