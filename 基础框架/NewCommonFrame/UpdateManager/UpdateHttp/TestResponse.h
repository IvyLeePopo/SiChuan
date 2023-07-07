#ifndef TESTRESPONSE_H
#define TESTRESPONSE_H

#include "json/json.h"
#include <QObject>

#include <UpdateCommon.h>
class TestResponse:public UpdateCommon
{
public:
    TestResponse();

    QString getFileMd5(QString fileName);

    QString generateQueryResponse();
    QString generateDownLoadResponse();

    static long long offset;
};

#endif // TESTRESPONSE_H
