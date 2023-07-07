#include "cdvjsondebitsecond.h"


CDVJsonDebitSecond::CDVJsonDebitSecond( const CDVJsonDebit* pDVJsonDebit)
{
    if(NULL != pDVJsonDebit)
    {
        m_dwConsumeTime = pDVJsonDebit->m_dwConsumeTime;
        Version			= pDVJsonDebit->Version;
        ProvinceID		= pDVJsonDebit->ProvinceID;
        Token			= pDVJsonDebit->Token;
    }
}

CDVJsonDebitSecond::~CDVJsonDebitSecond(void)
{
}

CDVJsonDebitSecond& CDVJsonDebitSecond::operator=( const CDVJsonDebitSecond& cSrc)
{
    if(this != &cSrc)
    {
        m_cDVJsonDebit			= cSrc.m_cDVJsonDebit;
        m_strLastPayIdentifier	= cSrc.m_strLastPayIdentifier;
    }
    return *this;
}
