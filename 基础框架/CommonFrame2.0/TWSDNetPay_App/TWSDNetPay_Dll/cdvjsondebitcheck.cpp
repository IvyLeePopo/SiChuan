#include "cdvjsondebitcheck.h"

CDVJsonDebitCheck::CDVJsonDebitCheck(void)
    :RecordMatchCode("")
    ,IDBindInPayPlatform("")
    ,IDBindInPayPlatformType(0)
    ,PayIdentifier("")
    ,DebitType(0)
    ,DebitMoney(0)
    ,CheckStateDelayTime(0)

    ,DebitStatusCode(0)
    ,DebitStatusDesc("")
    ,DebitOrderNumber("")
{

}

CDVJsonDebitCheck::~CDVJsonDebitCheck(void)
{
}


void CDVJsonDebitCheck::Reset( const int& iType)
{

}


CDVJsonDebitCheck::CDVJsonDebitCheck( const CDVJsonDebit* pDVJsonDebit)
{
    //基类数据复制
    Version					=	pDVJsonDebit->Version;
    PackageType				=	pDVJsonDebit->PackageType;
    ProvinceID				=	pDVJsonDebit->ProvinceID;
    Token					=	pDVJsonDebit->Token;
    SubmitCount				=	pDVJsonDebit->SubmitCount;
    SubmitTime				=	pDVJsonDebit->SubmitTime;
    QRCodeData				=	pDVJsonDebit->QRCodeData;
    QRCodeType				=	pDVJsonDebit->QRCodeType;
    ServerResponseCode		=	pDVJsonDebit->ServerResponseCode;
    ServerReceiveDataTime	=	pDVJsonDebit->ServerReceiveDataTime;
    ServerResponseDataTime	=	pDVJsonDebit->ServerResponseDataTime;
    //本地类数据复制
    RecordMatchCode			=	pDVJsonDebit->RecordMatchCode;
    IDBindInPayPlatform		=	pDVJsonDebit->IDBindInPayPlatform;
    IDBindInPayPlatformType =	pDVJsonDebit->IDBindInPayPlatformType;
    PayIdentifier			=   pDVJsonDebit->PayIdentifier;
    DebitType				=	pDVJsonDebit->DebitType;
    DebitMoney				=	pDVJsonDebit->DebitMoney;
    DebitTime				=	pDVJsonDebit->DebitTime;
    DebitStatusCode			=	pDVJsonDebit->DebitStatusCode;
    DebitStatusDesc			=	pDVJsonDebit->DebitStatusDesc;
    DebitOrderNumber		=	pDVJsonDebit->DebitOrderNumber;
}



CDVJsonDebitCheck::CDVJsonDebitCheck( CDVJsonDebitCheck* pDVJsonDebitCheck)
{
    //基类数据复制
    Version					=	pDVJsonDebitCheck->Version;
    PackageType				=	pDVJsonDebitCheck->PackageType;
    ProvinceID				=	pDVJsonDebitCheck->ProvinceID;
    Token					=	pDVJsonDebitCheck->Token;
    SubmitCount				=	pDVJsonDebitCheck->SubmitCount;
    SubmitTime				=	pDVJsonDebitCheck->SubmitTime;
    QRCodeData				=	pDVJsonDebitCheck->QRCodeData;
    QRCodeType				=	pDVJsonDebitCheck->QRCodeType;
    ServerResponseCode		=	pDVJsonDebitCheck->ServerResponseCode;
    ServerReceiveDataTime	=	pDVJsonDebitCheck->ServerReceiveDataTime;
    ServerResponseDataTime	=	pDVJsonDebitCheck->ServerResponseDataTime;
    //本地类数据复制
    RecordMatchCode			=	pDVJsonDebitCheck->RecordMatchCode;
    IDBindInPayPlatform		=	pDVJsonDebitCheck->IDBindInPayPlatform;
    IDBindInPayPlatformType =	pDVJsonDebitCheck->IDBindInPayPlatformType;
    PayIdentifier			=   pDVJsonDebitCheck->PayIdentifier;
    DebitType				=	pDVJsonDebitCheck->DebitType;
    DebitMoney				=	pDVJsonDebitCheck->DebitMoney;
    DebitTime				=	pDVJsonDebitCheck->DebitTime;
    DebitStatusCode			=	pDVJsonDebitCheck->DebitStatusCode;
    DebitStatusDesc			=	pDVJsonDebitCheck->DebitStatusDesc;
    DebitOrderNumber		=	pDVJsonDebitCheck->DebitOrderNumber;
}


CDVJsonDebitCheck& CDVJsonDebitCheck::operator=( const CDVJsonDebitCheck& cSrc)
{
    if(this != &cSrc)
    {
        //基类数据复制
        Version					=	cSrc.Version;
        PackageType				=	cSrc.PackageType;
        ProvinceID				=	cSrc.ProvinceID;
        Token					=	cSrc.Token;
        SubmitCount				=	cSrc.SubmitCount;
        SubmitTime				=	cSrc.SubmitTime;
        QRCodeData				=	cSrc.QRCodeData;
        QRCodeType				=	cSrc.QRCodeType;
        ServerDebitTime			=	cSrc.ServerDebitTime;
        ServerResponseCode		=	cSrc.ServerResponseCode;
        ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
        ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
        //本地类数据复制
        RecordMatchCode			=	cSrc.RecordMatchCode;
        IDBindInPayPlatform		=	cSrc.IDBindInPayPlatform;
        IDBindInPayPlatformType =	cSrc.IDBindInPayPlatformType;
        PayIdentifier			=   cSrc.PayIdentifier;
        DebitType				=	cSrc.DebitType;
        DebitMoney				=	cSrc.DebitMoney;
        DebitTime				=	cSrc.DebitTime;
        CheckStateDelayTime		=	cSrc.CheckStateDelayTime;
        DebitStatusCode			=	cSrc.DebitStatusCode;
        DebitStatusDesc			=	cSrc.DebitStatusDesc;
        DebitOrderNumber		=	cSrc.DebitOrderNumber;
    }
    return *this;
}







