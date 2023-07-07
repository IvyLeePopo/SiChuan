#ifndef THREAD_REMOTE_ACCESS_H
#define THREAD_REMOTE_ACCESS_H
//远程控制线程，此线程用于完成所有云端和终端的控制交互逻辑
/**
 * 交互控制过程大概可以分为
 * 1、云端和中间件，比如扣费
 * 2、中间件和终端，比如打开扫码头
 * 3、云端和终端，固件升级和自定义显示
 * 此单元完成所有的第3种交互
 */

#include "ThreadBase.h"
#include <afxinet.h>
#include <process.h>


class ThreadRemoteAccess : public CThreadBase
{
    DECLARE_DYNCREATE(ThreadRemoteAccess)
protected:
    ThreadRemoteAccess();           // protected constructor used by dynamic creation
    virtual ~ThreadRemoteAccess();
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual BOOL OnIdle(LONG lCount);
public:
    void waitTillIdle();
private:
    CRITICAL_SECTION idleMutex;
protected:

    DECLARE_MESSAGE_MAP()

protected:



};



#endif // THREAD_REMOTE_ACCESS_H
