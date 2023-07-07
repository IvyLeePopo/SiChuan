// ScanMachine.h : main header file for the ScanMachine DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "ThreadSMManager2.h"
//#include "ThreadSMManager.h"


// CScanMachineApp
// See ScanMachine.cpp for the implementation of this class
//

class CScanMachineApp : public CWinApp
{
public:
	CScanMachineApp();

	CString	csAppDir;	//��¼��������Ŀ¼
	CString	csLogFile;	//��¼����������־����·��
	CString	csDllFile;	//���صĵײ㶯̬��·��
	CString	csCfgFile;	//��ȡ�����ļ�·��ʧ�ܺ����õ�Ĭ�������ļ�·�����붯̬��ͬĿ¼��
	int		iWriteLog;	//�Ƿ�д��־���
	int		m_iLogFileMaxSize;	//��־�ļ����ֵ
	int		m_iNeedNotifyLoopState;//�Ƿ���Ҫ�·���������/�뿪ָ��

	CThreadSMManager2* m_pThreadSMManager;
//	CThreadSMManager* m_pThreadSMManager;

	CString GetCurrentDirNew();
	BOOL	CreateDir(LPCTSTR lpzPath);
	void	RecordMsgTemp(IN LPCTSTR strMsg,IN const BOOL& bForce=FALSE);

	time_t	OleTimeToTime(DATE dTime);
	time_t	CString2time_t(LPCTSTR lpztime);

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
	
};



BOOL WINAPI IF_InitDev(IN const UINT& dwThreadID, IN const HWND& hWnd,IN const UINT& dwNotifyMsgID, IN const int& iProvinceID, IN const int& iOverTime);
BOOL WINAPI IF_Destroy(void);
CString WINAPI IF_GetLastErrorDesc(void);
BOOL WINAPI IF_StartGetData(IN const int& iDelayTime);
BOOL WINAPI IF_StopGetData( void );
BOOL WINAPI IF_CheckDeviceStatus(IN OUT DWORD& dwState);
BOOL WINAPI IF_GetData(IN OUT CString& strData, IN OUT int& iDataType, IN OUT int& iSrcType);
BOOL WINAPI IF_Authority(IN const CString& strAuthKey);
BOOL WINAPI IF_GetDeviceFirewareVer(IN OUT CString& strXML);
BOOL WINAPI IF_GetDisplayContext(IN OUT int& iID, IN OUT CString& strXML);
BOOL WINAPI IF_SetDisplayContext(IN const int& iID, IN const CString& strXML);
BOOL WINAPI IF_IssuedData(IN const CString& strXML);
BOOL WINAPI IF_SetParam(IN const CString& strXML);
BOOL WINAPI IF_SetIOState(IN const int& iIOState);
