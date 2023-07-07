#include "CommonBus.h"
#include "CommonBusSource.h"
#include <stdio.h>

CommonBus::CommonBus()
{

}

CommonBus::~CommonBus()
{

}


void CommonBus::register2Bus(int key)
{
    CommonMap::iterator myMapIt = CommonBusSource::myMap.find(key);
    if(myMapIt!=CommonBusSource::myMap.end())
    {
        printf("[##CommonBus->register2Bus##]:this key=%d is exsited,can't add!\n",key);
        return;
    }
    else
    {
        printf("[##CommonBus->register2Bus##]:This key=%d register successful!\n",key);
        CommonBusSource::myMap.insert(PairCommonMap(key,this));
    }
}

void CommonBus::unRegister2Bus(int key)
{
    CommonMap::iterator myMapIt = CommonBusSource::myMap.find(key);
    if(myMapIt!=CommonBusSource::myMap.end())
    {
        CommonBusSource::myMap.erase(myMapIt);
        printf("[##CommonBus->unRegister2Bus##]: Have removed key %d\n",key);
    }
}

int CommonBus::send2Bus(int destKey, char *msg, int len,int srcKey)
{
    CommonMap::iterator myMapIt = CommonBusSource::myMap.find(destKey);
    if(myMapIt!=CommonBusSource::myMap.end())
    {
        CommonBus *baseServer = myMapIt->second;
        return baseServer->recvFromBus(destKey,msg,len,srcKey);
    }
    else
    {
        printf("[##CommonBus->send2Bus##]: This key %d had unload !\n",destKey);
        return -1;
    }
}

int CommonBus::recvFromBus(int destKey, char *msg, int len,int srcKey)
{

}
