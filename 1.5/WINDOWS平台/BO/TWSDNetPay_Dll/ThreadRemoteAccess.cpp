/*
* @Author: ÖìÑÇ±ù
* @Date:   2017-12-08
* @Last Modified by:   windf
* @Last Modified time: 2017-12-08
*/
#include "ThreadRemoteAccess.h"

ThreadRemoteAccess::ThreadRemoteAccess()
{
    ::InitializeCriticalSection(&idleMutex);
}
ThreadRemoteAccess::~ThreadRemoteAccess()
{

}
BOOL ThreadRemoteAccess::InitInstance()
{
    return TRUE;
}
int ThreadRemoteAccess::ExitInstance()
{
    ::DeleteCriticalSection(&idleMutex);
    return CWinThread::ExitInstance();
}
BOOL ThreadRemoteAccess::OnIdle(LONG lCount)
{
    CScopeLock tmpIdle(&idleMutex);
    if (m_bExit)
    {
        ::PostQuitMessage(0);
        return FALSE;
    }
    Sleep(100);
    return true;
}

void ThreadRemoteAccess::waitTillIdle()
{

    if (this->m_bExit)
    {
        return ;
    }
    CScopeLock tmpIdle(&idleMutex);
    this->m_bExit = true;

    return ;
}