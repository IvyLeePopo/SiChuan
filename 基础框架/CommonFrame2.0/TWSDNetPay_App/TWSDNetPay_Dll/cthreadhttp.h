#ifndef CTHREADHTTP_H
#define CTHREADHTTP_H

#include <QObject>
#include <QThread>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkAccessManager>
#include "basedefine.h"
#include "cdvjsonbase.h"
#include "cjsonoperationif.h"
#include "cjsondebitoperate.h"
#include "cjsonheartoperate.h"
#include "cthreadbase.h"
#include "cjsonoperationif.h"
#include <QTimer>

#define QUERY_URL     "http://60.205.140.198:8081/pay"
#define CANCEL_URL    "http://60.205.140.198:8081/pay/reverse"
#define CHARG_URL     "http://60.205.140.198:8081/pay"
#define HEART_URL     "http://60.205.140.198:8081/heartbeat"

// quickpass support
#define SIGNIN_URL      "http://60.205.140.198:8081/signInTest"
#define CORRECT_URL     "http://60.205.140.198:8081/correct"

class CThreadHttp : public CThreadBase
{
    Q_OBJECT
public:
    static CThreadHttp* instance();
    explicit CThreadHttp(void);
    ~CThreadHttp(void);

    //参数结构体定义
    typedef struct _tagHttpParam
    {
        QByteArray	    SendData;			//发送的数据信息
        QString			URL;				//URL地址
        quint32     	Port;				//链接端口号
        int				CallMode;			//数据递交类型(POST 或 GET)
        quint64			ConsumeTime;		//线程处理消耗时间(毫秒)
        quint64			ExecuteCode;		//线程处理结果代码
        QString			ServerResponseData;	//服务端返回数据
        quint64			ServerResponseCode;	//服务端返回代码
        bool			NeedEndThread;		//是否强制终止线程

        quint64			SessionReceiveTimeout;	//接收数据超时
        quint64			SessionSendTimeout;		//发送数据超时
        quint64			SessionControlTimeout;	//控制超时
    }tagHttpParam;

    void Init(void);

    // dump configs
    void dumpSpecailConfig();

private:
    void reInitNetwork();

signals:
     void WM_THREAD_HTTP_ARRIVAL_DATA(QString,int);
     void WM_THREAD_HTTP_ARRIVAL_HEARTBEAT(QString);
     void WM_THREAD_HTTP_REPLY(QNetworkReply* reply, int op);

     // quickpass support
     void WM_THREAD_HTTP_QUICKPASS_DATA(QString data, int op);

public slots:
    void  onFinished(QNetworkReply *reply);
    void  onFinishedDeviceMgt(QNetworkReply *reply);

    //支付请求-查询
    void onMsgPostQueryData(QString* t_data);
    //支付请求-撤单
    void onMsgPostCancelData(QString* t_data, bool beCancel);
    //支付请求-扣费
    void onMsgPostChargingData(QString* t_data);
    //支付请求-二次扣费
    void onMsgSecondPostChargingData(QString t_data);
    //常规查询-心跳检测
    void onMsgGetHeartbeat(void);

    // for quickpass support
    void onMsgPostSignInData(QString t_data);

    //add for DeviceManagement
signals:
    void WM_THREAD_HTTP_DEVICE_MGT(QNetworkReply* reply);

public slots:
    void onMsgPost4DeviceMgt(QString path, QString json);
    void onMsgPostDebitTimeout();

protected:
    //清除本类申请的资源
    void fnReleaseResource();
    //读取本类相关的配置信息(从指定配置文件)
    void fnReadSpecailConfig();

private:
    //属性
    QNetworkRequest m_request;
    QNetworkReply   *m_reply;
    QNetworkReply   *m_heartBeatReply;
    QNetworkAccessManager* m_manager;
    //属性
    //入口发起的查询所使用的端口
    quint32	m_wQueryEntryPort;
    //出口发起的查询所使用的端口
    quint32	m_wQueryExitPort;
    //扣款指令所使用的端口
    quint32	m_wDebitPort;
    //扣款二次处理指令所使用的端口
    quint32	m_wDebitSecondPort;
    //扣款查询所用端口
    quint32	m_wDebitCheckPort;
    //扣款撤单所用端口
    quint32	m_wDebitCancelPort;
    //开环借口所用端口
    quint32	m_wOpenLoopPort;
    //闭环接口所用端口
    quint32	m_wCloseLoopPort;
    //开放站过车接口所用端口
    quint32	m_wPassStationPort;
    //流水记录推送接口所用的端口
    quint32	m_wSendRecordPort;
    //心跳查询接口所用的端口
    quint32	m_wHeartPort;

    // current server port
    quint32 m_serverPort;

    //入口查询接口地址
    QString	m_strQueryEntryURL;
    //出口查询接口地址
    QString	m_strQueryExitURL;
    //扣款请求接口地址
    QString	m_strDebitURL;
    //扣款二次处理接口地址
    QString     m_strDebitSecondURL;
    //扣款查询接口地址
    QString     m_strDebitCheckURL;
    //扣款撤单接口地址
    QString     m_strDebitCancelURL;
    //入口开环接口地址
    QString     m_strOpenLoopURL;
    //出口闭环接口地址
    QString     m_strCloseLoopURL;
    //开放过站接口地址
    QString     m_strPassStationURL;
    //心跳查询接口使用的URL
    QString	m_strHeartURL;

    //记录推送URL地址
    QString     m_strSendRecordURL;

    //quickpass support
    QString     m_strQuickpassSigninURL;       // signin url
    QString     m_strQuickpassCorrectURL;      // correct url for quickcpass

    //中心反射服务器检测失败累加值
    quint64	m_dwFailForCenterReflectWeb;
    //运营平台服务器检测失败累加值
    quint64	m_dwFailForOperationServer;

    //判定提交网络检测失败多少次报警
    quint64	m_dwFailHearWarningTimes;
    //心跳检测时间间隔最小阈值
    quint64	m_dwHeartCheckMin;
    //心跳检测时间间隔最大阈值
    quint64	m_dwHeartCheckMax;

    //是否正在重发数据
    bool        m_bResendWorking;
    //重发数据的时间间隔
    time_t      m_tResendDataSpanTime;
    //发送数据子线程超时时间限制
    quint64     m_dwWaitThreadWorkTimeout;
    //撤单本地缓存数据处理类
    //CTableDebitCancel	m_cTableDebitCancel;
    //当前业务操作
    int         m_CurrentOperation;
    //心跳定时器
    QTimer      *m_HearTimer;

private:
    //inform that the request is running, so need to abort
    bool m_httpRequestAborted;

    static CThreadHttp *self;

    bool m_isHeartbeatInProgress;
    bool m_isNetworkAccessible;
};

#endif // CTHREADHTTP_H
