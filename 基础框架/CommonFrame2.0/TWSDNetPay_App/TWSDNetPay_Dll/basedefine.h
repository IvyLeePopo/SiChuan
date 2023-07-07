#ifndef BASEDEFINE_H
#define BASEDEFINE_H

#include <QString>
#include <time.h>

#define MAX_PATH          260
#define UNDER_QUEUE_NAME  "/ScanMachineMs"
#define QUEUE_MAX_SIZE    1024

//AES密钥
#define AES_ENC_KEY_DATA 	"2016042010012006"

#define AES_ENC_KEY_DATA_11	"2016020112532123"
#define AES_ENC_KEY_DATA_12	"2015102303451422"
#define AES_ENC_KEY_DATA_13	"2016102303231452"
#define AES_ENC_KEY_DATA_14	"2015090812451157"
#define AES_ENC_KEY_DATA_15	"2015090758413045"

#define AES_ENC_KEY_DATA_31	"2016102801020010"
#define AES_ENC_KEY_DATA_32	"2016102301020232"
#define AES_ENC_KEY_DATA_33	"2016102203402343"
#define AES_ENC_KEY_DATA_34	"2016110101020010"
#define AES_ENC_KEY_DATA_35	"2015110171020010"
#define AES_ENC_KEY_DATA_36	"2015110651033430"
#define AES_ENC_KEY_DATA_37	"2016110102040570"

#define AES_ENC_KEY_DATA_41	"2015012317102009"
#define AES_ENC_KEY_DATA_42	"2015022317102012"
#define AES_ENC_KEY_DATA_43	"2015032317102032"
#define AES_ENC_KEY_DATA_44	"2016081820151023"
#define AES_ENC_KEY_DATA_45	"2015042317102056"

#define AES_ENC_KEY_DATA_50	"2016101212451871"
#define AES_ENC_KEY_DATA_51	"2016101312231871"
#define AES_ENC_KEY_DATA_52	"2016101412442171"
#define AES_ENC_KEY_DATA_53	"2016101512425121"
#define AES_ENC_KEY_DATA_54	"2016101612432214"

#define AES_ENC_KEY_DATA_61	"2016100158471254"
#define AES_ENC_KEY_DATA_62	"2016100258234212"
#define AES_ENC_KEY_DATA_63	"2016100358232124"
#define AES_ENC_KEY_DATA_64	"2016100422412332"
#define AES_ENC_KEY_DATA_65	"2016100512451234"

#define FILE_NAME_DEVICE_DLL	"HTTWSDDevice.dll"
#define FILE_NAME_APP_CONFIG	"TWSDNetPayConfig.ini"
#define FILE_NAME_LOG_CONFIG	"LogConfig.ini"

//日志保存超时
#define  LOGOUTTIME   (3L*24*60*60)
//日志路径
#define  LOGOUTPATH   ("/var/log/")
//日志超时
#define  LOGOUTCHECKTIMER   (60L*60*1000)

#define     APPVERSION                    "/opt/scaner/version"


//JSON数据格式的类型区分
enum JSON_DATA_TYPE
{
    JSON_DT_SEND_IDLE           = 2,    //IDLE
    JSON_DT_SEND_DIBIT			= 3,	//扣费请求
    JSON_DT_SEND_DIBIT_CHECK	= 4,	//扣费查询
    JSON_DT_SEND_DIBIT_CANCEL	= 5,	//撤销订单
    JSON_DT_SEND_PASS_STATION	= 8,	//过开放收费站操作
    JSON_DT_SEND_HEART			= 9,	//心跳查询
    JSON_DT_SEND_DIBIT_SECOND	= 10,	//二次扣款

    // quickpass support
    JSON_DT_SEND_QUICKPASS_SIGNIN   = 11,   // NFC signin
    JSON_DT_SEND_QUICKPASS_CORRECT  = 12,   // NFC correct for quickpass

    JSON_CMD_TYPE_DEBIT_EXIT     = 1,	//封闭式出口扣费命令
    JSON_CMD_TYPE_DEBIT_OPEN     = 2,	//开放式扣费指令
    JSON_CMD_TYPE_DEBIT_ONLY_PAY = 3,	//纯扣款命令

    JSON_DT_PARAM_ACCOUNT		 = 0xFF01,	//用户参数JSON格式
    JSON_DT_PARAM_DEBIT_MONEY	 = 0xFF02,	//扣款参数JSON格式
    JSON_DT_PARAM_DEBIT_CANCLE	 = 0xFF03,	//撤单参数JSON格式
    JSON_DT_PARAM_DEBIT_RESULT	 = 0xFF04,	//扣款结果参数JSON格式
    JSON_DT_PARAM_MMI			 = 0xFF05,	//交互界面参数JSON格式
};



enum JSON_SERVER_RETURN_CODE
{
    HTTP_RC_DATA_LEGAL					= 200,	//回馈数据符合规则
    HTTP_RC_SERVER_ERROR                = 0,    //服务器错误
    HTTP_RC_DATA_VALID					= 1,	//数据正常
    HTTP_RC_BLACKLIST					= 2,	//二维码绑定支付用户在特微黑名单当中
    HTTP_RC_NO_REGISTER					= 3,	//二维码绑定支付用户未注册
    HTTP_RC_NO_ENTRY_BY_ID				= 4,	//二维码绑定支付用户无有效入口信息
    HTTP_RC_ID_IS_RUNNING				= 5,	//二维码绑定支付用户已经路网有入口

    HTTP_RC_DEBIT_NEED_PASSWORD			= 6,	//需要二维码绑定支付用户输入密码
    HTTP_RC_DEBIT_CODE_TIMEOUT			= 7,	//二维码有效期超时
    HTTP_RC_DEBIT_SRC_ILLGAL			= 8,	//二维码来源非法（非指定范围内的二维码）
    HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	        = 9,	//余额不足,允许重试
    HTTP_RC_DEBIT_FAIL					= 10,	//扣费失败(多种原因，不能重新尝试扣款)
    HTTP_RC_DEBIT_SUCCESS				= 11,	//扣费成功
    HTTP_RC_DEBIT_NO_VOUCHER			        = 12,	//协议格式错误（详细错误会放到message中）
    HTTP_RC_PROTOCOL_ERROR				= 13,	//车道产生的交易凭证关联的订单信息在平台没有找到
    HTTP_RC_PAYPLATROMR_ERROR			        = 14,	//支付平台错误
    HTTP_RC_NO_CANCEL_ORDER				= 15,	//订单已过有效期，禁止撤销
    HTTP_RC_NO_SERVICES					= 16,	//暂停服务
    HTTP_RC_VEHICLE_IS_RUNNING			= 17,	//车牌已经在路网运行
    HTTP_RC_VEHICLE_NO_ENTRY			= 18,	//无法检索到有效入口
    HTTP_RC_INPUT_PASSWORD_ERROR		= 20,	//用户输入密码错误
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



//车道配置信息结构体
typedef struct _tagLaneConfigInfo
{
    int		MainType;		//封闭式入出口、开放式
    int		SubType;		//ETC,MTC,自动发卡,便携收费机
    int		AreaID;			//区域编码(如果有)
    int		RoadID;			//路段编码(如果有)
    char	   StationID[32];	//站点编码
    int		LaneID;			//车道编码
    int		LaneType;		//车道类型(1,入口;2,出口)
    char	LaneTab[16];	//车道唯一索引
    char	StationTab[32]; //站点唯一索引
    int		STServerTpye;	//站级服务器类型
    char	STServerIP[24];	//站级服务器IP地址
    char	STServerID[24];	//站级服务器ID编码
    char	WebServerIP[24];//内网反射服务器IP地址
}tagLaneConfigInfo;

//车道APP类向各个线程类传递基本配置信息的结构体
typedef struct _tagTranstParam
{
    //省份编码
    int					ProvinceID;
    //车道配置基本信息
    tagLaneConfigInfo	LaneConfigInfo;
    //特殊主动触发消息ID
    unsigned int		ActiveNotifyMsgID;
    //调用者线程ID
    unsigned int		CallerThreadID;

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
    int		        DataType;			//数据类型(低字定义：1,纯数字字符串;2,暂未定义；高字定义：扫码器索引)
    int		        SrcType;			//数据来源(1,扫码器;2,暂未定义)
    unsigned int	DataSize;			//数据长度
    char	        DataContext[1024];	        //数据长度(以 NULL 结束的字符串)
}tagAccountInfo;




//xiaoxin lei
enum THREAD_DEVICE_NOTIFY_TYPE
{
   SEND_QRCODE_NOTIFY =1,
};

//定时器ID
enum TIMER_ID
{
    TIMER_ID_DEBIT_MONEY = 1,
    TIMER_ID_SCAN_CODE	 = 2,
    TIMER_ID_DEBIT_CHECK = 3,
};



//交易关键信息
typedef struct _tagPayKeyItems
{
    //是否有效
    bool	Valid;
    //超时等待时间
    int		OverTime;
    //扣款模式
    int		OperationMode;

    // add nfc suppport
    int m_payPlatformType;
    QString m_qrCodeData;

    time_t m_consDebitTime;

    /*****************Debit详细项************************/
    //交易识别码
    QString	PayIdentifier;
    //交易金额(单位：分)
    int		Money;

    int    Balance;
    //过车时间
   // time_t	SubTime;
    QString SubTime;
    //交易主分类
    int		DebitMainType;
    //第三方支付凭证
    QString PayCertificateCode;
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

    //OperatorID/OperatorName
    QString OperatorID;
    QString OperatorName;

    /*****************Entry详细项************************/
    //入口站点
    QString		EntryStationID;
    //入口站名
    QString     EntryStationName;
    //入口时间
    //time_t	EntryTime;
    QString     EntryTime;
    //入口车道
    int		EntryLane;
    //入口车牌
    QString	EntryVehicleLicense;
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
    QString  DebitTime;
    time_t  ConsumeTime;
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


//MMI参数的文字参数结构信息
typedef struct _tagMMIItemText
{
    int	Type;
    int Sub;
    int Row;
    bool  IsScroll;
    char Context[48];
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

//收費信息回調函數
typedef void (* Fun_ChargeResults)(int option,int result);

#endif // BASEDEFINE_H
