#ifndef TWSUTILS_H
#define TWSUTILS_H

#include <QString>

class TWSUtils
{
public:
    TWSUtils();

    static void setSystemTime(QString dtStr);
    static void addDebitCancelId2DB(QString id, int type);
    static QString formatDebitString(QString str, int len);
    static QString getFormatStrEntryST();
    static QString getFormatStrExitST();
};

#endif // TWSUTILS_H
