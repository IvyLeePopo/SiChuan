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

//AES��Կ
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

//ɨ��ƿ���״̬
enum SM_PROTOCOL_CONST
{
	SM_PRPTOCOL_LIGHT_SUCCESS		= 0x00,	//��ʾɨ��ɹ�
	SM_PRPTOCOL_LIGHT_SCAN			= 0x01, //��ʾ����ɨ��
	SM_PRPTOCOL_LIGHT_FAIL			= 0x02,	//��ʾɨ��ʧ��
	SM_PRPTOCOL_LIGHT_ALL_OFF		= 0x03, //���
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

//���������ߴ������
enum RESPONSE_CALLER_CODE
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
	int		MainType;		//���ʽ����ڡ�����ʽ
	int		SubType;		//ETC,MTC,�Զ�����,��Я�շѻ�
	int		AreaID;			//�������(�����)
	int		RoadID;			//·�α���(�����)
	int		StationID;		//վ�����
	int		LaneID;			//��������
	int		LaneType;		//��������(1,���;2,����)
	TCHAR	LaneTab[16];	//����Ψһ����
	TCHAR	StationTab[32]; //վ��Ψһ����
	int		STServerTpye;	//վ������������
	TCHAR	STServerIP[24];	//վ��������IP��ַ
	TCHAR	STServerID[24];	//վ��������ID����
	TCHAR	WebServerIP[24];//�������������IP��ַ
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

	/*****************Entry��ϸ��************************/
	//���վ��
	int		EntryStationID;
	//���վ��
	CString EntryStationName;
	//���ʱ��
	CTime	EntryTime;
	//��ڳ���
	int		EntryLane;
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