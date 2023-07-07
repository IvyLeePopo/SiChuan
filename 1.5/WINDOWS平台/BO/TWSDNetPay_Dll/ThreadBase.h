/*
 * Copyright(C) 2015,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadBase.h
 * 摘    要：车道软件内部线程类统一继承接口，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2015年11月11日
 */
#pragma once

#include "BaseDefine.h"

// CThreadBase

class CThreadBase : public CWinThread
{
	DECLARE_DYNCREATE(CThreadBase)

protected:
	CThreadBase();           // protected constructor used by dynamic creation
	virtual ~CThreadBase();
//系统重载函数
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

//接口定义
public:
	//传递车道基本设置参数
	void	SetParam(IN const tagTranstParam* pTranstParam,IN const HWND& hMainWnd); 
	//设置本地配置文件绝对路径
	void	SetLocalConfigFilePath(IN const CString& strFilePath){m_strPathLocalConfigFile = strFilePath;}
	//是否允许向主UI发送消息
	const BOOL IsAllowSendMsgToUI(IN const HWND& hWnd = NULL);
	//锁定操作
	inline void Lock(void)  {::EnterCriticalSection(&m_cs);}
	//解锁操作
	inline void UnLock(void){::LeaveCriticalSection(&m_cs);}
protected:
	DECLARE_MESSAGE_MAP()
//虚函数,必须继承
protected:
	//清除本类申请的资源
	virtual void fnReleaseResource(){}
	//读取本类相关的配置信息(从指定配置文件)
	virtual void fnReadSpecailConfig(){}
//Attributes
public:
	//线程退出标记
	BOOL	m_bExit;
protected:
	//主窗体句柄
	HWND	m_hMainWnd;
	//线程是否正处于工作状态
	BOOL	m_bWorking;
	//最后的一次错误提示
	CString	m_strLastErrorDesc;
	//配置文件绝对路径
	CString m_strPathLocalConfigFile;
	//车道基本配置信息
	tagTranstParam	m_sTranstParam;
	//临界区互锁变量
	CRITICAL_SECTION m_cs;
};


class CScopeLock
{
public:
    CScopeLock(CRITICAL_SECTION* lck)
        : mMutex(lck)
    {
		if(mMutex!=NULL)
		{
			::EnterCriticalSection(mMutex);
		}
        
    }
    ~CScopeLock()
    {
		if(mMutex!=NULL)
		{
			 ::LeaveCriticalSection(mMutex);
		}
       
    }
private:
    CRITICAL_SECTION* mMutex;
private:
    CScopeLock(const CScopeLock&);
    CScopeLock& operator = ( const CScopeLock&);

};



