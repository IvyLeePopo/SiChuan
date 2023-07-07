// proj_tr.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "proj_tr.h"
#include "..\..\src\third\CoreBusiness_Future.h"

#include "..\..\src\third\SuperLog.h"
#include "..\..\src\third\ThreadHelper.h"

#include "..\..\src\third\sql\sqlite3_agent.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//      则从此 DLL 导出的任何调入
//      MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//      该函数的最前面。
//
//      例如:
//
//      extern "C" BOOL PASCAL EXPORT ExportedFunction()
//      {
//          AFX_MANAGE_STATE(AfxGetStaticModuleState());
//          // 此处为普通函数体
//      }
//
//      此宏先于任何 MFC 调用
//      出现在每个函数中十分重要。这意味着
//      它必须作为函数中的第一个语句
//      出现，甚至先于所有对象变量声明，
//      这是因为它们的构造函数可能生成 MFC
//      DLL 调用。
//
//      有关其他详细信息，
//      请参阅 MFC 技术说明 33 和 58。
//



CCoreBusiness_Future* g_CoreThread = NULL;
bool g_ExitDll = false;
CRITICAL_SECTION* g_API_LOCK = new CRITICAL_SECTION;


int DoCreateCore()
{
    if (g_CoreThread)
    {
        return 0 ;
    }
    if (g_ExitDll)
    {
        return 0;
    }


    TW_TRACE_DIARY_TYPE("开始创建业务主线程", TW_DIARY_MSG_THREAD_API);
    g_CoreThread = (CCoreBusiness_Future*)::AfxBeginThread(RUNTIME_CLASS(CCoreBusiness_Future), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    if (g_CoreThread)
    {
        TW_TRACE_DIARY_TYPE("开始启动业务主线程...", TW_DIARY_MSG_THREAD_API);
        g_CoreThread->m_bAutoDelete = false;
        g_CoreThread->ResumeThread();
        TW_TRACE_DIARY_TYPE("启动业务主线程完毕", TW_DIARY_MSG_THREAD_API);
    }
    else
    {
        TW_TRACE_DIARY_TYPE("业务主线程创建失败", TW_DIARY_MSG_THREAD_API);
    }

    return 0;
}
int TryCreateCore()
{
    int i = DoCreateCore();
    return i;
}

int DoEndCore()
{

    if (g_CoreThread)
    {
        TW_TRACE_DIARY_TYPE("开始退出业务主线程", TW_DIARY_MSG_THREAD_API);
        delete g_CoreThread;
        g_CoreThread = NULL;
        TW_TRACE_DIARY_TYPE("退出业务主线程完毕", TW_DIARY_MSG_THREAD_API);
    }
    return 0;
}

int TryEndCore()
{
    //MessageBox(0,"退出HTDEV","",0);
    int i = DoEndCore();
    return i;
}

int WINAPI  soadd(int i)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return 0;
}
int WINAPI haha()
{
    return 0;
}
bool WINAPI IF_HAHA(char* i)
{
    return false;
}

bool WINAPI IF_InitDev(NotifyFun fun)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
    TryCreateCore();
    //return false;
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_InitDevice_L(fun);
    }

    return false;
}


bool WINAPI IF_Destroy()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);

    if (g_CoreThread && !g_ExitDll)
    {
        bool flag = g_CoreThread->IF_Terminate_L();
        TryEndCore();
        return flag;
    }

    return false;
}
bool WINAPI IF_InitPara( const std::string& paraStr, int paraType )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);

    if (g_CoreThread && !g_ExitDll)
    {
        //TW_TRACE_DIARY("WPIAPI:PARA:"+paraStr);
        return g_CoreThread->IF_InitPara_L(paraStr, paraType);
    }

    return false;
}
bool WINAPI IF_SetDevice( const std::string& paraStr, int paraType )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);

    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_SetDevice_L(paraStr, paraType);
    }

    return false;
}
bool WINAPI IF_QueryDevice( int queryType, int resultType, std::string& resultStr )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
    std::string resultInstead;

    if (g_CoreThread && !g_ExitDll)
    {
        //bool flag = g_CoreThread->IF_QueryDevice_L(queryType, resultType, resultInstead);
        //resultStr.Format(_T("%s"),resultInstead.c_str());
        return g_CoreThread->IF_QueryDevice_L(queryType, resultType, resultStr);
    }

    return false;
}
bool WINAPI IF_StartDebit( const std::string& paraStr, int paraType , CString& resultStr) //std::string& resultStr)
{

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
    std::string resultInstead = "";

    if (g_CoreThread && !g_ExitDll)
    {
        bool flag = g_CoreThread->IF_StartDebit_L(paraStr, paraType, resultInstead);

        if (!resultInstead.empty())
        {
            resultStr.Format(_T("%s"), resultInstead.c_str());
        }
        return flag;
        //return g_CoreThread->IF_StartDebit_L(paraStr, paraType,resultStr);

    }

    return false;
}
bool WINAPI IF_StartDebitCancle( const std::string& paraStr, int  paraType )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);

    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_StartDebitCancle_L(paraStr);
    }

    return false;
}
bool WINAPI IF_GetLastError(CString& errorInfo)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
    std::string resultInstead = "";


    if (g_CoreThread && !g_ExitDll)
    {
        bool flag = g_CoreThread->IF_GetLastError_L(resultInstead);
        if (!resultInstead.empty())
        {
            errorInfo.Format(_T("%s"), resultInstead.c_str());
        }

        return flag;

    }

    return false;
}
bool WINAPI IF_GetLastResult(int& type, std::string& resultStr)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);


    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_GetLastResult_L(type, resultStr);
    }

    return false;
}

//////////////////////////////////////////////////////////////
// 函数名称:IF_BreakDebit
//
// 功能描述:此接口，用于中断交易流程
//
// 输入参数:const std::string& payid,
// 输出参数:bool WINAPI
// 编写人员:朱亚冰
// 编写时间:2017-09-22
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
// 由于需要终端交易流程，故此流程需要具有与其他流程同步的能力，因此不会使用锁进行限制
//
//////////////////////////////////////////////////////////////
bool WINAPI IF_BreakDebit(const std::string& payid)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //CScopeLock accessAPI(g_API_LOCK);

    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_BreakDebit_L(payid);
    }

    return false;
}

//////////////////////////////////////////////////////////////
// 函数名称:IF_CheckDeviceStatus
//
// 功能描述：获取设备状态信息（例如：网络，扫码头，NFC）
//
// 输入参数:IN OUT DWORD& dwState,
// 输出参数:bool WINAPI
// 编写人员:朱亚冰
// 编写时间:2017-09-22
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool WINAPI  IF_CheckDeviceStatus(IN OUT DWORD& dwState)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    /*modify zhuyabing 2017-10 根据陕西遇到的情况，先屏蔽该反馈信息*/
    //dwState = 0;
    //return true;

    //CScopeLock accessAPI(g_API_LOCK);
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_CheckDeviceStatus_L(dwState);
    }

    return false;
}

bool WINAPI IF_PlaySound( const std::string& paraStr, int paraType )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_PlaySound_L(paraStr, paraType);
    }

    return false;

}



bool WINAPI IF_ShowText( const std::string& paraStr, int paraType )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_ShowText_L(paraStr, paraType);
    }

    return false;

}


//add 2018-01
bool WINAPI IF_AgencyDebitQuery(IN const std::string& szParamContext, IN const int& iFormatType, IN OUT int& iResult)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
	
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_AgencyDebitQuery_L( szParamContext,  iFormatType, iResult);
    }
    return false;
}


bool WINAPI IF_TransferImage(IN const std::string& image_para)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
	
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_TransferImage_L(image_para);
    }
	return false;
}

bool WINAPI IF_TransTickNo(const std::string& para)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
	
    if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_TransTickNo_L(para);
    }
	return false;
}
bool WINAPI IF_ExcuteComCommond(const std::string& hexStr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
	if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_ExcuteComCommond_L(hexStr);
    }
	return false;
}

bool WINAPI IF_TakeComFeedbackCommond(CString& jsonStr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);
	if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_TakeComFeedbackCommond_L(jsonStr);
    }
	return false;
}

bool WINAPI GreenChannelTest(int testType,const std::string& para,char* result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CScopeLock accessAPI(g_API_LOCK);

    if (g_CoreThread && !g_ExitDll)
    {
		std::string result_str;
        bool ret = g_CoreThread->GreenChannelTest_L(testType,para,result_str);
		if(!result_str.empty())
		{
			strcpy(result,result_str.c_str());
		}
		return ret;
    }
	return false;
}


bool WINAPI IF_TerminalSystemVersionStatus(int& status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CScopeLock accessAPI(g_API_LOCK);
	if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_TerminalSystemVersionStatus_L(status);
		
    }
	 return false;
	// return true;
}
bool WINAPI IF_ExchangeTerminalSystem(const int& targetSystemVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CScopeLock accessAPI(g_API_LOCK);
	if (g_CoreThread && !g_ExitDll)
    {
        return g_CoreThread->IF_ExchangeTerminalSystem_L(targetSystemVersion);
		
    }
	 return false;
}


// Cproj_trApp

BEGIN_MESSAGE_MAP(Cproj_trApp, CWinApp)
END_MESSAGE_MAP()



//////////////////////////////////////////////////////////////
// 函数名称:TmpGetDumpDir
//
// 功能描述:用于生成Dump文件的保存路径。
//
// 输入参数:
// 输出参数:std::string
// 编写人员:朱亚冰
// 编写时间:2017-09-22
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string TmpGetDumpDir()
{
    char modePath[300] = {0};
    memset(modePath, 0, 260);

    GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);

    std::string pathStr = modePath;
    size_t pos = pathStr.find_last_of('\\');
    if (pos == std::string::npos)
    {
        return "";
    }
    pathStr = pathStr.substr(0, pos) + "\\TWSDNetPayLog";
    return pathStr;
}
// Cproj_trApp 构造

Cproj_trApp::Cproj_trApp()//:m_pCatchDump(NULL)
{

}


// 唯一的一个 Cproj_trApp 对象

Cproj_trApp theApp;


// Cproj_trApp 初始化

BOOL Cproj_trApp::InitInstance()
{
    CWinApp::InitInstance();
    afxCurrentInstanceHandle = _AtlBaseModule.GetModuleInstance();
    afxCurrentResourceHandle = _AtlBaseModule.GetResourceInstance();
    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中



	//add 2018-04 修改sqlite的加载模式
	TCHAR szTempDir[260] = {0};
    ::GetModuleFileName(AfxGetInstanceHandle(), szTempDir, sizeof(TCHAR) * 260);
    CString strTemp(szTempDir);
    CString strDir;
    strDir = strTemp.Left(strTemp.ReverseFind('\\'));
  

	//add 2018-04 初始化sqlite
	CString sqlite_dll_file;
	sqlite_dll_file.Format(_T("%s\\sqlite3.dll"), strDir);
	sqlite3_agent::LoadSqlite(sqlite_dll_file);

    std::string tmpStr = TmpGetDumpDir();
    CString strDumpFilePath;
    strDumpFilePath.Format(_T("%s\\DumpFile.dmp"), tmpStr.c_str());

    //MessageBox(0,"退出HTDEV","",0);
    ::InitializeCriticalSection(g_API_LOCK);
    //MessageBox(0,"退出dddddd","",0);

    /*
    m_pCatchDump = new CCatchDump(1);

    if(NULL!=m_pCatchDump)
    {
        m_pCatchDump->InitCatchExecption(strDumpFilePath,FALSE,_T(""),2);
    }
	*/
    
    return TRUE;
}
BOOL Cproj_trApp::ExitInstance()
{
    //MessageBox(0,"退出HTDEV","",0);
    g_ExitDll = true;
    ::Sleep(500);/*此处的代码是为了等待调用的结束*/
	{
		CScopeLock accessAPI(g_API_LOCK);
		TryEndCore();
	}
    

    ::DeleteCriticalSection(g_API_LOCK);
    QUIT_LOG();
	sqlite3_agent::UnloadSqlite();

    /*
    if(m_pCatchDump)
    {
        delete m_pCatchDump;
        m_pCatchDump = NULL;
    }
	*/
    

    return CWinApp::ExitInstance();
}
