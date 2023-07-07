/********************************************
 *
 * 类   名：CJsonOperationIF
 * 摘   要：Json与标准数据类进行互相转换的接口定义基类，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CJSONOPERATIONIF_H
#define CJSONOPERATIONIF_H

#include <QObject>
#include "cdvjsondebit.h"
#include "cdvjsondebitcancel.h"
#include "cdvjsonparamdebitresult.h"
#include "cdvjsonparamaccount.h"


class CJsonOperationIF : public QObject
{
    Q_OBJECT
public:
    explicit CJsonOperationIF(QObject *parent = 0);
    ~CJsonOperationIF(void);
    const QString GetLastErrorDesc(){return m_strLastErrorDesc;}

//接口定义
    //将接收到的数据结构转化为标准的JSON格式 -- 支付请求(扣费)
    virtual QString	FormatSendData2Json( const CDVJsonDebit* pDVJsonDebit)=0;

    //将接收到的数据结构转化为标准的JSON格式 -- 支付请求(撤单)
    virtual QString	FormatSendData2Json( const CDVJsonDebitCancel* pDVJsonDebitCancel)=0;

    //将接收到的数据结构转化为标准的JSON格式 -- 支付请求参数数据变换(扣费结果)
    virtual QString FormatParamData2Json( const CDVJsonParamDebitResult* pDVJsonParamDebitResult)=0;
    //将接收到的数据格式转化为标准的JSON格式 -- 用户数据参数数据变换
    virtual QString FormatParamData2Json( const CDVJsonParamAccount* pDVJsonParamAccount)=0;

    //将服务器返回的JSON格式转化为标准数据结构类
    virtual CDVJsonBase* FormatResponseResult2Class( const QString& strResponseResult, const int& iDataType)=0;
//属性
protected:
    //产生的错误信息描述
    QString	m_strLastErrorDesc;

signals:

public slots:
};

#endif // CJSONOPERATIONIF_H
