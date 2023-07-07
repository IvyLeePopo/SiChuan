#ifndef DEVICEINFOMANAGEMENT_H
#define DEVICEINFOMANAGEMENT_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "cdvjsoncommanddebit.h"

class DeviceInfoManagement : public CDVJsonCommandDebit
{
    Q_OBJECT
public:
    explicit DeviceInfoManagement();

    bool DeviceInfoManagementInit();

    QByteArray SendHeartbeatCommand();

    QByteArray FeedBackHeartbeatCommand(char t_NumericalOrderH,char t_NumericalOrderL);


signals:
    void sigSendHeartCommand(int, int);

public slots:
   void ReadStateData();

private:
   QTimer  *m_ReadStateTimer;     //读取数据定时器



};

#endif // DEVICEINFOMANAGEMENT_H
