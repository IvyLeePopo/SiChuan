#ifndef _TRAWE_EMBEDDED_BASE_DEFINE_H_
#define _TRAWE_EMBEDDED_BASE_DEFINE_H_

#pragma once

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
//#include "SQLite3Base.h"

#include "json.h"
#ifdef _DEBUG
#pragma  comment(lib, "json_vc71_libmtd.lib")  
#else
#pragma  comment(lib, "json_vc71_libmt.lib")  
#endif

#define FILE_NAME_DEVICE_DLL	_T("HTTWSDDevice.dll")
#define FILE_NAME_APP_CONFIG	_T("TWSDNetPayConfig.ini")
#define FILE_NAME_LOG_CONFIG	_T("LogConfig.ini")
#define FILE_NAME_HTTP_CONFIG	_T("")
#define FILE_NAME_DEVICE_CONFIG _T("HTSMConfig1.ini")

#define OP_SUCCESS 0x00
#define OP_FAILED  0x01
#define OP_NO_MONEY                  0x02         //账户余额不足
#define OP_QRCODE_ILLEGAL            0x03         //二维码非法
#define OP_QRCODE_OT                 0x04         //二维码超出有效期
#define OP_NO_ACCOUNT_INFO           0x05         //支付凭证异常
#define OP_DEBIT_OT                  0x06         //扣款超时
#define OP_PSW_ERR                   0x07         //密码错误
#define OP_PARAM_ERR                 0x08         //参数错误
#define OP_LAST_TRADE_NOT_END        0x09         //上次交易未结束
#define OP_COM_NOT_INIT              0x0A         //组件未初始化
#define OP_COM_ILLEGAL               0x0B         //组件校验非法
#define OP_PAY_PLATFORM_ERR          0x0C         //支付平台网络故障

#define OP_DISCONNECT_TERMINAL		0xFD		//与终端通讯异常
#define OP_CANCLE  0xFE                           //取消交易

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

//设备控制线程，回馈业务管理线程获取的结果分类代码
enum RESPONSE_MANAGE_CODE
{
	RDM_NONE			= 0x00,		//未知类型

	RDM_DEVICE_LEGAL	= 0x01,		//判定终端是否合法使用
	RDM_DEBITE_START	= 0x02,		//发起扣款
	RDM_DEBITE_CANCEL	= 0x03,		//发起撤单
	RDM_ISSUE_DATE		= 0x04,		//下发收费流水关键信息
	RDM_GET_LAST_ERR	= 0x05,		//要求提供上次错误信息
	RDM_DEVICE_INIT		= 0x06,		//设备初始化

	RDM_HEART				= 101,		//心跳查询
	
	RDM_READ_HW_VER			= 102,		//读硬件版本
	RDM_READ_CAN_ID			= 103,		//读CA NID
	RDM_READ_MAX_CODE		= 104,		//读MAX码
	RDM_READ_FM_VER			= 105,		//读固件版本
	RDM_READ_SCAN_TIME_LIMIT= 106,		//读扫码时间限制设置
	RDM_READ_SERIAL_NO		= 107,		//读设备唯一序号
	RDM_READ_WELCOME_SC		= 108,		//读开机界面
	RDM_READ_NET0_IP_INFO	= 109,		//读网口0 IP信息
	RDM_READ_NET1_IP_INFO	= 110,		//读网口1 IP 信息
	RDM_READ_DEVICE_TMP		= 111,		//读硬件当前时间
	RDM_READ_DEVICE_TIME	= 112,		//读硬件当前时间
	
	RDM_SET_HEART_TIME		= 201,		//设置终端心跳间隔
	RDM_SET_WELCOME_SC		= 202,		//设置终端开机画面
	RDM_SET_MMI_INFO		= 203,		//控制交互（再见）
	RDM_SET_MMI_BYTE		= 204,		//控制交互（再见）
	RDM_SET_MMI_AD			= 205,		//控制交互（广告）
	RDM_SET_DEVICE_TIME		= 206,		//下发车道时间
	RDM_SET_NET0_IP_INFO	= 207,		//设置网口0 IP信息
	RDM_SET_NET1_IP_INFO	= 208,		//设置网口1 IP信息

	RDM_PARAM_WRITE_MAX_CODE		= 301,		//写MAX码
	RDM_PARAM_WRITE_CAN_ID			= 302,		//写CANID
	RDM_PARAM_WRITE_SERIAL_NO		= 303,		//写设备唯一序列号
	RDM_PARAM_WRITE_SACN_TIME_LIMIT = 304,		//设置扫码限制时间
	RDM_PARAM_TEST_SC				= 305,		//节目测试轮询测试
	RDM_PARAM_TT_DATA				= 306,		//透传数据到指定串口
};

//回馈调用者功能分类代码
enum RESPONSE_CALLER_FUN_CODE
{
	RCC_NONE			= 0x00,		//未知类型
	RCC_FUN_DEBIT		= 0x01,		//扣款回馈
	RCC_FUN_CANCEL		= 0x02,		//撤单回馈
	RCC_FUN_NO_STOP_CAR	= 0x03,		//不停车
};

//回馈调用者执行结果的错误代码
enum RESPONSE_CALLER_EXECUTE_CODE
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
	RCC_DEVICE_INIT_ERROR	= 0x0D,		//智能硬件初始化失败

	RCC_BREAK_OPERATION     =0xFE,      //取消交易

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
	int		MainType;			//封闭式入出口、开放式
	int		SubType;			//ETC,MTC,自动发卡,便携收费机
	int		AreaID;				//区域编码(如果有)
	TCHAR   str_AreaID[200];
	int		RoadID;				//路段编码(如果有)
	TCHAR   str_RoadID[200];
	int		StationID;			//站点编码
	TCHAR str_StationID[200];
	int		LaneID;				//车道编码
	TCHAR str_LaneID[200];
	int		LaneType;			//车道类型(1,入口;2,出口)
	TCHAR	LaneTab[16];		//车道唯一索引
	TCHAR	StationTab[32];		//站点唯一索引
	int		STServerTpye;		//站级服务器类型
	TCHAR	STServerIP[24];		//站级服务器IP地址
	TCHAR	STServerID[24];		//站级服务器ID编码

	int WebUpdatePort;				//服务器更新端口 add 2017-08
	int UseAngencyDll;              //是否启用代扣动态库， add 2018-01
	int ReturnPayChannelCodeUnify;   //是否统一返回代码
	int ForceUTC8;                  //强制使用UTC8，add 2018-01

	int		WebServerPort;			//反射服务器开放端口
	TCHAR	WebServerIP[24];		//反射服务器IP地址
	TCHAR   LocalPCWebServerIP[24];   //内网IP

	int		ConfigLocalIPMode;		//TR300平台网络IP设置模式(0,设置IP;1,自动获取)
	TCHAR	ConfigLocalIP0[24];		//TR300平台网口0的IP
	TCHAR	ConfigLocalSub0[24];	//TR300平台网口0的子网掩码
	TCHAR	ConfigLocalGate0[24];	//TR300平台网口0的网关
	TCHAR	ConfigLocalIP1[24];		//TR300平台网口1的IP
	TCHAR	ConfigLocalSub1[24];	//TR300平台网口1的子网掩码
	TCHAR	ConfigLocalGate1[24];	//TR300平台网口1的网关
	TCHAR	ConfigLocalDefGate[24];	//TR300平台默认网关
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

	int     PayModel;

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

	//收费员姓名
	CString OperatorName;
	//收费员工号
	CString OperatorID;
	

	/*****************Entry详细项************************/
	//入口站点
	int		EntryStationID;
	CString str_EntryStationID;//TCHAR str_EntryStationID[100];
	//入口站名
	CString EntryStationName;
	//入口时间
	CTime	EntryTime;
	//入口车道
	int		EntryLane;
	CString str_EntryLane;//TCHAR str_EntryLane[100];
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

	CString str_EntryRoadID;
	CString str_EntryAreaID;

	//add 2018-05
	CString AutoPlate;
	int AutoPlateColor;

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