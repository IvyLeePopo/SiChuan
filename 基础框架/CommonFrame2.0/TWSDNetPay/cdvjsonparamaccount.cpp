#include "cdvjsonparamaccount.h"

CDVJsonParamAccount::CDVJsonParamAccount(void)
    :DataType(0)
    ,DataSrc(0)
    ,DataContext("")
{
    Version = "1.0";
}


CDVJsonParamAccount::~CDVJsonParamAccount(void)
{
}

CDVJsonParamAccount& CDVJsonParamAccount::operator=( const CDVJsonParamAccount& cSrc)
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
        DataType		=	cSrc.DataType;
        DataSrc			=	cSrc.DataSrc;
        DataContext		=	cSrc.DataContext;
    }
    return *this;
}

