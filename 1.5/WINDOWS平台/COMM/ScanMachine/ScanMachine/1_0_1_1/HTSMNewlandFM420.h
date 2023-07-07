/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�SMNewlandFM420.h
 * ժ    Ҫ���´�½NSL-FM420ɨ����ͨѶЭ����������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��1��
 */
#pragma once
#include "HTSMDeviceBaseIF.h"

class CHTSMDeviceNLFM420 :	public CHTSMDeviceBaseIF
{
public:
	CHTSMDeviceNLFM420(IN const int& iSMType);
	~CHTSMDeviceNLFM420(void);
	
	//����Ϊ��������
	const CString	ResetDefaultParam(void);
	//����Ϊ�û�Ĭ�ϲ���
	const CString	ResetPrivateParam(IN OUT BYTE& byDataConvertType);
	//�����û����ò�������
	const CString	BeginSetPrivateParam(void);
	//�ݽ��û����ò���(����)
	const CString	SubmitPrivateParam(void);
	//��ȡɨ����ϵͳ��Ϣ
	const CString	GetSysInfo(void);
	//��������ָ��(ʵ�ʾ���һ���򵥵Ĳ�ѯָ��)
	const CString	GetHeartCmd(void){return _T("");}
	//����ͨѶ����
	const CString	SetRS232CommMode(IN const int& iRate = 9600,IN const int& iParityBit = 0,IN const int& iStopBit = 1,IN const int& iDataBit = 8);
	//����豸״̬
	const int	CheckDeviceStatus(IN const CString& strState);
	//�����汾��Ϣ�ַ���
	const BOOL	AnalysisVersionString(IN const CString& strVersionString){return FALSE;}

	/****************************Ӳ������*******************************/
	//����ɨ���ֻ�ģʽ
	virtual const CString	SetScanPhoneReadMode(void){return _T("");}
	//����ɨ�벹��ģʽ
	const CString	SetSupplementalLightMode(IN const int& iLightMode = SM_FUN_SUPPORT_LIGHT_FORBID);
	//���öԽ�ģʽ
	const CString	SetFocusingMode(IN const int& iMFocusode = SM_FUN_FOCUS_NONE);
	//�����Ƿ����������Ϻ���
	const CString	SetAllowDecodeVoice(IN const BOOL& bAllowVoice = TRUE);
	//���ý�����Ϻ����������
	const CString	SetDecodeVoiceType(IN const int& iVoiceType = SM_FUN_VOICLE_NORMAL);

	/****************************ɨ��ģʽ����*******************************/
	//����ɨ�볬ʱʱ��
	const CString	SetDelayTime(IN const int& iDealyTime = SM_DEFAULT_WAIT_SCAN_TIME);
	//������ʱģʽ(��ȫ��ʱ�������Ƿ��ȡ�����ݶ���ʱָ��ʱ�䣻����ȫ��ʱ���������򷵻�)
	const CString	SetDelayTimeMode(IN const int& iDelayMode = SM_FUN_DELAY_WAIT);
	//����ɨ��ģʽ
	const CString	SetScanMode(IN const int& iScanMode = SM_FUN_SCAN_CMD);
	//����������
	const CString	SetSensitivityRate(IN const int& iSensitivityRate = SM_FUN_SENSITIVITY_MID);
	//������������ֵ
	const CString	SetSensitivityValue(IN const int& iSensitivityValue);

	/****************************����ɸѡ����*******************************/

	//ֻ����ɨ��QR��
	const CString	AllowOnlyScanQRCode(void);
	//����QR��Ĳ���ΪĬ��
	const CString	ResetQRCodeParam(void);
	//��ʽ����ȡ����������
	CString FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize);
	//�������ִ�н���Ƿ���ȷ
	BOOL CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize);

	//����ǰ׺
	const CString AddPrefix(IN const CString& strSuffix){return _T("");}
	//���Ӻ�׺
	const CString AddSuffix(IN const CString& strSuffix){return _T("");}

	/****************************ʵʱ����*******************************/
	//����ɨ�����
	const CString	Start(void);
	//ֹͣɨ�����
	const CString	Stop(void);
	//��������
	const CString	MakeVoice(void);
	
//Inner Function
protected:
	//������ر������빦��
	const CString	AllowFunctionCode(IN const BOOL& bAllow);
};
