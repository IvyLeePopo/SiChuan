#include "StdAfx.h"
#include "DVJsonDebitCheck.h"

CDVJsonDebitCheck::CDVJsonDebitCheck(void)
	:RecordMatchCode(_T(""))
	,IDBindInPayPlatform(_T(""))
	,IDBindInPayPlatformType(0)
	,PayIdentifier(_T(""))
	,DebitType(0)
	,DebitMoney(0)
	,CheckStateDelayTime(0)	
	
	,DebitStatusCode(0)
	,DebitStatusDesc(_T(""))
	,DebitOrderNumber(_T(""))
{
}

CDVJsonDebitCheck::~CDVJsonDebitCheck(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCheck::Reset
//     
// ������������������
//     
// ���������IN const int& iType -- ��������
// ���������void
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��22��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
void CDVJsonDebitCheck::Reset(IN const int& iType)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCheck::CDVJsonDebitCheck
//     
// �������������캯��
//     
// ���������IN const CDVJsonDebit* pDVJsonDebit
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��16��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck::CDVJsonDebitCheck(IN const CDVJsonDebit* pDVJsonDebit)
{
	//�������ݸ���
	Version					=	pDVJsonDebit->Version;
	PackageType				=	pDVJsonDebit->PackageType;
	ProvinceID				=	pDVJsonDebit->ProvinceID;
	Token					=	pDVJsonDebit->Token;
	SubmitCount				=	pDVJsonDebit->SubmitCount;
	SubmitTime				=	pDVJsonDebit->SubmitTime;
	QRCodeData				=	pDVJsonDebit->QRCodeData;
	QRCodeType				=	pDVJsonDebit->QRCodeType;
	m_hCallerWnd			=	pDVJsonDebit->m_hCallerWnd;
	m_dwThreadID			=	pDVJsonDebit->m_dwThreadID;
	ServerResponseCode		=	pDVJsonDebit->ServerResponseCode;
	ServerReceiveDataTime	=	pDVJsonDebit->ServerReceiveDataTime;
	ServerResponseDataTime	=	pDVJsonDebit->ServerResponseDataTime;
	//���������ݸ���
	RecordMatchCode			=	pDVJsonDebit->RecordMatchCode;
	IDBindInPayPlatform		=	pDVJsonDebit->IDBindInPayPlatform;
	IDBindInPayPlatformType =	pDVJsonDebit->IDBindInPayPlatformType;
	PayIdentifier			=   pDVJsonDebit->PayIdentifier;
	DebitType				=	pDVJsonDebit->DebitType;
	DebitMoney				=	pDVJsonDebit->DebitMoney;
	DebitTime				=	pDVJsonDebit->DebitTime;
	DebitStatusCode			=	pDVJsonDebit->DebitStatusCode;
	DebitStatusDesc			=	pDVJsonDebit->DebitStatusDesc;
	DebitOrderNumber		=	pDVJsonDebit->DebitOrderNumber;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCheck::CDVJsonDebitCheck
//     
// �������������캯��
//     
// ���������IN const CDVJsonDebitCheck* pDVJsonDebitCheck
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck::CDVJsonDebitCheck(IN const CDVJsonDebitCheck* pDVJsonDebitCheck)
{
	//�������ݸ���
	Version					=	pDVJsonDebitCheck->Version;
	PackageType				=	pDVJsonDebitCheck->PackageType;
	ProvinceID				=	pDVJsonDebitCheck->ProvinceID;
	Token					=	pDVJsonDebitCheck->Token;
	SubmitCount				=	pDVJsonDebitCheck->SubmitCount;
	SubmitTime				=	pDVJsonDebitCheck->SubmitTime;
	QRCodeData				=	pDVJsonDebitCheck->QRCodeData;
	QRCodeType				=	pDVJsonDebitCheck->QRCodeType;
	m_hCallerWnd			=	pDVJsonDebitCheck->m_hCallerWnd;
	m_dwThreadID			=	pDVJsonDebitCheck->m_dwThreadID;
	ServerResponseCode		=	pDVJsonDebitCheck->ServerResponseCode;
	ServerReceiveDataTime	=	pDVJsonDebitCheck->ServerReceiveDataTime;
	ServerResponseDataTime	=	pDVJsonDebitCheck->ServerResponseDataTime;
	//���������ݸ���
	RecordMatchCode			=	pDVJsonDebitCheck->RecordMatchCode;
	IDBindInPayPlatform		=	pDVJsonDebitCheck->IDBindInPayPlatform;
	IDBindInPayPlatformType =	pDVJsonDebitCheck->IDBindInPayPlatformType;
	PayIdentifier			=   pDVJsonDebitCheck->PayIdentifier;
	DebitType				=	pDVJsonDebitCheck->DebitType;
	DebitMoney				=	pDVJsonDebitCheck->DebitMoney;
	DebitTime				=	pDVJsonDebitCheck->DebitTime;
	DebitStatusCode			=	pDVJsonDebitCheck->DebitStatusCode;
	DebitStatusDesc			=	pDVJsonDebitCheck->DebitStatusDesc;
	DebitOrderNumber		=	pDVJsonDebitCheck->DebitOrderNumber;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCheck::operator=
//     
// �������������������
//     
// ���������IN const CDVJsonDebitCheck& cSrc
// ���������CDVJsonDebitCheck&
// ��д��Ա��ROCY
// ��дʱ�䣺2015��12��15��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCheck& CDVJsonDebitCheck::operator=(IN const CDVJsonDebitCheck& cSrc)
{
	if(this != &cSrc)
	{
		//�������ݸ���
		Version					=	cSrc.Version;
		PackageType				=	cSrc.PackageType;
		ProvinceID				=	cSrc.ProvinceID;
		Token					=	cSrc.Token;
		SubmitCount				=	cSrc.SubmitCount;
		SubmitTime				=	cSrc.SubmitTime;
		QRCodeData				=	cSrc.QRCodeData;
		QRCodeType				=	cSrc.QRCodeType;
		m_hCallerWnd			=	cSrc.m_hCallerWnd;
		m_dwThreadID			=	cSrc.m_dwThreadID;
		ServerDebitTime			=	cSrc.ServerDebitTime;
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//���������ݸ���
		RecordMatchCode			=	cSrc.RecordMatchCode;
		IDBindInPayPlatform		=	cSrc.IDBindInPayPlatform;
		IDBindInPayPlatformType =	cSrc.IDBindInPayPlatformType;
		PayIdentifier			=   cSrc.PayIdentifier;
		DebitType				=	cSrc.DebitType;
		DebitMoney				=	cSrc.DebitMoney;
		DebitTime				=	cSrc.DebitTime;
		CheckStateDelayTime		=	cSrc.CheckStateDelayTime;
		DebitStatusCode			=	cSrc.DebitStatusCode;
		DebitStatusDesc			=	cSrc.DebitStatusDesc;
		DebitOrderNumber		=	cSrc.DebitOrderNumber;
	}
	return *this;
}