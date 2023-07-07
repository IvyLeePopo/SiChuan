#ifndef CDVJSONCOMMANDWRITE_H
#define CDVJSONCOMMANDWRITE_H

#include <QObject>
#include <QThread>

//写数据到底层

class CDVJsonCommandWrite : public QThread
{
    Q_OBJECT
public:
    explicit CDVJsonCommandWrite(QObject *parent = 0);
    void run();

    bool WriteCommand(char *CommandData,int lenth);
    bool getWriteResult();

signals:

public slots:

private:
    //数据容器
    char m_sendData[2048];
    //发送完成标志
    bool m_sendCompletionFlag;
    //发送结果
    bool m_sendResultsFlag;
    //发送长度
    int  m_length;


};

#endif // CDVJSONCOMMANDWRITE_H
