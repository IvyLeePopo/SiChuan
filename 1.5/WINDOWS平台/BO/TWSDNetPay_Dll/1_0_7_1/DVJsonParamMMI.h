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
};
