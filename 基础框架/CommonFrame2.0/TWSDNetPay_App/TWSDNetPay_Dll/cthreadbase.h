#ifndef CTHREADBASE_H
#define CTHREADBASE_H

#include <QObject>
#include "QThread"
#include "basedefine.h"

class CThreadBase : public QThread
{
    Q_OBJECT
public:
    explicit CThreadBase(void);

    //传递车道基本设置参数
    void	SetParam( const tagTranstParam* pTranstParam);
    //设置本地配置文件绝对路径
    void	SetLocalConfigFilePath( const QString& strFilePath){m_strPathLocalConfigFile = strFilePath;}

signals:

public slots:

public:
    //线程退出标记
    bool	m_bExit;
protected:

    //线程是否正处于工作状态
    bool	m_bWorking;
    //最后的一次错误提示
    QString	m_strLastErrorDesc;
    //配置文件绝对路径
    QString m_strPathLocalConfigFile;
    //车道基本配置信息
    tagTranstParam	m_sTranstParam;

};

#endif // CTHREADBASE_H
