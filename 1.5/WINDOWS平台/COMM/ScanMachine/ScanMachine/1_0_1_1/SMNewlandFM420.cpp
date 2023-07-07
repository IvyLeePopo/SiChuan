/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�SMNewlandFM420.cpp
 * ժ    Ҫ���´�½NSL-FM420ɨ����ͨѶЭ����������࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
#include "StdAfx.h"
#include "SMNewlandFM420.h"

CSMNewlandFM420::CSMNewlandFM420(IN const int& iSMType)
	:CScanMachineIF(iSMType)
{
	m_dwCodeLen = 18;
}

CSMNewlandFM420::~CSMNewlandFM420(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::AllowFunctionCode
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
const CString CSMNewlandFM420::AllowFunctionCode(IN const BOOL& bAllow)
{
	CString strCommand;
	strCommand.Format(bAllow?_T("NLS0302000;"):_T("NLS0006000;"));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::ResetDefaultParam
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
const CString CSMNewlandFM420::ResetDefaultParam(void)
{
	CString strCommand;
	strCommand.Format(_T("NLS0006000;"));
	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::ResetPrivateParam
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
const CString CSMNewlandFM420::ResetPrivateParam(IN OUT BYTE& byDataConvertType)
{
	CString strTemp;
	CString strCommand;
	
	//���������빦��
	strTemp		= AllowFunctionCode(TRUE);
	strCommand += strTemp;
	
	//����ɨ�뷽ʽΪ�����
	strTemp		= SetScanMode();
	strCommand += strTemp;
	
	//����������
//	strTemp		= SetAllowDecodeVoice(TRUE);
//	strCommand += strTemp;
	
	//��������
//	strTemp		= SetDecodeVoiceType(1);
//	strCommand += strTemp;
	
	//���ö������
//	strTemp		= SetSupplementalLightMode(1);
//	strCommand += strTemp;
	
	//���ö���Խ��ƹ�
//	strTemp		= SetFocusingMode(1);
//	strCommand += strTemp;
	
	//���ö�����ʱʱ��
	strTemp		= SetDelayTime();
	strCommand += strTemp;
	
	//����ֻ����ɨ��QR��
	strTemp		= AllowOnlyScanQRCode();
	strCommand += strTemp;
	
	//������������Ϊ�û�Ĭ������
	strTemp		= SubmitPrivateParam();
	strCommand += strTemp;
	
	//�ر������빦��
	strTemp = AllowFunctionCode(FALSE);
	strCommand += strTemp;

	strCommand = strCommand.Left(strCommand.GetLength() - 1);

	//����ת�����ݸ�ʽΪ����Ҫ
	byDataConvertType = 0;

	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::Start
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
const CString CSMNewlandFM420::Start()
{
	CString strCommand;
	strCommand.Format(_T("%02X%02X"),0x1B,0x31);
	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::Stop
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
const CString CSMNewlandFM420::Stop(void)
{
	CString strCommand;
	strCommand.Format(_T("%02X%02X"),0x1B,0x30);
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::MakeVoice
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
const CString CSMNewlandFM420::MakeVoice(void)
{
	CString strCommand;
	strCommand.Format(_T(""));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::FormatReadData
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
CString  CSMNewlandFM420::FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	CString strFormatData;
	if(!CheckCommandExecuteResult(byData,dwSize))
		return strFormatData;
	for(DWORD i=0;i<dwSize-1;++i)
	{
		byData[i] = byData[i+1];
	}
	--dwSize;
	strFormatData.Format(_T("%s"),(LPTSTR)byData);

	return strFormatData;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::CheckCommandExecuteResult
//     
// �����������������ִ�н���Ƿ���ȷ
//     
// ���������IN OUT LPBYTE byData
//         ��IN OUT DWORD& dwSize
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��7��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CSMNewlandFM420::CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)
{
	if(    (dwSize>0)
		&& (0x06 == byData[0])
		)
	{
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::BeginSetPrivateParam
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
const CString CSMNewlandFM420::BeginSetPrivateParam(void)
{
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SubmitPrivateParam
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
const CString CSMNewlandFM420::SubmitPrivateParam(void)
{
	CString strCommand;
	strCommand.Format(_T("NLS0001150;"));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::GetSysInfo
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
const CString CSMNewlandFM420::GetSysInfo(void)
{
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetRS232CommMode
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
const CString CSMNewlandFM420::SetRS232CommMode(IN const int& iRate/* = 9600*/,IN const int& iParityBit/* = 0*/,IN const int& iStopBit/* = 1*/,IN const int& iDataBit/* = 8*/)
{
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::CheckDeviceStatus
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
const int	CSMNewlandFM420::CheckDeviceStatus(IN const CString& strState)
{
	return 0;
}

/************************************************************************************************************Ӳ������************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetSupplementalLightMode
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
const CString CSMNewlandFM420::SetSupplementalLightMode(IN const int& iLightMode)
{
	CString strCommand;
	switch(iLightMode)
	{
	case SM_FUN_SUPPORT_LIGHT_ALLOW:
		strCommand.Format(_T("NLS0200010;"));
		break;
	case SM_FUN_SUPPORT_LIGHT_FORBID:
		strCommand.Format(_T("NLS0200010;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetFocusingMode
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
const CString CSMNewlandFM420::SetFocusingMode(IN const int& iMode)
{
	CString strCommand;
	switch(iMode)
	{
	case SM_FUN_FOCUS_LIGHT:
		strCommand.Format(_T("NLS0201000;"));
		break;
	case SM_FUN_FOCUS_FLICKER:
		strCommand.Format(_T("NLS0201000;"));
		break;
	case SM_FUN_FOCUS_NONE:
		strCommand.Format(_T("NLS0201000;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetAllowDecodeVoice
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
const CString CSMNewlandFM420::SetAllowDecodeVoice(IN const BOOL& bAllowVoice)
{
	CString strCommand;
	strCommand.Format(bAllowVoice?_T("NLS0203010;"):_T("NLS0203010;"));
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetDecodeVoiceType
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
const CString CSMNewlandFM420::SetDecodeVoiceType(IN const int& iVoiceType)
{
	CString strTemp;
	CString strCommand;
	strTemp.Format(_T("NLS0203020;"));//��������
	strCommand += strTemp;
	strTemp.Format(_T("NLS0203030;"));//����Ϊ��
	strCommand += strTemp;
	return strCommand;
}

/************************************************************************************************************ɨ��ģʽ����************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetDelayTime
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
const CString CSMNewlandFM420::SetDelayTime(IN const int& iDealyTime)
{
	CString strCommand;
	strCommand.Format(_T("NLS0313000=%d;"),iDealyTime);
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetDelayTimeMode
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
const CString CSMNewlandFM420::SetDelayTimeMode(IN const int& iDelayMode)
{
	CString strCommand;
	switch(iDelayMode)
	{
	case SM_FUN_DELAY_WAIT:
		strCommand.Format(_T("NLS0201000;"));
		break;
	case SM_FUN_DELAY_NO_WAIT:
		strCommand.Format(_T("NLS0201000;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetScanMode
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
const CString CSMNewlandFM420::SetScanMode(IN const int& iScanMode)
{
	CString strCommand;
	switch(iScanMode)
	{
	case SM_FUN_SCAN_CMD:
		strCommand.Format(_T("NLS0302000;"));
		break;
	case SM_FUN_SCAN_AUTO:
		strCommand.Format(_T("NLS0302000;"));
		break;
	case SM_FUN_SCAN_MANUAL:
		strCommand.Format(_T("NLS0302000;"));
		break;
	}
	return strCommand;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetSensitivityRate
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
const CString CSMNewlandFM420::SetSensitivityRate(IN const int& iSensitivityRate )
{
	CString strCommand;
	switch(iSensitivityRate)
	{
	case SM_FUN_SENSITIVITY_LOW:
		strCommand.Format(_T("NLS0312010;"));
		break;
	case SM_FUN_SENSITIVITY_MID:
		strCommand.Format(_T("NLS0312010;"));
		break;
	case SM_FUN_SENSITIVITY_HIGH:
		strCommand.Format(_T("NLS0312010;"));
		break;
	}
	return strCommand;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::SetSensitivityValue
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
const CString CSMNewlandFM420::SetSensitivityValue(IN const int& iSensitivityValue)
{
	return _T("");
}

/************************************************************************************************************����ɸѡ����************************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CString	CSMNewlandFM420::AllowOnlyScanQRCode
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
const CString	CSMNewlandFM420::AllowOnlyScanQRCode(void)
{
	CString strTemp;
	CString strCommand;
	strTemp.Format(_T("NLS0001010;"));//���Ƚ�������ɨ��
	strCommand += strTemp;
	strTemp.Format(_T("NLS0502020;"));//�ſ�QR��
	strCommand += strTemp;
	return strCommand;
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSMNewlandFM420::ResetQRCodeParam
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
const CString CSMNewlandFM420::ResetQRCodeParam(void)
{
	return _T("");
}