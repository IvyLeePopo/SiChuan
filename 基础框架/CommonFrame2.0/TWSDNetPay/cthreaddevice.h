#ifndef CTHREADDEVICE_H
#define CTHREADDEVICE_H

#include <QObject>
#include "cthreadbase.h"
#include "chtsddevicedll.h"
#include <QDebug>
#include<QTimer>
#include "cdevicemessage.h"


class CThreadDevice : public CThreadBase
{
    Q_OBJECT
public:
    explicit CThreadDevice(void);
    virtual ~CThreadDevice();

protected:
    void run();

public:
    bool Init();

    //获取硬件初始化状态
    inline  bool GetDeviceInitState(void)  {  if(NULL != m_pHTSDDeviceDll)  return m_pHTSDDeviceDll->GetDevInitState();   return false;  }
    //获取硬件设备状态
    quint32 GetDeviceState(void);
    /*add zhuyabing 201706*/
    bool BreakDebit(const QString& payID);

    void StartTimer(const quint32& nEventID, const quint64& dwDelayTime);
    void StopTimer(const quint32& nEventID);

    //仅仅为了发射信号
    void JUSTFORSENDCALLBACK(int nType, QString strData);

    //给外部调用，发射信号
        //display info
        void  SEND_WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString  wParam , int  lParam);
        //debit money
        void  SEND_WM_THREAD_DEVICE_START_DEBIT_FLOW(QString   wParam, int lParam);
        //debit cancel
        void  SEND_WM_THREAD_DEVICE_DEBIT_CANCEL(QString  wParam, int lParam);


signals:
    void  SENDCALLBACKRESULT(int, QString);


    //给硬件线程发通知
  void WM_THREAD_DEVICE_START_DEBIT_FLOW(QString  ,  int);//扣款
   void WM_THREAD_DEVICE_DEBIT_CANCEL(QString  ,  int);//撤单
   void WM_THREAD_DEVICE_SHOW_DEAL_INFO(QString ,  int);//费显，清屏


public slots:
    //接收到信息，要求执行费显功能
    void OnMsgShowDealInfo(QString  wParam, int lParam);
    //接收到消息，启动扣款流程
    void OnMsgStartDebitFlow(QString wParam, int lParam);
    //接收到消息，启动撤单流程
    void OnMsgDebitCancelFlow(QString  wParam, int lParam);

    //发送初始化指令
    void OnTimerInitConfigDevice();
    //线程存活检测
     void OnTimerActiveThread();
     //组件加载及初始化
     void OnTimerInitDevice();

     //方便处理回调结果
     void  OnMsgCallbackResult(int nType, QString dataResult);

public:
    //资源销毁
    void ResourceDestroy(void);

    bool Init_Dev();
    bool Destroy_Dev();
private:
    //往硬件下发站点配置信息
    bool InitConfigDevice(void);
    //获取通讯模块主动通知函数
    static void fnGetNotifyFromCommModule(int iType,const QByteArray& strData);

    //fill debit result
    bool TmpFillDebitResult( QString& ptr);

    //回调扣款结果，从数据块转成json数据
    QString custructDebitResult2Json(int iType,const QByteArray& strData);

private:

    //硬件控制接口类
    CHTSDDeviceDll*  m_pHTSDDeviceDll;
    //是否下发初始化指令
    bool			m_bInitConfigDevice;
    //初始化成功标识
    bool                    m_bInitDebviceSuccessed;

    QTimer m_timerID_InitConfigDevice;
    QTimer m_timerID_ActiveThread;
    QTimer m_timerID_InitDevice;

    //为了在回调中可以使用类中成员
    static CThreadDevice *pThis;

};

#endif // CTHREADDEVICE_H
