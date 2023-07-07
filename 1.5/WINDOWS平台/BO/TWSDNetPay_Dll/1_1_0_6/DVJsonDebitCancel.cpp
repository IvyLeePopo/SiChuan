#include "StdAfx.h"
#include "DVJsonDebitCancel.h"

CDVJsonDebitCancel::CDVJsonDebitCancel(void)
	:PayIdentifier(_T(""))
	,DebitType(0)
	,DebitMoney(0)
	,RequestingPartyType(1)
	,DebitOrderNumber(_T(""))
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCancel::CDVJsonDebitCancel
//     
// 功能描述：构造函数
//     
// 输入参数：IN const CDVJsonDebit* pDVJsonDebit
// 编写人员：ROCY
// 编写时间：2016年1月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel::CDVJsonDebitCancel(IN const CDVJsonDebit* pDVJsonDebit)
{
	//基类数据复制
	Version					=	pDVJsonDebit->Version;
	PackageType				=	pDVJsonDebit->PackageType;
	ProvinceID				=	pDVJsonDebit->ProvinceID;
	Token					=	pDVJsonDebit->Token;
	SubmitCount				=	pDVJsonDebit->SubmitCount;
	SubmitTime				=	pDVJsonDebit->SubmitTime;
	QRCodeData				=	pDVJsonDebit->QRCodeData;
	QRCodeType				=	pDVJsonDebit->QRCodeType;
	m_hCallerWnd			=	pDVJsonDebit->m_hCallerWnd;
	m_dwThreadID			=	pDVJsonDebit->m_dwThreadID;
	ServerResponseCode		=	pDVJsonDebit->ServerResponseCode;
	ServerReceiveDataTime	=	pDVJsonDebit->ServerReceiveDataTime;
	ServerResponseDataTime	=	pDVJsonDebit->ServerResponseDataTime;
	//本地类数据复制
	PayIdentifier			=	pDVJsonDebit->PayIdentifier;
	DebitType				=	pDVJsonDebit->DebitType;
	DebitMoney				=	pDVJsonDebit->DebitMoney;
	DebitTime				=	pDVJsonDebit->DebitTime;
	DebitOrderNumber		=	pDVJsonDebit->DebitOrderNumber;
	RequestingPartyType		=	0x01;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCancel::CDVJsonDebitCancel
//     
// 功能描述：构造函数
//     
// 输入参数：IN const CDVJsonDebitCheck* pDVJsonDebitCheck
// 编写人员：ROCY
// 编写时间：2016年1月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel::CDVJsonDebitCancel(IN const CDVJsonDebitCheck* pDVJsonDebitCheck)
{
	//基类数据复制
	Version					=	pDVJsonDebitCheck->Version;
	PackageType				=	pDVJsonDebitCheck->PackageType;
	ProvinceID				=	pDVJsonDebitCheck->ProvinceID;
	Token					=	pDVJsonDebitCheck->Token;
	SubmitCount				=	pDVJsonDebitCheck->SubmitCount;
	SubmitTime				=	pDVJsonDebitCheck->SubmitTime;
	QRCodeData				=	pDVJsonDebitCheck->QRCodeData;
	QRCodeType				=	pDVJsonDebitCheck->QRCodeType;
	m_hCallerWnd			=	pDVJsonDebitCheck->m_hCallerWnd;
	m_dwThreadID			=	pDVJsonDebitCheck->m_dwThreadID;
	ServerResponseCode		=	pDVJsonDebitCheck->ServerResponseCode;
	ServerReceiveDataTime	=	pDVJsonDebitCheck->ServerReceiveDataTime;
	ServerResponseDataTime	=	pDVJsonDebitCheck->ServerResponseDataTime;
	//本地类数据复制
	PayIdentifier			=	pDVJsonDebitCheck->PayIdentifier;
	DebitType				=	pDVJsonDebitCheck->DebitType;
	DebitMoney				=	pDVJsonDebitCheck->DebitMoney;
	DebitTime				=	pDVJsonDebitCheck->DebitTime;
	DebitOrderNumber		=	pDVJsonDebitCheck->DebitOrderNumber;
	RequestingPartyType		=	0x01;
}

CDVJsonDebitCancel::~CDVJsonDebitCancel(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCancel::Reset
//     
// 功能描述：重置数据
//     
// 输入参数：IN const int& iType -- 重置类型
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年1月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CDVJsonDebitCancel::Reset(IN const int& iType)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCancel::operator=
//     
// 功能描述：赋值运算符重载
//     
// 输入参数：IN const CDVJsonDebitCancel& cSrc
// 输出参数：CDVJsonDebitCancel&
// 编写人员：ROCY
// 编写时间：2016年1月6日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel& CDVJsonDebitCancel::operator=(IN const CDVJsonDebitCancel& cSrc)
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
		m_hCallerWnd			=	cSrc.m_hCallerWnd;
		m_dwThreadID			=	cSrc.m_dwThreadID;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//本地类数据复制
		PayIdentifier			=	cSrc.PayIdentifier;
		DebitType				=	cSrc.DebitType;
		DebitMoney				=	cSrc.DebitMoney;
		DebitTime				=	cSrc.DebitTime;
		DebitOrderNumber		=	cSrc.DebitOrderNumber;
		RequestingPartyType		=	cSrc.RequestingPartyType;
	}
	return *this;
}
