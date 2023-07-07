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
	//判断是否打开了串口
    inline BOOL	IsOpen(){return m_hCom != INVALID_HANDLE_VALUE;} 
	//判断串口监视线程是否在运行
	inline BOOL IsThreadRunning(){return m_hThreadHandle != NULL;}
	//打开串口，默认参数为：9600,8,N,1
	inline BOOL Open(IN const BYTE& byPort){return Open(byPort, 9600);}
	//打开串口，提供详细参数设置
	inline BOOL Open(IN const BYTE& byPort,IN const DWORD& dwBaudRate,IN const DWORD& dwByteSize = 8,IN const DWORD& dwParity = NOPARITY,IN const DWORD& dwStopBits = ONESTOPBIT);
	//设置串口参数
	inline BOOL SetDTR(IN const BOOL& bOnOrOff){return IsOpen() ? EscapeCommFunction(m_hCom, bOnOrOff ? SETDTR : CLRDTR) != 0 : FALSE;}
	inline BOOL SetRTS(IN const BOOL& bOnOrOff){return IsOpen() ? EscapeCommFunction(m_hCom, bOnOrOff ? SETRTS : CLRRTS) != 0 : FALSE;}
	inline BOOL SetState(IN DCB *pDcb = NULL){ASSERT(pDcb);if(NULL != pDcb) return NULL != memcpy(&m_dcb,pDcb,sizeof(DCB));}
	inline BOOL	SetState(IN const DWORD& dwBaudRate,IN const DWORD& dwByteSize = 8,IN const DWORD& dwParity = NOPARITY,IN const DWORD& dwStopBits = ONESTOPBIT);
	inline BOOL SetTimeouts(IN const LPCOMMTIMEOUTS& OverTime){return IsOpen() ? ::SetCommTimeouts(m_hCom, OverTime) == TRUE : FALSE;}
	inline BOOL SetBufferSize(IN const DWORD& dwInputSize,IN const DWORD& dwOutputSize){return IsOpen() ? ::SetupComm(m_hCom, dwInputSize, dwOutputSize) == TRUE : FALSE;}
	inline void SetNotifyNum(IN const DWORD& dwReadNum){m_dwNotifyNum = dwReadNum;}
	//开始串口监视线程
	inline BOOL BeginThread();
	//结束串口监视线程
	inline BOOL EndThread(IN const DWORD& dwWaitTime = 300);
	//挂起串口监视线程
	inline BOOL SuspendThread(){ return IsThreadRunning() ? 0xFFFFFFFF != ::SuspendThread(m_hThreadHandle): FALSE;}
	//重启串口监视线程
	inline BOOL ResumeThread() { return IsThreadRunning() ? 0xFFFFFFFF != ::ResumeThread(m_hThreadHandle): FALSE;}
	
	inline void SetMain(IN const DWORD& dwThreadID,IN const HWND& hWnd = NULL,IN const BYTE& byMode = 0);
	//获得串口句柄
	inline HANDLE GetHandle(){ return m_hCom; }
	//获得串口监视线程ID
	inline HANDLE GetThread(){ return m_hThreadHandle; }
	//设置线程监视事件ID
	inline void SetMaskEvent(IN const DWORD& dwEvent = DEFAULT_COM_MASK_EVENT);
	//清除串口输入缓冲区内容
	inline void ClearInputBuffer(){if(IsOpen())  ::PurgeComm(m_hCom, PURGE_RXABORT | PURGE_RXCLEAR );}
	//清除串口输出缓冲区内容
	inline void ClearOutputBuffer(){if(IsOpen()) ::PurgeComm(m_hCom, PURGE_TXABORT | PURGE_TXCLEAR );}
	//锁住串口资源
	inline void Lock(){::EnterCriticalSection(&m_Mutex);}
	//解锁串口资源
	inline void Unlock(){::LeaveCriticalSection(&m_Mutex);}
	//关闭串口
	inline void Close();
	//获取串口输入缓冲区大小
	inline const int  GetInputSize();
	//获取串口端号
	inline const int  GetCOMPort(){return m_byPort;}
	//获取串口详细参数结构体
	inline const DCB& GetState(){return m_dcb;}
	//获取串口超时设置参数
	inline LPCOMMTIMEOUTS GetTimeouts(void);
	//异步读取数据
	inline DWORD Read(IN OUT LPVOID lpData,IN OUT DWORD& dwRLen,const BOOL& bOnlyGetBuffer = TRUE);
	//同步读取数据
	inline DWORD ReadSync(IN LPVOID lpData,IN const DWORD& dwRLen);
	//异步写数据
	inline DWORD Write(IN const LPVOID lpData,IN const DWORD& dwWLen);
	inline DWORD Write(IN const char *szData){if(NULL != szData && ((DWORD)strlen(szData)>0)) return Write((void *)szData,(DWORD)strlen(szData));}
	inline DWORD Write(IN char *szData,IN const DWORD& dwWLen,IN const char * szFormat, ...);
	//同步写数据
	inline DWORD WriteSync(IN const LPVOID lpData,IN const DWORD& dwWLen);
	
protected:
	//串口初始化
    inline virtual void Init();                
	//在串口关闭前，进行相应的资源清理工作
    inline virtual void Destroy();
private:
	//打开串口的内部实现函数
	inline BOOL OpenCommPort(IN const TCHAR* szComDesStr);
	inline virtual DWORD ThreadFunc();
	//监视串口线程主体函数
	static DWORD WINAPI SerialThreadFunc(IN LPVOID lpPara){return ( (CSerialCom *)lpPara )->ThreadFunc();}
	//消息通知函数
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
	volatile BYTE		m_byPort;			//串口号
	volatile HANDLE		m_hCom;				//串口句柄
    volatile DWORD		m_dwNotifyNum;      //接受多少字节时发送通知消息
    volatile DWORD		m_dwMaskEvent;      //监视的事件
    volatile BOOL		m_bRunFlag;         //线程运行循环标志
	DWORD				m_dwMainThreadID;   //调用线程ID
	HWND				m_hMainWnd;			//调用窗口句柄
	BYTE				m_byParentMode;		//调用者模式(0,线程;1,窗口)
	HANDLE				m_hThreadHandle;	//串口线程句柄
	DWORD				m_dwIOMode;			//串口使用模式(重叠IO)
	DCB					m_dcb;				//串口参数结构
	OVERLAPPED			m_ReadOverlapped;   //读取重叠I/O
	OVERLAPPED			m_WriteOverlapped;  //写入重叠I/O
	OVERLAPPED			m_WaitOverlapped;	//等待重叠IO
	COMMTIMEOUTS		m_OverTime;			//串口超时设置结构
	CRITICAL_SECTION	m_Mutex;			//互斥类
};

inline void CSerialCom::SetMain(IN const DWORD& dwThreadID,IN const HWND& hWnd,IN const BYTE& byMode)
{ 
	if(0 == (m_byParentMode = byMode))
		m_dwMainThreadID = dwThreadID;
	else
		m_hMainWnd = hWnd;
}
/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:Init
////函数功能:初始化串口相关的资源
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:Destroy
////函数功能:销毁相关资源
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:Close
////函数功能:关闭串口操作
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:同时结束监视串口的线程
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
////函数名称:Open
////函数功能:打开串口，提供详细设置参数
////输入参数:IN const BYTE& byPort		--	打开的端口号
////		:IN const DWORD& dwBaudRate	--	通讯波特率
////		:IN const DWORD& dwByteSize	--	数据位大小
////		:IN const DWORD& dwParity	--	奇偶校验
////		:IN const DWORD& dwStopBits	--	停止位
////输出参数:TRUE	-- 操作成功
////		:FALSE	-- 操作失败
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:OpenCommPort
////函数功能:打开串口，提供详细设置参数
////输入参数:IN const TCHAR* szComDesStr		--	打开的端口号
////输出参数:TRUE	-- 操作成功
////		:FALSE	-- 操作失败
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
			Close();  ///创建线程失败
			return FALSE;
		}
		//默认输入输出缓冲区均为2K字节
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
////函数名称:SetState
////函数功能:设置串口的具体执行函数
////输入参数:IN const DWORD& dwBaudRate	--	通讯波特率
////		:IN const DWORD& dwByteSize	--	数据位大小
////		:IN const DWORD& dwParity	--	奇偶校验
////		:IN const DWORD& dwStopBits	--	停止位
////输出参数:TRUE	-- 操作成功
////		:FALSE	-- 操作失败
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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

		// 1017版本关闭串口软件流控制
		m_dcb.fInX = FALSE;
		m_dcb.fOutX = FALSE;

        return ::SetCommState(m_hCom, &m_dcb);
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:BeginThread
////函数功能:启动监视串口的线程
////输入参数:无
////输出参数:TRUE	-- 操作成功
////		:FALSE	-- 操作失败
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:EndThread
////函数功能:结束监视串口的线程
////输入参数:IN const DWORD& dwWaitTime	-- 结束等待时间，超时则直接强行结束线程
////输出参数:TRUE	-- 操作成功
////		:FALSE	-- 操作失败
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:Read
////函数功能:从串口中读取数据，异步读取
////输入参数:IN OUT LPVOID lpData		--	存放数据的缓冲区指针
////		:IN OUT DWORD& dwRLen		--	给定缓冲区长度	
////		:const BOOL& bOnlyGetBuffer	--	是否只读取串口缓冲区内的数据
////输出参数:DWORD -- 返回读取到的数据长度
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:ReadSync
////函数功能:从串口中读取数据，同步读取
////输入参数:IN OUT LPVOID lpData		--	存放数据的缓冲区指针
////		:IN OUT DWORD& dwRLen		--	给定缓冲区长度	
////输出参数:DWORD -- 返回读取到的数据长度
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:Write
////函数功能:往串口中写入数据，异步写入
////输入参数:IN OUT LPVOID lpData		--	需要写入串口的缓冲数据
////		:IN OUT DWORD& dwWLen		--	需要写入串口的缓冲数据长度	
////输出参数:DWORD -- 返回读取到的数据长度
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:
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
////函数名称:Write
////函数功能:往串口中写入数据，异步写入
////输入参数:IN char* szData			--	需要写入串口的缓冲数据
////		:IN const DWORD& dwWLen		--	需要写入串口的缓冲数据长度
////		:IN const char* szFormat	--	格式化字符串	
////输出参数:DWORD -- 返回读取到的数据长度
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:可变参数函数
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
////函数名称:WriteSync
////函数功能:往串口中写入数据，同步写入
////输入参数:IN char* szData			--	需要写入串口的缓冲数据
////		:IN const DWORD& dwWLen		--	需要写入串口的缓冲数据长度
////输出参数:DWORD -- 返回读取到的数据长度
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:可变参数函数
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
////函数名称:ThreadFunc
////函数功能:监视串口的线程函数
////输入参数:无
////输出参数:DWORD -- 
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:可变参数函数
//////////////////////////////////////////////////////////////////////////////////////////
inline DWORD CSerialCom::ThreadFunc()
{
    if(!::SetCommMask(m_hCom, m_dwMaskEvent))
    {
		TCHAR szBuffer[256] = {0};
   //     _sntprintf(szBuffer, 255, _T("%s(%d) : COM%d 调用WINODWS API函数 SetCommMask(%x, %x) 失败, 辅助线程工作异常! 错误代码 = %d"),
   //        __FILE__, __LINE__, m_byPort, m_hCom, m_dwMaskEvent, GetLastError());
   //     MessageBox(NULL, szBuffer, _T("串口类错误提示"), MB_OK);
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
////函数名称:OnMsgReceive
////函数功能:串口监视线程接收到数据
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到数据时，即向上级调用者发送数据获取通知
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgReceive()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_RECEIVE, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_RECEIVE, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:OnMsgDSR
////函数功能:串口状态监视
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
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
////函数名称:OnMsgCTS
////函数功能:串口状态监视
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
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
////函数名称:OnMsgBreak
////函数功能:串口状态监视（串口被中断）
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgBreak()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_BREAK, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_BREAK, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:OnMsgTXEmpty
////函数功能:串口状态监视（发送缓冲区数据已经发送完毕）
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgTXEmpty()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_TXEMPTY, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_TXEMPTY, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:OnMsgError
////函数功能:串口状态监视（串口发生错误）
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
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
////函数名称:OnMsgRing
////函数功能:串口状态监视（串口有振铃）
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgRing()
{
	if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_RING, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_RING, WPARAM(m_byPort), LPARAM(0));
}

/////////////////////////////////////////////////////////////////////////////////////////
////函数名称:OnMsgRLSD
////函数功能:串口状态监视
////输入参数:无
////输出参数:无
////编写时间:2009-07-10
////编写人员:ROCY
////修改时间:
////修改人员:
////备注说明:当接收到此消息时，即向上级调用者发送状态通知
//////////////////////////////////////////////////////////////////////////////////////////
inline void CSerialCom::OnMsgRLSD()
{
    if(0 == m_byParentMode && 0 != m_dwMainThreadID)
		::PostThreadMessage(m_dwMainThreadID,ON_COM_RLSD, WPARAM(m_byPort), LPARAM(0));	
	else if(1 == m_byParentMode && ::IsWindow(m_hMainWnd))
        ::PostMessage(m_hMainWnd, ON_COM_RLSD, WPARAM(m_byPort), LPARAM(0));
}

#endif // _SERIALCOM_H
