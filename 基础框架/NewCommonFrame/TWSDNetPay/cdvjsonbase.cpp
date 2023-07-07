#include "cdvjsonbase.h"

CDVJsonBase::CDVJsonBase(QObject *parent) : QObject(parent)
{

}



/*************************************************
 *函数名称：CDVJsonDebit::operator=
 *功能描述：重载运算符，使之俩个对象可以互相赋值
 *参   数：cSrc
 *返   回：this
**************************************************/
CDVJsonBase& CDVJsonBase::operator=( const CDVJsonBase& cSrc)
{
    if(this != &cSrc)
    {
        //消耗时间 -- 一般指数据从发送到服务器返回的处理时间
        m_dwConsumeTime  = cSrc.m_dwConsumeTime;
        /**************************发起请求需要填充的数据(公共)**************************/
        //数据包版本
        Version          = cSrc.Version;
        //数据包类型
        PackageType      = cSrc.PackageType;
        //数据包发送地所属省份
        ProvinceID       = cSrc.ProvinceID;
        //安全认证
        Token            = cSrc.Token;
        //递交次数
        SubmitCount      =  cSrc.SubmitCount;
        //递交时间
        SubmitTime       = cSrc.SubmitTime;
        //QR码数据内容
        QRCodeData       = cSrc.QRCodeData;
        //QR码来源
        QRCodeType       = cSrc.QRCodeType;
        //扣款识别码
        PayIdentifier    = cSrc.PayIdentifier;
        /**************************后台回馈需要填充的数据(公共)**************************/

        //服务器扣款时间
        ServerDebitTime          = cSrc.ServerDebitTime;
        //服务端反馈代码
        ServerResponseCode       = cSrc.ServerResponseCode;
        //错误信息描述
        ServerResponseResultDesc = cSrc.ServerResponseResultDesc;
        //服务器接收到请求的时间
        ServerReceiveDataTime    = cSrc.ServerReceiveDataTime;
        //服务器回馈数据的时间
        ServerResponseDataTime   = cSrc.ServerResponseDataTime;

    }
    return *this;
}
