#include "cdvjsonparamdebitcancel.h"

CDVJsonParamDebitCancel::CDVJsonParamDebitCancel(void)
     :PayIdentifier("")
{
    m_type = 1;
}

CDVJsonParamDebitCancel::~CDVJsonParamDebitCancel(void)
{
}


CDVJsonParamDebitCancel& CDVJsonParamDebitCancel::operator=( const CDVJsonParamDebitCancel& cSrc)
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
        m_type                  =   cSrc.m_type;
    }
    return *this;
}










