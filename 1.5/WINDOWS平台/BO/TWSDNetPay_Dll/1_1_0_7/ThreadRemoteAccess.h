#ifndef THREAD_REMOTE_ACCESS_H
#define THREAD_REMOTE_ACCESS_H
//Զ�̿����̣߳����߳�������������ƶ˺��ն˵Ŀ��ƽ����߼�
/**
 * �������ƹ��̴�ſ��Է�Ϊ
 * 1���ƶ˺��м��������۷�
 * 2���м�����նˣ������ɨ��ͷ
 * 3���ƶ˺��նˣ��̼��������Զ�����ʾ
 * �˵�Ԫ������еĵ�3�ֽ���
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
