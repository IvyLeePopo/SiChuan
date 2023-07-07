/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�SMHoneywell3301g.cpp
 * ժ    Ҫ������Τ��3310gɨ����Э�������࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
#include "StdAfx.h"
#include "HTSMHoneywell3310g.h"
#include "ScanMachine.h"
extern CScanMachineApp theApp;

CHTSMDeviceHW3310g::CHTSMDeviceHW3310g(IN const int& iSMType)
	:CHTSMDeviceBaseIF(iSMType)
	,m_strCommandHead(_T("164D0D"))
	
	,m_iScanCodeType(0)
{
	m_dwCodeLen = 18;

	TCHAR szCurrent[256]={0};
	::GetCurrentDirectory(256,szCurrent);
	CString strConfigFile;
	strConfigFile.Format(_T("%s\\HTSMConfig1.ini"),szCurrent);

	m_iScanCodeType = ::GetPrivateProfileInt(_T("SMControl"),_T("ScanCmdMode"),1,strConfigFile);
}

CHTSMDeviceHW3310g::~CHTSMDeviceHW3310g(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::AllowFunctionCode
//     
// ����������������ر������빦��
//     
// ���������IN const BOOL& bAllow
//         ��TRUE  - ����
//         ��FALSE - �ر�
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::AllowFunctionCode(IN const BOOL& bAllow)
{
	CString strCommand;
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::ResetDefaultParam
//     
// ��������������Ϊ��������
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::ResetDefaultParam(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("DEFOVR."))
		,m_strCommandHead,FormatCommandData(_T("DEFALT.")));

	//������������
	m_iLastCommandType = SM_CMD_RESET_FACTORY;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::ResetPrivateParam
//     
// ������������������Ϊ�û�ָ����ģʽ
//     
// ���������void
// ���������const CString
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::ResetPrivateParam(IN OUT BYTE& byDataConvertType)
{
	CString strTemp;
	CString strCommand;
	
	//����ɨ�뷽ʽΪ�����
	strTemp		= SetScanMode();
	strCommand += strTemp;
	
	//���ý���ɹ����Ƿ񷢳�����
	strTemp		= SetAllowDecodeVoice(TRUE);
	strCommand += strTemp;
	
	//��������
	strTemp		= SetDecodeVoiceType();
	strCommand += strTemp;
	
	//���ö������
	strTemp		= SetSupplementalLightMode(SM_FUN_SUPPORT_LIGHT_FORBID);
	strCommand += strTemp;
	
	//���ö���Խ��ƹ�
	strTemp		= SetFocusingMode(SM_FUN_FOCUS_FLICKER);
	strCommand += strTemp;
	
	//���ö�����ʱʱ��
	strTemp		= SetDelayTime();
	strCommand += strTemp;
	
	//����ɨ���ֻ���Ļģʽ
	strTemp		= SetScanPhoneReadMode();
	strCommand += strTemp;
	
	//����ֻ����ɨ��QR��
	strTemp		= AllowOnlyScanQRCode();
	strCommand += strTemp;
	
	//������������Ϊ�û�Ĭ������
	strTemp		= SubmitPrivateParam();
	strCommand += strTemp;

	//����ǰ׺
//	strTemp		= AddPrefix(_T("@BNG@"));
//	strCommand += strTemp;

	//���Ӻ�׺
//	strTemp		= AddSuffix(_T(""));
//	strCommand += strTemp;

	//����ת�����ݸ�ʽΪ��Ҫ
	byDataConvertType = 1;

	//������������
	m_iLastCommandType = SM_CMD_INIT;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::Start
//     
// ��������������ɨ�����
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::Start()
{
	CString strCommand;
	if(0 == m_iScanCodeType)
	{
		strCommand.Format(_T("%02X%02X%02X"),0x16,'T',0x0D);
	}
	else
	{
		strCommand.Format(_T("%02X%02X%02X%02X"),0x16,'T',0x0D,0x2E);
	}
	//������������
	m_iLastCommandType = SM_CMD_START;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::Stop
//     
// ����������ֹͣɨ�����
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::Stop(void)
{
	CString strCommand;
	if(0 == m_iScanCodeType)
	{
		strCommand.Format(_T("%02X%02X%02X"),0x16,'U',0x0D);
	}
	else
	{
		strCommand.Format(_T("%02X%02X%02X%02X"),0x16,'U',0x0D,0x2E);
	}
	

	//������������
	m_iLastCommandType = SM_CMD_STOP;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::GetHeartCmd
//     
// ������������������ָ��(ʵ�ʾ���һ���򵥵Ĳ�ѯָ��)
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::GetHeartCmd(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("cbrena.")));
	//������������
	m_iLastCommandType = SM_CMD_HEART;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::MakeVoice
//     
// ������������������
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::MakeVoice(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("MNUCDS.")));
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::FormatReadData
//     
// ������������ʽ��������������
//     
// ���������IN OUT LPBYTE byData
//         ��IN const DWORD& dwSize
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��2��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CHTSMDeviceHW3310g::FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	CString strFormatData(_T("FAILED"));
	BOOL bFindSuccessFlag = FALSE;
	for(DWORD i = 0;i<dwSize;++i)
	{
		if(0x06 == byData[i])
		{
			bFindSuccessFlag = TRUE;
			break;
		}
	}

	//if(bFindSuccessFlag)
	//{
	//	for(DWORD i=0;i<dwSize-1;++i)
	//	{
	//		byData[i] = byData[i+1];
	//	}
	//	--dwSize;
	//}

	strFormatData.Format(_T("%s"),(LPTSTR)byData);
	strFormatData.TrimLeft();
	strFormatData.TrimRight();

	return strFormatData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::BeginSetPrivateParam
//     
// ���������������û����ò�������
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵�������豸�̼���֧�ָù���
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::BeginSetPrivateParam(void)
{
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SubmitPrivateParam
//     
// �����������ݽ��û����ò���(����)
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SubmitPrivateParam(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("MNUCDS.")));
	//������������
	m_iLastCommandType = SM_CMD_RESET_PERSION;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::GetSysInfo
//     
// ������������ȡɨ����ϵͳ��Ϣ
//     
// ���������IN OUT CString& strSysInfo
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::GetSysInfo(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("REVINF.")));
	//������������
	m_iLastCommandType = SM_CMD_VER;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetRS232CommMode
//     
// ��������������ͨѶ����
//     
// ���������IN const int& iRate = 9600
//         ��IN const int& iParityBit = 0
//         ��IN const int& iStopBit = 1
//         ��IN const int& iDataBit = 8
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetRS232CommMode(IN const int& iRate/* = 9600*/,IN const int& iParityBit/* = 0*/,IN const int& iStopBit/* = 1*/,IN const int& iDataBit/* = 8*/)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("232WRD2."))
		,m_strCommandHead,FormatCommandData(_T("232BAD5.")));
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::CheckDeviceStatus
//     
// ��������������豸״̬
//     
// ���������IN const CString& strState -- �豸״̬����
// ���������const int
//         ��0		-	�豸����
//		   ����0	-	�豸�쳣
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const int  CHTSMDeviceHW3310g::CheckDeviceStatus(IN const CString& strState)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::CheckCommandExecuteResult
//     
// �����������������ִ�н���Ƿ���ȷ
//     
// ���������IN OUT LPBYTE byData
//         ��IN OUT DWORD& dwSize
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��30��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSMDeviceHW3310g::CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	BOOL bReturn = FALSE;

	int iSuccessFlagCount = 0;
	for(DWORD i = 0;i<dwSize;++i)
	{
		if(0x06 == *byData++)
		{
			++iSuccessFlagCount;
		}
	}

	switch(m_iLastCommandType)
	{
	case SM_CMD_INIT://��ʼ��
		{
			if(iSuccessFlagCount>3)
			{
				bReturn = TRUE;
			}
		}
		break;
	case SM_CMD_VER://���汾
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_START://ɨ�봥��
		{
			if(dwSize>=18)
			{
				bReturn = TRUE;
			}
		}
		break;
	case SM_CMD_STOP://ֹͣɨ��
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_HEART://����
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_RESET_FACTORY://���ó���
		{
			bReturn = TRUE;
		}
		break;
	case SM_CMD_RESET_PERSION://�����û�Ĭ��
		{
			bReturn = TRUE;
		}
		break;
	}
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�BOOL	CHTSMDeviceHW3310g::AnalysisVersionString
//     
// ���������������汾��Ϣ�ַ���
//     
// ���������IN const CString& strVersionString
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��1��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL	CHTSMDeviceHW3310g::AnalysisVersionString(IN const CString& strVersionString)
{
	BOOL bReturn = FALSE;
	CString strRealString(strVersionString);
	if(strRealString.GetLength()<128)
		return bReturn;

	int iBegin	= strRealString.Find(_T("Product Name:")) + (int)_tcslen(_T("Product Name:"));
	int iCount	= strRealString.Find(_T("Boot Revision:")) - strRealString.Find(_T("Product Name:")) - (int)_tcslen(_T("Boot Revision:"));
	m_strProductName		= strRealString.Mid(iBegin,iCount);

	iBegin	= strRealString.Find(_T("Boot Revision: :")) + (int)_tcslen(_T("Boot Revision: :"));
	iCount	= 6;
	m_strBootRevision		= strRealString.Mid(iBegin,iCount);
	
	iBegin	= strRealString.Find(_T("Serial Number:")) + (int)_tcslen(_T("Serial Number:"));
	iCount	= strRealString.Find(_T("Supported IF:")) - strRealString.Find(_T("Serial Number:")) - (int)_tcslen(_T("Supported IF:")) - 1;
	m_strSerialNumber		= strRealString.Mid(iBegin,iCount);
	
	iBegin	= strRealString.Find(_T("Engine Firmware")) + (int)_tcslen(_T("Engine Firmware"));
	iCount	= strRealString.GetLength() - iBegin;
	m_strFirmwareVersion	= strRealString.Mid(iBegin,iCount);

	bReturn = TRUE;
	return bReturn;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CString	CHTSMDeviceHW3310g::FormatCommandData
//     
// �������������������ݣ������ַ�����ת������ӦASCII���16���Ƹ�ʽ
//     
// ���������IN const CString& strCmd
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::FormatCommandData(IN const CString& strCmd)
{
	CString strTempCmd(strCmd);
	BYTE	byData[512]={0};
	memcpy(&byData[0],strTempCmd.GetBuffer(0),strTempCmd.GetLength());strTempCmd.ReleaseBuffer();
	CString strASCIICmdData = theApp.Bin2Hex(byData,strTempCmd.GetLength());
	return strASCIICmdData;
}

/************************************************************************************************************Ӳ������************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetScanPhoneReadMode
//     
// ��������������ɨ���ֻ�ģʽ
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetScanPhoneReadMode(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("PAPHHC.")));
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetSupplementalLightMode
//     
// ��������������ɨ�벹��ģʽ
//     
// ���������IN const int& iLightMode
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetSupplementalLightMode(IN const int& iLightMode)
{
	CString strCommand;
	switch(iLightMode)
	{
	case SM_FUN_SUPPORT_LIGHT_ALLOW:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNLED1.")));
		break;
	case SM_FUN_SUPPORT_LIGHT_FORBID:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNLED0.")));
		break;
	}
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetFocusingMode
//     
// �������������öԽ�ģʽ
//     
// ���������IN const int& iMode
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetFocusingMode(IN const int& iMode)
{
	CString strCommand;
	switch(iMode)
	{
	case SM_FUN_FOCUS_LIGHT:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNAIM1.")));
		break;
	case SM_FUN_FOCUS_FLICKER:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNAIM2.")));
		break;
	case SM_FUN_FOCUS_NONE:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("SCNAIM0.")));
		break;
	}
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetAllowDecodeVoice
//     
// ���������������Ƿ����������Ϻ���
//     
// ���������IN const BOOL& bAllowVoice
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetAllowDecodeVoice(IN const BOOL& bAllowVoice)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,
		bAllowVoice ? FormatCommandData(_T("BEPBEP1.")):FormatCommandData(_T("BEPBEP0.")));	//����
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetDecodeVoiceType
//     
// �������������ý�����Ϻ����������
//     
// ���������IN const int& iVoiceType
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetDecodeVoiceType(IN const int& iVoiceType)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("BEPLVL3."))			//��������
		,m_strCommandHead,FormatCommandData(_T("BEPFQ14200.")));	//����Ϊ��
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

/************************************************************************************************************ɨ��ģʽ����************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetDelayTime
//     
// ��������������ɨ�볬ʱʱ��
//     
// ���������IN const int& iDealyTime=2000
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetDelayTime(IN const int& iDealyTime)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(theApp.FmtStr(_T("232LPT%03d."),iDealyTime)));
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetDelayTimeMode
//     
// ����������������ʱģʽ(��ȫ��ʱ�������Ƿ��ȡ�����ݶ���ʱָ��ʱ�䣻����ȫ��ʱ���������򷵻�)
//     
// ���������IN const int& iDelayMode = 1
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetDelayTimeMode(IN const int& iDelayMode)
{
	CString strCommand;

	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetScanMode
//     
// ��������������ɨ��ģʽ
//     
// ���������IN const int& iScanMode = 1
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetScanMode(IN const int& iScanMode)
{
	CString strCommand;
	switch(iScanMode)
	{
	case SM_FUN_SCAN_CMD:	//�����
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("HSTCEN1.")));
		break;
	case SM_FUN_SCAN_AUTO:	//�Զ�ɨ��
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("TRGST3.")));
		break;
	case SM_FUN_SCAN_MANUAL://�ֶ�
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("PAPHHF.")));
		break;
	}
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetSensitivityRate
//     
// ��������������������
//     
// ���������IN const int& iSensitivityRate
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetSensitivityRate(IN const int& iSensitivityRate )
{
	CString strCommand;
	switch(iSensitivityRate)
	{
	case SM_FUN_SENSITIVITY_LOW:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(theApp.FmtStr(_T("TRGPMS%X."),5)));
		break;
	case SM_FUN_SENSITIVITY_MID:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(theApp.FmtStr(_T("TRGPMS%X."),15)));
		break;
	case SM_FUN_SENSITIVITY_HIGH:
		strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(theApp.FmtStr(_T("TRGPMS%X."),20)));
		break;
	}

	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::SetSensitivityValue
//     
// ����������������������ֵ
//     
// ���������IN const int& iSensitivityValue
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::SetSensitivityValue(IN const int& iSensitivityValue)
{
	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return _T("");
}

/************************************************************************************************************����ɸѡ����************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CString	CHTSMDeviceHW3310g::AllowOnlyScanQRCode
//     
// ����������ֻ����ɨ��QR��
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::AllowOnlyScanQRCode(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s%s%s")
		,m_strCommandHead,FormatCommandData(_T("ALLENA0."))		//��ֹ������
		,m_strCommandHead,FormatCommandData(_T("QRCENA1.")));	//�����QR��

	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::ResetQRCodeParam
//     
// ��������������QR��Ĳ���ΪĬ��
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2015��11��27��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::ResetQRCodeParam(void)
{
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,FormatCommandData(_T("QRCDFT.")));//����QR�����

	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::AddPrefix
//     
// ��������������ǰ׺
//     
// ���������IN const CString& strPreFix
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::AddPrefix(IN const CString& strPrefix)
{
	CString strRealPrefix(strPrefix);
	if(strRealPrefix.IsEmpty())
	{
		strRealPrefix.Format(_T("@BGN@"));
	}
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,
		FormatCommandData(theApp.FmtStr(_T("PREBK2##%s."),strRealPrefix)));//����ǰ׺

	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSMDeviceHW3310g::AddSuffix
//     
// �������������Ӻ�׺
//     
// ���������IN const CString& strSuffix
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��2��28��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const CString CHTSMDeviceHW3310g::AddSuffix(IN const CString& strSuffix)
{
	CString strRealSuffix(strSuffix);
	if(strRealSuffix.IsEmpty())
	{
		strRealSuffix.Format(_T("@END@"));
	}
	CString strCommand;
	strCommand.Format(_T("%s%s"),m_strCommandHead,
		FormatCommandData(theApp.FmtStr(_T("SUFBK2.K9KK9K%s"),strRealSuffix)));//���Ӻ�׺

	//������������
	m_iLastCommandType = SM_CMD_UNKNOW;

	return strCommand;
}