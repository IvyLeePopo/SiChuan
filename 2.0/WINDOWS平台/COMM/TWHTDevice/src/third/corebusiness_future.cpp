/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: CoreBusiness_Future.cpp
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-10
* 修改记录:
* 2017-04 由于目前下位机无法关联指定查询信息的序列号，因此在回复序列号上面，无法和上位机的序列号对应起来，因此回复的C和B的序列号统一为0
*/
#include "stdafx.h"
#include "CoreBusiness_Future.h"
#include "WorkingPara.h"
#include "ThreadHelper.h"
#include "ErrorHelper.h"
#include "RawFrameConv.h"
#include "DataAuth.h"
#include "TypeHelper.h"
#include "SystemTrace.h"
#include "BusinessConv.h"
#include "json/json.h"
#include <algorithm>
#include <bitset>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CCoreBusiness_Future, CWinThread)

CCoreBusiness_Future::CCoreBusiness_Future()
{
	SetDefault();
	mpCmd = (CCommondDak*)::AfxBeginThread(RUNTIME_CLASS(CCommondDak), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (mpCmd)
	{
		TW_TRACE_DIARY_TYPE("开始通讯线程...", TW_DIARY_MSG_THREAD_API);
		mpCmd->m_bAutoDelete = false;
		mpCmd->ResumeThread();
		TW_TRACE_DIARY_TYPE("启动通讯线程完毕", TW_DIARY_MSG_THREAD_API);
	}
	else
	{
		TW_TRACE_DIARY_TYPE("通讯创建失败", TW_DIARY_MSG_THREAD_API);
	}
	::InitializeCriticalSection(&mFrameMutex);
	::InitializeCriticalSection(&mInterfaceMutex);
	::InitializeCriticalSection(&mDebitCancleMutex);
	::InitializeCriticalSection(&mRespondMutex);
	::InitializeCriticalSection(&m_FeedbackCommondMutex);
	


}
CCoreBusiness_Future::~CCoreBusiness_Future()
{
	{
		CScopeLock teminal(&mInterfaceMutex);
		Stop();
		delete mpCmd;
		mpCmd = NULL;
	}
	::DeleteCriticalSection(&mFrameMutex);
	::DeleteCriticalSection(&mInterfaceMutex);
	::DeleteCriticalSection(&mDebitCancleMutex);
	::DeleteCriticalSection(&mRespondMutex);
	::DeleteCriticalSection(&m_FeedbackCommondMutex);

	memset(&mFrameMutex, 0, sizeof(CRITICAL_SECTION));
	memset(&mInterfaceMutex, 0, sizeof(CRITICAL_SECTION));
	memset(&mDebitCancleMutex, 0, sizeof(CRITICAL_SECTION));
	memset(&mRespondMutex, 0, sizeof(CRITICAL_SECTION));

}


/**
 * 接口实现 TR300
 */
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_InitDevice_L
//
// 功能描述:用于初始化设备，此接口只会调用一次
//
// 输入参数:IN  NotifyFun fun,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
// 目前此结构被调用后，是直接返回成功的，不会因为串口或者其他失败而返回失败
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_InitDevice_L(IN  NotifyFun fun)
{
	CInterfaceTraceInfo tmpLog("IF_InitDevice");
	CScopeLock accessL(&mInterfaceMutex);
	if (mHasInit)
	{
		return mInitResult;
	}
	mHasInit = true;
	mInitResult = true;
	return mInitResult;

}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_Terminate_L
//
// 功能描述:资源释放接口，此接口只会被调用一次，用于释放资源
//
// 输入参数:
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_Terminate_L()
{
	CInterfaceTraceInfo tmpLog("IF_Terminate");
	mTerminate = true;
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}
	mInitResult = false;

	return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_GetLastError_L
//
// 功能描述:用于最后一次接口操作的错误信息
//
// 输入参数:std::string& errorInfo,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_GetLastError_L(std::string& errorInfo)
{
	CInterfaceTraceInfo tmpLog("IF_GetLastError");
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}
	errorInfo = GetErrorDesc();
	return true;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_GetLastResult_L
//
// 功能描述:暂时保留使用。
//
// 输入参数:int& type, std::string& resultStr,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_GetLastResult_L(int& type, std::string& resultStr)
{
	CInterfaceTraceInfo tmpLog("IF_GetLastResult");
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}
	TW_SET_ERROR(TW_ERROR_OPERATION_NOT_SUPPORTED);
	return false;//
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_InitPara_L
//
// 功能描述:用于初始化扣费参数，进行扣款操作前，需要先进行此操作
//
// 输入参数:const std::string& paraStr, int paraType,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
// 此初始化操作，在终端上电自检后，会重新发送一次该信息。
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_InitPara_L( const std::string& paraStr, int paraType )
{
	CInterfaceTraceInfo tmpLog("IF_InitPara", paraStr);
	CScopeLock accessL(&mInterfaceMutex);

	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}

	mInitJsonStr = paraStr;
	TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_INITJSON, GetNextSN());
	TWProtocolData_A_SendData oneData(oneFrame);
	oneData.data.pInitJson = new  char[paraStr.size() + 1];
	if (oneData.data.pInitJson == NULL)
	{
		TW_SET_ERROR(TW_ERROR_SYSTEM_MEMORY_ALLOC);
		return false;
	}
	memset(oneData.data.pInitJson, 0, paraStr.size() + 1);
	strcpy(oneData.data.pInitJson, paraStr.c_str());

	FillFrameData(oneFrame, oneData);

	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);
	mpCmd->SendCmd(oneFrame);
	int ret = WaitRespond(crk);
	if (ret != 0)
	{
		TW_SET_ERROR(ret);
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_SetDevice_L
//
// 功能描述:进行硬件设置的相关操作，例如扫码超时、IP信息、屏幕控制
//
// 输入参数:const std::string& paraStr, int paraType,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_SetDevice_L( const std::string& paraStr, int paraType )
{
	CInterfaceTraceInfo tmpLog("IF_SetDevice", paraStr);
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}
	std::string typeStr;
	std::vector<std::string> names;
	names.push_back("Type");
	if (!JsonGetTargetValue(paraStr, names, typeStr))
	{
		TW_TRACE_DIARY_TYPE("硬件设置接口:参数查找失败-" + paraStr, TW_DIARY_MSG_THREAD_API);
		return false;
	}
	int convType = ConvResponseType(Str2Int(typeStr));

	std::map<std::string, std::string> paraMap;
	if (!Json2MapData(paraStr, convType, paraMap))
	{
		TW_TRACE_DIARY_TYPE("硬件设置接口:参数转换失败-" + paraStr, TW_DIARY_MSG_THREAD_API);
		return false;
	}
	std::list<TWProtocol_BaseFrame> cmds;
	if (!GenerateDevieSetFrames(paraMap, convType, cmds))
	{
		return false;
	}
	for (std::list<TWProtocol_BaseFrame>::iterator it = cmds.begin(); it != cmds.end(); ++it)
	{
		TWCmdKey crk = mpCmd->CalcCmdKey(*it);
		mpCmd->SendCmd(*it);
		this->QuerySendResult(crk);
	}
	return true;

}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_QueryDevice_L
//
// 功能描述:查询硬件信息，例如扫码超时、Ip信息
//
// 输入参数:int queryType, int resultType, std::string& resultStr,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_QueryDevice_L( int queryType, int resultType, std::string& resultStr )
{
	CInterfaceTraceInfo tmpLog("IF_QueryDevice");
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}
	bool flag ;
	std::map<std::string, std::string> paraMap;
	if (queryType == 1)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_AUTHCONTROL, TWPC_A_AUTHCONTROL_CHECK, GetNextSN());
		TWProtocolData_A_Auth oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		return true;
	}
	if (queryType == 5)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_LASTERROR, GetNextSN());
		TWProtocolData_A_SendData oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		return true;
	}
	else if (queryType == 101)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_BEAT, TWPC_A_BEAT_HANDSHAKE, GetNextSN());
		TWProtocolData_A_Beat oneData(oneFrame);
		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		return true;
	}

	else if (queryType == 102)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_HARDVERSION, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);


		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);
		return flag;

	}

	else if (queryType == 103)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_CANID, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);
		return flag;

	}

	else if (queryType == 104)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_MAX, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);


		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);
		return flag;
	}

	else if (queryType == 105)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_FIRMWARE, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);


		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);
		return flag;

	}

	else if (queryType == 106)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SCANTIME, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);


		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);
		return flag;
	}

	else if (queryType == 107)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ID, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);


		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);

		return flag;
	}

	else if (queryType == 108)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_BOOTSCREEN, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);


		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);

		return flag;
	}

	else if (queryType == 109)
	{
		TWProtocol_BaseFrame oneFrame1(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ETH0IP, GetNextSN());
		TWProtocol_BaseFrame oneFrame2(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ETH0MASK, GetNextSN());
		TWProtocol_BaseFrame oneFrame3(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ETH0GATEWAY, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData1(oneFrame1), oneData2(oneFrame2), oneData3(oneFrame3);

		flag = FillFrameData(oneFrame1, oneData1);
		flag = flag ? FillFrameData(oneFrame2, oneData2) : flag;
		flag = flag ? FillFrameData(oneFrame3, oneData3) : flag;




		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame1);
		flag = flag ? mpCmd->SendCmd(oneFrame1) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}

		CScopeLock bLock(&mRespondMutex);//获取结果数据
		std::string str1, str2, str3;
		flag =  MakeReturnJsonString(mBHardwareConfigDataQueue, crk, str1);
		resultStr = str1;
		return flag;
	}

	else if (queryType == 110)
	{
		TWProtocol_BaseFrame oneFrame1(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ETH1IP, GetNextSN());
		TWProtocol_BaseFrame oneFrame2(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ETH1MASK, GetNextSN());
		TWProtocol_BaseFrame oneFrame3(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_ETH1GATEWAY, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData1(oneFrame1), oneData2(oneFrame2), oneData3(oneFrame3);


		flag = FillFrameData(oneFrame1, oneData1);
		flag = flag ? FillFrameData(oneFrame2, oneData2) : flag;
		flag = flag ? FillFrameData(oneFrame3, oneData3) : flag;




		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame1);
		flag = flag ? mpCmd->SendCmd(oneFrame1) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}

		CScopeLock bLock(&mRespondMutex);//获取结果数据
		std::string str1, str2, str3;
		flag =  MakeReturnJsonString(mBHardwareConfigDataQueue, crk, str1) ;
		//  flag =flag ? MakeReturnJsonString(mBHardwareConfigDataQueue, oneFrame2, str2):flag;
		//flag =flag ? MakeReturnJsonString(mBHardwareConfigDataQueue, oneFrame3, str3):flag;
		///flag =flag ? MergeIPJsonString(str1, str2, str3,resultStr):flag;
		resultStr = str1;

		return flag;
	}

	else if (queryType == 111)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_HARDWARETEMP, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}

		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);

		return flag;
	}

	else if (queryType == 112)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_HARDWARETIME, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		TWCmdKey  crk = mpCmd->CalcRespondKey(oneFrame);
		flag = flag ? mpCmd->SendCmd(oneFrame) : flag ;
		if (!flag)
		{
			return false;
		}
		int ret = WaitRespond(crk);
		if (ret != 0)
		{
			TW_SET_ERROR(ret);
			return false;
		}
		CScopeLock bLock(&mRespondMutex);//获取结果数据
		flag = MakeReturnJsonString(mBHardwareConfigDataQueue, crk, resultStr);

		return flag;
	}
	return false;
}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_StartDebit_L
//
// 功能描述:进行扣款操作
//
// 输入参数:const std::string& paraStr, int paraType, std::string& resultStr,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_StartDebit_L( const std::string& paraStr, int paraType, std::string& resultStr )
{
	CInterfaceTraceInfo tmpLog("IF_StartDebit", paraStr);

	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}

	std::map<std::string, std::string> paraMap;

	if (!Json2MapData(paraStr, BUSINESSCONV_STARTPAY, paraMap))
	{
		TW_TRACE_DIARY_TYPE("扣款接口:参数转换失败-" + paraStr, TW_DIARY_MSG_THREAD_API);
		TW_SET_ERROR(OP_PARAM_ERR);
		return false;
	}

	bool flag;
	TWProtocol_BaseFrame oneFrame(TWPC_A_STARTPAY, TWPC_A_STARTPAY_PAYBLOCK, GetNextSN());

	TWProtocolData_A_StartPay oneA(oneFrame);

	strncpy(oneA.data.PayBlock.TransactionIDCode, paraMap["PayIdentifier"].c_str(), 40);
	oneA.data.PayBlock.Money = Str2Int(paraMap["Money"]);
	oneA.data.PayBlock.VehTypeAndKind = Str2Int(paraMap["VehClass"]) + 10 * (Str2Int(paraMap["VehType"]) - 1);
	// oneA.data.PayBlock.VehType = Str2Int(paraMap["VType"]);
	// oneA.data.PayBlock.VehKind = Str2Int(paraMap["VKind"]);
	oneA.data.PayBlock.VehPlateColor = Str2Int(paraMap["VLColor"]);
	strncpy(oneA.data.PayBlock.Plate , paraMap["License"].c_str(), 16);
	oneA.data.PayBlock.Balance = Str2Int(paraMap["Balance"]);

	strncpy(oneA.data.PayBlock.EnStationNO, paraMap["StationID"].c_str(), 20);
	strncpy(oneA.data.PayBlock.EnStationName, paraMap["StationName"].c_str(), 24);
	oneA.data.PayBlock.EnTime = Str2Datetime(paraMap["Time"]);
	oneA.data.PayBlock.StaticDate = Str2Int(paraMap["TollDate"]);
	oneA.data.PayBlock.StaticShift = Str2Int(paraMap["ShiftID"]);
	//strncpy(oneA.data.PayBlock.CardID, paraMap["CardID"].c_str(), 10);

	std::string card_str = paraMap["CardID"];
	oneA.data.PayBlock.CardLen = card_str.size();
	if (card_str.size() % 2 != 0)
	{
		card_str = "0" + card_str;
	}
	if (card_str.size() > 20)
	{
		card_str = card_str.substr(0, 20);
		oneA.data.PayBlock.CardLen = 20;
	}
	for (int i = 0; i < (std::min)(10, (int)card_str.size() / 2); ++i)
	{
		char ch;
		sscanf(card_str.substr(2 * i, 2).c_str(), "%x", &ch);
		//下面的代码是为了实现卡号的右对齐，
		oneA.data.PayBlock.CardID[sizeof(oneA.data.PayBlock.CardID) - (oneA.data.PayBlock.CardLen + 1) / 2 + i] = ch;
	}



	//strncpy(oneA.data.PayBlock.CardID, paraMap["CardID"].c_str(), 10);
	//oneA.data.PayBlock.FlagStationCount = Str2Int(paraMap["FlagStationCount"]);
	oneA.data.PayBlock.AxleCount = Str2Int(paraMap["AxisCount"]);
	oneA.data.PayBlock.Weight = Str2Int(paraMap["Weight"]);
	// oneA.data.PayBlock.WeightLimit = Str2Int(paraMap["WeightLimit"]);
	oneA.data.PayBlock.DebitMode = Str2Int(paraMap["OperationMode"]);
	strncpy(oneA.data.PayBlock.OperatorID, paraMap["OperatorID"].c_str(), 20);
	strncpy(oneA.data.PayBlock.OperatorName, paraMap["OperatorName"].c_str(), 10);
	oneA.data.PayBlock.DebitOverTime =  Str2Int(paraMap["OverTime"]) / 1000; //传输的单位为毫秒
	oneA.data.PayBlock.ExTime = Str2Datetime(paraMap["SubTime"]);

	//add 2018-05
	strncpy(oneA.data.PayBlock.EntryPlate,paraMap["EntryLicense"].c_str(),sizeof(oneA.data.PayBlock.EntryPlate));
	oneA.data.PayBlock.EntryPlateColor = Str2Int(paraMap["EntryVColor"]);
	oneA.data.PayBlock.PassCertificateType =  Str2Int(paraMap["PassCertificateType"]);
	strncpy(oneA.data.PayBlock.AutoPlate,paraMap["AutoPlate"].c_str(),sizeof(oneA.data.PayBlock.AutoPlate));
	oneA.data.PayBlock.AutoPlateColor =  Str2Int(paraMap["AutoPlateColor"]);
	strncpy(oneA.data.PayBlock.TickNo,paraMap["TickNo"].c_str(),sizeof(oneA.data.PayBlock.TickNo));

	if(paraMap.find("PayModel")!=paraMap.end())
	{
		oneA.data.PayBlock.PayModel = Str2Int(paraMap["PayModel"]);
	}
	


	mLastPayId = paraMap["PayIdentifier"];
	mWaitBPayOverTime = oneA.data.PayBlock.DebitOverTime;

	if (std::find(mLatestCanclePayQueue.begin(), mLatestCanclePayQueue.end(), mLastPayId) != mLatestCanclePayQueue.end())
	{
		TW_SET_ERROR(TW_ERROR_OPERATOR_CANCLE);
		return false;
	}

	mBreakCurrentDebit = false;
	flag = FillFrameData(oneFrame, oneA);
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);

	//return false;
	//mpCmd->SendCmd(oneFrame);
	mpCmd->SendVIPCmd(oneFrame);//modify 2018-04
	mLastPayResponseCrk = crk;

	{
		CScopeLock bLock(&mRespondMutex);  //获取结果数据
		mRespondRecord.erase(crk);//有一定的概率，终端会返回两次扣款结果到中间件，如果此处不清理，会导致扣款结果错位
	}

	int ret = WaitRespond(crk, mWaitBPayOverTime);
	if (ret != 0)
	{
		TW_SET_ERROR(ret);
		return false;
	}
	CScopeLock bLock(&mRespondMutex);  //获取结果数据
	flag = MakeReturnJsonString(mBStartPayDataQueue, crk, resultStr);
	return flag;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_BreakDebit_L
//
// 功能描述:终止交易接口
//
// 输入参数:const std::string& payID,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_BreakDebit_L(const std::string& payID)
{
	CInterfaceTraceInfo tmpLog("IF_BreakDebit", payID);

	mLatestCanclePayQueue.push_front(payID);
	while (mLatestCanclePayQueue.size() > 40)
	{
		mLatestCanclePayQueue.pop_back();
	}
	if (payID != mLastPayId)
	{
		TW_TRACE_DIARY_TYPE("终止交易接口:与最近交易标识符不相同，暂不处理", TW_DIARY_MSG_THREAD_API);
		return false;
	}
	mBreakCurrentDebit = true;
	TW_TRACE_DIARY_TYPE("终止交易接口:已经发出中断信号", TW_DIARY_MSG_THREAD_API);

	Sleep(100);
	return true;
}

bool CCoreBusiness_Future::IF_CheckDeviceStatus_L(DWORD& dwState)
{
	CInterfaceTraceInfo tmpLog("IF_CheckDeviceStatus");
	if ( (time(NULL) - mLastReadFrameTime) >= 20)
	{
		return false;//长时间没有接收到帧，认为设备已经断开。
	}
	dwState = mHardwareStatus;
	return true;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_StartDebitCancle_L
//
// 功能描述:进行撤单操作
//
// 输入参数:const std::string& paraStr,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_StartDebitCancle_L( const std::string& paraStr )
{
	CInterfaceTraceInfo tmpLog("IF_StartDebitCancle", paraStr);
	CScopeLock accessL(&mInterfaceMutex);
	CScopeLock accessL_Cancle(&mDebitCancleMutex);
	if (!mInitResult)
	{
		return false;
	}

	std::map<std::string, std::string> paraMap;
	if (!Json2MapData(paraStr, BUSINESSCONV_CANCLEPAY, paraMap))
	{
		TW_TRACE_DIARY_TYPE("撤单接口:参数转换失败-" + paraStr, TW_DIARY_MSG_THREAD_API);
		return false;
	}

	bool flag;
	CDVDebitCancel cdc;
	cdc.PayIdentifier = paraMap["PayIdentifier"].c_str();

	cdc.CreateTime = CTime::GetCurrentTime();

	if (!mDebitCancleEnsure.Save(cdc))
	{
		TW_TRACE_DIARY_TYPE("撤单接口:保存撤单数据失败", TW_DIARY_MSG_THREAD_API);
		return false;
	}


	TWProtocol_BaseFrame oneFrame(TWPC_A_CANCLE, TWPC_A_CANCLE_JSON, GetNextSN());
	TWProtocolData_A_Cancle oneA(oneFrame);

	//strncpy(oneA.DataBlock.PayCode, paraMap["PayIdentifier"].c_str(), 32);
	oneA.data.pDataJson = new char[paraStr.size() + 1];
	if (oneA.data.pDataJson == NULL)
	{
		return false;
	}

	strcpy(oneA.data.pDataJson, paraStr.c_str());

	flag = FillFrameData(oneFrame, oneA);
	TWCmdKey crk = mpCmd->CalcCmdKey(oneFrame);

	mpCmd->SendCmd(oneFrame);


	bool ret = QuerySendResult(crk);//WaitRespond(crk);

	if (!ret)
	{
		TW_TRACE_DIARY_TYPE("撤单接口:发起撤单失败", TW_DIARY_MSG_THREAD_API);
		//TW_SET_ERROR(ret);
		return false;
	}

	TW_TRACE_DIARY_TYPE("撤单接口:发起撤单成功", TW_DIARY_MSG_THREAD_API);
	mDebitCancleEnsure.UpdateProcessMark(paraMap["PayIdentifier"].c_str());//cdc.PayIdentifier);
	mDebitCancleEnsure.Delete();

	return flag;
}


bool CCoreBusiness_Future::GreenChannelTest_L(int testType,const std::string& para,std::string& testResult)
{
	
	CInterfaceTraceInfo tmpLog("GreenChannelTest", para);
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		return false;
	}
	if(testType==1)
	{//获取NFC参数
		
		TWProtocol_BaseFrame oneFrame(TWPC_A_GREENCHANNEL, TWPC_A_GREENCHANNEL_NFC, GetNextSN());


		TWProtocolData_A_GreenChannel oneA(oneFrame);

		FillFrameData(oneFrame, oneA);

		TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);
		mpCmd->SendCmd(oneFrame);
		int ret = WaitRespond(crk);

		if (ret == TW_ERROR_WAIT_RESULT )
		{
			TW_TRACE_DIARY_TYPE("GreenChannelTest查询失败", TW_DIARY_MSG_THREAD_API);
			TW_SET_ERROR(ret);
			return false;
		}
	
		CScopeLock bLock(&mRespondMutex);  //获取结果数据
		MakeReturnJsonString(mBGreenChannelDataQueue, crk, testResult);
		
	}
	return true;
}

/**
 * 接口实现
 */

/**
 * 继承实现
 */

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::SendRemainDebitCancle
//
// 功能描述:重发
//
// 输入参数:const int& callCount,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::SendRemainDebitCancle(const int& callCount)
{
	static time_t lastSentTime = 0;
	if (lastSentTime == 0)
	{
		lastSentTime = time(NULL);
		return false;
	}
	time_t currTime = time(NULL);
	if ( (currTime - lastSentTime) / (30) < 1 )
	{
		return false;//30s更新一次
	}
	TW_TRACE_DIARY_TYPE("查询未撤单信息", TW_DIARY_MSG_THREAD_MAIN);
	lastSentTime = currTime;
	CScopeLock accessL_Cancle(&mDebitCancleMutex);

	CDVDebitCancel cdc;

	if (!mDebitCancleEnsure.Retrieval(cdc))
	{
		return false;
	}

	bool flag;
	TWProtocol_BaseFrame oneFrame(TWPC_A_CANCLE, TWPC_A_CANCLE_JSON, GetNextSN());
	TWProtocolData_A_Cancle oneA(oneFrame);

	//strncpy(oneA.data.DataBlock.PayCode, paraMap["PayIdentifier"].c_str(), 32);
	std::string paraStr;
	MakeDebitCancleJsonStr(LPCSTR(cdc.PayIdentifier), paraStr, TWPD_A_CANCLE_FEEDBACK_NO);

	TW_TRACE_DIARY_TYPE("重发撤单信息:" + std::string(LPCSTR(cdc.PayIdentifier)), TW_DIARY_MSG_THREAD_MAIN);
	oneA.data.pDataJson = new char[paraStr.size() + 1];
	if (oneA.data.pDataJson == NULL)
	{
		return false;
	}
	strcpy(oneA.data.pDataJson, paraStr.c_str());
	flag = FillFrameData(oneFrame, oneA);

	TWCmdKey crk = mpCmd->CalcCmdKey(oneFrame);
	mpCmd->SendCmd(oneFrame);
	bool sendRet = QuerySendResult(crk);//mpCmd->QuerySendResult(crk);
	if (sendRet)
	{
		TW_TRACE_DIARY_TYPE("重发撤单成功:" + std::string(LPCSTR(cdc.PayIdentifier)), TW_DIARY_MSG_THREAD_MAIN);
		mDebitCancleEnsure.UpdateProcessMark(cdc.PayIdentifier);
		mDebitCancleEnsure.Delete();
	}

	return true;
}
BOOL CCoreBusiness_Future::InitInstance()
{
	return true;
}
int CCoreBusiness_Future::ExitInstance()
{
	return CWinThread::ExitInstance();
}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::Stop
//
// 功能描述:用于停止主线程
//
// 输入参数:
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::Stop()
{
	TW_TRACE_DIARY_TYPE("停止业务主线程" , TW_DIARY_MSG_THREAD_MAIN);
	if (this->m_bExit)
	{
		TW_TRACE_DIARY_TYPE("业主主线程已经停止" , TW_DIARY_MSG_THREAD_MAIN);
		return ;
	}
	this->m_bExit = true;
	::PostThreadMessage(this->m_nThreadID, WM_QUIT, 0, 0);
	//::WaitForSingleObject(this->m_hThread,INFINITE);//直接使用等待可能无限等待，故使用下面的方法MsgWaitForMultipleObjects
	DWORD dRet;
	MSG msg;
	TW_TRACE_DIARY_TYPE("等待业务主线程结束" , TW_DIARY_MSG_THREAD_MAIN);
	while (1)
	{
		dRet =::MsgWaitForMultipleObjects(1, &this->m_hThread, FALSE, INFINITE, QS_ALLINPUT);

		if (dRet == WAIT_OBJECT_0 + 1)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			break;
		}
	}
	TW_TRACE_DIARY_TYPE("业务主线程停止成功" , TW_DIARY_MSG_THREAD_MAIN);

}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::OnIdle
//
// 功能描述:主处理内容
//
// 输入参数:LONG lCount,
// 输出参数:BOOL
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
BOOL CCoreBusiness_Future::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return false;
	}

	ReadFrame();
	HandleRecvFrameMessage();
	SendRemainDebitCancle(lCount);
	CheckAlive();
	ClearRespondRecord();
	Sleep(10);
	return true;
}
void CCoreBusiness_Future::CheckAlive()
{
	static time_t last_check_time = 0;
	time_t currTime = time(NULL);
	if ( (currTime - last_check_time) / (5 * 60) < 1 )
	{
		return ;
	}
	last_check_time = currTime;
	TW_TRACE_DIARY_TYPE("线程存活检测日志记录", TW_DIARY_MSG_THREAD_MAIN);
}


/**
 * 内部实现内容
 */
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::SetDefault
//
// 功能描述:设置系统默认参数。
//
// 输入参数:
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:设置默认结果，
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::SetDefault()
{

	mProvinceID = 0;
	// mCommunicationOverTime = 0;
	mWaitBOverTime = 2;
	mWaitBPayOverTime = 2;
	mInitResult = false;
	mHasInit = false;
	mHardwareStatus = 0;
	m_bExit = false;
	mFrameSN = 0;
	mTerminate = false;

	mLastReadFrameTime = 0;


//    mIOCore.SetEventOwner(this, 123);
	std::string path = "" ;
	mDebitCancleEnsure.SetDBFilePath(SystemParaDBPath().c_str());
	TW_TRACE_DIARY_TYPE("数据库地址:" + SystemParaDBPath(), TW_DIARY_MSG_THREAD_MAIN);
	mDebitCancleEnsure.SetLastDelayTime(30);
}



//将串口原始数据队列的内容转换为协议帧格式的内容
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::ReadFrame
//
// 功能描述:从串口数据中读取一帧内容
//
// 输入参数:
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::ReadFrame()
{
	CScopeLock dataToLock(&mFrameMutex);
	TWProtocol_BaseFrame oneFrame;
	while (mpCmd->TakeCmd(oneFrame))
	{
		mFrameQueue.push_back(oneFrame);
		mLastReadFrameTime = time(NULL);
	}
	return ;
}


//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleRecvFrameMessage
//
// 功能描述:处理收到的帧信息，此处处理的都是完整的帧信息。
//
// 输入参数:
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleRecvFrameMessage()
{
	CScopeLock frameLock(&mFrameMutex);//为了防止阻塞，此处可以使用swap处理
	while (!mFrameQueue.empty())
	{
		TWProtocol_BaseFrame eFrame = mFrameQueue.front();
		mFrameQueue.pop_front();
		switch (eFrame.TOPCMD)
		{
		case TWPC_B_BEAT:
			HandleFrame_B_Beat(eFrame);
			break;
		case TWPC_B_INTERACTIVE:
			HandleFrame_B_Interactive(eFrame);
			break;
		case TWPC_B_STARTPAY:
			HandleFrame_B_StartPay(eFrame);
			break;
		case TWPC_B_CANCLE:
			HandleFrame_B_CANCLE(eFrame);
			break;
		case TWPC_B_SENDDATA:
			HandleFrame_B_SendData(eFrame);
			break;
		case TWPC_B_HARDWARECONFIG:
			HandleFrame_B_HardwareConfig(eFrame);
			break;
		case TWPC_B_AUTHCONTROL:
			HandleFrame_B_Auth(eFrame);
			break;
		case TWPC_B_AGENCYDEBIT:
			HandleFrame_B_AgencyDebit(eFrame);
			break;
		case TWPC_B_GREENCHANNEL:
			HandleFrame_B_GreenChannel(eFrame);
			break;
		case TWPC_C_REPLY:
			HandleFrame_C_Reply(eFrame);
			break;
		case TWPC_D_QUERY:
			HandleFrame_D_Query(eFrame);
			break;
		case TWPC_D_SELFCHECK:
			HandleFrame_D_SelfCheck(eFrame);
			break;
		case 0xBE:
			NewFeedbackCommand(eFrame);
			break;
		case 0x11:
			Handle_11_Frame(eFrame);
			break;	
		case 0x12:
			Handle_12_Frame(eFrame);
			break;
		default:
			break;/*some error info */
		}

	}

}


//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_Beat
//
// 功能描述:处理心跳帧信息
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_Beat(const TWProtocol_BaseFrame& info)
{
	TWProtocolData_B_Beat normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}

	if (info.SUBCMD == TWPC_B_BEAT_AUTO
	        || info.SUBCMD == TWPC_B_BEAT_FEEDBACK
	   )
	{

		mHardwareStatus = normalData.data.BeatInfo.DeviceStatus;
		//add zhuyabing 2017-08 由于协议文档中的设备状态含义和规划一（提供给外部的接口）中的设备状态含义不一致，此处需要作转换
		std::bitset<sizeof(unsigned int) * 8> result_b;
		std::bitset<sizeof(unsigned int) * 8> dev_b(normalData.data.BeatInfo.DeviceStatus);

		result_b[3] = dev_b[0];
		result_b[6] = dev_b[1];
		result_b[7] = dev_b[2];
		result_b[8] = dev_b[3];
		result_b[9] = dev_b[4];
		result_b[5] = dev_b[5];

		mHardwareStatus = result_b.to_ulong();


		if (info.SUBCMD == TWPC_B_BEAT_FEEDBACK)
		{
			CScopeLock bLock(&mRespondMutex);
			TWCmdKey  oneKey = mpCmd->CalcCmdKey(info);
			RespondResult rr;
			rr.resultCode = normalData.data.BeatInfo.Result;
			mRespondRecord[oneKey] = rr;
			mBBeatDataQueue[oneKey] = normalData;
		}

	}


}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_Interactive
//
// 功能描述:处理控制交互的帧信息
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_Interactive(const TWProtocol_BaseFrame& info)
{
	CScopeLock bLock(&mRespondMutex);
	TWProtocolData_B_Interactive normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}

	TWCmdKey  oneKey = mpCmd->CalcCmdKey(info);
	RespondResult rr;
	rr.resultCode = normalData.data.Result;
	mRespondRecord[oneKey] = rr;
	mBInterDataQueue[oneKey] = normalData;

}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_StartPay
//
// 功能描述:处理扣款结果帧信息
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_StartPay(const TWProtocol_BaseFrame& info)
{
	CScopeLock bLock(&mRespondMutex);
	TWProtocolData_B_StartPay normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}

	TWCmdKey  oneKey = mpCmd->CalcCmdKey(info);
	RespondResult rr;
	rr.resultCode = normalData.data.DataBlock.Result;
	mRespondRecord[oneKey] = rr;
	mBStartPayDataQueue[oneKey] = normalData;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_CANCLE
//
// 功能描述:处理撤单回馈
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_CANCLE(const TWProtocol_BaseFrame& info)
{

	TWProtocolData_B_CANCLE normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}



	//收到撤单回馈的时候，删除数据库中的记录
	std::string cancleID;
	std::string jsonVal = normalData.data.pDataJson;
	std::vector<std::string> names;
	names.push_back("DebitCancelParamContext");
	names.push_back("Data");
	names.push_back("PayIdentifier");
	if (!JsonGetTargetValue(jsonVal, names, cancleID))
	{
		TW_TRACE_DIARY("获取识别码失败");
		return ;
	}

	names.clear();
	names.push_back("DebitCancelParamContext");
	names.push_back("Data");
	names.push_back("ResultType");
	std::string retStr;
	if (!JsonGetTargetValue(jsonVal, names, retStr))
	{
		TW_TRACE_DIARY("获取错误码失败");
		return ;
	}
	if (retStr != "0")
	{
		int err = Str2Int(retStr);
		TW_TRACE_DIARY_TYPE("撤单失败，错误码:" + retStr, TW_DIARY_MSG_THREAD_MAIN);
		TW_SET_ERROR(err);
		return ;
	}


	TW_TRACE_DIARY_TYPE("收到撤单结果数据:" + cancleID, TW_DIARY_MSG_THREAD_MAIN);
	mDebitCancleEnsure.UpdateProcessMark(cancleID.c_str());
	mDebitCancleEnsure.Delete();

	CScopeLock bLock(&mRespondMutex);
	TWCmdKey  oneKey = mpCmd->CalcCmdKey(info);
	RespondResult rr;
	rr.resultCode = Str2Int(retStr);//normalData.data.DataBlock.Result;
	mRespondRecord[oneKey] = rr;
	mBCANCLEQueue[oneKey] = normalData;


}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_SendData
//
// 功能描述:
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-05-26
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_SendData(const TWProtocol_BaseFrame& info)
{

	CScopeLock bLock(&mRespondMutex);

	TWProtocolData_B_SendData normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}
	TWCmdKey crk = mpCmd->CalcCmdKey(info);
	//TW_TRACE_DIARY("B5："+Int2Str(crk));
	mBSendDataQueue[crk] = normalData;
	RespondResult rr;
	rr.resultCode = normalData.data.Result;
	mRespondRecord[crk] = rr;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_HardwareConfig
//
// 功能描述:
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_HardwareConfig(const TWProtocol_BaseFrame& info)
{

	CScopeLock bLock(&mRespondMutex);

	TWProtocolData_B_HardwareConfig normalData(info);
	// TW_TRACE_DIARY("收到硬件数据a");
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}
	// TW_TRACE_DIARY("收到硬件数据b");

	TWCmdKey crk = mpCmd->CalcCmdKey(info);
	mBHardwareConfigDataQueue[crk] = normalData;
	RespondResult rr;
	rr.resultCode = 0;//硬件设置都认为成功
	mRespondRecord[crk] = rr;

}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_B_Auth
//
// 功能描述:
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_B_Auth(const TWProtocol_BaseFrame& info)
{
	CScopeLock bLock(&mRespondMutex);
	TWProtocolData_B_Auth normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}


	TWCmdKey crk = mpCmd->CalcCmdKey(info);
	mBAuthDataQueue[crk] = normalData;
	RespondResult rr;
	rr.resultCode = 0;
	mRespondRecord[crk] = rr;
}


void CCoreBusiness_Future::HandleFrame_B_AgencyDebit(const TWProtocol_BaseFrame& info)
{
	CScopeLock bLock(&mRespondMutex);
	TWProtocolData_B_AgencyDebit normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}
	
	

	TWCmdKey  oneKey = mpCmd->CalcCmdKey(info);
	RespondResult rr;
	rr.resultCode = normalData.data.Result;
	mRespondRecord[oneKey] = rr;
	mBAgencyDebitDataQueue[oneKey] = normalData;
	//TW_TRACE_DIARY("BA:"+Int2Str(normalData.data.Result)+","+Int2Str(oneKey));

}

void CCoreBusiness_Future::HandleFrame_B_GreenChannel(const TWProtocol_BaseFrame& info)
{
	CScopeLock bLock(&mRespondMutex);
	TWProtocolData_B_GreenChannel normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}

	
	TWCmdKey  oneKey = mpCmd->CalcCmdKey(info);
	RespondResult rr;
	rr.resultCode =0;
	mRespondRecord[oneKey] = rr;
	mBGreenChannelDataQueue[oneKey] = normalData;
}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_C_Reply
//
// 功能描述:
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_C_Reply(const TWProtocol_BaseFrame& info)
{

	TWProtocolData_C_Reply normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		TW_TRACE_DIARY_TYPE("回馈数据不合法", TW_DIARY_MSG_THREAD_MAIN);
		return;/*some error info*/
	}
	if (info.SUBCMD == TWPC_C_REPLY_SUCCESS)
	{
		// TW_TRACE_DIARY_TYPE("收到接收回馈", TW_DIARY_MSG_THREAD_MAIN);
		// mReplyListen.erase(info.SN);
	}
	else if (info.SUBCMD == TWPC_C_REPLY_INVALIDFORMAT)
	{
		TW_TRACE_DIARY_TYPE("校验失败（标识长度错误）", TW_DIARY_MSG_THREAD_MAIN);
	}
	else if (info.SUBCMD == TWPC_C_REPLY_INVALIDDATACHECK)
	{
		TW_TRACE_DIARY_TYPE("校验失败（校验码错误)", TW_DIARY_MSG_THREAD_MAIN);
	}
	else if (info.SUBCMD == TWPC_C_REPLY_INVALIDDATAUNDEF  )
	{
		TW_TRACE_DIARY_TYPE("校验失败（未知错误）", TW_DIARY_MSG_THREAD_MAIN);
	}
	else if (info.SUBCMD == TWPC_C_REPLY_INVALIDCMD)
	{
		TW_TRACE_DIARY_TYPE("校验失败（主命令非法）", TW_DIARY_MSG_THREAD_MAIN);
	}

}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_D_Query
//
// 功能描述:
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_D_Query(const TWProtocol_BaseFrame& info)
{
	TWProtocolData_D_Query normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}
	if (info.SUBCMD == TWPC_D_QUERY_STATUS)
	{
		TWProtocol_BaseFrame eFrame;
		eFrame.TOPCMD = TWPC_E_QUERY;
		eFrame.SUBCMD = TWPC_E_QUERY_STATUS;
		eFrame.SN = info.SN;

		mpCmd->SendCmd(eFrame);

	}
	else if (info.SUBCMD == TWPC_D_QUERY_VERSION)
	{
		TWProtocol_BaseFrame eFrame;
		eFrame.TOPCMD = TWPC_E_QUERY;
		eFrame.SUBCMD = TWPC_E_QUERY_VERSION;
		eFrame.SN = info.SN;

		mpCmd->SendCmd(eFrame);

	}
	else
	{

	}
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::HandleFrame_D_SelfCheck
//
// 功能描述:
//
// 输入参数:const TWProtocol_BaseFrame& info,
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::HandleFrame_D_SelfCheck(const TWProtocol_BaseFrame& info)
{
	TWProtocolData_D_SelfCheck normalData(info);
	if (!FillProtocolStruct(normalData, info))
	{
		return;/*some error info*/
	}


	TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_INITJSON, GetNextSN());
	TWProtocolData_A_SendData oneData(oneFrame);

	/*更改初始化中的时间字段*/
	mInitJsonStr = ModifyInitStrTime(mInitJsonStr);

	oneData.data.pInitJson = new  char[mInitJsonStr.size() + 1];
	if (oneData.data.pInitJson == NULL)
	{
		return ;
	}
	memset(oneData.data.pInitJson, 0, mInitJsonStr.size() + 1);
	strcpy(oneData.data.pInitJson, mInitJsonStr.c_str());

	TW_TRACE_DIARY_TYPE("处理上电自检，发送扣费参数:" + mInitJsonStr, TW_DIARY_MSG_THREAD_MAIN);
	bool flag = FillFrameData(oneFrame, oneData);
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);
	mpCmd->SendCmd(oneFrame);
}
unsigned short CCoreBusiness_Future::GetNextSN()
{
	return mFrameSN++;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::WaitRespond
//
// 功能描述:等待某一个数据的回馈，
//
// 输入参数:TWCmdKey crk, int overTime,
// 输出参数:int
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
int CCoreBusiness_Future::WaitRespond(TWCmdKey crk, int overTime)
{
	int SleepUnit = 100;
	int SleepCount = overTime * 1000 / SleepUnit;
	for (int i = 0; i < SleepCount; ++i)
	{
		if (mTerminate) /*收到释放资源信号，直接退出*/
		{
			return TW_ERROR_WAIT_RESULT;
		}
		{
			CScopeLock tmpL(&mRespondMutex);
			std::map<TWCmdKey, RespondResult>::iterator it = mRespondRecord.find(crk);

			//TW_TRACE_DIARY_TYPE("find"+Int2Str(crk), TW_DIARY_MSG_THREAD_API);
			//TW_TRACE_DIARY("wait:"+Int2Str(crk));
			if (it != mRespondRecord.end())
			{
				int result = it->second.resultCode;
				mRespondRecord.erase(it);
			
				return result;
			}
		}
		if (mBreakCurrentDebit &&  crk == mLastPayResponseCrk)
		{
			TW_TRACE_DIARY_TYPE("扣款接口:外部调用撤单，停止等待交易结果", TW_DIARY_MSG_THREAD_API);
			return TW_ERROR_OPERATOR_CANCLE;
		}
		Sleep(SleepUnit);
	}
	return TW_ERROR_WAIT_RESULT;
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::ClearRespondRecord
//
// 功能描述:定期清理回馈内容
//
// 输入参数:
// 输出参数:void
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
void CCoreBusiness_Future::ClearRespondRecord()
{
	static time_t lastCheckTime = time(NULL);
	time_t currTime = time(NULL);
	if ( (currTime - lastCheckTime) / 60 < 10)
	{
		return ;/*一分钟检测一次*/
	}
	lastCheckTime = currTime;
	CScopeLock tmpL(&mRespondMutex);
	std::list<TWCmdKey> outDates;
	for (std::map<TWCmdKey, RespondResult>::const_iterator it = mRespondRecord.begin(); it != mRespondRecord.end(); ++it )
	{
		if ( (currTime - it->second.genTime) / 60 > 1)
		{
			outDates.push_back(it->first);
		}
	}
	for (std::list<TWCmdKey>::const_iterator it = outDates.begin(); it != outDates.end(); ++it)
	{
		mRespondRecord.erase(*it);
		mBBeatDataQueue.erase(*it);
		mBInterDataQueue.erase(*it);
		mBStartPayDataQueue.erase(*it);
		mBCANCLEQueue.erase(*it);
		mBSendDataQueue.erase(*it);
		mBHardwareConfigDataQueue.erase(*it);
		mBAuthDataQueue.erase(*it);
		mBAgencyDebitDataQueue.erase(*it);
	}
}
//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::QuerySendResult
//
// 功能描述:查询一个数据的发送结果，成功true，失败或者超时false
//
// 输入参数:TWCmdKey crk,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::QuerySendResult(TWCmdKey crk)
{
	int waitMSecs = 2 * 1000;
	int waitNumbers = 2 * 1000 / 100;
	int waitInterval = 100;
	for (int i = 0; i < waitNumbers; ++i)
	{
		if (mpCmd->QuerySendResult(crk))
		{
			return true;
		}
		Sleep(waitInterval);
	}
	return false;
}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::GenerateDevieSetFrames
//
// 功能描述:生成硬件信息设置内容
//
// 输入参数:std::map<std::string, std::string>& paraMap, int convType, std::list<TWProtocol_BaseFrame>& cmdList,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::GenerateDevieSetFrames(std::map<std::string, std::string>& paraMap, int convType, std::list<TWProtocol_BaseFrame>& cmdList)
{
	bool flag;
	if (convType == BUSINESSCONV_RESPONSE_201)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_BEAT, TWPC_A_BEAT_SET, GetNextSN());
		TWProtocolData_A_Beat oneBeat(oneFrame);

		oneBeat.data.BeatInterval = Str2Int(paraMap["Value"]);
		flag = FillFrameData(oneFrame, oneBeat);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;
	}
	else if (convType == BUSINESSCONV_RESPONSE_202)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETBOOTSCREENID, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		oneData.data.ScreenID.Val = Str2Int(paraMap["Value"]);

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;
	}

	else if (convType == BUSINESSCONV_RESPONSE_203)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_INTERACTIVE, TWPC_A_INTERACTIVE_INTERFACEBLOCK, GetNextSN());
		TWProtocolData_A_Interactive oneData(oneFrame);

		/*
		oneData.data.pInteractionJson = new char[paraStr.size() + 1];
		if (oneData.data.pInteractionJson == NULL)
		{
		    return false;
		}
		strcpy(oneData.data.pInteractionJson, paraStr.c_str());
		*/
		oneData.data.InteractionBlock.Money = Str2Int(paraMap["Money"]);
		oneData.data.InteractionBlock.VehType = Str2Int(paraMap["VType"]);
		oneData.data.InteractionBlock.VehKind = Str2Int(paraMap["VClass"]);
		oneData.data.InteractionBlock.VehPlateColor = Str2Int(paraMap["VColor"]);
		strncpy(oneData.data.InteractionBlock.Plate , paraMap["License"].c_str(), 16);
		oneData.data.InteractionBlock.Balance = Str2Int(paraMap["Balance"]);
		strncpy(oneData.data.InteractionBlock.EnStationName, paraMap["StationName"].c_str(), 24);
		oneData.data.InteractionBlock.FlagStationCount = Str2Int(paraMap["CroseNum"]);
		oneData.data.InteractionBlock.AxleCount = Str2Int(paraMap["AxisCount"]);
		oneData.data.InteractionBlock.Weight = Str2Int(paraMap["Weight"]);
		oneData.data.InteractionBlock.WeightLimit = Str2Int(paraMap["LimitWeight"]);
		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;
	}

	else if (convType == BUSINESSCONV_RESPONSE_204)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_INTERACTIVE, TWPC_A_INTERACTIVE_BYE, GetNextSN());
		TWProtocolData_A_Interactive oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;


	}
	else if (convType == BUSINESSCONV_RESPONSE_205)
	{
		TW_SET_ERROR(TW_ERROR_OPERATION_NOT_SUPPORTED);
		return false;
	}

	else if (convType == BUSINESSCONV_RESPONSE_206)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_LANETIME, GetNextSN());
		TWProtocolData_A_SendData oneData(oneFrame);
		oneData.data.LaneUnixTime = Str2Datetime(paraMap["Value"]);//1492244331;

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;

	}
	else if (convType == BUSINESSCONV_RESPONSE_207)
	{
		TWProtocol_BaseFrame oneFrame1(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETETH0IP, GetNextSN());
		TWProtocol_BaseFrame oneFrame2(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETETH0MASK, GetNextSN());
		TWProtocol_BaseFrame oneFrame3(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETETH0GATEWAY, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData1(oneFrame1), oneData2(oneFrame2), oneData3(oneFrame3);
		unsigned int ipInt = String2IP(paraMap["IP"]);
		unsigned int maskInt = String2IP(paraMap["Mask"]);
		unsigned int gatewayInt = String2IP(paraMap["Gateway"]);



		oneData1.data.IPInfo.IPVer = TWPD_A_HARDWARECONFIG_IPV4;
		strncpy((char*)oneData1.data.IPInfo.IP, paraMap["IP"].c_str(), 15);
		// oneData1.IPInfo.IP[0] = HIBYTE(HIWORD(ipInt));
		// oneData1.IPInfo.IP[1] = LOBYTE(HIWORD(ipInt));
		// oneData1.IPInfo.IP[2] = HIBYTE(LOWORD(ipInt));
		// oneData1.IPInfo.IP[3] = LOBYTE(LOWORD(ipInt));

		// oneData2.IPInfo.IPVer = TWPD_A_HARDWARECONFIG_IPV4;
		// oneData2.IPInfo.IP[0] = HIBYTE(HIWORD(maskInt));
		// oneData2.IPInfo.IP[1] = LOBYTE(HIWORD(maskInt));
		// oneData2.IPInfo.IP[2] = HIBYTE(LOWORD(maskInt));
		// oneData2.IPInfo.IP[3] = LOBYTE(LOWORD(maskInt));

		// oneData3.IPInfo.IPVer = TWPD_A_HARDWARECONFIG_IPV4;
		// oneData3.IPInfo.IP[0] = HIBYTE(HIWORD(gatewayInt));
		// oneData3.IPInfo.IP[1] = LOBYTE(HIWORD(gatewayInt));
		// oneData3.IPInfo.IP[2] = HIBYTE(LOWORD(gatewayInt));
		// oneData3.IPInfo.IP[3] = LOBYTE(LOWORD(gatewayInt));

		flag = FillFrameData(oneFrame1, oneData1);

		if (flag)
		{
			cmdList.push_back(oneFrame1);
		}
		else
		{
			return false;
		}
		return true;
	}


	else if (convType == BUSINESSCONV_RESPONSE_208)
	{
		TWProtocol_BaseFrame oneFrame1(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETETH1IP, GetNextSN());
		TWProtocol_BaseFrame oneFrame2(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETETH1MASK, GetNextSN());
		TWProtocol_BaseFrame oneFrame3(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETETH1GATEWAY, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData1(oneFrame1), oneData2(oneFrame2), oneData3(oneFrame3);
		unsigned int ipInt = String2IP(paraMap["IP"]);
		unsigned int maskInt = String2IP(paraMap["Mask"]);
		unsigned int gatewayInt = String2IP(paraMap["Gateway"]);



		oneData1.data.IPInfo.IPVer = TWPD_A_HARDWARECONFIG_IPV4;
		oneData1.data.IPInfo.IP[0] = HIBYTE(HIWORD(ipInt));
		oneData1.data.IPInfo.IP[1] = LOBYTE(HIWORD(ipInt));
		oneData1.data.IPInfo.IP[2] = HIBYTE(LOWORD(ipInt));
		oneData1.data.IPInfo.IP[3] = LOBYTE(LOWORD(ipInt));

		oneData2.data.IPInfo.IPVer = TWPD_A_HARDWARECONFIG_IPV4;
		oneData2.data.IPInfo.IP[0] = HIBYTE(HIWORD(maskInt));
		oneData2.data.IPInfo.IP[1] = LOBYTE(HIWORD(maskInt));
		oneData2.data.IPInfo.IP[2] = HIBYTE(LOWORD(maskInt));
		oneData2.data.IPInfo.IP[3] = LOBYTE(LOWORD(maskInt));

		oneData3.data.IPInfo.IPVer = TWPD_A_HARDWARECONFIG_IPV4;
		oneData3.data.IPInfo.IP[0] = HIBYTE(HIWORD(gatewayInt));
		oneData3.data.IPInfo.IP[1] = LOBYTE(HIWORD(gatewayInt));
		oneData3.data.IPInfo.IP[2] = HIBYTE(LOWORD(gatewayInt));
		oneData3.data.IPInfo.IP[3] = LOBYTE(LOWORD(gatewayInt));

		flag = FillFrameData(oneFrame1, oneData1);
		flag = flag ? FillFrameData(oneFrame2, oneData2) : flag;
		flag = flag ? FillFrameData(oneFrame3, oneData3) : flag;




		if (flag)
		{
			cmdList.push_back(oneFrame1);
			cmdList.push_back(oneFrame2);
			cmdList.push_back(oneFrame3);
		}
		else
		{
			return false;
		}
		return true;
	}

	else if (convType == BUSINESSCONV_RESPONSE_301)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETMAX, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		oneData.data.MaxInfo.Batch[0] = paraMap["Batch"].size() > 0 ? paraMap["Batch"][0] : 0;
		oneData.data.MaxInfo.Batch[1] = paraMap["Batch"].size() > 1 ? paraMap["Batch"][1] : 0;
		oneData.data.MaxInfo.Batch[2] = paraMap["Batch"].size() > 2 ? paraMap["Batch"][2] : 0;
		oneData.data.MaxInfo.Batch[3] = paraMap["Batch"].size() > 3 ? paraMap["Batch"][3] : 0;
		strncpy(oneData.data.MaxInfo.BurnTime, paraMap["BurnTime"].c_str(), 14);

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;

	}

	else if (convType == BUSINESSCONV_RESPONSE_302)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETCANID, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		oneData.data.CANID.Value = Str2Int(paraMap["Value"]) ;

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;

	}

	else if (convType == BUSINESSCONV_RESPONSE_303)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETID, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		strncpy(oneData.data.DeviceID.Val, paraMap["Value"].c_str(), 20);

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;

	}

	else if (convType == BUSINESSCONV_RESPONSE_304)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_SETSCANTIME, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		oneData.data.ScanTime.TimeVal = Str2Int(paraMap["Value"]);

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;

	}

	else if (convType == BUSINESSCONV_RESPONSE_305)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_TESTLED, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;

	}

	else if (convType == BUSINESSCONV_RESPONSE_306)
	{
		TWProtocol_BaseFrame oneFrame(TWPC_A_HARDWARECONFIG, TWPC_A_HARDWARECONFIG_DATATRANS, GetNextSN());
		TWProtocolData_A_HARDWARECONFIG oneData(oneFrame);

		oneData.data.DataTrans.PortNO = Str2Int(paraMap["PortNo"]);
		oneData.data.DataTrans.pContent = new char[paraMap["TransData"].size() + 1];
		if (oneData.data.DataTrans.pContent == NULL)
		{
			TW_SET_ERROR(TW_ERROR_SYSTEM_MEMORY_ALLOC);
			return false;
		}
		strcpy(oneData.data.DataTrans.pContent, paraMap["TransData"].c_str());

		flag = FillFrameData(oneFrame, oneData);

		if (flag)
		{
			cmdList.push_back(oneFrame);
		}
		else
		{
			return false;
		}
		return true;
	}
	return false;
}




bool CCoreBusiness_Future::IF_PlaySound_L( const std::string& paraStr, int paraType )
{
	std::string soundStr;
	if (!ParseSoundPara(paraStr, soundStr))
	{
		return false;
	}

	TWProtocol_BaseFrame oneFrame(TWPC_A_INTERACTIVE, TWPC_A_INTERACTIVE_TTS, GetNextSN());
	TWProtocolData_A_Interactive oneData(oneFrame);

	oneData.data.TTS.contentLen = soundStr.size();
	oneData.data.TTS.content = new char[soundStr.size() + 1];
	if (oneData.data.TTS.content == NULL)
	{
		return false;
	}
	memset(oneData.data.TTS.content, 0, soundStr.size() + 1);
	strcpy(oneData.data.TTS.content, soundStr.c_str());

	if (!FillFrameData(oneFrame, oneData))
	{
		return false;
	}
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);
	mpCmd->SendCmd(oneFrame);

	return true;
}


bool CCoreBusiness_Future::IF_ShowText_L( const std::string& paraStr, int paraType )
{
	TWProtocol_BaseFrame oneFrame(TWPC_A_INTERACTIVE, TWPC_A_INTERACTIVE_FREE_DISPLAY, GetNextSN());
	TWProtocolData_A_Interactive oneData(oneFrame);
	if (!ParseFreeTextPara(paraStr, oneData));

	if (!FillFrameData(oneFrame, oneData))
	{
		return false;
	}
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);
	mpCmd->SendCmd(oneFrame);

	return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:CCoreBusiness_Future::IF_AgencyDebitQuery_L
//
// 功能描述:进行代扣查询
//
// 输入参数:IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2018-01-08
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool CCoreBusiness_Future::IF_AgencyDebitQuery_L(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult)
{
	CInterfaceTraceInfo tmpLog("IF_AgencyDebitQuery", szParamContext);
	
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}

	bool flag;
	TWProtocol_BaseFrame oneFrame(TWPC_A_AGENCYDEBIT, TWPC_A_AGENCYDEBIT_QUERY, GetNextSN());

	TWProtocolData_A_AgencyDebit oneA(oneFrame);
	if (!Json2BusinessData(szParamContext, oneA))
	{
		TW_TRACE_DIARY_TYPE("agency query接口:参数转换失败-" + szParamContext, TW_DIARY_MSG_THREAD_API);
		TW_SET_ERROR(OP_PARAM_ERR);
		return false;
	}

	flag = FillFrameData(oneFrame, oneA);
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);

	mpCmd->SendCmd(oneFrame);
	{
		CScopeLock bLock(&mRespondMutex);  //获取结果数据
		mRespondRecord.erase(crk);//有一定的概率，终端会返回两次扣款结果到中间件，如果此处不清理，会导致扣款结果错位
	}

	int ret = WaitRespond(crk);
	if(TW_ERROR_WAIT_RESULT == ret)
	{
		TW_SET_ERROR(ret);
		return false;
	}
	iResult = ret;
	return true;
}



bool CCoreBusiness_Future::IF_TransferImage_L(IN const std::string& image_para)
{
    /**
     * modify 2018-04
     * 增加对清晰图像的处理，
     */
	CInterfaceTraceInfo tmpLog("IF_TransferImage",image_para);
	
	CScopeLock accessL(&mInterfaceMutex);
	if(!SystemParaEnableTransferImage())
	{
		TW_TRACE_DIARY_TYPE("IF_TransferImage-disable image transfer", TW_DIARY_MSG_THREAD_API);
		TW_SET_ERROR(OP_CONFIG_DISABLE_ERR);
		return false;
	}
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}
	std::string FileProp;
	std::vector<unsigned char> ByteArray;
	if(!ParseImageFile(image_para,ByteArray,FileProp))
	{
		TW_TRACE_DIARY_TYPE("IF_TransferImage-解析失败", TW_DIARY_MSG_THREAD_API);
		return false;
	}

	return TransferBigFile(ByteArray,FileProp );

	//废弃代扣图像传输接口

	bool flag;
	TWProtocol_BaseFrame oneFrame(TWPC_A_AGENCYDEBIT, TWPC_A_AGENCYDEBIT_TRANSFERIMAGE, GetNextSN());

	TWProtocolData_A_AgencyDebit oneA(oneFrame);
	if (!Json2BusinessData(image_para, oneA))
	{
		TW_TRACE_DIARY_TYPE("IF_TransferImage接口:参数转换失败-" + image_para, TW_DIARY_MSG_THREAD_API);
		TW_SET_ERROR(OP_PARAM_ERR);
		return false;
	}


	flag = FillFrameData(oneFrame, oneA);
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);

	mpCmd->SendCmd(oneFrame);
	{
		CScopeLock bLock(&mRespondMutex);
		mRespondRecord.erase(crk);
	}

	int ret = WaitRespond(crk);
	if(0 != ret)
	{
		TW_SET_ERROR(ret);
		return false;
	}
	return true;
}

bool CCoreBusiness_Future::IF_TransTickNo_L(const std::string& para)
{
	CInterfaceTraceInfo tmpLog("IF_TransTickNo_L");
	
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}

	bool flag;
	TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_TICKNOTRANS, GetNextSN());
	

	TWProtocolData_A_SendData oneA(oneFrame);
	oneA.data.pTickNo = new char[para.size()+1];
	memset(oneA.data.pTickNo,0,para.size()+1);
	strcpy(oneA.data.pTickNo,para.c_str());
	

	flag = FillFrameData(oneFrame, oneA);
	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);

	mpCmd->SendCmd(oneFrame);
	{
		CScopeLock bLock(&mRespondMutex);  //获取结果数据
		mRespondRecord.erase(crk);//有一定的概率，终端会返回两次扣款结果到中间件，如果此处不清理，会导致扣款结果错位
	}

	int ret = WaitRespond(crk);
	if(TW_ERROR_WAIT_RESULT == ret)
	{
		TW_SET_ERROR(ret);
		return false;
	}

	return true;
}

bool CCoreBusiness_Future::IF_ExcuteComCommond_L(const std::string& hexStr)
{
	CInterfaceTraceInfo tmpLog("IF_ExcuteComCommond_L");
	
	CScopeLock accessL(&mInterfaceMutex);
	if (!mInitResult)
	{
		TW_SET_ERROR(OP_COM_NOT_INIT);
		return false;
	}

	bool flag;
	TWProtocol_BaseFrame oneFrame;
	if(!HexStrToBaseFrame(hexStr,oneFrame))
	{
		TW_TRACE_DIARY( "IF_ExcuteComCommond Conv Error");
		return false;
	}

	TWCmdKey crk = mpCmd->CalcRespondKey(oneFrame);

	mpCmd->SendCmd(oneFrame);

	return true;
}

//传输文件，1kb一个单位
bool CCoreBusiness_Future::TransferBigFile(const std::vector<unsigned char>& byteArray,const std::string& FileProp )
{
    const static int each_group_size = 1024;//不能超过结构体的容量
	unsigned short prop_size = FileProp.size();
	std::vector<unsigned char> prop_bytes = ConvertNetBytes(prop_size);
	if(prop_size>500)
	{
		return false;
	}
    int group_num = byteArray.size() / each_group_size;//完整包的个数
    int mantissa_size = byteArray.size()% each_group_size;//尾数
	int total_len = byteArray.size();// + group_num * (prop_size+2);
	
	std::string big_file_info = "发送文件：总大小："+Int2Str(total_len)+",分片数量：" + Int2Str(group_num);
	TW_TRACE_DIARY( big_file_info);
    for(int i=0;i<group_num;++i)
    {
        TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_FILETRANS, GetNextSN());
        TWProtocolData_A_SendData oneData(oneFrame);
		oneData.data.FileData.Version=1;
        oneData.data.FileData.Type=1;
        oneData.data.FileData.TotalLen=total_len;
        oneData.data.FileData.Offset=i*(each_group_size);
        oneData.data.FileData.DataLen=each_group_size + prop_size + 2 ;
        for(int j=0;j<each_group_size;++j)
        {//may use std::copy
			oneData.data.FileData.Data[j] = byteArray[i*each_group_size+j];
        }
		//添加属性信息
		for(int j=0;j<prop_size;++j)
		{
			oneData.data.FileData.Data[each_group_size+j] = FileProp[j];
		}
		std::copy(prop_bytes.begin(),prop_bytes.end(),oneData.data.FileData.Data+each_group_size+prop_size);
		
	
        FillFrameData(oneFrame,oneData);
        mpCmd->SendCmd(oneFrame);
    }
    if(mantissa_size !=0)
    {
        TWProtocol_BaseFrame oneFrame(TWPC_A_SENDDATA, TWPC_A_SENDDATA_FILETRANS, GetNextSN());
        TWProtocolData_A_SendData oneData(oneFrame);
        oneData.data.FileData.Version=1;
        oneData.data.FileData.Type=1;
        oneData.data.FileData.TotalLen=total_len;
        oneData.data.FileData.Offset=group_num*(each_group_size);
        oneData.data.FileData.DataLen=mantissa_size + prop_size + 2;
        for(int j=0;j<mantissa_size;++j)
        {//may use std::copy
			oneData.data.FileData.Data[j] = byteArray[group_num*each_group_size+j];
        }
		//添加属性信息
		for(int j=0;j<prop_size;++j)
		{
			oneData.data.FileData.Data[mantissa_size+j] = FileProp[j];
		}
		std::copy(prop_bytes.begin(),prop_bytes.end(),oneData.data.FileData.Data+mantissa_size+prop_size);

        FillFrameData(oneFrame,oneData);
        mpCmd->SendCmd(oneFrame);
    }
    return true;
}

bool CCoreBusiness_Future::IF_TakeComFeedbackCommond_L(CString& jsonStr)
{
	std::string val;
	if(!TakeOneFeedbackCommondJsonStr(val))
	{
		return false;
	}
	if(val.empty())
	{
		return false;
	}
	jsonStr.Format(_T("%s"),val.c_str());
	
	return true;
}


void CCoreBusiness_Future::NewFeedbackCommand(TWProtocol_BaseFrame& frame)
{
	CScopeLock feedbackLock(&m_FeedbackCommondMutex);

	Json::Value rootN;
	std::string topStr,subStr,dataStr,lenStr,snStr,verifyStr;
	topStr = Bin2Hex(&frame.TOPCMD,1);
	subStr = Bin2Hex(&frame.SUBCMD,1);
	
	dataStr = Bin2Hex(frame.DATA,frame.LEN);
	lenStr = Bin2Hex(ConvertNetBytes(frame.LEN));
	snStr = Bin2Hex(ConvertNetBytes(frame.SN));
	verifyStr = Bin2Hex(ConvertNetBytes(frame.VERIFY));

	RemoveWhiteSpaceInStr(verifyStr);
	RemoveWhiteSpaceInStr(lenStr);
	RemoveWhiteSpaceInStr(snStr);
	
	rootN["TOPCMD"] = Json::Value(topStr);
	rootN["SUBCMD"] = Json::Value(subStr);
	rootN["DATA"] = Json::Value(dataStr);
	rootN["LEN"] = Json::Value(lenStr);
	rootN["SN"] = Json::Value(snStr);
	rootN["VERIFY"] = Json::Value(verifyStr);
	Json::FastWriter jsonWriter;

	std::string strVal = jsonWriter.write(rootN);
	//TW_TRACE_DIARY(strVal);
	m_FeedbackCommondStrQueue.push_back(strVal);
	

	static time_t cleanCheckTime = time(NULL);
	time_t currTime = time(NULL);
	if(  (currTime-cleanCheckTime) < 60 )
	{
		return;
	}
	cleanCheckTime = currTime;
	while(m_FeedbackCommondStrQueue.size() > 20)
	{
		m_FeedbackCommondStrQueue.pop_front();
	}
}
bool CCoreBusiness_Future::TakeOneFeedbackCommondJsonStr(std::string& jsonStr)
{
	CScopeLock feedbackLock(&m_FeedbackCommondMutex);
	if(m_FeedbackCommondStrQueue.empty())
	{
		return false;
	}
	jsonStr = m_FeedbackCommondStrQueue.front();
	m_FeedbackCommondStrQueue.pop_front();
	return true;
}
bool CCoreBusiness_Future::IF_TerminalSystemVersionStatus_L(int& status)
{
	ClearSystemStatusRespond();
	std::string tmpComStr = "DF FF FF FF FF 00 04 02 00 00 00 00 B4 72 FD";
	mpCmd->SendSelfCmd(tmpComStr);

	//查询状态
	int sleepUnitMillSec = 100;
	int sleepTotalSlice = 10;
	for(int i=0;i<sleepTotalSlice;++i)
	{
		if(TakeOneSystemStatusRespond(status))
		{
			return true;
		}
		Sleep(sleepUnitMillSec);

	}
	
	return false;
}
bool CCoreBusiness_Future::IF_ExchangeTerminalSystem_L(const int& targetVer)
{
	ClearSystemSwitchRespond();
	int sleepUnitMillSec = 100;
	int sleepTotalSlice = 10;
	int switchStatus=0;
	if(targetVer==15)//切换到1.5
	{
		//send syn
		std::string tmpComStr = "DF FF FF FF FF 00 04 01 00 00 00 14 BB 36 FD";
		mpCmd->SendSelfCmd(tmpComStr);

		//wait ack syn
		switchStatus=-99;
		for(int i=0;i<sleepTotalSlice;++i)
		{
			if(TakeOneSystemSwitchRespond(switchStatus))
			{
				break;
			}
			Sleep(sleepUnitMillSec);
		}
		if(switchStatus!=SWITCHTO_V1_5_SYN_ACK)
		{
			return false;
		}

		// send ack
		tmpComStr = "DF FF FF FF FF 00 04 01 00 00 00 16 7A B7 FD";
		mpCmd->SendSelfCmd(tmpComStr);
		return true;
		
	}
	else if(targetVer ==20)//切换到2.0
	{
		//send syn
		std::string tmpComStr = "DF FF FF FF FF 00 04 01 00 00 00 0A B3 B6 FD";
		mpCmd->SendSelfCmd(tmpComStr);

		//wait ack syn
		switchStatus=-99;
		for(int i=0;i<sleepTotalSlice;++i)
		{
			if(TakeOneSystemSwitchRespond(switchStatus))
			{
				break;
			}
			Sleep(sleepUnitMillSec);
		}
		if(switchStatus!=SWITCHTO_V2_0_SYN_ACK)
		{
			return false;
		}

		// send ack
		tmpComStr = "DF FF FF FF FF 00 04 01 00 00 00 0C B1 36 FD";
		mpCmd->SendSelfCmd(tmpComStr);
		return true;
	}

	return false;
}


bool CCoreBusiness_Future::NewSystemSwitchRespond(const int &val)
{
	CScopeLock comLock(&mRespondMutex);
	m_SystemSwitchRespondQueue.push_back(val);
	while(m_SystemSwitchRespondQueue.size()>10)
	{
		m_SystemSwitchRespondQueue.pop_front();
	}
	return true;
}
bool CCoreBusiness_Future::TakeOneSystemSwitchRespond(int& val)
{
	CScopeLock comLock(&mRespondMutex);
	if(m_SystemSwitchRespondQueue.empty())
	{
		return false;
	}
	val = m_SystemSwitchRespondQueue.front();
	m_SystemSwitchRespondQueue.pop_front();
	return true;
}
bool CCoreBusiness_Future::NewSystemStatusRespond(const int& val)
{
	CScopeLock comLock(&mRespondMutex);
	m_SystemStatusRespondQueue.push_back(val);
	while(m_SystemStatusRespondQueue.size()>10)
	{
		m_SystemStatusRespondQueue.pop_front();
	}
	return true;
}
bool CCoreBusiness_Future::TakeOneSystemStatusRespond(int& val)
{
	CScopeLock comLock(&mRespondMutex);
	if(m_SystemStatusRespondQueue.empty())
	{
		return false;
	}
	val = m_SystemStatusRespondQueue.front();
	m_SystemStatusRespondQueue.pop_front();
	return true;
}

bool CCoreBusiness_Future::ClearSystemSwitchRespond()
{
	CScopeLock comLock(&mRespondMutex);
	m_SystemSwitchRespondQueue.clear();
	return true;
}
bool CCoreBusiness_Future::ClearSystemStatusRespond()
{
	CScopeLock comLock(&mRespondMutex);
	m_SystemStatusRespondQueue.clear();
	return true;
}

void CCoreBusiness_Future::Handle_11_Frame(const TWProtocol_BaseFrame& info)
{
	int val = MAKEINTBIG(info.DATA[0],info.DATA[1],info.DATA[2],info.DATA[3]);
	NewSystemSwitchRespond(val);
}
void CCoreBusiness_Future::Handle_12_Frame(const TWProtocol_BaseFrame& info)
{
	int val = MAKEINTBIG(info.DATA[0],info.DATA[1],info.DATA[2],info.DATA[3]);
	NewSystemStatusRespond(val);
}