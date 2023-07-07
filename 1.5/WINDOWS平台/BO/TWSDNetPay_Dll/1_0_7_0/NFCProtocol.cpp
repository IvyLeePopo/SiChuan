/*
 * Copyright(C) 2017,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�NFCProtocol.cpp
 * ժ    Ҫ��������Ŀ�ǽ�ģ��Э������࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2017��9��21��
 */
#include "StdAfx.h"
#include "NFCProtocol.h"
#include "TWSDNetPay_Dll.h"

CNFCProtocol::CNFCProtocol(void)
	:FramrType(0x00)
	,IsAyalyze(FALSE)
	,Head(0x00)
	,End(0x00)
	,Length(0x00)
	,Path(0x00)
	,LrcVerify(0x00)
	,TradeID(_T(""))
	,Context(_T(""))
	,MerchantHex(_T(""))
	,TerminalHex(_T(""))
	,CurrentConsumeErrorCode(0)
{
	//�̻������ն˺ų�ʼȡĬ��ֵ
	MerchantHex.Format(_T("38 39 38 33 31 30 31 37 33 39 39 30 38 37 37"));
	TerminalHex.Format(_T("37 30 30 38 30 32 33 35"));

	memset(&BinData[0],0,5120);
}

CNFCProtocol::~CNFCProtocol(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::Analyze
//     
// ��������������NFCģ�����������,���ؽ����ö������ UNIPAY_FRAME_TYPE
//     
// ���������IN const CString& strRevData -- �յ���Э������
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const int CNFCProtocol::Analyze(IN const CString& strRevData)
{
	FramrType = UFT_UNKNOW;
	try
	{
		CString strProcessData(strRevData);
		//����У��
		if(0 != (strRevData.GetLength()%2))
			throw -1;
		if(strRevData.GetLength()>5120)
			throw -2;
		//����ת��
		BYTE byRevData[5120]={0};
		GetGlobalApp()->Hex2Bin(strProcessData.GetBuffer(0),byRevData,strProcessData.GetLength());
		strProcessData.ReleaseBuffer();

		int iBinLen = strProcessData.GetLength()/2; 
		//��ȡ���ݰ���ͨ����
		Head		= byRevData[0];
		End			= byRevData[iBinLen-2];
		LrcVerify	= byRevData[iBinLen-1];
		Length		= MAKEWORD(byRevData[2],byRevData[1]);
		Path		= byRevData[3];
		TradeID		= strProcessData.Mid(10,12);
		Context		= strProcessData.Mid(22,strProcessData.GetLength() - 26 );
		memcpy(&BinData[0],&byRevData[11],iBinLen-13);

		//������ͨУ��
		if(0x02 != Head)
			throw -3;
		if(0x03 != End)
			throw -4;
		if(Length != (iBinLen - 5))
			throw -5;
		//����У��
		BYTE byCalcLRC = 0x00;
		for(int i = 0;i<iBinLen-2;++i)
		{
			byCalcLRC ^= byRevData[i+1];
		}
		if( byCalcLRC != LrcVerify)
			throw -6;
		//�������ݣ�·�ɷִ���
		switch(Path)
		{
		//��λ�����͸��ǽ�ģ��
		case 0x01:
			break;
		case 0x03:
			break;
		case 0x06:
			break;
		case 0x08:
			break;

		//�ǽ�ģ�����͸���λ��
		case 0x02:
			break;
		case 0x04:
			{
				//�������ͻ���
				if(   (0x57 == BinData[0])
					&&(0x31 == BinData[1])
					)
				{
					//��ȡ��������
					if(   (0x30 == BinData[3])
						&&(0x31 == BinData[4])
						)
					{
						//���п�
						FramrType = UFT_N_CONSUME_RES_W2;
					}
					else if(   (0x30 == BinData[3])
							 &&(0x32 == BinData[4])
							 )
					{
						//POSͨ
						FramrType = UFT_N_CONSUME_RES_W3;
					}
					//��ȡ�̻���
					int iTempCount = 0x00;
					for(int i = 0;i<15;++i)
					{
						iTempCount += BinData[6+i];
					}
					if(iTempCount>0)
					{
						memcpy(&MerchantBin[0],&BinData[6],15);
						MerchantHex.Empty();
						for(int i = 0;i<15;++i)
						{
							CString strTemp;
							strTemp.Format(_T("%02X "),MerchantBin[i]);
							if(i == 14)
							{
								strTemp.TrimRight();
							}
							MerchantHex += strTemp;
						}
					}
					//��ȡ�ն˺�
					iTempCount = 0x00;
					for(int i = 0;i<8;++i)
					{
						iTempCount += BinData[22+i];
					}
					if(iTempCount>0)
					{
						memcpy(&TerminalBin[0],&BinData[22],8);
						TerminalHex.Empty();
						for(int i = 0;i<8;++i)
						{
							CString strTemp;
							strTemp.Format(_T("%02X "),TerminalBin[i]);
							if(i == 7)
							{
								strTemp.TrimRight();
							}
							TerminalHex += strTemp;
						}
					}
					break;
				}
				//ACK
				if(0x06 == BinData[0])
				{
					//ǩ��ȷ�ϣ��ɹ���
					if(   (0x30 == BinData[2])
						&&(0x30 == BinData[3])
						)
					{
						switch(::GetGlobalApp()->GetUnipayWorkFlowState())
						{
						case UMS_SIGNING:
							FramrType = UFT_N_SIGN_CONFIRM;
							break;
						case UMS_TEST_LINK:
							FramrType = UFT_N_TEST_LINK_SUCCESS;
							break;
						}
						
						break;
					}
				}
				//NAK
				if(0x15 == BinData[0])
				{
					//if(0x31 == byRevData[6])
					//{
					//	FramrType = UFT_N_TEST_LINK_ERROR;
					//	break;
					//}
					switch(::GetGlobalApp()->GetUnipayWorkFlowState())
					{
					case UMS_SIGNING:
						FramrType = UFT_N_SIGN_VERIFY_ERR;
						break;
					case UMS_TEST_LINK:
						FramrType = UFT_N_TEST_LINK_ERROR;
						break;
					}
				}
				break;
			}
			break;
		case 0x05:
			{
				switch(BinData[0])
				{
				case 0x15://NAK
					{
						//��װ�������ݣ�ˢ�����쳣
						if(   (0x58 == BinData[2])
							&&(0x58 == BinData[3])
							)
						{
							FramrType = UFT_N_CONSUME_ERR;
							CurrentConsumeErrorCode = BinData[3];
							break;
						}
						//ģ��δǩ��
						if(   (0x41 == BinData[2])
							&&(0x34 == BinData[3])
							)
						{
							FramrType = UFT_N_CONSUME_NO_SIGN;
							break;
						}
						//ǩ��У�����
						if(   (0x58 == BinData[2])
							&&(0x35 == BinData[3])
							)
						{
							FramrType = UFT_N_SIGN_VERIFY_ERR;
							break;
						}
					}
					break;
				case 0x60://����
					{
						//��������
						if(   (0x02 == BinData[11])
							&&(0x00 == BinData[12])
							)
						{
							FramrType = UFT_N_CONSUME_RES_DATA;
							break;
						}
						//��������
						if(   (0x04 == BinData[11])
							&&(0x00 == BinData[12])
							)
						{
							FramrType = UFT_N_CORRECT_RES_DATA;
							break;
						}
						//ǩ������
						if(   (0x08 == BinData[11])
							&&(0x00 == BinData[12])
							)
						{
							FramrType = UFT_N_SIGN_IN_RES_DATA;
							break;
						}
					}
					break;
				}
			}
			break;
		case 0x07:
			{
				//������Ϣ����
				if(   (0x39 == BinData[0])
					&&(0x37 == BinData[1])
					)
				{
					FramrType = UFT_N_CONSUME_TIP;
					break;
				}
			}
			break;
		case 0x55:
			break;
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
		case -3:
			break;
		case -4:
			break;
		case -5:
			break;
		case -6:
			break;
		}
	}
	//�ж��Ƿ��Ѿ������ɹ�
	if( UFT_UNKNOW != FramrType)
		IsAyalyze = TRUE;

	return FramrType ;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructStopExecuteCmd
//     
// ��������������ִֹͣ��ָ��
//     
// ���������void
// ���������const CString
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructStopExecuteCmd(void)
{
	CString strCMD;
	strCMD.Format(_T("02 00 09 01 00 31 32 33 34 35 36 15 03 19"));
	strCMD.Replace(_T(" "),_T(""));
	return strCMD;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructTestLinkCmd
//     
// ������������������ָ��
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructTestLinkCmd(void)
{
	CString strCMD;
	strCMD.Format(_T("02 00 0A 01 01 31 32 33 34 35 36 39 39 03 0E"));
	strCMD.Replace(_T(" "),_T(""));
	return strCMD;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructStartSignInCmd
//     
// �������������췢��ǩ��ָ��
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructStartSignInCmd(void)
{
	CString strCMD;
	CString strTemp;
	strTemp.Format(_T("02 00 27 03 01 31 32 33 34 35 36 35 31 1C 30 31 1C %s 1C %s 1C 03"),MerchantHex,TerminalHex);
	//��ȡУ����
	BYTE byVerifyLRC = 0x00;
	BYTE byTemp[128] = {0};

	CString strCovTemp(strTemp);
	strCovTemp.Replace(_T(" "),_T(""));
	GetGlobalApp()->Hex2Bin(strCovTemp.GetBuffer(0),byTemp,strCovTemp.GetLength());
	strCovTemp.ReleaseBuffer();

	for(int i = 1;i<strCovTemp.GetLength()/2;++i)
	{
		byVerifyLRC ^= byTemp[i];
	}
	strCMD.Format(_T("%s %02X"),strTemp,byVerifyLRC);

	strCMD.Replace(_T(" "),_T(""));

	return strCMD;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructSignDataFromHttpCmd
//     
// ��������������HTTP������ǩ��ָ��
//     
// ���������IN const CString& strContext
// ���������const CString
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructSignDataFromHttpCmd(IN const CString& strContext)
{
	CString strCMD;
	//���㳤��
	WORD wCmdLen = strContext.GetLength()/2 + 2 + 6;
	CString strLen;
	strLen.Format(_T("%02X %02X"),HIBYTE(wCmdLen),LOBYTE(wCmdLen));

	CString strTemp;
	strTemp.Format(_T("02 %s 06 01 31 32 33 34 35 36 %s 03"),strLen,strContext);
	//��ȡУ����
	BYTE byVerifyLRC = 0x00;
	BYTE byTemp[5120] = {0};

	CString strCovTemp(strTemp);
	strCovTemp.Replace(_T(" "),_T(""));
	GetGlobalApp()->Hex2Bin(strCovTemp.GetBuffer(0),byTemp,strCovTemp.GetLength());
	strCovTemp.ReleaseBuffer();

	for(int i = 1;i<strCovTemp.GetLength()/2;++i)
	{
		byVerifyLRC ^= byTemp[i];
	}
	strCMD.Format(_T("%s %02X"),strTemp,byVerifyLRC);

	strCMD.Replace(_T(" "),_T(""));

	return strCMD;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructStartConsumeCmd
//     
// �������������췢������ָ��
//     
// ���������IN const int& iMoney
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructStartConsumeCmd(IN const int& iMoney)
{
	CString strCMD;
	CString strOrg1;
	CString strTemp;
	strOrg1.Format(_T("02 00 18 03 00 31 32 33 34 35 36 57 31 1C"));
	CString strMoney;
	strMoney.Format(_T("%012d"),iMoney);
	BYTE byMoney[12]={0};
	memcpy(&byMoney[0],strMoney.GetBuffer(0),strMoney.GetLength());strMoney.ReleaseBuffer();
	strMoney = GetGlobalApp()->Bin2Hex(byMoney,12);
	
	strTemp.Format(_T("%s %s 1C 03"),strOrg1,strMoney);
	//������ָ�����У��
	BYTE byVerifyCode = 0x00;

	BYTE byCmdBuf[1024]={0};
	CString strCovTemp(strTemp);
	strCovTemp.Replace(_T(" "),_T(""));
	GetGlobalApp()->Hex2Bin(strCovTemp.GetBuffer(0),byCmdBuf,strCovTemp.GetLength());
	strCovTemp.ReleaseBuffer();

	for(int i = 1;i<strCovTemp.GetLength()/2;++i)
    {
        byVerifyCode ^= byCmdBuf[i];
    }

	strCMD.Format(_T("%s %s 1C 03 %02X"),strOrg1,strMoney,byVerifyCode);

	strCMD.Replace(_T(" "),_T(""));
	return strCMD;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructExecuteConsumeCmd
//     
// ��������������ִ�о�����������ָ��
//     
// ���������IN const int& iConsumeType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructExecuteConsumeCmd(IN const int& iConsumeType)
{
	CString strCMD;
	CString strOrg1;
	CString strTemp;

	BYTE byConsumeType = 0x00;
	if(0x01 == iConsumeType)
	{
		byConsumeType = 0x32;
	}
	else
	{
		byConsumeType = 0x33;
	}

	strTemp.Format(_T("02 00 24 03 00 31 32 33 34 35 36 57 %02X 1C %s 1C %s 1C 03"),byConsumeType,MerchantHex,TerminalHex);
	//������ָ�����У��
	BYTE byVerifyCode = 0x00;

	BYTE byCmdBuf[1024]={0};
	CString strCovString(strTemp);
	strCovString.Replace(_T(" "),_T(""));
	GetGlobalApp()->Hex2Bin(strCovString.GetBuffer(0),byCmdBuf,strCovString.GetLength());strCovString.ReleaseBuffer();

	for(int i = 1;i<strTemp.GetLength()/2;++i)
    {
        byVerifyCode ^= byCmdBuf[i];
    }

	strCMD.Format(_T("%s %02X"),strTemp,byVerifyCode);

	strCMD.Replace(_T(" "),_T(""));

	return strCMD;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CNFCProtocol::ConstructCorrectCmd
//     
// �����������������ָ��
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��21��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CNFCProtocol::ConstructCorrectCmd(void)
{
	CString strCMD;
	CString strTemp;
	//strTemp.Format(_T("02 00 24 03 01 31 32 33 34 35 36 38 33 1C 30 31 1C %s 1C %s 1C 03"),MerchantHex,TerminalHex);
	strTemp.Format(_T("02 00 1C 03 01 31 32 33 34 35 36 38 33 1C %s 1C 1C 03"),MerchantHex);
	//��ȡУ����
	BYTE byVerifyLRC = 0x00;
	BYTE byTemp[128] = {0};
	CString strLRCString = strTemp.Right(strTemp.GetLength() - 2);

	strLRCString.Replace(_T(" "),_T(""));
	GetGlobalApp()->Hex2Bin(strLRCString.GetBuffer(0),byTemp,strLRCString.GetLength());
	for(int i = 0;i<strLRCString.GetLength()/2;++i)
	{
		byVerifyLRC ^= byTemp[i];
		/*
		CString tmp;
		tmp.Format(_T("correct:%02X"),byVerifyLRC);
		theApp.RecordLog(tmp);
		*/
	}
	strCMD.Format(_T("%s %02X"),strTemp,byVerifyLRC);

	strCMD.Replace(_T(" "),_T(""));

	return strCMD;
}