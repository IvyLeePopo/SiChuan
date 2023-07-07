#ifndef CTHREADMANAGE_H
#define CTHREADMANAGE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QDateTime>

#include<map>
#include "basedefine.h"
#include "cthreadbase.h"
#include "chtsddevicedll.h"
#include "cthreaddevice.h"
#include "basedefine.h"
#include "cjsonoperationif.h"
#include "cdvjsonbase.h"
#include "cthreaduppermange.h"
#include "cjsondebitoperate.h"

using namespace std;


class CThreadManage : public CThreadBase
{
    Q_OBJECT
public:
    explicit CThreadManage(void);
    ~CThreadManage();
    //启动工作线程
//   const  bool StartWorkThreads( const tagTranstParam& sTranstParam);
    //判断用户数据是否已经就绪（硬件提供）
    const bool GetAccountDataReadyState(void)   { return m_bAccountInfoIsReady; }
    //设置用户数据是否准备就绪
    void SetAccountDataReadyState( const bool& bState);
    //获取上次检索到的用户数据
    const tagAccountInfo GetLastAccountInfo(void) {return m_sAccountInfo;}
    //设置定时器
    void StartTimer( const quint32& nEventID, const quint64& dwDelayTime);
    //停止定时器
    void StopTimer(const quint32& nEventID);
    //获取硬件驱动加载状况  //(第二个const修饰函数)
    const bool GetDeviceLoadStae(void) const  {return m_bDeviceLoadState;}
    //资源销毁
    void ResourceDestroy(void);

    /*add zhuyabing 2017-06*/
    bool SendBreakDebit(const QString& payID);


protected:
    void run();
private:    
        //交易结束的处理
        QString FinishTrade( const bool& bNormal);

    //构造发送到支付子平台的数据
   bool ContructDebitMoneyInfo( int NS,const tagAccountInfo* pAccountInfo);
    //构造发送到支付子平台的数据
    bool ContructDebitMoneyInfo(void);
    //构造发送到支付子平台的数据  撤单
    bool ContructDebitCancelInfo(const QString& strPayIdentifier);


    //JUST FOR EMIT THE SIGNAL
public:
    //display info
    void  SEND_WM_THREAD_MANAGE_SHOW_DEAL_INFO(QString  wParam , int  lParam);
    //debit money
    void  SEND_WM_THREAD_MANAGE_START_DEBIT_FLOW(QString   wParam, int lParam);
    //debit cancel
    void  SEND_WM_THREAD_MANAGE_DEBIT_CANCEL(QString  wParam, int lParam);

signals:
    //display info
    void  WM_THREAD_MANAGE_SHOW_DEAL_INFO(QString  , int);
    //debit money
    void  WM_THREAD_MANAGE_START_DEBIT_FLOW(QString , int);
    //debit cancel
    void  WM_THREAD_MANAGE_DEBIT_CANCEL(QString  , int);

public slots:
    //执行扣款操作(整合模式)
    void OnMsgManageStartDebitFlow(QString  wParam, int lParam);
    //执行撤单操作
    void OnMsgDebitCancel(QString  wParam, int lParam);
    //执行显示操作
    void OnMsgShowInfo(QString  wParam, int lParam);

    ///////////////////定时器响应函数///////////////////////////////
    //定时器超时处理--扣费
    void TimerIDForDebitMoneyProc();

    //空闲处理
    void TimerIDforDeviceState();
    void TimerIDForThreadActive();
    void TimerIDForDeleteLogFile();
    /////////////////定时器响应函数///////////////////////////////////

    //日志检查定时器处理--删除
    void CheckAndDeleteLogFile();

    //Attribute
public:
    //硬件控制类指针
    CThreadDevice*		m_pThreadDevice;
    //扣款数据结构体
    CDVJsonDebit		m_sDVJsonDebit;
    //默认输入密码的超时时间
//    quint64			m_dwInputPasswordDelayTime;


private:

    //用户信息是否准备就绪(数据被取走后，需要重置状态)
    bool				m_bAccountInfoIsReady;
    //硬件驱动加载情况
    bool				m_bDeviceLoadState;
    //定时器ID(扣款超时)
    QTimer				m_nTimerIDForDebitMoney;
    //定时器ID(撤单超时)
     QTimer				m_nTimerIDForDebitCancel;

     //日志检查定时器
     QTimer      m_LogCheckDeleteTimer;
     QTimer      m_nTimerIDForDeviceState;
     QTimer      m_nTimerIDForThreadActive;
     QTimer      m_nTimerIDForDeleteLogFile;

    //JSON数据操作
    CJsonDebitOperate               m_pJsonIF;

    tagAccountInfo  m_sAccountInfo;



public:

   tagAccountInfo lastAccountInfo;  //保存的上一次的扫码数据



public:
    bool Init_Dev();
    bool Destroy_Dev();

};

#endif // CTHREADMANAGE_H
