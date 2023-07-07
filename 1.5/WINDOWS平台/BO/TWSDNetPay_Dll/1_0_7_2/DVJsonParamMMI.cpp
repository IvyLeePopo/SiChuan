#include "StdAfx.h"
#include "DVJsonParamMMI.h"

CDVJsonParamMMI::CDVJsonParamMMI(void)
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