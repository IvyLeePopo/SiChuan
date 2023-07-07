/*
 * Copyright(C) 2016,SQUIRREL 个人工作室 保留所有权利。( All rights reserved. )
 * 
 * 文件名称：HTSMProtocol.cpp
 * 摘    要：扫码器内部通讯协议解析类，实现文件
 * 当前版本：1.0.0.1
 * 作    者：ROCY
 * 创建日期：2016年2月22日
 */
#include "StdAfx.h"
#include "HTSMDeviceControl.h"
#include "ScanMachine.h"

extern CScanMachineApp theApp;

CHTSMDeviceControl::CHTSMDeviceControl(void)
	:m_pSMIF(NULL)
	,m_nCallerThreadID(0)
	,m_bIsHaveFrameFlag(FALSE)
	,m_tLastHeartTimeForBoard(0)
	,m_dwBeginScanTime(0)
	,m_bNeedReSendLastData(FALSE)
{
	memset(&m_stForBeginScan,0,sizeof(SYSTEMTIME));
	memset(&m_stForEndScan,0,sizeof(SYSTEMTIME));
}

CHTSMDeviceControl::~CHTSMDeviceControl(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::AnalyseReciveData
//     
// 功能描述：解析接收到的数据，判断是否符合协议
//     
// 输入参数：IN OUT BYTE* lpData			-- 接收到的串口数据指针
//         ：IN const int& iDataSize	-- 需要解析的数据长度
//         ：IN OUT  LPBYTE& pBegin		-- 帧头位置
//         ：IN OUT  LPBYTE& pEnd		-- 帧尾位置
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年2月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::AnalyseReciveData(IN OUT LPBYTE lpData,IN const int& iDataSize,IN OUT LPBYTE& pBegin,IN OUT LPBYTE& pEnd)
{
	pBegin = NULL;
	pEnd   = NULL;
	//帧头搜索标记
	BOOL bFindBegin = FALSE;
	//帧尾搜索标记
	BOOL bFindEnd   = FALSE;
	//设置检索数据帧标记为假
	m_bIsHaveFrameFlag = FALSE;
	
	// 若数据大小为1,判断是否为帧头
	if( 1 == iDataSize )
	{
		if ( SM_PROTOCOL_HEADER == lpData[0])
		{
			pBegin = lpData;
		}
		return;
	}
	
	// 从右往左搜索帧尾和帧头
	for ( register int iIndex = iDataSize - 1; iIndex >= 0; --iIndex )
	{
		if ( (!bFindEnd) && (SM_PROTOCOL_END == lpData[iIndex]))
		{
			bFindEnd = TRUE;
			pEnd = lpData + iIndex;
		}
		else if ( (!bFindBegin))// && ((m_dwFrameHeader == lpData[iIndex]) && (m_dwFrameHeader == lpData[iIndex-1])) )
		{
			if(SM_PROTOCOL_HEADER == lpData[iIndex])
			{
				bFindBegin = TRUE;
				pBegin = lpData + iIndex;
				if(bFindBegin && bFindEnd)
				{
					m_bIsHaveFrameFlag = TRUE;
				}
				return;
			}
		}
	}	

	pEnd = NULL;// 若没有找到帧头,找到的帧尾没有意义,将帧尾置空
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::GetDataFrameType
//     
// 功能描述：根据接收到的数据判断数据帧
//     
// 输入参数：IN const LPBYTE lpFrame
//         ：IN const DWORD& dwSize
//         ：IN OUT CString& strReceiveFrameType
// 输出参数：const int
// 编写人员：ROCY
// 编写时间：2016年2月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const int CHTSMDeviceControl::GetDataFrameType(IN const LPBYTE lpFrame,IN const DWORD& dwSize,IN OUT CString& strReceiveFrameType)
{
	int iProtocolType = SM_PROTOCOL_FRAME_TYPE_UNKNOW;

	if(   ( SM_PROTOCOL_FRAME_TYPE_B0 == lpFrame[1])
		||( SM_PROTOCOL_FRAME_TYPE_B1 == lpFrame[1])
		||( SM_PROTOCOL_FRAME_TYPE_B2 == lpFrame[1])
		||( SM_PROTOCOL_FRAME_TYPE_C0 == lpFrame[1])
		)
	{
		iProtocolType = lpFrame[1];
	}
	return iProtocolType;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::InitInnerDevice
//     
// 功能描述：初始化设备
//     
// 输入参数：IN const int& iInnerDeviceType
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::InitInnerDevice(IN OUT CString& strLog,IN const int& iInnerDeviceType)
{
	BOOL bReturn = FALSE;
	if(NULL != m_pSMIF)
	{
		delete m_pSMIF;
		m_pSMIF = NULL;
	}
	switch(iInnerDeviceType)
	{
	default:
		m_pSMIF = new CHTSMDeviceHW3310g(SM_TYPE_HONEYWELL_3310G);
		strLog.Format(_T("扫码管理线程,启动的扫码器型号为[霍尔韦尔3310G]"));
		break;
	} 
	if(NULL != m_pSMIF)
		bReturn = TRUE;
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::IsAllowAnalyseReceiveDataOnBytesNum
//     
// 功能描述：接收到的数据长度是否允许进行数据帧校验
//     
// 输入参数：IN const DWORD& dwHadReceiveDataSize
// 输出参数：inline
// 编写人员：ROCY
// 编写时间：2016年2月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::IsAllowAnalyseReceiveDataOnBytesNum(IN const DWORD& dwHadReceiveDataSize) const
{
	return dwHadReceiveDataSize >= 2;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::VerifyFrameData
//     
// 功能描述：数据帧校验,同时将数据进行转义处理
//     
// 输入参数：IN const LPBYTE lpData	--	数据帧内容
//         ：IN const DWORD& dwSize	--	数据帧长度
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月22日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：CRC校验，数据帧为未经过转译处理后的原始数据。
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::VerifyFrameData(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	BOOL bReturn = FALSE;
	BYTE byBuff[512] = {0};
	if(NULL != lpData)
	{
		memcpy(byBuff,lpData,dwSize);

		//数据转义
		DWORD dwRealDataLen = dwSize;
		CovertData(byBuff,dwRealDataLen,FALSE);

		//获取帧类型
		BYTE byFrameType = byBuff[1];
		//获取数据区长度
		WORD wDataContentLen = MAKEWORD(byBuff[3],byBuff[2]);
		//获取数据帧的CRC校验码
		WORD wOrgCRCCode = MAKEWORD(byBuff[dwRealDataLen-2],byBuff[dwRealDataLen-3]);
		//获取计算的CRC校验码
		WORD wCalcCRCCode = GetVerifyCode(byFrameType,wDataContentLen,&byBuff[4]);

		//判断是否一致
		bReturn = (wOrgCRCCode == wCalcCRCCode)?TRUE:FALSE;

		if(!bReturn && (dwSize>100))
			bReturn = TRUE;
	}
	
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA1
//     
// 功能描述：构造A1帧 -- 配置主板参数
//     
// 输入参数：IN const CString& strParam
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA1(IN const CString& strParam)
{
	CString strFrameHexData;
	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA2
//     
// 功能描述：构造A2帧 -- 读取主板配置信息
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA2(void)
{
	CString strFrameHexData;
	
	BYTE bySendData[512] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A2;
	//数据长度
	bySendData[2] = 0x00;
	bySendData[3] = 0x00;
	//校验码
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[3]);
	bySendData[4] = HIBYTE(wVerifyCode);
	bySendData[5] = LOBYTE(wVerifyCode);
	//协议尾
	bySendData[6] = SM_PROTOCOL_END;

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData,7);

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA3
//     
// 功能描述：构造A3帧 -- 控制亮灯状态
//     
// 输入参数：IN const int& iLightIndex	-- 设置亮灯索引（0,全灭;1,等待扫码;2,扫码失败;3,扫码成功)
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA3(IN const int& iLightIndex)
{
	CString strFrameHexData;

	BYTE bySendData[8] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A3;
	//数据长度
	bySendData[2] = 0x00;
	bySendData[3] = 0x01;
	//获取数据内容
	WORD dwData = 0x0000;
	BYTE byTemp = 0x01;

	dwData = byTemp<<iLightIndex;

//	bySendData[4] = HIBYTE(dwData);
	bySendData[4] = LOBYTE(dwData);
	//校验码
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[4]);
	bySendData[5] = HIBYTE(wVerifyCode);
	bySendData[6] = LOBYTE(wVerifyCode);
	//协议尾
	bySendData[7] = SM_PROTOCOL_END;

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData,8);
	//发送IO控制指令，则需要将A5指令清除
	m_strLastSendFrameData.Empty();

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA4
//     
// 功能描述：构造A4帧	-- 读取亮灯状态
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA4(void)
{
	CString strFrameHexData;

	BYTE bySendData[512] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A4;
	//数据长度
	bySendData[2] = 0x00;
	bySendData[3] = 0x00;
	//校验码
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[3]);
	bySendData[4] = HIBYTE(wVerifyCode);
	bySendData[5] = LOBYTE(wVerifyCode);
	//协议尾
	bySendData[6] = SM_PROTOCOL_END;

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData,7);
	//发送IO控制指令，则需要将A5指令清除
	m_strLastSendFrameData.Empty();

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA5
//     
// 功能描述：构造A5帧	-- 转发数据给制定扫码设备
//     
// 输入参数：IN const LPBYTE lpData -- 需要发送的数据
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA5(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	CString strFrameHexData;
	//获取校验码
	BYTE byTempBuff[512] = {0};
	memcpy(byTempBuff,lpData,dwSize);
	WORD wVerifyCode = GetVerifyCode(SM_PROTOCOL_FRAME_TYPE_A5,dwSize,byTempBuff);

	BYTE bySendData[1024] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A5;
	//数据长度
	bySendData[2] = HIBYTE(LOWORD(dwSize));
	bySendData[3] = LOBYTE(LOWORD(dwSize));

	//获取数据内容
	memcpy(&bySendData[4],&byTempBuff[0],dwSize);

	//校验码
	bySendData[4+dwSize]		= HIBYTE(wVerifyCode);
	bySendData[4+dwSize + 1]	= LOBYTE(wVerifyCode);
	//协议尾
	bySendData[4+dwSize + 2]	= SM_PROTOCOL_END;

	//进行数据转译
	DWORD dwRealDataLen = 7+dwSize;
	CovertData(bySendData,dwRealDataLen,TRUE);

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData,dwRealDataLen);
	//保存当前发送的数据
	m_strLastSendFrameData = strFrameHexData;

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA5
//     
// 功能描述：构造A5帧	-- 新增用于智能扫码硬件显示
//     
// 输入参数：tagScanDisplay sDisplay -- 显示内容结构体
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA5(tagScanDisplay sDisplay)
{
	CString strFrameHexData = _T("");
	WORD	wSzie = 0;
	WORD wVerifyCode = 0;

	BYTE byTempBuff[512] = {0};
	BYTE bySendData[1024] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A5;

	if( 2 == sDisplay.Type )//只有时间字段显示
	{
		//	wSzie = 0x0008;

		wSzie = 0x0012;
		//数据帧长度
		bySendData[2] = HIBYTE( wSzie );
		bySendData[3] = LOBYTE( wSzie );

		//内挂命令号
		bySendData[4] = 0x00;
		bySendData[5] = 0x56;

		//内部数据长度
		bySendData[6] = 0x00;
		//bySendData[7] = 0x04;
		//当前时间的Unix格式
		//DWORD dwTime = sDisplay.tShowtime;
		//bySendData[9]	= HIBYTE(HIWORD(dwTime));
		//bySendData[10]	= LOBYTE(HIWORD(dwTime));
		//bySendData[11]	= HIBYTE(LOWORD(dwTime));
		//bySendData[12]	= LOBYTE(LOWORD(dwTime));
		//计算校验码
		//wVerifyCode = GetVerifyCode(bySendData[1], wSzie, &bySendData[4]);
		//年月日时分秒(14)
		bySendData[7] = 0x0E;

		
		//获取年月日时分秒
		CTime cTime(sDisplay.tShowtime);
		int iYear	= cTime.GetYear();
		int iMonth	= cTime.GetMonth();
		int iDay	= cTime.GetDay();
		int iHour	= cTime.GetHour();
		int iMin	= cTime.GetMinute();
		int iSecond = cTime.GetSecond();

		CString strTemp;
		strTemp.Format(_T("%04d"),iYear);
		memcpy(&bySendData[8],strTemp.GetBuffer(0),4);
		strTemp.ReleaseBuffer();

		strTemp.Format(_T("%02d"),iMonth);
		memcpy(&bySendData[12],strTemp.GetBuffer(0),2);
		strTemp.ReleaseBuffer();

		strTemp.Format(_T("%02d"),iDay);
		memcpy(&bySendData[14],strTemp.GetBuffer(0),2);
		strTemp.ReleaseBuffer();

		strTemp.Format(_T("%02d"),iHour);
		memcpy(&bySendData[16],strTemp.GetBuffer(0),2);
		strTemp.ReleaseBuffer();

		strTemp.Format(_T("%02d"),iMin);
		memcpy(&bySendData[18],strTemp.GetBuffer(0),2);
		strTemp.ReleaseBuffer();

		strTemp.Format(_T("%02d"),iSecond);
		memcpy(&bySendData[20],strTemp.GetBuffer(0),2);
		strTemp.ReleaseBuffer();

		//计算校验码
		wVerifyCode = GetVerifyCode(bySendData[1], wSzie, &bySendData[4]);		
	}
	else//if( 1 == sDisplay.Type )//默认显示车型，重量，金额
	{
		BYTE byDataMode = 0x01;
		//内挂命令号
		bySendData[4] = 0x00;
		switch(sDisplay.Type)
		{
		case 0x01:
			bySendData[5] = 0x55;
			byDataMode = 0x01;
			break;
		case 0x02:
			break;
		case 0x03:
			bySendData[5] = 0x57;
			byDataMode = 0x01;
			break;
		case 0x04:
			bySendData[5] = 0x58;
			byDataMode = 0x02;
			break;
		case 0x05:
			bySendData[5] = 0x59;
			byDataMode = 0x02;
			break;
		case 0x06:
			bySendData[5] = 0x60;
			byDataMode = 0x02;
			break;
		}

		if(1 == byDataMode)
		{
			wSzie = 0x0031;

			//数据帧长度
			bySendData[2] = HIBYTE( wSzie );
			bySendData[3] = LOBYTE( wSzie );

			//内部数据长度
			bySendData[6] = 0x00;
			bySendData[7] = 0x2D;

			//扣款车型	1~4 客车		11~15 货车
			BYTE bVType = 0;
			if( 2 == sDisplay.VehicleType)//车种	1客2货
			{
				bVType = 10 + sDisplay.VehicleClass;
			}
			else//if( 1 == sDisplay.VehicleType)	//默认客车
			{
				bVType = sDisplay.VehicleClass;
			}
			bySendData[8] = bVType;

			//车辆重量	4字节整型，高字在前，低字在后，单位：公斤
			DWORD dwWeight = sDisplay.Weight;
			bySendData[9]	= HIBYTE(HIWORD(dwWeight));
			bySendData[10]	= LOBYTE(HIWORD(dwWeight));
			bySendData[11]	= HIBYTE(LOWORD(dwWeight));
			bySendData[12]	= LOBYTE(LOWORD(dwWeight));

			//扣款金额	4字节整型，高字在前，低字在后，单位：分
			DWORD dwMoney = sDisplay.Money;
			bySendData[13]	= HIBYTE(HIWORD(dwMoney));
			bySendData[14]	= LOBYTE(HIWORD(dwMoney));
			bySendData[15]	= HIBYTE(LOWORD(dwMoney));
			bySendData[16]	= LOBYTE(LOWORD(dwMoney));

			//限重	4字节整型，高字在前，低字在后
			DWORD dwLimit = sDisplay.LimitWeight;
			bySendData[17]	= HIBYTE(HIWORD(dwLimit));
			bySendData[18]	= LOBYTE(HIWORD(dwLimit));
			bySendData[19]	= HIBYTE(LOWORD(dwLimit));
			bySendData[20]	= LOBYTE(LOWORD(dwLimit));

			//轴数	4字节整型，高字在前，低字在后
			DWORD dwAxisCount = sDisplay.AxisCount;
			bySendData[21]	= HIBYTE(HIWORD(dwAxisCount));
			bySendData[22]	= LOBYTE(HIWORD(dwAxisCount));
			bySendData[23]	= HIBYTE(LOWORD(dwAxisCount));
			bySendData[24]	= LOBYTE(LOWORD(dwAxisCount));
			
			//余额	4字节整型，高字在前，低字在后
			DWORD dwBalance = sDisplay.Balance ;
			bySendData[25]	= HIBYTE(HIWORD(dwBalance));
			bySendData[26]	= LOBYTE(HIWORD(dwBalance));
			bySendData[27]	= HIBYTE(LOWORD(dwBalance));
			bySendData[28]	= LOBYTE(LOWORD(dwBalance));

			//入口站名
			memcpy(&bySendData[29],&sDisplay.EntrySTName[0],24);
		}
		else
		{
			wSzie = 0x0002;

			//数据帧长度
			bySendData[2] = HIBYTE( wSzie );
			bySendData[3] = LOBYTE( wSzie );
		}

		//计算校验码
		wVerifyCode = GetVerifyCode(bySendData[1], wSzie, &bySendData[4]);		
	}

	//校验码
	bySendData[4+wSzie]		= HIBYTE(wVerifyCode);
	bySendData[4+wSzie + 1]	= LOBYTE(wVerifyCode);

	//协议尾
	bySendData[4+wSzie + 2]	= SM_PROTOCOL_END;

	//进行数据转译
	DWORD dwRealDataLen = 7+wSzie;
	CovertData(bySendData, dwRealDataLen, TRUE);

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData, dwRealDataLen);

	BOOL bNeedSend = TRUE;
	//对于新增的车辆到达/车辆离开指令，视配置情况看是否需要发送
	if( (0x59 == bySendData[5]) || (0x60 == bySendData[5]) )
	{
		//配置确定不允许进行下发线圈状态指令
		if(0 == theApp.m_iNeedNotifyLoopState)
		{
			bNeedSend = FALSE;
		}
	}

	if(bNeedSend)
	{
		//保存当前发送的数据
		m_strLastSendFrameData = strFrameHexData;
	}
	else
	{
		strFrameHexData.Empty();
	}

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ConstructFrameA8
//     
// 功能描述：构造A8帧	-- 复位设备
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA6(void)
{
	CString strFrameHexData;

	BYTE bySendData[512] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A6;
	//数据长度
	bySendData[2] = 0x00;
	bySendData[3] = 0x00;
	//校验码
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[4]);
	bySendData[4] = HIBYTE(wVerifyCode);
	bySendData[5] = LOBYTE(wVerifyCode);
	//协议尾
	bySendData[6] = SM_PROTOCOL_END;

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData,7);

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ContructFrameC0
//     
// 功能描述：构造C0帧 -- 指令应答帧
//     
// 输入参数：IN const BYTE& byResponseType
//         ：IN const BYTE& byResponseFrame
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月29日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ContructFrameC0(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame)
{
	CString strFrameHexData;

	BYTE bySendData[512] = {0};
	//协议头
	bySendData[0] = SM_PROTOCOL_HEADER;
	//数据类型
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_C0;
	//数据长度
	bySendData[2] = 0x00;
	bySendData[3] = 0x02;
	//接收帧号
	bySendData[4] = byResponseFrame;
	//执行状态
	bySendData[5] = byResponseType;
	//校验码
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[4]);
	bySendData[6] = HIBYTE(wVerifyCode);
	bySendData[7] = LOBYTE(wVerifyCode);
	//协议尾
	bySendData[8] = SM_PROTOCOL_END;

	//转化成16进制字符串
	strFrameHexData = theApp.Bin2Hex(bySendData,9);

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ProcessReceiveFrameData
//     
// 功能描述：处理接收数据
//     
// 输入参数：IN const LPBYTE lpData	--	数据帧内容
//         ：IN const DWORD& dwSize --	数据帧长度
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::ProcessReceiveFrameData(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	BOOL bReturn  = TRUE;

	//校验数据
	if(VerifyFrameData(lpData,dwSize))
	{
		CString strFrameType;
		switch(GetDataFrameType(lpData,dwSize,strFrameType))
		{
		case SM_PROTOCOL_FRAME_TYPE_B0:	//初始化信息帧
			ProcessFrameB0(lpData,dwSize);
			break;
		case SM_PROTOCOL_FRAME_TYPE_B1:	//心跳帧
			ProcessFrameB1(lpData,dwSize);
			break;
		case SM_PROTOCOL_FRAME_TYPE_B2:	//数据上传信息帧
			ProcessFrameB2(lpData,dwSize);
			break;
		case SM_PROTOCOL_FRAME_TYPE_C0:	//指令应答帧
			ProcessFrameC0(lpData,dwSize);
			break;
		default:
			bReturn = FALSE;
			break;
		}
	}
	else
	{
		bReturn = FALSE;
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ProcessFrameB0
//     
// 功能描述：处理B0帧
//     
// 输入参数：IN const LPBYTE lpData
//         ：IN const DWORD& dwSize
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameB0(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	if(NULL != lpData)
	{
		//获取长度
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//获取数据区内容
		BYTE byDataSection[512]={0};
		memcpy(&byDataSection[0],&lpData[0],wDataLen);
		//开始处理
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ProcessFrameB1
//     
// 功能描述：处理B1帧
//     
// 输入参数：IN const LPBYTE lpData
//         ：IN const DWORD& dwSize
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameB1(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	if(NULL != lpData)
	{
		//获取长度
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//获取数据区内容
		BYTE byDataSection[512]={0};
		memcpy(&byDataSection[0],&lpData[0],wDataLen);
		//开始处理
		m_tLastHeartTimeForBoard = time(NULL) ;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ProcessFrameB2
//     
// 功能描述：处理B2帧
//     
// 输入参数：IN const LPBYTE lpData
//         ：IN const DWORD& dwSize
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameB2(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	try
	{
		if(NULL == lpData)
			throw -1;
		//获取长度
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//获取数据区内容
		BYTE byDataSection[512]={0};
		memcpy(&byDataSection[0],&lpData[4],wDataLen);
		//去除前后缀
		//获得回馈数据的串口索引
		BYTE byDeviceID = byDataSection[0];
		//获得响应的数据帧标记
		BYTE byResponseFrameType = byDataSection[1];

		if(SM_PROTOCOL_FRAME_TYPE_A5 != byResponseFrameType)
		{
			switch(byResponseFrameType)
			{
			case 0xC0:
				break;
			case 0xA4:
				break;
			}
		}
		else
		{
			//获得响应的内挂设备指令标识
			BYTE byResponseDeviceCommandType = byDataSection[2];
			//获得内挂设备指定协议的原始数据
			BYTE byInnerProtocolContent[512]={0};
			DWORD dwInnerProtocolLen = wDataLen - 3;
			memcpy(&byInnerProtocolContent[0],&byDataSection[3],dwInnerProtocolLen);
			
			//根据内挂设备的指定协议进行解析
			CHTSMDeviceBaseIF* pSMIF = GetInnerDeviceProtocolClass();
			if(NULL == pSMIF)
				throw -2;
			////检测扫码器内部协议结果状态
			//if(!pSMIF->CheckCommandExecuteResult(byInnerProtocolContent,dwInnerProtocolLen))
			//	throw -3;
			//格式化数据
			CString strFormatCode = pSMIF->FormatReadData(byInnerProtocolContent,dwInnerProtocolLen);
			//处理结果
			switch(byResponseDeviceCommandType)
			{
			case SM_CMD_INIT:
				fnInnerDeviceProtocolProcessResultForInit(byDeviceID,strFormatCode);
				break;
			case SM_CMD_VER:
				fnInnerDeviceProtocolProcessResultForVer(byDeviceID,strFormatCode);
				break;
			case SM_CMD_START:
			case 0x57:
				fnInnerDeviceProtocolProcessResultForStartScan(byDeviceID,strFormatCode);
				break;
			case SM_CMD_STOP:
				fnInnerDeviceProtocolProcessResultForStopScan(byDeviceID,strFormatCode);
				break;
			case SM_CMD_HEART:
				fnInnerDeviceProtocolProcessResultForHeart(byDeviceID,strFormatCode);
				break;
			case SM_CMD_RESET_FACTORY:
				fnInnerDeviceProtocolProcessResultForDefaultFactory(byDeviceID,strFormatCode);
				break;
			case SM_CMD_RESET_PERSION:
				fnInnerDeviceProtocolProcessResultForDefaultPersion(byDeviceID,strFormatCode);
				break;
			}
			//重置上次的命令类型
			pSMIF->ResetLastCommandType();
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		case -2:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ProcessFrameC0
//     
// 功能描述：处理C0帧
//     
// 输入参数：IN const LPBYTE lpData
//         ：IN const DWORD& dwSize
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameC0(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	m_bNeedReSendLastData = FALSE;
	try
	{
		if(NULL == lpData)
			throw -1;
		//获取长度
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//获取响应帧标记
		BYTE byFrameType = lpData[4];
		//获取执行状态
		BYTE byOpState   = lpData[5];
		//如果状态返回非法，并且上次发送指令为A5帧，标记需要进行重发数据
		if(0xA5 == byFrameType)
		{
			if(0x00 != byOpState)
			{
				m_bNeedReSendLastData = TRUE;
			}
		}
	}
	catch(int& iErrorCode)
	{
		switch(iErrorCode)
		{
		case -1:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::CovertData
//     
// 功能描述：数据转译
//     
// 输入参数：IN OUT LPBYTE lpData		--	原始/转译后 数据内容
//         ：IN OUT DWORD& dwSize		--	原始/转译后 数据长度
//         ：IN const BOOL& bSendState	--	转译接收到的数据或转译发送的数据
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::CovertData(IN OUT LPBYTE lpData,IN OUT DWORD& dwSize,IN const BOOL& bSendState)
{
	BYTE byTemp[2048]={0};
	int iIndex=0;

	if(dwSize>2048)
		return;
	
	::memcpy(byTemp,lpData,dwSize);
	
	//设置帧头
	lpData[0]=SM_PROTOCOL_HEADER;
	//处理数据帧其它区域
	iIndex=1;
	for(DWORD i=1;i<(dwSize-1);i++)
	{
		if(!bSendState)//接收数据
		{
			if(SM_PROTOCOL_HEAND_CONVERT_CHAR1 == byTemp[i] && SM_PROTOCOL_HEAND_CONVERT_CHAR2 == byTemp[i+1])
			{
				lpData[iIndex++] = SM_PROTOCOL_HEADER;
				i++;
			}
			else if(SM_PROTOCOL_HEAND_CONVERT_CHAR1 == byTemp[i] && SM_PROTOCOL_CONVERT_CHAR == byTemp[i+1])
			{
				lpData[iIndex++] = SM_PROTOCOL_HEAND_CONVERT_CHAR1;
				i++;
			}
			else if(SM_PROTOCOL_END_CONVERT_CHAR1 == byTemp[i] && SM_PROTOCOL_END_CONVERT_CHAR2 == byTemp[i+1])
			{
				lpData[iIndex++] = SM_PROTOCOL_END;
				i++;
			}
			else if(SM_PROTOCOL_END_CONVERT_CHAR1 == byTemp[i] && SM_PROTOCOL_CONVERT_CHAR == byTemp[i+1])
			{
				lpData[iIndex++] = SM_PROTOCOL_END_CONVERT_CHAR1;
				i++;
			}
			else
			{
				lpData[iIndex++] = byTemp[i];
			}
		}
		else		//发送数据
		{
			if(SM_PROTOCOL_HEADER == byTemp[i])
			{
				lpData[iIndex++]=SM_PROTOCOL_HEAND_CONVERT_CHAR1;
				lpData[iIndex++]=SM_PROTOCOL_CONVERT_CHAR;
			}
			else if(SM_PROTOCOL_HEAND_CONVERT_CHAR1 == byTemp[i])
			{
				lpData[iIndex++]=SM_PROTOCOL_HEAND_CONVERT_CHAR1;	
				lpData[iIndex++]=SM_PROTOCOL_CONVERT_CHAR;	
			}
			else if(SM_PROTOCOL_END == byTemp[i])
			{
				lpData[iIndex++]=SM_PROTOCOL_END_CONVERT_CHAR1;
				lpData[iIndex++]=SM_PROTOCOL_END_CONVERT_CHAR2;
			}
			else if(SM_PROTOCOL_END_CONVERT_CHAR1 == byTemp[i])
			{
				lpData[iIndex++]=SM_PROTOCOL_END_CONVERT_CHAR1;	
				lpData[iIndex++]=SM_PROTOCOL_CONVERT_CHAR;	
			}
			else
			{
				lpData[iIndex++]=byTemp[i];
			}
		}
	}
	//设置帧尾
	lpData[iIndex++] = SM_PROTOCOL_END;
	//获得数据长度
	dwSize = iIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::GetVerifyCode
//     
// 功能描述：获取校验码
//     
// 输入参数：IN const LPBYTE lpData	--	需要校验的数据内容
//         ：IN const DWORD& dwSize --  需要校验的数据长度
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年2月23日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const WORD CHTSMDeviceControl::GetVerifyCode(IN const BYTE& byFrameType,IN const WORD& wSize,IN const LPBYTE lpData) const
{
	BYTE byCode = 0x00;
	BYTE byBuff[1024] = {0};
	WORD wCalcCRCCode = 0x00;
	if(NULL != lpData)
	{
		if(wSize>512)
			return 0x00;

		memcpy(byBuff,lpData,wSize>500?500:wSize);

		//校核数据组合的内容
		BYTE byVerifyDataSection[1024] = {0};
		//校核数据组合的长度
		DWORD dwVerifyDataLen = 0x00;

		byVerifyDataSection[dwVerifyDataLen++] = byFrameType;

		byVerifyDataSection[dwVerifyDataLen++] = HIBYTE(wSize);
		byVerifyDataSection[dwVerifyDataLen++] = LOBYTE(wSize);

		memcpy(&byVerifyDataSection[dwVerifyDataLen],&byBuff[0],wSize);
		dwVerifyDataLen += wSize;

		//获取重新计算的CRC码
		wCalcCRCCode = theApp.CRC16(byVerifyDataSection,dwVerifyDataLen);
	}
	return wCalcCRCCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForInit
//     
// 功能描述：设备内部协议处理函数 -- 处理初始化指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForInit(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;
	
	if(bState)
	{
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(byComIndex == m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID)
			{
				if(!m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsInit)
				{
					m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsInit = TRUE;
				}
				else
				{
					for(INT_PTR j = 0;j<m_cInnerDeviceInfo.SMStateList.GetSize();++j)
					{
						if(!m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsInit)
						{
							m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsInit = TRUE;
							break;
						}
					}
				}
				break;
			}
		}
	}

	if(++m_cInnerDeviceInfo.HasProcessDeviceInitResultCount == m_cInnerDeviceInfo.SMInitList.GetSize())
	{
		//重设已处理内挂扫码器数量
		m_cInnerDeviceInfo.HasProcessDeviceInitResultCount = 0;
		//获取执行结果
		WORD wTotal = m_cInnerDeviceInfo.SMStateList.GetSize();
		WORD wState = 0x00;

		WORD wInitCount = 0;
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(m_cInnerDeviceInfo.SMStateList.GetAt(i).IsInit)
			{
				wState |= m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID;
				++wInitCount;
			}
		}
		//通知调用者线程
		if(0 != m_nCallerThreadID)
		{
			tagSMInnerDeviceState* pState = NULL;
			pState = new tagSMInnerDeviceState();
			if(NULL != pState)
			{
				memset(pState,0,sizeof(tagSMInnerDeviceState));
				
				pState->ScanConsumeTime	= 0;
				pState->DeviceID		= 0;
				pState->ScanResult		= (wTotal == wInitCount)?0:1;

				CString strDesc;
				if(wTotal == wInitCount)
				{
					strDesc.Format(_T("扫描器初始化成功"));
				}
				else
				{
					strDesc.Format(_T("扫码器初始化异常(总数:%d,成功:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}
			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_INIT,0),(LPARAM)pState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForVer
//     
// 功能描述：设备内部协议处理函数 -- 处理版本读取指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForVer(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;
	if(bState)
	{
		CString strRealResult(strFormatData);
		strRealResult = strRealResult.Left(strRealResult.GetLength() - 8);
		bState = m_pSMIF->AnalysisVersionString(strRealResult);
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(byComIndex == m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID)
			{
				if(!m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsReadVer)
				{
					m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsReadVer = TRUE;
				}
				else
				{
					for(INT_PTR j = 0;j<m_cInnerDeviceInfo.SMStateList.GetSize();++j)
					{
						if(!m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsReadVer)
						{
							m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsReadVer = TRUE;
							break;
						}
					}
				}
				break;
			}
		}
	}

	if(++m_cInnerDeviceInfo.HasProcessDeviceVerResultCount == m_cInnerDeviceInfo.SMInitList.GetSize())
	{
		//重设已处理内挂扫码器数量
		m_cInnerDeviceInfo.HasProcessDeviceVerResultCount = 0;
		//获取执行结果
		WORD wTotal = m_cInnerDeviceInfo.SMStateList.GetSize();
		WORD wState = 0x00;

		WORD wInitCount = 0;
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(m_cInnerDeviceInfo.SMStateList.GetAt(i).IsReadVer)
			{
				wState |= m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID;
				++wInitCount;
			}
		}
		//通知调用者线程
		if(0 != m_nCallerThreadID)
		{
			tagSMInnerDeviceState* pState = NULL;
			pState = new tagSMInnerDeviceState();
			if(NULL != pState)
			{
				memset(pState,0,sizeof(tagSMInnerDeviceState));
				
				pState->ScanConsumeTime	= 0;
				pState->DeviceID		= 0;
				pState->ScanResult		= (wTotal == wInitCount)?0:1;

				CString strDesc;
				if(wTotal == wInitCount)
				{
					strDesc.Format(_T("扫描器版本信息读取成功"));
				}
				else
				{
					strDesc.Format(_T("扫码器版本信息读取异常(总数:%d,成功:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}

			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_VER,byComIndex),(LPARAM)pState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStartScan
//     
// 功能描述：设备内部协议处理函数 -- 处理开始扫码指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStartScan(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;

	CString strRevData(strFormatData);
	tagSMInnerDeviceState* pState = NULL;
	if(bState)
	{
		//截至计时
		::GetLocalTime(&m_stForEndScan);
		//获得支付平台二维码
		CString strFormatCode(strFormatData);
		
		pState = new tagSMInnerDeviceState();
		if(NULL != pState)
		{
			memset(pState,0,sizeof(tagSMInnerDeviceState));
			
			pState->ScanConsumeTime	= ::GetTickCount() - m_dwBeginScanTime;
			pState->DeviceID		= byComIndex;
			pState->ScanResult		= 0;
			pState->BeginTime		= m_stForBeginScan;
			pState->EndTime			= m_stForEndScan;
			_tcscpy_s(pState->Code,_countof(pState->Code),strFormatCode.GetBuffer(0));strFormatCode.ReleaseBuffer();
		}
	}
	//通知调用者线程
	if(0 != m_nCallerThreadID)
	{
		::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,byComIndex),(LPARAM)pState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStopScan
//     
// 功能描述：设备内部协议处理函数 -- 处理停止扫码指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStopScan(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForHeart
//     
// 功能描述：设备内部协议处理函数 -- 处理设备心跳指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForHeart(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	//判断心跳是否正常
	BOOL bCurrentDeviceState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;

	CString strRevData(strFormatData);
	tagSMInnerDeviceState* pState = NULL;
	pState = new tagSMInnerDeviceState();
	if(NULL != pState)
	{
		memset(pState,0,sizeof(tagSMInnerDeviceState));
		
		pState->ScanConsumeTime	= 0;
		pState->DeviceID		= byComIndex;
		pState->ScanResult		= bCurrentDeviceState?0:1;
		_tcscpy_s(pState->Code,_countof(pState->Code),strRevData.GetBuffer(0));strRevData.ReleaseBuffer();
	}

	//通知调用者线程
	if(0 != m_nCallerThreadID)
	{
		::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_HEART,byComIndex),(LPARAM)bCurrentDeviceState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForDefaultFactory
//     
// 功能描述：设备内部协议处理函数 -- 处理重设出厂默认环境参数指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForDefaultFactory(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;

	if(bState)
	{
		CString strRealResult(strFormatData);
		strRealResult = strRealResult.Left(strRealResult.GetLength() - 8);
		bState = m_pSMIF->AnalysisVersionString(strRealResult);
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(byComIndex == m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID)
			{
				if(!m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsSetFactory)
				{
					m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsSetFactory = TRUE;
				}
				else
				{
					for(INT_PTR j = 0;j<m_cInnerDeviceInfo.SMStateList.GetSize();++j)
					{
						if(!m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsSetFactory)
						{
							m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsSetFactory = TRUE;
							break;
						}
					}
				}
				break;
			}
		}
	}

	if(++m_cInnerDeviceInfo.HasProcessDeviceCommResultCount == m_cInnerDeviceInfo.SMInitList.GetSize())
	{
		//重设已处理内挂扫码器数量
		m_cInnerDeviceInfo.HasProcessDeviceCommResultCount = 0;
		//获取执行结果
		WORD wTotal = m_cInnerDeviceInfo.SMStateList.GetSize();
		WORD wState = 0x00;

		WORD wInitCount = 0;
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(m_cInnerDeviceInfo.SMStateList.GetAt(i).IsSetFactory)
			{
				wState |= m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID;
				++wInitCount;
			}
		}
		//通知调用者线程
		if(0 != m_nCallerThreadID)
		{
			tagSMInnerDeviceState* pState = NULL;
			pState = new tagSMInnerDeviceState();
			if(NULL != pState)
			{
				memset(pState,0,sizeof(tagSMInnerDeviceState));
				
				pState->ScanConsumeTime	= 0;
				pState->DeviceID		= 0;
				pState->ScanResult		= (wTotal == wInitCount)?0:1;

				CString strDesc;
				if(wTotal == wInitCount)
				{
					strDesc.Format(_T("扫描器恢复出厂设置成功"));
				}
				else
				{
					strDesc.Format(_T("扫码器恢复出厂设置异常(总数:%d,成功:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}

			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_RESET_FACTORY,0),(LPARAM)bState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForDefaultPersion
//     
// 功能描述：设备内部协议处理函数 -- 处理重设用户自定义环境指令返回数据
//     
// 输入参数：IN const BYTE& byComIndex		 -- 返回数据的设备关联串口号
//         ：IN const CString& strFormatData --	格式化后的协议数据
// 输出参数：void
// 编写人员：ROCY
// 编写时间：2016年3月2日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForDefaultPersion(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;

	if(bState)
	{
		CString strRealResult(strFormatData);
		strRealResult = strRealResult.Left(strRealResult.GetLength() - 8);
		bState = m_pSMIF->AnalysisVersionString(strRealResult);
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(byComIndex == m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID)
			{
				if(!m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsSetPersion)
				{
					m_cInnerDeviceInfo.SMStateList.ElementAt(i).IsSetPersion = TRUE;
				}
				else
				{
					for(INT_PTR j = 0;j<m_cInnerDeviceInfo.SMStateList.GetSize();++j)
					{
						if(!m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsSetPersion)
						{
							m_cInnerDeviceInfo.SMStateList.ElementAt(j).IsSetPersion = TRUE;
							break;
						}
					}
				}
				break;
			}
		}
	}

	if(++m_cInnerDeviceInfo.HasProcessDeviceCommResultCount == m_cInnerDeviceInfo.SMInitList.GetSize())
	{
		//重设已处理内挂扫码器数量
		m_cInnerDeviceInfo.HasProcessDeviceCommResultCount = 0;
		//获取执行结果
		WORD wTotal = m_cInnerDeviceInfo.SMStateList.GetSize();
		WORD wState = 0x00;

		WORD wInitCount = 0;
		for(INT_PTR i = 0;i<m_cInnerDeviceInfo.SMStateList.GetSize();++i)
		{
			if(m_cInnerDeviceInfo.SMStateList.GetAt(i).IsSetPersion)
			{
				wState |= m_cInnerDeviceInfo.SMStateList.GetAt(i).DeviceID;
				++wInitCount;
			}
		}
		//通知调用者线程
		if(0 != m_nCallerThreadID)
		{
			tagSMInnerDeviceState* pState = NULL;
			pState = new tagSMInnerDeviceState();
			if(NULL != pState)
			{
				memset(pState,0,sizeof(tagSMInnerDeviceState));
				
				pState->ScanConsumeTime	= 0;
				pState->DeviceID		= 0;
				pState->ScanResult		= (wTotal == wInitCount)?0:1;

				CString strDesc;
				if(wTotal == wInitCount)
				{
					strDesc.Format(_T("扫描器用户个性定制化设置成功"));
				}
				else
				{
					strDesc.Format(_T("扫码器用户个性定制化设置异常(总数:%d,成功:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}

			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_RESET_PERSION,0),(LPARAM)bState);
		}
	}
}


/*************************************【外部接口】*************************************/


////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ReadConfigInfoFromBoard
//     
// 功能描述：读取串口板配置
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ReadConfigInfoFromBoard(void)
{
	CString strSendData = ConstructFrameA2();
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::UpdateConfigInfoFromBoard
//     
// 功能描述：更新串口板配置
//     
// 输入参数：IN const CString& strConfigInfo
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::UpdateConfigInfoFromBoard(IN const CString& strConfigInfo)
{
	CString strSendData = ConstructFrameA1(strConfigInfo);
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::SetLightMode
//     
// 功能描述：设置亮灯模式
//     
// 输入参数：IN const int& iLightIndex
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SetLightMode(IN const int& iLightIndex)
{
	CString strSendData = ConstructFrameA3(iLightIndex);
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::GetLightMode
//     
// 功能描述：查询亮灯状态
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::GetLightMode(void)
{
	CString strSendData = ConstructFrameA4();
	return strSendData;
}






////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::GetLightMode
//     
// 功能描述：查询亮灯状态
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SetScanDisplay(tagScanDisplay	sDisplay)
{
	CString strSendData = ConstructFrameA5( sDisplay );
	return strSendData;
}







////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::ResetBoard
//     
// 功能描述：复位串口板
//     
// 输入参数：void
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月8日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ResetBoard(void)
{
	CString strSendData = ConstructFrameA6();
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::SendUserDefineCmd
//     
// 功能描述：发送自定义指令
//     
// 输入参数：IN const BYTE& byDeviceCmdType		--	设备协议指令类别，见枚举 SM_CMD_TYPE
//		   ：IN const CString& strCmdContent	--	设备命令内容
//         ：IN const BYTE& byDeviceIndex		--	设备索引(按位进行标记，为0标识通知所有设备)
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年3月3日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SendUserDefineCmd(IN const CString& strCmdContent,IN const BYTE& byDeviceIndex)
{
	CString strTemp(strCmdContent);
	CString strSendData;
	DWORD dwSize = strTemp.GetLength()/2 + 1;
	BYTE byData[1024] = {0};
	byData[0] = (0 == byDeviceIndex)?0x3F:byDeviceIndex;
	theApp.Hex2Bin(strTemp.GetBuffer(0),&byData[1],strTemp.GetLength());strTemp.ReleaseBuffer();
	if(SM_CMD_START == byData[2])
	{
		m_dwBeginScanTime = ::GetTickCount();
		::GetLocalTime(&m_stForBeginScan);
	}
	strSendData = ConstructFrameA5(byData,dwSize);

	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// 函数名称：CHTSMDeviceControl::SendCmdResponse
//     
// 功能描述：新增发送回馈指令
//     
// 输入参数：IN const BYTE& byResponseType
//         ：IN const BYTE& byResponseFrame
// 输出参数：const
// 编写人员：ROCY
// 编写时间：2016年8月29日
// 修改人员：
// 修改时间：
// 函数版本：1.0.0.1
// 备注说明：
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SendCmdResponse(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame)
{
	return ContructFrameC0(byResponseType,byResponseFrame);
}