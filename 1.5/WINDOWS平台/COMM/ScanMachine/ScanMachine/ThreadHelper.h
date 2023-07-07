/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: ThreadHelper.h
* ժ   Ҫ:
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-08
* �޸ļ�¼:
*/

#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

#include <Windows.h>   //import CRITICAL_SECTION windows7 or early
#ifndef _WIN32_WINNT
//#error ��Ҫ�������ϵͳ�汾�������Ա�ȷ��API����
#elif (_WIN32_WINNT >= 0x06020000)
#include <Synchapi.h>  //import CRITICAL_SECTION Windows8
#endif 


class CScopeLock
{
public:
    CScopeLock(CRITICAL_SECTION* lck)
        : mMutex(lck)
    {
		if(mMutex!=NULL)
		{
			::EnterCriticalSection(mMutex);
		}
        
    }
    ~CScopeLock()
    {
		if(mMutex!=NULL)
		{
			 ::LeaveCriticalSection(mMutex);
		}
       
    }
private:
    CRITICAL_SECTION* mMutex;
private:
    CScopeLock(const CScopeLock&);
    CScopeLock& operator = ( const CScopeLock&);

};



#endif // THREAD_HELPER_H
