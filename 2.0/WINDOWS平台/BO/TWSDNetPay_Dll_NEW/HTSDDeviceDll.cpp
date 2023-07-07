#include "StdAfx.h"
#include "HTSDDeviceDll.h"

CHTSDDeviceDll::CHTSDDeviceDll(void)
	:m_hModule(NULL)
	,m_bInitDll(FALSE)
	,m_bInitDev(FALSE)
	,m_strLastError(_T(""))
	,fnIF_InitDev(NULL)
	,fnIF_Destroy(NULL)
	,fnIF_GetLastErrorDesc(NULL)
	,fnIF_CheckDeviceStatus(NULL)
	,fnIF_GetData(NULL)
	,fnIF_Authority(NULL)
	,fnIF_GetDeviceFirewareVer(NULL)
	,fnIF_GetDisplayContext(NULL)
	,fnIF_SetDisplayContext(NULL)
	,fnIF_IssuedData(NULL)
	,fnIF_SetIOState(NULL)

{
}

CHTSDDeviceDll::~CHTSDDeviceDll(void)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CSDNetPayDll::Init
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
const BOOL CHTSDDeviceDll::Init(IN const CString& strDllFilePath)
{
	//��ȡ��̬���ļ�����·��Ŀ¼
	CString strDir = strDllFilePath.Left(strDllFilePath.ReverseFind('\\'));
	//��ȡ��ǰ����Ŀ¼
	TCHAR szOldDir[260] = {0};
	::GetCurrentDirectory(260,szOldDir);
	//���ù���Ŀ¼
	::SetCurrentDirectory(strDir);
	//���ض�̬��
	if(NULL != (m_hModule = ::LoadLibrary(strDllFilePath)))
	{
		fnIF_InitDev				= (defIF_InitDev)::GetProcAddress(m_hModule,"IF_InitDev");
		fnIF_Destroy				= (defIF_Destroy)::GetProcAddress(m_hModule,"IF_Destroy");
		fnIF_GetLastErrorDesc		= (defIF_GetLastErrorDesc)::GetProcAddress(m_hModule,"IF_GetLastErrorDesc");
		fnIF_CheckDeviceStatus		= (defIF_CheckDeviceStatus)::GetProcAddress(m_hModule,"IF_CheckDeviceStatus");
		fnIF_StopGetData			= (defIF_StopGetData)::GetProcAddress(m_hModule,"IF_StopGetData");

		fnIF_GetData				= (defIF_GetData)::GetProcAddress(m_hModule,"IF_GetData");
		fnIF_Authority				= (defIF_Authority)::GetProcAddress(m_hModule,"IF_Authority");
		fnIF_GetDeviceFirewareVer	= (defIF_GetDeviceFirewareVer)::GetProcAddress(m_hModule,"IF_GetDeviceFirewareVer");
		fnIF_GetDisplayContext		= (defIF_GetDisplayContext)::GetProcAddress(m_hModule,"IF_GetDisplayContext");
		fnIF_SetDisplayContext		= (defIF_SetDisplayContext)::GetProcAddress(m_hModule,"IF_SetDisplayContext");
		fnIF_IssuedData				= (defIF_IssuedData)::GetProcAddress(m_hModule,"IF_IssuedData");
		fnIF_SetIOState				= (defIF_SetIOState)::GetProcAddress(m_hModule,"IF_SetIOState");
		fnIF_DebitMoney				= (defIF_DebitMoney)::GetProcAddress(m_hModule,"IF_DebitMoney");
		fnIF_DebitCancel			= (defIF_DebitCancel)::GetProcAddress(m_hModule,"IF_DebitCancel");		
		fnIF_InitDeviecCMD          = (defIF_InitDeviecCMD)::GetProcAddress(m_hModule,"IF_InitConfigDevice");

		if(		(NULL != fnIF_InitDev)
			&& (NULL != fnIF_Destroy)
			&& (NULL != fnIF_GetLastErrorDesc)
			&& (NULL != fnIF_CheckDeviceStatus)
			&& (NULL != fnIF_StopGetData)
			&& (NULL != fnIF_GetData)
			&& (NULL != fnIF_Authority)
			&& (NULL != fnIF_GetDeviceFirewareVer)
			&& (NULL != fnIF_GetDisplayContext)
			&& (NULL != fnIF_SetDisplayContext)
			&& (NULL != fnIF_IssuedData)
			&& (NULL != fnIF_SetIOState)
			&& (NULL != fnIF_DebitMoney)
			&& (NULL != fnIF_DebitCancel)
			&& (NULL != fnIF_InitDeviecCMD)
			)
		{
			m_bInitDll = TRUE;
		}
	}
	//�ظ�����Ŀ¼
	::SetCurrentDirectory(szOldDir);
	//����ִ��״̬
	return m_bInitDll;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::InitDev
//     
// ���������������ʼ��
//     
// ���������IN const UINT& nThreadID
//         ��IN const HWND& hWnd
//         ��IN const unsigned int& nNotifyMsgID
//         ��IN const int& iProvinceID
//         ��IN const int& iOverTime
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��24��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::InitDev(IN const UINT& nThreadID,IN const HWND& hWnd,IN const unsigned int& nNotifyMsgID,IN const int& iProvinceID,IN const int& iOverTime)
{
	if(m_bInitDll)
	{
		m_bInitDev = fnIF_InitDev(nThreadID,hWnd,nNotifyMsgID,iProvinceID,iOverTime);
		return m_bInitDev;
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::Destroy
//     
// ����������������Դ
//     
// ���������void
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::Destroy(void)
{
	if(m_bInitDll)
		return fnIF_Destroy();
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::GetLastErrorDesc
//     
// ������������ȡ��һ�δ�����Ϣ
//     
// ���������void
// ���������CString
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
CString CHTSDDeviceDll::GetLastErrorDesc(void)
{
	if(m_bInitDll)
		return fnIF_GetLastErrorDesc();
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::StartGetData
//     
// ����������ִ��������ȡ���ݲ���
//     
// ���������void
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::StartGetData(IN const int& iOvertime)
{
	if(m_bInitDll && m_bInitDev)
	{}
	//		return fnIF_StartGetData(iOvertime);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::StopGetData
//     
// ����������ִ��ֹͣ��ȡ���ݲ���
//    
// ���������void
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::StopGetData(void)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_StopGetData();
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::CheckDeviceStatus
//     
// ����������ִ�м��Ӳ��״̬����
//     
// ���������IN OUT DWORD& dwState
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::CheckDeviceStatus(IN OUT DWORD& dwState)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_CheckDeviceStatus(dwState);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::GetData
//     
// ������������ȡ���ݽ��
//     
// ���������IN OUT CString& strData
//         ��IN OUT int& iDataType
//         ��IN OUT int& iSrcType
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::GetData(IN OUT CString& strData,IN OUT WORD& iDataType,IN OUT int& iSrcType)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_GetData(strData,iDataType,iSrcType);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::Authority
//     
// ����������Ȩ����֤����
//     
// ���������IN const CString& strData
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::Authority(IN const CString& strData)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_Authority(strData);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::GetDeviceFirewareVer
//     
// ������������ȡ�̼��汾��Ϣ
//     
// ���������IN OUT CString& strXML
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::GetDeviceFirewareVer(IN OUT CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_GetDeviceFirewareVer(strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::GetDisplayContext
//     
// ������������ȡ��ʾ������Ϣ
//     
// ���������IN OUT int& iID
//         ��CString& strXML
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::GetDisplayContext(IN OUT int& iID, CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_GetDisplayContext(iID,strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::SetDisplayContext
//     
// ����������������ʾ������Ϣ
//     
// ���������IN const int& iID
//         ��IN const CString& strXML
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��8��4��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::SetDisplayContext(IN const int& iID,IN const CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_SetDisplayContext(iID,strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::IssuedData
//     
// ����������ִ�������·�����
//     
// ���������IN const CString& strXML
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::IssuedData(IN const CString& strXML)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_IssuedData(strXML);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�SetIOState
//     
// ����������ִ�п���IO����
//     
// ���������IN const int& iIOState
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2016��7��23��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::SetIOState(IN const int& iIOState)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_SetIOState(iIOState);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::DebitMoney
//     
// ����������ִ�пۿ����
//     
// ���������CString& strDebitInfo
//         ��int iType
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��5��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::DebitMoney(IN const CString& strDebitInfo,IN const int& inType)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_DebitMoney(strDebitInfo,inType);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::DebitCancel
//     
// ����������ִ���˿����
//     
// ���������IN const CString& strCancelInfo
//         ��IN const int& inType
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��5��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::DebitCancel(IN const CString& strCancelInfo,IN const int& inType)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_DebitCancel(strCancelInfo,inType);
	return FALSE;

}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CHTSDDeviceDll::InitConfigDevice
//     
// ����������ִ���豸��ʼ������
//     
// ���������IN const CString& strInitInfo
//         ��IN const int& inType
// ���������BOOL
// ��д��Ա��ROCY
// ��дʱ�䣺2017��5��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHTSDDeviceDll::InitConfigDevice(IN const CString& strInitInfo,IN const int& inType)
{
	if(m_bInitDll && m_bInitDev)
		return fnIF_InitDeviecCMD(strInitInfo,inType);
	return FALSE;
}
