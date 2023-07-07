#ifndef CTHREADMANAGE_H
#define CTHREADMANAGE_H

#include <QObject>
#include <QThread>
#include "basedefine.h"
#include "cthreadhttp.h"
#include <QTimer>
#include "cthreadbase.h"
#include "chtsddevicedll.h"
#include "cthreaddevice.h"
#include "basedefine.h"
#include <QDateTime>
#include "cjsonoperationif.h"
#include "cdvjsonbase.h"
#include "cdvjsondebitcheck.h"
#include "cthreaduppermange.h"
#include "cdvcallbackthread.h"
#include "TAesClass.h"
#include <QDir>
#include <QDebug>
#include <QFile>
#include "crypt/AesECB.h"
#include <QNetworkReply>
#include "upay/upaytmc.h"
#include "deviceManager/devicemgtthread.h"

class CThreadManage : public CThreadBase
{
    Q_OBJECT
public:
    explicit CThreadManage(void);
    //启动工作线程
    const bool StartWorkThreads( const tagTranstParam& sTranstParam);
    //判断用户数据是否已经就绪（硬件提供）
    const bool GetAccountDataReadyState(void) {return m_bAccountInfoIsReady;}
    //设置用户数据是否准备就绪
    void SetAccountDataReadyState( const bool& bState);
    //获取上次检索到的用户数据
    const tagAccountInfo GetLastAccountInfo(void){return m_sAccountInfo;}
    //设置定时器
    void StartTimer( const quint32& nEventID, const quint64& dwDelayTime);
    //停止定时器
    void StopTimer(const quint32& nEventID);
    //获取硬件驱动加载状况
    const bool GetDeviceLoadStae(void) const {return m_bDeviceLoadState;}

    // update sTranstParam
    void updateTransParam(const tagTranstParam& sTranstParam);


    //theAPP
    void  SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString sParm);
    void  SEND_WM_THREAD_MANAGE_START_DEBIT_FLOW(const QString &sParm);
    void  SEND_WM_THREAD_MANAGE_ONLY_DEBIT_MONEY(void);
    void  SEND_WM_THREAD_DEVICE_STOP_GET_DATA(void);
    void  SEND_WM_THREAD_DEVICE_START_GET_DATA(int lParm,int wParm);
    void  SEND_WM_THREAD_MANAGE_DEBIT_CANCEL(QString sParm);
    void  SEND_WM_THREAD_DEVICE_CLEAR(int lParm,QString sParm);



    void  SEND_WM_THREAD_DEVICE_NOTIFY();
    void  SEND_WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT();
    void  SEND_WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE();
    void  SEND_WM_THREAD_DESTROY_RESOURCE();




private:

    //构造发送到支付子平台的数据
    bool ContructDebitMoneyInfo( const tagAccountInfo* pAccountInfo);
    //构造发送到支付子平台的数据
    bool ContructDebitMoneyInfo(void);
    //构造发送到支付子平台的数据
    bool ContructDebitCancelInfo(const QString& strPayIdentifier);

    // quickpass support
    bool ContructDebitCancelInfo(const QString& strPayIdentifier, QString contdata);

    //交易结束的处理
    QString FinishTrade( const bool& bNormal);
    //获取交易匹配码
    const QString GetNetPayRecordMatchCode(const QString& strEncKey="");

signals:
    void WM_THREAD_DEVICE_START_GET_DATA_TODEVICE(int);
    void WM_THREAD_DEVICE_STOP_GET_DATA_TODEVICE(void);
//    void WM_THREAD_DEVICE_NOTIFY_TODEVICE(int,QString);
    void WM_THREAD_DEVICE_SHOW_DEAL_INFO_TODEVICE(QString);
    void WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI_TODEVICE(int,QString);
    void WM_THREAD_DEVICE_START_DEBIT_FLOW_TODEVICE(const QString& data);
    void WM_THREAD_DEVICE_CLEAR_TODEVICE(int,QString);

    void WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE(void);

    //网络信号
    void WM_THREAD_HTTP_SEND_PostQuery(QString* );
    void WM_THREAD_HTTP_SEND_PostCancel(QString* t_data, bool beCancel);
    void WM_THREAD_HTTP_SEND_PostCharging(QString* );
    void WM_THREAD_HTTP_SEND_SecondPostCharging(QString);
    void WM_THREAD_HTTP_SEND_GetHeartbeat(void);


//TheAPP 信号
    void  WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString);
    void  WM_THREAD_MANAGE_START_DEBIT_FLOW(const QString &data);
    void  WM_THREAD_MANAGE_ONLY_DEBIT_MONEY(void);
    void  WM_THREAD_DEVICE_STOP_GET_DATA(void);
    void  WM_THREAD_DEVICE_START_GET_DATA(int,int);
    void  WM_THREAD_MANAGE_DEBIT_CANCEL(QString);


    void  WM_THREAD_DEVICE_NOTIFY();
    void  WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT();
 //   void  WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE();
    void  WM_THREAD_DESTROY_RESOURCE();

    // quickpass support
    void WM_THREAD_QUICKPASS_CORRECT(QString payId);

private:
    // 增加撤单相关处理
    void updateDebitCancelById(QString id);
    void addDebitCancelId2DB(QString id);
    QString getDebitCancelIdFromDB();

    // api for contron lcd
    void refreshLCDDisplay();

public slots:
    // slot of clear LED display
    void slTimeoutClearDisplay();

    //处理启动获取用户信息操作的消息
    void OnMsgStartGetAccountInfo(int wParam,int lParam);
    //处理停止获取用户信息操作的消息
    void OnMsgStopGetAccountInfo(void);
    //处理获取用户信息的消息
    void OnMsgReceiveAccountInfo(int wParam, tagAccountInfo *lParam);
    //处理支付子平台回馈的消息
    void OnMsgDealSubPayPlatformResponse(CDVJsonBase *wParam,int wOpType,int wOpState);
    //查询用户输入密码的结果状态
    void OnMsgCheckDebitPasswordState(void);
    //执行扣款操作(整合模式)
    void OnMsgManageStartDebitFlow(const QString &wParam);
    //执行扣款操作(单体模式)
    void OnMsgOnlyDebitMoney(void);
    //执行撤单操作
    void OnMsgDebitCancel(QString wParam);
    //执行显示操作
    void OnMsgShowInfo(QString wParam);
    //解析HTTP数据
    void HttpDataReception(QString t_httpData,int m_modeType);

    //http reply
    void slHttpReplyReception(QNetworkReply* reply, int op);

    //parse heartbeat from httpThread
    void slHttpDataHeartBeat(QString t_httpData);

    //定时器超时处理--扫码
    void TimerIDForScanCodeProc();
    //定时器超时处理--扣费
    void TimerIDForDebitMoneyProc();
    //定时器超时处理--查询
    void TimerIDForCheckDebitProc();
    //资源销毁
    void ResourceDestroy(void);
    //日志检查定时器处理--删除
    void CheckAndDeleteLogFile();

    // quickpass support
    void slCorrectLastdealFromPOS(QString payId, QString contdata);

private:
    void handleDebitCancel(QString payId, QString contdata);


        //Attribute
public:
        //硬件控制类指针
        CThreadDevice*		m_pThreadDevice;
        //扣款状态查询
        CDVJsonDebitCheck*  m_pDVJsonDebitCheck;
        //扣款数据结构体
        CDVJsonDebit		m_sDVJsonDebit;
        //默认输入密码的超时时间
        quint64				m_dwInputPasswordDelayTime;




private:
        //消息回调函数
        CdvcallbackThread  m_callbackThread;
        //消息队列
        CThreadUpperMange  m_UpperMange;
        //支付子平台通讯线程类指针
        CThreadHttp*		m_pThreadHttp;

        UpayTMC*            m_pUpayThread;
        DeviceMgtThread*    m_deviceMgtThread;

        //用户信息是否准备就绪(数据被取走后，需要重置状态)
        bool				m_bAccountInfoIsReady;
        //硬件驱动加载情况
        bool				m_bDeviceLoadState;
        //定时器分辨率
        unsigned int		m_nAccuracy;

        //定时器ID(扫码超时)
        QTimer				m_nTimerIDForScanCode;
        //定时器ID(扣款超时)
        QTimer				m_nTimerIDForDebitMoney;
        //定时器ID(扣款状态查询)
        QTimer				m_nTimerIDForCheckDebitState;
        //扣款状态查询时间间隔
        unsigned int		m_dwCheckDebitSpanTime;
        //扣款状态查询时间总计
        quint32				m_dwCheckDebitAccumulateTime;
        //上次获取的用户数据
        tagAccountInfo		m_sAccountInfo;
        //JSON数据操作
        CJsonDebitOperate   m_pJsonIF;
        //JSON数据
        QString    m_strSendJsonData;

        //AES加密模块
        CAesECB *m_AES;

        //日志检查定时器
        QTimer *m_LogCheckDeleteTimer;

        QString m_lastPayId4Cancel;

        // debitCancel payId from buffer of DB
        QString m_debitCancelPayIdFromDB;
};

#endif // CTHREADMANAGE_H
