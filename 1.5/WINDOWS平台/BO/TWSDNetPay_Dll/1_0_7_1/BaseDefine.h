#ifndef _TRAWE_EMBEDDED_BASE_DEFINE_H_
#define _TRAWE_EMBEDDED_BASE_DEFINE_H_

#pragma once

#include <io.h>

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
#include "SQLite3Base.h"

#include "json.h"
#ifdef _DEBUG
#pragma  comment(lib, "json_vc71_libmtd.lib")  
#else
#pragma  comment(lib, "json_vc71_libmt.lib")  
#endif

#include "AesECB.h"

//AES密钥
#define AES_ENC_KEY_DATA 	_T("2016042010012006")

#define AES_ENC_KEY_DATA_11	_T("2016020112532123")
#define AES_ENC_KEY_DATA_12	_T("2015102303451422")
#define AES_ENC_KEY_DATA_13	_T("2016102303231452")
#define AES_ENC_KEY_DATA_14	_T("2015090812451157")
#define AES_ENC_KEY_DATA_15	_T("2015090758413045")

#define AES_ENC_KEY_DATA_31	_T("2016102801020010")
#define AES_ENC_KEY_DATA_32	_T("2016102301020232")
#define AES_ENC_KEY_DATA_33	_T("2016102203402343")
#define AES_ENC_KEY_DATA_34	_T("2016110101020010")
#define AES_ENC_KEY_DATA_35	_T("2015110171020010")
#define AES_ENC_KEY_DATA_36	_T("2015110651033430")
#define AES_ENC_KEY_DATA_37	_T("2016110102040570")

#define AES_ENC_KEY_DATA_41	_T("2015012317102009")
#define AES_ENC_KEY_DATA_42	_T("2015022317102012")
#define AES_ENC_KEY_DATA_43	_T("2015032317102032")
#define AES_ENC_KEY_DATA_44	_T("2016081820151023")
#define AES_ENC_KEY_DATA_45	_T("2015042317102056")

#define AES_ENC_KEY_DATA_50	_T("2016101212451871")
#define AES_ENC_KEY_DATA_51	_T("2016101312231871")
#define AES_ENC_KEY_DATA_52	_T("2016101412442171")
#define AES_ENC_KEY_DATA_53	_T("2016101512425121")
#define AES_ENC_KEY_DATA_54	_T("2016101612432214")

#define AES_ENC_KEY_DATA_61	_T("2016100158471254")
#define AES_ENC_KEY_DATA_62	_T("2016100258234212")
#define AES_ENC_KEY_DATA_63	_T("2016100358232124")
#define AES_ENC_KEY_DATA_64	_T("2016100422412332")
#define AES_ENC_KEY_DATA_65	_T("2016100512451234")


#define FILE_NAME_DEVICE_DLL	_T("HTTWSDDevice.dll")
#define FILE_NAME_APP_CONFIG	_T("TWSDNetPayConfig.ini")
#define FILE_NAME_LOG_CONFIG	_T("LogConfig.ini")
#define FILE_NAME_HTTP_CONFIG	_T("")
#define FILE_NAME_DEVICE_CONFIG _T("HTSMConfig1.ini")
#define FILE_NAME_UPDATE_CONFIG	_T("TWPayITMS.ini")

#define OP_SUCCESS 0

//#define DEFAULT_TIME			978278400	// 2001-01-01
#define DEFAULT_TIME			0			// 1970-01-01 08:00:00 (BeiJing time)

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
#define WM_THREAD_DEVICE_CONTROL_DEAL_RESULT_MMI		WM_APP + 12
#define WM_THREAD_DEVICE_START_DEBIT_FLOW				WM_APP + 13

#define WM_THREAD_UPDATE_RESPONSE						WM_APP + 20
#define WM_THREAD_UPDATE_CONFIRM						WM_APP + 21

#define WM_THREAD_NFC_ISSUE_DATA						WM_APP + 31
#define WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_HTTP		WM_APP + 32
#define WM_THREAD_NFC_SIGN_DATA_RETURN_FROM_POS			WM_APP + 33
#define WM_THREAD_NFC_START_SIGN						WM_APP + 34

//JSON数据格式的类型区分
enum JSON_DATA_TYPE
{
	JSON_DT_SEND_DIBIT			= 3,	//扣费请求
	JSON_DT_SEND_DIBIT_CHECK	= 4,	//扣费查询
	JSON_DT_SEND_DIBIT_CANCEL	= 5,	//撤销订单
	JSON_DT_SEND_PASS_STATION	= 8,	//过开放收费站操作
	JSON_DT_SEND_HEART			= 9,	//心跳查询
	JSON_DT_SEND_DIBIT_SECOND	= 10,	//二次扣款
	JSON_DT_SEND_NFC_SIGN_IN	= 11,	//NFC模块签到

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
	HTTP_RC_DEBIT_QUERY_ERROR			= 21,	//扣费后续查询失败（扣费执行中，暂无结果）
	HTTP_RC_NFC_NO_SUPPORT				= 22,	//NFC,卡片不支持闪付
	HTTP_RC_NFC_NO_INIT					= 23,	//NFC,卡片未初始化
	HTTP_RC_NFC_NO_SERVICES				= 24,	//NFC,银联闪付暂停服务
	HTTP_RC_NFC_NEED_SIGN				= 25,	//NFC,需要重新进行签到
};

//扫码灯控制状态
enum SM_PROTOCOL_CONST
{
	SM_PRPTOCOL_LIGHT_SUCCESS		= 0x00,	//提示扫码成功
	SM_PRPTOCOL_LIGHT_SCAN			= 0x01, //提示正在扫码
	SM_PRPTOCOL_LIGHT_FAIL			= 0x02,	//提示扫码失败
	SM_PRPTOCOL_LIGHT_ALL_OFF		= 0x03, //灭灯

	SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT= 0x04,	//请输入密码
	SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR	= 0x05,	//密码错误
	SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1	= 0x06,	//二维码非法
	SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2	= 0x07,	//二维码超时
	SM_PRPTOCOL_LIGHT_FALI_BL_ERR	= 0x08,	//余额不足
	SM_PRPTOCOL_LIGHT_FALI_NET_ERR	= 0x09,	//网络超时	
	SM_PRPTOCOL_LIGHT_FALI_PL_STOP	= 0x0A,	//暂停服务	
	
	SM_PRPTOCOL_LIGHT_FALI_NFC_01	= 0x0B,	//NFC,卡片不支持闪付
	SM_PRPTOCOL_LIGHT_FALI_NFC_02	= 0x0C,	//NFC,卡片未初始化	
	SM_PRPTOCOL_LIGHT_FALI_NFC_03	= 0x0D,	//NFC,银联闪付暂停服务	
};

//定时器ID
enum TIMER_ID
{
	TIMER_ID_DEBIT_MONEY			= 1,
	TIMER_ID_SCAN_CODE				= 2,
	TIMER_ID_DEBIT_CHECK			= 3,
	TIMER_ID_NFC_WAIT_NORMAL_DATA	= 4,
	TIMER_ID_NFC_WAIT_CORRECT_DATA	= 5,
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
	RCC_DEBIT_QUERY_ERROR	= 0x0D,		//扣费查询失败

	RCC_INNER_ERROR			= 0xFF,		//内部错误
};

//支付渠道类型
enum PAY_CHANNEL_TYPE_CODE
{
	PCTC_WENXIN		= 0x01,		//微信当面付
	PCTC_ALIIPAY	= 0x02,		//支付宝当面付
	PCTC_BAIDU		= 0x03,		//百度支付
	PCTC_JD			= 0x04,		//京东支付
	PCTC_UNIPAY_QR	= 0x05,		//银联二维码支付
	PCTC_UNIPAY_NFC	= 0x06,		//银联NFC支付
	
	PCTC_ALIIPAY_AGENCY	= 51,		//支付宝代扣
	PCTC_WEIXIN_AGENCY	= 52,		//微信代扣
	PCTC_BAIDU_AGENCY	= 53,		//百度代扣
	PCTC_JD_AGENCY		= 54,		//京东代扣
	PCTC_UNIPAY_AGENCY	= 55,		//银联代扣
};

//NFC模块工作状态
enum UNIPAY_MODULE_STATE
{
	UMS_UNKNOW		= 0x00,
	UMS_SIGNING		= 0x01,				//签到
	UMS_DEBITING	= 0x02,				//开始读卡
	UMS_CORRECT		= 0x03,				//冲正
	UMS_STOP		= 0x04,				//停止读卡
	UMS_TEST_LINK	= 0x05,				//链路测试
};

//NFC协议帧类型
enum UNIPAY_FRAME_TYPE
{
	UFT_UNKNOW			= 0x00, //未知指令
	//公共指令
	UFT_NAK				= 0x15,	//拒绝响应
	UFT_ACK				= 0x06, //允许响应
//	UFT_DATA			= 0x60, //数据

	//上位机指令
	UFT_M_SIGN			= 0x01,	//发起签到操作
	UFT_M_CONSUME_W1	= 0x02,	//发起消费（查询消费类型）
	UFT_M_CONSUME_W2	= 0x03,	//要求执行消费（具体W2类型）
	UFT_M_CONSUME_W3	= 0x04,	//要求执行消费（具体W3类型）
	UFT_M_CORRECT		= 0x05,	//发起冲正
	UFT_M_STOP			= 0x06, //停止执行上一次命令
	UFT_M_TEST_LINK		= 0x07, //测试链路指令

	//非接模块（NFC）回馈数据
	UFT_N_CONSUME_RES_W2	= 0x11,	//消费类型查询回馈（银行卡消费）
	UFT_N_CONSUME_RES_W3	= 0x12, //消费类型查询回馈（POS通消费）
	UFT_N_CONSUME_TIP		= 0x13, //消费信息提示(NFC模块回馈)
	UFT_N_CONSUME_RES_DATA	= 0x14, //消费数据
	UFT_N_CONSUME_ERR		= 0x15, //消费异常（刷卡或组装报文出错）
	UFT_N_CONSUME_NO_SIGN	= 0x16, //未签到

	UFT_N_CORRECT_RES_DATA  = 0x17, //冲正数据
	
	UFT_N_SIGN_IN_RES_DATA	= 0x18, //签到数据
	UFT_N_SIGN_VERIFY_ERR	= 0x19, //签到数据校验错
	UFT_N_SIGN_CONFIRM		= 0x1A, //签到确认
	
	UFT_N_TEST_LINK_SUCCESS	= 0x1B, //回馈测试成功
	UFT_N_TEST_LINK_ERROR	= 0x1C, //回馈测试失败
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
//	int		StationID;		//站点编码
	TCHAR	StationID[36];	//站点编码
	TCHAR	LaneID[8];		//车道编码
	int		LaneType;		//车道类型(1,入口;2,出口)
	TCHAR	LaneTab[16];	//车道唯一索引
	TCHAR	StationTab[32]; //站点唯一索引
	int		STServerTpye;	//站级服务器类型
	TCHAR	STServerIP[24];	//站级服务器IP地址
	TCHAR	STServerID[24];	//站级服务器ID编码
	TCHAR	WebServerIP[24];//内网反射服务器IP地址
}tagLaneConfigInfo;

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

//MMI参数的媒体参数结构信息
typedef struct _tagMMIMedia
{
	int		Type;
	TCHAR	Content[128];
	int		Spare;
}tagMMIMedia;




#pragma pack(1)

//更新信息
struct tagUpdateInfo
{
	int		Type;			//更新类型：1-文字广告,2-语言广告
	char	InfoNo[32];		//更新编码
	char	InfoName[64];	//更新名称
	char	InfoVersion[32];//更新版本
	time_t	UpdateTime;		//信息修改时间
	time_t	LoadTime;		//信息下发时间
	time_t	BeginDate;		//启用日期
	time_t	EndDate;		//停用日期
	int		IsCondense;		//是否压缩：0-不压缩，1-压缩
	int		FileType;		//文件类型：1-文本
	char	Content[256];	//文件内容
	char	MD5[40];		//MD5
	int		TransferMark;	//传输标记
	int		Spare;			//备用
};

#pragma pack()


#endif //_TRAWE_EMBEDDED_BASE_DEFINE_H_




//更新数据基础类
class CUpdateData
{
public:
	CUpdateData()
	{
		csVersion		= _T("");
		csPacktype		= _T("");
		tPackTime		= 0;
		ProvinceID		= 0;
		RoadID			= 0;
		csStationID		= _T("");
		LaneID			= 0;

		csDeviceNo		= _T("");
		csDeviceType	= _T("");
		UpdateCode		= _T("");
		csAEScode		= _T("");
		csMD5code		= _T("");

		ErrorCount		= 0;

		m_aryUpdateInfo.RemoveAll();
	}

	virtual ~CUpdateData()
	{
		m_aryUpdateInfo.RemoveAll();
	}

	CUpdateData& operator=(CUpdateData &c_update)
	{
		this->csVersion		= c_update.csVersion;
		this->csPacktype	= c_update.csPacktype;
		this->tPackTime		= c_update.tPackTime;
		this->ProvinceID	= c_update.ProvinceID;
		this->RoadID		= c_update.RoadID;
		this->csStationID	= c_update.csStationID;
		this->LaneID		= c_update.LaneID;

		this->csDeviceNo	= c_update.csDeviceNo;
		this->csDeviceType	= c_update.csDeviceType;
		this->UpdateCode	= c_update.UpdateCode;
		this->csAEScode		= c_update.csAEScode;
		this->csMD5code		= c_update.csMD5code;

		this->ErrorCount	= c_update.ErrorCount;


		this->m_aryUpdateInfo.Copy(c_update.m_aryUpdateInfo);

		return *this;
	}

public:
	CString	csVersion;		//协议版本号
	CString	csPacktype;		//报文类型
	time_t	tPackTime;		//报文时间
	int		ProvinceID;		//省份编码
	int		RoadID;			//路段编码
	CString	csStationID;	//站点编码
	int		LaneID;			//车道编码
	CString	csDeviceNo;		//设备编码
	CString	csDeviceType;	//设备类型
	CString	UpdateCode;		//更新请求唯一码
	CString	csAEScode;		//AES
	CString	csMD5code;		//MD5

	int		ErrorCount;		//下载过程中的出错计数

	CArray<tagUpdateInfo, tagUpdateInfo&>m_aryUpdateInfo;	//更新信息队列
};
