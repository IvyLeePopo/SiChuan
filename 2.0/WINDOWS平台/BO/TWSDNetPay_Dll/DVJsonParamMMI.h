#pragma once
#include "dvjsonbase.h"

class CDVJsonParamMMI :
	public CDVJsonBase
{
public:
	CDVJsonParamMMI(void);
	~CDVJsonParamMMI(void);
//运算符重载
	CDVJsonParamMMI& operator=(IN const CDVJsonParamMMI& cSrc);
//Interface
public:
	//重置数据
	void  Reset(IN const int& iType=0);
	//生成数据校验码
	DWORD CreateInnerVerifyCode(void){return 0;}
	//内部数据码校验
	BOOL  InnerVerify(void){return TRUE;}
//属性
public:
	//交互界面特殊处理标记
	int ClearMMI;
	//文字显示数据
	CArray<tagMMIItemText,tagMMIItemText&>		Text;
	//声音数据
	CArray<tagMMIItemVoice,tagMMIItemVoice&>	Voice;
	//图形数据
	CArray<tagMMIItemGraph,tagMMIItemGraph&>	Graph;

public:
		//入口车牌
	CString	EntryVehicleLicense;
	//入口车牌颜色
	int		EntryVLColor;
	//入口车种
	int		EntryVehicleType;
	//入口车型
	int		EntryVehicleClass;
	//入口站点
	int		EntryStationID;
	//入口站名
	CString EntryStationName;
		//轴数
	int		AxisCount;
	//重量
	int		Weight;
	//限重
	int		LimitWeight;
	//交易金额(单位：分)
	int		Money;
	//余额(单位：分)
	int nBalance;
	//经过站点个数
	int nCroseNum;
	//节目
	int nProgram;
	//备注信息
	CString csRemark1;
	CString csRemark2;


};
