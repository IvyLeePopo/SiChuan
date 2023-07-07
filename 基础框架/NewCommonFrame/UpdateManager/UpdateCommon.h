#ifndef UPDATECOMMON_H
#define UPDATECOMMON_H

#include <string>
#include <QObject>

class UpdateCommon:public QObject
{
public:
    UpdateCommon();

    std::string datetime2Str(time_t dt);

    void readFileName();
    static QList<QString> dllNameList;

};

#endif // UPDATECOMMON_H
