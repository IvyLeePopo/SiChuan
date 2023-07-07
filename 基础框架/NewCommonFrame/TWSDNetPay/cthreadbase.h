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
    ~CThreadBase();

    //传递车道基本设置参数
    void	SetParam( const tagTranstParam* pTranstParam);
    //设置本地配置文件绝对路径
    void	SetLocalConfigFilePath( const QString& strFilePath){m_strPathLocalConfigFile = strFilePath;}

signals:

public slots:


protected:


    //最后的一次错误提示
    QString	 m_strLastErrorDesc;
    //配置文件绝对路径
    QString m_strPathLocalConfigFile;
    //车道基本配置信息
    tagTranstParam	m_sTranstParam;

};

#endif // CTHREADBASE_H
