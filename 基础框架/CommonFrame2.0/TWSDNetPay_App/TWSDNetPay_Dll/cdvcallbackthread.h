#ifndef CDVCALLBACKTHREAD_H
#define CDVCALLBACKTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>

class CdvcallbackThread : public QThread
{
    Q_OBJECT
public:
    explicit CdvcallbackThread(void);
    void setResults(int t_option, int t_result);
    void run();

signals:

public slots:

private:
    bool Isavailable;
    int m_option;           // 操作类型
    int m_result;           //操作结果

    // Mutex for locking
    QMutex m_mutex;
};

#endif // CDVCALLBACKTHREAD_H
