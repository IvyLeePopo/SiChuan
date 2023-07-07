#include "StdAfx.h"
#include "DVJsonDebitCancel.h"

CDVJsonDebitCancel::CDVJsonDebitCancel(void)
	:PayIdentifier(_T(""))
	,DebitType(0)
	,DebitMoney(0)
	,RequestingPartyType(1)
	,DebitOrderNumber(_T(""))
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCancel::CDVJsonDebitCancel
//     
// �������������캯��
//     
// ���������IN const CDVJsonDebit* pDVJsonDebit
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel::CDVJsonDebitCancel(IN const CDVJsonDebit* pDVJsonDebit)
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
	PayIdentifier			=	pDVJsonDebit->PayIdentifier;
	DebitType				=	pDVJsonDebit->DebitType;
	DebitMoney				=	pDVJsonDebit->DebitMoney;
	DebitTime				=	pDVJsonDebit->DebitTime;
	DebitOrderNumber		=	pDVJsonDebit->DebitOrderNumber;
	RequestingPartyType		=	0x01;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCancel::CDVJsonDebitCancel
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
CDVJsonDebitCancel::CDVJsonDebitCancel(IN const CDVJsonDebitCheck* pDVJsonDebitCheck)
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
	PayIdentifier			=	pDVJsonDebitCheck->PayIdentifier;
	DebitType				=	pDVJsonDebitCheck->DebitType;
	DebitMoney				=	pDVJsonDebitCheck->DebitMoney;
	DebitTime				=	pDVJsonDebitCheck->DebitTime;
	DebitOrderNumber		=	pDVJsonDebitCheck->DebitOrderNumber;
	RequestingPartyType		=	0x01;
}

CDVJsonDebitCancel::~CDVJsonDebitCancel(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCancel::Reset
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
void CDVJsonDebitCancel::Reset(IN const int& iType)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonDebitCancel::operator=
//     
// ������������ֵ���������
//     
// ���������IN const CDVJsonDebitCancel& cSrc
// ���������CDVJsonDebitCancel&
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��6��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonDebitCancel& CDVJsonDebitCancel::operator=(IN const CDVJsonDebitCancel& cSrc)
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
		ServerResponseCode		=	cSrc.ServerResponseCode;
		ServerReceiveDataTime	=	cSrc.ServerReceiveDataTime;
		ServerResponseDataTime	=	cSrc.ServerResponseDataTime;
		//���������ݸ���
		PayIdentifier			=	cSrc.PayIdentifier;
		DebitType				=	cSrc.DebitType;
		DebitMoney				=	cSrc.DebitMoney;
		DebitTime				=	cSrc.DebitTime;
		DebitOrderNumber		=	cSrc.DebitOrderNumber;
		RequestingPartyType		=	cSrc.RequestingPartyType;
	}
	return *this;
}
