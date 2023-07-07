#include "StdAfx.h"
#include "PayKeyItems.h"

CPayKeyItems::CPayKeyItems(void)
	:Valid(FALSE)
	,OverTime(0)
	,OperationMode(0)
	,PayIdentifier(_T(""))
	,Money(0)
	,SubTime(CTime(0))
	,DebitMainType(0)
	,PayCertificateCode(_T(""))
	,PayCertificateType(0)
	,VehicleType(0)
	,VehicleClass(0)
	,VLColor(0)
	,VehicleLicense(_T(""))
	,AxisCount(0)
	,TotalWeight(0)
	,LimitWeight(0)
	,PassCertificateType(0)
	,CardID(_T(""))
	,TollDate(0)
	,ShiftID(0)
	,TicketNo(_T(""))
	,OperatorID(_T(""))
	,OperatorName(_T(""))
	,EntryStationID(_T(""))
	,EntryStationName(_T(""))
	,EntryTime(CTime(0))
	,EntryLane(_T(""))
	,EntryVehicleLicense(_T(""))
	,EntryVLColor(0)
	,EntryVehicleType(0)
	,EntryVehicleClass(0)
	,EntryAreaID(_T("0"))
	,EntryRoadID(_T("0"))

	,NFCPayData(_T(""))
	,NFCRevData(_T(""))
{
}

CPayKeyItems::~CPayKeyItems(void)
{
}
