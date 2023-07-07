/********************************************
 *
 * 类   名：CDVJsonDebitCancel
 * 摘   要：撤单请求数据结构体描述，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-18
 * 修改日期：
 *
*********************************************/

#ifndef CDVJSONDEBITCANCEL_H
#define CDVJSONDEBITCANCEL_H

#include <QObject>
#include "cdvjsondebit.h"
#include "cdvjsondebitcheck.h"


//完
class CDVJsonDebitCancel : public CDVJsonBase
{
    Q_OBJECT
public:
    explicit CDVJsonDebitCancel(void );

    ~CDVJsonDebitCancel(void);
    CDVJsonDebitCancel( const CDVJsonDebit* pDVJsonDebit);
    CDVJsonDebitCancel( const CDVJsonDebitCheck* pDVJsonDebitCheck);
    //运算符重载
    CDVJsonDebitCancel& operator=( const CDVJsonDebitCancel& cSrc);
    //Interface
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
    /***********************************************发起请求需要填充的数据********************************************************/
    //扣款识别码
    QString PayIdentifier;
    //扣款类型
    int		DebitType;
    //扣款金额
    int		DebitMoney;
    //扣款递交时间(与该条记录的产生时间一致)
   // time_t	DebitTime;
    QString	DebitTime;

    //撤单请求方类型
    int		RequestingPartyType;

    //NFC support
    int     m_payPlatformType;
    QString m_contdata;

    /***********************************************后台回馈需要填充的数据********************************************************/
    //扣款订单号(仅仅当撤单成功才有可能有该项数据)
    QString	DebitOrderNumber;
};

#endif // CDVJSONDEBITCANCEL_H
