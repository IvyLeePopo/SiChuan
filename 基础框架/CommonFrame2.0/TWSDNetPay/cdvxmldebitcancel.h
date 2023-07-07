/********************************************
 *
 * 类   名：CDVXMLDebitCancel
 * 摘   要：撤单XML数据结构体，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
******************************************** */
#ifndef CDVXMLDEBITCANCEL_H
#define CDVXMLDEBITCANCEL_H

#include <QObject>
#include "cdvxmlbase.h"

//完
class CDVXMLDebitCancel : public CDVXMLBase
{
    Q_OBJECT
public:
    CDVXMLDebitCancel(void);
    ~CDVXMLDebitCancel(void);
    //解析XML字符串
    bool Init( const QString& strXMLString);

signals:

public slots:

public:
    //交易识别码
    QString PayIdentifier;
};

#endif // CDVXMLDEBITCANCEL_H
