#include "upayutils.h"
#include "json/json.h"
#include "json/writer.h"
#include "json/reader.h"
#include "upaytmc.h"

#include <QDebug>

UPayUtils::UPayUtils()
{

}

QString UPayUtils::getNFCPayInfo(QString conthex)
{
    Json::FastWriter write;
    Json::Value root;

//NFCPayInfo:
//{
//    Terminal: "",               // 终端机型，二进制最大20字节，Base64编码字符串
//    Sn: "",                     // 硬件序列号，二进制最大38字节，Base64编码字符串
//    PayData: "",                   // 扣费发送的数据，Base64编码字符串
//    RevData: "",                   // 冲正发送的数据，Base64编码字符串
//},

//    qDebug() << "UPayUtils::getNFCPayInfo: " << conthex.length() << "/" << conthex;

    QByteArray contdata;
    int contLen = conthex.length()/2;
    contdata.append(contLen/256);
    contdata.append(contLen%256);

    contdata.append(QByteArray::fromHex(conthex.toLatin1()));

    qDebug() << "\n\nUPayUtils::getNFCPayInfo: " << contdata.length() <<"/" << contLen << "/" << contdata.toHex();

    QString terminal = QString(UpayTMC::instance()->getUpayTerminalBase64());
    QString sn = QString(UpayTMC::instance()->getUpaySerialBase64());
    QString correct = QString(UpayTMC::instance()->getUpayCorrectBase64());

    root["Terminal"] = terminal.toStdString();
    root["Sn"] = sn.toStdString();
    root["PayData"] = QString(contdata.toBase64()).toStdString();
    root["RevData"] = correct.toStdString();

    QString json = QString::fromStdString(write.write(root));
    qDebug()<<"NFCPayInfo json: " << "/" << json;

    return json;
}
