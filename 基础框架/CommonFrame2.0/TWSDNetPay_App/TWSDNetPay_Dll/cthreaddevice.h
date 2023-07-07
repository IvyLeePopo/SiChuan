#ifndef CTHREADDEVICE_H
#define CTHREADDEVICE_H

#include <QObject>
#include "cthreadbase.h"
#include "chtsddevicedll.h"
#include <QDebug>
#include "HWPerformance/hwperformance.h"

class CThreadDevice : public CThreadBase
{
    Q_OBJECT
public:
    explicit CThreadDevice(void);
    virtual ~CThreadDevice();

    bool Init(void);

    //Operations
  //  inline const bool GetDeviceInitState(void){if(NULL != m_pHTSDDeviceDll) return m_pHTSDDeviceDll->GetDevInitState(); return false;}
    //获取硬件设备状态
    quint32 GetDeviceState(void);

signals:
    void WM_THREAD_DEVICE_MS_GETDATA_NOTIFY(int,tagAccountInfo*);


public slots:
    //执行启动获取数据的操作
    void OnMsgStartGetData(int wParam);
    //执行停止获取数据的操作
    void OnMsgStopGetData(void);
    //接收到硬件推送的消息，进行数据获取
    void OnMsgGetData(int lParam);
    //接收到信息，要求执行费显功能
    void OnMsgShowDealInfo(QString wParam);
    //接收到消息，启动扣款流程
    void OnMsgStartDebitFlow(const QString &wParam);
    //接收到消息，要求展示缴费结果交互界面
    void OnMsgControlResultMMI(int wParam, QString lParam);
    //资源销毁
    void ResourceDestroy(void);


private:
    //底层通信消息队列通信
//    CDevicemessage  *m_UnderDeviceMssage;
    //智能硬件版本(1,初始版本;2,1.5版本)
    int				m_iDeviceVersion;
    //硬件控制接口类
//  CHTSDDeviceDll*	m_pHTSDDeviceDll;

    HWPerformance*	m_pHWPerformance;
    //硬件上次检测的状态
    quint32			m_dwLastCheckState;
};

#endif // CTHREADDEVICE_H
