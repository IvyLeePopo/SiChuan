#include "StdAfx.h"
#include "DVJsonNFCSignIn.h"

CDVJsonNFCSignIn::CDVJsonNFCSignIn(void)
	:TestingData(FALSE)
	,NetPayDllVersion(_T(""))
	,DeviceDllVersion(_T(""))
	,TWProductSerialNo(_T(""))
	,NFCTerminal(_T(""))
	,NFCSerialNo(_T(""))
	,NFCSignData(_T(""))
{
}

CDVJsonNFCSignIn::~CDVJsonNFCSignIn(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CDVJsonNFCSignIn::operator=
//     
// �������������������
//     
// ���������IN const CDVJsonNFCSignIn& cSrc
// ���������CDVJsonNFCSignIn&
// ��д��Ա��ROCY
// ��дʱ�䣺2017��9��20��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CDVJsonNFCSignIn& CDVJsonNFCSignIn::operator=(IN const CDVJsonNFCSignIn& cSrc)
{
	if(&cSrc != this)
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
		TestingData			= cSrc.TestingData;
		NetPayDllVersion	= cSrc.NetPayDllVersion;
		DeviceDllVersion	= cSrc.DeviceDllVersion;
		TWProductSerialNo	= cSrc.TWProductSerialNo;
		NFCTerminal			= cSrc.NFCTerminal;
		NFCSerialNo			= cSrc.NFCSerialNo;
		NFCSignData			= cSrc.NFCSignData;
	}
	return *this;
}