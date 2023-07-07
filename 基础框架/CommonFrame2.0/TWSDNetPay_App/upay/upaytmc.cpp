#include "upaytmc.h"
#include <QDebug>
#include "cthreadhttp.h"
#include "TWSDNetPay_Dll/ctwsdnetpay_dllapp.h"
#include "basedefine.h"

UpayTMC *UpayTMC::self = NULL;

UpayTMC::UpayTMC()
{
    m_httwsDevice = HTTWSDDevice::instance();

    QByteArray terminal = "LANDI-E5X0";
    char hxxx=0x00;
    terminal.append(hxxx);

    QByteArray sn = "SN2015010801";
    sn.append(hxxx);

    m_ptzTerminal = terminal;
    m_ptzSn = sn;
    m_upayCorrectData.clear();

    m_isPosSignInSuccess = false;
    m_isNetworkNormal = false;

    initConnect();
}

UpayTMC* UpayTMC::instance ()
{
    if (NULL == self) {
        self = new UpayTMC();
    }

    return self;
}

bool UpayTMC::getServerNetworkStatus()
{
    m_isNetworkNormal = !theApp.m_bNetErrorHttpServerInTollNetNotExist;

    return m_isNetworkNormal;
}

bool UpayTMC::getPosSignInStatus()
{
    return m_isPosSignInSuccess;
}

QByteArray UpayTMC::getUpayCorrectBase64()
{
    return m_upayCorrectData.toBase64();
}

QByteArray UpayTMC::getUpayTerminalBase64()
{
    return m_ptzTerminal.toBase64();
}

QByteArray UpayTMC::getUpaySerialBase64()
{
    return m_ptzSn.toBase64();
}

// inform message to http thread
void UpayTMC::slMessageFromPayModule(QString contdata, QString type)
{
    QString json = "";

    // sync params from theApp
    getTheAppParams();

    // json format
    if (0 == type.compare("signin")) {
        json = packSignInMessage(contdata);

        emit sigSignIn2HttpThread(json);
    }
    else if (0 == type.compare("correct")) {
//        json = packCorrectMessage(contdata);

////        emit sigCorrect2HttpThread(json);
//        emit sigCorrectLastdeal2MGR(m_currentDebitCancelId, contdata);

//        m_currentDebitCancelId.clear();

        QByteArray contArray;
        int contLen = contdata.length()/2;
        contArray.append(contLen/255);
        contArray.append(contLen%255);

        contArray.append(QByteArray::fromHex(contdata.toLatin1()));

        m_upayCorrectData = contArray;
    }
    else if (0 == type.compare("57")) {
        QList<QByteArray> contList = QByteArray::fromHex(contdata.toLatin1()).split(0x1c);
//        m_ptzTerminal = contList.at(2);
//        m_ptzSn = contList.at(3);
//        m_ptzMerchant = contList.at(4);

        qDebug() << "update ptzTerminal: " << m_ptzTerminal << "/" << m_ptzSn << "/" << m_ptzMerchant;
    }
    else if (0 == type.compare("signIned")) {
        m_isPosSignInSuccess = true;
        m_lastPosSignInedTime = QDateTime::currentDateTime();
    }
    else if (0 == type.compare("signIned-not")) {
        m_isPosSignInSuccess = false;
        m_lastPosSignInedTime = QDateTime::currentDateTime();
    }
    else
        json.clear();

    qDebug() << "~~~~~~签到状态~~~~~~~: " << m_isPosSignInSuccess << "/" << m_lastPosSignInedTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
}

// transfer message to pos module
void UpayTMC::slMessageFromHttpThread(QString json, int op)
{
    int status = -1;

    if (json.isEmpty() || 0 == json.compare("success"))
        return;

    switch (op) {
    case JSON_DT_SEND_QUICKPASS_SIGNIN:
        status = handleRedata4SignIn(json);
        break;

    case JSON_DT_SEND_QUICKPASS_CORRECT:
        status = handleRedata4Correct(json);
        break;

    case JSON_DT_SEND_DIBIT:
    case JSON_DT_SEND_DIBIT_CHECK:
    case JSON_DT_SEND_DIBIT_CANCEL:
        status = handleRedataFromDebitData(json);
        break;

    default:
        break;
    }
}

// request correct op
void UpayTMC::slRequestCorrectOperation(QString payId)
{
    m_currentDebitCancelId = payId;

    emit sigTriggerCorrect();    
}

// trigger signIn
void UpayTMC::triggerFirstSignIn()
{
    emit sigFirstSignIn();
}

void UpayTMC::getTheAppParams()
{
    m_provinceId = theApp.m_sTranstParam.ProvinceID;

//    qDebug() << "----------UpayTMC::getTheAppParams----------";
//    qDebug() << "ProvinceID :   " << m_provinceId;
}

// pack json message
QString UpayTMC::packSignInMessage(QString conthex)
{
//    qDebug() << "UpayTMC::packSignInMessage: " << conthex.length() << "/" << conthex;

    QByteArray contdata;
    int contLen = conthex.length()/2;
    contdata.append(contLen/255);
    contdata.append(contLen%255);

    contdata.append(QByteArray::fromHex(conthex.toLatin1()));

    qDebug() << "contdata: " << contdata.length() << QString(contdata.toHex());

    Json::FastWriter write;
    Json::Value root;

//URL:/signIn 调用方式:Ajax 方法:POST 请求参数:
//{
//    Version:"1.0",           // 响应报文版本
//    ProvinceID:63,           // 递交数据所属省份
//    PackageType:31,           // 响应报文类别（见请求包分类定义）
//    Token: '',               // 身份验证的token，暂时为空

//    Terminal: "",               // 终端机型，二进制最大20字节，Base64编码字符串
//    Sn: "",                     // 硬件序列号，二进制最大38字节，Base64编码字符串
//    Data: "",                   // 发送数据，Base64编码字符串

//    SubmitCount: 2,          // 递交次数
//    SubmitTime:'2015-12-22 11:43:52' //递交时间
//}

    QString submitTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    root["Version"] = "1.0";
    root["ProvinceID"] = QString::number(m_provinceId).toStdString();
    root["PackageType"] = 41;
    root["Token"] = "";
    root["Terminal"] = QString(m_ptzTerminal.toBase64()).toStdString();
    root["Sn"] = QString(m_ptzSn.toBase64()).toStdString();
    root["Data"] = QString(contdata.toBase64()).toStdString();
    root["SubmitCount"] = "1";
    root["SubmitTime"] = submitTime.toStdString();

    qDebug() << "UpayTMC::packSignInMessage: " << QString(m_ptzTerminal) << "/" << QString(m_ptzSn);

    QString signInJson = QString::fromStdString(write.write(root));

    return signInJson;
}

QString UpayTMC::testSignInReturn()
{
    Json::FastWriter write;
    Json::Value root;

    root["Version"] = "1.0";
    root["PackageType"] = 1;
    root["Status"] = 12;
    root["Message"] = "LANDI-E5X0";
    root["ReData"] = "SN2015010801SN2015010801SN2015010801SN2015010801SN2015010801SN2015010801";
    root["ReceveTime"] = "2017-06-14 11:26:00";
    root["ResponseTime"] = "2017-06-14 11:28:22";

    QString signInReturnJson = QString::fromStdString(write.write(root));
    qDebug()<<"signInReturnJson: " << "/" << signInReturnJson;

    return signInReturnJson;
}

int UpayTMC::handleRedataFromDebitData(QString json)
{
    QString redata;
    QString message;
    int status = -1;

    if (0 == json.compare("success"))
        return 0;

    //进行数据格式转换
    Json::Reader reader;
    Json::Value  root;

    if (!reader.parse(json.toLocal8Bit().data(), root)) {
        qDebug() << "UpayTMC::handleRedataFromDebitData: json parse error";
        return -1;
    }

    status = root["Status"].asInt();

    if  (Json::stringValue == root["Message"].type())
        message = root["Message"].asCString();

    if  (Json::stringValue == root["ReData"].type())
        redata = root["ReData"].asCString();

    if (!redata.isEmpty())
        emit sigMessage2HTTWSD(redata.toLocal8Bit());

    return status;
}

// parse json ack message from http
int UpayTMC::handleRedata4SignIn(QString json)
{
    qDebug() << "UpayTMC::handleRedata4SignIn: " << json;

//响应参数:
//{
//    Version:"1.0",   // 响应报文版本
//    PackageType:1,   // 响应报文类别
//    Status:11,       // 响应状态 11:成功,其它参照文档《服务端返回错误代码定义》
//    Message:"",      // 不成功时的错误提示
//    ReData:"",       // 返回的报文内容
//    ReceveTime:"yyyy-mm-dd HH:mm:ss.SSS",
//    ResponseTime:"yyyy-mm-dd HH:mm:ss.SSS",

//}
//返回状态码：
//    0: 撤单失败
//    1: 执行成功
//    12: 协议格式错误（详细错误会放到message中）

    QString redata;
    QString message;
    int status = -1;

    //进行数据格式转换
    Json::Reader reader;
    Json::Value  root;

    if (!reader.parse(json.toLocal8Bit().data(), root)) {
        qDebug() << "UpayTMC::handleRedata4SignIn: json parse error";
        return -1;
    }

    status = root["Status"].asInt();

    if  (Json::stringValue == root["Message"].type())
        message = root["Message"].asCString();

    if  (Json::stringValue == root["ReData"].type())
        redata = root["ReData"].asCString();

    if  (Json::stringValue == root["recvRet"].type())
        redata = root["recvRet"].asCString();

    if (!redata.isEmpty()) {
        QByteArray decredata = QByteArray::fromBase64(redata.toLocal8Bit());
        emit sigMessage2HTTWSD(decredata);
    }

    return status;
}

int UpayTMC::handleRedata4Correct(QString json)
{
    return 0;
}

QString UpayTMC::packCorrectMessage(QString contdata)
{
    qDebug() << "UpayTMC::packCorrectMessage: " << contdata;
    return contdata;
}

// get contdata from json
QString UpayTMC::getContDataMessage(QString json)
{
    qDebug() << "UpayTMC::getContDataMessage: " << json;
    return json;
}

// init connect of signals/slots
void UpayTMC::initConnect()
{
    connect(m_httwsDevice, SIGNAL(sigMessage2TMC(QString, QString)), this, SLOT(slMessageFromPayModule(QString, QString)));
    connect(this, SIGNAL(sigFirstSignIn()), m_httwsDevice, SLOT(slStartFirstSignIn()));
    connect(this, SIGNAL(sigTriggerCorrect()), m_httwsDevice, SLOT(slCorrectLastdeal()));
    connect(this, SIGNAL(sigMessage2HTTWSD(QByteArray)), m_httwsDevice, SLOT(slMessageFromTMC(QByteArray)));
}
