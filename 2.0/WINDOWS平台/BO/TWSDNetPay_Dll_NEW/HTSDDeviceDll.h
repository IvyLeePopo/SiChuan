/*
* Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
* 
* �ļ����ƣ�HTSDDeviceDll.h
* ժ    Ҫ������Ӳ���ӿڶ�̬������࣬�����ļ�
* ��ǰ�汾��1.0.0.1
* ��    �ߣ�ROCY
* �������ڣ�2016��7��24��
*/
#pragma once

class CHTSDDeviceDll
{
	//��������
public:
	/*******************************��������************************************/
	//��ʼ��Ӳ��
	typedef BOOL	(WINAPI  *defIF_InitDev)(IN const UINT& nThreadID,IN const HWND& hWnd,IN const UINT& nNotifyMsgID,IN const int& iProvinceID,IN const int& iOverTime);
	//������Դ
	typedef BOOL	(WINAPI  *defIF_Destroy)(void);
	//��ȡ��һ�δ�����Ϣ
	typedef CString (WINAPI  *defIF_GetLastErrorDesc)(void);
	//ִ�м��Ӳ��״̬����
	typedef BOOL	(WINAPI  *defIF_CheckDeviceStatus)(IN OUT DWORD& dwState);
	//ִ�������·�����
	typedef BOOL	(WINAPI  *defIF_IssuedData)(IN const CString& strXML);
	//ִ�п���IO����
	typedef BOOL	(WINAPI  *defIF_SetIOState)(IN const int& iIOState);
	//Ȩ����֤����
	typedef	BOOL	(WINAPI  *defIF_Authority)(IN const CString& strData);
	//��ȡ�̼��汾��Ϣ
	typedef	BOOL	(WINAPI  *defIF_GetDeviceFirewareVer)(IN OUT CString& strXML);
	
	/*******************************TR200����************************************/
	//ִ��������ȡ���ݲ���
	typedef BOOL	(WINAPI  *defIF_StartGetData)(IN const int& iOvertime);
	//ִ��ֹͣ��ȡ���ݲ��� 
	typedef BOOL	(WINAPI  *defIF_StopGetData)(void);
	//��ȡ���ݽ��
	typedef BOOL	(WINAPI  *defIF_GetData)(IN OUT CString& strData,IN OUT WORD& iDataType,IN OUT int& iSrcType);
	//��ȡ��ʾ������Ϣ
	typedef	BOOL	(WINAPI  *defIF_GetDisplayContext)(IN OUT int&, CString&);
	//������ʾ������Ϣ
	typedef	BOOL	(WINAPI  *defIF_SetDisplayContext)(IN const int&,IN const CString& strXML);

	/*******************************TR200����************************************/
	//ִ�пۿ����
	typedef BOOL    (WINAPI *defIF_DebitMoney)(IN const CString& strDebitInfo, IN const int iFormatType);
	//ִ�г�������
	typedef BOOL    (WINAPI *defIF_DebitCancel)(IN const CString& strDebitInfo, IN const int iFormatType);
	//ִ���豸��ʼ���ò���
	typedef BOOL    (WINAPI *defIF_InitDeviecCMD)(IN const CString& strInitInfo, IN const int iFormatType);


public:
	CHTSDDeviceDll(void);
	~CHTSDDeviceDll(void);
public:
	//��ʼ����̬��
	const BOOL Init(IN const CString& strDllFilePath);
	//��ȡ��̬�����״̬
	inline const BOOL GetDllInitState(void){return m_bInitDll;}
	//��ȡ�����ʼ��״̬
	inline const BOOL GetDevInitState(void){return m_bInitDev;}
	//Operations
public:
	/*******************************��������************************************/
	//�����ʼ��
	BOOL InitDev(IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const int& iProvinceID,IN const int& iOverTime);
	//������Դ
	BOOL Destroy(void);
	//��ȡ��һ�δ�����Ϣ
	CString GetLastErrorDesc(void);
	//ִ�м��Ӳ��״̬����
	BOOL CheckDeviceStatus(IN OUT DWORD& dwState);
	//Ȩ����֤����
	BOOL Authority(IN const CString& strData);
	//��ȡ�̼��汾��Ϣ
	BOOL GetDeviceFirewareVer(IN OUT CString& strXML);
	//ִ�������·�����
	BOOL IssuedData(IN const CString& strXML);
	
	/*******************************TR200����************************************/
	//ִ��������ȡ���ݲ���
	BOOL StartGetData(IN const int& iOvertime);
	//ִ��ֹͣ��ȡ���ݲ���
	BOOL StopGetData(void);
	//��ȡ���ݽ��
	BOOL GetData(IN OUT CString& strData,IN OUT WORD& iDataType,IN OUT int& iSrcType);
	//��ȡ��ʾ������Ϣ
	BOOL GetDisplayContext(IN OUT int& iID, CString& strXML);
	//������ʾ������Ϣ
	BOOL SetDisplayContext(IN const int& iID,IN const CString& strXML);
	//ִ�п���IO����
	BOOL SetIOState(IN const int& iIOState);
	
	/*******************************TR300����************************************/
	//�ۿ����
	BOOL DebitMoney(IN const CString& strDebitInfo,IN const int& iType);
	//��������
	BOOL DebitCancel(IN const CString& strCancelInfo,IN const int& iType);
	//�豸��ʼ�����ò���
	BOOL InitConfigDevice(IN const CString& strInitInfo,IN const int& iType);

	//Attributes
private:
	//��̬����ؾ��
	HMODULE	m_hModule;
	//��̬���ʼ�����
	BOOL	m_bInitDll;
	//�����ʼ�����
	BOOL	m_bInitDev;
	//����˵��
	CString m_strLastError;
	//�����������
	defIF_InitDev				fnIF_InitDev;
	defIF_Destroy				fnIF_Destroy;
	defIF_GetLastErrorDesc		fnIF_GetLastErrorDesc;
	defIF_StopGetData			fnIF_StopGetData;
	defIF_CheckDeviceStatus		fnIF_CheckDeviceStatus;
	defIF_GetData				fnIF_GetData;
	defIF_Authority				fnIF_Authority;
	defIF_GetDeviceFirewareVer	fnIF_GetDeviceFirewareVer;
	defIF_GetDisplayContext		fnIF_GetDisplayContext;
	defIF_SetDisplayContext		fnIF_SetDisplayContext;
	defIF_IssuedData			fnIF_IssuedData;
	defIF_SetIOState			fnIF_SetIOState;

	defIF_DebitMoney			fnIF_DebitMoney;
	defIF_DebitCancel           fnIF_DebitCancel;
	defIF_InitDeviecCMD			fnIF_InitDeviecCMD;
};
