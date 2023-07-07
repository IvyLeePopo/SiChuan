// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions






#include <io.h>
#include <afxtempl.h>

#include "ATLComTime.h"

#include <Mmsystem.h>
#pragma  comment(lib, "Winmm.lib")

#include "usualfuns.h"

#include "Markup.h"



/*
//扫码器序列排号
enum SCAN_MACHINE_INDEX
{
	SM_INDEX_DWON_LEFT	= 0x01,	//左下扫码器(客车1）
	SM_INDEX_DWON_RIGHT	= 0x02,	//右下扫码器(客车2)
	SM_INDEX_UP_LEFT	= 0x04,	//左上扫码器(货车1)
	SM_INDEX_UP_RIGHT	= 0x08,	//右上扫码器(货车2)
	SM_INDEX_HAND		= 0x80,	//手持扫码器
};

#define OP_SUCCESS	0							//执行成功 -- 函数返回值非布尔型的话，必须要校验返回值


#define MSG_START_ID_SM 100

//扫码器控制相关消息
#define WM_THREAD_SM_START_SCAN							WM_APP + MSG_START_ID_SM + 1	//强制开始扫码
#define WM_THREAD_SM_STOP_SCAN							WM_APP + MSG_START_ID_SM + 2	//初始化所有的工作线程
#define WM_THREAD_SM_RESET_MACHINE						WM_APP + MSG_START_ID_SM + 3	//初始化指定扫码器
#define WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO	WM_APP + MSG_START_ID_SM + 4	//工作线程扫码状态提示信息
#define WM_THREAD_SM_CONTROL_BOARD						WM_APP + MSG_START_ID_SM + 5	//控制串口板

#define WM_THREAD_SM_GET_SCAN_CODE_DATA					WM_APP + MSG_START_ID_SM + 6	//工作线程获取到扫码结果
#define WM_THREAD_SM_STATE								WM_APP + MSG_START_ID_SM + 7	//工作线程扫码状态提示信息


#define WM_THREAD_SM_GET_SYSTEM_INFO		WM_APP + MSG_START_ID_SM + 2	//工作线程获取扫码器系统信息
#define WM_THREAD_SM_SCANING_STATE			WM_APP + MSG_START_ID_SM + 3	//工作线程扫码状态提示信息
#define WM_THREAD_SM_SCAN_CODE				WM_APP + MSG_START_ID_SM + 8	//强制开始扫码



//车道APP类向各个线程类传递基本配置信息的结构体
typedef struct _tagTranstParam
{
	//省份编码
	int					ProvinceID;
	//车道配置基本信息
//	tagLaneConfigInfo	LaneConfigInfo;
	//特殊主动触发消息ID
	DWORD				ActiveNotifyMsgID;
	/////////////////////各种路径集合///////////////////////////

	//车道初始目录绝对路径,默认为运行程序的当前目录
	TCHAR	PathCommDir[MAX_PATH];
	//车道存储数据目录绝对路径
	TCHAR	PathDataDir[MAX_PATH];
	//车道硬件驱动目录绝对路径
	TCHAR	PathDeviceDir[MAX_PATH];
	//车道日志目录绝对路径
	TCHAR	PathLogDir[MAX_PATH];
	//车道数据备份目录绝对路径
	TCHAR	PathDataBackupDir[MAX_PATH];
	//生产过程产生的临时关键文件存储目录绝对路径
	TCHAR	PathProduceDir[MAX_PATH];
	//临时文件存储目录绝对路径
	TCHAR	PathTempDir[MAX_PATH];
}tagTranstParam;

//扫码器类型
enum SCAN_MACHINE_TYPE
{
	SM_TYPE_HONEYWELL_3310G = 1,	//霍尼韦尔3310g
	SM_TYPE_NEW_LAND_FM_420 = 2,	//新大陆FM420
};

//串口参数信息结构体
typedef struct _tagCOMParam
{
	int		Port;					//串口
	int		BaudRate;				//波特率
	int		ParityBit;				//校验位
	int		StopBit;				//停止位
	int		DataBit;				//数据位

	DWORD	InBufferSize;			//输入缓冲区大小
	DWORD	OutBufferSize;			//输出缓冲区大小
}tagCOMParam;

//智能扫码器主板控制参数数据描述结构体
struct tagSMMainBoardParam
{
	BYTE	Type;			//数据类型(1,发送;2,回馈)
	BYTE	CMDFlag;		//命令标号
	BYTE	ParamData[512];	//参数内容
	WORD	ParamSize;		//参数长度
};


//智能扫码设备内部挂接扫码器的初始化数据结构定义
struct tagSMInnerDeviceInit
{
	BYTE					DeviceID;		//设备编码ID
	tagCOMParam				COM;			//串口参数结构
};

//智能扫码设备内部挂接扫码器的状态数据结构定义
struct tagSMInnerDeviceState
{
	BYTE		DeviceID;				//设备编码ID
	BYTE		StateType;				//状态类型(0,扫码器;1,通讯串口)
	BOOL		IsInit;					//是否初始化
	BOOL		IsReadVer;				//是否读取版本信息
	BOOL		IsSetFactory;			//是否已经恢复出厂
	BOOL		IsSetPersion;			//是否已经设置个性化
	int			StateCode;				//状态提示码
	TCHAR		StateDesc[512];			//状态具体提示信息
	
	//扫码相关变量
	int			ScanResult;				//扫码状态(0,成功；其它，失败)
	int			ScanTask;				//扫码具体步骤标记(1,准备扫描,并计时;2,获取规格化命令数据;3,命令已发送，进行扫码;4,扫码默认等待超时，命令重发;5,扫码结束)
	BYTE		RecScanCmdCount;		//设备同一轮扫码操作过程当中，接收扫码命令的次数
	
	DWORD		ScanConsumeTime;		//扫码花费的时间，单位：毫秒
	SYSTEMTIME	BeginTime;				//扫码起始时间
	SYSTEMTIME	EndTime;				//扫码结束时间
	TCHAR		OriginalResult[512];	//扫码原始结果
	TCHAR		Code[512];				//扫码格式化后的数据
	BYTE		CodeType;				//初步判断扫码结果来源
	
	//心跳相关变量
	BYTE		HeartFaileCount;		//设备心跳失败次数
	BOOL		LastDeviceState;		//设备上次心跳状态
	
	//初始化相关变量
	time_t		LastInitTime;			//设备上次的初始化时间
	time_t		LastRecordCOMStateTime;	//上次尝试发送串口状态的时间
	time_t		LastReadVerTime;		//上次尝试读取版本的时间

	//版本相关变量
	TCHAR		ProductName[128];		//设备内部版本全称(格式如下 设备编码ID,版本;设备编码ID,版本;)
	TCHAR		BootRevision[128];		//启动版本信息(格式如下 设备编码ID,版本;设备编码ID,版本;)
	TCHAR		SerialNumber[128];		//设备序列号(格式如下 设备编码ID,序列号;设备编码ID,序列号;)
	TCHAR		FirmwareVersion[128];	//设备固件版本(格式如下 设备编码ID,版本;设备编码ID,版本;)
};

//智能扫码设备内部挂接扫码器的相关数据结构定义集合
class CSMInnerDeviceInfo
{
public:
	CSMInnerDeviceInfo(void)
	{
		DeviceID					= 0;

		SMType						= 0;
		EffectiveDataLen			= 0;
		SingleReadDataSpanTime		= 0;
		SMRoundTimeOutValue			= 0;
		SMDefaultScanTimeOutValue	= 0;

		HasProcessDeviceInitResultCount = 0;
		HasProcessDeviceVerResultCount	= 0;
		HasProcessDeviceCommResultCount = 0;

		SMInitList.RemoveAll();
		SMStateList.RemoveAll();
	}
	virtual ~CSMInnerDeviceInfo(void){}
//Attributes
public:
	BYTE	DeviceID;					//设备编码ID
	DWORD	SMType;						//扫码器类型
	
	DWORD	EffectiveDataLen;			//有效数据长度
	DWORD	SingleReadDataSpanTime;		//单次串口读取间隔时间，单位：毫秒
	DWORD	SMRoundTimeOutValue;		//每轮扫码持续时间,单位：毫秒
	DWORD	SMDefaultScanTimeOutValue;	//扫码器默认读取等待超时，单位：毫秒

	DWORD	HasProcessDeviceCommResultCount;//执行普通命令后已处理回馈信息的设备数	
	DWORD	HasProcessDeviceInitResultCount;//执行初始化命令后已处理回馈信息的设备数	
	DWORD	HasProcessDeviceVerResultCount; //执行读版本命令后已处理回馈信息的设备数	
	//设备的初始化信息
	CArray<tagSMInnerDeviceInit,tagSMInnerDeviceInit&>	SMInitList;
	//设备的状态信息
	CArray<tagSMInnerDeviceState,tagSMInnerDeviceState&>SMStateList;
};

*/




#define COLOR_PAY_WAIT				RGB(145,40,140)		//扣款等待
#define COLOR_PAY_FAIL				RGB(255,0,0)		//扣款失败
#define COLOR_PAY_SUCCESS			RGB(0,128,64)		//扣款成功

/*******************************************************普通宏定义********************************************************************/
#define OP_SUCCESS	0							//执行成功 -- 函数返回值非布尔型的话，必须要校验返回值

//AES密钥
#define AES_ENC_KEY_DATA	_T("1111222233334444")

/*******************************************************消息定义********************************************************************/

//定义各个线程消息的起始索引
#define	MSG_START_ID_SQL		0		//SQL线程处理消息起始索引号
#define	MSG_START_ID_SM			100		//扫码线程处理消息起始索引号
#define	MSG_START_ID_IC			200		//读写器线程处理消息起始索引号
#define	MSG_START_ID_EQUIPEMENT	300		//IO外设线程处理消息起始索引号
#define MSG_START_ID_HTTP		400		//Http通讯线程处理消息起始索引号
#define MSG_START_ID_PLATE		500		//车牌识别仪线程消息起始索引号
#define MSG_START_ID_VDM		600		//VDM处理线程消息起始索引号
#define MSG_START_ID_TFI		700		//费显处理线程消息起始索引号
#define MSG_START_ID_REMOTE		800		//远程调图类消息起始索引号
#define MSG_START_ID_AXIS		900		//计重线程消息起始索引号
#define MSG_START_ID_SOCKET		1000	//Socket通讯相关消息起始索引
#define MSG_START_ID_DM			1100	//核心数据管理线程相关消息起始索引
#define MSG_START_ID_MAIN_UI	1200	//主界面相关消息起始索引号


//Http线程类处理的消息
#define WM_THREAD_HTTP_SEND_DATA_REQUEST			WM_APP + MSG_START_ID_HTTP + 1	//发送查询请求消息定义
#define WM_WND_DEAL_PLATFORM_RESPONSE_RESULT		WM_APP + MSG_START_ID_HTTP + 2	//回馈窗体Http平台服务器处理结果
#define WM_THREAD_HTTP_RESEND_DATA_REQUEST			WM_APP + MSG_START_ID_HTTP + 3	//重发数据


//运营平台通讯类处理的消息
#define	WM_WND_DEAL_RESPONSE_FOR_ENTRY_QUERY		WM_APP + MSG_START_ID_HTTP + 3	//从HTTP线程接收到【入口查询】的回馈结果
#define	WM_WND_DEAL_RESPONSE_FOR_EXIT_QUERY			WM_APP + MSG_START_ID_HTTP + 4	//从HTTP线程接收到【出口查询】的回馈结果
#define WM_WND_DEAL_RESPONSE_FOR_OPEN_LOOP			WM_APP + MSG_START_ID_HTTP + 5	//从HTTP线程接收到【入口开环操作】的回馈结果
#define WM_WND_DEAL_RESPONSE_FOR_CLOSE_LOOP			WM_APP + MSG_START_ID_HTTP + 6	//从HTTP线程接收到【出口闭环操作】的回馈结果
#define WM_WND_DEAL_RESPONSE_FOR_DEBIT				WM_APP + MSG_START_ID_HTTP + 7	//从HTTP线程接收到【扣费操作】的回馈结果
#define WM_WND_DEAL_RESPONSE_FOR_DEBIT_CHECK		WM_APP + MSG_START_ID_HTTP + 8	//从HTTP线程接收到【扣费状态查询】的回馈结果
#define WM_WND_DEAL_RESPONSE_FOR_PASS_OPEN_STATION	WM_APP + MSG_START_ID_HTTP + 9	//从HTTP线程接收到【过开放式收费站】的回馈结果


//扫码器控制相关消息
#define WM_THREAD_SM_INIT_WORK				WM_APP + MSG_START_ID_SM + 25	//初始化所有的工作线程

#define WM_THREAD_SM_GET_SCAN_CODE_DATA					WM_APP + MSG_START_ID_SM + 1	//获取到扫码结果
#define WM_THREAD_SM_GET_SYSTEM_INFO					WM_APP + MSG_START_ID_SM + 2	//获取扫码器系统信息
#define WM_THREAD_SM_START_SCAN							WM_APP + MSG_START_ID_SM + 3	//强制开始扫码
#define WM_THREAD_SM_STOP_SCAN							WM_APP + MSG_START_ID_SM + 4	//初始化所有的工作线程
#define WM_THREAD_SM_RESET_MACHINE						WM_APP + MSG_START_ID_SM + 5	//初始化指定扫码器
#define WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO	WM_APP + MSG_START_ID_SM + 6	//协议控制类发送给管理线程关于扫码状态提示信息
#define WM_THREAD_SM_CONTROL_BOARD						WM_APP + MSG_START_ID_SM + 7	//控制串口板
#define WM_THREAD_SM_STATE								WM_APP + MSG_START_ID_SM + 8	//管理线程发送给UI关于扫码器的状态提示信息
#define WM_THREAD_SM_SET_LIGHT_MODE						WM_APP + MSG_START_ID_SM + 9	//控制灯光情况

#define WM_WND_NET_SM_DEBIT_TOLL						WM_APP + MSG_START_ID_SM + 10	//仅仅扣款
#define WM_WND_CHECK_DEBIT_PASSWORD_STATE				WM_APP + MSG_START_ID_SM + 11	//扣款结果查询

//新增加设置显示消息
#define WM_THREAD_SM_SET_DISPLAY						WM_APP + MSG_START_ID_SM + 12	//设置显示内容



#define FILE_SIZE_LOG_MAX	5*1024*1024		//日志文件最大值
#define DIR_NAME_DEVICE_SCAN		_T("ScanMachine")	//扫码器

#define FILE_NAME_DLL_SCAN			_T("HTSM.dll")			//扫码器控制动态库
#define FILE_NAME_HTTP_CONFIG			_T("ReflectWeb.ini")			//反射服务器配置信息文件名

#define FILE_NAME_DEVICE_SM_CONFIG			_T("HTSMConfig.ini")		//硬件配置信息文件名--扫码器
#define FILE_NAME_DEVICE_SM_CONFIG1			_T("HTSMConfig1.ini")		//硬件配置信息文件名--扫码器




//硬件分类
enum HARDWARE_TYPE
{
	HT_ICREADER		= 1,//读写器
	HT_SCANMACHINE	= 2,//扫码器
	HT_PRINTER		= 3,//打印机
	HT_VDM			= 4,//字符叠加器
	HT_TFI			= 5,//费额显示器
	HT_PLATE		= 6,//车牌识别仪
	HT_AXIS			= 7,//计重仪
};


//硬件物理接口类型
enum HT_INTERFACE_TYPE
{
	HTIF_COM	= 1,//串口
	HTIF_NET	= 2,//网口
	HTIF_LPT	= 3,//并口
	HTIF_USB	= 4,//USB
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
};


//扫码器类型
enum SCAN_MACHINE_TYPE
{
	SM_TYPE_HONEYWELL_3310G = 1,	//霍尼韦尔3310g
	SM_TYPE_NEW_LAND_FM_420 = 3,	//新大陆
};


//扫码器序列排号
enum SCAN_MACHINE_INDEX
{
	SM_INDEX_DWON_LEFT	= 0x01,	//左下扫码器(客车1）
	SM_INDEX_DWON_RIGHT	= 0x02,	//右下扫码器(客车2)
	SM_INDEX_UP_LEFT	= 0x04,	//左上扫码器(货车1)
	SM_INDEX_UP_RIGHT	= 0x08,	//右上扫码器(货车2)
	SM_INDEX_HAND		= 0x80,	//手持扫码器
};


//扫描器命令类型 //1,初始化;2,读版本;3,扫码触发;4,停止扫码;5,心跳;6,重置出厂;7,重置用户默认
enum SM_CMD_TYPE
{
	SM_CMD_UNKNOW			= 0,//未知
	SM_CMD_INIT				= 1,//初始化
	SM_CMD_VER				= 2,//读版本
	SM_CMD_START			= 3,//扫码触发
	SM_CMD_STOP				= 4,//停止扫码
	SM_CMD_HEART			= 5,//心跳
	SM_CMD_RESET_FACTORY	= 6,//重置出厂
	SM_CMD_RESET_PERSION 	= 7,//重置用户默认

	SM_FLOW_STATE_COM			= 1,//扫码器管理流程状态，串口状态描述
	SM_FLOW_STATE_INIT			= 2,//扫码器管理流程状态，初始化状态描述
	SM_FLOW_STATE_VER			= 3,//扫码器管理流程状态，版本查询状态描述
	SM_FLOW_STATE_SCAN			= 4,//扫码器管理流程状态，扫码过程状态描述
	SM_FLOW_STATE_STOP			= 5,//扫码器管理流程状态，强制停止扫码状态描述
	SM_FLOW_STATE_RESET_FACTORY	= 6,//扫码器管理流程状态，重设出厂默认参数环境状态描述
	SM_FLOW_STATE_RESET_PERSION = 7,//扫码器管理流程状态，重设个人默认参数环境状态描述
	SM_FLOA_STATE_HEART			= 8,//扫码器管理流程状态，心跳状态描述
};


//互联网支付接入平台类型
enum NET_PAY_PLATFORM_TYPE
{
	NPPT_MICRO_MSG	= 1,	//微信
	NPPT_ALI		= 2,	//支付宝
	NPPT_BAI_DU		= 3,	//百度
};




#pragma pack(1)

//车道配置信息结构体
typedef struct _tagLaneConfigInfo
{
	int		MainType;		//封闭式入出口、开放式
	int		SubType;		//ETC,MTC,自动发卡,便携收费机
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


//车道APP类向各个线程类传递基本配置信息的结构体
typedef struct _tagTranstParam
{
	//省份编码
	int					ProvinceID;
	//车道配置基本信息
	tagLaneConfigInfo	LaneConfigInfo;
	//特殊主动触发消息ID
	DWORD				ActiveNotifyMsgID;
	/******************各种路径集合******************/

	//车道初始目录绝对路径,默认为运行程序的当前目录
	TCHAR	PathCommDir[MAX_PATH];
	//生产过程产生的临时关键文件存储目录绝对路径
	TCHAR	PathDataDir[MAX_PATH];
	//车道硬件驱动目录绝对路径
	TCHAR	PathDeviceDir[MAX_PATH];
	//车道日志目录绝对路径
	TCHAR	PathLogDir[MAX_PATH];
	//车道数据备份目录绝对路径
	TCHAR	PathDataBackupDir[MAX_PATH];
	//临时文件存储目录绝对路径
	TCHAR	PathTempDir[MAX_PATH];
	//车道下载参数目录绝对路径
	TCHAR	PathDownloadDir[MAX_PATH];

	//核心数据二级目录（车道号）
	TCHAR	PathDataSecondDirForLaneID[MAX_PATH];
	//核心数据二级目录（数据）
	TCHAR	PathDataSecondDirForCore[MAX_PATH];
	//核心数据二级目录（参数）
	TCHAR	PathDataSecondDirForParam[MAX_PATH];
	//核心数据二级目录（图像）
	TCHAR   PathDataSecondDirForImage[MAX_PATH];
}tagTranstParam;


//串口参数信息结构体
typedef struct _tagCOMParam
{
	int		Port;					//串口
	int		BaudRate;				//波特率
	int		ParityBit;				//校验位
	int		StopBit;				//停止位
	int		DataBit;				//数据位

	DWORD	InBufferSize;			//输入缓冲区大小
	DWORD	OutBufferSize;			//输出缓冲区大小
}tagCOMParam;


//扫码工作线程初始化参数结构体
typedef struct _tagSMWorkInit
{
	DWORD		Index;						//扫码线程类索引
	DWORD		SMType;						//扫码器类型
	DWORD		ParentsThread;				//父线程
	DWORD		EffectiveDataLen;			//有效数据长度
	DWORD		SingleReadDataSpanTime;		//单次串口读取间隔时间，单位：毫秒
	DWORD		ReadTimeOutValue;			//预设串口等待超时,单位：毫秒
	DWORD		SMDefaultReadTimeOutValue;	//扫码器默认读取等待超时，单位：毫秒
	DWORD		SMOtherCMDTimeoutValue;		//扫码器除去扫码动作之外的其余命令超时时间，单位：毫秒
	DWORD		SMRecanCount;				//重扫码次数（每一轮扫码前设置为0）
	tagCOMParam COM;						//串口参数结构
}tagSMWorkInit;


//扫码器工作线程回馈结果信息结构体
typedef struct _tagSMWorkResult
{
	int			StateCode;				//扫码状态(0,成功；其它，失败)
	DWORD		ThreadID;				//回馈的线程ID
	DWORD		Index;					//扫码器索引
	DWORD		ScanConsumeTime;		//扫码花费的时间，单位：毫秒
	SYSTEMTIME	BeginTime;				//扫码起始时间
	SYSTEMTIME	EndTime;				//扫码结束时间
	TCHAR		OriginalResult[128];	//扫码原始结果
	TCHAR		Code[32];				//扫码格式化后的数据
	BYTE		CodeType;				//初步判断扫码结果来源

	BYTE		ScanTask;				//扫码具体步骤标记(1,准备扫描,并计时;2,获取规格化命令数据;3,命令已发送，进行扫码;4,扫码默认等待超时，命令重发;5,扫码结束)
	TCHAR		ScaningStateDesc[128];	//扫码过程当中的具体提示信息
}tagSMWorkResult;


//智能扫码器主板控制参数数据描述结构体
struct tagSMMainBoardParam
{
	BYTE	Type;			//数据类型(1,发送;2,回馈)
	BYTE	CMDFlag;		//命令标号
	BYTE	ParamData[512];	//参数内容
	WORD	ParamSize;		//参数长度
};


//智能扫码设备内部挂接扫码器的初始化数据结构定义
struct tagSMInnerDeviceInit
{
	BYTE					DeviceID;		//设备编码ID
	tagCOMParam				COM;			//串口参数结构
};


//智能扫码设备内部挂接扫码器的状态数据结构定义
struct tagSMInnerDeviceState
{
	BYTE		DeviceID;				//设备编码ID
	BYTE		StateType;				//状态类型(0,扫码器;1,通讯串口)
	BOOL		IsInit;					//是否初始化
	BOOL		IsReadVer;				//是否读取版本信息
	BOOL		IsSetFactory;			//是否已经恢复出厂
	BOOL		IsSetPersion;			//是否已经设置个性化
	int			StateCode;				//状态提示码
	TCHAR		StateDesc[512];			//状态具体提示信息
	
	//扫码相关变量
	int			ScanResult;				//扫码状态(0,成功；其它，失败)
	int			ScanTask;				//扫码具体步骤标记(1,准备扫描,并计时;2,获取规格化命令数据;3,命令已发送，进行扫码;4,扫码默认等待超时，命令重发;5,扫码结束)
	BYTE		RecScanCmdCount;		//设备同一轮扫码操作过程当中，接收扫码命令的次数
	
	DWORD		ScanConsumeTime;		//扫码花费的时间，单位：毫秒
	SYSTEMTIME	BeginTime;				//扫码起始时间
	SYSTEMTIME	EndTime;				//扫码结束时间
	TCHAR		OriginalResult[512];	//扫码原始结果
	TCHAR		Code[512];				//扫码格式化后的数据
	BYTE		CodeType;				//初步判断扫码结果来源
	
	//心跳相关变量
	BYTE		HeartFaileCount;		//设备心跳失败次数
	BOOL		LastDeviceState;		//设备上次心跳状态
	
	//初始化相关变量
	time_t		LastInitTime;			//设备上次的初始化时间
	time_t		LastRecordCOMStateTime;	//上次尝试发送串口状态的时间
	time_t		LastReadVerTime;		//上次尝试读取版本的时间

	//版本相关变量
	TCHAR		ProductName[128];		//设备内部版本全称(格式如下 设备编码ID,版本;设备编码ID,版本;)
	TCHAR		BootRevision[128];		//启动版本信息(格式如下 设备编码ID,版本;设备编码ID,版本;)
	TCHAR		SerialNumber[128];		//设备序列号(格式如下 设备编码ID,序列号;设备编码ID,序列号;)
	TCHAR		FirmwareVersion[128];	//设备固件版本(格式如下 设备编码ID,版本;设备编码ID,版本;)
};


struct tagScanDisplay
{
	int		iSerial;		//序列号，用于区分不用的显示命令，加1递增
	int		Type;			//显示类型		1-包含金额，重量，车型；	2-仅仅包含当前时间；3-包含金额，重量，车型,并启动扫码；
	DWORD	Money;			//金额，单位分
	DWORD	Weight;			//重量，单位千克
	int		VehicleType;	//车种	1客2货
	int		VehicleClass;	//车型，1~5
	time_t	tShowtime;		//显示时间
	int		LimitWeight;	//限重
	int		Balance;		//余额
	int		AxisCount;		//轴数
	TCHAR	EntrySTName[32];//入口站点
};



#pragma pack()



//智能扫码设备内部挂接扫码器的相关数据结构定义集合
class CSMInnerDeviceInfo
{
public:
	CSMInnerDeviceInfo(void)
	{
		DeviceID					= 0;

		SMType						= 0;
		EffectiveDataLen			= 0;
		SingleReadDataSpanTime		= 0;
		SMRoundTimeOutValue			= 0;
		SMDefaultScanTimeOutValue	= 0;

		HasProcessDeviceInitResultCount = 0;
		HasProcessDeviceVerResultCount	= 0;
		HasProcessDeviceCommResultCount = 0;

		SMInitList.RemoveAll();
		SMStateList.RemoveAll();
	}
	virtual ~CSMInnerDeviceInfo(void){}
//Attributes
public:
	BYTE	DeviceID;					//设备编码ID
	DWORD	SMType;						//扫码器类型
	
	DWORD	EffectiveDataLen;			//有效数据长度
	DWORD	SingleReadDataSpanTime;		//单次串口读取间隔时间，单位：毫秒
	DWORD	SMRoundTimeOutValue;		//每轮扫码持续时间,单位：毫秒
	DWORD	SMDefaultScanTimeOutValue;	//扫码器默认读取等待超时，单位：毫秒

	DWORD	HasProcessDeviceCommResultCount;//执行普通命令后已处理回馈信息的设备数	
	DWORD	HasProcessDeviceInitResultCount;//执行初始化命令后已处理回馈信息的设备数	
	DWORD	HasProcessDeviceVerResultCount; //执行读版本命令后已处理回馈信息的设备数	
	//设备的初始化信息
	CArray<tagSMInnerDeviceInit,tagSMInnerDeviceInit&>	SMInitList;
	//设备的状态信息
	CArray<tagSMInnerDeviceState,tagSMInnerDeviceState&>SMStateList;
};



