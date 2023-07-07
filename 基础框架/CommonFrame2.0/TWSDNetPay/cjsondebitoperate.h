/********************************************
 *
 * 类   名：CJsonDebitOperate
 * 摘   要：控制发起扣款请求的操作，进行数据存取以及格式化流程，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-23
 * 修改日期：
 *
*********************************************/
#ifndef CJSONDEBITOPERATE_H
#define CJSONDEBITOPERATE_H

#include <QObject>
#include "cjsonoperationif.h"
//#include "TWSDNetPay_Dll.h"
#include "basedefine.h"

class CJsonDebitOperate : public CJsonOperationIF
{
    Q_OBJECT
public:
    explicit CJsonDebitOperate(void);
    ~CJsonDebitOperate(void);

    //将接收到的数据结构转化为标准的JSON格式 -- 支付请求(扣费)
    QString	FormatSendData2Json( const CDVJsonDebit* pDVJsonDebit);

    //将接收到的数据结构转化为标准的JSON格式 -- 支付请求(撤单)
    QString	FormatSendData2Json( const CDVJsonDebitCancel* pDVJsonDebitCancel);

    //将服务器返回的JSON格式转化为标准数据结构类
    CDVJsonBase* FormatResponseResult2Class( const QString& strResponseResult,const int& iDataType);

signals:

public slots:

private:
       //该函数不需要外部调用，屏蔽且并不实现
 //       QString	FormatSendData2Json(const CDVJsonHeart* ){return "";}
        //该函数不需要外部调用，屏蔽且并不实现
        QString FormatParamData2Json(const CDVJsonParamDebitResult* ) {return "";}
        //该函数不需要外部调用，屏蔽且并不实现
        QString FormatParamData2Json(const CDVJsonParamAccount* ) {return "";}

        /******************************内部辅助函数******************************/

        //解析字符串，获得支付请求（扣费）服务端回馈数据
        CDVJsonDebit* fnAnalysisStringForDebit(const QString& strResponseResult);
        //解析字符串，获得支付请求（撤单）服务端回馈数据
        CDVJsonDebitCancel* fnAnalysisStringForDebitCancel(const QString& strResponseResult);
        //解析字符串，获得支付请求（二次处理）服务端回馈数据
        CDVJsonDebit* fnAnalysisStringForDebitSecond(const QString& strResponseResult);
};



#endif // CJSONDEBITOPERATE_H
