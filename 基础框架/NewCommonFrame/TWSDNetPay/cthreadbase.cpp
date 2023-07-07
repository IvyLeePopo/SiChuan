#include "cthreadbase.h"
#include <QDebug>

CThreadBase::CThreadBase(void):
    m_strLastErrorDesc(""),
    m_strPathLocalConfigFile("")
{
    memset(&m_sTranstParam, 0 , sizeof(tagTranstParam));


}

CThreadBase::~CThreadBase()
{

}


void CThreadBase::SetParam(const tagTranstParam* pTranstParam)
{
    memcpy(&m_sTranstParam,pTranstParam,sizeof(tagTranstParam));
}
