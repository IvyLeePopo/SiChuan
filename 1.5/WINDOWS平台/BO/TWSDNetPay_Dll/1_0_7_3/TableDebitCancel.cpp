/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：TableDebitCancel.cpp
 * 摘    要：
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年8月25日
 */
#include "StdAfx.h"
#include "TableDebitCancel.h"
#include "md5.h"
#include <shlwapi.h>

CTableDebitCancel::CTableDebitCancel(void)
	:m_bInitDatabase(FALSE)
	,m_llLastDelayTime(20)
	,m_pSQLite3DB(NULL)
	,m_strProcessTableName(_T(""))
	,m_strLastErrorDesc(_T(""))
	,m_strSQLiteDBFilePath(_T(""))
{
	m_pSQLite3DB = new CSQLite3();
	ASSERT(m_pSQLite3DB);

	m_strProcessTableName.Format(_T("CancelRecord"));
}

CTableDebitCancel::~CTableDebitCancel(void)
{
	if(NULL != m_pSQLite3DB)
	{
		CloseDB();

		delete m_pSQLite3DB;
		m_pSQLite3DB = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::Save
//     
// 功能描述：保存一条撤单数据
//     
// 输入参数：IN const CDVDebitCancel& cDebitCancle
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::Save(IN const CDVDebitCancel& cDebitCancle)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//判断是否有合法的数据表
		if(!CreateTable())
			throw -1;
		//获取校验码
		CString strVerifyCode = GetVerifyCode(cDebitCancle);
		//构造插入语句
		strSQL.Format(_T("INSERT INTO %s (ProvinceID,RoadID,StationID,ProcessMark,PayIdentifier,VerifyCode,SubTime,CreateTime,ExecuteTime)VALUES(%d,%d,%d,%d,'%s','%s','%s','%s','%s')"),m_strProcessTableName,
			cDebitCancle.ProvinceID,
			cDebitCancle.RoadID,
			cDebitCancle.StationID,
			cDebitCancle.ProcessMark,
			cDebitCancle.PayIdentifier,
			strVerifyCode,
			cDebitCancle.SubTime,
			cDebitCancle.CreateTime.Format(_T("%Y-%m-%d %H:%M:%S")),
			cDebitCancle.ExecuteTime.Format(_T("%Y-%m-%d %H:%M:%S")));

		//执行数据插入SQL语句
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
		{
			throw -2;
		}

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://不存在合法的数据表
			break;
		case -2://插入数据操作失败
			break;
		}
	}
	//关闭数据库
	CloseDB();
	//返回结果
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::Delete
//     
// 功能描述：删除一条撤单数据(根据处理标记)
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::Delete(void)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//判断是否有合法的数据文件
		if(!::PathFileExists(m_strSQLiteDBFilePath))
			throw -1;

		//打开数据库
		if(NULL == m_pSQLite3DB)
			throw -2;
		//判断数据库状态是否正常，并尝试重新初始化数据库
		if(!m_bInitDatabase)
		{
			if(!InitDB(m_strSQLiteDBFilePath))
			{
				throw -3;
			}
		}

		//构造删除语句
		strSQL.Format(_T("DELETE FROM %s WHERE ProcessMark=1 "),m_strProcessTableName);

		//执行数据删除SQL语句
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -4;

		bReturn = TRUE;

		//构造检索语句
		strSQL.Format(_T("SELECT * FROM %s WHERE ProcessMark=0 LIMIT 1"),m_strProcessTableName);

		//执行数据检索SQL语句
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -5;
		//判断是否需要删除文件
		if(0 == m_pSQLite3DB->EffectRows(strSQL))
		{
			CString strFilePath = m_strSQLiteDBFilePath;
			if(::PathFileExists(strFilePath))
			{
				m_pSQLite3DB->CloseRecordSet();
				m_pSQLite3DB->CloseConnection();
				//删除文件
				if(CloseDB())
				{
					DeleteFile(strFilePath);
				}
			}
		}	
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://不存在合法的数据表
			break;
		case -2://执行数据删除操作失败
			break;
		case -3://执行数据检索操作失败
			break;
		}
	}
	//关闭数据库
	CloseDB();
	//返回结果
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::Retrieval
//     
// 功能描述：检索一条撤单数据
//     
// 输入参数：IN OUT CDVDebitCancel& cDebitCancle
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::Retrieval(IN OUT CDVDebitCancel& cDebitCancle)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//判断是否有合法的数据文件
		if(!::PathFileExists(m_strSQLiteDBFilePath))
			throw -1;
		//打开数据库
		if(NULL == m_pSQLite3DB)
			throw -2;
		//判断数据库状态是否正常，并尝试重新初始化数据库
		if(!m_bInitDatabase)
		{
			if(!InitDB(m_strSQLiteDBFilePath))
			{
				throw -3;
			}
		}

		//构造检索语句
		strSQL.Format(_T("SELECT * FROM %s WHERE LENGTH(PayIdentifier)>0 AND ProcessMark=0 ORDER BY CreateTime LIMIT 1"),m_strProcessTableName);

		//执行数据检索SQL语句
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -4;
		
		//判断记录集是否存在数据
		if(0 == m_pSQLite3DB->EffectRows(strSQL))
			throw -5;

		//读取数据
		if(m_pSQLite3DB->IsEOF())
			throw -6;

		//读取数据明细项
		if(!ReadItem(cDebitCancle))
			throw -7;

		//获取校验码
		CString strReCalcCode = GetVerifyCode(cDebitCancle);
		//获取时间差
		CTimeSpan timeSpan = CTime::GetCurrentTime() - cDebitCancle.CreateTime;
		//如果校验码不一致或该条记录已超过超时时间
		if(     (cDebitCancle.VerifyCode != strReCalcCode)
			||  (timeSpan.GetMinutes() > m_llLastDelayTime)
			)
		{
			//构造删除语句
			strSQL.Format(_T("DELETE FROM %s WHERE PayIdentifier='%s' "),m_strProcessTableName,cDebitCancle.PayIdentifier);

			//执行数据删除SQL语句
			if(!m_pSQLite3DB->ExecuteSQL(strSQL))
				throw -8;

			bReturn = FALSE;
		}
		else
		{
			bReturn = TRUE;
		}		
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://不存在合法的数据表
			break;
		case -2://执行数据检索操作失败
			break;
		case -4://执行数据检索SQL语句
		case -5://判断记录集是否存在数据
		case -6://无法检索到有效数据
			{
				CString strFilePath = m_strSQLiteDBFilePath;
				if(::PathFileExists(strFilePath))
				{
					m_pSQLite3DB->CloseRecordSet();
					m_pSQLite3DB->CloseConnection();
					//删除文件
					if(CloseDB())
					{
						DeleteFile(strFilePath);
					}
				}
			}
			break;
		case -7://读取数据明细项出错
			break;
		case -8://删除指定记录出错
			break;
		}
	}
	//关闭数据库
	CloseDB();
	//返回结果
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::UpdateProcessMark
//     
// 功能描述：更新一条撤单数据的处理标记
//     
// 输入参数：IN const CString& PayIdentifier
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::UpdateProcessMark(IN const CString& PayIdentifier)
{
	BOOL bReturn = FALSE;
	CString strSQL;
	try
	{
		//判断是否有合法的数据文件
		if(!::PathFileExists(m_strSQLiteDBFilePath))
			throw -1;
		//打开数据库
		if(NULL == m_pSQLite3DB)
			throw -2;
		//判断数据库状态是否正常，并尝试重新初始化数据库
		if(!m_bInitDatabase)
		{
			if(!InitDB(m_strSQLiteDBFilePath))
			{
				throw -3;
			}
		}

		//构造删除语句
		strSQL.Format(_T("UPDATE %s SET ProcessMark=1 WHERE PayIdentifier='%s' "),m_strProcessTableName,PayIdentifier);

		//执行数据更新SQL语句
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -4;

		bReturn = TRUE;

		//构造检索语句
		strSQL.Format(_T("SELECT * FROM %s WHERE ProcessMark=0 LIMITE 1"),m_strProcessTableName);

		//执行数据检索SQL语句
		if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			throw -5;
		//判断是否需要删除文件
		if(0 == m_pSQLite3DB->EffectRows(strSQL))
		{
			CString strFilePath = m_strSQLiteDBFilePath;
			m_pSQLite3DB->CloseConnection();
			//删除文件
			if(CloseDB())
			{
				DeleteFile(strFilePath);
			}
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://不存在合法的数据表
			break;
		case -2://执行数据删除操作失败
			break;
		case -3://执行数据检索操作失败
			break;
		}
	}
	//关闭数据库
	CloseDB();
	//返回结果
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::InitDB
//     
// 功能描述：初始化数据库
//     
// 输入参数：IN const CString& strDBPath
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::InitDB(IN const CString& strDBPath)
{
	//获得新的数据库
	m_strSQLiteDBFilePath = strDBPath;
	if(m_pSQLite3DB->IsOpenConnection())
	{
		m_pSQLite3DB->CloseConnection();
	}
	//重新打开数据库
	m_bInitDatabase = m_pSQLite3DB->OpenConnection(m_strSQLiteDBFilePath);
	return m_bInitDatabase;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::CloseDB
//     
// 功能描述：关闭数据库
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::CloseDB(void)
{
	BOOL bReturn = FALSE;
	if(NULL != m_pSQLite3DB) 
	{
		m_pSQLite3DB->CloseRecordSet();

		bReturn = m_pSQLite3DB->CloseConnection();

		m_bInitDatabase = !bReturn;
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::CreateTable
//     
// 功能描述：创建表
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::CreateTable(void)
{
	BOOL bReturn = FALSE;

	CString strSQL;
	try
	{
		//判断数据库指针是否非法
		if(NULL == m_pSQLite3DB)
			throw -1;
		//判断数据库状态是否正常，并尝试重新初始化数据库
		if(!m_bInitDatabase)
		{
			if(!InitDB(m_strSQLiteDBFilePath))
			{
				throw -2;
			}
		}

		//构造创建表的SQL语句
		strSQL.Format(_T("CREATE TABLE %s (ProvinceID int NOT NULL,RoadID int NOT NULL,StationID int NOT NULL,ProcessMark int NOT NULL,PayIdentifier varchar(64) NOT NULL,SubTime varchar(64) NOT NULL,VerifyCode varchar(64) NOT NULL,CreateTime varchar(64) NOT NULL,ExecuteTime varchar(64) NOT NULL,PRIMARY KEY(PayIdentifier))"),m_strProcessTableName);

		//尝试创建数据表
		if(!m_pSQLite3DB->CheckTableExists(m_strProcessTableName))
		{
			if(!m_pSQLite3DB->ExecuteSQL(strSQL))
			{
				throw -3;
			}
		}

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://数据库指针非法
			break;
		case -2://数据库未能正常初始化
			break;
		case -3://创建数据表失败
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::ReadItem
//     
// 功能描述：读取数据明细项
//     
// 输入参数：IN OUT cDebitCancle& cData
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月25日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CTableDebitCancel::ReadItem(IN OUT CDVDebitCancel& cData)
{
	BOOL bReturn = FALSE;
	try
	{
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("ProvinceID"),cData.ProvinceID))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("RoadID"),cData.RoadID))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("StationID"),cData.StationID))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("ProcessMark"),cData.ProcessMark))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("PayIdentifier"),cData.PayIdentifier))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("SubTime"),cData.SubTime))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("VerifyCode"),cData.VerifyCode))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("CreateTime"),cData.CreateTime))
			throw -4;
		//读取数据明细项
		if(!m_pSQLite3DB->GetColumnValue(_T("ExecuteTime"),cData.ExecuteTime))
			throw -4;

		bReturn = TRUE;
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1://不存在合法的数据表
			break;
		case -4://读取数据明细项出错
			break;
		}
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CTableDebitCancel::GetVerifyCode
//     
// 功能描述：获取记录校核码
//     
// 输入参数：IN const CDVDebitCancel& cData
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2017年1月4日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CTableDebitCancel::GetVerifyCode(IN const CDVDebitCancel& cData)
{
	CString strVerifyCode;
	CString strData;
	strData.Format(_T("%s%d%d%d%s"),
		cData.PayIdentifier,
		cData.RoadID,
		cData.ProvinceID,
		cData.StationID,
		cData.SubTime);
	CMD5 md5((LPCSTR)strData);
	strVerifyCode = md5.GetMD5();
	return strVerifyCode;
}