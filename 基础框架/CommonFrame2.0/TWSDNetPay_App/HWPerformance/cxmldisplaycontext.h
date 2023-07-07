#ifndef CXMLDISPLAYCONTEXT_H
#define CXMLDISPLAYCONTEXT_H

#include <QObject>
#include "../TWSDNetPay_Dll/cdvxmlbase.h"
#include "mystdafx.h"
#include <QDebug>

class CXmlDisplayContext : public CDVXMLBase
{
    Q_OBJECT
public:
    explicit CXmlDisplayContext(void);

    //解析XML字符串
    const bool  Init( const QString& strXMLString);
    const void  GetTagScanDisplay( tagScanDisplay&	t_sScanTemp);

    tagScanDisplay  m_sScanTemp;

signals:

public slots:
};

#endif // CXMLDISPLAYCONTEXT_H
