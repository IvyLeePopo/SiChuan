// TWSDNetPay_Dll.h : main header file for the TWSDNetPay_Dll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "BaseDefine.h"
#include "ThreadManage.h"
#include "SuperLog.h"

#include "DVXMLAccount.h"
//#include "DVXMLDebitMoney.h"
#include "DVXMLDebitCancel.h"
#include "DVXMLDebitResult.h"
#include "DVXMLMMI.h"
#include "PayKeyItems.h"
#include "DVXMLDebitFlow.h"

#include "JsonParamOperate.h"

#pragma  comment(lib, "Version.lib") 

#include "Ntsecapi.h"

#include "AgencyPay.h"

//通用回调函数
typedef void (WINAPI *NotifyCallerFun)(int iOption,int iResult);
//广西专用回调函数
typedef void (WINAPI *NotifyCallerFun45)(int iOption,int iResult,void* pIndPtr);

class CTWSDNetPay_DllApp : public CWinApp
{
public:
	CTWSDNetPay_DllApp();
//Behavior
public:
	//初始化组件环境			-- 接口1
	static bool WINAPI  IF_InitEnvironment (IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID);
	//初始化组件环境			-- 接口1-1
	static bool WINAPI  IF_InitEnvironment2 (IN const UINT& nThreadID,IN const HWND& hWnd,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID,void (WINAPI *fun)(int option, int result));
	//初始化组件环境			-- 接口1-2(专门为广西而定制开发）
	static bool WINAPI  IF_InitEnvironment3 (IN const UINT& nThreadID,IN const HWND& hWnd,IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID,IN OUT void* pIndPtr,void (WINAPI *fun)(int option, int result,void* pIndPtr));
	//清理资源					-- 接口2
	static bool WINAPI  IF_Destroy(void);
	//获取上一次错误信息		-- 接口3
	static const char* WINAPI IF_GetLastErrorDesc(void);
	//启动获取用户信息操作		-- 接口4
	static bool WINAPI  IF_StartGetAccountInfo(IN const int& iOvertime);
	//停止获取用户信息操作		-- 接口5
	static bool WINAPI  IF_StopGetAccountInfo(void);
	//获取用户信息				-- 接口6
	static bool WINAPI  IF_GetAccountInfo(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType);
	//执行扣款操作				-- 接口7
	static bool WINAPI  IF_DebitMoney(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//执行撤单操作				-- 接口8
	static bool WINAPI  IF_DebitCancel(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//获取扣款结果				-- 接口9
	static bool WINAPI  IF_GetDebitResult(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType);
	//设置交互界面				-- 接口10
	static bool WINAPI	IF_SetMMI(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//获取硬件参数				-- 接口11
	static bool WINAPI	IF_GetParam(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT char* szResultContext,IN OUT int& iResultSize);
	//检测组件状态				-- 接口12
	static bool WINAPI  IF_GetComponentStatus(IN OUT unsigned int& uStatus);
	//执行传输关键信息数据操作	-- 接口13
	static bool WINAPI  IF_TranslateData(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//传输图像	-- 接口14
	static bool WINAPI  IF_TransferImage(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType);
	//代扣查询
	static bool WINAPI	IF_AgencyDebitQuery(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT int& iResult);
	
	
	//内部接口，获取硬件的终端序号
	static bool WINAPI	IF_InnerGetDeviceNo(IN char* szParamContext,IN int& iParamSize);
	//内部接口，获取指定数据
	static bool WINAPI  IF_InnerGetSpecialData(IN char* szParamContext,IN int& iParamSize);
	//内部接口，传输指定参数
	static bool WINAPI	IF_InnerSetSpecialData(IN const char* szParamContext,IN const int& iParamSize);

    // 重载不同类型消息的写日志函数
#ifdef _DEBUG
    static void WINAPI RecordLog(IN const CString &strLog,IN const int& iLevel = 0,IN const BOOL& bForce = FALSE);
#else
    static void WINAPI RecordLog(IN const CString &strLog,IN const int& iLevel = 1,IN const BOOL& bForce = FALSE);
#endif
	//线程关闭函数
	static int fnCloseThread(IN const HANDLE& hThread,IN const DWORD& dwMilliseconds);

protected:
	//权限验证
	static bool WINAPI  IF_Authority(IN const char* szAuthKey,IN const int& iAuthKeySize);
	//读取配置信息
	static void WINAPI  ReadConfigInfo(void);
//内部辅助函数
public:
	//实现类是CString类的Format功能
	static CString FmtStr(const TCHAR* lpsFmt,...);
	//将时间的字符串形式转化为time_t格式
	static time_t CString2time_t(LPCTSTR lpstime);
	//获取GUID
	const CString GetGuidString(IN const int& iType);
	//获取指定文件的版本号
	static CString GetVersionOfFile(LPCTSTR lpsPath);
	//获取当前时间的字符串形式
	static const CString GetCurrentTimeString(IN const int& iTimeType);
	//创建指定目录
	static BOOL MakeDirectory(CString strCreateDir);
	//显示上一次的系统错误信息
	static LPCTSTR DisplayWinError(DWORD dwErrorCode,int nType=0,BOOL bPrompt=FALSE);
	//获取当前进程所使用的内存
	static DWORD GetProcessMemory(IN const int& iShowType=0);
	//通知外部调用者，异步处理结果
	static void NotifyCaller(IN const int& iOption,IN const int& iResult);
	//测试输入字符串是否为字母和数字的组合
	static BOOL CheckIsAlNum(IN const char* szData,IN const int& iType=0);

	static BOOL CheckStringLetter(char * strCheck, int nType=0);

	//进制转换（16进制转BASE64）
	static BOOL HexToBase64(IN const CString& strSrcHex,OUT CString& strDesBase64);
	static INT  BASE64_Encode(const BYTE* inputBuffer, INT inputCount, TCHAR* outputBuffer);
	static BOOL Base64ToHex(IN const CString& strSrcBase64,OUT CString& strDesHex);
	static BYTE Decode_GetByte(char c);
	//16进制转2进制
	static void Hex2Bin(IN LPTSTR pHex,OUT PBYTE pBin,IN DWORD dwHex_len);
	//2进制转16进制
	static CString Bin2Hex(IN PBYTE pBin,IN DWORD dwBin_Len);

	//解析代扣扣款接口返回来的数据
	static int GetAgencyResult(CString strXML,DWORD dwConsumeTime);
	//构造代扣扣费数据并执行代扣
	static bool AgencyConstructDebitData();
	//构造代扣撤单数据并执行撤单
	static int AgencyConstructCancelData(IN const CString& strPayIdentifier);
	//初始化代扣动态库
	static bool AgencyInitDll(IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID);

	//获取银联非接模块状态
	static int GetUnipayWorkFlowState(void) {return m_iNFCModuleState;}
	//设置银联非接模块状态
	static void SetUnipayWorkFlowState(IN const int& iUnipayState){ m_iNFCModuleState = iUnipayState; }

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
//Attributes
public:
	//业务管理线程类指针
	static CThreadManage*	m_pThreadManage;
	//标识组建环境是否正常初始化
	static BOOL m_bIsInitEnvironment;
	//标识调用是否经过合法授权
	static BOOL m_bAuthorizedLegal;
	//标识支付子平台网络是否正常
	static BOOL m_bNetErrorHttpServerInTollNetNotExist;
	//标识支付子平台服务器是否正常
	static BOOL m_bNetErrorHttpServerTrawePlatformNotExist;
	//标识是否需要保存日志
	static BOOL	m_bSaveLog;
	//线程存活检测时间间隔(单位:秒),默认5分钟检测一次
	static DWORD   m_dwThreadCheckSpanTime;
	//存储主动态库工作目录
	static CString m_strWorkDir;
	//存储上次错误信息指针
	static CString m_strLastErrorDesc;
	//存储虚拟卡号前10位
	static CString m_strVirtualCardPrefix;
	//存储线程配置信息
	static tagTranstParam	 m_sTranstParam;
	//当前的扣款操作是整合操作还是单体操作
	static BOOL	m_bCurrentDebitMode;
	//标记当前是否正在交易(由业务管理线程进行重置:1.交易完毕;2,超时重置)
	static BOOL m_bIsTrading;
	//存储当前扣款操作的关键信息
	static CPayKeyItems	m_pCurrentDebitInfo;
	//存储当前扣款结果的关键信息
	static tagDebitResultItems	m_sCurrentDebitResult;
	//当前扣款操作的超时起始时间
	static DWORD m_dwTradeBeginOvertime;
	//往智能硬件更新时间的间隔
	static DWORD m_dwUpdateDevieTimeSpanTime;
	//标识测试数据
	static BOOL	 m_bIsTestData;
	//业务动态库版本信息
	static CString m_strNetPayDllVersion;
	//设备控制动态库版本
	static CString m_strDeviceDllVersion;
	//日志删除操作指定时间段起点
	static int	m_iDeleteLogStartHour;
	//日志删除操作指定时间段终点
	static int	m_iDeleteLogEndHour;
	//日志删除操作轮询时间
	static int	m_iDeleteLogSpanTime;
	//日志保存天数
	static int  m_iDeleteLogSaveDays;
	//标识扣款流程编码
	static int	m_iDebitTaskNo;
	//标识组件通知外部调用者的方式(1,消息回抛；2，使用回调函数)
	static int	m_iNotifyMode;
	//撤单本地缓存限时时间（单位：分钟）
	static int  m_iLastDelayTime;
	//重发数据的时间间隔（单位：秒）
	static time_t m_tResendDataSpanTime;
	//标识组件通知外部使用的回调函数指针
	static NotifyCallerFun m_fnCallbackNotifyFun;
	//标识组件通知外部使用的回掉函数指针（广西专用）
	static NotifyCallerFun45 m_fnCallbackNotifyFun45;
	//专门为广西定做的初始化接口3，用于保存一个个性化指针，在回掉通知的时候传递出去
	static void* m_pIndPtrFor45;
	//等待退出时间(单位：毫秒）
	static int	m_iExitWaitForTime;
	//更新本地存储文件锁
	HANDLE	hFileUpdate;
	//HTTP内部版本号，区分不同的处理逻辑
	static int	m_iHTTPVersion;	
	//区分底层设备类型，目前0默认表示特微自有设备，1表示郎为设备
	static int	m_iDeviceType;	
	//是否将代扣与扫码支付返回渠道代码统一
	static bool m_bReturnPayChannelCodeUnify;
	//代扣扣费超时时间，单位：毫秒
	static int  m_iAgencyDebitTimeout;
	//清屏限制延迟时间，单位：毫秒，默认200毫秒
	static int	m_iClearMMIDelayTime;
	//是否启用代扣动态库
	static bool	m_bUseAgencyDll;
	//代扣动态库加载类
	static CAgencyPay	m_AgencyPay;
	//特微终端产品序列号
	static CString m_strTWProductSerialNo;
	//是否扩展MMI交互细节
	static BOOL	   m_bIsExternMMIDetail;

	//是否启动银联非接支付业务
	static bool m_bNFCIsUse;
	//银联模块的工作状态
	static int  m_iNFCModuleState;
	//银联模块是否已经签到成功
	static bool m_bNFCSignInSuccess;
	//银联模块签到状态监测时间间隔，单位：秒(默认10秒一次）
	static int	m_iNFCCheckSignSpanTime;
	//银联模块NFC支付异常
	static BOOL m_bNFCPayFail;
	//银联模块NFC支付失败次数（连续）
	static int	m_iNFCPayFailCount;
	//银联模块终端编码（BASE64字符格式）
	static CString m_strNFCTerminal;
	//银联模块硬件序列号（BASE64字符格式）
	static CString m_strNFCSerialNo;
	//银联模块签到交互次数
	static int m_iNFCSignFlowDataCount;
	//银联模块定时重签检测时间起始点
	static int m_iNFCDefaultStartReSignHour;
	//银联模块定时重签检测时间截止点
	static int m_iNFCDefaultEndReSignHour;
	//银联模块每天定点重签是否执行完毕（每天一次）
	static BOOL m_bNFCAutoSignIsExecute;
	//银联模块签到数据交互时间间隔限制（单位：秒）
	static int  m_iNFCSignDataLimitSpanTime;
	//银联模块上次签到数据获取时间（每一个签到轮回当中有效）
	static time_t m_tNFCLastSignDataTime;
	//银联支付流程，是否允许没有获取冲正报文也允许发送扣费请求
	static BOOL	m_bNFCIsAllowDebitWithoutCancelData;
	//银联支付流程，获取到刷卡数据后，限制等待冲正数据的时间（单位：秒）
	static int m_iNFCGetCancelDataLimitTime;
	//银联模块每次开始签到的时间点
	static time_t m_tNFCStartSignInTime;
};

extern CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp();