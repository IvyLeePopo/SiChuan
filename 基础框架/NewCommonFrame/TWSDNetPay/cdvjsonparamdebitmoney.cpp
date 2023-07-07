#include "cdvjsonparamdebitmoney.h"

CDVJsonParamDebitMoney::CDVJsonParamDebitMoney(void)
    :OverTime(0)
    ,OperationMode(0)

    ,PayIdentifier("")
    ,PayCertificateCode("")
    ,PayCertificateType(0)

    ,Money(0)
    ,SubTime("")
    ,DebitMainType(0)

    ,VehicleType(0)
    ,VehicleClass(0)
    ,VLColor(0)
    ,VehicleLicense("")
    ,AxisCount(0)
    ,Weight(0)
    ,LimitWeight(0)

    ,PassCertificateType(0)
    ,CardID("")
    ,TollDate(0)
    ,ShiftID(0)
    ,TicketNo("")
    ,OperatorID("")
    ,OperatorName("")


    ,EntryAreaID(0)
    ,EntryRoadID(0)
    ,EntryStationID(0)
    ,EntryStationName("")
    ,EntryTime("")
    ,EntryLane(0)
    ,EntryVehicleLicense("")
    ,EntryVLColor(0)
    ,EntryVehicleType(0)
    ,EntryVehicleClass(0)

{


}


CDVJsonParamDebitMoney::~CDVJsonParamDebitMoney(void)
{

}



CDVJsonParamDebitMoney& CDVJsonParamDebitMoney::operator=( const CDVJsonParamDebitMoney& cSrc)
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
        OverTime				=	cSrc.OverTime;
        OperationMode			=	cSrc.OperationMode;

        PayIdentifier			=	cSrc.PayIdentifier;
        PayCertificateCode		=	cSrc.PayCertificateCode;
        PayCertificateType		=	cSrc.PayCertificateType;
        Money					=	cSrc.Money;
        SubTime					=	cSrc.SubTime;
        DebitMainType			=	cSrc.DebitMainType;

        VehicleType				=	cSrc.VehicleType;
        VehicleClass			=	cSrc.VehicleClass;
        VLColor					=	cSrc.VLColor;
        VehicleLicense			=	cSrc.VehicleLicense;
        AxisCount				=	cSrc.AxisCount;
        Weight					=	cSrc.Weight;
        LimitWeight				=	cSrc.LimitWeight;

        PassCertificateType		=	cSrc.PassCertificateType;
        CardID					=	cSrc.CardID;
        TollDate				=	cSrc.TollDate;
        ShiftID					=	cSrc.ShiftID;
        TicketNo				=	cSrc.TicketNo;

        EntryAreaID				=	cSrc.EntryAreaID;
        EntryRoadID				=	cSrc.EntryRoadID;
        EntryStationID			=	cSrc.EntryStationID;
        EntryStationName		=	cSrc.EntryStationName;
        EntryTime				=	cSrc.EntryTime;
        EntryLane				=	cSrc.EntryLane;
        EntryVehicleLicense		=	cSrc.EntryVehicleLicense;
        EntryVLColor			=	cSrc.EntryVLColor;
        EntryVehicleType		=	cSrc.EntryVehicleType;
        EntryVehicleClass		=	cSrc.EntryVehicleClass;
    }
    return *this;
}
