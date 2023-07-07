#include "twsutils.h"
#include <stdlib.h>
#include <QDebug>
#include <QDateTime>
#include "paydbinf.h"
#include "TWSDNetPay_Dll/ctwsdnetpay_dllapp.h"

TWSUtils::TWSUtils()
{

}

void TWSUtils::setSystemTime(QString dt)
{
    QDateTime dataTime = QDateTime::fromString(dt, "yyyy-MM-dd hh:mm:ss");
    QDateTime utcdt = dataTime.toTimeSpec(Qt::UTC);
    QString dtstr = utcdt.toString("yyyyMMddhhmmss");

    QString dtStr = QString("date -s %1").arg(dtstr);
    dtStr.insert(20, ".");
    QString hwclockStr = QString("hwclock -w");

    qDebug() << "TWSUtils::setSystemTime: " << dtStr << "/" << hwclockStr;

    system(dtStr.toStdString().c_str());
    system(hwclockStr.toStdString().c_str());

    system("hwclock -s");
}

void TWSUtils::addDebitCancelId2DB(QString id, int type)
{
//    qDebug() << "TWSUtils::addDebitCancelId2DB: " << type << "/" << id;
    PayDBInf::instance()->addCancelData2DB(id.toStdString(), type);

//    QString strLog = "[主进程响应], PayDBInf::addCancelData2DB: "+id;
//    qDebug() << strLog;
}

//站点编码，少于10个字符的，左补0到10个字符；大于10个字符小于20个字符，左补0到20个字符
QString TWSUtils::formatDebitString(QString str, int len)
{
    int provinceId = GetGlobalApp()->m_sTranstParam.ProvinceID;

    QString str7 = "0000000";
    QString str10 = "0000000000";
    QString str20 = "00000000000000000000";
    QString strDebit;
    QString leftStrTemp;

    int iRemainCount = 0;

    if (63 == provinceId) {
        if (len <= 7) {
            iRemainCount = 7 - len;
            leftStrTemp = str7.left(iRemainCount);
            strDebit = QString("%1%2").arg(leftStrTemp).arg(str);
        }
        else
            strDebit = str;
    }
    else {
        if (len <= 10) {
            iRemainCount = 10 - len;
            leftStrTemp = str10.left(iRemainCount);
            strDebit = QString("%1%2").arg(leftStrTemp).arg(str);
        }
        else if (len < 20){
            iRemainCount = 20 - len;
            leftStrTemp = str20.left(iRemainCount);
            strDebit = QString("%1%2").arg(leftStrTemp).arg(str);
        }
        else {
            strDebit = str;
        }
    }

    return strDebit;
}

QString TWSUtils::getFormatStrEntryST()
{
    QString entryStationId = GetGlobalApp()->m_sCurrentDebitInfo.EntryStationID;
    qDebug()<<"下发入口站点:  "<<entryStationId;
    int len = entryStationId.length();

    return formatDebitString(entryStationId, len);
}

QString TWSUtils::getFormatStrExitST()
{
    QString stationId = QString::fromLocal8Bit(GetGlobalApp()->m_sTranstParam.LaneConfigInfo.StationID);
    qDebug()<<"下发出口站点: "<<stationId;
    int len = stationId.length();

    return formatDebitString(stationId, len);
}

