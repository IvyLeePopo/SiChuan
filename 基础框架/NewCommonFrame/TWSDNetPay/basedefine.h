#ifndef BASEDEFINE_H
#define BASEDEFINE_H

#include "time.h"
#include "QString"

#define MAX_PATH          260



#define UNDER_QUEUE_NAME  "/ScanMachineMs"
#define QUEUE_MAX_SIZE    1024

//AES密钥
#define AES_ENC_KEY_DATA 	  "2016102301020232"
#define AES_ENC_KEY_DATA_63	  "1111222233334444"

#define AES_ENC_KEY_DATA_36       "2015110651033430"
#define AES_ENC_KEY_DATA_32       "2016102301020232"
#define AES_ENC_KEY_DATA_44	  "2016081820151023"
#define AES_ENC_KEY_DATA_51       "2016101312231871"
#define AES_ENC_KEY_DATA_61       "2016100158471254"

#define FILE_NAME_DEVICE_DLL	"/lib/libScanMachine_Dll.so.1.0.0"
#define FILE_NAME_APP_CONFIG	"TWSDNetPayConfig.ini"
#define FILE_NAME_LOG_CONFIG	"LogConfig.ini"

//日志保存超时
#define  LOGOUTTIME   (5L*24*60*60)
//日志路径
#define  LOGOUTPATH   ("/home/xgssoft/log/")
//日志超时
#define  LOGOUTCHECKTIMER   (60L*60*1000)

//重传时间间隔
#define  RETRANTIMEINTERVAL   (3*1000)  // 3秒


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



//JSON数据格式的类型区分
enum JSON_DATA_TYPE
{
    JSON_DT_SEND_DIBIT		= 3,	//扣费请求
    JSON_DT_SEND_DIBIT_CHECK	= 4,	//扣费查询
    JSON_DT_SEND_DIBIT_CANCEL	= 5,	//撤销订单


    JSON_CMD_TYPE_DEBIT_EXIT     = 1,	//封闭式出口扣费命令
    JSON_CMD_TYPE_DEBIT_OPEN     = 2,	//开放式扣费指令
    JSON_CMD_TYPE_DEBIT_ONLY_PAY = 3,	//纯扣款命令

    JSON_DT_PARAM_ACCOUNT	 = 0xFF01,	//用户参数JSON格式
    JSON_DT_PARAM_DEBIT_MONEY	 = 0xFF02,	//扣款参数JSON格式
    JSON_DT_PARAM_DEBIT_CANCLE	 = 0xFF03,	//撤单参数JSON格式
    JSON_DT_PARAM_DEBIT_RESULT	 = 0xFF04,	//扣款结果参数JSON格式
    JSON_DT_PARAM_MMI		 = 0xFF05	//交互界面参数JSON格式
};



//智能设备故障状态描述
enum DEVICE_STATE_DESC
{
    DEVICE_STATE_LOAD                    = 0x0001,	//驱动加载失败
    DEVICE_STATE_SERIAL_OPEN        = 0x0002,	//设备串口无法打开
    DEVICE_STATE_SERIAL_COMM      = 0x0004,	//设备串口通讯故障
    DEVICE_STATE_PAY_WEB               = 0x0008,	//支付子平台网络故障
    DEVICE_STATE_PAY_SERVER           = 0x0010,	//支付子平台服务器故障
    DEVICE_STATE_4G                         = 0x0020        //云端通讯设备故障
};



//车道配置信息结构体
typedef struct _tagLaneConfigInfo
{
    int		MainType;		//封闭式入出口、开放式
    int		SubType;                   //ETC,MTC,自动发卡,便携收费机
    int		AreaID;			//区域编码(如果有)
    int		RoadID;			//路段编码(如果有)
    long long 		StationID;         //站点编码
    char    str_StationID[100];       //站点编码--字符串形式
    int		LaneID;			//车道编码
    int		LaneType;                   //车道类型(1,入口;2,出口)
    char            LaneTab[16];            	//车道唯一索引
    char            StationTab[32];       //站点唯一索引
    int		STServerTpye;           //站级服务器类型
    char            STServerIP[24];             //站级服务器IP地址
    char            STServerID[24];             //站级服务器ID编码

    int               WebServerPort;         //反射服务器开放端口
    char            WebServerIP[24];        //内网反射服务器IP地址

    int		ConfigLocalIPMode;		//TR300平台网络IP设置模式(0,设置IP;1,自动获取)
    char	ConfigLocalIP0[24];		//TR300平台网口0的IP
    char	ConfigLocalSub0[24];	//TR300平台网口0的子网掩码
    char	ConfigLocalGate0[24];	//TR300平台网口0的网关
    char	ConfigLocalIP1[24];		//TR300平台网口1的IP
    char	ConfigLocalSub1[24];	//TR300平台网口1的子网掩码
    char	ConfigLocalGate1[24];	//TR300平台网口1的网关
    char	ConfigLocalDefGate[24];	//TR300平台默认网关
    int       AutoUpdatePort;                         //自动升级端口号


}tagLaneConfigInfo;

//车道APP类向各个线程类传递基本配置信息的结构体
typedef struct _tagTranstParam
{
    //省份编码
    int		        ProvinceID;
    //车道配置基本信息
    tagLaneConfigInfo	LaneConfigInfo;
    //特殊主动触发消息ID
    unsigned int	ActiveNotifyMsgID;
    //调用者线程ID
    unsigned int	CallerThreadID;

    /******************各种路径集合******************/

    //车道初始目录绝对路径,默认为运行程序的当前目录
    char	PathCommDir[MAX_PATH];
    //生产过程产生的临时关键文件存储目录绝对路径
    char	PathDataDir[MAX_PATH];
    //车道日志目录绝对路径
    char	PathLogDir[MAX_PATH];
}tagTranstParam;


//用户信息结构体
typedef struct _tagAccountInfo
{
    int		        DataType;                      //数据类型(低字定义：1,纯数字字符串;2,暂未定义；高字定义：扫码器索引)
    int		        SrcType;                        //数据来源(1,扫码器;2,暂未定义)
    unsigned int        	DataSize;                      //数据长度
    char                     DataContext[5120];      //数据长度(以 NULL 结束的字符串)
}tagAccountInfo;


//定时器ID
enum TIMER_ID
{
    TIMER_ID_DEBIT_MONEY = 1,
    TIMER_ID_DEVICE_STATE ,
    TIMER_ID_THREAD_DEVICE ,
    TIMER_ID_THREAD_MANAGE,
    TIMER_ID_DELETE_LOG_FILE ,
    TIMER_ID_INITCONFIG_DEVICE ,
    TIMER_ID_INIT_DEVICE
};


//交易关键信息
typedef struct _tagPayKeyItems
{
    //是否有效
    bool	Valid;
    //超时等待时间 (单位：毫秒)
    int		OverTime;
    //扣款模式
    int		OperationMode;  // 1: 整合操作, 2: 单体操作

    /*****************Debit详细项************************/
    //交易识别码
    QString   	PayIdentifier;
    //交易金额(单位：分)
    int		Money;

    int    Balance;
    //过车时间
   // time_t	SubTime;
    QString SubTime;
    //交易主分类
    int		DebitMainType;

    /*****************Vehicle详细项************************/
    //车种
    int		VehicleType;
    //车型
    int		VehicleClass;
    //车牌颜色
    int		VLColor;
    //车牌
    QString	VehicleLicense;
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
    QString CardID;
    //统计日
    int		TollDate;
    //统计班次
    int		ShiftID;
    //打印票据号
    QString TicketNo;

    //收费员姓名
    QString OperatorName;
    //收费员工号
    QString OperatorID;


    /*****************Entry详细项************************/
    //入口站点
    int		EntryStationID;
    //入口站点----字符串形式
    QString   str_EntryStationID;
    //入口站名
    QString     EntryStationName;
    //入口时间
    //time_t	EntryTime;
    QString     EntryTime;
    //入口车道
    int		EntryLane;
    //入口车牌
    QString 	EntryVehicleLicense;
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
    bool	Valid;

    int		ResultType;
    int		ExecuteCode;
    int		PayPlatformType;
    QString     DebitTime;
    int                 ConsumeTime;
    char	PayIdentifier[128];
    char	DebitOrder[128];
    char	ExecuteDesc[256];
    char	TradeSecurityCode[64];
}tagDebitResultItems;


//回馈调用者错误代码
enum RESPONSE_CALLER_CODE
{
    RCC_OK					= 0x00,		//成功
    RCC_INPUT_PASSWORD		= 0x01,		//用户正在输入密码
    RCC_NO_MONEY                          = 0x02,		//用户余额不足
    RCC_QRCODE_ILLEGAL                = 0x03,		//二维码非法
    RCC_QRCODE_OT                         = 0x04,		//二维码有效期超时
    RCC_NO_ACCOUNT_INFO		= 0x05,		//智能设备未能获取用户信息
    RCC_DEBIT_OT                             = 0x06,		//扣款超时
    RCC_PSW_ERROR                          = 0x07,		//密码输入错误
    RCC_PARAM_ERROR			= 0x08,		//传入参数有误
    RCC_LAST_TRADE_NOT_END         = 0x09,		//上次交易未完成
    RCC_COM_NOT_INIT                    = 0x0A,		//组件未初始化
    RCC_COM_ILLEGAL			= 0x0B,		//组件未经授权
    RCC_PAY_PLATFORM_ERROR	= 0x0C,		//支付平台服务器故障
    RCC_DEVICE_INIT_ERROR	= 0x0D,		//智能硬件初始化失败
    RCC_BREAK_OPERATION     =0xFE,      //取消交易
    RCC_INNER_ERROR			= 0xFF		//内部错误

};

//MMI参数的文字参数结构信息
typedef struct _tagMMIItemText
{
    int	Type;
    int     Sub;
    int     Row;
    bool  IsScroll;
    char Context[48];
}tagMMIItemText;


//MMI参数的声音参数结构信息
typedef struct _tagMMIItemVoice
{
    int	Type;
    int     No;
}tagMMIItemVoice;

//MMI参数的图像参数结构信息
typedef struct _tagMMIItemGraph
{
    int	Type;
    int     No;
}tagMMIItemGraph;


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
    RDM_PARAM_TT_DATA				= 306		//透传数据到指定串口
};

//回馈调用者功能分类代码
enum RESPONSE_CALLER_FUN_CODE
{
    RCC_NONE			= 0x00,		//未知类型
    RCC_FUN_DEBIT		= 0x01,		//扣款回馈
    RCC_FUN_CANCEL		= 0x02    ,      //撤单回馈

    RCC_FUN_DEVICE_INIT    = 0xB5             //初始化成功回馈
};


#endif // BASEDEFINE_H
