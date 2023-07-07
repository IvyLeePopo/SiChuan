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
//ɨ���������ź�
enum SCAN_MACHINE_INDEX
{
	SM_INDEX_DWON_LEFT	= 0x01,	//����ɨ����(�ͳ�1��
	SM_INDEX_DWON_RIGHT	= 0x02,	//����ɨ����(�ͳ�2)
	SM_INDEX_UP_LEFT	= 0x04,	//����ɨ����(����1)
	SM_INDEX_UP_RIGHT	= 0x08,	//����ɨ����(����2)
	SM_INDEX_HAND		= 0x80,	//�ֳ�ɨ����
};

#define OP_SUCCESS	0							//ִ�гɹ� -- ��������ֵ�ǲ����͵Ļ�������ҪУ�鷵��ֵ


#define MSG_START_ID_SM 100

//ɨ�������������Ϣ
#define WM_THREAD_SM_START_SCAN							WM_APP + MSG_START_ID_SM + 1	//ǿ�ƿ�ʼɨ��
#define WM_THREAD_SM_STOP_SCAN							WM_APP + MSG_START_ID_SM + 2	//��ʼ�����еĹ����߳�
#define WM_THREAD_SM_RESET_MACHINE						WM_APP + MSG_START_ID_SM + 3	//��ʼ��ָ��ɨ����
#define WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO	WM_APP + MSG_START_ID_SM + 4	//�����߳�ɨ��״̬��ʾ��Ϣ
#define WM_THREAD_SM_CONTROL_BOARD						WM_APP + MSG_START_ID_SM + 5	//���ƴ��ڰ�

#define WM_THREAD_SM_GET_SCAN_CODE_DATA					WM_APP + MSG_START_ID_SM + 6	//�����̻߳�ȡ��ɨ����
#define WM_THREAD_SM_STATE								WM_APP + MSG_START_ID_SM + 7	//�����߳�ɨ��״̬��ʾ��Ϣ


#define WM_THREAD_SM_GET_SYSTEM_INFO		WM_APP + MSG_START_ID_SM + 2	//�����̻߳�ȡɨ����ϵͳ��Ϣ
#define WM_THREAD_SM_SCANING_STATE			WM_APP + MSG_START_ID_SM + 3	//�����߳�ɨ��״̬��ʾ��Ϣ
#define WM_THREAD_SM_SCAN_CODE				WM_APP + MSG_START_ID_SM + 8	//ǿ�ƿ�ʼɨ��



//����APP��������߳��ഫ�ݻ���������Ϣ�Ľṹ��
typedef struct _tagTranstParam
{
	//ʡ�ݱ���
	int					ProvinceID;
	//�������û�����Ϣ
//	tagLaneConfigInfo	LaneConfigInfo;
	//��������������ϢID
	DWORD				ActiveNotifyMsgID;
	/////////////////////����·������///////////////////////////

	//������ʼĿ¼����·��,Ĭ��Ϊ���г���ĵ�ǰĿ¼
	TCHAR	PathCommDir[MAX_PATH];
	//�����洢����Ŀ¼����·��
	TCHAR	PathDataDir[MAX_PATH];
	//����Ӳ������Ŀ¼����·��
	TCHAR	PathDeviceDir[MAX_PATH];
	//������־Ŀ¼����·��
	TCHAR	PathLogDir[MAX_PATH];
	//�������ݱ���Ŀ¼����·��
	TCHAR	PathDataBackupDir[MAX_PATH];
	//�������̲�������ʱ�ؼ��ļ��洢Ŀ¼����·��
	TCHAR	PathProduceDir[MAX_PATH];
	//��ʱ�ļ��洢Ŀ¼����·��
	TCHAR	PathTempDir[MAX_PATH];
}tagTranstParam;

//ɨ��������
enum SCAN_MACHINE_TYPE
{
	SM_TYPE_HONEYWELL_3310G = 1,	//����Τ��3310g
	SM_TYPE_NEW_LAND_FM_420 = 2,	//�´�½FM420
};

//���ڲ�����Ϣ�ṹ��
typedef struct _tagCOMParam
{
	int		Port;					//����
	int		BaudRate;				//������
	int		ParityBit;				//У��λ
	int		StopBit;				//ֹͣλ
	int		DataBit;				//����λ

	DWORD	InBufferSize;			//���뻺������С
	DWORD	OutBufferSize;			//�����������С
}tagCOMParam;

//����ɨ����������Ʋ������������ṹ��
struct tagSMMainBoardParam
{
	BYTE	Type;			//��������(1,����;2,����)
	BYTE	CMDFlag;		//������
	BYTE	ParamData[512];	//��������
	WORD	ParamSize;		//��������
};


//����ɨ���豸�ڲ��ҽ�ɨ�����ĳ�ʼ�����ݽṹ����
struct tagSMInnerDeviceInit
{
	BYTE					DeviceID;		//�豸����ID
	tagCOMParam				COM;			//���ڲ����ṹ
};

//����ɨ���豸�ڲ��ҽ�ɨ������״̬���ݽṹ����
struct tagSMInnerDeviceState
{
	BYTE		DeviceID;				//�豸����ID
	BYTE		StateType;				//״̬����(0,ɨ����;1,ͨѶ����)
	BOOL		IsInit;					//�Ƿ��ʼ��
	BOOL		IsReadVer;				//�Ƿ��ȡ�汾��Ϣ
	BOOL		IsSetFactory;			//�Ƿ��Ѿ��ָ�����
	BOOL		IsSetPersion;			//�Ƿ��Ѿ����ø��Ի�
	int			StateCode;				//״̬��ʾ��
	TCHAR		StateDesc[512];			//״̬������ʾ��Ϣ
	
	//ɨ����ر���
	int			ScanResult;				//ɨ��״̬(0,�ɹ���������ʧ��)
	int			ScanTask;				//ɨ����岽����(1,׼��ɨ��,����ʱ;2,��ȡ�����������;3,�����ѷ��ͣ�����ɨ��;4,ɨ��Ĭ�ϵȴ���ʱ�������ط�;5,ɨ�����)
	BYTE		RecScanCmdCount;		//�豸ͬһ��ɨ��������̵��У�����ɨ������Ĵ���
	
	DWORD		ScanConsumeTime;		//ɨ�뻨�ѵ�ʱ�䣬��λ������
	SYSTEMTIME	BeginTime;				//ɨ����ʼʱ��
	SYSTEMTIME	EndTime;				//ɨ�����ʱ��
	TCHAR		OriginalResult[512];	//ɨ��ԭʼ���
	TCHAR		Code[512];				//ɨ���ʽ���������
	BYTE		CodeType;				//�����ж�ɨ������Դ
	
	//������ر���
	BYTE		HeartFaileCount;		//�豸����ʧ�ܴ���
	BOOL		LastDeviceState;		//�豸�ϴ�����״̬
	
	//��ʼ����ر���
	time_t		LastInitTime;			//�豸�ϴεĳ�ʼ��ʱ��
	time_t		LastRecordCOMStateTime;	//�ϴγ��Է��ʹ���״̬��ʱ��
	time_t		LastReadVerTime;		//�ϴγ��Զ�ȡ�汾��ʱ��

	//�汾��ر���
	TCHAR		ProductName[128];		//�豸�ڲ��汾ȫ��(��ʽ���� �豸����ID,�汾;�豸����ID,�汾;)
	TCHAR		BootRevision[128];		//�����汾��Ϣ(��ʽ���� �豸����ID,�汾;�豸����ID,�汾;)
	TCHAR		SerialNumber[128];		//�豸���к�(��ʽ���� �豸����ID,���к�;�豸����ID,���к�;)
	TCHAR		FirmwareVersion[128];	//�豸�̼��汾(��ʽ���� �豸����ID,�汾;�豸����ID,�汾;)
};

//����ɨ���豸�ڲ��ҽ�ɨ������������ݽṹ���弯��
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
	BYTE	DeviceID;					//�豸����ID
	DWORD	SMType;						//ɨ��������
	
	DWORD	EffectiveDataLen;			//��Ч���ݳ���
	DWORD	SingleReadDataSpanTime;		//���δ��ڶ�ȡ���ʱ�䣬��λ������
	DWORD	SMRoundTimeOutValue;		//ÿ��ɨ�����ʱ��,��λ������
	DWORD	SMDefaultScanTimeOutValue;	//ɨ����Ĭ�϶�ȡ�ȴ���ʱ����λ������

	DWORD	HasProcessDeviceCommResultCount;//ִ����ͨ������Ѵ��������Ϣ���豸��	
	DWORD	HasProcessDeviceInitResultCount;//ִ�г�ʼ��������Ѵ��������Ϣ���豸��	
	DWORD	HasProcessDeviceVerResultCount; //ִ�ж��汾������Ѵ��������Ϣ���豸��	
	//�豸�ĳ�ʼ����Ϣ
	CArray<tagSMInnerDeviceInit,tagSMInnerDeviceInit&>	SMInitList;
	//�豸��״̬��Ϣ
	CArray<tagSMInnerDeviceState,tagSMInnerDeviceState&>SMStateList;
};

*/




#define COLOR_PAY_WAIT				RGB(145,40,140)		//�ۿ�ȴ�
#define COLOR_PAY_FAIL				RGB(255,0,0)		//�ۿ�ʧ��
#define COLOR_PAY_SUCCESS			RGB(0,128,64)		//�ۿ�ɹ�

/*******************************************************��ͨ�궨��********************************************************************/
#define OP_SUCCESS	0							//ִ�гɹ� -- ��������ֵ�ǲ����͵Ļ�������ҪУ�鷵��ֵ

//AES��Կ
#define AES_ENC_KEY_DATA	_T("1111222233334444")

/*******************************************************��Ϣ����********************************************************************/

//��������߳���Ϣ����ʼ����
#define	MSG_START_ID_SQL		0		//SQL�̴߳�����Ϣ��ʼ������
#define	MSG_START_ID_SM			100		//ɨ���̴߳�����Ϣ��ʼ������
#define	MSG_START_ID_IC			200		//��д���̴߳�����Ϣ��ʼ������
#define	MSG_START_ID_EQUIPEMENT	300		//IO�����̴߳�����Ϣ��ʼ������
#define MSG_START_ID_HTTP		400		//HttpͨѶ�̴߳�����Ϣ��ʼ������
#define MSG_START_ID_PLATE		500		//����ʶ�����߳���Ϣ��ʼ������
#define MSG_START_ID_VDM		600		//VDM�����߳���Ϣ��ʼ������
#define MSG_START_ID_TFI		700		//���Դ����߳���Ϣ��ʼ������
#define MSG_START_ID_REMOTE		800		//Զ�̵�ͼ����Ϣ��ʼ������
#define MSG_START_ID_AXIS		900		//�����߳���Ϣ��ʼ������
#define MSG_START_ID_SOCKET		1000	//SocketͨѶ�����Ϣ��ʼ����
#define MSG_START_ID_DM			1100	//�������ݹ����߳������Ϣ��ʼ����
#define MSG_START_ID_MAIN_UI	1200	//�����������Ϣ��ʼ������


//Http�߳��ദ�����Ϣ
#define WM_THREAD_HTTP_SEND_DATA_REQUEST			WM_APP + MSG_START_ID_HTTP + 1	//���Ͳ�ѯ������Ϣ����
#define WM_WND_DEAL_PLATFORM_RESPONSE_RESULT		WM_APP + MSG_START_ID_HTTP + 2	//��������Httpƽ̨������������
#define WM_THREAD_HTTP_RESEND_DATA_REQUEST			WM_APP + MSG_START_ID_HTTP + 3	//�ط�����


//��Ӫƽ̨ͨѶ�ദ�����Ϣ
#define	WM_WND_DEAL_RESPONSE_FOR_ENTRY_QUERY		WM_APP + MSG_START_ID_HTTP + 3	//��HTTP�߳̽��յ�����ڲ�ѯ���Ļ������
#define	WM_WND_DEAL_RESPONSE_FOR_EXIT_QUERY			WM_APP + MSG_START_ID_HTTP + 4	//��HTTP�߳̽��յ������ڲ�ѯ���Ļ������
#define WM_WND_DEAL_RESPONSE_FOR_OPEN_LOOP			WM_APP + MSG_START_ID_HTTP + 5	//��HTTP�߳̽��յ�����ڿ����������Ļ������
#define WM_WND_DEAL_RESPONSE_FOR_CLOSE_LOOP			WM_APP + MSG_START_ID_HTTP + 6	//��HTTP�߳̽��յ������ڱջ��������Ļ������
#define WM_WND_DEAL_RESPONSE_FOR_DEBIT				WM_APP + MSG_START_ID_HTTP + 7	//��HTTP�߳̽��յ����۷Ѳ������Ļ������
#define WM_WND_DEAL_RESPONSE_FOR_DEBIT_CHECK		WM_APP + MSG_START_ID_HTTP + 8	//��HTTP�߳̽��յ����۷�״̬��ѯ���Ļ������
#define WM_WND_DEAL_RESPONSE_FOR_PASS_OPEN_STATION	WM_APP + MSG_START_ID_HTTP + 9	//��HTTP�߳̽��յ���������ʽ�շ�վ���Ļ������


//ɨ�������������Ϣ
#define WM_THREAD_SM_INIT_WORK				WM_APP + MSG_START_ID_SM + 25	//��ʼ�����еĹ����߳�

#define WM_THREAD_SM_GET_SCAN_CODE_DATA					WM_APP + MSG_START_ID_SM + 1	//��ȡ��ɨ����
#define WM_THREAD_SM_GET_SYSTEM_INFO					WM_APP + MSG_START_ID_SM + 2	//��ȡɨ����ϵͳ��Ϣ
#define WM_THREAD_SM_START_SCAN							WM_APP + MSG_START_ID_SM + 3	//ǿ�ƿ�ʼɨ��
#define WM_THREAD_SM_STOP_SCAN							WM_APP + MSG_START_ID_SM + 4	//��ʼ�����еĹ����߳�
#define WM_THREAD_SM_RESET_MACHINE						WM_APP + MSG_START_ID_SM + 5	//��ʼ��ָ��ɨ����
#define WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO	WM_APP + MSG_START_ID_SM + 6	//Э������෢�͸������̹߳���ɨ��״̬��ʾ��Ϣ
#define WM_THREAD_SM_CONTROL_BOARD						WM_APP + MSG_START_ID_SM + 7	//���ƴ��ڰ�
#define WM_THREAD_SM_STATE								WM_APP + MSG_START_ID_SM + 8	//�����̷߳��͸�UI����ɨ������״̬��ʾ��Ϣ
#define WM_THREAD_SM_SET_LIGHT_MODE						WM_APP + MSG_START_ID_SM + 9	//���Ƶƹ����

#define WM_WND_NET_SM_DEBIT_TOLL						WM_APP + MSG_START_ID_SM + 10	//�����ۿ�
#define WM_WND_CHECK_DEBIT_PASSWORD_STATE				WM_APP + MSG_START_ID_SM + 11	//�ۿ�����ѯ

//������������ʾ��Ϣ
#define WM_THREAD_SM_SET_DISPLAY						WM_APP + MSG_START_ID_SM + 12	//������ʾ����



#define FILE_SIZE_LOG_MAX	5*1024*1024		//��־�ļ����ֵ
#define DIR_NAME_DEVICE_SCAN		_T("ScanMachine")	//ɨ����

#define FILE_NAME_DLL_SCAN			_T("HTSM.dll")			//ɨ�������ƶ�̬��
#define FILE_NAME_HTTP_CONFIG			_T("ReflectWeb.ini")			//���������������Ϣ�ļ���

#define FILE_NAME_DEVICE_SM_CONFIG			_T("HTSMConfig.ini")		//Ӳ��������Ϣ�ļ���--ɨ����
#define FILE_NAME_DEVICE_SM_CONFIG1			_T("HTSMConfig1.ini")		//Ӳ��������Ϣ�ļ���--ɨ����




//Ӳ������
enum HARDWARE_TYPE
{
	HT_ICREADER		= 1,//��д��
	HT_SCANMACHINE	= 2,//ɨ����
	HT_PRINTER		= 3,//��ӡ��
	HT_VDM			= 4,//�ַ�������
	HT_TFI			= 5,//�Ѷ���ʾ��
	HT_PLATE		= 6,//����ʶ����
	HT_AXIS			= 7,//������
};


//Ӳ������ӿ�����
enum HT_INTERFACE_TYPE
{
	HTIF_COM	= 1,//����
	HTIF_NET	= 2,//����
	HTIF_LPT	= 3,//����
	HTIF_USB	= 4,//USB
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
};


//ɨ��������
enum SCAN_MACHINE_TYPE
{
	SM_TYPE_HONEYWELL_3310G = 1,	//����Τ��3310g
	SM_TYPE_NEW_LAND_FM_420 = 3,	//�´�½
};


//ɨ���������ź�
enum SCAN_MACHINE_INDEX
{
	SM_INDEX_DWON_LEFT	= 0x01,	//����ɨ����(�ͳ�1��
	SM_INDEX_DWON_RIGHT	= 0x02,	//����ɨ����(�ͳ�2)
	SM_INDEX_UP_LEFT	= 0x04,	//����ɨ����(����1)
	SM_INDEX_UP_RIGHT	= 0x08,	//����ɨ����(����2)
	SM_INDEX_HAND		= 0x80,	//�ֳ�ɨ����
};


//ɨ������������ //1,��ʼ��;2,���汾;3,ɨ�봥��;4,ֹͣɨ��;5,����;6,���ó���;7,�����û�Ĭ��
enum SM_CMD_TYPE
{
	SM_CMD_UNKNOW			= 0,//δ֪
	SM_CMD_INIT				= 1,//��ʼ��
	SM_CMD_VER				= 2,//���汾
	SM_CMD_START			= 3,//ɨ�봥��
	SM_CMD_STOP				= 4,//ֹͣɨ��
	SM_CMD_HEART			= 5,//����
	SM_CMD_RESET_FACTORY	= 6,//���ó���
	SM_CMD_RESET_PERSION 	= 7,//�����û�Ĭ��

	SM_FLOW_STATE_COM			= 1,//ɨ������������״̬������״̬����
	SM_FLOW_STATE_INIT			= 2,//ɨ������������״̬����ʼ��״̬����
	SM_FLOW_STATE_VER			= 3,//ɨ������������״̬���汾��ѯ״̬����
	SM_FLOW_STATE_SCAN			= 4,//ɨ������������״̬��ɨ�����״̬����
	SM_FLOW_STATE_STOP			= 5,//ɨ������������״̬��ǿ��ֹͣɨ��״̬����
	SM_FLOW_STATE_RESET_FACTORY	= 6,//ɨ������������״̬���������Ĭ�ϲ�������״̬����
	SM_FLOW_STATE_RESET_PERSION = 7,//ɨ������������״̬���������Ĭ�ϲ�������״̬����
	SM_FLOA_STATE_HEART			= 8,//ɨ������������״̬������״̬����
};


//������֧������ƽ̨����
enum NET_PAY_PLATFORM_TYPE
{
	NPPT_MICRO_MSG	= 1,	//΢��
	NPPT_ALI		= 2,	//֧����
	NPPT_BAI_DU		= 3,	//�ٶ�
};




#pragma pack(1)

//����������Ϣ�ṹ��
typedef struct _tagLaneConfigInfo
{
	int		MainType;		//���ʽ����ڡ�����ʽ
	int		SubType;		//ETC,MTC,�Զ�����,��Я�շѻ�
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


//����APP��������߳��ഫ�ݻ���������Ϣ�Ľṹ��
typedef struct _tagTranstParam
{
	//ʡ�ݱ���
	int					ProvinceID;
	//�������û�����Ϣ
	tagLaneConfigInfo	LaneConfigInfo;
	//��������������ϢID
	DWORD				ActiveNotifyMsgID;
	/******************����·������******************/

	//������ʼĿ¼����·��,Ĭ��Ϊ���г���ĵ�ǰĿ¼
	TCHAR	PathCommDir[MAX_PATH];
	//�������̲�������ʱ�ؼ��ļ��洢Ŀ¼����·��
	TCHAR	PathDataDir[MAX_PATH];
	//����Ӳ������Ŀ¼����·��
	TCHAR	PathDeviceDir[MAX_PATH];
	//������־Ŀ¼����·��
	TCHAR	PathLogDir[MAX_PATH];
	//�������ݱ���Ŀ¼����·��
	TCHAR	PathDataBackupDir[MAX_PATH];
	//��ʱ�ļ��洢Ŀ¼����·��
	TCHAR	PathTempDir[MAX_PATH];
	//�������ز���Ŀ¼����·��
	TCHAR	PathDownloadDir[MAX_PATH];

	//�������ݶ���Ŀ¼�������ţ�
	TCHAR	PathDataSecondDirForLaneID[MAX_PATH];
	//�������ݶ���Ŀ¼�����ݣ�
	TCHAR	PathDataSecondDirForCore[MAX_PATH];
	//�������ݶ���Ŀ¼��������
	TCHAR	PathDataSecondDirForParam[MAX_PATH];
	//�������ݶ���Ŀ¼��ͼ��
	TCHAR   PathDataSecondDirForImage[MAX_PATH];
}tagTranstParam;


//���ڲ�����Ϣ�ṹ��
typedef struct _tagCOMParam
{
	int		Port;					//����
	int		BaudRate;				//������
	int		ParityBit;				//У��λ
	int		StopBit;				//ֹͣλ
	int		DataBit;				//����λ

	DWORD	InBufferSize;			//���뻺������С
	DWORD	OutBufferSize;			//�����������С
}tagCOMParam;


//ɨ�빤���̳߳�ʼ�������ṹ��
typedef struct _tagSMWorkInit
{
	DWORD		Index;						//ɨ���߳�������
	DWORD		SMType;						//ɨ��������
	DWORD		ParentsThread;				//���߳�
	DWORD		EffectiveDataLen;			//��Ч���ݳ���
	DWORD		SingleReadDataSpanTime;		//���δ��ڶ�ȡ���ʱ�䣬��λ������
	DWORD		ReadTimeOutValue;			//Ԥ�贮�ڵȴ���ʱ,��λ������
	DWORD		SMDefaultReadTimeOutValue;	//ɨ����Ĭ�϶�ȡ�ȴ���ʱ����λ������
	DWORD		SMOtherCMDTimeoutValue;		//ɨ������ȥɨ�붯��֮����������ʱʱ�䣬��λ������
	DWORD		SMRecanCount;				//��ɨ�������ÿһ��ɨ��ǰ����Ϊ0��
	tagCOMParam COM;						//���ڲ����ṹ
}tagSMWorkInit;


//ɨ���������̻߳��������Ϣ�ṹ��
typedef struct _tagSMWorkResult
{
	int			StateCode;				//ɨ��״̬(0,�ɹ���������ʧ��)
	DWORD		ThreadID;				//�������߳�ID
	DWORD		Index;					//ɨ��������
	DWORD		ScanConsumeTime;		//ɨ�뻨�ѵ�ʱ�䣬��λ������
	SYSTEMTIME	BeginTime;				//ɨ����ʼʱ��
	SYSTEMTIME	EndTime;				//ɨ�����ʱ��
	TCHAR		OriginalResult[128];	//ɨ��ԭʼ���
	TCHAR		Code[32];				//ɨ���ʽ���������
	BYTE		CodeType;				//�����ж�ɨ������Դ

	BYTE		ScanTask;				//ɨ����岽����(1,׼��ɨ��,����ʱ;2,��ȡ�����������;3,�����ѷ��ͣ�����ɨ��;4,ɨ��Ĭ�ϵȴ���ʱ�������ط�;5,ɨ�����)
	TCHAR		ScaningStateDesc[128];	//ɨ����̵��еľ�����ʾ��Ϣ
}tagSMWorkResult;


//����ɨ����������Ʋ������������ṹ��
struct tagSMMainBoardParam
{
	BYTE	Type;			//��������(1,����;2,����)
	BYTE	CMDFlag;		//������
	BYTE	ParamData[512];	//��������
	WORD	ParamSize;		//��������
};


//����ɨ���豸�ڲ��ҽ�ɨ�����ĳ�ʼ�����ݽṹ����
struct tagSMInnerDeviceInit
{
	BYTE					DeviceID;		//�豸����ID
	tagCOMParam				COM;			//���ڲ����ṹ
};


//����ɨ���豸�ڲ��ҽ�ɨ������״̬���ݽṹ����
struct tagSMInnerDeviceState
{
	BYTE		DeviceID;				//�豸����ID
	BYTE		StateType;				//״̬����(0,ɨ����;1,ͨѶ����)
	BOOL		IsInit;					//�Ƿ��ʼ��
	BOOL		IsReadVer;				//�Ƿ��ȡ�汾��Ϣ
	BOOL		IsSetFactory;			//�Ƿ��Ѿ��ָ�����
	BOOL		IsSetPersion;			//�Ƿ��Ѿ����ø��Ի�
	int			StateCode;				//״̬��ʾ��
	TCHAR		StateDesc[512];			//״̬������ʾ��Ϣ
	
	//ɨ����ر���
	int			ScanResult;				//ɨ��״̬(0,�ɹ���������ʧ��)
	int			ScanTask;				//ɨ����岽����(1,׼��ɨ��,����ʱ;2,��ȡ�����������;3,�����ѷ��ͣ�����ɨ��;4,ɨ��Ĭ�ϵȴ���ʱ�������ط�;5,ɨ�����)
	BYTE		RecScanCmdCount;		//�豸ͬһ��ɨ��������̵��У�����ɨ������Ĵ���
	
	DWORD		ScanConsumeTime;		//ɨ�뻨�ѵ�ʱ�䣬��λ������
	SYSTEMTIME	BeginTime;				//ɨ����ʼʱ��
	SYSTEMTIME	EndTime;				//ɨ�����ʱ��
	TCHAR		OriginalResult[512];	//ɨ��ԭʼ���
	TCHAR		Code[512];				//ɨ���ʽ���������
	BYTE		CodeType;				//�����ж�ɨ������Դ
	
	//������ر���
	BYTE		HeartFaileCount;		//�豸����ʧ�ܴ���
	BOOL		LastDeviceState;		//�豸�ϴ�����״̬
	
	//��ʼ����ر���
	time_t		LastInitTime;			//�豸�ϴεĳ�ʼ��ʱ��
	time_t		LastRecordCOMStateTime;	//�ϴγ��Է��ʹ���״̬��ʱ��
	time_t		LastReadVerTime;		//�ϴγ��Զ�ȡ�汾��ʱ��

	//�汾��ر���
	TCHAR		ProductName[128];		//�豸�ڲ��汾ȫ��(��ʽ���� �豸����ID,�汾;�豸����ID,�汾;)
	TCHAR		BootRevision[128];		//�����汾��Ϣ(��ʽ���� �豸����ID,�汾;�豸����ID,�汾;)
	TCHAR		SerialNumber[128];		//�豸���к�(��ʽ���� �豸����ID,���к�;�豸����ID,���к�;)
	TCHAR		FirmwareVersion[128];	//�豸�̼��汾(��ʽ���� �豸����ID,�汾;�豸����ID,�汾;)
};


struct tagScanDisplay
{
	int		iSerial;		//���кţ��������ֲ��õ���ʾ�����1����
	int		Type;			//��ʾ����		1-���������������ͣ�	2-����������ǰʱ�䣻3-����������������,������ɨ�룻
	DWORD	Money;			//����λ��
	DWORD	Weight;			//��������λǧ��
	int		VehicleType;	//����	1��2��
	int		VehicleClass;	//���ͣ�1~5
	time_t	tShowtime;		//��ʾʱ��
	int		LimitWeight;	//����
	int		Balance;		//���
	int		AxisCount;		//����
	TCHAR	EntrySTName[32];//���վ��
};



#pragma pack()



//����ɨ���豸�ڲ��ҽ�ɨ������������ݽṹ���弯��
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
	BYTE	DeviceID;					//�豸����ID
	DWORD	SMType;						//ɨ��������
	
	DWORD	EffectiveDataLen;			//��Ч���ݳ���
	DWORD	SingleReadDataSpanTime;		//���δ��ڶ�ȡ���ʱ�䣬��λ������
	DWORD	SMRoundTimeOutValue;		//ÿ��ɨ�����ʱ��,��λ������
	DWORD	SMDefaultScanTimeOutValue;	//ɨ����Ĭ�϶�ȡ�ȴ���ʱ����λ������

	DWORD	HasProcessDeviceCommResultCount;//ִ����ͨ������Ѵ��������Ϣ���豸��	
	DWORD	HasProcessDeviceInitResultCount;//ִ�г�ʼ��������Ѵ��������Ϣ���豸��	
	DWORD	HasProcessDeviceVerResultCount; //ִ�ж��汾������Ѵ��������Ϣ���豸��	
	//�豸�ĳ�ʼ����Ϣ
	CArray<tagSMInnerDeviceInit,tagSMInnerDeviceInit&>	SMInitList;
	//�豸��״̬��Ϣ
	CArray<tagSMInnerDeviceState,tagSMInnerDeviceState&>SMStateList;
};



