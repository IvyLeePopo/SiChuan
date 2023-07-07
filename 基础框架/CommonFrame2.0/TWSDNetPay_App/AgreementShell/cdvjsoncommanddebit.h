#ifndef CDVJSONCOMMANDDEBIT_H
#define CDVJSONCOMMANDDEBIT_H

#include <QObject>
#include "TWSDNetPay_Dll/json/json.h"
#include <QDebug>


#define  COMMAND_START   0XDF
#define  COMMAND_END     0XFD


class CDVJsonCommandDebit : public QObject
{
    Q_OBJECT
public:
    explicit CDVJsonCommandDebit(QObject *parent = 0);
    bool fnAnalysisStringForData(const QString &strResponseResult);
    QString	FormatParamData2Json();

    ushort GetVerifyCode(QByteArray t_lpData, int t_wLength);



signals:

public slots:

public:
   QString m_Version;

};

#endif // CDVJSONCOMMANDDEBIT_H
