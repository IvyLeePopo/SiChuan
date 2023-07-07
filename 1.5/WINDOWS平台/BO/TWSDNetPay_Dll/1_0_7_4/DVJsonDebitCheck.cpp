#include "StdAfx.h"
#include "DVJsonDebitCheck.h"

CDVJsonDebitCheck::CDVJsonDebitCheck(void)
	:RecordMatchCode(_T(""))
	,IDBindInPayPlatform(_T(""))
	,IDBindInPayPlatformType(0)
	,PayIdentifier(_T(""))
	,DebitType(0)
	,DebitMoney(0)
	,CheckStateDelayTime(0)	
	
	,DebitStatusCode(0)
	,DebitStatusDesc(_T(""))
	,DebitOrderNumber(_T(""))
{
}

CDVJsonDebitCheck::~CDVJsonDebitCheck(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCheck::Reset
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
void CDVJsonDebitCheck::Reset(IN const int& iType)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCheck::CDVJsonDebitCheck
//     
// 功能描述：构造函数
//     
// 输入参数：IN const CDVJsonDebit* pDVJsonDebit
// 编写人员：ROCY
// 编写时间：2015年12月16日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck::CDVJsonDebitCheck(IN const CDVJsonDebit* pDVJsonDebit)
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
	RecordMatchCode			=	pDVJsonDebit->RecordMatchCode;
	IDBindInPayPlatform		=	pDVJsonDebit->IDBindInPayPlatform;
	IDBindInPayPlatformType =	pDVJsonDebit->IDBindInPayPlatformType;
	PayIdentifier			=   pDVJsonDebit->PayIdentifier;
	DebitType				=	pDVJsonDebit->DebitType;
	DebitMoney				=	pDVJsonDebit->DebitMoney;
	DebitTime				=	pDVJsonDebit->DebitTime;
	DebitStatusCode			=	pDVJsonDebit->DebitStatusCode;
	DebitStatusDesc			=	pDVJsonDebit->DebitStatusDesc;
	DebitOrderNumber		=	pDVJsonDebit->DebitOrderNumber;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCheck::CDVJsonDebitCheck
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
CDVJsonDebitCheck::CDVJsonDebitCheck(IN const CDVJsonDebitCheck* pDVJsonDebitCheck)
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
	RecordMatchCode			=	pDVJsonDebitCheck->RecordMatchCode;
	IDBindInPayPlatform		=	pDVJsonDebitCheck->IDBindInPayPlatform;
	IDBindInPayPlatformType =	pDVJsonDebitCheck->IDBindInPayPlatformType;
	PayIdentifier			=   pDVJsonDebitCheck->PayIdentifier;
	DebitType				=	pDVJsonDebitCheck->DebitType;
	DebitMoney				=	pDVJsonDebitCheck->DebitMoney;
	DebitTime				=	pDVJsonDebitCheck->DebitTime;
	DebitStatusCode			=	pDVJsonDebitCheck->DebitStatusCode;
	DebitStatusDesc			=	pDVJsonDebitCheck->DebitStatusDesc;
	DebitOrderNumber		=	pDVJsonDebitCheck->DebitOrderNumber;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitCheck::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonDebitCheck& cSrc
// 输出参数：CDVJsonDebitCheck&
// 编写人员：ROCY
// 编写时间：2015年12月15日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck& CDVJsonDebitCheck::operator=(IN const CDVJsonDebitCheck& cSrc)
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
		ServerDebitTime			=	cSrc.ServerDebitTime;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//本地类数据复制
		RecordMatchCode			=	cSrc.RecordMatchCode;
		IDBindInPayPlatform		=	cSrc.IDBindInPayPlatform;
		IDBindInPayPlatformType =	cSrc.IDBindInPayPlatformType;
		PayIdentifier			=   cSrc.PayIdentifier;
		DebitType				=	cSrc.DebitType;
		DebitMoney				=	cSrc.DebitMoney;
		DebitTime				=	cSrc.DebitTime;
		CheckStateDelayTime		=	cSrc.CheckStateDelayTime;
		DebitStatusCode			=	cSrc.DebitStatusCode;
		DebitStatusDesc			=	cSrc.DebitStatusDesc;
		DebitOrderNumber		=	cSrc.DebitOrderNumber;
	}
	return *this;
}