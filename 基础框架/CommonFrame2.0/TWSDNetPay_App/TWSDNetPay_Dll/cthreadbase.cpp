#include "cthreadbase.h"
#include <QDebug>

CThreadBase::CThreadBase(void)
{

}


void CThreadBase::SetParam(const tagTranstParam* pTranstParam)
{
    memcpy(&m_sTranstParam,pTranstParam,sizeof(tagTranstParam));

}
