#ifndef HWPERFORMANCE_H
#define HWPERFORMANCE_H

#include <QObject>
#include "cxmldisplaycontext.h"
#include "mystdafx.h"

extern  int      G_msgScanDataLent;
extern  char      G_msgScanData[1024];

class HWPerformance : public QObject
{
    Q_OBJECT
public:
    static HWPerformance* instance();

    //主动资源申请
    bool InitDev(int iOverTime);
    //主动资源注销
    bool Destroy(void);
    //主动获取数据
    bool GetData(char* strData, int& iDataType, int& iSrcType);
    //整合支付操作 设置费显
    bool SetDisplayContext(const int iID, const int iType, QString istrData);
    //设置支付结果
    bool SetDisplayPayResults(const int iID, QString strData);

    //inform that get scaner data
    void onMessageUnderToDevice(int);

private:
    explicit HWPerformance(QObject *parent = 0);

signals:
    void ON_MSSAGE_UNDER_TO_DEVICER(int);

public slots:

private:
    static HWPerformance *self;
};

#endif // HWPERFORMANCE_H
