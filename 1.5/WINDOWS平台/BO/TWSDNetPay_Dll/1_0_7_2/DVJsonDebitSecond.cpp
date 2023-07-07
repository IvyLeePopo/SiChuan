#include "StdAfx.h"
#include "DVJsonDebitSecond.h"

CDVJsonDebitSecond::CDVJsonDebitSecond(IN const CDVJsonDebit* pDVJsonDebit)
{
	if(NULL != pDVJsonDebit)
	{
		m_dwConsumeTime = pDVJsonDebit->m_dwConsumeTime;
		m_hCallerWnd	= pDVJsonDebit->m_hCallerWnd;
		m_dwThreadID	= pDVJsonDebit->m_dwThreadID;
		Version			= pDVJsonDebit->Version;
		ProvinceID		= pDVJsonDebit->ProvinceID;
		Token			= pDVJsonDebit->Token;
	}
}

CDVJsonDebitSecond::~CDVJsonDebitSecond(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitSecond::operator=
//     
// 功能描述：运算符重载
//     
// 输入参数：IN const CDVJsonDebitSecond& cSrc
// 输出参数：CDVJsonDebitSecond&
// 编写人员：ROCY
// 编写时间：2016年5月30日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitSecond& CDVJsonDebitSecond::operator=(IN const CDVJsonDebitSecond& cSrc)
{
	if(this != &cSrc)
	{
		m_cDVJsonDebit			= cSrc.m_cDVJsonDebit;
		m_strLastPayIdentifier	= cSrc.m_strLastPayIdentifier;
	}
	return *this;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CDVJsonDebitSecond::Reset
//     
// 功能描述：重置数据
//     
// 输入参数：IN const int& iType
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年5月30日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void  CDVJsonDebitSecond::Reset(IN const int& iType)
{
	m_cDVJsonDebit.Reset();
	m_strLastPayIdentifier.Empty();
}