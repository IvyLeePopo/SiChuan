#ifndef PAYDBINF_H
#define PAYDBINF_H

#include <QtSql>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QMutex>

#include <string>

using namespace std;

class PayDBInf : public QObject
{
    Q_OBJECT
public:
    static PayDBInf* instance();


    // api
    void addCancelData2DB(string payId);
    void addCancelData2DB(string id, int type);
    string getCancelDataPayId();
    int getCancelTypeById(string payId);
    void updateCancelDataById(string payId);

    QString getCurrentOfCancelPayId();

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

private:
    QString m_curPayId;
    QMutex m_mutex4PayId;
};

#endif // PAYDBINF_H
