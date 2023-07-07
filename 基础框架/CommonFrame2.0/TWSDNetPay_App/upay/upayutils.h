#ifndef UPAYUTILS_H
#define UPAYUTILS_H

#include <QObject>

class UPayUtils : public QObject
{
    Q_OBJECT
public:
    UPayUtils();

    static QString getNFCPayInfo(QString contdata);
};

#endif // UPAYUTILS_H
