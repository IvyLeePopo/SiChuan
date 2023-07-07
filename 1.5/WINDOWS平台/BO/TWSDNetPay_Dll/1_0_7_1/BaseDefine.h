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

//AES��Կ
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

//JSON���ݸ�ʽ����������
enum JSON_DATA_TYPE
{
	JSON_DT_SEND_DIBIT			= 3,	//�۷�����
	JSON_DT_SEND_DIBIT_CHECK	= 4,	//�۷Ѳ�ѯ
	JSON_DT_SEND_DIBIT_CANCEL	= 5,	//��������
	JSON_DT_SEND_PASS_STATION	= 8,	//�������շ�վ����
	JSON_DT_SEND_HEART			= 9,	//������ѯ
	JSON_DT_SEND_DIBIT_SECOND	= 10,	//���οۿ�
	JSON_DT_SEND_NFC_SIGN_IN	= 11,	//NFCģ��ǩ��

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
	HTTP_RC_DEBIT_QUERY_ERROR			= 21,	//�۷Ѻ�����ѯʧ�ܣ��۷�ִ���У����޽����
	HTTP_RC_NFC_NO_SUPPORT				= 22,	//NFC,��Ƭ��֧������
	HTTP_RC_NFC_NO_INIT					= 23,	//NFC,��Ƭδ��ʼ��
	HTTP_RC_NFC_NO_SERVICES				= 24,	//NFC,����������ͣ����
	HTTP_RC_NFC_NEED_SIGN				= 25,	//NFC,��Ҫ���½���ǩ��
};

//ɨ��ƿ���״̬
enum SM_PROTOCOL_CONST
{
	SM_PRPTOCOL_LIGHT_SUCCESS		= 0x00,	//��ʾɨ��ɹ�
	SM_PRPTOCOL_LIGHT_SCAN			= 0x01, //��ʾ����ɨ��
	SM_PRPTOCOL_LIGHT_FAIL			= 0x02,	//��ʾɨ��ʧ��
	SM_PRPTOCOL_LIGHT_ALL_OFF		= 0x03, //���

	SM_PRPTOCOL_LIGHT_FALI_PSW_INPUT= 0x04,	//����������
	SM_PRPTOCOL_LIGHT_FAIL_PSW_ERR	= 0x05,	//�������
	SM_PRPTOCOL_LIGHT_FAIL_QR_ERR1	= 0x06,	//��ά��Ƿ�
	SM_PRPTOCOL_LIGHT_FAIL_QR_ERR2	= 0x07,	//��ά�볬ʱ
	SM_PRPTOCOL_LIGHT_FALI_BL_ERR	= 0x08,	//����
	SM_PRPTOCOL_LIGHT_FALI_NET_ERR	= 0x09,	//���糬ʱ	
	SM_PRPTOCOL_LIGHT_FALI_PL_STOP	= 0x0A,	//��ͣ����	
	
	SM_PRPTOCOL_LIGHT_FALI_NFC_01	= 0x0B,	//NFC,��Ƭ��֧������
	SM_PRPTOCOL_LIGHT_FALI_NFC_02	= 0x0C,	//NFC,��Ƭδ��ʼ��	
	SM_PRPTOCOL_LIGHT_FALI_NFC_03	= 0x0D,	//NFC,����������ͣ����	
};

//��ʱ��ID
enum TIMER_ID
{
	TIMER_ID_DEBIT_MONEY			= 1,
	TIMER_ID_SCAN_CODE				= 2,
	TIMER_ID_DEBIT_CHECK			= 3,
	TIMER_ID_NFC_WAIT_NORMAL_DATA	= 4,
	TIMER_ID_NFC_WAIT_CORRECT_DATA	= 5,
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
	RCC_DEBIT_QUERY_ERROR	= 0x0D,		//�۷Ѳ�ѯʧ��

	RCC_INNER_ERROR			= 0xFF,		//�ڲ�����
};

//֧����������
enum PAY_CHANNEL_TYPE_CODE
{
	PCTC_WENXIN		= 0x01,		//΢�ŵ��渶
	PCTC_ALIIPAY	= 0x02,		//֧�������渶
	PCTC_BAIDU		= 0x03,		//�ٶ�֧��
	PCTC_JD			= 0x04,		//����֧��
	PCTC_UNIPAY_QR	= 0x05,		//������ά��֧��
	PCTC_UNIPAY_NFC	= 0x06,		//����NFC֧��
	
	PCTC_ALIIPAY_AGENCY	= 51,		//֧��������
	PCTC_WEIXIN_AGENCY	= 52,		//΢�Ŵ���
	PCTC_BAIDU_AGENCY	= 53,		//�ٶȴ���
	PCTC_JD_AGENCY		= 54,		//��������
	PCTC_UNIPAY_AGENCY	= 55,		//��������
};

//NFCģ�鹤��״̬
enum UNIPAY_MODULE_STATE
{
	UMS_UNKNOW		= 0x00,
	UMS_SIGNING		= 0x01,				//ǩ��
	UMS_DEBITING	= 0x02,				//��ʼ����
	UMS_CORRECT		= 0x03,				//����
	UMS_STOP		= 0x04,				//ֹͣ����
	UMS_TEST_LINK	= 0x05,				//��·����
};

//NFCЭ��֡����
enum UNIPAY_FRAME_TYPE
{
	UFT_UNKNOW			= 0x00, //δָ֪��
	//����ָ��
	UFT_NAK				= 0x15,	//�ܾ���Ӧ
	UFT_ACK				= 0x06, //������Ӧ
//	UFT_DATA			= 0x60, //����

	//��λ��ָ��
	UFT_M_SIGN			= 0x01,	//����ǩ������
	UFT_M_CONSUME_W1	= 0x02,	//�������ѣ���ѯ�������ͣ�
	UFT_M_CONSUME_W2	= 0x03,	//Ҫ��ִ�����ѣ�����W2���ͣ�
	UFT_M_CONSUME_W3	= 0x04,	//Ҫ��ִ�����ѣ�����W3���ͣ�
	UFT_M_CORRECT		= 0x05,	//�������
	UFT_M_STOP			= 0x06, //ִֹͣ����һ������
	UFT_M_TEST_LINK		= 0x07, //������·ָ��

	//�ǽ�ģ�飨NFC����������
	UFT_N_CONSUME_RES_W2	= 0x11,	//�������Ͳ�ѯ���������п����ѣ�
	UFT_N_CONSUME_RES_W3	= 0x12, //�������Ͳ�ѯ������POSͨ���ѣ�
	UFT_N_CONSUME_TIP		= 0x13, //������Ϣ��ʾ(NFCģ�����)
	UFT_N_CONSUME_RES_DATA	= 0x14, //��������
	UFT_N_CONSUME_ERR		= 0x15, //�����쳣��ˢ������װ���ĳ���
	UFT_N_CONSUME_NO_SIGN	= 0x16, //δǩ��

	UFT_N_CORRECT_RES_DATA  = 0x17, //��������
	
	UFT_N_SIGN_IN_RES_DATA	= 0x18, //ǩ������
	UFT_N_SIGN_VERIFY_ERR	= 0x19, //ǩ������У���
	UFT_N_SIGN_CONFIRM		= 0x1A, //ǩ��ȷ��
	
	UFT_N_TEST_LINK_SUCCESS	= 0x1B, //�������Գɹ�
	UFT_N_TEST_LINK_ERROR	= 0x1C, //��������ʧ��
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
//	int		StationID;		//վ�����
	TCHAR	StationID[36];	//վ�����
	TCHAR	LaneID[8];		//��������
	int		LaneType;		//��������(1,���;2,����)
	TCHAR	LaneTab[16];	//����Ψһ����
	TCHAR	StationTab[32]; //վ��Ψһ����
	int		STServerTpye;	//վ������������
	TCHAR	STServerIP[24];	//վ��������IP��ַ
	TCHAR	STServerID[24];	//վ��������ID����
	TCHAR	WebServerIP[24];//�������������IP��ַ
}tagLaneConfigInfo;

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

//MMI������ý������ṹ��Ϣ
typedef struct _tagMMIMedia
{
	int		Type;
	TCHAR	Content[128];
	int		Spare;
}tagMMIMedia;




#pragma pack(1)

//������Ϣ
struct tagUpdateInfo
{
	int		Type;			//�������ͣ�1-���ֹ��,2-���Թ��
	char	InfoNo[32];		//���±���
	char	InfoName[64];	//��������
	char	InfoVersion[32];//���°汾
	time_t	UpdateTime;		//��Ϣ�޸�ʱ��
	time_t	LoadTime;		//��Ϣ�·�ʱ��
	time_t	BeginDate;		//��������
	time_t	EndDate;		//ͣ������
	int		IsCondense;		//�Ƿ�ѹ����0-��ѹ����1-ѹ��
	int		FileType;		//�ļ����ͣ�1-�ı�
	char	Content[256];	//�ļ�����
	char	MD5[40];		//MD5
	int		TransferMark;	//������
	int		Spare;			//����
};

#pragma pack()


#endif //_TRAWE_EMBEDDED_BASE_DEFINE_H_




//�������ݻ�����
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
	CString	csVersion;		//Э��汾��
	CString	csPacktype;		//��������
	time_t	tPackTime;		//����ʱ��
	int		ProvinceID;		//ʡ�ݱ���
	int		RoadID;			//·�α���
	CString	csStationID;	//վ�����
	int		LaneID;			//��������
	CString	csDeviceNo;		//�豸����
	CString	csDeviceType;	//�豸����
	CString	UpdateCode;		//��������Ψһ��
	CString	csAEScode;		//AES
	CString	csMD5code;		//MD5

	int		ErrorCount;		//���ع����еĳ������

	CArray<tagUpdateInfo, tagUpdateInfo&>m_aryUpdateInfo;	//������Ϣ����
};
