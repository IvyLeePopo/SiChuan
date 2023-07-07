/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: ThreadHelper.h
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-08
* 修改记录:
*/

#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

#include <Windows.h>   //import CRITICAL_SECTION windows7 or early
#ifndef _WIN32_WINNT
//#error 需要定义操作系统版本变量，以便确定API调用
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
