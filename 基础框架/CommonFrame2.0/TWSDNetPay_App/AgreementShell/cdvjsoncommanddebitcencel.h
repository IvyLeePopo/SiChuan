/****
A4
*****/

#ifndef CDVJSONCOMMANDDEBITCENCEL_H
#define CDVJSONCOMMANDDEBITCENCEL_H

#include <QObject>
#include "cdvjsoncommanddebit.h"

class CDVJsonCommandDebitCencel : public CDVJsonCommandDebit
{
    Q_OBJECT
public:
    explicit CDVJsonCommandDebitCencel();
    bool fnAnalysisStringForData(const QString &strResponseResult);
    QString	FormatParamData2Json();

signals:

public slots:

public:
   QString m_PayIdentifier;
    int m_type;
};

#endif // CDVJSONCOMMANDDEBITCENCEL_H
