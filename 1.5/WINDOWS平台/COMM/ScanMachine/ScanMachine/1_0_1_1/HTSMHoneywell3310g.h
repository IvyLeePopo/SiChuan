/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�SMHoneywell3301g.h
 * ժ    Ҫ������Τ��3310gɨ����Э�������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
#pragma once
#include "HTSMDeviceBaseIF.h"

class CHTSMDeviceHW3310g :
	public CHTSMDeviceBaseIF
{
public:
	CHTSMDeviceHW3310g(IN const int& iSMType);
	~CHTSMDeviceHW3310g(void);

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
	//����ͨѶ����
	const CString	SetRS232CommMode(IN const int& iRate = 9600,IN const int& iParityBit = 0,IN const int& iStopBit = 1,IN const int& iDataBit = 8);
	//����豸״̬
	const int	CheckDeviceStatus(IN const CString& strState);
	//��������ָ��(ʵ�ʾ���һ���򵥵Ĳ�ѯָ��)
	const CString GetHeartCmd(void);
	//�����汾��Ϣ�ַ���
	const BOOL	AnalysisVersionString(IN const CString& strVersionString);
	/****************************Ӳ������*******************************/
	//����ɨ���ֻ�ģʽ
	virtual const CString	SetScanPhoneReadMode(void);
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
	CString	FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize);
	//�������ִ�н���Ƿ���ȷ
	BOOL CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize);

	//����ǰ׺
	const CString AddPrefix(IN const CString& strSuffix);
	//���Ӻ�׺
	const CString AddSuffix(IN const CString& strSuffix);

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
	//���������ݣ������ַ�����ת������ӦASCII���16���Ƹ�ʽ
	const CString	FormatCommandData(IN const CString& strCmd);
	//3310G����ͷ
	CString	m_strCommandHead;

	int m_iScanCodeType;
};
