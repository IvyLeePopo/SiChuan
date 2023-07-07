/********************************************
 *
 * 类   名：CDVJsonHeart
 * 摘   要：心跳接口数据交互描述结构体，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONHEART_H
#define CDVJSONHEART_H

#include <QObject>
#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QDomAttr>
#include "cdvjsonbase.h"


//完
class CDVJsonHeart :  public CDVJsonBase
{
    Q_OBJECT
public:
    CDVJsonHeart(void);
    ~CDVJsonHeart(void);
//运算符重载
    CDVJsonHeart& operator=( const CDVJsonHeart& cSrc);

    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}

signals:

public slots:

public:
    //站点编码
    int		StationID;
    //站点名称
    QString	StationName;
    //车道编码
    int		LaneID;
    //心跳时间
    time_t	HeartTime;
};

#endif // CDVJSONHEART_H
