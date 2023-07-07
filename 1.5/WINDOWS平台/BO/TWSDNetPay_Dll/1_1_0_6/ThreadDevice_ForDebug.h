/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：ThreadDevice.h
 * 摘    要：硬件控制线程，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年7月23日
 */
#pragma once
#include "ThreadBase.h"
#include "HTSDDeviceDll.h"
#include "NFCProtocol.h"
#include "DVXMLUnipay.h"

// CThreadDevice_ForDebug

class CThreadDevice_ForDebug : public CThreadBase
{
	DECLARE_DYNCREATE(CThreadDevice_ForDebug)

public://protected --> public 由于未知的原因，ExitInstance在线程退出的时候并没有被执行，因此需要手动清理资源。
	CThreadDevice_ForDebug();           // protected constructor used by dynamic creation
	virtual ~CThreadDevice_ForDebug();

public:
	CString debug_str;
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

};


