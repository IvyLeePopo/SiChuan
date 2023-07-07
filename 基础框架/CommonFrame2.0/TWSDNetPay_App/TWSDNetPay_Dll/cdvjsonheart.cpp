#include "cdvjsonheart.h"

CDVJsonHeart::CDVJsonHeart(void)
    :StationID(0)
    ,StationName("")
    ,LaneID(0)
{

}


CDVJsonHeart::~CDVJsonHeart(void)
{
}


void CDVJsonHeart::Reset(const int& iType)
{

}


CDVJsonHeart& CDVJsonHeart::operator=( const CDVJsonHeart& cSrc)
{
    if(&cSrc != this)
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
        StationID	= cSrc.StationID;
        StationName	= cSrc.StationName;
        LaneID		= cSrc.LaneID;
        HeartTime	= cSrc.HeartTime;
    }
    return *this;
}
