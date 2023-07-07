/********************************************
 *
 * 类   名：CJsonHeartOperate
 * 摘   要：心跳数据结构体描述，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CJSONHEARTOPERATE_H
#define CJSONHEARTOPERATE_H

#include <QObject>
#include "cjsonoperationif.h"
#include "cdvjsonbase.h"


//完
class CJsonHeartOperate : public CJsonOperationIF
{
    Q_OBJECT
public:
    explicit CJsonHeartOperate(void );
    ~CJsonHeartOperate(void);
    //重载函数
    public:
        //将接收到的数据结构转化为标准的JSON格式 -- 心跳
         QString  FormatSendData2Json( const CDVJsonHeart* pDVJsonHeart);

        //将服务器返回的JSON格式转化为标准数据结构类
        CDVJsonBase* FormatResponseResult2Class(const QString& strResponseResult,const int& iDataType);
    private:
        //该函数不需要外部调用，屏蔽且并不实现
        QString	FormatSendData2Json( const CDVJsonDebit* pDVJsonDebit){return "";}
        //该函数不需要外部调用，屏蔽且并不实现
        QString	FormatSendData2Json( const CDVJsonDebitSecond* pDVJsonDebitSecond){return "";}
        //该函数不需要外部调用，屏蔽且并不实现
        QString	FormatSendData2Json( const CDVJsonDebitCheck* pDVJsonDebitCheck){return "";}
        //该函数不需要外部调用，屏蔽且并不实现
        QString	FormatSendData2Json( const CDVJsonDebitCancel* pDVJsonDebitCancel){return "";}

        //该函数不需要外部调用，屏蔽且并不实现
        QString FormatParamData2Json( const CDVJsonParamDebitResult* pDVJsonParamDebitResult) {return "";}
        //该函数不需要外部调用，屏蔽且并不实现
        QString FormatParamData2Json( const CDVJsonParamAccount* pDVJsonParamAccount) {return "";}


signals:

public slots:
};

#endif // CJSONHEARTOPERATE_H
