#ifndef UPAYTMC_H
#define UPAYTMC_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include "httwsddevice.h"
#include "twsdnetpay_dll_global.h"

class UpayTMC : public QObject
{
    Q_OBJECT
public:
    static UpayTMC* instance();

    void triggerFirstSignIn();

    QByteArray getUpayTerminalBase64();
    QByteArray getUpaySerialBase64();
    QByteArray getUpayCorrectBase64();

    // get terminal status
    bool getPosSignInStatus();
    bool getServerNetworkStatus();

private:
    UpayTMC();

    void initConnect();

    // pack json message
    QString packSignInMessage(QString contdata);
    QString packCorrectMessage(QString contdata);

    // parse json ack message from http
    int handleRedata4SignIn(QString json);
    int handleRedata4Correct(QString json);
    int handleRedataFromDebitData(QString json);

    // get contdata from json
    QString getContDataMessage(QString json);

    // get env form theApp
    void getTheAppParams();

private:
    QString testSignInReturn();

signals:
    void sigMessage2HTTWSD(QByteArray array);
    void sigSignIn2HttpThread(QString data);

    // correct
    void sigCorrect2HttpThread(QString data);
    void sigCorrectLastdeal2MGR(QString payId, QString data);

    void sigFirstSignIn();
    void sigTriggerCorrect();

public slots:
    void slMessageFromPayModule(QString str, QString type);
    void slMessageFromHttpThread(QString data, int op);

    // request correct op
    void slRequestCorrectOperation(QString payId);

private:
    HTTWSDDevice *m_httwsDevice;

    // params
private:
    static UpayTMC *self;

    int m_provinceId;

    QString m_currentDebitCancelId;

    //
    QByteArray m_ptzTerminal;
    QByteArray m_ptzSn;
    QByteArray m_ptzMerchant;
    QByteArray m_upayCorrectData;  ////冲正数据

private:
    bool m_isPosSignInSuccess;
    bool m_isNetworkNormal;
    QDateTime m_lastPosSignInedTime;
};

#endif // UPAYTMC_H
