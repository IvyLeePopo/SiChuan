#include "PaySystem.h"
#include <stdio.h>
#include<stdlib.h>

PaySystem *PaySystem::self=NULL;
PaySystem *PaySystem::instance()
{
    if(self==NULL)
        self =  new PaySystem();
    return self;
}

PaySystem::~PaySystem()
{
    unRegister2Bus(KEY_BUSINESS);
    printf("unload PaySystem successful!.\n");
    if(self!=NULL)
    {
        printf("PaySystem is running!.\n");
    }
    self = NULL;
}

void PaySystem::recvFromBus(int destKey,char *msg,int len, int srcKey)
{
    printf("qwe.\n");
}

PaySystem::PaySystem()
{
    register2Bus(KEY_BUSINESS);
    m_CommandManagement.CommandManagementInit();
}

CommonBus *getInstance()
{
    return PaySystem::instance();
}

CommonBus *deleteInstance()
{
    delete PaySystem::instance();
}
