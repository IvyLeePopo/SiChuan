// CatchDump.cpp: implementation of the CCatchDump class.
//
//////////////////////////////////////////////////////////////////////

#include "CatchDump.h"
#include  <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//静态变量初始化
BOOL	CCatchDump::m_bDebugDumpShowAlert	= TRUE;
CString CCatchDump::m_strDebugDumpTip		= _T("");
CString CCatchDump::m_strDebugDumpFilePath	= _T("");
int		CCatchDump::m_iDumpDataType			= 0;

CCatchDump::CCatchDump(IN const int& iCompilerType)
	:m_iDebugDumpCompilerType(iCompilerType)
{

}

CCatchDump::~CCatchDump()
{

}

// ========================================================================================================================
// 
// 函数名称: CCatchDump::InitCatchExecption()
// 
// 函数功能: 初始化程序产生DUMP文件的环境
// 输入参数: IN const CString& strDumpFilePath	--	生成的Dump文件路径
//         : IN const BOOL& bShowAlert			--	在程序发生崩溃后，是否需要显示提示信息(TRUE,提示;FALSE,不提示)
//         : IN const CString& strTip			--	在程序发生崩溃后，显示的提示信息内容(为空则有默认的提示信息)
//		   : IN const int& iDumpDataType		--	生成的Dump文件的数据内容
//													0,默认最小数据格式,仅仅包含最小数据内容,不可查看堆内容
//												    1,指定用户数据收集模式,可以查看全局变量值
//												    2,指定全内存数据模式,将发生异常的使用内存信息保存在dump文件,占用空间较大
// 返回参数: void
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2015-1-23 13:55:00]
// 备注说明: 对外接口,该函数必须在APP类靠前的地方调用
// 
// ======================================================================================================================== 
void CCatchDump::InitCatchExecption(IN const CString& strDumpFilePath,IN const BOOL& bShowAlert,IN const CString& strTip,IN const int& iDumpDataType)
{
	m_bDebugDumpShowAlert	= bShowAlert;
	m_strDebugDumpFilePath	= strDumpFilePath;
	m_strDebugDumpTip		= strTip;
	m_iDumpDataType			= iDumpDataType;
	
	::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	
	//对于高于6.0版本的IDE,需要调用下述接口禁止系统CRT自带的异常处理参数设置
	if(0 != m_iDebugDumpCompilerType)
	{
		DisableSetUnhandledExceptionFilter();
	}
}

// ========================================================================================================================
// 
// 函数名称: BOOL CCatchDump::CreateDumpFile()
// 
// 函数功能: 创建Dump文件,路径为参数指定
// 输入参数: IN const CString& strDumpFilePath		--	指定的DUMP文件路径
//		   : IN OUT EXCEPTION_POINTERS *pException	--	产生的异常基本类指针
// 返回参数: const BOOL
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2015-1-23 10:03:51]
// 备注说明: 内部辅助函数
// 
// ======================================================================================================================== 
const BOOL CCatchDump::CreateDumpFile(IN const CString& strDumpFilePath,IN OUT EXCEPTION_POINTERS *pException)
{
	BOOL bReturn = FALSE;
	//已经存在该文件,则尝试进行删除
	if(::PathFileExists(strDumpFilePath))
	{
		::SetFileAttributes(strDumpFilePath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(strDumpFilePath);
	}
	//获取指定文件目录
	CString strDir = strDumpFilePath.Left(strDumpFilePath.ReverseFind('\\'));
	if(CreateDumpFileDir(strDir))
	{
		//试图尝试创建DUMP文件
		HANDLE hDumpFile = CreateFile(strDumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		if(INVALID_HANDLE_VALUE != hDumpFile)
		{
			//设置产生Dump文件的信息  
			MINIDUMP_EXCEPTION_INFORMATION dumpInfo;

			dumpInfo.ExceptionPointers	= pException;
			dumpInfo.ThreadId			= GetCurrentThreadId();
			dumpInfo.ClientPointers		= TRUE;  
  
			// 写入Dump文件内容
			// 函数详细定义在URL https://msdn.microsoft.com/en-us/library/ms680519(v=vs.85).aspx
			// 关键 第4个参数 MINIDUMP_TYPE DumpType
			// MiniDumpNormal							-- 默认标记,其值为0
			// MiniDumpWithFullMemory					-- 全内存标识，该模式下dmp文件将会很大，一般为几个MB
			// MiniDumpWithPrivateReadWriteMemory		-- 包括所有可读和可写的私有内存页的内容，可以察看栈、堆甚至TLS的数据，PEB和TEB也包括在里面。
			// MiniDumpWithIndirectlyReferencedMemory	-- 查线程栈内存中的每一个指针。这些指针可能指向线程地址空间的其他可读内存页。一旦发现这样的指针，程序会读取指针附近1024字节的内容存到minidump中（指针前的256字节和指针后的768字节）
			// MiniDumpWithDataSegs						-- 包括进程装载的所有可执行模块的可写数据段,可以查看全局变量，该模式下的dump文件比全内存标记下的文件要小许多
			// MiniDumpWithCodeSegs						-- 包括所有进程装载的可执行模块的代码段
			// MiniDumpWithHandleData					-- 包括故障时刻进程故障表里面的所有句柄
			// MiniDumpWithThreadInfo					-- 收集进程中线程的附加信息[线程时间 (创建时间,执行用户代码和内核代码的时间),入口地址,相关性]
			// MiniDumpWithProcessThreadData			-- 包含PEB和TEB占据的内存页
			// MiniDumpWithFullMemoryInfo				-- 包含进程虚拟内存布局的完整信息
			// MiniDumpWithoutOptionalData				-- 用来减小保存在dump中的内存的内容。当指定这个标志是，只有MiniDumpNormal指定的内存会被保存。
			// MiniDumpFilterMemory						-- 栈内存的内容会在保存之前进行过滤,只有重建调用栈需要的数据才会被保留,其他数据会被写成0
			// MiniDumpFilterModulePaths				-- 是否包括模块路径
			// MiniDumpScanMemory						-- 节约minidump占用的空间,把调试不需要的可执行模块去掉
			
			MINIDUMP_TYPE enMinidumpType;
			switch(m_iDumpDataType)
			{
			case 0:
				enMinidumpType = MiniDumpNormal;
				break;
			case 1:
				enMinidumpType = MiniDumpWithDataSegs;
				break;
			case 2:
				enMinidumpType = MiniDumpWithFullMemory;
				break;
			}
			//产生dump文件
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, enMinidumpType, &dumpInfo, NULL, NULL);  
			//关闭句柄
			CloseHandle(hDumpFile);
			//函数执行返回成功
			bReturn = TRUE;
		}
	}
	return bReturn;
}

// ========================================================================================================================
// 
// 函数名称: CALLBACK CCatchDump::ApplicationCrashHandler()
// 
// 函数功能: 处理Unhandled Exception的回调函数
// 输入参数: EXCEPTION_POINTERS *pException
// 返回参数: LONG
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2015-1-23 10:05:00]
// 备注说明: 内部辅助函数
// 
// ======================================================================================================================== 
long CALLBACK CCatchDump::ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	//创建Dump文件
	if(m_strDebugDumpFilePath.IsEmpty())
		m_strDebugDumpFilePath.Format(_T("D:\\UnHandleExecption.dmp"));
	CreateDumpFile(m_strDebugDumpFilePath,pException);
	//进行一些业务处理

	//业务处理完毕,弹出一个错误对话框并退出程序  
	if(m_bDebugDumpShowAlert)
	{  
		if(m_strDebugDumpTip.IsEmpty())
			m_strDebugDumpTip.Format(_T("*** 软件检测到系统环境产生异常,无法正常提供服务，程序即将退出,请点击【确定】后重新启动软件。***"));
		FatalAppExit(0,m_strDebugDumpTip);  
	}


	//记录日志
	//remove by bing WRITE_LOG(FmtStr(_T("系统异常：发生未捕捉到的错误(错误代码:%d)，软件异常中止")),pException->ExceptionRecord->ExceptionCode);
	//注销系统
	//remove by bing ReStartComputerEx(NULL,EWX_LOGOFF | EWX_FORCE);
	//强制结束进程
//	::ExitProcess(0);
	::ExitProcess(0);

	return EXCEPTION_EXECUTE_HANDLER;
}

// ========================================================================================================================
// 
// 函数名称: CCatchDump::DisableSetUnhandledExceptionFilter()
// 
// 函数功能: 新版本的CRT实现在异常处理中,强制删除所有应用程序先前设置的捕获函数,导致让程序无法产生DUMP文件
//		     拦截CRT调用SetUnhandledExceptionFilter函数，使之无效
// 返回参数: void
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2015-1-23 10:10:15]
// 备注说明: 该函数x86环境下有效
// 
// ======================================================================================================================== 
void CCatchDump::DisableSetUnhandledExceptionFilter()
{
	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),"SetUnhandledExceptionFilter");
	if(NULL != addr)
	{
		BYTE byCode[16] = {0};
		int iSize = 0;

		byCode[iSize++] = 0x33;
		byCode[iSize++] = 0xC0;
		byCode[iSize++] = 0xC2;
		byCode[iSize++] = 0x04;
		byCode[iSize++] = 0x00;

		DWORD dwOldFlag		= 0;
		DWORD dwTempFlag	= 0;
		VirtualProtect(addr, iSize, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, byCode, iSize, NULL);
		VirtualProtect(addr, iSize, dwOldFlag, &dwTempFlag);
	}
}

// ========================================================================================================================
// 
// 函数名称: BOOL CCatchDump::CreateDumpFileDir()
// 
// 函数功能: 根据指定的路径创建目录
// 输入参数: IN const CString& strDirPath	--	指定的目录路径
// 返回参数: const BOOL
// 函数版本: 1.0.0.1
// 编写人员: [ROCY]
// 编写时间: [2015-1-23 15:04:38]
// 备注说明: 内部辅助函数
// 
// ======================================================================================================================== 
const BOOL CCatchDump::CreateDumpFileDir(IN const CString& strDirPath)
{
	BOOL bNetPath	= FALSE;
	CStringArray	aDirList;
	CString strPath(strDirPath);

	if(strPath.Find(_T("\\\\"))==0) 
		bNetPath=TRUE;

	while(::_taccess(strPath,0)!=0)
	{
		if(-1 == strPath.Find('\\')	) 
			return FALSE;
		CString strTemp = strPath.Mid(strPath.ReverseFind('\\'));
		aDirList.InsertAt(0,strTemp);
		strPath = strPath.Left(strPath.ReverseFind('\\'));
		if(		bNetPath 
			&&	strPath.GetLength()<2) 
			return FALSE;
	}

	while(aDirList.GetSize()!=0)
	{
		strPath += aDirList[0];
		aDirList.RemoveAt(0);
		if(!::CreateDirectory(strPath,NULL))
			return FALSE;
	}

	return TRUE;
}