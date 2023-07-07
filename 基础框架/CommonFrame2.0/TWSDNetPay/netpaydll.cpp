#include "netpaydll.h"

#include "twsdnetpay.h"

#include <QDebug>

TWSDNetPay_Dll*  TWSDNetPay_Dll::self  = NULL;

TWSDNetPay_Dll::TWSDNetPay_Dll()
{
    commpentsKey = KEY_BUSINESS;
    printf("[##TWSDNetPay_Dll::TWSDNetPay_Dll()##]->register2Bus(KEY_BUSINESS).\n");
    register2Bus(KEY_BUSINESS);
}

TWSDNetPay_Dll::~TWSDNetPay_Dll()
{
    unRegister2Bus(KEY_BUSINESS);
    printf("[##TWSDNetPay_Dll::~TWSDNetPay_Dll()##]->unload TWSDNetPay_Dll successful!.\n");
    if(self!=NULL)
    {
        printf("[##TWSDNetPay_Dll::~TWSDNetPay_Dll()##]->TWSDNetPay_Dll  go downed!.\n");
    }
    self = NULL;
}

TWSDNetPay_Dll * TWSDNetPay_Dll::instance()
{
    if(NULL == self)
    {
        self = new TWSDNetPay_Dll();
    }

    printf("[##TWSDNetPay_Dll::instance()##]->self = new TWSDNetPay_Dll()\n");
    return self;
}

CommonBus *getInstance()
{
    return TWSDNetPay_Dll::instance();
}

CommonBus *deleteInstance()
{
    delete TWSDNetPay_Dll::self;
}



bool NetPay_IF_InitEnvironment(const char *tty, int baudRate, const char *serverIP, int serverPort,
                          bool isOpenServerPort, bool logFlag, int logSaveTime, const char *szAreaInfo,
                          const char *szLoaclStation, const char *szLoaclLaneID, const char *szServerInfo,
                          const int &iProvinceID, void (*fun)(int, int))
{
    return Inner_IF_InitEnvironment( tty,  baudRate,  serverIP,  serverPort,  isOpenServerPort,  logFlag,  logSaveTime,  szAreaInfo, szLoaclStation,  szLoaclLaneID, szServerInfo,  iProvinceID, fun);
}

bool NetPay_IF_Destroy()
{
    return Inner_IF_GetLastErrorDesc();
}

const char * NetPay_IF_GetLastErrorDesc()
{
    return Inner_IF_GetLastErrorDesc();
}

bool NetPay_IF_StartGetAccountInfo(const int &iOvertime)
{
    return Inner_IF_StartGetAccountInfo(iOvertime);
}

bool NetPay_IF_StopGetAccountInfo()
{
    return Inner_IF_StopGetAccountInfo();
}

bool NetPay_IF_GetAccountInfo(char *szParamContext, int &iParamSize, const int &iFormatType)
{
    return Inner_IF_GetAccountInfo(szParamContext, iParamSize, iFormatType);
}

bool NetPay_IF_DebitMoney(const char *szParamContext, const int &iParamSize, const int &iFormatType)
{
    return Inner_IF_DebitMoney( szParamContext,  iParamSize, iFormatType);
}

bool NetPay_IF_DebitCancel(const char *szParamContext, const int &iParamSize, const int &iFormatType)
{
    return Inner_IF_DebitCancel( szParamContext, iParamSize, iFormatType);
}

bool NetPay_IF_GetDebitResult(char *szParamContext, int &iParamSize, const int &iFormatType)
{
    return Inner_IF_GetDebitResult(szParamContext, iParamSize, iFormatType);
}

bool NetPay_IF_SetMMI(const char *szParamContext, const int &iParamSize, const int &iFormatType)
{
    return Inner_IF_SetMMI( szParamContext,  iParamSize, iFormatType);
}

bool NetPay_IF_GetParam(const char *szParamContext, const int &iParamSize, const int &iFormatType, char *szResultContext, int &iResultSize)
{
    return Inner_IF_GetParam(  szParamContext,  iParamSize,  iFormatType, szResultContext, iResultSize);
}

bool NetPay_IF_GetComponentStatus(unsigned int &uStatus)
{
    return Inner_IF_GetComponentStatus(uStatus);
}

bool NetPay_IF_TranslateData(const char *szParamContext, const int &iParamSize, const int &iFormatType)
{
    return Inner_IF_TranslateData(szParamContext, iParamSize, iFormatType);
}


//重载
int TWSDNetPay_Dll::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
    if(srcKey == KEY_COMMON_FRAME)
    {
        printf("[##TWSDNetPay_Dll::recvFromBus##] srcKey = %d\n",srcKey);
        //如果当前没有进行业务，则返回1
        return 1;
    }

    return 0;
}

