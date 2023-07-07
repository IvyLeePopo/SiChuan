/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：TableDebitCancel.h
 * 摘    要：撤单数据后续处理类，声明文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月25日
 */

#pragma once
#include "SQLite3Base.h"
#include "DVDebitCancel.h"


class CTableDebitCancel
{
public:
	CTableDebitCancel(void);
	~CTableDebitCancel(void);
//Operations
public:
	//设置数据库绝对路径
	inline void SetDBFilePath(IN const CString& strFilePath){m_strSQLiteDBFilePath = strFilePath;}
	//设置撤单数据缓存时间（单位：分钟）
	inline void SetLastDelayTime(IN const int& iLastDelayTime){m_llLastDelayTime = iLastDelayTime;}
	//保存一条撤单数据
	const BOOL Save(IN const CDVDebitCancel& cDebitCancle);
	//删除一条撤单数据(根据处理标记)
	const BOOL Delete(void);
	//检索一条撤单数据
	const BOOL Retrieval(IN OUT CDVDebitCancel& cDebitCancle);
	//更新一条撤单数据的处理标记
	const BOOL UpdateProcessMark(IN const CString& PayIdentifier);
private:
	//初始化数据库
	const BOOL	  InitDB(IN const CString& strDBPath);
	//关闭数据库
	const BOOL	  CloseDB(void);	
	//创建表
	const BOOL CreateTable(void);
	//读取数据明细项
	const BOOL ReadItem(IN OUT CDVDebitCancel& cData);
	//获取记录校核码
	const CString GetVerifyCode(IN const CDVDebitCancel& cData);
//Attributes
public:
	//数据库是否已经初始化
	BOOL	m_bInitDatabase;
	//缓存撤单时限(单位：分钟)
	LONGLONG	m_llLastDelayTime;
	//数据库句柄
	CSQLite3* m_pSQLite3DB;
	//数据表名
	CString m_strProcessTableName;
	//最新的错误描述信息
	CString m_strLastErrorDesc;
	//数据文件绝对路径
	CString m_strSQLiteDBFilePath;
};
