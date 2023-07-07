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

#include "JsonParamOperate.h"

#pragma  comment(lib, "Version.lib") 

#include "Ntsecapi.h"

class CTWSDNetPay_DllApp : public CWinApp
{
public:
	CTWSDNetPay_DllApp();
	//Behavior
public:
	//��ʼ���������			-- �ӿ�1
	static bool WINAPI  IF_InitEnvironment (IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const char* szAreaInfo,IN const char* szLoaclStationID,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID);
	//������Դ					-- �ӿ�2
	static bool WINAPI  IF_Destroy(void);
	//��ȡ��һ�δ�����Ϣ		-- �ӿ�3
	static const char* WINAPI IF_GetLastErrorDesc(void);
	//������ȡ�û���Ϣ����		-- �ӿ�4
	static bool WINAPI  IF_StartGetAccountInfo(IN const int& iOvertime);
	//ֹͣ��ȡ�û���Ϣ����		-- �ӿ�5
	static bool WINAPI  IF_StopGetAccountInfo(void);
	//��ȡ�û���Ϣ				-- �ӿ�6 
	static bool WINAPI  IF_GetAccountInfo(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType);
	//ִ�пۿ����				-- �ӿ�7
	static bool WINAPI  IF_DebitMoney(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//ִ�г�������				-- �ӿ�8
	static bool WINAPI  IF_DebitCancel(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//��ȡ�ۿ���				-- �ӿ�9
	static bool WINAPI  IF_GetDebitResult(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType);
	//���ý�������				-- �ӿ�10
	static bool WINAPI	IF_SetMMI(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	//��ȡӲ������				-- �ӿ�11
	static bool WINAPI	IF_GetParam(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType,IN OUT char* szResultContext,IN OUT int& iResultSize);
	//������״̬				-- �ӿ�12
	static bool WINAPI  IF_GetComponentStatus(IN OUT unsigned int& uStatus);
	//ִ�д���ؼ���Ϣ���ݲ���	-- �ӿ�13
	static bool WINAPI  IF_TranslateData(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);

	//test
	static bool WINAPI IF_GetC0SuccFaileCount(OUT int& nSucce, OUT int& nFailed, OUT int& nReSendCount, OUT char* strA2Frame, OUT char* strC0Frame);

	// ���ز�ͬ������Ϣ��д��־����
#ifdef _DEBUG
	static void WINAPI RecordLog(IN const CString &strLog,IN const int& iLevel = 0,IN const BOOL& bForce = FALSE);
#else
	static void WINAPI RecordLog(IN const CString &strLog,IN const int& iLevel = 1,IN const BOOL& bForce = FALSE);
#endif
	//�̹߳رպ���
	static int fnCloseThread(IN const HANDLE& hThread,IN const DWORD& dwMilliseconds);

protected:
	//Ȩ����֤
	static bool WINAPI  IF_Authority(IN const char* szAuthKey,IN const int& iAuthKeySize);
	//��ȡ������Ϣ
	static void WINAPI  ReadConfigInfo(void);
	//�ڲ���������
public:
	//ʵ������CString���Format����
	static CString FmtStr(const TCHAR* lpsFmt,...);
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
	static LPCTSTR DisplayWinError(DWORD dwErrorCode,int nType=0,BOOL bPrompt=FALSE);
	//��ȡ��ǰ������ʹ�õ��ڴ�
	static DWORD GetProcessMemory(IN const int& iShowType=0);
	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
private:
	//�ع��ۿ�JSON/xml
	static CString CostructDebitInfo(int nFormatType, tagPayKeyItems& pkItems);

	//ת����ȡ�Ŀۿ���Ϊxml/json��ʽ������
	static void	ConverToFormatData(int iFormatType,IN OUT CString& strResult);

	//�ع�����josn/xml
	static BOOL ConstructMMIInfo(int nFormateType, IN OUT CString& strResult,OUT BOOL& bClearCmd);

public:
	//�ع�����JSON/xml
	static CString ConstructCancleInfo(int nFormatType,CString strPayID);

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
	static tagPayKeyItems	m_sCurrentDebitInfo;
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


};

extern CTWSDNetPay_DllApp theApp;

CTWSDNetPay_DllApp* GetGlobalApp();