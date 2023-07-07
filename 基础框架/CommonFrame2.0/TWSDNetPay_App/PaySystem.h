#ifndef PAYSYSTEM_H
#define PAYSYSTEM_H

#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    #if defined(PAYSYSTEM_LIBRARY)
    #  define PAYSYSTEMSHARED_EXPORT __declspec(dllexport)
    #else
    #  define PAYSYSTEMSHARED_EXPORT __declspec(dllimport)
    #endif
#else
    #  define PAYSYSTEMSHARED_EXPORT
#endif

#include <CommonBus.h>
#include "AgreementShell/commandmanagement.h"

#include <CommonBus.h>
class PaySystem:public CommonBus
{
public:
    static PaySystem* instance();
    static PaySystem*self;
    virtual ~PaySystem();
    void recvFromBus(int destKey,char *msg,int len, int srcKey=0);

public:
    PaySystem();

    CommandManagement m_CommandManagement;

};
extern "C" PAYSYSTEMSHARED_EXPORT CommonBus* getInstance(); //获取类TestDll的对象
extern "C" PAYSYSTEMSHARED_EXPORT CommonBus* deleteInstance(); //获取类TestDll的对象
#endif // PAYSYSTEM_H
