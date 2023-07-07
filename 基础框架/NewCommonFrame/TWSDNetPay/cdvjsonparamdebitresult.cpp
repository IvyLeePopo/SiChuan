#include "cdvjsonparamdebitresult.h"

CDVJsonParamDebitResult::CDVJsonParamDebitResult(void)
    :ResultType(0)
    ,ExecuteCode(-1)
    ,ExecuteDesc("")

    ,PayIdentifier("")
    ,PayPlatformType(0)
    ,TradeSecurityCode("")
    ,DebitOrder("")
{

    Version = "1.0";
}


CDVJsonParamDebitResult::~CDVJsonParamDebitResult(void)
{
}


CDVJsonParamDebitResult& CDVJsonParamDebitResult::operator=( const CDVJsonParamDebitResult& cSrc)
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
        ResultType				=	cSrc.ResultType;
        ExecuteCode				=	cSrc.ExecuteCode;
        ExecuteDesc				=	cSrc.ExecuteDesc;

        PayIdentifier			=	cSrc.PayIdentifier;
        PayPlatformType			=	cSrc.PayPlatformType;
        TradeSecurityCode		=	cSrc.TradeSecurityCode;
        DebitOrder				=	cSrc.DebitOrder;
    }
    return *this;
}
