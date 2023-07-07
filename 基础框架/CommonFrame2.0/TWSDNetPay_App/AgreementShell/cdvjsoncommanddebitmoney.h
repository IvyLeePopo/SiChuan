/*****
 A3
******/
#ifndef CDVJSONCOMMANDDEBITMONEY_H
#define CDVJSONCOMMANDDEBITMONEY_H

#include <QObject>
/************
 A3
*************/
#include "cdvjsoncommanddebit.h"
#include <QDateTime>


class CDVJsonCommandDebitMoney : public CDVJsonCommandDebit
{
    Q_OBJECT
public:
    explicit CDVJsonCommandDebitMoney();
    bool fnAnalysisStringForData(const QString &strResponseResult);
    QString	FormatParamData2Json();

signals:

public slots:

public:
    /*******Debit******/
     int     m_Money;
     QString m_PayCertificateCode;
     int     m_PayCertificateType;
     QString m_PayIdentifier;
     QString m_SubTime;
     int     m_DeType;

     /******Entry******/
     int     m_EntryAreaID;
     int     m_EntryLaneID;
     QString m_EntryEnLicense;
     int     m_EntryRoadID;
     QString m_EntryStationID;
     QString m_EntryStationName;
     QString m_EntryTime;
     int     m_EntryClass;
     QString m_EntryLicense;
     int     m_EntryColor;
     int     m_EntryType;

     /******Operatio****/
     QString m_CardID;
     int     m_PassCertificateType;
     int     m_ShiftID;
     int     m_TollDate;
     int     m_OperationMode;
     int     m_OverTime;

     QString m_OperatorID;
     QString m_OperatorName;

     /*******Vehicle******/
     int     m_VehicleAxisCount;
     int     m_VehicleClass;
     QString m_VehicleLicense;
     int     m_VehicleType;
     int     m_VehicleColor;
     int     m_VehicleWeight;

};

#endif // CDVJSONCOMMANDDEBITMONEY_H
