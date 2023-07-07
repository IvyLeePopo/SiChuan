#include "cdvjsondebit.h"

CDVJsonDebit::CDVJsonDebit()
    :TestingData(false)
    ,NetPayDllVersion((""))
    ,DeviceDllVersion((""))

    ,IDBindInPayPlatform((""))
    ,IDBindInPayPlatformType(0)
    ,RecordMatchCode((""))
    ,TradeSecurityCode((""))
    ,QRCodeData((""))
    ,DebitType(0)
    ,DebitMoney(0)
    ,SMVirtualCardID((""))
    ,PayVirtualCardID((""))
    ,DebitTollDate(0)
    ,DebitTollShift(0)
    ,AccountInfoSrcType(0)
    ,AccountInfoDeviceID(0)
    ,AccountInfoDataType(0)

    ,EntryProvinceID(0)
    ,EntryStationID(0)
    ,EntryStationName((""))
    ,EntryLane(0)
    ,EntryInfoUniqueFlag((""))
    ,EntryRoadID(0)
    ,EntryAreaID(0)

    ,PayProvinceID(0)
    ,PayStationID(0)
    ,PayStationName((""))
    ,PayLane(0)
    ,PayRoadID(0)

    ,VehicleClass(0)
    ,VehicleType(0)
    ,VehicleLicense((""))
    ,VehicleColor(0)
    ,AxisCount(0)
    ,TotalWeight(0)
    ,LimitWeight(0)

    ,ServerRealDebitMoney(0)
    ,ServerJudgeDebitIDType(0)
    ,DebitStatusCode(0)
    ,DebitStatusDesc((""))
    ,DebitOrderNumber("")
    ,sn(0)
{

}


CDVJsonDebit::~CDVJsonDebit()
{
}



/******************************************************
 * 函数名称：Reset
 * 功能描述：重置数据
 * 参   数：iType == 0 为重设
 * 返   回：void
*******************************************************/
void CDVJsonDebit::Reset(const int& iType)
{
    switch(iType)
    {
    case 0:
    {
        TestingData				= false;
        NetPayDllVersion		= "";
        DeviceDllVersion		= "";
        IDBindInPayPlatform		= "";
        IDBindInPayPlatformType = 0;
        RecordMatchCode		= "";
        TradeSecurityCode	= "";
        QRCodeData			= "";
        DebitType			= 0;
        DebitMoney			= 0;
        SMVirtualCardID		= "";
        PayVirtualCardID	= "";
        PayIdentifier		= "";
        //  DebitTime			= CTime(0);
        DebitTime			= "";
        DebitTollDate		= 0;
        DebitTollShift		= 0;
        AccountInfoSrcType	= 0;
        AccountInfoDeviceID	= 0;
        AccountInfoDataType	= 0;

        EntryProvinceID		= 0;
        EntryStationID		= 0;
        EntryStationName	= "";
        EntryLane			= 0;
        EntryInfoUniqueFlag	= "";
        EntryRoadID			= 0;
        EntryAreaID			= 0;

        PayProvinceID		= 0;
        PayStationID		= 0;
        PayStationName		= "";
        PayLane				= 0;
        PayRoadID			= 0;

        VehicleClass		= 0;
        VehicleType			= 0;
        VehicleLicense		= "";
        VehicleColor		= 0;
        AxisCount			= 0;
        TotalWeight			= 0;
        LimitWeight			= 0;

        DebitStatusCode		= 0;
        DebitStatusDesc		= "";
        DebitOrderNumber	= "";

        sn       = 0;
    }
        break;
    }
}
/*************************************************
 *函数名称：CDVJsonDebit::operator=
 *功能描述：重载运算符，使之俩个对象可以互相赋值
 *参   数：cSrc
 *返   回：this
**************************************************/
CDVJsonDebit& CDVJsonDebit::operator=( const CDVJsonDebit& cSrc)
{
    if(this != &cSrc)
    {
        TestingData				=	cSrc.TestingData;
        NetPayDllVersion		=	cSrc.NetPayDllVersion;
        DeviceDllVersion		=	cSrc.DeviceDllVersion;
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
        ServerReceiveDataTime       	=	cSrc.ServerReceiveDataTime;
        ServerResponseDataTime          =	cSrc.ServerResponseDataTime;
        //本地类数据复制
        IDBindInPayPlatform		=	cSrc.IDBindInPayPlatform;
        IDBindInPayPlatformType	=	cSrc.IDBindInPayPlatformType;
        RecordMatchCode			=	cSrc.RecordMatchCode;
        QRCodeData				=	cSrc.QRCodeData;
        DebitType                                   =	cSrc.DebitType;
        DebitMoney				=	cSrc.DebitMoney;
        SMVirtualCardID			=	cSrc.SMVirtualCardID;
        PayVirtualCardID                    =	cSrc.PayVirtualCardID;
        PayIdentifier                           =	cSrc.PayIdentifier;
        DebitTime				=	cSrc.DebitTime;
        DebitTollDate                           =	cSrc.DebitTollDate;
        DebitTollShift                          =	cSrc.DebitTollShift;
        AccountInfoSrcType                  =	cSrc.AccountInfoSrcType;
        AccountInfoDeviceID		=	cSrc.AccountInfoDeviceID;
        AccountInfoDataType		=	cSrc.AccountInfoDataType;

        EntryProvinceID			=	cSrc.EntryProvinceID;
        EntryStationID                  	=	cSrc.EntryStationID;
        EntryStationName                    =	cSrc.EntryStationName;
        EntryLane                                   =	cSrc.EntryLane;
        EntryInfoUniqueFlag		=	cSrc.EntryInfoUniqueFlag;
        EntryRoadID				=	cSrc.EntryRoadID;
        EntryAreaID				=	cSrc.EntryAreaID;
        PayProvinceID                           =	cSrc.PayProvinceID;
        PayStationID                            =	cSrc.PayStationID;
        PayStationName			=	cSrc.PayStationName;
        PayLane					=	cSrc.PayLane;
        PayRoadID				=	cSrc.PayRoadID;
        VehicleClass                            =	cSrc.VehicleClass;
        VehicleType				=	cSrc.VehicleType;
        VehicleLicense                       =	cSrc.VehicleLicense;
        VehicleColor                            =	cSrc.VehicleColor;
        AxisCount				=	cSrc.AxisCount;
        TotalWeight                          =	cSrc.TotalWeight;
        LimitWeight                           =   cSrc.LimitWeight;

        ServerRealDebitMoney           =	cSrc.ServerRealDebitMoney;
        ServerJudgeDebitIDType         =	cSrc.ServerJudgeDebitIDType;
        DebitStatusCode			=	cSrc.DebitStatusCode;
        DebitStatusDesc			=	cSrc.DebitStatusDesc;
        DebitOrderNumber                    =	cSrc.DebitOrderNumber;

        sn                                              =   cSrc.sn;
    }
    return *this;
}
