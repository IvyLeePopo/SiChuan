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
// �������ƣ�CDVJsonDebitSecond::operator=
//     
// �������������������
//     
// ���������IN const CDVJsonDebitSecond& cSrc
// ���������CDVJsonDebitSecond&
// ��д��Ա��ROCY
// ��дʱ�䣺2016��5��30��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CDVJsonDebitSecond::Reset
//     
// ������������������
//     
// ���������IN const int& iType
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��5��30��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void  CDVJsonDebitSecond::Reset(IN const int& iType)
{
	m_cDVJsonDebit.Reset();
	m_strLastPayIdentifier.Empty();
}