#ifndef COMMONFRAMEMAIN_H
#define COMMONFRAMEMAIN_H


#define COMMONFRAMEMAIN_LIBRARY//add for exports

#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
   #include <wtypes.h>
    //#include <winbase.h>
    #include <iostream>
    #if defined(COMMONFRAMEMAIN_LIBRARY)
    #  define COMMONFRAMEMAINSHARED_EXPORT __declspec(dllexport)
    #else
    #  define COMMONFRAMEMAINSHARED_EXPORT __declspec(dllimport)
    #endif

#else
#  define COMMONFRAMEMAINSHARED_EXPORT
#endif
//////////////bad bad bad


#include "CommonBus.h"
#include <string>
#include <map>
#include<list>
#include <set>
#include "FrameConfigFileOperate.h"



//#pragma  comment(lib, "CommonBus.lib")

using namespace std;


#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
  typedef map<string,HINSTANCE> LibMap;
  typedef pair<string,HINSTANCE> PairLibMap;
  #define LOAD_LIB_PATH  "..\\loadLib\\"
  #define DOWNLOAD_FILE_PATH  "..\\download\\"
  #define COMMON_LIB_PATH     "..\\lib\\"
	#define dlsym GetProcAddress  //add 2017-11 zhuyabing for windows
#else
  typedef map<string,void* > LibMap;
  typedef pair<string,void* > PairLibMap;

#endif

typedef map<string,int> NameMap;
typedef pair<string,int > PairNameMap;

typedef int (*recvFromBus_X_def)(int destKey, char *msg, int len, int srcKey);

typedef int (*GenerateMainBus)(recvFromBus_X_def fun);
typedef int (*send2Bus_Target)(int main_handle,int destKey, char *msg, int len,int srcKey);
typedef void (*register2Bus_Target)(int main_handle,int key);
typedef void (*unRegister2Bus_Target)(int main_handle,int key);



class /*COMMONFRAMEMAINSHARED_EXPORT*/ CommonFrameMain/*:public CommonBus 2018-01，组件平台，不再静态加载动态库，以免出现工作路径问题*/
{

public:
	int send2Bus(int destKey,char *msg,int len,int srcKey=0);
private:
	void loadCommonBus();
	void register2Bus(int key);
    void unRegister2Bus(int key);
    
	send2Bus_Target m_send_core;
	register2Bus_Target m_register_core;
	unRegister2Bus_Target m_unRegister_core;
	int m_bushandle_core;
public:
	CRITICAL_SECTION dllMutex;
	struct NetPayInit1
	{
		UINT nThreadID;
		HWND hWnd;
		unsigned int nNotifyMsgID;
		std::string szAreaInfo;
		std::string szLoaclStationID;
		std::string szLoaclLaneID;
		std::string szServerInfo;
		int iProvinceID;
	};
	struct NetPayInit2
	{
		UINT nThreadID;
		HWND hWnd;
		std::string szAreaInfo;
		std::string szLoaclStation;
		std::string szLoaclLaneID;
		std::string szServerInfo;
		int iProvinceID;
		void (WINAPI*fun)(int option, int result);
	};

	struct NetPayInit3
	{
		UINT nThreadID;
		HWND hWnd;
		std::string szAreaInfo;
		std::string szLoaclStation;
		std::string szLoaclLaneID;
		std::string szServerInfo;
		int iProvinceID;
		void* pIndPtr;
		void (WINAPI*fun)(int option, int result, void* pIndPtr);
	};
	std::string moduleDir;
	int  initNumber;
	NetPayInit1 mInitInfo1;
	NetPayInit2 mInitInfo2;
	NetPayInit3 mInitInfo3;
	std::string updCode;
	void InitNetPay();
	std::set<std::string> ParseConfigDll(const std::string& configPath);
public:
    static CommonFrameMain* instance();
    static CommonFrameMain* self;
    virtual ~CommonFrameMain();
    int recvFromBus(int destKey,char *msg,int len, int srcKey=0); 
    /*加载动态库
     * filePath:为动态库文件夹路径，不包含动态库本身名称
     */
    void loadLib(string filePath,bool& load_flag);
   /**
    卸载动态库。
    fileName：只是动态库本身的名称
    */
    void unLoadLib(string fileName);

    void createThreadEx();

	static unsigned int  LoadDllSelf(void* arg);
	void LoadDllThread();
#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    static int threadFun(void *);
#else
    static void *threadFun(void *);
#endif
    void updateDll(string fileDllName);
    void moveFile2Lib(string fileName);
    void moveFile2loadLib(string fileName);


    void tryUpdateDLL();

     LibMap libMap;

     FrameConfigFileOperate myFrameConfigFileOperate;
     ST_FRAME_CONFIG_INFO myST_FRAME_CONFIG_INFO;
public:
	void SwitchToTarget(const std::string& targetVer);
	bool HasSwitchRequest();
	void ResetSwitchRequest();
	void ExcuteSwitch();
private:
	void LoadTargetDll(const std::string& path);
	CRITICAL_SECTION m_switchMutex;
	std::string m_CurrSystemVersion;//1.5 or 2.0
	bool m_NeedSwitch;
private:
    CommonFrameMain(); 
//    LibMap libMap;
    NameMap nameMap;
    string filePath;

    list<string> allDllList;
    list<string> updateDllList;
	std::string m_thisModulePath;

    bool isStateFlag;

    void readFileName();
    void* linuxLib;

	
#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    HINSTANCE hDLL;
#endif
};




class CScopeLock
{
public:
    CScopeLock(CRITICAL_SECTION* lck)
        : mMutex(lck)
    {
		if(mMutex!=NULL)
		{
			::EnterCriticalSection(mMutex);
		}
        
    }
    ~CScopeLock()
    {
		if(mMutex!=NULL)
		{
			 ::LeaveCriticalSection(mMutex);
		}
       
    }
private:
    CRITICAL_SECTION* mMutex;
private:
    CScopeLock(const CScopeLock&);
    CScopeLock& operator = ( const CScopeLock&);

};

//extern "C" COMMONFRAMEMAINSHARED_EXPORT CommonBus* getInstance(); //获取类TestDll的对象
//extern "C" COMMONFRAMEMAINSHARED_EXPORT CommonBus* deleteInstance(); //获取类TestDll的对象




///////////////////////////API    Interface////////////////////////////////////////////////////////

//初始化组件环境		    -- 接口1
/******************************************
 *参数列表
        1.串口号
        2.串口波特率
        3.代理服务器地址，可留空
        4.服务器链接端口，默认80（公网测试为8081）
        5.服务器是否开放端口，默认0（公网测试为1）
        6.是否产生日志标记（0或1），默认为1
        7.日志保存天数，整型，默认20
        8.区域信息
        9.本地站点信息
        10.本地车道号信息
        11.本地服务器信息
        12.省份编码
        13.回调函数声明
* ***************************************/
/*
extern "C" COMMONFRAMEMAINSHARED_EXPORT   bool WINAPI IF_InitEnvironment (IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID);
extern "C" COMMONFRAMEMAINSHARED_EXPORT   bool WINAPI IF_InitEnvironment2 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, void ( *fun)(int option, int result));
extern "C" COMMONFRAMEMAINSHARED_EXPORT   bool WINAPI IF_InitEnvironment3 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, IN OUT void* pIndPtr, void ( *fun)(int option, int result, void* pIndPtr));


//清理资源			-- 接口2
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_Destroy(void);
//获取上一次错误信息		-- 接口3
extern "C" COMMONFRAMEMAINSHARED_EXPORT  const char*  WINAPI IF_GetLastErrorDesc(void);
//启动获取用户信息操作		-- 接口4
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_StartGetAccountInfo( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_StopGetAccountInfo(void);
//获取用户信息				-- 接口6
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_GetComponentStatus( unsigned int& uStatus);
//执行传输关键信息数据操作                  -- 接口13
extern "C" COMMONFRAMEMAINSHARED_EXPORT  bool  WINAPI IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType);
*/

//////////////////////////////////////////////////////////////////////////////////////////////////







//为导出业务动态库 声明的接口指针

//函数定义
//初始化硬件
//
typedef bool  (WINAPI *CommonFrame_InitEnvironment) (IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID);
typedef bool  (WINAPI *CommonFrame_InitEnvironment2) (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, void (WINAPI *fun)(int option, int result));
typedef bool  (WINAPI *CommonFrame_InitEnvironment3) (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, IN OUT void* pIndPtr, void (WINAPI *fun)(int option, int result, void* pIndPtr));


//清理资源			-- 接口2
typedef bool  (WINAPI *CommonFrame_Destroy)(void);
//获取上一次错误信息
typedef char*       ( WINAPI *CommonFrame_GetLastErrorDesc) (void);
//启动获取用户信息操作		-- 接口4
typedef bool  (WINAPI *CommonFrame_StartGetAccountInfo) ( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
typedef bool  (WINAPI *CommonFrame_StopGetAccountInfo) (void);
//获取用户信息				-- 接口6
typedef bool  (WINAPI *CommonFrame_GetAccountInfo) (char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
typedef bool	(  WINAPI *CommonFrame_DebitMoney) ( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
typedef bool	( WINAPI *CommonFrame_DebitCancel) ( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
typedef bool	( WINAPI *CommonFrame_GetDebitResult) (char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
typedef bool	( WINAPI *CommonFrame_SetMMI) (const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
typedef bool	( WINAPI *CommonFrame_GetParam) ( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
typedef bool	( WINAPI *CommonFrame_GetComponentStatus) ( unsigned int& uStatus);
//执行传输关键信息数据操作                  -- 接口13
typedef bool (WINAPI *CommonFrame_TranslateData) ( const char* szParamContext, const int& iParamSize, const int& iFormatType);


typedef bool (WINAPI *CommonFrame_TransferImage)(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType);

typedef bool (WINAPI *CommonFrame_AgencyDebitQuery)(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, IN OUT int& iResult);



typedef int (WINAPI* CommonFrame_ITCEntryTrade)(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType, OUT char szResponse[2048], IN int iTimeout);
typedef int (WINAPI* CommonFrame_ITCExitPreTrade) (IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, OUT char szEntryInfo[2048], IN int iTimeout);
typedef int (WINAPI* CommonFrame_ITCExitTrade) (IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, OUT char szResponse[2048], IN int iTimeout);

typedef bool(WINAPI* CommonFrame_CompletionTicketNo)(IN const char* szParamContext,IN const int& iParamSize,IN const int& iFormatType);

bool WINAPI IF_Destroy(void);

//add 2018-05
typedef bool (WINAPI* CommonFrame_SetVehicleStatus)(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType);
typedef bool (WINAPI* CommonFrame_PrinterTicket)(IN  const  char* szParamContext, IN const int& iParamSize, IN const int& iFormatType);
typedef bool (WINAPI* CommonFrame_ReadCardInfo)(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType);
typedef bool (WINAPI* CommonFrame_WriteCardInfo)(IN OUT char* szParamContext,IN OUT int& iParamSize,IN const int& iFormatType);


#endif // COMMONFRAMEMAIN_H
