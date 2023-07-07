#pragma once




typedef int	(WINAPI * defIF_InitEnvironment)(IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID);
typedef int (WINAPI * defIF_Destroy)(void);
typedef int (WINAPI * defIF_AgencyDebitQuery)(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
typedef int (WINAPI * defIF_AgencyDebitCancel)(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
typedef int (WINAPI * defIF_AgencyDebitMoney)(IN const char* szParamContext, IN OUT char* szResultContext,IN const int& iParamSize,IN const int& iFormatType);
typedef int (WINAPI * defIF_AgencyDebitImage)(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
typedef void (WINAPI * defIF_GetAgencyErrorDesc)(int err_no, IN OUT char msg[1024]);




class CAgencyPay
{
public:
	CAgencyPay(void);
	virtual ~CAgencyPay(void);

public:
	//初始化动态库
	const BOOL Init(IN const CString& strDllFilePath);
	//获取组件加载状态
	const BOOL GetDllInitState(void){return m_bInitDll;}
	//获取组件环境初始化状态
	const BOOL GetInitEnvironmentState(void){return m_bInitEnvironment;}

//Operations
public:
	int InitEnvironment(IN const char* szAreaInfo,IN const char* szLoaclStation,IN const char* szLoaclLaneID,IN const char* szServerInfo,IN const int& iProvinceID);
	int Destroy(void);
	CString	GetLastErrorDesc(int err_no, IN OUT char msg[1024]);

	int AgencyDebitQuery(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	int AgencyDebitCancel(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);
	int AgencyDebitMoney(IN const char* szParamContext, IN OUT char* szResultContext,IN const int& iParamSize,IN const int& iFormatType);
	int AgencyDebitImage(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);

//Attributes
private:
	HMODULE	m_hModule;			//动态库加载句柄
	BOOL	m_bInitDll;			//动态库初始化标记
	BOOL	m_bInitEnvironment;	//组件初始化标记

	//函数定义变量
	defIF_InitEnvironment		fnIF_InitEnvironment;
	defIF_Destroy				fnIF_Destroy;
	defIF_AgencyDebitQuery		fnIF_AgencyDebitQuery;
	defIF_AgencyDebitCancel		fnIF_AgencyDebitCancel;
	defIF_AgencyDebitMoney		fnIF_AgencyDebitMoney;
	defIF_AgencyDebitImage		fnIF_AgencyDebitImage;
	defIF_GetAgencyErrorDesc	fnIF_GetAgencyErrorDesc;
};
