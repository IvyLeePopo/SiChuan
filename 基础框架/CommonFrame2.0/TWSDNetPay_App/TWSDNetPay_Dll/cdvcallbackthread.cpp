#include "cdvcallbackthread.h"
#include "ctwsdnetpay_dllapp.h"

CdvcallbackThread::CdvcallbackThread(void)
{
    Isavailable = false;
}

void CdvcallbackThread::setResults(int t_option, int t_result)
{
    // lock
    m_mutex.lock();

    m_option = t_option;
    m_result = t_result;
    Isavailable = true;

    // release lock
    m_mutex.unlock();
}

void CdvcallbackThread::run()
{
    while (true)
    {
        // lock
        m_mutex.lock();
        if (Isavailable)
        {
            int option = m_option;
            int result = m_result;
            Isavailable = false;

            m_mutex.unlock();

            // inform the results
            theApp.m_Fun_ChargeResults(option, result);
        }
        else
            m_mutex.unlock();

//        msleep(300);
        msleep(100);
    }
}
