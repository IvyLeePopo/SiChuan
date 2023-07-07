#include "cjsonoperationif.h"

CJsonOperationIF::CJsonOperationIF(QObject *parent) : QObject(parent)
{
    m_strLastErrorDesc = "";

}

CJsonOperationIF::~CJsonOperationIF(void)
{

}
