#include "paydbinf.h"
#include "twsdnetpay.h"

PayDBInf *PayDBInf::self = NULL;

PayDBInf::PayDBInf()
{

    dbDriver = "QSQLITE";
    dbName   = config::sDatabaseName;
    userName = config::sUserName;
    userPwd  = config::sPassword;
    hostName = config::sHostName;
    hostPort = 5432;

    qDebug() << "Db instance inited...XXX";

    openDB();
}

PayDBInf::~PayDBInf()
{

}

PayDBInf* PayDBInf::instance ()
{
    if (NULL == self) {
        self = new PayDBInf();
    }

    return self;
}

/***************************************
函数说明：数据库链接
修改时间：2017-05-10
****************************************/
bool PayDBInf::connDB()
{
    QString strlog;

    db = QSqlDatabase::addDatabase(dbDriver);
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(userPwd);


    if (!db.open())
    {
        qDebug() << db.lastError().text();
        strlog = "[数据库撤单模块]，数据库打开失败，错误代码："+db.lastError().text();
        theApp.RecordLog(strlog);
        return false;
    }
    else
    {
        bool breturn;

        QSqlQuery query;

        //        breturn = query.exec("create table orderCancelData(id integer not null primary key autoincrement,       \
        //                             payId text,                                                                        \
        //                             json text,                                                                         \
        //                             comment text)                                                                      \
        //                ");

        breturn = query.exec("create table orderCancelData(id integer not null primary key autoincrement,payId text,json text,comment text)");
        if(breturn)
        {
            strlog.clear();
            strlog = "[数据库撤单模块]，建立数据库表成功";
            theApp.RecordLog(strlog);
        }
        else
        {
            strlog.clear();
            strlog = "[数据库撤单模块]，数据库表已存在";
            theApp.RecordLog(strlog);
        }
        qDebug()<<strlog;
    }

    return true;
}

void PayDBInf::closeDB()
{
    if (db.isOpen())
    {
        qDebug() << "------close db------: ";
        theApp.RecordLog("            Close  db");
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

void PayDBInf::testPayDb()
{
    // delete records
    qDebug() << "DELETE FROM orderCancelData"
                ;
    QSqlQuery q;
    q.prepare("DELETE FROM orderCancelData");
    if (!q.exec()) {
        qDebug() << "clearAllRecord: " <<  q.lastError().text();
    }

    //
    addCancelData2DB("88888888");

    string payId = getCancelDataPayId();
    qDebug() << "get PayId: " << QString::fromStdString(payId);

    updateCancelDataById("88888888");
    qDebug() << "get2 PayId: " << QString::fromStdString(getCancelDataPayId());
}

/***************************************
函数说明：增加撤单数据
修改时间：2017-05-10
****************************************/
void PayDBInf::addCancelData2DB(string id)
{
    qDebug() << "PayDBInf::addCancelData2DB";

    if (isPayIdExist(id))
        return;

    QString payId = QString::fromStdString(id);

    QSqlQuery query;
    QString sql = "INSERT INTO orderCancelData(payId) VALUES(:payId)";

    query.prepare(sql);
    query.bindValue(":payId", payId);

    QString strLog="into addCancelData2DB:   ";
    theApp.RecordLog(strLog);
    strLog = tr("insert into orderCancelData(payId) values %1").arg(payId);
    theApp.RecordLog(strLog);

    if (!query.exec())
    {
        qDebug() << "addCancelData2DB: INSERT INTO orderCancelData(payId)" << query.lastError().text() << ", " << query.lastError().number();
        strLog = strLog + tr("insert into orderCancelData(payId) values %1 ERROR#####").arg(payId);
        return;
    }
}


/***************************************
函数说明：获得撤单数据
修改时间：2017-05-10
****************************************/
string PayDBInf::getCancelDataPayId()
{
    string str = "";

    static time_t lastime = time(NULL);
    int intval = abs(time(NULL) - lastime);
    if (intval < 30)
        return str;
    else
        lastime = time(NULL);

    QString strLog;
    strLog = "into getCancelDataPayId:  ";

    QSqlQuery q;
    q.prepare("SELECT payId FROM orderCancelData;");
    if (!q.exec())
    {
        theApp.RecordLog(strLog + "SELECT payId FROM orderCancelData ERROR#####" + q.lastError().text());
        return str;
    }

    while (q.next())
    {
        QString payId = q.value(0).toString();

        if (!payId.isEmpty())
        {
            strLog = "取得1条记录:  " + payId;
            theApp.RecordLog(strLog);

            str = payId.toStdString();
            return str;
        }
    }

    return str;
}


/***************************************
函数说明：跟新撤单数据
修改时间：2017-05-10
****************************************/
void PayDBInf::updateCancelDataById(string id)
{
    qDebug() << "PayDBInf::updateCancelDataById: " << QString::fromStdString(id);

    QString payId = QString::fromStdString(id);

    QString strLog;
    strLog = "into updateCancelDataById:     ";
    theApp.RecordLog(strLog);

    QSqlQuery q;
    q.prepare("DELETE FROM orderCancelData WHERE payId = :payId;");
    q.bindValue(":payId", payId);
    if (!q.exec())
    {
        qDebug() << "```updateCancelDataById DELETE FROM orderCancelData: " << q.lastError().text();
        strLog = tr("执行  DELETE FROM orderCancelData WHERE payId = %1 ERROR####").arg(payId);
        theApp.RecordLog(strLog);
        return;
    }
    strLog = tr("执行  DELETE FROM orderCancelData WHERE payId = %1 成功").arg(payId);
    theApp.RecordLog(strLog);

    return;
}


/***************************************
函数说明：判断撤单数据是否存在
修改时间：2017-05-10
****************************************/
bool PayDBInf::isPayIdExist(string id)
{
    QString payId = QString::fromStdString(id);

    QString strLog;
    strLog = "into isPayIdExist:   ";
    theApp.RecordLog(strLog);

    QSqlQuery q;
    q.prepare("SELECT payId FROM orderCancelData WHERE payId = :payId;");
    q.bindValue(":payId", payId);
    if (!q.exec())
    {
        qDebug() << "isPayIdExist SELECT payId FROM orderCancelData: " << q.lastError().text();
        return false;
    }

    while (q.next())
    {
        strLog = tr("撤单ID  %1 表中已存在").arg(payId);
        theApp.RecordLog(strLog);
        return true;
    }


    return false;
}
