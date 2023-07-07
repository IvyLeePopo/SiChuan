#ifndef PAYDBINF_H
#define PAYDBINF_H

#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include "config.h"
#include <string>

using namespace std;

class PayDBInf : public QObject
{
    Q_OBJECT
public:
    static PayDBInf* instance();

    // test function
    void testPayDb();

    // api
    void addCancelData2DB(string payId);
    string getCancelDataPayId();
    void updateCancelDataById(string payId);

private:
    bool isPayIdExist(string payId);

private:
    bool connDB();
    void closeDB();
    bool openDB();
    QSqlDatabase database();

    PayDBInf();
    ~PayDBInf();

private:
    static PayDBInf *self;
    QSqlDatabase db;

    QString dbDriver;
    QString dbName;
    QString userName;
    QString userPwd;
    QString hostName;

    int hostPort;
};

#endif // PAYDBINF_H
