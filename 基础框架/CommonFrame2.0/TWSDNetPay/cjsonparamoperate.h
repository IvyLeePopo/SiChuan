#ifndef CJSONPARAMOPERATE_H
#define CJSONPARAMOPERATE_H

#include <QObject>
#include "cjsonoperationif.h"
#include "basedefine.h"
#include "cdvjsonparamdebitmoney.h"
#include "cdvjsonparamdebitcancel.h"



class CJsonParamOperate : public CJsonOperationIF
{
    Q_OBJECT
public:
    explicit CJsonParamOperate(void);
    ~CJsonParamOperate(void);

    //重载函数
    public:
        //将接收到的数据结构转化为标准的JSON格式 -- 支付请求参数数据变换(扣费结果)
        QString FormatParamData2Json( const CDVJsonParamDebitResult* pDVJsonParamDebitResult);
        //将接收到的数据格式转化为标准的JSON格式 -- 用户数据参数数据变换
        QString FormatParamData2Json( const CDVJsonParamAccount* pDVJsonParamAccount);

        //将JSON格式字符串转化为标准数据结构类
        CDVJsonBase* FormatResponseResult2Class( const QString& strResponseResult, const int& iDataType);
    private:
        //该函数不需要外部调用，屏蔽且并不实现
        QString	FormatSendData2Json( const CDVJsonDebit* ){return "";}

        //该函数不需要外部调用，屏蔽且并不实现
        QString	FormatSendData2Json( const CDVJsonDebitCancel* ){return "";}

signals:

public slots:
};

#endif // CJSONPARAMOPERATE_H
