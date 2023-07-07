
#if !defined(AFX_CATCHDUMP_H__INCLUDED_)
#define AFX_CATCHDUMP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************说明********************************************************
		
1.VC6.0环境当中,类生成采用默认构造函数(默认参数)
2.VC2005及其以上版本，类生成采用默认构造函数,构造函数参数必须为1
3.程序在Release版本当中，必须将调试信息增加上(编译器开关，勾选 普通调试信息 生成MAP文件)
4.使用方式 -- 在程序开始阶段,调用接口 InitCatchExecption 即可
5.VS调试现场恢复：选择用相应的IDE打开dmp文件,然后单步进行调试即可看到问题发生的语句所在

注意:a VC6.0 默认支持SHE异常,可以通过 try{} catch(...){} 捕获所有的异常【包括C++异常与CPU异常】
     b VS2005以上的IDE默认不支持捕捉CPU异常，只能捕捉C++异常，需要在C++编译选项中设置:/Eha
	 c WinDbg的在线数据库:    SRV*C:\Symbols*http://msdl.microsoft.com/download/symbols
	   需要在指定符号文件路径中增添

******************************************结束********************************************************/

#include "Shlwapi.h"
#pragma comment(lib,"Shlwapi.lib")  

//产生dump文件必须包含的东西
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

class CCatchDump  
{
public:
	CCatchDump(IN const int& iCompilerType = 0);
	virtual ~CCatchDump();
//Impl
public:
	//初始化生成dump文件的环境
	void InitCatchExecption(IN const CString& strDumpFilePath,IN const BOOL& bShowAlert = TRUE,IN const CString& strTip = _T(""),IN const int& iDumpDataType = 0);
private:
	//创建Dump文件,参数为该文件的绝对路径
	static const BOOL CreateDumpFile(IN const CString& strDumpFilePath,IN OUT EXCEPTION_POINTERS *pException);
	//处理Unhandled Exception的回调函数
	static long CALLBACK ApplicationCrashHandler(EXCEPTION_POINTERS *pException);
	//创建指定路径下的目录
	static const BOOL CreateDumpFileDir(IN const CString& strDirPath);

	//在VC2005版本,需要调用下述接口,禁止系统自动过滤人工设定DUMP参数
	void DisableSetUnhandledExceptionFilter();
//Attributes
private:
	//编译器版本
	//0,默认为VC6版本的IDE
	//1,VS2005以上版本的IDE
	int		m_iDebugDumpCompilerType;

	//软件崩溃后是否提供界面提示
	static BOOL	m_bDebugDumpShowAlert;
	//软件崩溃后的提示信息
	static CString	m_strDebugDumpTip;
	//Dump文件绝对路径
	static CString	m_strDebugDumpFilePath;
	//生成的dump文件数据包含内容
	static int m_iDumpDataType;
};

#endif // !defined(AFX_CATCHDUMP_H__INCLUDED_)
