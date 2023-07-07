#include "paydbinf.h"

PayDBInf *PayDBInf::self = NULL;

PayDBInf::PayDBInf()
{
    dbDriver = "QSQLITE";
    dbName = "/opt/scaner/app/trawe.db";
    userName = "trawe";
    userPwd = "trawe";
    hostName = "localhost";
    hostPort = 5432;
    qDebug() << "Db instance inited...";

    openDB();
}

PayDBInf::~PayDBInf()
{
    closeDB();
}

PayDBInf* PayDBInf::instance ()
{
    if (NULL == self) {
        self = new PayDBInf();
    }

    return self;
}

bool PayDBInf::connDB()
{
    qDebug() << "Db::connDb -> " << hostName << "/" << dbName;

    db = QSqlDatabase::addDatabase(dbDriver);
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(userPwd);

    QString strlog;

    // create connection
    if (!db.open()) {
        strlog = "[数据库撤单模块]，数据库打开失败，错误代码："+db.lastError().text();
        qDebug() << strlog;

        return false;
    }

    bool breturn;

    //建立数据库表
    QSqlQuery query;
    breturn = query.exec("create table orderCancelData(id integer not null primary key autoincrement,       \
                         payId text,                                                                        \
                         json text,                                                                         \
                         comment text)                                                                      \
            ");

            if (breturn) {
            strlog.clear();
            strlog = "[数据库撤单模块]，建立数据库表成功";
}
            else {
            strlog.clear();
            strlog = "[数据库撤单模块]，数据库表已存在";
}

            qDebug() << "PayDBInf::connDB" + strlog;

    return true;
}

void PayDBInf::closeDB()
{
    if (db.isOpen()) {
        qDebug() << "------close db------: ";
        db.close();
    }
}

bool PayDBInf::openDB()
{
    return connDB();
}

QSqlDatabase PayDBInf::database()
{
    return db;
}

void PayDBInf::addCancelData2DB(string id)
{
    if (isPayIdExist(id))
        return;

    QString payId = QString::fromStdString(id);

    QSqlQuery query;
    QString sql = "INSERT INTO orderCancelData (payId) VALUES(:payId);";

    query.prepare(sql);
    query.bindValue(":payId", payId);

    if (!query.exec())
    {
        qDebug()<<tr("撤单Id %1 加入缓存失败%2").arg(payId).arg(query.lastError().text());
        return;
    }
    qDebug()<<tr("撤单Id %1 已加入缓存").arg(payId);
}

void PayDBInf::addCancelData2DB(string id, int type)
{
    if (isPayIdExist(id))
        return;

    QString payId = QString::fromStdString(id);

    QSqlQuery query;
    QString sql = "INSERT INTO orderCancelData (payId, type) VALUES (:payId, :type);";

    query.prepare(sql);
    query.bindValue(":payId", payId);
    query.bindValue(":type", type);

    if (!query.exec())
    {
        qDebug()<<tr("撤单Id %1 加入缓存失败%2").arg(payId).arg(query.lastError().text());
        return;
    }
    qDebug()<<tr("撤单Id %1 已加入缓存").arg(payId);
}

QString PayDBInf::getCurrentOfCancelPayId()
{
    QString payId;

    m_mutex4PayId.lock();
    payId = m_curPayId;
    m_curPayId.clear();
    m_mutex4PayId.unlock();

    return payId;
}

string PayDBInf::getCancelDataPayId()
{
    string str = "";

    static time_t lastime = time(NULL);
    int intval = abs(time(NULL) - lastime);
    if (intval < 30)
        return str;
    else
        lastime = time(NULL);

    QSqlQuery q;
    QString sql = "SELECT payId FROM orderCancelData;";
    q.prepare(sql);
    if (!q.exec())
    {
        qDebug() << "into GetCancelDataPayId ERROR: " << q.lastError().text();
        return str;
    }

    while (q.next())
    {
        QString payId = q.value(0).toString();

        qDebug() << "取得1条撤单缓存数据: " << payId;
        if (!payId.isEmpty())
        {
            str = payId.toStdString();
            return str;
        }
    }

    return str;
}

int PayDBInf::getCancelTypeById(string payId)
{
    int type = 1;
    return type;

    QSqlQuery q;
    QString sql = "SELECT type FROM orderCancelData WHERE payId=:payId;";
    q.prepare(sql);
    q.bindValue(":payId", QString::fromStdString(payId));

    if (!q.exec())
    {
       // qDebug() << "into getCancelTypeById ERROR: " << q.lastError().text();
        return type;
    }

    while (q.next())
    {
        int type = q.value(0).toInt();
        qDebug() << "取得1条撤单缓存数据: " << type << "/" << QString::fromStdString(payId);

        return type;
    }

    qDebug() << "\n~~~~~~PayDBInf::getCancelTypeById~~~~~~: " << type << "/" << QString::fromStdString(payId);

    return type;
}

void PayDBInf::updateCancelDataById(string id)
{
    qDebug() << "PayDBInf::updateCancelDataById: " << QString::fromStdString(id);

    QString payId = QString::fromStdString(id);

    QSqlQuery q;
    QString sql = "DELETE FROM orderCancelData WHERE payId=:payId;";
    q.prepare(sql);
    q.bindValue(":payId", payId);
    if (!q.exec())
    {
        qDebug() << "into updateCancelDataById ERROR: " << q.lastError().text();
        return;
    }

    m_mutex4PayId.lock();
    m_curPayId = payId;
    m_mutex4PayId.unlock();

    return;
}

bool PayDBInf::isPayIdExist(string id)
{
    QString payId = QString::fromStdString(id);

    QSqlQuery q;
    QString sql = "SELECT payId FROM orderCancelData WHERE payId=:payId;";
    q.prepare(sql);
    q.bindValue(":payId", payId);
    if (!q.exec())
    {
        qDebug() << "into isPayIdExist ERROR: " << q.lastError().text();
        return false;
    }

    while (q.next())
    {
        qDebug()<<tr("into IsPayIdExist: %1 此撤单Id已存在,无需加入缓存").arg(payId);
        return true;
    }

    return false;
}
