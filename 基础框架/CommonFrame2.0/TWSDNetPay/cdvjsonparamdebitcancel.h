/********************************************
 *
 * 类   名：CDVJsonParamDebitCancel
 * 摘   要：撤单接口参数JSON数据格式解析，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONPARAMDEBITCANCEL_H
#define CDVJSONPARAMDEBITCANCEL_H

#include <QObject>
#include "cdvjsonbase.h"

class CDVJsonParamDebitCancel : public CDVJsonBase
{
    Q_OBJECT
public:
    CDVJsonParamDebitCancel(void);
    ~CDVJsonParamDebitCancel(void);
//运算符重载
    CDVJsonParamDebitCancel& operator=(const CDVJsonParamDebitCancel& cSrc);
//Interface
public:
    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}



signals:

public slots:

//属性
public:
//交易识别码
QString PayIdentifier;
};

#endif // CDVJSONPARAMDEBITCANCEL_H
