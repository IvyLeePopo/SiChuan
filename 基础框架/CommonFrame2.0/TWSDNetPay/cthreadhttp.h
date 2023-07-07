#ifndef CTHREADHTTP_H
#define CTHREADHTTP_H

#include <QObject>
#include <QThread>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include "basedefine.h"
#include "cdvjsonbase.h"
#include "cjsonoperationif.h"
#include "cjsondebitoperate.h"
#include "cjsonheartoperate.h"
#include "cthreadbase.h"
#include "cjsonoperationif.h"
#include <QTimer>
#include <QTime>

//#define QUERY_URL     "http://59.60.61.44:8081/pay"
//#define CANCEL_URL    "http://59.60.61.44:8081/pay/reverse"
//#define CHARG_URL     "http://59.60.61.44:8081/pay"
//#define HEART_URL     "http://59.60.61.44:8081/heartbeat"

#define QUERY_URL     "http://60.205.140.198:8081/pay"
#define CANCEL_URL    "http://60.205.140.198:8081/pay/reverse"
#define CHARG_URL     "http://60.205.140.198:8081/pay"
#define HEART_URL     "http://60.205.140.198:8081/heartbeat"

//#define QUERY_URL     "http://192.168.101.21:80/pay"
//#define CANCEL_URL    "http://192.168.101.21:80/pay/reverse"
//#define CHARG_URL     "http://192.168.101.21:80/pay"
//#define HEART_URL     "http://192.168.101.21:80/heartbeat"



class CThreadHttp : public CThreadBase
{
    Q_OBJECT
public:
    explicit CThreadHttp(void);
    ~CThreadHttp(void);

    //参数结构体定义
    typedef struct _tagHttpParam
    {
        QByteArray	    SendData;			//发送的数据信息
        QString		    URL;                                //URL地址
        quint32     	    Port;                           //链接端口号
        int                       CallMode;			//数据递交类型(POST 或 GET)
        quint64		    ConsumeTime;		//线程处理消耗时间(毫秒)
        quint64		    ExecuteCode;		//线程处理结果代码
        QString		    ServerResponseData;	        //服务端返回数据
        quint64		    ServerResponseCode;	        //服务端返回代码
        bool                      NeedEndThread;		//是否强制终止线程

        quint64		    SessionReceiveTimeout;	//接收数据超时
        quint64		    SessionSendTimeout;		//发送数据超时
        quint64		    SessionControlTimeout;	//控制超时
    }tagHttpParam;


    void Init(void);

signals:
    void WM_THREAD_HTTP_ARRIVAL_DATA(QString,int);   //云端数据
    void WM_THREAD_HTTP_ARRIVAL_HEARTBEAT(QString);  //云端心跳


public slots:
    //支付业务数据---处理
    void  onFinished(QNetworkReply *reply);
    //平台心跳数据---处理
    void  onFinishedHear(QNetworkReply *reply);

    //支付请求-查询
    void onMsgPostQueryData(QString* t_data);
    //支付请求-撤单
    void onMsgPostCancelData(QString* t_data);
    //支付请求-扣费
    void onMsgPostChargingData(QString* t_data);
    //支付请求-二次扣费
    void onMsgSecondPostChargingData(QString t_data);
    //常规查询-心跳检测
    void onMsgGetHeartbeat(void);


protected:
    //清除本类申请的资源
    void fnReleaseResource();
    //读取本类相关的配置信息(从指定配置文件)
    void fnReadSpecailConfig();
    //获取发送数据类型描述
    const QString GetSendDataTypeDesc(const int& iDataType);
    //获取服务器回馈代码描述
    const QString GetServerResponseCodeDesc( const int& iResponseType);

private:
    //进行心跳检测
    void CheckHeartState();
    //数据递交 -- 线程工作函数
    quint32  SendData(tagHttpParam lpParam);
    //数据格式转换,从调用者传过来的DVJson格式转换成字符串形式
    const bool DataConvertFromJson2String(const quint32& wSendDataType, CDVJsonBase* pDVJsonReceive, QString& strURL, quint32& wPort, QString& strSendData, int& iCallMode, CJsonOperationIF** pJsonIF);
    //获取随机数，在给定的范围内
    quint32 GetRandNumber( const quint32& nMin, const quint32& nMax);


    //支付业务数据---处理
    QNetworkRequest m_request;
    QNetworkAccessManager* m_manager;
    //平台心跳数据---处理
    QNetworkRequest m_requestHear;
    QNetworkAccessManager* m_managerHear;

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
    QString	 m_strHeartURL;

    //记录推送URL地址
    QString     m_strSendRecordURL;

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


};

#endif // CTHREADHTTP_H
