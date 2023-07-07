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
#define OP_NO_MONEY                  0x02         //�˻�����
#define OP_QRCODE_ILLEGAL            0x03         //��ά��Ƿ�
#define OP_QRCODE_OT                 0x04         //��ά�볬����Ч��
#define OP_NO_ACCOUNT_INFO           0x05         //֧��ƾ֤�쳣
#define OP_DEBIT_OT                  0x06         //�ۿʱ
#define OP_PSW_ERR                   0x07         //�������
#define OP_PARAM_ERR                 0x08         //��������
#define OP_LAST_TRADE_NOT_END        0x09         //�ϴν���δ����
#define OP_COM_NOT_INIT              0x0A         //���δ��ʼ��
#define OP_COM_ILLEGAL               0x0B         //���У��Ƿ�
#define OP_PAY_PLATFORM_ERR          0x0C         //֧��ƽ̨�������

#define OP_DISCONNECT_TERMINAL		0xFD		//���ն�ͨѶ�쳣
#define OP_CANCLE  0xFE                           //ȡ������

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


//JSON���ݸ�ʽ����������
enum JSON_DATA_TYPE
{
	JSON_DT_SEND_DIBIT			= 3,	//�۷�����
	JSON_DT_SEND_DIBIT_CHECK	= 4,	//�۷Ѳ�ѯ
	JSON_DT_SEND_DIBIT_CANCEL	= 5,	//��������
	JSON_DT_SEND_PASS_STATION	= 8,	//�������շ�վ����
	JSON_DT_SEND_HEART			= 9,	//������ѯ
	JSON_DT_SEND_DIBIT_SECOND	= 10,	//���οۿ�

	JSON_CMD_TYPE_DEBIT_EXIT     = 1,	//���ʽ���ڿ۷�����
	JSON_CMD_TYPE_DEBIT_OPEN     = 2,	//����ʽ�۷�ָ��
	JSON_CMD_TYPE_DEBIT_ONLY_PAY = 3,	//���ۿ�����

	JSON_DT_PARAM_ACCOUNT		 = 0xFF01,	//�û�����JSON��ʽ
	JSON_DT_PARAM_DEBIT_MONEY	 = 0xFF02,	//�ۿ����JSON��ʽ
	JSON_DT_PARAM_DEBIT_CANCLE	 = 0xFF03,	//��������JSON��ʽ
	JSON_DT_PARAM_DEBIT_RESULT	 = 0xFF04,	//�ۿ�������JSON��ʽ
	JSON_DT_PARAM_MMI			 = 0xFF05,	//�����������JSON��ʽ
};

//��Ӫƽ̨�������붨��
enum JSON_SERVER_RETURN_CODE
{
	HTTP_RC_DATA_LEGAL					= 200,	//�������ݷ��Ϲ���
	HTTP_RC_DATA_VALID					= 1,	//��������
	HTTP_RC_BLACKLIST					= 2,	//��ά���֧���û�����΢����������
	HTTP_RC_NO_REGISTER					= 3,	//��ά���֧���û�δע��
	HTTP_RC_NO_ENTRY_BY_ID				= 4,	//��ά���֧���û�����Ч�����Ϣ
	HTTP_RC_ID_IS_RUNNING				= 5,	//��ά���֧���û��Ѿ�·�������

	HTTP_RC_DEBIT_NEED_PASSWORD			= 6,	//��Ҫ��ά���֧���û���������
	HTTP_RC_DEBIT_CODE_TIMEOUT			= 7,	//��ά����Ч�ڳ�ʱ
	HTTP_RC_DEBIT_SRC_ILLGAL			= 8,	//��ά����Դ�Ƿ�����ָ����Χ�ڵĶ�ά�룩
	HTTP_RC_DEBIT_INSUFFICIENT_BALANCE	= 9,	//����,��������
	HTTP_RC_DEBIT_FAIL					= 10,	//�۷�ʧ��(����ԭ�򣬲������³��Կۿ�)
	HTTP_RC_DEBIT_SUCCESS				= 11,	//�۷ѳɹ�
	HTTP_RC_DEBIT_NO_VOUCHER			= 12,	//Э���ʽ������ϸ�����ŵ�message�У�
	HTTP_RC_PROTOCOL_ERROR				= 13,	//���������Ľ���ƾ֤�����Ķ�����Ϣ��ƽ̨û���ҵ�
	HTTP_RC_PAYPLATROMR_ERROR			= 14,	//֧��ƽ̨����
	HTTP_RC_NO_CANCEL_ORDER				= 15,	//�����ѹ���Ч�ڣ���ֹ����
	HTTP_RC_NO_SERVICES					= 16,	//��ͣ����
	HTTP_RC_VEHICLE_IS_RUNNING			= 17,	//�����Ѿ���·������
	HTTP_RC_VEHICLE_NO_ENTRY			= 18,	//�޷���������Ч���
	HTTP_RC_INPUT_PASSWORD_ERROR		= 20,	//�û������������
};

//��ʱ��ID
enum TIMER_ID
{
	TIMER_ID_DEBIT_MONEY = 1,
	TIMER_ID_DEBIT_CHECK = 2,
	TIMER_ID_DEBIT_CANCEL = 3,
};

//�����豸����״̬����
enum DEVICE_STATE_DESC
{
	DEVICE_STATE_LOAD			= 0x0001,	//��������ʧ��
	DEVICE_STATE_SERIAL_OPEN	= 0x0002,	//�豸�����޷���
	DEVICE_STATE_SERIAL_COMM	= 0x0004,	//�豸����ͨѶ����
	DEVICE_STATE_PAY_WEB		= 0x0008,	//֧����ƽ̨�������
	DEVICE_STATE_PAY_SERVER		= 0x0010,	//֧����ƽ̨����������
	DEVICE_STATE_4G				= 0x0020,	//�ƶ�ͨѶ�豸����
};

//�豸�����̣߳�����ҵ������̻߳�ȡ�Ľ���������
enum RESPONSE_MANAGE_CODE
{
	RDM_NONE			= 0x00,		//δ֪����

	RDM_DEVICE_LEGAL	= 0x01,		//�ж��ն��Ƿ�Ϸ�ʹ��
	RDM_DEBITE_START	= 0x02,		//����ۿ�
	RDM_DEBITE_CANCEL	= 0x03,		//���𳷵�
	RDM_ISSUE_DATE		= 0x04,		//�·��շ���ˮ�ؼ���Ϣ
	RDM_GET_LAST_ERR	= 0x05,		//Ҫ���ṩ�ϴδ�����Ϣ
	RDM_DEVICE_INIT		= 0x06,		//�豸��ʼ��

	RDM_HEART				= 101,		//������ѯ
	
	RDM_READ_HW_VER			= 102,		//��Ӳ���汾
	RDM_READ_CAN_ID			= 103,		//��CA NID
	RDM_READ_MAX_CODE		= 104,		//��MAX��
	RDM_READ_FM_VER			= 105,		//���̼��汾
	RDM_READ_SCAN_TIME_LIMIT= 106,		//��ɨ��ʱ����������
	RDM_READ_SERIAL_NO		= 107,		//���豸Ψһ���
	RDM_READ_WELCOME_SC		= 108,		//����������
	RDM_READ_NET0_IP_INFO	= 109,		//������0 IP��Ϣ
	RDM_READ_NET1_IP_INFO	= 110,		//������1 IP ��Ϣ
	RDM_READ_DEVICE_TMP		= 111,		//��Ӳ����ǰʱ��
	RDM_READ_DEVICE_TIME	= 112,		//��Ӳ����ǰʱ��
	
	RDM_SET_HEART_TIME		= 201,		//�����ն��������
	RDM_SET_WELCOME_SC		= 202,		//�����ն˿�������
	RDM_SET_MMI_INFO		= 203,		//���ƽ������ټ���
	RDM_SET_MMI_BYTE		= 204,		//���ƽ������ټ���
	RDM_SET_MMI_AD			= 205,		//���ƽ�������棩
	RDM_SET_DEVICE_TIME		= 206,		//�·�����ʱ��
	RDM_SET_NET0_IP_INFO	= 207,		//��������0 IP��Ϣ
	RDM_SET_NET1_IP_INFO	= 208,		//��������1 IP��Ϣ

	RDM_PARAM_WRITE_MAX_CODE		= 301,		//дMAX��
	RDM_PARAM_WRITE_CAN_ID			= 302,		//дCANID
	RDM_PARAM_WRITE_SERIAL_NO		= 303,		//д�豸Ψһ���к�
	RDM_PARAM_WRITE_SACN_TIME_LIMIT = 304,		//����ɨ������ʱ��
	RDM_PARAM_TEST_SC				= 305,		//��Ŀ������ѯ����
	RDM_PARAM_TT_DATA				= 306,		//͸�����ݵ�ָ������
};

//���������߹��ܷ������
enum RESPONSE_CALLER_FUN_CODE
{
	RCC_NONE			= 0x00,		//δ֪����
	RCC_FUN_DEBIT		= 0x01,		//�ۿ����
	RCC_FUN_CANCEL		= 0x02,		//��������
	RCC_FUN_NO_STOP_CAR	= 0x03,		//��ͣ��
};

//����������ִ�н���Ĵ������
enum RESPONSE_CALLER_EXECUTE_CODE
{
	RCC_OK					= 0x00,		//�ɹ�
	RCC_INPUT_PASSWORD		= 0x01,		//�û�������������
	RCC_NO_MONEY			= 0x02,		//�û�����
	RCC_QRCODE_ILLEGAL		= 0x03,		//��ά��Ƿ�
	RCC_QRCODE_OT			= 0x04,		//��ά����Ч�ڳ�ʱ
	RCC_NO_ACCOUNT_INFO		= 0x05,		//�����豸δ�ܻ�ȡ�û���Ϣ
	RCC_DEBIT_OT			= 0x06,		//�ۿʱ
	RCC_PSW_ERROR			= 0x07,		//�����������
	RCC_PARAM_ERROR			= 0x08,		//�����������
	RCC_LAST_TRADE_NOT_END	= 0x09,		//�ϴν���δ���
	RCC_COM_NOT_INIT		= 0x0A,		//���δ��ʼ��
	RCC_COM_ILLEGAL			= 0x0B,		//���δ����Ȩ
	RCC_PAY_PLATFORM_ERROR	= 0x0C,		//֧��ƽ̨����������
	RCC_DEVICE_INIT_ERROR	= 0x0D,		//����Ӳ����ʼ��ʧ��

	RCC_BREAK_OPERATION     =0xFE,      //ȡ������

	RCC_INNER_ERROR			= 0xFF,		//�ڲ�����
};

//�û���Ϣ�ṹ��
typedef struct _tagAccountInfo
{
	int		DataType;			//��������(���ֶ��壺1,�������ַ���;2,��δ���壻���ֶ��壺ɨ��������)
	int		SrcType;			//������Դ(1,ɨ����;2,��δ����)
	DWORD	DataSize;			//���ݳ���
	TCHAR	DataContext[5120];	//���ݳ���(�� NULL �������ַ���)
}tagAccountInfo;

//����������Ϣ�ṹ��
typedef struct _tagLaneConfigInfo
{
	int		MainType;			//���ʽ����ڡ�����ʽ
	int		SubType;			//ETC,MTC,�Զ�����,��Я�շѻ�
	int		AreaID;				//�������(�����)
	TCHAR   str_AreaID[200];
	int		RoadID;				//·�α���(�����)
	TCHAR   str_RoadID[200];
	int		StationID;			//վ�����
	TCHAR str_StationID[200];
	int		LaneID;				//��������
	TCHAR str_LaneID[200];
	int		LaneType;			//��������(1,���;2,����)
	TCHAR	LaneTab[16];		//����Ψһ����
	TCHAR	StationTab[32];		//վ��Ψһ����
	int		STServerTpye;		//վ������������
	TCHAR	STServerIP[24];		//վ��������IP��ַ
	TCHAR	STServerID[24];		//վ��������ID����

	int WebUpdatePort;				//���������¶˿� add 2017-08
	int UseAngencyDll;              //�Ƿ����ô��۶�̬�⣬ add 2018-01
	int ReturnPayChannelCodeUnify;   //�Ƿ�ͳһ���ش���
	int ForceUTC8;                  //ǿ��ʹ��UTC8��add 2018-01

	int		WebServerPort;			//������������Ŷ˿�
	TCHAR	WebServerIP[24];		//���������IP��ַ
	TCHAR   LocalPCWebServerIP[24];   //����IP

	int		ConfigLocalIPMode;		//TR300ƽ̨����IP����ģʽ(0,����IP;1,�Զ���ȡ)
	TCHAR	ConfigLocalIP0[24];		//TR300ƽ̨����0��IP
	TCHAR	ConfigLocalSub0[24];	//TR300ƽ̨����0����������
	TCHAR	ConfigLocalGate0[24];	//TR300ƽ̨����0������
	TCHAR	ConfigLocalIP1[24];		//TR300ƽ̨����1��IP
	TCHAR	ConfigLocalSub1[24];	//TR300ƽ̨����1����������
	TCHAR	ConfigLocalGate1[24];	//TR300ƽ̨����1������
	TCHAR	ConfigLocalDefGate[24];	//TR300ƽ̨Ĭ������
}tagLaneConfigInfo;

//���׹ؼ���Ϣ
typedef struct _tagPayKeyItems
{
	//�Ƿ���Ч
	BOOL	Valid;
	//��ʱ�ȴ�ʱ��
	int		OverTime;
	//�ۿ�ģʽ
	int		OperationMode;

	int     PayModel;

	/*****************Debit��ϸ��************************/
	//����ʶ����
	CString	PayIdentifier;
	//���׽��(��λ����)
	int		Money;
	//����ʱ��
	CTime	SubTime;
	//����������
	int		DebitMainType;
	//������֧��ƾ֤
	CString PayCertificateCode;
	//֧��ƾ֤����
	int		PayCertificateType;

	/*****************Vehicle��ϸ��************************/
	//����
	int		VehicleType;
	//����
	int		VehicleClass;
	//������ɫ
	int		VLColor;
	//����
	CString	VehicleLicense;
	//����
	int		AxisCount;
	//����
	int		TotalWeight;
	//����
	int		LimitWeight;

	/*****************Operation��ϸ��************************/
	//ͨ��ƾ֤����
	int		PassCertificateType;
	//ͨ��ƾ֤����
	CString CardID;
	//ͳ����
	int		TollDate;
	//ͳ�ư��
	int		ShiftID;
	//��ӡƱ�ݺ�
	CString TicketNo;

	//�շ�Ա����
	CString OperatorName;
	//�շ�Ա����
	CString OperatorID;
	

	/*****************Entry��ϸ��************************/
	//���վ��
	int		EntryStationID;
	CString str_EntryStationID;//TCHAR str_EntryStationID[100];
	//���վ��
	CString EntryStationName;
	//���ʱ��
	CTime	EntryTime;
	//��ڳ���
	int		EntryLane;
	CString str_EntryLane;//TCHAR str_EntryLane[100];
	//��ڳ���
	CString	EntryVehicleLicense;
	//��ڳ�����ɫ
	int		EntryVLColor;
	//��ڳ���
	int		EntryVehicleType;
	//��ڳ���
	int		EntryVehicleClass;
	//����������
	int		EntryAreaID;
	//���·�κ�
	int		EntryRoadID;

	CString str_EntryRoadID;
	CString str_EntryAreaID;

	//add 2018-05
	CString AutoPlate;
	int AutoPlateColor;

}tagPayKeyItems;

//�ۿ����ؼ���Ϣ��ϸ
typedef struct _tagDebitResultItems
{
	//�Ƿ���Ч
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

//����APP��������߳��ഫ�ݻ���������Ϣ�Ľṹ��
typedef struct _tagTranstParam
{
	//ʡ�ݱ���
	int					ProvinceID;
	//�������û�����Ϣ
	tagLaneConfigInfo	LaneConfigInfo;
	//��������������ϢID
	UINT				ActiveNotifyMsgID;
	//�������߳�ID
	UINT				CallerThreadID;
	//�����ߴ�����
	HWND				CallerWnd;
	//��̨ҵ������߳�
	HANDLE				ThreadManageHandle;
	/******************����·������******************/

	//������ʼĿ¼����·��,Ĭ��Ϊ���г���ĵ�ǰĿ¼
	TCHAR	PathCommDir[MAX_PATH];
	//�������̲�������ʱ�ؼ��ļ��洢Ŀ¼����·��
	TCHAR	PathDataDir[MAX_PATH];
	//������־Ŀ¼����·��
	TCHAR	PathLogDir[MAX_PATH];
}tagTranstParam;

//MMI���������ֲ����ṹ��Ϣ
typedef struct _tagMMIItemText
{
	int	Type;
	int Sub;
	int Row;
	BOOL  IsScroll;
	TCHAR Context[48];
}tagMMIItemText;

//MMI���������������ṹ��Ϣ
typedef struct _tagMMIItemVoice
{
	int	Type;
	int No;
}tagMMIItemVoice;

//MMI������ͼ������ṹ��Ϣ
typedef struct _tagMMIItemGraph
{
	int	Type;
	int No;
}tagMMIItemGraph;

#endif //_TRAWE_EMBEDDED_BASE_DEFINE_H_