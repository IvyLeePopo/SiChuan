#ifndef CDVXMLACCOUNTCONTEXT_H
#define CDVXMLACCOUNTCONTEXT_H

#include <QObject>
#include "cdvxmlbase.h"

class CDVXMLAccountContext : public CDVXMLBase
{
    Q_OBJECT
public:
    explicit CDVXMLAccountContext(void );

    //解析XML字符串
     bool	  Init( const QString& strXMLString);
    //提供XML字符串
    const QString GetXMLString(const QString& strData, const int& iDataType, const int& iSource);

signals:

public slots:

    //Attributes
public:
   //数据类型
   int		Type;
   //数据来源
   int		Src;
   //数据内容
   QString Context;
};

#endif // CDVXMLACCOUNTCONTEXT_H
