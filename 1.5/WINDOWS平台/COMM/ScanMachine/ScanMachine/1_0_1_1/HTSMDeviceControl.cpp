/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�HTSMProtocol.cpp
 * ժ    Ҫ��ɨ�����ڲ�ͨѶЭ������࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��2��22��
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
// �������ƣ�CHTSMDeviceControl::AnalyseReciveData
//     
// �����������������յ������ݣ��ж��Ƿ����Э��
//     
// ���������IN OUT BYTE* lpData			-- ���յ��Ĵ�������ָ��
//         ��IN const int& iDataSize	-- ��Ҫ���������ݳ���
//         ��IN OUT  LPBYTE& pBegin		-- ֡ͷλ��
//         ��IN OUT  LPBYTE& pEnd		-- ֡βλ��
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::AnalyseReciveData(IN OUT LPBYTE lpData,IN const int& iDataSize,IN OUT LPBYTE& pBegin,IN OUT LPBYTE& pEnd)
{
	pBegin = NULL;
	pEnd   = NULL;
	//֡ͷ�������
	BOOL bFindBegin = FALSE;
	//֡β�������
	BOOL bFindEnd   = FALSE;
	//���ü�������֡���Ϊ��
	m_bIsHaveFrameFlag = FALSE;
	
	// �����ݴ�СΪ1,�ж��Ƿ�Ϊ֡ͷ
	if( 1 == iDataSize )
	{
		if ( SM_PROTOCOL_HEADER == lpData[0])
		{
			pBegin = lpData;
		}
		return;
	}
	
	// ������������֡β��֡ͷ
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

	pEnd = NULL;// ��û���ҵ�֡ͷ,�ҵ���֡βû������,��֡β�ÿ�
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::GetDataFrameType
//     
// �������������ݽ��յ��������ж�����֡
//     
// ���������IN const LPBYTE lpFrame
//         ��IN const DWORD& dwSize
//         ��IN OUT CString& strReceiveFrameType
// ���������const int
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CHTSMDeviceControl::InitInnerDevice
//     
// ������������ʼ���豸
//     
// ���������IN const int& iInnerDeviceType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		strLog.Format(_T("ɨ������߳�,������ɨ�����ͺ�Ϊ[����Τ��3310G]"));
		break;
	} 
	if(NULL != m_pSMIF)
		bReturn = TRUE;
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::IsAllowAnalyseReceiveDataOnBytesNum
//     
// �������������յ������ݳ����Ƿ������������֡У��
//     
// ���������IN const DWORD& dwHadReceiveDataSize
// ���������inline
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::IsAllowAnalyseReceiveDataOnBytesNum(IN const DWORD& dwHadReceiveDataSize) const
{
	return dwHadReceiveDataSize >= 2;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::VerifyFrameData
//     
// ��������������֡У��,ͬʱ�����ݽ���ת�崦��
//     
// ���������IN const LPBYTE lpData	--	����֡����
//         ��IN const DWORD& dwSize	--	����֡����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����CRCУ�飬����֡Ϊδ����ת�봦����ԭʼ���ݡ�
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::VerifyFrameData(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	BOOL bReturn = FALSE;
	BYTE byBuff[512] = {0};
	if(NULL != lpData)
	{
		memcpy(byBuff,lpData,dwSize);

		//����ת��
		DWORD dwRealDataLen = dwSize;
		CovertData(byBuff,dwRealDataLen,FALSE);

		//��ȡ֡����
		BYTE byFrameType = byBuff[1];
		//��ȡ����������
		WORD wDataContentLen = MAKEWORD(byBuff[3],byBuff[2]);
		//��ȡ����֡��CRCУ����
		WORD wOrgCRCCode = MAKEWORD(byBuff[dwRealDataLen-2],byBuff[dwRealDataLen-3]);
		//��ȡ�����CRCУ����
		WORD wCalcCRCCode = GetVerifyCode(byFrameType,wDataContentLen,&byBuff[4]);

		//�ж��Ƿ�һ��
		bReturn = (wOrgCRCCode == wCalcCRCCode)?TRUE:FALSE;

		if(!bReturn && (dwSize>100))
			bReturn = TRUE;
	}
	
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA1
//     
// ��������������A1֡ -- �����������
//     
// ���������IN const CString& strParam
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA1(IN const CString& strParam)
{
	CString strFrameHexData;
	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA2
//     
// ��������������A2֡ -- ��ȡ����������Ϣ
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA2(void)
{
	CString strFrameHexData;
	
	BYTE bySendData[512] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A2;
	//���ݳ���
	bySendData[2] = 0x00;
	bySendData[3] = 0x00;
	//У����
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[3]);
	bySendData[4] = HIBYTE(wVerifyCode);
	bySendData[5] = LOBYTE(wVerifyCode);
	//Э��β
	bySendData[6] = SM_PROTOCOL_END;

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData,7);

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA3
//     
// ��������������A3֡ -- ��������״̬
//     
// ���������IN const int& iLightIndex	-- ��������������0,ȫ��;1,�ȴ�ɨ��;2,ɨ��ʧ��;3,ɨ��ɹ�)
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA3(IN const int& iLightIndex)
{
	CString strFrameHexData;

	BYTE bySendData[8] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A3;
	//���ݳ���
	bySendData[2] = 0x00;
	bySendData[3] = 0x01;
	//��ȡ��������
	WORD dwData = 0x0000;
	BYTE byTemp = 0x01;

	dwData = byTemp<<iLightIndex;

//	bySendData[4] = HIBYTE(dwData);
	bySendData[4] = LOBYTE(dwData);
	//У����
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[4]);
	bySendData[5] = HIBYTE(wVerifyCode);
	bySendData[6] = LOBYTE(wVerifyCode);
	//Э��β
	bySendData[7] = SM_PROTOCOL_END;

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData,8);
	//����IO����ָ�����Ҫ��A5ָ�����
	m_strLastSendFrameData.Empty();

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA4
//     
// ��������������A4֡	-- ��ȡ����״̬
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA4(void)
{
	CString strFrameHexData;

	BYTE bySendData[512] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A4;
	//���ݳ���
	bySendData[2] = 0x00;
	bySendData[3] = 0x00;
	//У����
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[3]);
	bySendData[4] = HIBYTE(wVerifyCode);
	bySendData[5] = LOBYTE(wVerifyCode);
	//Э��β
	bySendData[6] = SM_PROTOCOL_END;

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData,7);
	//����IO����ָ�����Ҫ��A5ָ�����
	m_strLastSendFrameData.Empty();

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA5
//     
// ��������������A5֡	-- ת�����ݸ��ƶ�ɨ���豸
//     
// ���������IN const LPBYTE lpData -- ��Ҫ���͵�����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA5(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	CString strFrameHexData;
	//��ȡУ����
	BYTE byTempBuff[512] = {0};
	memcpy(byTempBuff,lpData,dwSize);
	WORD wVerifyCode = GetVerifyCode(SM_PROTOCOL_FRAME_TYPE_A5,dwSize,byTempBuff);

	BYTE bySendData[1024] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A5;
	//���ݳ���
	bySendData[2] = HIBYTE(LOWORD(dwSize));
	bySendData[3] = LOBYTE(LOWORD(dwSize));

	//��ȡ��������
	memcpy(&bySendData[4],&byTempBuff[0],dwSize);

	//У����
	bySendData[4+dwSize]		= HIBYTE(wVerifyCode);
	bySendData[4+dwSize + 1]	= LOBYTE(wVerifyCode);
	//Э��β
	bySendData[4+dwSize + 2]	= SM_PROTOCOL_END;

	//��������ת��
	DWORD dwRealDataLen = 7+dwSize;
	CovertData(bySendData,dwRealDataLen,TRUE);

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData,dwRealDataLen);
	//���浱ǰ���͵�����
	m_strLastSendFrameData = strFrameHexData;

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA5
//     
// ��������������A5֡	-- ������������ɨ��Ӳ����ʾ
//     
// ���������tagScanDisplay sDisplay -- ��ʾ���ݽṹ��
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA5(tagScanDisplay sDisplay)
{
	CString strFrameHexData = _T("");
	WORD	wSzie = 0;
	WORD wVerifyCode = 0;

	BYTE byTempBuff[512] = {0};
	BYTE bySendData[1024] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A5;

	if( 2 == sDisplay.Type )//ֻ��ʱ���ֶ���ʾ
	{
		//	wSzie = 0x0008;

		wSzie = 0x0012;
		//����֡����
		bySendData[2] = HIBYTE( wSzie );
		bySendData[3] = LOBYTE( wSzie );

		//�ڹ������
		bySendData[4] = 0x00;
		bySendData[5] = 0x56;

		//�ڲ����ݳ���
		bySendData[6] = 0x00;
		//bySendData[7] = 0x04;
		//��ǰʱ���Unix��ʽ
		//DWORD dwTime = sDisplay.tShowtime;
		//bySendData[9]	= HIBYTE(HIWORD(dwTime));
		//bySendData[10]	= LOBYTE(HIWORD(dwTime));
		//bySendData[11]	= HIBYTE(LOWORD(dwTime));
		//bySendData[12]	= LOBYTE(LOWORD(dwTime));
		//����У����
		//wVerifyCode = GetVerifyCode(bySendData[1], wSzie, &bySendData[4]);
		//������ʱ����(14)
		bySendData[7] = 0x0E;

		
		//��ȡ������ʱ����
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

		//����У����
		wVerifyCode = GetVerifyCode(bySendData[1], wSzie, &bySendData[4]);		
	}
	else//if( 1 == sDisplay.Type )//Ĭ����ʾ���ͣ����������
	{
		BYTE byDataMode = 0x01;
		//�ڹ������
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

			//����֡����
			bySendData[2] = HIBYTE( wSzie );
			bySendData[3] = LOBYTE( wSzie );

			//�ڲ����ݳ���
			bySendData[6] = 0x00;
			bySendData[7] = 0x2D;

			//�ۿ��	1~4 �ͳ�		11~15 ����
			BYTE bVType = 0;
			if( 2 == sDisplay.VehicleType)//����	1��2��
			{
				bVType = 10 + sDisplay.VehicleClass;
			}
			else//if( 1 == sDisplay.VehicleType)	//Ĭ�Ͽͳ�
			{
				bVType = sDisplay.VehicleClass;
			}
			bySendData[8] = bVType;

			//��������	4�ֽ����ͣ�������ǰ�������ں󣬵�λ������
			DWORD dwWeight = sDisplay.Weight;
			bySendData[9]	= HIBYTE(HIWORD(dwWeight));
			bySendData[10]	= LOBYTE(HIWORD(dwWeight));
			bySendData[11]	= HIBYTE(LOWORD(dwWeight));
			bySendData[12]	= LOBYTE(LOWORD(dwWeight));

			//�ۿ���	4�ֽ����ͣ�������ǰ�������ں󣬵�λ����
			DWORD dwMoney = sDisplay.Money;
			bySendData[13]	= HIBYTE(HIWORD(dwMoney));
			bySendData[14]	= LOBYTE(HIWORD(dwMoney));
			bySendData[15]	= HIBYTE(LOWORD(dwMoney));
			bySendData[16]	= LOBYTE(LOWORD(dwMoney));

			//����	4�ֽ����ͣ�������ǰ�������ں�
			DWORD dwLimit = sDisplay.LimitWeight;
			bySendData[17]	= HIBYTE(HIWORD(dwLimit));
			bySendData[18]	= LOBYTE(HIWORD(dwLimit));
			bySendData[19]	= HIBYTE(LOWORD(dwLimit));
			bySendData[20]	= LOBYTE(LOWORD(dwLimit));

			//����	4�ֽ����ͣ�������ǰ�������ں�
			DWORD dwAxisCount = sDisplay.AxisCount;
			bySendData[21]	= HIBYTE(HIWORD(dwAxisCount));
			bySendData[22]	= LOBYTE(HIWORD(dwAxisCount));
			bySendData[23]	= HIBYTE(LOWORD(dwAxisCount));
			bySendData[24]	= LOBYTE(LOWORD(dwAxisCount));
			
			//���	4�ֽ����ͣ�������ǰ�������ں�
			DWORD dwBalance = sDisplay.Balance ;
			bySendData[25]	= HIBYTE(HIWORD(dwBalance));
			bySendData[26]	= LOBYTE(HIWORD(dwBalance));
			bySendData[27]	= HIBYTE(LOWORD(dwBalance));
			bySendData[28]	= LOBYTE(LOWORD(dwBalance));

			//���վ��
			memcpy(&bySendData[29],&sDisplay.EntrySTName[0],24);
		}
		else
		{
			wSzie = 0x0002;

			//����֡����
			bySendData[2] = HIBYTE( wSzie );
			bySendData[3] = LOBYTE( wSzie );
		}

		//����У����
		wVerifyCode = GetVerifyCode(bySendData[1], wSzie, &bySendData[4]);		
	}

	//У����
	bySendData[4+wSzie]		= HIBYTE(wVerifyCode);
	bySendData[4+wSzie + 1]	= LOBYTE(wVerifyCode);

	//Э��β
	bySendData[4+wSzie + 2]	= SM_PROTOCOL_END;

	//��������ת��
	DWORD dwRealDataLen = 7+wSzie;
	CovertData(bySendData, dwRealDataLen, TRUE);

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData, dwRealDataLen);

	BOOL bNeedSend = TRUE;
	//���������ĳ�������/�����뿪ָ�������������Ƿ���Ҫ����
	if( (0x59 == bySendData[5]) || (0x60 == bySendData[5]) )
	{
		//����ȷ������������·���Ȧ״ָ̬��
		if(0 == theApp.m_iNeedNotifyLoopState)
		{
			bNeedSend = FALSE;
		}
	}

	if(bNeedSend)
	{
		//���浱ǰ���͵�����
		m_strLastSendFrameData = strFrameHexData;
	}
	else
	{
		strFrameHexData.Empty();
	}

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ConstructFrameA8
//     
// ��������������A8֡	-- ��λ�豸
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ConstructFrameA6(void)
{
	CString strFrameHexData;

	BYTE bySendData[512] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_A6;
	//���ݳ���
	bySendData[2] = 0x00;
	bySendData[3] = 0x00;
	//У����
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[4]);
	bySendData[4] = HIBYTE(wVerifyCode);
	bySendData[5] = LOBYTE(wVerifyCode);
	//Э��β
	bySendData[6] = SM_PROTOCOL_END;

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData,7);

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ContructFrameC0
//     
// ��������������C0֡ -- ָ��Ӧ��֡
//     
// ���������IN const BYTE& byResponseType
//         ��IN const BYTE& byResponseFrame
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��29��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ContructFrameC0(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame)
{
	CString strFrameHexData;

	BYTE bySendData[512] = {0};
	//Э��ͷ
	bySendData[0] = SM_PROTOCOL_HEADER;
	//��������
	bySendData[1] = SM_PROTOCOL_FRAME_TYPE_C0;
	//���ݳ���
	bySendData[2] = 0x00;
	bySendData[3] = 0x02;
	//����֡��
	bySendData[4] = byResponseFrame;
	//ִ��״̬
	bySendData[5] = byResponseType;
	//У����
	WORD wVerifyCode = GetVerifyCode(bySendData[1],MAKEWORD(bySendData[3],bySendData[2]),&bySendData[4]);
	bySendData[6] = HIBYTE(wVerifyCode);
	bySendData[7] = LOBYTE(wVerifyCode);
	//Э��β
	bySendData[8] = SM_PROTOCOL_END;

	//ת����16�����ַ���
	strFrameHexData = theApp.Bin2Hex(bySendData,9);

	return strFrameHexData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ProcessReceiveFrameData
//     
// ���������������������
//     
// ���������IN const LPBYTE lpData	--	����֡����
//         ��IN const DWORD& dwSize --	����֡����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CHTSMDeviceControl::ProcessReceiveFrameData(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	BOOL bReturn  = TRUE;

	//У������
	if(VerifyFrameData(lpData,dwSize))
	{
		CString strFrameType;
		switch(GetDataFrameType(lpData,dwSize,strFrameType))
		{
		case SM_PROTOCOL_FRAME_TYPE_B0:	//��ʼ����Ϣ֡
			ProcessFrameB0(lpData,dwSize);
			break;
		case SM_PROTOCOL_FRAME_TYPE_B1:	//����֡
			ProcessFrameB1(lpData,dwSize);
			break;
		case SM_PROTOCOL_FRAME_TYPE_B2:	//�����ϴ���Ϣ֡
			ProcessFrameB2(lpData,dwSize);
			break;
		case SM_PROTOCOL_FRAME_TYPE_C0:	//ָ��Ӧ��֡
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
// �������ƣ�CHTSMDeviceControl::ProcessFrameB0
//     
// ��������������B0֡
//     
// ���������IN const LPBYTE lpData
//         ��IN const DWORD& dwSize
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameB0(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	if(NULL != lpData)
	{
		//��ȡ����
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//��ȡ����������
		BYTE byDataSection[512]={0};
		memcpy(&byDataSection[0],&lpData[0],wDataLen);
		//��ʼ����
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ProcessFrameB1
//     
// ��������������B1֡
//     
// ���������IN const LPBYTE lpData
//         ��IN const DWORD& dwSize
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameB1(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	if(NULL != lpData)
	{
		//��ȡ����
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//��ȡ����������
		BYTE byDataSection[512]={0};
		memcpy(&byDataSection[0],&lpData[0],wDataLen);
		//��ʼ����
		m_tLastHeartTimeForBoard = time(NULL) ;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ProcessFrameB2
//     
// ��������������B2֡
//     
// ���������IN const LPBYTE lpData
//         ��IN const DWORD& dwSize
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameB2(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	try
	{
		if(NULL == lpData)
			throw -1;
		//��ȡ����
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//��ȡ����������
		BYTE byDataSection[512]={0};
		memcpy(&byDataSection[0],&lpData[4],wDataLen);
		//ȥ��ǰ��׺
		//��û������ݵĴ�������
		BYTE byDeviceID = byDataSection[0];
		//�����Ӧ������֡���
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
			//�����Ӧ���ڹ��豸ָ���ʶ
			BYTE byResponseDeviceCommandType = byDataSection[2];
			//����ڹ��豸ָ��Э���ԭʼ����
			BYTE byInnerProtocolContent[512]={0};
			DWORD dwInnerProtocolLen = wDataLen - 3;
			memcpy(&byInnerProtocolContent[0],&byDataSection[3],dwInnerProtocolLen);
			
			//�����ڹ��豸��ָ��Э����н���
			CHTSMDeviceBaseIF* pSMIF = GetInnerDeviceProtocolClass();
			if(NULL == pSMIF)
				throw -2;
			////���ɨ�����ڲ�Э����״̬
			//if(!pSMIF->CheckCommandExecuteResult(byInnerProtocolContent,dwInnerProtocolLen))
			//	throw -3;
			//��ʽ������
			CString strFormatCode = pSMIF->FormatReadData(byInnerProtocolContent,dwInnerProtocolLen);
			//������
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
			//�����ϴε���������
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
// �������ƣ�CHTSMDeviceControl::ProcessFrameC0
//     
// ��������������C0֡
//     
// ���������IN const LPBYTE lpData
//         ��IN const DWORD& dwSize
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::ProcessFrameC0(IN const LPBYTE lpData,IN const DWORD& dwSize)
{
	m_bNeedReSendLastData = FALSE;
	try
	{
		if(NULL == lpData)
			throw -1;
		//��ȡ����
		WORD wDataLen = MAKEWORD(lpData[3],lpData[2]);
		//��ȡ��Ӧ֡���
		BYTE byFrameType = lpData[4];
		//��ȡִ��״̬
		BYTE byOpState   = lpData[5];
		//���״̬���طǷ��������ϴη���ָ��ΪA5֡�������Ҫ�����ط�����
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
// �������ƣ�CHTSMDeviceControl::CovertData
//     
// ��������������ת��
//     
// ���������IN OUT LPBYTE lpData		--	ԭʼ/ת��� ��������
//         ��IN OUT DWORD& dwSize		--	ԭʼ/ת��� ���ݳ���
//         ��IN const BOOL& bSendState	--	ת����յ������ݻ�ת�뷢�͵�����
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::CovertData(IN OUT LPBYTE lpData,IN OUT DWORD& dwSize,IN const BOOL& bSendState)
{
	BYTE byTemp[2048]={0};
	int iIndex=0;

	if(dwSize>2048)
		return;
	
	::memcpy(byTemp,lpData,dwSize);
	
	//����֡ͷ
	lpData[0]=SM_PROTOCOL_HEADER;
	//��������֡��������
	iIndex=1;
	for(DWORD i=1;i<(dwSize-1);i++)
	{
		if(!bSendState)//��������
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
		else		//��������
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
	//����֡β
	lpData[iIndex++] = SM_PROTOCOL_END;
	//������ݳ���
	dwSize = iIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::GetVerifyCode
//     
// ������������ȡУ����
//     
// ���������IN const LPBYTE lpData	--	��ҪУ�����������
//         ��IN const DWORD& dwSize --  ��ҪУ������ݳ���
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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

		//У��������ϵ�����
		BYTE byVerifyDataSection[1024] = {0};
		//У��������ϵĳ���
		DWORD dwVerifyDataLen = 0x00;

		byVerifyDataSection[dwVerifyDataLen++] = byFrameType;

		byVerifyDataSection[dwVerifyDataLen++] = HIBYTE(wSize);
		byVerifyDataSection[dwVerifyDataLen++] = LOBYTE(wSize);

		memcpy(&byVerifyDataSection[dwVerifyDataLen],&byBuff[0],wSize);
		dwVerifyDataLen += wSize;

		//��ȡ���¼����CRC��
		wCalcCRCCode = theApp.CRC16(byVerifyDataSection,dwVerifyDataLen);
	}
	return wCalcCRCCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForInit
//     
// �����������豸�ڲ�Э�鴦���� -- �����ʼ��ָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		//�����Ѵ����ڹ�ɨ��������
		m_cInnerDeviceInfo.HasProcessDeviceInitResultCount = 0;
		//��ȡִ�н��
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
		//֪ͨ�������߳�
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
					strDesc.Format(_T("ɨ������ʼ���ɹ�"));
				}
				else
				{
					strDesc.Format(_T("ɨ������ʼ���쳣(����:%d,�ɹ�:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}
			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_INIT,0),(LPARAM)pState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForVer
//     
// �����������豸�ڲ�Э�鴦���� -- ����汾��ȡָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		//�����Ѵ����ڹ�ɨ��������
		m_cInnerDeviceInfo.HasProcessDeviceVerResultCount = 0;
		//��ȡִ�н��
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
		//֪ͨ�������߳�
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
					strDesc.Format(_T("ɨ�����汾��Ϣ��ȡ�ɹ�"));
				}
				else
				{
					strDesc.Format(_T("ɨ�����汾��Ϣ��ȡ�쳣(����:%d,�ɹ�:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}

			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_VER,byComIndex),(LPARAM)pState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStartScan
//     
// �����������豸�ڲ�Э�鴦���� -- ����ʼɨ��ָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStartScan(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;

	CString strRevData(strFormatData);
	tagSMInnerDeviceState* pState = NULL;
	if(bState)
	{
		//������ʱ
		::GetLocalTime(&m_stForEndScan);
		//���֧��ƽ̨��ά��
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
	//֪ͨ�������߳�
	if(0 != m_nCallerThreadID)
	{
		::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_START,byComIndex),(LPARAM)pState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStopScan
//     
// �����������豸�ڲ�Э�鴦���� -- ����ֹͣɨ��ָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForStopScan(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	BOOL bState = (0 != strFormatData.CompareNoCase(_T("FAILED")))?TRUE:FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForHeart
//     
// �����������豸�ڲ�Э�鴦���� -- �����豸����ָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForHeart(IN const BYTE& byComIndex,IN const CString& strFormatData)
{
	//�ж������Ƿ�����
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

	//֪ͨ�������߳�
	if(0 != m_nCallerThreadID)
	{
		::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_HEART,byComIndex),(LPARAM)bCurrentDeviceState);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForDefaultFactory
//     
// �����������豸�ڲ�Э�鴦���� -- �����������Ĭ�ϻ�������ָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		//�����Ѵ����ڹ�ɨ��������
		m_cInnerDeviceInfo.HasProcessDeviceCommResultCount = 0;
		//��ȡִ�н��
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
		//֪ͨ�������߳�
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
					strDesc.Format(_T("ɨ�����ָ��������óɹ�"));
				}
				else
				{
					strDesc.Format(_T("ɨ�����ָ����������쳣(����:%d,�ɹ�:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}

			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_RESET_FACTORY,0),(LPARAM)bState);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::fnInnerDeviceProtocolProcessResultForDefaultPersion
//     
// �����������豸�ڲ�Э�鴦���� -- ���������û��Զ��廷��ָ�������
//     
// ���������IN const BYTE& byComIndex		 -- �������ݵ��豸�������ں�
//         ��IN const CString& strFormatData --	��ʽ�����Э������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
		//�����Ѵ����ڹ�ɨ��������
		m_cInnerDeviceInfo.HasProcessDeviceCommResultCount = 0;
		//��ȡִ�н��
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
		//֪ͨ�������߳�
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
					strDesc.Format(_T("ɨ�����û����Զ��ƻ����óɹ�"));
				}
				else
				{
					strDesc.Format(_T("ɨ�����û����Զ��ƻ������쳣(����:%d,�ɹ�:%d)"),wTotal,wInitCount);
				}
				strcpy_s(pState->StateDesc,_countof(pState->StateDesc),strDesc.GetBuffer(0));strDesc.ReleaseBuffer();

			}

			::PostThreadMessage(m_nCallerThreadID,WM_THREAD_SM_DEAL_INNER_DEVICE_RESPONSE_INFO,(WPARAM)MAKELONG(SM_CMD_RESET_PERSION,0),(LPARAM)bState);
		}
	}
}


/*************************************���ⲿ�ӿڡ�*************************************/


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ReadConfigInfoFromBoard
//     
// ������������ȡ���ڰ�����
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ReadConfigInfoFromBoard(void)
{
	CString strSendData = ConstructFrameA2();
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::UpdateConfigInfoFromBoard
//     
// �������������´��ڰ�����
//     
// ���������IN const CString& strConfigInfo
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::UpdateConfigInfoFromBoard(IN const CString& strConfigInfo)
{
	CString strSendData = ConstructFrameA1(strConfigInfo);
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::SetLightMode
//     
// ������������������ģʽ
//     
// ���������IN const int& iLightIndex
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SetLightMode(IN const int& iLightIndex)
{
	CString strSendData = ConstructFrameA3(iLightIndex);
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::GetLightMode
//     
// ������������ѯ����״̬
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::GetLightMode(void)
{
	CString strSendData = ConstructFrameA4();
	return strSendData;
}






////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::GetLightMode
//     
// ������������ѯ����״̬
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SetScanDisplay(tagScanDisplay	sDisplay)
{
	CString strSendData = ConstructFrameA5( sDisplay );
	return strSendData;
}







////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::ResetBoard
//     
// ������������λ���ڰ�
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��8��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::ResetBoard(void)
{
	CString strSendData = ConstructFrameA6();
	return strSendData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceControl::SendUserDefineCmd
//     
// ���������������Զ���ָ��
//     
// ���������IN const BYTE& byDeviceCmdType		--	�豸Э��ָ����𣬼�ö�� SM_CMD_TYPE
//		   ��IN const CString& strCmdContent	--	�豸��������
//         ��IN const BYTE& byDeviceIndex		--	�豸����(��λ���б�ǣ�Ϊ0��ʶ֪ͨ�����豸)
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��3��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
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
// �������ƣ�CHTSMDeviceControl::SendCmdResponse
//     
// �����������������ͻ���ָ��
//     
// ���������IN const BYTE& byResponseType
//         ��IN const BYTE& byResponseFrame
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��29��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceControl::SendCmdResponse(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame)
{
	return ContructFrameC0(byResponseType,byResponseFrame);
}