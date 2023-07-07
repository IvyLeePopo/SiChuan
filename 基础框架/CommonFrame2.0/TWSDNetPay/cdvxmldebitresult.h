/********************************************
 *
 * 类   名：CDVXMLDebitResult
 * 摘   要：生成XML字符串
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVXMLDEBITRESULT_H
#define CDVXMLDEBITRESULT_H

#include <QObject>
#include "cdvxmlbase.h"

//完
class CDVXMLDebitResult : public CDVXMLBase
{
    Q_OBJECT
public:
    explicit CDVXMLDebitResult(void);
    ~CDVXMLDebitResult(void);
    //解析XML字符串
     bool Init( const QString& strXMLString){return false;}
    //生成XML字符串
    QString GetXMLString(void);

signals:

public :
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
    QString DebitTime;

    //消耗时间，单位：毫秒
    quint64	ConsumeTime;
};

#endif // CDVXMLDEBITRESULT_H
