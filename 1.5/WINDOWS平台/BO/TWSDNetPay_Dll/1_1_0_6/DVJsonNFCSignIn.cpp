#include "StdAfx.h"
#include "DVJsonNFCSignIn.h"

CDVJsonNFCSignIn::CDVJsonNFCSignIn(void)
	:TestingData(FALSE)
	,NetPayDllVersion(_T(""))
	,DeviceDllVersion(_T(""))
	,TWProductSerialNo(_T(""))
	,NFCTerminal(_T(""))
	,NFCSerialNo(_T(""))
	,NFCSignData(_T(""))
{
}

CDVJsonNFCSignIn::~CDVJsonNFCSignIn(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonNFCSignIn::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonNFCSignIn& cSrc
// 输出参数：CDVJsonNFCSignIn&
// 编写人员：ROCY
// 编写时间：2017年9月20日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonNFCSignIn& CDVJsonNFCSignIn::operator=(IN const CDVJsonNFCSignIn& cSrc)
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
		m_hCallerWnd			=	cSrc.m_hCallerWnd;
		m_dwThreadID			=	cSrc.m_dwThreadID;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//本地类数据复制
		TestingData			= cSrc.TestingData;
		NetPayDllVersion	= cSrc.NetPayDllVersion;
		DeviceDllVersion	= cSrc.DeviceDllVersion;
		TWProductSerialNo	= cSrc.TWProductSerialNo;
		NFCTerminal			= cSrc.NFCTerminal;
		NFCSerialNo			= cSrc.NFCSerialNo;
		NFCSignData			= cSrc.NFCSignData;
	}
	return *this;
}