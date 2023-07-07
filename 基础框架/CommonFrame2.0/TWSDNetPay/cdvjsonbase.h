/********************************************
 *
 * 类   名：CDVJsonBase
 * 摘   要：Json数据载体的基本类，其它类继承于它
 * 作   者：CWX
 * 创建日期：2016-11-18
 * 修改日期：
 *
*********************************************/

#ifndef CDVJSONBASE_H
#define CDVJSONBASE_H

#include <QObject>
#include <QDebug>
#include "jsoncpp/json.h"

//完
class CDVJsonBase : public QObject
{
    Q_OBJECT
public:
    explicit CDVJsonBase(QObject *parent = 0);

    //运算符重载
    CDVJsonBase& operator=( const CDVJsonBase& cSrc);

    //重置数据
    void  Reset(const int& iType=0){}
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}



signals:

public slots:

public:
    //消耗时间 -- 一般指数据从发送到服务器返回的处理时间
    quint64	m_dwConsumeTime;
    /**************************发起请求需要填充的数据(公共)**************************/

    //数据包版本
    QString	 Version;
    //数据包类型
    int		PackageType;
    //数据包发送地所属省份
    int		ProvinceID;
    //安全认证
    QString	Token;
    //递交次数
    int		SubmitCount;
    //递交时间
    QString	SubmitTime;
    //QR码数据内容
    QString	QRCodeData;
    //QR码来源
    int		QRCodeType;
    //扣款识别码
    QString PayIdentifier;
    /**************************后台回馈需要填充的数据(公共)**************************/

    //服务器扣款时间
    time_t	ServerDebitTime;
    //服务端反馈代码
    int	     ServerResponseCode;
    //错误信息描述
    QString     ServerResponseResultDesc;
    //服务器接收到请求的时间
    QString     ServerReceiveDataTime;
    //服务器回馈数据的时间
    QString     ServerResponseDataTime;

};

#endif // CDVJSONBASE_H
