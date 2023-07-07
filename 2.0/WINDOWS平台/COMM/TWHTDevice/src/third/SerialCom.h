// SerialCom.h: interface for the CSerialCom class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SERIALCOM_H
#define _SERIALCOM_H

#pragma once



#define ON_COM_RECEIVE WM_APP + 5001    
#define ON_COM_CTS     WM_APP + 5002 
#define ON_COM_DSR     WM_APP + 5003 
#define ON_COM_RING    WM_APP + 5004
#define ON_COM_RLSD    WM_APP + 5005
#define ON_COM_BREAK   WM_APP + 5006
#define ON_COM_TXEMPTY WM_APP + 5007
#define ON_COM_ERROR   WM_APP + 5008 
#define DEFAULT_COM_MASK_EVENT  EV_RXCHAR | EV_ERR | EV_CTS | EV_DSR | EV_BREAK | EV_TXEMPTY | EV_RING | EV_RLSD


class CSerialCom  
{
public:
	CSerialCom();
	virtual ~CSerialCom();
//Operations
	//�ж��Ƿ���˴���
    inline BOOL	IsOpen(){return m_hCom != INVALID_HANDLE_VALUE;} 
	//�жϴ��ڼ����߳��Ƿ�������
	inline BOOL IsThreadRunning(){return m_hThreadHandle != NULL;}
	//�򿪴��ڣ�Ĭ�ϲ���Ϊ��9600,8,N,1
	inline BOOL Open(IN const BYTE& byPort){return Open(byPort, 9600);}
	//�򿪴��ڣ��ṩ��ϸ��������
	inline BOOL Open(IN const BYTE& byPort,IN const DWORD& dwBaudRate,IN const DWORD& dwByteSize = 8,IN const DWORD& dwParity = NOPARITY,IN const DWORD& dwStopBits = ONESTOPBIT);
	//���ô��ڲ���
	inline BOOL SetDTR(IN const BOOL& bOnOrOff){return IsOpen() ? EscapeCommFunction(m_hCom, bOnOrOff ? SETDTR : CLRDTR) != 0 : FALSE;}
	inline BOOL SetRTS(IN const BOOL& bOnOrOff){return IsOpen() ? EscapeCommFunction(m_hCom, bOnOrOff ? SETRTS : CLRRTS) != 0 : FALSE;}
	inline BOOL SetState(IN DCB *pDcb = NULL){ASSERT(pDcb);if(NULL != pDcb) return NULL != memcpy(&m_dcb,pDcb,sizeof(DCB));}
	inline BOOL	SetState(IN const DWORD& dwBaudRate,IN const DWORD& dwByteSize = 8,IN const DWORD& dwParity = NOPARITY,IN const DWORD& dwStopBits = ONESTOPBIT);
	inline BOOL SetTimeouts(IN const LPCOMMTIMEOUTS& OverTime){return IsOpen() ? ::SetCommTimeouts(m_hCom, OverTime) == TRUE : FALSE;}
	inline BOOL SetBufferSize(IN const DWORD& dwInputSize,IN const DWORD& dwOutputSize){return IsOpen() ? ::SetupComm(m_hCom, dwInputSize, dwOutputSize) == TRUE : FALSE;}
	inline void SetNotifyNum(IN const DWORD& dwReadNum){m_dwNotifyNum = dwReadNum;}
	//��ʼ���ڼ����߳�
	inline BOOL BeginThread();
	//�������ڼ����߳�
	inline BOOL EndThread(IN const DWORD& dwWaitTime = 300);
	//���𴮿ڼ����߳�
	inline BOOL SuspendThread(){ return IsThreadRunning() ? 0xFFFFFFFF != ::SuspendThread(m_hThreadHandle): FALSE;}
	//�������ڼ����߳�
	inline BOOL ResumeThread() { return IsThreadRunning() ? 0xFFFFFFFF != ::ResumeThread(m_hThreadHandle): FALSE;}
	
	inline void SetMain(IN const DWORD& dwThreadID,IN const HWND& hWnd = NULL,IN const BYTE& byMode = 0);
	//��ô��ھ��
	inline HANDLE GetHandle(){ return m_hCom; }
	//��ô��ڼ����߳�ID
	inline HANDLE GetThread(){ return m_hThreadHandle; }
	//�����̼߳����¼�ID
	inline void SetMaskEvent(IN const DWORD& dwEvent = DEFAULT_COM_MASK_EVENT);
	//����������뻺��������
	inline void ClearInputBuffer(){if(IsOpen())  ::PurgeComm(m_hCom, PURGE_RXABORT | PURGE_RXCLEAR );}
	//��������������������
	inline void ClearOutputBuffer(){if(IsOpen()) ::PurgeComm(m_hCom, PURGE_TXABORT | PURGE_TXCLEAR );}
	//��ס������Դ
	inline void Lock(){::EnterCriticalSection(&m_Mutex);}
	//����������Դ
	inline void Unlock(){::LeaveCriticalSection(&m_Mutex);}
	//�رմ���
	inline void Close();
	//��ȡ�������뻺������С
	inline const int  GetInputSize();
	//��ȡ���ڶ˺�
	inline const int  GetCOMPort(){return m_byPort;}
	//��ȡ������ϸ�����ṹ��
	inline const DCB& GetState(){return m_dcb;}
	//��ȡ���ڳ�ʱ���ò���
	inline LPCOMMTIMEOUTS GetTimeouts(void);
	//�첽��ȡ����
	inline DWORD Read(IN OUT LPVOID lpData,IN OUT DWORD& dwRLen,const BOOL& bOnlyGetBuffer = TRUE);
	//ͬ����ȡ����
	inline DWORD ReadSync(IN LPVOID lpData,IN const DWORD& dwRLen);
	//�첽д����
	inline DWORD Write(IN const LPVOID lpData,IN const DWORD& dwWLen);
	inline DWORD Write(IN const char *szData){if(NULL != szData && ((DWORD)strlen(szData)>0)) return Write((void *)szData,(DWORD)strlen(szData));}
	inline DWORD Write(IN char *szData,IN const DWORD& dwWLen,IN const char * szFormat, ...);
	//ͬ��д����
	inline DWORD WriteSync(IN const LPVOID lpData,IN const DWORD& dwWLen);
	
protected:
	//���ڳ�ʼ��
    inline virtual void Init();                
	//�ڴ��ڹر�ǰ��������Ӧ����Դ������
    inline virtual void Destroy();
private:
	//�򿪴��ڵ��ڲ�ʵ�ֺ���
	inline BOOL OpenCommPort(IN const TCHAR* szComDesStr);
	inline virtual DWORD ThreadFunc();
	//���Ӵ����߳����庯��
	static DWORD WINAPI SerialThreadFunc(IN LPVOID lpPara){return ( (CSerialCom *)lpPara )->ThreadFunc();}
	//��Ϣ֪ͨ����
	inline virtual void OnMsgReceive();
    inline virtual void OnMsgDSR();
    inline virtual void OnMsgCTS();
    inline virtual void OnMsgBreak();
    inline virtual void OnMsgTXEmpty();
    inline virtual void OnMsgError();
    inline virtual void OnMsgRing();
    inline virtual void OnMsgRLSD();
//Attributes
private:
	volatile BYTE		m_byPort;			//���ں�
	volatile HANDLE		m_hCom;				//���ھ��
    volatile DWORD		m_dwNotifyNum;      //���ܶ����ֽ�ʱ����֪ͨ��Ϣ
    volatile DWORD		m_dwMaskEvent;      //���ӵ��¼�
    volatile BOOL		m_bRunFlag;         //�߳�����ѭ����־
	DWORD				m_dwMainThreadID;   //�����߳�ID
	HWND				m_hMainWnd;			//���ô��ھ��
	BYTE				m_byParentMode;		//������ģʽ(0,�߳�;1,����)
	HANDLE				m_hThreadHandle;	//�����߳̾��
	DWORD				m_dwIOMode;			//����ʹ��ģʽ(�ص�IO)
	DCB					m_dcb;				//���ڲ����ṹ
	OVERLAPPED			m_ReadOverlapped;   //��ȡ�ص�I/O
	OVERLAPPED			m_WriteOverlapped;  //д���ص�I/O
	OVERLAPPED			m_WaitOverlapped;	//�ȴ��ص�IO
	COMMTIMEOUTS		m_OverTime;			//���ڳ�ʱ���ýṹ
	CRITICAL_SECTION	m_Mutex;			//������
};


#include "SystemTrace.h"



inline void CSerialCom::SetMain(IN const DWORD& dwThreadID,IN const HWND& hWnd,IN const BYTE& byMode)
{ 
	if(0 == (m_byParentMode = byMode))
		m_dwMainThreadID = dwThreadID;
	else
		m_hMainWnd = hWnd;
}
/////////////////////////////////////////////////////////////////////////////////////////
////��������:Init
////��������:��ʼ��������ص���Դ
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::Init() 
{
    memset(&m_dcb, 0, sizeof(m_dcb));
    m_dcb.DCBlength = sizeof(m_dcb);
    
    memset(&m_ReadOverlapped, 0, sizeof(m_ReadOverlapped));
    memset(&m_WriteOverlapped, 0, sizeof(m_WriteOverlapped));
    
    m_ReadOverlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    ASSERT(m_ReadOverlapped.hEvent != NULL); 
    
    m_WriteOverlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    ASSERT(m_WriteOverlapped.hEvent != NULL);
	
    memset(&m_WaitOverlapped, 0, sizeof(m_WaitOverlapped));
    m_WaitOverlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    ASSERT(m_WaitOverlapped.hEvent != NULL); 

    ::InitializeCriticalSection(&m_Mutex);
} 

/////////////////////////////////////////////////////////////////////////////////////////
////��������:Destroy
////��������:���������Դ
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::Destroy()
{
    if(m_ReadOverlapped.hEvent != NULL)
        CloseHandle(m_ReadOverlapped.hEvent);
    
    if(m_WriteOverlapped.hEvent != NULL)
        CloseHandle(m_WriteOverlapped.hEvent);
    
    if(m_WaitOverlapped.hEvent != NULL)
        CloseHandle(m_WaitOverlapped.hEvent);

    ::DeleteCriticalSection(&m_Mutex);
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:Close
////��������:�رմ��ڲ���
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:ͬʱ�������Ӵ��ڵ��߳�
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::Close()
{
    if(IsOpen())  
    {
        PurgeComm(m_hCom, PURGE_TXABORT | PURGE_TXCLEAR); 
        
        EndThread();
        ::CloseHandle(m_hCom);
        
        m_hCom = INVALID_HANDLE_VALUE;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:Open
////��������:�򿪴��ڣ��ṩ��ϸ���ò���
////�������:IN const BYTE& byPort		--	�򿪵Ķ˿ں�
////		:IN const DWORD& dwBaudRate	--	ͨѶ������
////		:IN const DWORD& dwByteSize	--	����λ��С
////		:IN const DWORD& dwParity	--	��żУ��
////		:IN const DWORD& dwStopBits	--	ֹͣλ
////�������:TRUE	-- �����ɹ�
////		:FALSE	-- ����ʧ��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSerialCom::Open(IN const BYTE& byPort,IN const DWORD& dwBaudRate,IN const DWORD& dwByteSize,IN const DWORD& dwParity,IN const DWORD& dwStopBits)
{
    if(byPort < 1 || byPort > 256)
        return FALSE;
    
	m_byPort = byPort;
	TCHAR szComDesStr[32] = {0};
	TCHAR szTemp[3] = {0};
    
    _tcscpy_s(szComDesStr,_countof(szComDesStr),_T("\\\\.\\COM")); 
    _itot_s(m_byPort, szTemp, 10);
    strcat_s(szComDesStr,_countof(szComDesStr),szTemp);
    
    if(!OpenCommPort(szComDesStr))
        return FALSE;
       
    return SetState(dwBaudRate,dwByteSize,dwParity,dwStopBits);
}
/////////////////////////////////////////////////////////////////////////////////////////
////��������:OpenCommPort
////��������:�򿪴��ڣ��ṩ��ϸ���ò���
////�������:IN const TCHAR* szComDesStr		--	�򿪵Ķ˿ں�
////�������:TRUE	-- �����ɹ�
////		:FALSE	-- ����ʧ��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSerialCom::OpenCommPort(IN const TCHAR* szComDesStr)
{
    if(IsOpen())
        Close();

    if(NULL != szComDesStr)
	{
		m_hCom = ::CreateFile(
			szComDesStr,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | m_dwIOMode, 
			NULL
				);
    
		if(!(IsOpen() && BeginThread()))
		{
			Close();  ///�����߳�ʧ��
			return FALSE;
		}
		//Ĭ�����������������Ϊ2K�ֽ�
		if(!::SetupComm(m_hCom, 2048, 2048))
			return FALSE; 

		if(!::GetCommTimeouts(m_hCom, &m_OverTime))
			return FALSE;
		m_OverTime.ReadIntervalTimeout = 0;
		m_OverTime.ReadTotalTimeoutMultiplier = 1;
		m_OverTime.ReadTotalTimeoutConstant = 1000;
		m_OverTime.WriteTotalTimeoutMultiplier = 1;
		m_OverTime.WriteTotalTimeoutConstant = 1000;

		if(!::SetCommTimeouts(m_hCom, &m_OverTime))
			return FALSE; 
    
		if(!::PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ))
			return FALSE; 
	}
    return IsOpen();
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:SetState
////��������:���ô��ڵľ���ִ�к���
////�������:IN const DWORD& dwBaudRate	--	ͨѶ������
////		:IN const DWORD& dwByteSize	--	����λ��С
////		:IN const DWORD& dwParity	--	��żУ��
////		:IN const DWORD& dwStopBits	--	ֹͣλ
////�������:TRUE	-- �����ɹ�
////		:FALSE	-- ����ʧ��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSerialCom::SetState(IN const DWORD& dwBaudRate,IN const DWORD& dwByteSize,IN const DWORD& dwParity,IN const DWORD& dwStopBits)
{
    if(IsOpen())
    {
        if(::GetCommState(m_hCom, &m_dcb) != TRUE)
            return FALSE;
        m_dcb.BaudRate = dwBaudRate;
        m_dcb.ByteSize = (BYTE)dwByteSize;
        m_dcb.Parity   = (BYTE)dwParity;
        m_dcb.StopBits = (BYTE)dwStopBits;

		// 1017�汾�رմ������������
		m_dcb.fInX = FALSE;
		m_dcb.fOutX = FALSE;

        return ::SetCommState(m_hCom, &m_dcb);
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:BeginThread
////��������:�������Ӵ��ڵ��߳�
////�������:��
////�������:TRUE	-- �����ɹ�
////		:FALSE	-- ����ʧ��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSerialCom::BeginThread() 
{
    if(!IsThreadRunning()) 
    {
        m_bRunFlag = TRUE;
        m_hThreadHandle = NULL;
        
        DWORD dwThreadID;
        m_hThreadHandle = ::CreateThread(NULL, 0, SerialThreadFunc, this, 0, &dwThreadID); 
        return (m_hThreadHandle != NULL); 
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:EndThread
////��������:�������Ӵ��ڵ��߳�
////�������:IN const DWORD& dwWaitTime	-- �����ȴ�ʱ�䣬��ʱ��ֱ��ǿ�н����߳�
////�������:TRUE	-- �����ɹ�
////		:FALSE	-- ����ʧ��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline BOOL CSerialCom::EndThread(IN const DWORD& dwWaitTime)
{
    if(IsThreadRunning()) 
    {
        m_bRunFlag = FALSE;
        ::SetCommMask(m_hCom, 0);
        ::SetEvent(m_WaitOverlapped.hEvent);
        if(::WaitForSingleObject(m_hThreadHandle, dwWaitTime) != WAIT_OBJECT_0)
            if(!::TerminateThread(m_hThreadHandle, 0))
                return FALSE;
            
            ::CloseHandle(m_hThreadHandle);
            ::ResetEvent(m_WaitOverlapped.hEvent);
            
            m_hThreadHandle = NULL;
            
            return TRUE;
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:Read
////��������:�Ӵ����ж�ȡ���ݣ��첽��ȡ
////�������:IN OUT LPVOID lpData		--	������ݵĻ�����ָ��
////		:IN OUT DWORD& dwRLen		--	��������������	
////		:const BOOL& bOnlyGetBuffer	--	�Ƿ�ֻ��ȡ���ڻ������ڵ�����
////�������:DWORD -- ���ض�ȡ�������ݳ���
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::Read(IN OUT LPVOID lpData,IN OUT DWORD& dwRLen,const BOOL& bOnlyGetBuffer)
{
    if(!IsOpen())
        return 0;
    
    COMSTAT  State;
    DWORD dwError;
    
    if(::ClearCommError(m_hCom, &dwError, &State) && dwError > 0)
    {
        ::PurgeComm(m_hCom, PURGE_RXABORT | PURGE_RXCLEAR);
        return 0;
    }
    
    if(bOnlyGetBuffer)
        dwRLen = dwRLen > State.cbInQue ? State.cbInQue : dwRLen;

    DWORD dwReadNum = 0;
    if(!::ReadFile(m_hCom, lpData, dwRLen, &dwReadNum, &m_ReadOverlapped)) 
    {
        if(::GetLastError() == ERROR_IO_PENDING) 
        {
            while(!::GetOverlappedResult(m_hCom, &m_ReadOverlapped, &dwReadNum, TRUE))     
            {
                if(::GetLastError() != ERROR_IO_INCOMPLETE)
                {
                    ::ClearCommError(m_hCom, &dwError, &State);
                    break;
                }
				Sleep(5);
            }
        }
        else
            ::ClearCommError(m_hCom, &dwError, &State);
    }
    
    return dwReadNum;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:ReadSync
////��������:�Ӵ����ж�ȡ���ݣ�ͬ����ȡ
////�������:IN OUT LPVOID lpData		--	������ݵĻ�����ָ��
////		:IN OUT DWORD& dwRLen		--	��������������	
////�������:DWORD -- ���ض�ȡ�������ݳ���
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::ReadSync(IN LPVOID lpData,IN const DWORD& dwRLen)
{
    if(!IsOpen())
        return 0;
    
    DWORD dwError;
    if(::ClearCommError(m_hCom, &dwError, NULL) && dwError > 0)
    {
        ::PurgeComm(m_hCom, PURGE_RXABORT | PURGE_RXCLEAR);
        return 0;
    }

    DWORD dwReadNum = 0;
    ::ReadFile(m_hCom,lpData,dwRLen, &dwReadNum, NULL);

    return dwReadNum;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:Write
////��������:��������д�����ݣ��첽д��
////�������:IN OUT LPVOID lpData		--	��Ҫд�봮�ڵĻ�������
////		:IN OUT DWORD& dwWLen		--	��Ҫд�봮�ڵĻ������ݳ���	
////�������:DWORD -- ���ض�ȡ�������ݳ���
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::Write(IN const LPVOID lpData,IN const DWORD& dwWLen)
{
    if(!IsOpen())
        return 0;
    
    DWORD dwError;
    
    if(::ClearCommError(m_hCom, &dwError, NULL) && dwError > 0) 
        ::PurgeComm(m_hCom, PURGE_TXABORT | PURGE_TXCLEAR); 
    
    DWORD dwWriteNum = 0;
    
    if(!::WriteFile(m_hCom, lpData, dwWLen, &dwWriteNum, &m_WriteOverlapped))
	{
        if(::GetLastError() != ERROR_IO_PENDING)
            dwWriteNum = 0;
	}
        
	return dwWriteNum;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:Write
////��������:��������д�����ݣ��첽д��
////�������:IN char* szData			--	��Ҫд�봮�ڵĻ�������
////		:IN const DWORD& dwWLen		--	��Ҫд�봮�ڵĻ������ݳ���
////		:IN const char* szFormat	--	��ʽ���ַ���	
////�������:DWORD -- ���ض�ȡ�������ݳ���
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�ɱ��������
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::Write(IN char* szData,IN const DWORD& dwWLen,IN const char* szFormat, ...)
{
    if(!IsOpen())
        return 0;
    
    va_list va;
    va_start(va, szFormat);
    _vsnprintf_s(szData, dwWLen,_tcslen(szFormat), szFormat, va);
    va_end(va);
    
    return Write(szData);
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:WriteSync
////��������:��������д�����ݣ�ͬ��д��
////�������:IN char* szData			--	��Ҫд�봮�ڵĻ�������
////		:IN const DWORD& dwWLen		--	��Ҫд�봮�ڵĻ������ݳ���
////�������:DWORD -- ���ض�ȡ�������ݳ���
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�ɱ��������
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::WriteSync(IN const LPVOID lpData,IN const DWORD& dwWLen)
{
    if(!IsOpen())
        return 0;
    
    DWORD dwError;
    if(::ClearCommError(m_hCom, &dwError, NULL) && dwError > 0) 
        ::PurgeComm(m_hCom, PURGE_TXABORT | PURGE_TXCLEAR); 
    DWORD dwErrorCode = 0;
    DWORD dwWriteNum = 0;
    if(!::WriteFile(m_hCom, lpData,dwWLen, &dwWriteNum, NULL))
		dwErrorCode = ::GetLastError();

    return dwWriteNum;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:ThreadFunc
////��������:���Ӵ��ڵ��̺߳���
////�������:��
////�������:DWORD -- 
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�ɱ��������
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::ThreadFunc()
{
    if(!::SetCommMask(m_hCom, m_dwMaskEvent))
    {
		TCHAR szBuffer[256] = {0};
   //     _sntprintf(szBuffer, 255, _T("%s(%d) : COM%d ����WINODWS API���� SetCommMask(%x, %x) ʧ��, �����̹߳����쳣! ������� = %d"),
   //        __FILE__, __LINE__, m_byPort, m_hCom, m_dwMaskEvent, GetLastError());
   //     MessageBox(NULL, szBuffer, _T("�����������ʾ"), MB_OK);
        return 1;
    }
    
    COMSTAT State;
    DWORD dwError;
    
    for(DWORD dwLength, dwMask = 0; m_bRunFlag && IsOpen(); dwMask = 0)
    {
        if(!::WaitCommEvent(m_hCom, &dwMask, &m_WaitOverlapped))
        {
            if(::GetLastError() == ERROR_IO_PENDING)////// asynchronous
                ::GetOverlappedResult(m_hCom, &m_WaitOverlapped, &dwLength, TRUE);
            else
                continue;
        }
        
        if(0 == dwMask)
            continue;
        
        switch(dwMask)
        {
        case EV_RXCHAR :
            ::ClearCommError(m_hCom, &dwError, &State);
            if(State.cbInQue >= m_dwNotifyNum)
                OnMsgReceive();
            break;
            
        case EV_TXEMPTY :
            OnMsgTXEmpty();
            break;
            
        case EV_CTS :
            OnMsgCTS();
            break;
            
        case EV_DSR :
            OnMsgDSR();
            break;
            
        case EV_RING :
            OnMsgRing();
            break;
            
        case EV_RLSD :
            OnMsgRLSD();
            break;
            
        case EV_BREAK:
            OnMsgBreak();
            break;
            
        case EV_ERR :
            OnMsgError();
            break;
		default:
			break;            
        }///case
    }///for
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgReceive
////��������:���ڼ����߳̽��յ�����
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����ʱ�������ϼ������߷������ݻ�ȡ֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgReceive()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_RECEIVE, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_RECEIVE, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgDSR
////��������:����״̬����
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgDSR()
{
	DWORD Status;
	if(GetCommModemStatus(m_hCom, &Status))
	{
		if(0 == m_byParentMode && 0 != m_dwMainThreadID)
			::PostThreadMessage(m_dwMainThreadID,ON_COM_DSR, WPARAM(m_byPort), LPARAM(0));	
		else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
			::PostMessage(m_hMainWnd, ON_COM_DSR, WPARAM(m_byPort), LPARAM(0));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgCTS
////��������:����״̬����
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgCTS()
{
	DWORD Status;
	if(GetCommModemStatus(m_hCom, &Status))
	{
		if(0 == m_byParentMode && 0 != m_dwMainThreadID)
			::PostThreadMessage(m_dwMainThreadID,ON_COM_CTS, WPARAM(m_byPort), LPARAM(0));	
		else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
			::PostMessage(m_hMainWnd, ON_COM_CTS, WPARAM(m_byPort), LPARAM(0));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgBreak
////��������:����״̬���ӣ����ڱ��жϣ�
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgBreak()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_BREAK, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_BREAK, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgTXEmpty
////��������:����״̬���ӣ����ͻ����������Ѿ�������ϣ�
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgTXEmpty()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_TXEMPTY, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_TXEMPTY, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgError
////��������:����״̬���ӣ����ڷ�������
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgError()
{
    DWORD dwError;
    ::ClearCommError(m_hCom, &dwError, NULL);

	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_ERROR, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_ERROR, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgRing
////��������:����״̬���ӣ����������壩
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgRing()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_RING, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_RING, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////��������:OnMsgRLSD
////��������:����״̬����
////�������:��
////�������:��
////��дʱ��:2009-07-10
////��д��Ա:ROCY
////�޸�ʱ��:
////�޸���Ա:
////��ע˵��:�����յ�����Ϣʱ�������ϼ������߷���״̬֪ͨ
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgRLSD()
{
    if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_RLSD, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_RLSD, WPARAM(m_byPort), LPARAM(0));
}

#endif // _SERIALCOM_H
