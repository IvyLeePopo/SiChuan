/********************************************
 *
 * 类   名：CDVJsonDebitCheck
 * 摘   要：扣款状态查询JSON结构信息描述，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-18
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONDEBITCHECK_H
#define CDVJSONDEBITCHECK_H

#include <QObject>
#include "cdvjsonbase.h"
#include "cdvjsondebit.h"

//完
class CDVJsonDebitCheck : 	public CDVJsonBase
{
    Q_OBJECT
public:
    explicit CDVJsonDebitCheck(void);
    CDVJsonDebitCheck( const CDVJsonDebit* pDVJsonDebit);
    CDVJsonDebitCheck( CDVJsonDebitCheck* pDVJsonDebitCheck);
    ~CDVJsonDebitCheck(void);
//运算符重载
    CDVJsonDebitCheck& operator=( const CDVJsonDebitCheck& cSrc);
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

        //车道产生的交易凭证号
        QString RecordMatchCode;
        //扣款识别码
        QString PayIdentifier;

        //二维码绑定的支付平台ID
        QString IDBindInPayPlatform;
        //二维码绑定的支付平台类型
        int		IDBindInPayPlatformType;
        //扣款类型
        int		DebitType;
        //扣款金额
        int		DebitMoney;
        //扣款递交时间(与该条记录的产生时间一致)
       // time_t	DebitTime;
        QString DebitTime;

        //扣款状态查询延迟时间(单位：毫秒)
        quint64	CheckStateDelayTime;

        /***********************************************后台回馈需要填充的数据********************************************************/

        //扣款状态代码
        int		DebitStatusCode;
        //扣款状态描述
        QString DebitStatusDesc;
        //扣款订单号(仅仅当扣款成功才有该项数据)
        QString	DebitOrderNumber;
};

#endif // CDVJSONDEBITCHECK_H
