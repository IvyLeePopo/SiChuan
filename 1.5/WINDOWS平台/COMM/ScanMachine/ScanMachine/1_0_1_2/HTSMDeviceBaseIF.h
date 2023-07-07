/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ScanMachineIF.h
 * ժ    Ҫ��ɨ�������ܶ�������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
#pragma once

class CHTSMDeviceBaseIF
{
//�ڲ�ö�ٶ���
public:
	enum SM_FUN_TYPE
	{
		SM_FUN_SUPPORT_LIGHT_ALLOW	= 1,	//ɨ�����������
		SM_FUN_SUPPORT_LIGHT_FORBID = 2,	//ɨ����̲�������

		SM_FUN_FOCUS_LIGHT			= 1,	//�Խ��еƹ�
		SM_FUN_FOCUS_FLICKER		= 2,	//�Խ��ƹ���˸
		SM_FUN_FOCUS_NONE			= 3,	//�Խ��޵ƹ�

		SM_FUN_VOICLE_NORMAL		= 1,	//��������
		SM_FUN_VOICE_BIG			= 2,	//�ϴ�����

		SM_FUN_SCAN_CMD				= 1,	//�����
		SM_FUN_SCAN_AUTO			= 2,	//�Զ�����
		SM_FUN_SCAN_MANUAL			= 3,	//�ֶ�����

		SM_FUN_DELAY_WAIT			= 1,	//ɨ������ȴ�
		SM_FUN_DELAY_NO_WAIT		= 2,	//ɨ�벻�����ȴ�

		SM_FUN_SENSITIVITY_LOW		= 1,	//�����ȵ�
		SM_FUN_SENSITIVITY_MID		= 2,	//����������
		SM_FUN_SENSITIVITY_HIGH		= 3,	//�����ȸ�

		SM_DEFAULT_WAIT_SCAN_TIME   = 30000,//ɨ��ȴ�����ʱ��,Ĭ��30��
	};

public:
	CHTSMDeviceBaseIF();
	CHTSMDeviceBaseIF(IN const int& iSMType);
	~CHTSMDeviceBaseIF(void);
//Interface
public:
	/****************************ϵͳ��������*******************************/

	//����Ϊ��������
	virtual const CString ResetDefaultParam(void)=0;
	//����Ϊ�û�Ĭ�ϲ���
	virtual const CString	ResetPrivateParam(IN OUT BYTE& byDataConvertType)=0;
	//�����û����ò�������
	virtual const CString	BeginSetPrivateParam(void)=0;
	//�ݽ��û����ò���(����)
	virtual const CString	SubmitPrivateParam(void)=0;
	//��ȡɨ����ϵͳ��Ϣ
	virtual const CString	GetSysInfo(void)=0;
	//��������(����һ����ѯ)
	virtual const CString GetHeartCmd(void)=0;
	//����ͨѶ����
	virtual const CString	SetRS232CommMode(IN const int& iRate = 9600,IN const int& iParityBit = 0,IN const int& iStopBit = 1,IN const int& iDataBit = 8)=0;
	//����豸״̬
	virtual const int	CheckDeviceStatus(IN const CString& strState)=0;
	//��ȡɨ��������
	virtual const int	GetSMType(void) const {return m_iSMType;}
	//�����汾��Ϣ�ַ���
	virtual const BOOL	AnalysisVersionString(IN const CString& strVersionString)=0;
	//�Ƿ���Ҫ��ȡ�汾��Ϣ
	const BOOL IsNeedReadVersionInfo(void);

	/****************************Ӳ������*******************************/
	//����ɨ���ֻ�ģʽ
	virtual const CString	SetScanPhoneReadMode(void) = 0;
	//����ɨ�벹��ģʽ
	virtual const CString	SetSupplementalLightMode(IN const int& iLightMode = SM_FUN_SUPPORT_LIGHT_FORBID)=0;
	//���öԽ�ģʽ
	virtual const CString	SetFocusingMode(IN const int& iMFocusode = SM_FUN_FOCUS_NONE)=0;
	//�����Ƿ����������Ϻ���
	virtual const CString	SetAllowDecodeVoice(IN const BOOL& bAllowVoice = TRUE)=0;
	//���ý�����Ϻ����������
	virtual const CString	SetDecodeVoiceType(IN const int& iVoiceType = SM_FUN_VOICLE_NORMAL)=0;

	/****************************ɨ��ģʽ����*******************************/
	//����ɨ�볬ʱʱ��
	virtual const CString	SetDelayTime(IN const int& iDealyTime = SM_DEFAULT_WAIT_SCAN_TIME)=0;
	//������ʱģʽ(��ȫ��ʱ�������Ƿ��ȡ�����ݶ���ʱָ��ʱ�䣻����ȫ��ʱ���������򷵻�)
	virtual const CString	SetDelayTimeMode(IN const int& iDelayMode = SM_FUN_DELAY_WAIT)=0;
	//����ɨ��ģʽ
	virtual const CString	SetScanMode(IN const int& iScanMode = SM_FUN_SCAN_CMD)=0;
	//����������
	virtual const CString	SetSensitivityRate(IN const int& iSensitivityRate = SM_FUN_SENSITIVITY_MID)=0;
	//������������ֵ
	virtual const CString	SetSensitivityValue(IN const int& iSensitivityValue)=0;

	/****************************����ɸѡ����*******************************/
	
	//ֻ����ɨ��QR��
	virtual const CString	AllowOnlyScanQRCode(void)=0;
	//����QR��Ĳ���ΪĬ��
	virtual const CString	ResetQRCodeParam(void)=0;
	
	//��ʽ����ȡ����������
	virtual CString FormatReadData(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)=0;

	//�������ִ�н���Ƿ���ȷ
	virtual BOOL CheckCommandExecuteResult(IN OUT LPBYTE byData,IN OUT DWORD& dwSize)=0;

	//��ȡ�ϴ�ִ�е���������
	const int GetLastCommandType(void) const {return m_iLastCommandType;}

	//�����ϴε���������
	void ResetLastCommandType(void) {m_iLastCommandType = 0;}
	
	//����ǰ׺
	const CString AddPrefix(IN const CString& strSuffix);
	//���Ӻ�׺
	const CString AddSuffix(IN const CString& strSuffix);

	/****************************ʵʱ����*******************************/
	//����ɨ�����
	virtual const CString	Start(void) = 0;
	//ֹͣɨ�����
	virtual const CString	Stop(void) = 0;
	//��������
	virtual const CString	MakeVoice(void) = 0;
//Inner Function
protected:
	//������ر������빦��
	virtual const CString	AllowFunctionCode(IN const BOOL& bAllow) = 0;
//Attributes
protected:
	int		m_iSMType;				//ɨ��������(1,�´�½NLS-FM420)
	DWORD	m_dwCodeLen;			//ɨ������ȡ������񻯵����ݵ�λ��
	DWORD	m_iLastCommandType;		//1,��ʼ��;2,���汾;3,ɨ�봥��;4,ֹͣɨ��;5,����;6,���ó���;7,�����û�Ĭ��
	CString m_strProductName;		//�ڲ���Ʒȫ��
	CString m_strBootRevision;		//�����汾��Ϣ
	CString	m_strSerialNumber;		//��Ʒ���к�
	CString m_strFirmwareVersion;	//�̼��汾����
};
