#include "cdvjsondebitcancel.h"

CDVJsonDebitCancel::CDVJsonDebitCancel(void)
    :PayIdentifier("")
    ,DebitType(0)
    ,DebitMoney(0)
    ,RequestingPartyType(1)
    ,DebitOrderNumber("")
{

}


CDVJsonDebitCancel::CDVJsonDebitCancel(const CDVJsonDebit* pDVJsonDebit)
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
    PayIdentifier			=	pDVJsonDebit->PayIdentifier;
    DebitType				=	pDVJsonDebit->DebitType;
    DebitMoney				=	pDVJsonDebit->DebitMoney;
    DebitTime				=	pDVJsonDebit->DebitTime;
    DebitOrderNumber		=	pDVJsonDebit->DebitOrderNumber;
    RequestingPartyType		=	0x01;

    m_payPlatformType       =   0x0;
    m_contdata.clear();
}

CDVJsonDebitCancel::CDVJsonDebitCancel( const CDVJsonDebitCheck* pDVJsonDebitCheck)
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
    PayIdentifier			=	pDVJsonDebitCheck->PayIdentifier;
    DebitType				=	pDVJsonDebitCheck->DebitType;
    DebitMoney				=	pDVJsonDebitCheck->DebitMoney;
    DebitTime				=	pDVJsonDebitCheck->DebitTime;
    DebitOrderNumber		=	pDVJsonDebitCheck->DebitOrderNumber;
    RequestingPartyType		=	0x01;
}

CDVJsonDebitCancel::~CDVJsonDebitCancel(void)
{
}



CDVJsonDebitCancel& CDVJsonDebitCancel::operator=( const CDVJsonDebitCancel& cSrc)
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
        ServerResponseCode		=	cSrc.ServerResponseCode;
        ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
        ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
        //本地类数据复制
        PayIdentifier			=	cSrc.PayIdentifier;
        DebitType				=	cSrc.DebitType;
        DebitMoney				=	cSrc.DebitMoney;
        DebitTime				=	cSrc.DebitTime;
        DebitOrderNumber		=	cSrc.DebitOrderNumber;
        RequestingPartyType		=	cSrc.RequestingPartyType;
    }
    return *this;
}















