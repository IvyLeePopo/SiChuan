/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�HTSMProtocol.h
 * ժ    Ҫ��ɨ�����ڲ�ͨѶЭ������࣬�����ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��2��22��
 */
#pragma once
#include "HTSMHoneywell3310g.h"

#include <list>
#include "ProtocolHelper.h"

#define MAKEINTBIG(fir,sec,thir,four) ( MAKELONG(MAKEWORD((four),(thir)),MAKEWORD((sec),(fir) )) )

#define SWITCHTO_V2_0_SYN  10
#define SWITCHTO_V2_0_SYN_ACK 11
#define SWITCHTO_V2_0_ACK  12
#define SWITCHTO_V1_5_SYN  20
#define SWITCHTO_V1_5_SYN_ACK  21
#define SWITCHTO_V1_5_ACK  22

class CHTSMDeviceControl
{
public:
//�ڲ���������
	enum SM_PROTOCOL_CONST
	{
		SM_PROTOCOL_HEADER	= 0xDF,	//Э����ʼ��
		SM_PROTOCOL_END		= 0xFD,	//Э����ֹ��

		SM_PROTOCOL_CONVERT_CHAR		= 0x00,	//Э��ת�빫���ַ�

		SM_PROTOCOL_HEAND_CONVERT_CHAR1	= 0xDE,	//Э����ʼ��ת���ַ�1
		SM_PROTOCOL_HEAND_CONVERT_CHAR2	= 0x01,	//Э����ʼ��ת���ַ�2

		SM_PROTOCOL_END_CONVERT_CHAR1	= 0xFC,	//Э����ֹ��ת���ַ�1
		SM_PROTOCOL_END_CONVERT_CHAR2	= 0x01,	//Э����ֹ��ת���ַ�2

		/**************************�Զ���Э��֡����*****************************/
		SM_PROTOCOL_FRAME_TYPE_UNKNOW	= 0x00,	//δ֪����֡

		SM_PROTOCOL_FRAME_TYPE_A1		= 0xA1,	//����д����ָ��
		SM_PROTOCOL_FRAME_TYPE_A2		= 0xA2, //���Ͷ�����ָ��
		SM_PROTOCOL_FRAME_TYPE_A3		= 0xA3,	//����IO״̬����ָ��
		SM_PROTOCOL_FRAME_TYPE_A4		= 0xA4,	//����IO״̬��ȡָ��
		SM_PROTOCOL_FRAME_TYPE_A5		= 0xA5, //�����Զ���ָ��
		SM_PROTOCOL_FRAME_TYPE_A6		= 0xA6, //�����豸��λָ��

		SM_PROTOCOL_FRAME_TYPE_A7		= 0xA7, //����͸��
		SM_PROTOCOL_FRAME_TYPE_A8		= 0xA8, //add 2017-12 
		SM_PROTOCOL_FRAME_TYPE_A9		= 0xA9, //add 2017-12
		SM_PROTOCOL_FRAME_TYPE_AD		= 0xAD, //add 2017-12
		SM_PROTOCOL_FRAME_TYPE_AE		= 0xAE, //add 2017-12


		SM_PROTOCOL_FRAME_TYPE_B0		= 0xB0,	//��ʼ����Ϣ֡
		SM_PROTOCOL_FRAME_TYPE_B1		= 0xB1, //����֡
		SM_PROTOCOL_FRAME_TYPE_B2		= 0xB2, //�����ϴ���Ϣ֡
		SM_PROTOCOL_FRAME_TYPE_B3		= 0xB3, //����͸����Ϣ֡
		SM_PROTOCOL_FRAME_TYPE_B4		= 0xB4, //ref to doc
		SM_PROTOCOL_FRAME_TYPE_B5		= 0xB5, //ref to doc

		
		SM_PROTOCOL_FRAME_TYPE_C0		= 0xC0, //ָ��Ӧ��֡�������λ�����͵�A1~A8��ָ��Ľ������
	
		SM_PRPTOCOL_LIGHT_ALL_OFF		= 0x03, //���
		SM_PRPTOCOL_LIGHT_SCAN			= 0x01, //��ʾ����ɨ��
		SM_PRPTOCOL_LIGHT_SUCCESS		= 0x00,	//��ʾɨ��ɹ�
		SM_PRPTOCOL_LIGHT_FAIL			= 0x02,	//��ʾɨ��ʧ��
	};

public:
	CHTSMDeviceControl(void);
	~CHTSMDeviceControl(void);
//Interfaces
public:
	//���ò���
	void SetParam(IN const UINT& nCallThreadID){m_nCallerThreadID = nCallThreadID;}
	//��ȡ�ڹ��豸Э�������
	CHTSMDeviceBaseIF* GetInnerDeviceProtocolClass(void) {return m_pSMIF;}
	//��ʼ���豸
	const BOOL InitInnerDevice(IN OUT CString& strLog,IN const int& iInnerDeviceType=1);
	//���յ������ݳ����Ƿ������������֡У��
	const BOOL IsAllowAnalyseReceiveDataOnBytesNum(IN const DWORD& dwHadReceiveDataSize) const ;
	//�������յ������ݣ��ж��Ƿ����Э��
	void AnalyseReciveData(IN OUT LPBYTE pData,IN const int& iDataSize,IN OUT LPBYTE& pBegin,IN OUT LPBYTE& pEnd);
	//add 2017-10 ����������֡�����
	void AnalyseReciveData_list(IN OUT LPBYTE pData,IN const int& iDataSize,std::list<LPBYTE>& pBegin_list,std::list<LPBYTE>& pEnd,int& deal_size);
	//�����������
	const BOOL ProcessReceiveFrameData(IN const LPBYTE lpData,IN const DWORD& dwSize, IN OUT int& iFrameType);

	//��ȡ���ڰ�����
	const CString ReadConfigInfoFromBoard(IN const int& iType = 0x00);
	//���´��ڰ�����
	const CString UpdateConfigInfoFromBoard(IN const CString& strConfigInfo);
	//��������ģʽ
	const CString SetLightMode(IN const int& iLightIndex);
	//��ѯ����״̬
	const CString GetLightMode(void);

	//��������ɨ������ʾ
	const CString SetScanDisplay(tagScanDisplay	sDisplay);
	const CString ConstructFrameA1(tagMMIMedia	sMedia);

	//�������ͻ���ָ��
	const CString SendCmdResponse(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame);

	const CString SetTransData(const tagTransmissionData& trans_data);

	//add 2017-11 zhuyabing
	const CString ConstructTTS(const std::string& msg);

	//add 2017-12 zhuyabing
	const CString ConstructUpgradeControl(const FirmwareUpgradeControl& upgradOption);
	const CString ConstructUpgradeData(const FirmwareUpgradeTransfer& upgradeData);
	const CString ConstructFreeDisplayData(const FreeDisplayData& data);
	const CString ConstructFreeDisplayStyle(const FreeDisplayStyle& style);
	
	const CString ConstructTerminalStatusQuery();
	const CString ConstructSwitchSync(const int& status);

	//��λ���ڰ�
	const CString ResetBoard(void);
	//�����Զ���ָ��
	const CString SendUserDefineCmd(IN const CString& strCmdContent,IN const BYTE& byDeviceIndex=0);

	bool ParseSpecialCommond01(const LPBYTE lpData, const DWORD& dwSize,  int& result);
	bool ParseSpecialCommond02(const LPBYTE lpData, const DWORD& dwSize,  int& result);
//Inner Fuctions
private:
	//����A1֡ -- д����������Ϣ
	const CString ConstructFrameA1(IN const CString& strParam);
	const CString ConstructFrameA1(IN const int& iType);

	//����A2֡ -- ������������Ϣ
	const CString ConstructFrameA2(void);
	//����A3֡ -- ����״̬����
	const CString ConstructFrameA3(IN const int& iLightIndex);
	//����A4֡ -- ����״̬��ȡ
	const CString ConstructFrameA4(void);
	//����A5֡ -- ����ɨ���豸ָ��
	const CString ConstructFrameA5(IN const LPBYTE lpData,IN const DWORD& dwSize);

	//����������������Ӳ����ʾA5ָ��
	const CString ConstructFrameA5(tagScanDisplay sDisplay);

	//����A8֡ -- ��λ����
	const CString ConstructFrameA6(void);
	//����C0֡ -- ָ��Ӧ��֡
	const CString ContructFrameC0(IN const BYTE& byResponseType,IN const BYTE& byResponseFrame);

	const CString ConstructFrameA7(const tagTransmissionData& trans_data);

	const CString ConstructFrameA8(const FirmwareUpgradeControl& upgradOption);
	const CString ConstructFrameA9(const FirmwareUpgradeTransfer& upgradeData);
	

	//���ݽ��յ��������ж�����֡
	const int GetDataFrameType(IN const LPBYTE lpFrame,IN const DWORD& dwSize,IN OUT CString& strReceiveFrameType);
	//����֡У��
	const BOOL VerifyFrameData(IN const LPBYTE pData,IN const DWORD& dwSize);

	//����ת��
	void ConvertData(IN OUT LPBYTE lpData,IN OUT DWORD& dwSize,IN const BOOL& bSendState);
	//��ȡУ����
	const WORD GetVerifyCode(IN const BYTE& byFrameType,IN const WORD& wSize,IN const LPBYTE lpData) const;
	
	//����B0֡
	void ProcessFrameB0(IN const LPBYTE lpData,IN const DWORD& dwSize);
	//����B1֡
	void ProcessFrameB1(IN const LPBYTE lpData,IN const DWORD& dwSize);
	//����B2֡
	void ProcessFrameB2(IN const LPBYTE lpData,IN const DWORD& dwSize);
	void ProcessFrameB3(IN const LPBYTE lpData,IN const DWORD& dwSize);
	void ProcessFrameB4(IN const LPBYTE lpData,IN const DWORD& dwSize);
	void ProcessFrameB5(IN const LPBYTE lpData,IN const DWORD& dwSize);
	//����C0֡
	void ProcessFrameC0(IN const LPBYTE lpData,IN const DWORD& dwSize);

	//�豸�ڲ�Э�鴦���� -- �����ʼ��ָ�������
	void fnInnerDeviceProtocolProcessResultForInit(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//�豸�ڲ�Э�鴦���� -- ����汾��ȡָ�������
	void fnInnerDeviceProtocolProcessResultForVer(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//�豸�ڲ�Э�鴦���� -- ����ʼɨ��ָ�������
	void fnInnerDeviceProtocolProcessResultForStartScan(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//�豸�ڲ�Э�鴦���� -- ����ֹͣɨ��ָ�������
	void fnInnerDeviceProtocolProcessResultForStopScan(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//�豸�ڲ�Э�鴦���� -- �����豸����ָ�������
	void fnInnerDeviceProtocolProcessResultForHeart(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//�豸�ڲ�Э�鴦���� -- �����������Ĭ�ϻ�������ָ�������
	void fnInnerDeviceProtocolProcessResultForDefaultFactory(IN const BYTE& byDeviceID,IN const CString& strFormatData);
	//�豸�ڲ�Э�鴦���� -- ���������û��Զ��廷��ָ�������
	void fnInnerDeviceProtocolProcessResultForDefaultPersion(IN const BYTE& byDeviceID,IN const CString& strFormatData);

//Attributes
public:
	//ɨ����״̬������
	CSMInnerDeviceInfo	m_cInnerDeviceInfo;
	//���ڰ���Ϣ -- �ϴ��յ�������ʱ��
	time_t	m_tLastHeartTimeForBoard;
	//�ϴη��͵�������Ϣ(������A5֡��Ч)
	CString m_strLastSendFrameData;
	//����Ƿ���Ҫ�ط�����
	BOOL	m_bNeedReSendLastData;
private:
	//�豸Э�������
	CHTSMDeviceBaseIF* m_pSMIF;
	//�������߳�ID
	UINT	m_nCallerThreadID;
	
	//���ڰ���Ϣ -- �Ƿ��Ѿ���⵽����֡���
	BOOL	m_bIsHaveFrameFlag;
	
	//�ڲ��豸��Ϣ -- ɨ��ָ���ʱ��
	DWORD		m_dwBeginScanTime;
	SYSTEMTIME	m_stForBeginScan;
	//�ڲ��豸��Ϣ -- ɨ��ָ���û���ʱ��
	SYSTEMTIME	m_stForEndScan;
};
