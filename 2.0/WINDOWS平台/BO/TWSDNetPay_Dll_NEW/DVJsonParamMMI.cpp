#include "StdAfx.h"
#include "DVJsonParamMMI.h"

CDVJsonParamMMI::CDVJsonParamMMI(void)
	:ClearMMI(0)
	,EntryVehicleLicense(_T(""))
	,EntryVLColor(0)
	,EntryVehicleType(0)
	,EntryVehicleClass(0)
	,EntryStationID(0)
	,EntryStationName(_T(""))
	,AxisCount(0)
	,Weight(0)
	,LimitWeight(0)
	,Money(0)
	,Balance(0)
	,CroseNum(0)
	,Program(0)
	,Remark1(_T(""))
	,Remark2(_T(""))
{
	Version.Format(_T("1.0"));
}

CDVJsonParamMMI::~CDVJsonParamMMI(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonParamMMI::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonParamMMI& cSrc
// 输出参数：CDVJsonParamMMI&
// 编写人员：ROCY
// 编写时间：2016年8月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonParamMMI& CDVJsonParamMMI::operator=(IN const CDVJsonParamMMI& cSrc)
{
	if(this != &cSrc)
	{
	}

	return *this;
}