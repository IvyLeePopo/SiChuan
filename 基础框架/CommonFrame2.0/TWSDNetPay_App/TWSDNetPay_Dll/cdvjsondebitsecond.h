/********************************************
 *
 * 类   名：CDVJsonDebitSecond
 * 摘   要：二次扣款处理的数据结构体描述类
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONDEBITSECOND_H
#define CDVJSONDEBITSECOND_H

#include <QObject>
#include "cdvjsonbase.h"
#include "cdvjsondebit.h"


//完
class CDVJsonDebitSecond:public CDVJsonBase
{
    Q_OBJECT
public:
    CDVJsonDebitSecond( const CDVJsonDebit* pDVJsonDebit);
    ~CDVJsonDebitSecond(void);
    //运算符重载
    CDVJsonDebitSecond& operator=( const CDVJsonDebitSecond& cSrc);

    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}


    /***********************************************发起请求需要填充的数据********************************************************/

    //扣款信息结构体
    CDVJsonDebit m_cDVJsonDebit;
    //上一次扣款支付请求识别码
    QString		 m_strLastPayIdentifier;
};

#endif // CDVJSONDEBITSECOND_H
