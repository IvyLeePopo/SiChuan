// TWSDNetPay_Dll.h : main header file for the TWSDNetPay_Dll DLL
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "BaseDefine.h"
#include "ThreadManage.h"
#include "SuperLog.h"

#include "DVXMLAccount.h"
//#include "DVXMLDebitMoney.h"
#include "DVXMLDebitCancel.h"
#include "DVXMLDebitResult.h"
#include "DVXMLMMI.h"
#include "PayKeyItems.h"
#include "DVXMLDebitFlow.h"

#include "JsonParamOperate.h"

#pragma  comment(lib, "Version.lib")

#include "Ntsecapi.h"

#include "AgencyPay.h"

#include "LPRInteractive.h"

//#include "CommonBus.h"/*add zhuyabing 2017-11 ���ƽ̨�ӿ���*/
//#pragma  comment(lib, "CommonBus.lib")

//#include "unexcept\CatchDump.h"
/**
 * 
 */

//ͨ�ûص�����
typedef void (WINAPI *NotifyCallerFun)(int iOption, int iResult);
//����ר�ûص�����
typedef void (WINAPI *NotifyCallerFun45)(int iOption, int iResult, void* pIndPtr);


typedef int (*recvFromBus_X_def)(int destKey, char *msg, int len, int srcKey);

typedef int (*GenerateMainBus)(recvFromBus_X_def fun);
typedef int (*send2Bus_Target)(int main_handle,int destKey, char *msg, int len,int srcKey);
typedef void (*register2Bus_Target)(int main_handle,int key);
typedef void (*unRegister2Bus_Target)(int main_handle,int key);

typedef int (*NewBusConnector)(recvFromBus_X_def fun);
typedef void (*RemoveBusConnector)(int conn_handle);



class CTWSDNetPay_DllApp : public CWinApp//, public CommonBus /**/
{
public:
	CTWSDNetPay_DllApp();
    ~CTWSDNetPay_DllApp();//add 2017-11 for CommonBus

private:
	//CCatchDump* m_pCatchDump;
//Behavior
public:
	//��ʼ���������			-- �ӿ�1
	static bool WINAPI  IF_InitEnvironment (IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID);
	//��ʼ���������			-- �ӿ�1-1
	static bool WINAPI  IF_InitEnvironment2 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, void (WINAPI *fun)(int option, int result));
	//��ʼ���������			-- �ӿ�1-2(ר��Ϊ���������ƿ�����
	static bool WINAPI  IF_InitEnvironment3 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, IN OUT void* pIndPtr, void (WINAPI *fun)(int option, int result, void* pIndPtr));
	//������Դ					-- �ӿ�2
	static bool WINAPI  IF_Destroy(void);
	//��ȡ��һ�δ�����Ϣ		-- �ӿ�3
	static const char* WINAPI IF_GetLastErrorDesc(void);
	//������ȡ�û���Ϣ����		-- �ӿ�4
	static bool WINAPI  IF_StartGetAccountInfo(IN const int& iOvertime);
	//ֹͣ��ȡ�û���Ϣ����		-- �ӿ�5
	static bool WINAPI  IF_StopGetAccountInfo(void);
	//��ȡ�û���Ϣ				-- �ӿ�6
	static bool WINAPI  IF_GetAccountInfo(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType);
	//ִ�пۿ����				-- �ӿ�7
	static bool WINAPI  IF_DebitMoney(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType);
	//ִ�г�������				-- �ӿ�8
	static bool WINAPI  IF_DebitCancel(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType);
	//��ȡ�ۿ���				-- �ӿ�9
	static bool WINAPI  IF_GetDebitResult(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType);
	//���ý�������				-- �ӿ�10
	static bool WINAPI	IF_SetMMI(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType);
	//��ȡӲ������				-- �ӿ�11
	static bool WINAPI	IF_GetParam(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, IN OUT char* szResultContext, IN OUT int& iResultSize);
	//������״̬				-- �ӿ�12
	static bool WINAPI  IF_GetComponentStatus(IN OUT unsigned int& uStatus);
	//ִ�д���ؼ���Ϣ���ݲ���	-- �ӿ�13
	static bool WINAPI  IF_TranslateData(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType);
	//����ͼ��	-- �ӿ�14
	static bool WINAPI  IF_TransferImage(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType);
	//���۲�ѯ
	static bool WINAPI	IF_AgencyDebitQuery(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, IN OUT int& iResult);


	//�ڲ��ӿڣ���ȡӲ�����ն����
	static bool WINAPI	IF_InnerGetDeviceNo(IN char* szParamContext, IN int& iParamSize);
	//�ڲ��ӿڣ���ȡָ������
	static bool WINAPI  IF_InnerGetSpecialData(IN char* szParamContext, IN int& iParamSize);
	//�ڲ��ӿڣ�����ָ������
	static bool WINAPI	IF_InnerSetSpecialData(IN const char* szParamContext, IN const int& iParamSize);

struct NFCParaInfo
{
	char Company[100];//��˾
	char Type[100];//�ͺ�
	char SerialNumber[100];//Spec���к�
	char SoftVersion[100];
	bool valid;
	NFCParaInfo()
	{
		reset();
	}
	void reset()
	{
		memset(this,0,sizeof(NFCParaInfo));
	}
};

private:
    static void GenerateRawInitPara(const tagTranstParam& para);
    static std::string m_CommonInitPara;
	int DevcieQueryInterrupt(const char* para);//Ӳ����ѯ�жϣ�Ŀǰֻ�в��Թ��ߣ��ᷢ�������
	// int SelfServiceTerminalInterrupt(int destKey,const char* para,int size,int srcKey);
	//add 2018-04
	int QueryImageIdFromLPRSystem();//�ӳ���ʶ��ϵͳ��ץȡ���µ�ʶ��ͼ��ID��
	
	void SetLPRQueryOverTimeMilli(int LPRQueryOverTimeMilli=100);
	int GetLPRQueryOverTimeMilli();
	int m_LPRQueryOverTimeMilli;
public:
	bool m_NfcLinkSuccess;
	NFCParaInfo m_NfcPara;
	LPRInteractive m_LPR;//
/*commonbus */
//add 2018-03 ���޸�����ļ��ط�ʽ
public:
	int send2Bus(int destKey,char *msg,int len,int srcKey=0);
	int recvFromBus(int destKey, char *msg, int len, int srcKey );
	
	void LPR_PackPayResult(int LPR);
private:/*ʵ�����ƽ̨�ӿ�*/
	void ConnectBus();
	void DisConnectBus();
	void register2Bus(int key);
    void unRegister2Bus(int key);


	NewBusConnector m_connect_bus_core;
	RemoveBusConnector m_disconnect_bus_core;
	send2Bus_Target m_send_core;
	register2Bus_Target m_register_core;
	unRegister2Bus_Target m_unRegister_core;
	int m_bushandle_core;
	HMODULE m_commonbus_module;
/*commonbus above*/
private:
	static time_t m_lastPayTime;//�����ж�������޽��ף������¶�̬��
public:
	
	///static CommonBus*  WINAPI getInstance(); //
	//static CommonBus* WINAPI deleteInstance(); //

	// ���ز�ͬ������Ϣ��д��־����
#ifdef _DEBUG
	static void WINAPI RecordLog(IN const CString &strLog, IN const int& iLevel = 0, IN const BOOL& bForce = FALSE);
#else
	static void WINAPI RecordLog(IN const CString &strLog, IN const int& iLevel = 1, IN const BOOL& bForce = FALSE);
#endif
	//�̹߳رպ���
	static int fnCloseThread(IN const HANDLE& hThread, IN const DWORD& dwMilliseconds);

protected:
	//Ȩ����֤
	static bool WINAPI  IF_Authority(IN const char* szAuthKey, IN const int& iAuthKeySize);
	//��ȡ������Ϣ
	static void WINAPI  ReadConfigInfo(void);
//�ڲ���������
public:
	//ʵ������CString���Format����
	static CString FmtStr(const TCHAR* lpsFmt, ...);
	//��ʱ����ַ�����ʽת��Ϊtime_t��ʽ
	static time_t CString2time_t(LPCTSTR lpstime);
	//��ȡGUID
	const CString GetGuidString(IN const int& iType);
	//��ȡָ���ļ��İ汾��
	static CString GetVersionOfFile(LPCTSTR lpsPath);
	//��ȡ��ǰʱ����ַ�����ʽ
	static const CString GetCurrentTimeString(IN const int& iTimeType);
	//����ָ��Ŀ¼
	static BOOL MakeDirectory(CString strCreateDir);
	//��ʾ��һ�ε�ϵͳ������Ϣ
	static LPCTSTR DisplayWinError(DWORD dwErrorCode, int nType = 0, BOOL bPrompt = FALSE);
	//��ȡ��ǰ������ʹ�õ��ڴ�
	static DWORD GetProcessMemory(IN const int& iShowType = 0);
	//֪ͨ�ⲿ�����ߣ��첽������
	static void NotifyCaller(IN const int& iOption, IN const int& iResult);
	//���������ַ����Ƿ�Ϊ��ĸ�����ֵ����
	static BOOL CheckIsAlNum(IN const char* szData, IN const int& iType = 0);

	static BOOL CheckStringLetter(char * strCheck, int nType = 0);

	//����ת����16����תBASE64��
	static BOOL HexToBase64(IN const CString& strSrcHex, OUT CString& strDesBase64);
	static INT  BASE64_Encode(const BYTE* inputBuffer, INT inputCount, TCHAR* outputBuffer);
	static BOOL Base64ToHex(IN const CString& strSrcBase64, OUT CString& strDesHex);
	static BYTE Decode_GetByte(char c);
	//16����ת2����
	static void Hex2Bin(IN LPTSTR pHex, OUT PBYTE pBin, IN DWORD dwHex_len);
	//2����ת16����
	static CString Bin2Hex(IN PBYTE pBin, IN DWORD dwBin_Len);

	//�������ۿۿ�ӿڷ�����������
	static int GetAgencyResult(CString strXML, DWORD dwConsumeTime);
	//������ۿ۷����ݲ�ִ�д���
	static bool AgencyConstructDebitData();
	//������۳������ݲ�ִ�г���
	static int AgencyConstructCancelData(IN const CString& strPayIdentifier);
	//��ʼ�����۶�̬��
	static bool AgencyInitDll(IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID);

	//��ȡ�����ǽ�ģ��״̬
	static int GetUnipayWorkFlowState(void) {return m_iNFCModuleState;}
	//���������ǽ�ģ��״̬
	static void SetUnipayWorkFlowState(IN const int& iUnipayState) { m_iNFCModuleState = iUnipayState; }
	

	static CTime FixUTC8(const CTime& old_time);//ǿ��ת��UTC8
	static time_t FixUTC(const time_t& old_time);//ǿ��ת��UTC
	static CTime FixUTC(const CTime& old_time);//ǿ��ת��UTC
// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
private:/*���ƽ̨����*/

	
//Attributes
public:
	//ҵ������߳���ָ��
	static CThreadManage*	m_pThreadManage;
	//��ʶ�齨�����Ƿ�������ʼ��
	static BOOL m_bIsInitEnvironment;
	//��ʶ�����Ƿ񾭹��Ϸ���Ȩ
	static BOOL m_bAuthorizedLegal;
	//��ʶ֧����ƽ̨�����Ƿ�����
	static BOOL m_bNetErrorHttpServerInTollNetNotExist;
	//��ʶ֧����ƽ̨�������Ƿ�����
	static BOOL m_bNetErrorHttpServerTrawePlatformNotExist;
	//��ʶ�Ƿ���Ҫ������־
	static BOOL	m_bSaveLog;
	//�̴߳����ʱ����(��λ:��),Ĭ��5���Ӽ��һ��
	static DWORD   m_dwThreadCheckSpanTime;
	//�洢����̬�⹤��Ŀ¼
	static CString m_strWorkDir;
	//�洢�ϴδ�����Ϣָ��
	static CString m_strLastErrorDesc;
	//�洢���⿨��ǰ10λ
	static CString m_strVirtualCardPrefix;
	//�洢�߳�������Ϣ
	static tagTranstParam	 m_sTranstParam;
	//��ǰ�Ŀۿ���������ϲ������ǵ������
	static BOOL	m_bCurrentDebitMode;
	//��ǵ�ǰ�Ƿ����ڽ���(��ҵ������߳̽�������:1.�������;2,��ʱ����)
	static BOOL m_bIsTrading;
	//�洢��ǰ�ۿ�����Ĺؼ���Ϣ
	static CPayKeyItems	m_pCurrentDebitInfo;
	//�洢��ǰ�ۿ����Ĺؼ���Ϣ
	static tagDebitResultItems	m_sCurrentDebitResult;
	//��ǰ�ۿ�����ĳ�ʱ��ʼʱ��
	static DWORD m_dwTradeBeginOvertime;
	//������Ӳ������ʱ��ļ��
	static DWORD m_dwUpdateDevieTimeSpanTime;
	//��ʶ��������
	static BOOL	 m_bIsTestData;
	//ҵ��̬��汾��Ϣ
	static CString m_strNetPayDllVersion;
	//�豸���ƶ�̬��汾
	static CString m_strDeviceDllVersion;
	//��־ɾ������ָ��ʱ������
	static int	m_iDeleteLogStartHour;
	//��־ɾ������ָ��ʱ����յ�
	static int	m_iDeleteLogEndHour;
	//��־ɾ��������ѯʱ��
	static int	m_iDeleteLogSpanTime;
	//��־��������
	static int  m_iDeleteLogSaveDays;
	//��ʶ�ۿ����̱���
	static int	m_iDebitTaskNo;
	//��ʶ���֪ͨ�ⲿ�����ߵķ�ʽ(1,��Ϣ���ף�2��ʹ�ûص�����)
	static int	m_iNotifyMode;
	//�������ػ�����ʱʱ�䣨��λ�����ӣ�
	static int  m_iLastDelayTime;
	//�ط����ݵ�ʱ��������λ���룩
	static time_t m_tResendDataSpanTime;
	//��ʶ���֪ͨ�ⲿʹ�õĻص�����ָ��
	static NotifyCallerFun m_fnCallbackNotifyFun;
	//��ʶ���֪ͨ�ⲿʹ�õĻص�����ָ�루����ר�ã�
	static NotifyCallerFun45 m_fnCallbackNotifyFun45;
	//ר��Ϊ���������ĳ�ʼ���ӿ�3�����ڱ���һ�����Ի�ָ�룬�ڻص�֪ͨ��ʱ�򴫵ݳ�ȥ
	static void* m_pIndPtrFor45;
	//�ȴ��˳�ʱ��(��λ�����룩
	static int	m_iExitWaitForTime;
	//���±��ش洢�ļ���
	HANDLE	hFileUpdate;
	//HTTP�ڲ��汾�ţ����ֲ�ͬ�Ĵ����߼�
	static int	m_iHTTPVersion;
	//���ֵײ��豸���ͣ�Ŀǰ0Ĭ�ϱ�ʾ��΢�����豸��1��ʾ��Ϊ�豸
	static int	m_iDeviceType;
	//�Ƿ񽫴�����ɨ��֧��������������ͳһ
	static bool m_bReturnPayChannelCodeUnify;
	//���ۿ۷ѳ�ʱʱ�䣬��λ������
	static int  m_iAgencyDebitTimeout;
	//���������ӳ�ʱ�䣬��λ�����룬Ĭ��200����
	static int	m_iClearMMIDelayTime;
	//�Ƿ����ô��۶�̬��
	static bool	m_bUseAgencyDll;
	//���۶�̬�������
	static CAgencyPay	m_AgencyPay;
	//��΢�ն˲�Ʒ���к�
	static CString m_strTWProductSerialNo;
	//add 2017-12 
	static CString m_strTWProductFirmwareVer;
	static CString m_strTWProductSlaveFirmwareVer;
	//�Ƿ���չMMI����ϸ��
	static BOOL	   m_bIsExternMMIDetail;
	//�Ƿ�����ʹ���°���չ����ϸ��ģʽ
	static BOOL	   m_bIsAllowUseNewMMIDetailMode;

	//�Ƿ����������ǽ�֧��ҵ��
	static bool m_bNFCIsUse;
	//����ģ��Ĺ���״̬
	static int  m_iNFCModuleState;
	//����ģ���Ƿ��Ѿ�ǩ���ɹ�
	static bool m_bNFCSignInSuccess;
	//����ģ��ǩ��״̬���ʱ��������λ����(Ĭ��10��һ�Σ�
	static int	m_iNFCCheckSignSpanTime;
	//����ģ��NFC֧���쳣
	static BOOL m_bNFCPayFail;
	//����ģ��NFC֧��ʧ�ܴ�����������
	static int	m_iNFCPayFailCount;
	//����ģ���ն˱��루BASE64�ַ���ʽ��
	static CString m_strNFCTerminal;
	//����ģ��Ӳ�����кţ�BASE64�ַ���ʽ��
	static CString m_strNFCSerialNo;
	//����ģ��ǩ����������
	static int m_iNFCSignFlowDataCount;
	//����ģ�鶨ʱ��ǩ���ʱ����ʼ��
	static int m_iNFCDefaultStartReSignHour;
	//����ģ�鶨ʱ��ǩ���ʱ���ֹ��
	static int m_iNFCDefaultEndReSignHour;
	//����ģ��ÿ�춨����ǩ�Ƿ�ִ����ϣ�ÿ��һ�Σ�
	static BOOL m_bNFCAutoSignIsExecute;
	//����ģ��ǩ�����ݽ���ʱ�������ƣ���λ���룩
	static int  m_iNFCSignDataLimitSpanTime;
	//����ģ���ϴ�ǩ�����ݻ�ȡʱ�䣨ÿһ��ǩ���ֻص�����Ч��
	static time_t m_tNFCLastSignDataTime;
	//����֧�����̣��Ƿ�����û�л�ȡ��������Ҳ�����Ϳ۷�����
	static BOOL	m_bNFCIsAllowDebitWithoutCancelData;
	//����֧�����̣���ȡ��ˢ�����ݺ����Ƶȴ��������ݵ�ʱ�䣨��λ���룩
	static int m_iNFCGetCancelDataLimitTime;
	//����ģ��ÿ�ο�ʼǩ����ʱ���
	static time_t m_tNFCStartSignInTime;
};

extern CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp();

enum COMMPENTS_KEY_TYPE
{
    KEY_COMMON_FRAME=1,

    KEY_COM_MANAGER=10,
    //ҵ����
    KEY_BUSINESS=20,
    KEY_BUSINESS_DEBIT=21,
    KEY_BUSINESS_DEBIT_CANCEL=22,
    KEY_BUSINESS_DEBIT_INIT=23,



    KEY_DB_MANAGER=40, //���ݿ�
    KEY_AUTO_UPDATE_MANAGER=60,//�Զ�����
    KEY_UPDATE_QUERY,
    KEY_UPDATE_DOWNLOAD,
    KEY_UPDATE_CONFIRM,
    KEY_UPDATE_HTTP_MANAGER,

    KEY_HTTP_MANAGER=70,//HTTP������
    //
    //����Ӳ����
    KEY_HEARTBEAT_INQUIRY_COM=101,//������ѯ
    KEY_HARDWARE_VERSION=102,//��Ӳ���汾
    KEY_READ_CANID=103,//��CANID
    KEY_READ_MAX=104,//��MAX��
    KEY_FIRMWARE_REVISION=105,//���̼��汾
    KEY_SCANNING_TIME=106, //��ɨ��ʱ����������
    KEY_SERIAL_NUMBER=107,//Ψһ���
    KEY_READ_BOOT_INTERFAC=108,//����������
    KEY_READ_IP_ADDERSS0=109,//������0(eth0) IP ��ַ
    KEY_READ_IP_ADDERSS1=110,//������1(eth1) IP ��ַ
    KEY_READ_TEMPERATURE=111,//��Ӳ���¶�
    KEY_READ_CURRENT_TIME=112,//��Ӳ����ǰʱ��
    KEY_READ_ETH0_NETMASK=113,
    KEY_READ_ETH0_GATEWAY=114,
    KEY_READ_ETH1_NETMASK=115,
    KEY_READ_ETH1_GATEWAY=116,
    KEY_READ_DEFAULT_GATEWAY=117,
    KEY_READ_SCANER_VERSION=118,

    KEY_SET_LCD_PARAMS=200,
    KEY_SET_HEARTBEAT_SEPTUM=201,//�����ն��������
    KEY_SET_BOOT_INTERFACE=202,//���ÿ�������
    KEY_DEBIT_FLOW_CHART_COM=203,//���ƽ�����ʾ��Ϣ(���Թ���
    KEY_DISPLAY_SEE_AGAIN_COM=204,//���ƽ������ټ���
    KEY_DISPALY_ADVERTISING_COM=205, //���ƽ�������棩
    KEY_ISSUED_TIME_COM=206,//�·�����ʱ��
    KEY_SET_IP_ADDERSS0_COM=207,//��������0(eth0) IP ��ַ
    KEY_SET_IP_ADDERSS1_COM=208, //��������1(eth1) IP ��ַ
    KEY_SET_ETH0_NETMASK=209,
    KEY_SET_ETH0_GATEWAY=210,
    KEY_SET_ETH1_NETMASK=211,
    KEY_SET_ETH1_GATEWAY=212,
    KEY_SET_DEFAULT_GATEWAY=213,
    KEY_SET_HW_VERSION214=220,

    KEY_SET_MAX_COM=301,//дCANID
    KEY_SET_CANID_COM=302, //дMAX��
    KEY_SET_SERIAL_NUMBER_COM=303,//Ψһ���
    KEY_SET_SCANNING_TIME_COM=304,//��ɨ��ʱ����������
    KEY_POLL_TEST_COM=305, //��Ŀ������ѯ����
    KEY_DATA_TO_PORT_COM=306,//͸�����ݵ�ָ������
    KEY_SYS_REBOOT_ST=307,//reboot smartTerminal
	
	KEY_COMMONBUS_ACCESS = 2002,//
    KEY_DEVICE_TEST = 9527
};