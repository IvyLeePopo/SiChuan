#ifndef COMMONFRAMEMAIN_H
#define COMMONFRAMEMAIN_H


#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    #include <wtypes.h>
    #include <winbase.h>
    #include <iostream>
    #if defined(COMMONFRAMEMAIN_LIBRARY)
    #  define COMMONFRAMEMAINSHARED_EXPORT __declspec(dllexport)
    #else
    #  define COMMONFRAMEMAINSHARED_EXPORT __declspec(dllimport)
    #endif

#else
#  define COMMONFRAMEMAINSHARED_EXPORT
#endif



#include <CommonBus.h>
#include <string>
#include <map>
#include<list>
#include "FrameConfigFileOperate.h"

using namespace std;


#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
  typedef map<string,HINSTANCE> LibMap;
  typedef pair<string,HINSTANCE> PairLibMap;
  #define LOAD_LIB_PATH  "..\\loadLib\\"
  #define DOWNLOAD_FILE_PATH  "..\\download\\"
  #define COMMON_LIB_PATH     "..\\lib\\"
#else
  typedef map<string,void* > LibMap;
  typedef pair<string,void* > PairLibMap;

#endif

typedef map<string,int> NameMap;
typedef pair<string,int > PairNameMap;

class COMMONFRAMEMAINSHARED_EXPORT CommonFrameMain:public CommonBus
{
public:
    static CommonFrameMain* instance();
    static CommonFrameMain* self;
    virtual ~CommonFrameMain();
    int recvFromBus(int destKey,char *msg,int len, int srcKey=0);
    /*加载动态库
     * filePath:为动态库文件夹路径，不包含动态库本身名称
     */
    void loadLib(string filePath);
   /**
    卸载动态库。
    fileName：只是动态库本身的名称
    */
    void unLoadLib(string fileName);

    void createThreadEx();
#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    static int threadFun(void *);
#else
    static void *threadFun(void *);
#endif
    void updateDll(string fileDllName);
    void moveFile2Lib(string fileName);
    void moveFile2loadLib(string fileName);
    void moveFile2Etc(string fileName);


    void tryUpdateDLL();

     LibMap libMap;

     FrameConfigFileOperate myFrameConfigFileOperate;
     ST_FRAME_CONFIG_INFO myST_FRAME_CONFIG_INFO;
private:
    CommonFrameMain();
//    LibMap libMap;
    NameMap nameMap;
    string filePath;

    list<string> allDllList;
    list<string> updateDllList;

    bool isStateFlag;

    void readFileName();
    void* linuxLib;
#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    HINSTANCE hDLL;
#endif
};

extern "C" COMMONFRAMEMAINSHARED_EXPORT CommonBus* getInstance(); //获取类TestDll的对象
extern "C" COMMONFRAMEMAINSHARED_EXPORT CommonBus* deleteInstance(); //获取类TestDll的对象




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
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool IF_InitEnvironment(
        const char *tty,
        int        baudRate,
        const char *serverIP,
        int        serverPort,
        bool       isOpenServerPort,
        bool       logFlag,
        int        logSaveTime,
        const char *szAreaInfo,
        const char *szLoaclStation,
        const char *szLoaclLaneID,
        const char *szServerInfo,
        const int  &iProvinceID,
        void (*fun)(int option, int result)
        );


//清理资源			-- 接口2
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_Destroy(void);
//获取上一次错误信息		-- 接口3
extern "C" COMMONFRAMEMAINSHARED_EXPORT const char*  IF_GetLastErrorDesc(void);
//启动获取用户信息操作		-- 接口4
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_StartGetAccountInfo( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_StopGetAccountInfo(void);
//获取用户信息				-- 接口6
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_GetComponentStatus( unsigned int& uStatus);
//执行传输关键信息数据操作                  -- 接口13
extern "C" COMMONFRAMEMAINSHARED_EXPORT bool  IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType);

//////////////////////////////////////////////////////////////////////////////////////////////////










//为导出业务动态库 声明的接口指针

//函数定义
//初始化硬件
typedef bool  (*CommonFrame_InitEnvironment)(const char *tty,
                                    int        baudRate,
                                    const char *serverIP,
                                    int        serverPort,
                                    bool       isOpenServerPort,
                                    bool       logFlag,
                                    int        logSaveTime,
                                    const char *szAreaInfo,
                                    const char *szLoaclStation,
                                    const char *szLoaclLaneID,
                                    const char *szServerInfo,
                                    const int  &iProvinceID,
                                    void (*fun)(int option, int result) );

//清理资源			-- 接口2
typedef bool  (*CommonFrame_Destroy)(void);
//获取上一次错误信息
typedef char*       ( *CommonFrame_GetLastErrorDesc) (void);
//启动获取用户信息操作		-- 接口4
typedef bool  (*CommonFrame_StartGetAccountInfo) ( const int& iOvertime);
//停止获取用户信息操作		-- 接口5
typedef bool  (*CommonFrame_StopGetAccountInfo) (void);
//获取用户信息				-- 接口6
typedef bool  (*CommonFrame_GetAccountInfo) (char* szParamContext, int& iParamSize,const int& iFormatType);
//执行扣款操作				-- 接口7
typedef bool	(  *CommonFrame_DebitMoney) ( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//执行撤单操作				-- 接口8
typedef bool	( *CommonFrame_DebitCancel) ( const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取扣款结果				-- 接口9
typedef bool	( *CommonFrame_GetDebitResult) (char* szParamContext, int& iParamSize, const int& iFormatType);
//设置交互界面				-- 接口10
typedef bool	( *CommonFrame_SetMMI) (const char* szParamContext, const int& iParamSize, const int& iFormatType);
//获取硬件参数				-- 接口11
typedef bool	( *CommonFrame_GetParam) ( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize);
//检测组件状态				-- 接口12
typedef bool	( *CommonFrame_GetComponentStatus) ( unsigned int& uStatus);
//执行传输关键信息数据操作                  -- 接口13
typedef bool (*CommonFrame_TranslateData) ( const char* szParamContext, const int& iParamSize, const int& iFormatType);







#endif // COMMONFRAMEMAIN_H
