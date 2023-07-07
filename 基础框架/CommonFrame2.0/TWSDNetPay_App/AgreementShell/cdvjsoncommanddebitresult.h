#ifndef CDVJSONCOMMANDDEBITRESULT_H
#define CDVJSONCOMMANDDEBITRESULT_H

#include <QObject>
#include "cdvjsoncommanddebit.h"


class CDVJsonCommandDebitResult : public CDVJsonCommandDebit
{
    Q_OBJECT
public:
    explicit CDVJsonCommandDebitResult();
    bool fnAnalysisStringForData(const QString &strResponseResult);
    QString	FormatParamData2Json();

    QString DebitResultSuccess();
    QString DebitResultfail(int t_ResultType,int t_ExecuteCode);

    QByteArray DebitResultCommand(char t_NumericalOrderH,char t_NumericalOrderL,QString ResultJson);
    QString CencelResultCommand(char t_result, QString payId);

signals:

public slots:



private:
     int      m_ExecuteCode;
     QString  m_ExecuteDesc;
     int      m_ResultType;
     int      m_ConsumeTime;
     QString  m_DebitOrder;
     QString  m_DebitTime;
     QString  m_PayIdentifier;
     int      m_PayPlatformType;
     QString  m_TradeSecurityCode;

};

#endif // CDVJSONCOMMANDDEBITRESULT_H
