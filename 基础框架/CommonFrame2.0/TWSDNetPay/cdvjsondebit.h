/********************************************
 *
 * 类   名：CDVJsonDebit
 * 摘   要：需要扣费发起的请求JSON数据结构体
 * 作   者：CWX
 * 创建日期：2016-11-18
 * 修改日期：
 *
*********************************************/
#ifndef CDVJSONDEBIT_H
#define CDVJSONDEBIT_H

#include <QObject>
#include "cdvjsonbase.h"

//完
class CDVJsonDebit : public CDVJsonBase
{
    Q_OBJECT
public:
    explicit CDVJsonDebit(void);
    ~CDVJsonDebit(void);

    //运算符重载
    CDVJsonDebit& operator=( const CDVJsonDebit& cSrc);
    //Interface
    //重置数据
    void  Reset( const int& iType=0);
    //生成数据校验码
    quint64 CreateInnerVerifyCode(void){return 0;}
    //内部数据码校验
    bool  InnerVerify(void){return true;}

signals:

public slots:

public:
    /***********************************************发起请求需要填充的数据********************************************************/

    /*********************基本信息*********************/

    //是否测试数据
    bool	TestingData;
    QString 	NetPayDllVersion;
    QString     DeviceDllVersion;

    //二维码绑定的支付平台ID
    QString     IDBindInPayPlatform;
    //二维码绑定的支付平台类型
    int		IDBindInPayPlatformType;
    //交易唯一凭证(车道产生)
    QString     RecordMatchCode;
    //交易记录安全认证码
    QString     TradeSecurityCode;
    //扣款QR码
    QString	QRCodeData;
    //扣款类型(1,封闭路段扣费；4,开放路段扣费; 2，入口收费；3,补票)
    int		DebitType;
    //扣款金额
    int		DebitMoney;
    //扣款递交时间(与该条记录的产生时间一致)
    //time_t DebitTime;
    QString     DebitTime;
    //虚拟卡号（通行凭证）
    QString     SMVirtualCardID;
    //支付卡号
    QString     PayVirtualCardID;
    //统计日
    int		DebitTollDate;
    //统计班次
    int		DebitTollShift;
    //用户数据设备来源(1,扫码器)
    int		AccountInfoSrcType;
    //用户数据具体设备号
    int		AccountInfoDeviceID;
    //用户数据具体类型
    int		AccountInfoDataType;

    /*********************附加信息*********************/
    //入口省份
    int		EntryProvinceID;
    //入口站点
    int		EntryStationID;
    //入口站名
    QString	EntryStationName;
    //入口车道号
    int		EntryLane;
    //入站时间
    //time_t	EntryTime;
    QString     EntryTime;
    //入口信息唯一标识
    QString     EntryInfoUniqueFlag;
    //入口所属路段
    int		EntryRoadID;
    //入口所属区域编码
    int		EntryAreaID;

    //扣费地点所属省份
    int		PayProvinceID;
    //扣费站点
    int		PayStationID;
    //扣费站名
    QString     PayStationName;
    //扣费车道
    int		PayLane;
    //付费站点所属路段号
    int		PayRoadID;

    //扣费相关车辆车型
    int		VehicleClass;
    //扣费相关车辆种类
    int		VehicleType;
    //扣费相关车辆牌照
    QString	VehicleLicense;
    //扣费相关车辆车牌颜色
    int		VehicleColor;
    //扣费车辆轴数
    int		AxisCount;
    //扣费车辆载重
    int		TotalWeight;
    //扣费车辆限重
    int		LimitWeight;

    /***********************************************后台回馈需要填充的数据********************************************************/

    //服务器实际扣款金额
    int		ServerRealDebitMoney;
    //服务器判定支付平台类型(1,微信；2,支付宝）
    int		ServerJudgeDebitIDType;
    //扣款状态代码
    int		DebitStatusCode;
    //扣款状态描述
    QString     DebitStatusDesc;
    //扣款订单号(仅仅当扣款成功才有该项数据)
    QString	 DebitOrderNumber;


    //20170719
    int sn;  // 重传次数
};

#endif // CDVJSONDEBIT_H
