/********************************************
 *
 * 类   名：CDVJsonParamAccount
 * 摘   要：获取用户信息JSON数据格式解析，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONPARAMACCOUNT_H
#define CDVJSONPARAMACCOUNT_H

#include <QObject>
#include "cdvjsonbase.h"

//完
class CDVJsonParamAccount : public CDVJsonBase
{
    Q_OBJECT
public:
    CDVJsonParamAccount(void);
    ~CDVJsonParamAccount(void);
    //运算符重载
    CDVJsonParamAccount& operator=( const CDVJsonParamAccount& cSrc);
    //Interface
    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}

signals:

public slots:


public:
    //数据类型
    int	DataType;
    //数据来源
    int	DataSrc;
    //数据内容
    QString DataContext;
};

#endif // CDVJSONPARAMACCOUNT_H
