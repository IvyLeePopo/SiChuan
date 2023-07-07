/********************************************
 *
 * 类   名：CDVJsonParamDebitResult
 * 摘   要：获取扣款接口参数JSON数据格式解析，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONPARAMDEBITRESULT_H
#define CDVJSONPARAMDEBITRESULT_H

#include <QObject>
#include "cdvjsonbase.h"

class CDVJsonParamDebitResult : public CDVJsonBase
{
    Q_OBJECT
public:

    CDVJsonParamDebitResult(void);
    ~CDVJsonParamDebitResult(void);
//运算符重载
    CDVJsonParamDebitResult& operator=( const CDVJsonParamDebitResult& cSrc);


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
    //结果类型
    int		ResultType;
    //执行状态
    int		ExecuteCode;
    //执行结果描述
    QString ExecuteDesc;


    //交易识别码
    QString PayIdentifier;
    //第三方支付平台类型
    int		PayPlatformType;
    //交易安全码
    QString TradeSecurityCode;
    //订单号
    QString DebitOrder;
    //服务器扣款时间
   // time_t	DebitTime;
    QString  DebitTime;

    //消耗时间，单位：毫秒
    int		ConsumeTime;
};

#endif // CDVJSONPARAMDEBITRESULT_H
