#ifndef MYSTDAFX
#define MYSTDAFX

struct tagScanDisplay
{
    int         iSerial;        //序列号，用于区分不用的显示命令，加1递增
    int         Type;           //显示类型      1-包含金额，重量，车型；   2-仅仅包含当前时间； 3-包含金额，重量，车型,并启动扫码；
    int         Money;      //金额，单位分
    int         Weight;     //重量，单位千克
    int         VehicleType;	//车种	1客2货
    int         VehicleClass;	//车型，1~5
    // time_t	tShowtime;	//显示时间
    int         LimitWeight;	//限重
    int         Balance;        //余额
    int         AxisCount;      //轴数
    char      EntrySTName[32];//入口站点
};













#endif // MYSTDAFX

