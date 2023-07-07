/********************************************
 *
 * 类   名：CDVJsonParamDebitMoney
 * 摘   要：扣款接口参数JSON数据格式解析，声明文件
 * 作   者：CWX
 * 创建日期：2016-11-21
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONPARAMDEBITMONEY_H
#define CDVJSONPARAMDEBITMONEY_H

#include <QObject>
#include "cdvjsonbase.h"

//完
class CDVJsonParamDebitMoney : public CDVJsonBase
{
    Q_OBJECT
public:

    CDVJsonParamDebitMoney(void);
    ~CDVJsonParamDebitMoney(void);
    //运算符重载
    CDVJsonParamDebitMoney& operator=( const CDVJsonParamDebitMoney& cSrc);

    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}
signals:

public slots:



public:
    //超时等待时间
        int		OverTime;
        //扣款模式
        int		OperationMode;
        /*****************Debit详细项************************/
        //交易识别码
        QString  PayIdentifier;
        //第三方支付凭证内容
        QString  PayCertificateCode;
        //第三方支付凭证类别
        int		 PayCertificateType;
        //交易金额(单位：分)
        int		 Money;
        //过车时间
        //time_t	 SubTime;
        QString  SubTime;
        //交易主分类
        int		 DebitMainType;

        /*****************Vehicle详细项************************/
        //车种
        int		VehicleType;
        //车型
        int		VehicleClass;
        //车牌颜色
        int		VLColor;
        //车牌
        QString	VehicleLicense;
        //轴数
        int		AxisCount;
        //重量
        int		Weight;
        //限重
        int		LimitWeight;

        /*****************Operation详细项************************/
        //通行凭证类型
        int		PassCertificateType;
        //通行凭证编码
        QString CardID;
        //统计日
        int		TollDate;
        //统计班次
        int		ShiftID;
        //打印票据号
        QString TicketNo;

        //收费员工号
        QString OperatorID;
        //收费员姓名
        QString OperatorName;


        /*****************Entry详细项************************/
        //入口区域编码
        int		EntryAreaID;
        //入口路段编码
        int		EntryRoadID;
        //入口站点
        int		EntryStationID;
        //入口站点----字符串形式
        QString    str_EntryStationID;
        //入口站名
        QString EntryStationName;
        //入口时间
       // time_t	EntryTime;
         QString EntryTime;
        //入口车道
        int		EntryLane;
        //入口车牌
        QString	EntryVehicleLicense;
        //入口车牌颜色
        int		EntryVLColor;
        //入口车种
        int		EntryVehicleType;
        //入口车型
        int		EntryVehicleClass;
};

#endif // CDVJSONPARAMDEBITMONEY_H
