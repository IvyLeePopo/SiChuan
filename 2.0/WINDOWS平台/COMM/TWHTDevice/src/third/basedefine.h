#ifndef _TRAWE_EMBEDDED_BASE_DEFINE_H_
#define _TRAWE_EMBEDDED_BASE_DEFINE_H_

#pragma once
//#include <usualfuns.h>
//#include <AesAlg.h>
#include "md5.h"

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#include <winsock2.h>
#pragma comment(lib, "ws2_32")

#include <math.h>

#include <Mmsystem.h>
#pragma  comment(lib, "Winmm.lib")  
#include <sys/stat.h>

#include "Psapi.h"
#pragma  comment(lib, "Psapi.lib")  


#include "Markup.h"
#include "sql\SQLite3Base.h" //modify

#include "json\json.h"
#ifdef _DEBUG
#pragma  comment(lib, "json_vc71_libmtd.lib")  
#else
#pragma  comment(lib, "json_vc71_libmt.lib")  
#endif

#include "AesECB.h"

//AES密钥
#define AES_ENC_KEY_DATA 	_T("2016042010012006")
#define AES_ENC_KEY_DATA_63	_T("1111222233334444")
#define AES_ENC_KEY_DATA_44	_T("2016081820151023")

#define FILE_NAME_DEVICE_DLL	_T("HTTWSDDevice.dll")
#define FILE_NAME_APP_CONFIG	_T("TWSDNetPayConfig.ini")
#define FILE_NAME_LOG_CONFIG	_T("LogConfig.ini")
#define FILE_NAME_HTTP_CONFIG	_T("")
#define FILE_NAME_DEVICE_CONFIG _T("HTSMConfig1.ini")

#define OP_SUCCESS 0

#define WM_THREAD_MANAGE_ONLY_DEBIT_MONEY				WM_APP + 1
#define WM_THREAD_MANAGE_DEAL_PLATFORM_RESPONSE_RESULT	WM_APP + 2
#define WM_THREAD_MANAGE_CHECK_DEBIT_PASSWORD_STATE		WM_APP + 3	
#define WM_THREAD_MANAGE_START_DEBIT_FLOW				WM_APP + 4
#define WM_THREAD_MANAGE_DEBIT_CANCEL					WM_APP + 5

#define WM_THREAD_HTTP_SEND_DATA_REQUEST				WM_APP + 6
#define WM_THREAD_HTTP_SEND_HEART						WM_APP + 7

#define WM_THREAD_DEVICE_START_GET_DATA					WM_APP + 8
#define WM_THREAD_DEVICE_STOP_GET_DATA					WM_APP + 9
#define WM_THREAD_DEVICE_NOTIFY							WM_APP + 10
#define WM_THREAD_DEVICE_SHOW_DEAL_INFO					WM_APP + 11
#define WM_THREAD_DEVICE_START_DEBIT_FLOW				WM_APP + 13
#define WM_THREAD_DEVICE_DEBIT_CANCEL					WM_APP + 14


//JSON数据格式的类型区分
enum JSON_DATA_TYPE
{
	JSON_DT_SEND_DIBIT			= 3,	//扣费请求
	JSON_DT_SEND_DIBIT_CHECK	= 4,	//扣费查询
	JSON_DT_SEND_DIBIT_CANCEL	= 5,	//撤销订单
	JSON_DT_SEND_PASS_STATION	= 8,	//过开放收费站操作
	JSON_DT_SEND_HEART			= 9,	//心跳查询
	JSON_DT_SEND_DIBIT_SECOND	= 10,	//二次扣款

	JSON_CMD_TYPE_DEBIT_EXIT     = 1,	//封闭式出口扣费命令
	JSON_CMD_TYPE_DEBIT_OPEN     = 2,	//开放式扣费指令
	JSON_CMD_TYPE_DEBIT_ONLY_PAY = 3,	//纯扣款命令

	JSON_DT_PARAM_ACCOUNT		 = 0xFF01,	//用户参数JSON格式
	JSON_DT_PARAM_DEBIT_MONEY	 = 0xFF02,	//扣款参数JSON格式
	JSON_DT_PARAM_DEBIT_CANCLE	 = 0xFF03,	//撤单参数JSON格式
	JSON_DT_PARAM_DEBIT_RESULT	 = 0xFF04,	//扣款结果参数JSON格式
	JSON_DT_PARAM_MMI			 = 0xFF05,	//交互界面参数JSON格式
};

//运营平台回馈代码定义
enum JSON_SERVER_RETURN_CODE
{
	HTTP_RC_DATA_LEGAL					= 200,	//回馈数据符合规则
	HTTP_RC_DATA_VALID					= 1,	//数据正常
	HTTP_RC_BLACKLIST					= 2,	//二维码绑定支付用户在特微黑名单当中
	HTTP_RC_NO_REGISTER					= 3,	//二维码绑定支付用户未注册
	HTTP_RC_NO_ENTRY_BY_ID				= 4,	//二维码绑定支付用户无有效入口信息
	HTTP_RC_ID_IS_RUNNING				= 5,	//二维码绑定支付用户已经路网有入口

	HTTP_RC_DEBIT_NEED_PASSWORD			= 6,	//需要二维码绑定支付用户输入密码
	HTTP_RC_DEBIT_CODE_TIMEOUT			= 7,	//二维码有效期超时
	HTTP_RC_DEBIT_SRC_ILLGAL			= 8,	//二维码来源非法（非指定范围内的二维码）
	HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	= 9,	//余额不足,允许重试
	HTTP_RC_DEBIT_FAIL					= 10,	//扣费失败(多种原因，不能重新尝试扣款)
	HTTP_RC_DEBIT_SUCCESS				= 11,	//扣费成功
	HTTP_RC_DEBIT_NO_VOUCHER			= 12,	//协议格式错误（详细错误会放到message中）
	HTTP_RC_PROTOCOL_ERROR				= 13,	//车道产生的交易凭证关联的订单信息在平台没有找到
	HTTP_RC_PAYPLATROMR_ERROR			= 14,	//支付平台错误
	HTTP_RC_NO_CANCEL_ORDER				= 15,	//订单已过有效期，禁止撤销
	HTTP_RC_NO_SERVICES					= 16,	//暂停服务
	HTTP_RC_VEHICLE_IS_RUNNING			= 17,	//车牌已经在路网运行
	HTTP_RC_VEHICLE_NO_ENTRY			= 18,	//无法检索到有效入口
	HTTP_RC_INPUT_PASSWORD_ERROR		= 20,	//用户输入密码错误
};

//扫码灯控制状态
enum SM_PROTOCOL_CONST
{
	SM_PRPTOCOL_LIGHT_SUCCESS		= 0x00,	//提示扫码成功
	SM_PRPTOCOL_LIGHT_SCAN			= 0x01, //提示正在扫码
	SM_PRPTOCOL_LIGHT_FAIL			= 0x02,	//提示扫码失败
	SM_PRPTOCOL_LIGHT_ALL_OFF		= 0x03, //灭灯
};

//定时器ID
enum TIMER_ID
{
	TIMER_ID_DEBIT_MONEY = 1,
	TIMER_ID_DEBIT_CHECK = 2,
	TIMER_ID_DEBIT_CANCEL = 3,
};

//智能设备故障状态描述
enum DEVICE_STATE_DESC
{
	DEVICE_STATE_LOAD			= 0x0001,	//驱动加载失败
	DEVICE_STATE_SERIAL_OPEN	= 0x0002,	//设备串口无法打开
	DEVICE_STATE_SERIAL_COMM	= 0x0004,	//设备串口通讯故障
	DEVICE_STATE_PAY_WEB		= 0x0008,	//支付子平台网络故障
	DEVICE_STATE_PAY_SERVER		= 0x0010,	//支付子平台服务器故障
	DEVICE_STATE_4G				= 0x0020,	//云端通讯设备故障
};

//回馈调用者错误代码
enum RESPONSE_CALLER_CODE
{
	RCC_OK					= 0x00,		//成功
	RCC_INPUT_PASSWORD		= 0x01,		//用户正在输入密码
	RCC_NO_MONEY			= 0x02,		//用户余额不足
	RCC_QRCODE_ILLEGAL		= 0x03,		//二维码非法
	RCC_QRCODE_OT			= 0x04,		//二维码有效期超时
	RCC_NO_ACCOUNT_INFO		= 0x05,		//智能设备未能获取用户信息
	RCC_DEBIT_OT			= 0x06,		//扣款超时
	RCC_PSW_ERROR			= 0x07,		//密码输入错误
	RCC_PARAM_ERROR			= 0x08,		//传入参数有误
	RCC_LAST_TRADE_NOT_END	= 0x09,		//上次交易未完成
	RCC_COM_NOT_INIT		= 0x0A,		//组件未初始化
	RCC_COM_ILLEGAL			= 0x0B,		//组件未经授权
	RCC_PAY_PLATFORM_ERROR	= 0x0C,		//支付平台服务器故障

	RCC_INNER_ERROR			= 0xFF,		//内部错误
};

//用户信息结构体
typedef struct _tagAccountInfo
{
	int		DataType;			//数据类型(低字定义：1,纯数字字符串;2,暂未定义；高字定义：扫码器索引)
	int		SrcType;			//数据来源(1,扫码器;2,暂未定义)
	DWORD	DataSize;			//数据长度
	TCHAR	DataContext[5120];	//数据长度(以 NULL 结束的字符串)
}tagAccountInfo;

//车道配置信息结构体
typedef struct _tagLaneConfigInfo
{
	int		MainType;		//封闭式入出口、开放式
	int		SubType;		//ETC,MTC,自动发卡,便携收费机
	int		AreaID;			//区域编码(如果有)
	int		RoadID;			//路段编码(如果有)
	int		StationID;		//站点编码
	int		LaneID;			//车道编码
	int		LaneType;		//车道类型(1,入口;2,出口)
	TCHAR	LaneTab[16];	//车道唯一索引
	TCHAR	StationTab[32]; //站点唯一索引
	int		STServerTpye;	//站级服务器类型
	TCHAR	STServerIP[24];	//站级服务器IP地址
	TCHAR	STServerID[24];	//站级服务器ID编码
	TCHAR	WebServerIP[24];//内网反射服务器IP地址
}tagLaneConfigInfo;

//交易关键信息
typedef struct _tagPayKeyItems
{
	//是否有效
	BOOL	Valid;
	//超时等待时间
	int		OverTime;
	//扣款模式
	int		OperationMode;

	/*****************Debit详细项************************/
	//交易识别码
	CString	PayIdentifier;
	//交易金额(单位：分)
	int		Money;
	//过车时间
	CTime	SubTime;
	//交易主分类
	int		DebitMainType;
	//第三方支付凭证
	CString PayCertificateCode;
	//支付凭证类型
	int		PayCertificateType;

	/*****************Vehicle详细项************************/
	//车种
	int		VehicleType;
	//车型
	int		VehicleClass;
	//车牌颜色
	int		VLColor;
	//车牌
	CString	VehicleLicense;
	//轴数
	int		AxisCount;
	//重量
	int		TotalWeight;
	//限重
	int		LimitWeight;

	/*****************Operation详细项************************/
	//通行凭证类型
	int		PassCertificateType;
	//通行凭证编码
	CString CardID;
	//统计日
	int		TollDate;
	//统计班次
	int		ShiftID;
	//打印票据号
	CString TicketNo;

	/*****************Entry详细项************************/
	//入口站点
	int		EntryStationID;
	//入口站名
	CString EntryStationName;
	//入口时间
	CTime	EntryTime;
	//入口车道
	int		EntryLane;
	//入口车牌
	CString	EntryVehicleLicense;
	//入口车牌颜色
	int		EntryVLColor;
	//入口车种
	int		EntryVehicleType;
	//入口车型
	int		EntryVehicleClass;
	//入口区域编码
	int		EntryAreaID;
	//入口路段号
	int		EntryRoadID;
}tagPayKeyItems;

//扣款结果关键信息明细
typedef struct _tagDebitResultItems
{
	//是否有效
	BOOL	Valid;

	int		ResultType;
	int		ExecuteCode;
	int		PayPlatformType;
	time_t	DebitTime;
	int		ConsumeTime;
	TCHAR	PayIdentifier[128];
	TCHAR	DebitOrder[128];
	TCHAR	ExecuteDesc[256];
	TCHAR	TradeSecurityCode[64];
}tagDebitResultItems;

//车道APP类向各个线程类传递基本配置信息的结构体
typedef struct _tagTranstParam
{
	//省份编码
	int					ProvinceID;
	//车道配置基本信息
	tagLaneConfigInfo	LaneConfigInfo;
	//特殊主动触发消息ID
	UINT				ActiveNotifyMsgID;
	//调用者线程ID
	UINT				CallerThreadID;
	//调用者窗体句柄
	HWND				CallerWnd;
	//后台业务管理线程
	HANDLE				ThreadManageHandle;
	/******************各种路径集合******************/

	//车道初始目录绝对路径,默认为运行程序的当前目录
	TCHAR	PathCommDir[MAX_PATH];
	//生产过程产生的临时关键文件存储目录绝对路径
	TCHAR	PathDataDir[MAX_PATH];
	//车道日志目录绝对路径
	TCHAR	PathLogDir[MAX_PATH];
}tagTranstParam;

//MMI参数的文字参数结构信息
typedef struct _tagMMIItemText
{
	int	Type;
	int Sub;
	int Row;
	BOOL  IsScroll;
	TCHAR Context[48];
}tagMMIItemText;

//MMI参数的声音参数结构信息
typedef struct _tagMMIItemVoice
{
	int	Type;
	int No;
}tagMMIItemVoice;

//MMI参数的图像参数结构信息
typedef struct _tagMMIItemGraph
{
	int	Type;
	int No;
}tagMMIItemGraph;

#endif //_TRAWE_EMBEDDED_BASE_DEFINE_H_