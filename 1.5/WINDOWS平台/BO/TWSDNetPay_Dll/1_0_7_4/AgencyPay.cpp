#include "StdAfx.h"
#include "AgencyPay.h"

CAgencyPay::CAgencyPay(void)
{
	fnIF_InitEnvironment	= NULL;
	fnIF_Destroy			= NULL;
	fnIF_AgencyDebitQuery	= NULL;
	fnIF_AgencyDebitCancel	= NULL;
	fnIF_AgencyDebitMoney	= NULL;
	fnIF_AgencyDebitImage	= NULL;
	fnIF_GetAgencyErrorDesc	= NULL;
}

CAgencyPay::~CAgencyPay(void)
{
	if( NULL != m_hModule )
	{
		::FreeLibrary( m_hModule );
	}

	fnIF_InitEnvironment	= NULL;
	fnIF_Destroy			= NULL;
	fnIF_AgencyDebitQuery	= NULL;
	fnIF_AgencyDebitCancel	= NULL;
	fnIF_AgencyDebitMoney	= NULL;
	fnIF_AgencyDebitImage	= NULL;
	fnIF_GetAgencyErrorDesc	= NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CAgencyPay::Init
//     
// ������������ʼ����̬��
//     
// ���������IN const CString& strDllFilePath
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CAgencyPay::Init(IN const CString& strDllFilePath)
{
	if(NULL != (m_hModule = ::LoadLibrary(strDllFilePath)))
	{
		fnIF_InitEnvironment	= (defIF_InitEnvironment)::GetProcAddress(m_hModule,"IF_InitEnvironment");
		fnIF_Destroy			= (defIF_Destroy)::GetProcAddress(m_hModule,"IF_Destroy");
		fnIF_AgencyDebitQuery	= (defIF_AgencyDebitQuery)::GetProcAddress(m_hModule,"IF_AgencyDebitQuery");
		fnIF_AgencyDebitCancel	= (defIF_AgencyDebitCancel)::GetProcAddress(m_hModule,"IF_AgencyDebitCancel");
		fnIF_AgencyDebitMoney	= (defIF_AgencyDebitMoney)::GetProcAddress(m_hModule,"IF_AgencyDebitMoney");
		fnIF_AgencyDebitImage	= (defIF_AgencyDebitImage)::GetProcAddress(m_hModule,"IF_AgencyDebitImage");
		fnIF_GetAgencyErrorDesc	= (defIF_GetAgencyErrorDesc)::GetProcAddress(m_hModule,"IF_GetAgencyErrorDesc");

		if(		(NULL != fnIF_InitEnvironment)
			 && (NULL != fnIF_Destroy)
			 && (NULL != fnIF_AgencyDebitQuery)
			 && (NULL != fnIF_AgencyDebitCancel)
			 && (NULL != fnIF_AgencyDebitMoney)
			 && (NULL != fnIF_AgencyDebitImage)
			 && (NULL != fnIF_GetAgencyErrorDesc)
			 )
		{
			m_bInitDll = TRUE;
		}
	}
	return m_bInitDll;
}











int CAgencyPay::InitEnvironment(IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID)
{
	if(m_bInitDll)
	{
		int iRet = 0;
		iRet = fnIF_InitEnvironment(szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);
		if( 0 == iRet)
		{
			m_bInitEnvironment = TRUE;
		}
		else
		{
			m_bInitEnvironment = FALSE;
		}
		return iRet;
	}
	return 255;
}




int CAgencyPay::Destroy(void)
{
	if(m_bInitDll)
	{
		int iRet = fnIF_Destroy();

		if( m_hModule )
		{
			::FreeLibrary( m_hModule );
		}
		m_hModule = NULL;
		return iRet;
	}
	return 255;
}




CString CAgencyPay::GetLastErrorDesc(int err_no, IN OUT char msg[1024])
{
	CString	csRet = _T("");
	if(m_bInitDll)
	{
		fnIF_GetAgencyErrorDesc(err_no, msg);
		csRet.Format( _T("%s"), msg );
	}
	return csRet;
}




int CAgencyPay::AgencyDebitQuery(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	if(m_bInitDll && m_bInitEnvironment)
		return fnIF_AgencyDebitQuery(szParamContext, iParamSize, iFormatType);
	return 255;
}



int CAgencyPay::AgencyDebitCancel(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	if(m_bInitDll && m_bInitEnvironment)
		return fnIF_AgencyDebitCancel(szParamContext, iParamSize, iFormatType);
	return 255;
}




int CAgencyPay::AgencyDebitMoney(IN const char* szParamContext, IN OUT char* szResultContext,IN const int& iParamSize,IN const int& iFormatType)
{
	if(m_bInitDll && m_bInitEnvironment)
		return fnIF_AgencyDebitMoney(szParamContext, szResultContext, iParamSize, iFormatType);
	return 255;
}




int CAgencyPay::AgencyDebitImage(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType)
{
	if(m_bInitDll && m_bInitEnvironment)
		return fnIF_AgencyDebitImage(szParamContext, iParamSize, iFormatType);
	return 255;
}



