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

#include "JsonParamOperate.h"

#pragma  comment(lib, "Version.lib") 

#include "Ntsecapi.h"

class CTWSDNetPay_DllApp : public CWinApp
{
public:
	CTWSDNetPay_DllApp();
	//Behavior
public:
	//初始化组件环境			-- 接口1
	static bool WINAPI  IF_InitEnvironment (IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const char* szAreaInfo,IN const char* szLoaclStationID,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID);
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

	//test
	static bool WINAPI IF_GetC0SuccFaileCount(OUT int& nSucce, OUT int& nFailed, OUT int& nReSendCount, OUT char* strA2Frame, OUT char* strC0Frame);

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
	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
private:
	//重构扣款JSON/xml
	static CString CostructDebitInfo(int nFormatType, tagPayKeyItems& pkItems);

	//转换获取的扣款结果为xml/json格式的数据
	static void	ConverToFormatData(int iFormatType,IN OUT CString& strResult);

	//重构费显josn/xml
	static BOOL ConstructMMIInfo(int nFormateType, IN OUT CString& strResult,OUT BOOL& bClearCmd);

public:
	//重构撤单JSON/xml
	static CString ConstructCancleInfo(int nFormatType,CString strPayID);

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
	static tagPayKeyItems	m_sCurrentDebitInfo;
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


};

extern CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp();