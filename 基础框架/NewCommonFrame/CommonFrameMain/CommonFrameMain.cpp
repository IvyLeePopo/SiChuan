#include "CommonFrameMain.h"


#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
#include <WINDOWS.H>
#include <process.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include<dlfcn.h>
#include <pthread.h>
#endif
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <set>
#include "pystring.h"
#include <algorithm>
#include <fstream>


//#include "fs_funcs.h"

using namespace std;
class ChangeWorkDirOnce
{
private:
    std::string old_dir;
public:
    ChangeWorkDirOnce(std::string new_dir)
    {
        TCHAR szOldDir[260] = {0};
        ::GetCurrentDirectory(260, szOldDir);
        old_dir = std::string(szOldDir);
        ::SetCurrentDirectory(new_dir.c_str());
    }
    ~ChangeWorkDirOnce()
    {
        if (!old_dir.empty())
        {
            ::SetCurrentDirectory(old_dir.c_str());
        }
    }
};


#include "CommonBus.h"
void addddddd()
{
    CommonBus* tmp_p = NULL;
    tmp_p->register2Bus(0);
}

//此版本是为了做出兼容优化，
int recvFromBus_X(int destKey, char *msg, int len, int srcKey)
{
    return CommonFrameMain::instance()->recvFromBus(destKey, msg, len, srcKey);
}

unsigned int CoreFun(void*)
{
    CommonFrameMain::instance();
    Sleep(100);
    return 0;

}
void OutPutDbg(std::string info)
{
	OutputDebugString(info.c_str());
}

int DoCreateCoreThread()
{
    _beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void*))CoreFun, 0, 0, NULL);
    return 0;
}
void TryCreateCoreThr()
{
    static int i = DoCreateCoreThread();
}

void InitUpdateCore(IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID);


CommonFrameMain *CommonFrameMain::self = NULL;

//modify 2917-11 zhuyabing
//CommonFrameMain *CommonFrameMain::self = new CommonFrameMain();
bool g_debug_interface = false;

typedef CommonBus* (*getUnitInstance)();//定义函数指针,获取类CommonBus对象;
typedef CommonBus* (*deleteUnitInstance)();//定义函数指针,获取类CommonBus对象;


/*
CommonBus *getInstance()
{
    return CommonFrameMain::instance();
}

CommonBus *deleteInstance()
{
    delete CommonFrameMain::self;
    return NULL;
}
*/


CommonFrameMain *CommonFrameMain::instance()
{
    if (self == NULL)
        self =  new CommonFrameMain();
    return self;
}

CommonFrameMain::~CommonFrameMain()
{
    unRegister2Bus(KEY_COMMON_FRAME);
    self = NULL;
    ::DeleteCriticalSection(&dllMutex);
    ::DeleteCriticalSection(&m_switchMutex);
}
CommonFrameMain::CommonFrameMain()
{

    locale::global(locale(""));//将全局区域设为操作系统默认区域,处理中文路径问题
    ::InitializeCriticalSection(&dllMutex);
    ::InitializeCriticalSection(&m_switchMutex);

    m_CurrSystemVersion = "1.5";
    m_NeedSwitch = false;
    updCode = "123";
    initNumber = -1;
    isStateFlag = false;
    //commpentsKey = KEY_COMMON_FRAME;

    linuxLib = NULL;
#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    hDLL = NULL;
#endif
    memset(&myST_FRAME_CONFIG_INFO, 0, sizeof(ST_FRAME_CONFIG_INFO));
    if (false == myFrameConfigFileOperate.parseFilePathConfig(&myST_FRAME_CONFIG_INFO, m_thisModulePath))
    {
        printf("[#CommonFrameMain#] read %s failed.", FILE_PATH_CONFIG_PATH);
        exit(0);
    }

    m_send_core = NULL;
    m_register_core = NULL;
    m_unRegister_core = NULL;
    m_bushandle_core = 0;
    loadCommonBus();
    register2Bus(KEY_COMMON_FRAME);


    moduleDir  = pystring::os::path::dirname(myST_FRAME_CONFIG_INFO.loadLibPath);
    bool unuse_flag;
    loadLib(myST_FRAME_CONFIG_INFO.loadLibPath, unuse_flag);
    //LoadDllThread();
    LoadTargetDll(myST_FRAME_CONFIG_INFO.netPayPath);

//   downloadFilePath = myST_FRAME_CONFIG_INFO.downloadPath;
    createThreadEx();

    //tryUpdateDLL(); //debug

}


std::set<std::string> CommonFrameMain::ParseConfigDll(const std::string& configPath)
{
    std::set<std::string> dllList;
    std::ifstream configStream(configPath.c_str());
    //MessageBox(0,configPath.c_str(),"",0);
    if (!configStream.is_open())
    {
        return dllList;
    }

    std::istreambuf_iterator<char> beg(configStream), end;
    std::string configJson(beg, end);
    configStream.close();

    Json::Reader readerInfo;
    Json::Value rootN;
    if (!readerInfo.parse(configJson, rootN))
    {
        return dllList;
    }

    Json::Value::Members allMems = rootN.getMemberNames();
    for (Json::Value::Members::const_iterator it = allMems.begin(); it != allMems.end(); ++it)
    {
        if (rootN[*it].type() == Json::stringValue)
        {
            std::string fullName = pystring::os::path::join(myST_FRAME_CONFIG_INFO.loadLibPath, rootN[*it].asString());

            dllList.insert(pystring::lower(fullName));
        }
    }
    return dllList;
}


unsigned int CommonFrameMain::LoadDllSelf(void* arg)
{
    CommonFrameMain *pCommonFrameMain = (CommonFrameMain *)arg;
    bool unuse;
    pCommonFrameMain->loadLib(pCommonFrameMain->myST_FRAME_CONFIG_INFO.loadLibPath, unuse);
    return 0;

}
void CommonFrameMain::LoadDllThread()
{
    _beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))LoadDllSelf, (LPVOID)this, 0, NULL);

}

#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)

int CommonFrameMain::recvFromBus(int destKey, char *msg, int len, int srcKey)
{
    if (srcKey == KEY_AUTO_UPDATE_MANAGER)
    {
        isStateFlag = true;
        updCode = "123";
        if (msg != NULL)
        {
            updCode = std::string(msg);
        }
    }
    else if (srcKey == KEY_BUSINESS)
    {
        Json::Value rootN;
        Json::Reader jsonReader;
        if (!jsonReader.parse(msg,rootN))
        {
            return -1;
        }
        std::string dataType;
        if (rootN["Type"].type() != Json::stringValue)
        {
            return -1;
        }
        dataType = rootN["Type"].asString();
        if (dataType == "ExchSystem")
        {
            std::string switchTo;
            if (rootN["SwitchTo"].type() != Json::stringValue)
            {
                return -1;
            }
            switchTo = rootN["SwitchTo"].asString();
            SwitchToTarget(switchTo);

        }

    }

    return 0;
}

void CommonFrameMain::createThreadEx()
{
    _beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))threadFun, (LPVOID)this, 0, NULL);

}
int CommonFrameMain::threadFun(void* arg)
{
    CommonFrameMain *pCommonFrameMain = (CommonFrameMain *)arg;

    /*
        Sleep(1000*10);//debug
        while(true)
        {
             pCommonFrameMain->tryUpdateDLL();
             Sleep(1000*10);
        }
    */

    {
        static int i = 0;
        if (i == 0)
        {
            i = 100;
            // pCommonFrameMain->isStateFlag = true;
            // Sleep(1000 * 50); //debug
        }
    }
	/*
	while(true)
	{
		Sleep(20*1000);
		pCommonFrameMain->ExcuteSwitch();
	}
	*/

    while (true)
    {
        if (pCommonFrameMain->HasSwitchRequest())
        {
           pCommonFrameMain->ExcuteSwitch();
		   pCommonFrameMain->ResetSwitchRequest();
		   
        }

        if (pCommonFrameMain->isStateFlag)
        {
            Sleep(1000 * 2);
            pCommonFrameMain->tryUpdateDLL();
        }
        pCommonFrameMain->isStateFlag = false;
        Sleep(1000 * 2);
    }

}

void CommonFrameMain::tryUpdateDLL()
{
    int df = 123123;
    // CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    //MessageBox(0,"","",0);
    string szPath = std::string(myST_FRAME_CONFIG_INFO.downloadPath) + "\\*.dll";
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    int i = 0;
    /* remove
    int bufSize = MultiByteToWideChar(CP_ACP, 0, szPath.c_str(), -1, NULL, 0);

    wchar_t* wp = new wchar_t[bufSize];
    memset(wp, 0, bufSize);
    MultiByteToWideChar(CP_ACP, 0, szPath.c_str(), -1, wp, bufSize);
    //wcout.imbue(locale("chs"));

    hFind = FindFirstFile(wp, &wfd);
    */
    std::set<std::string> dll_check_list;//组件列表
    std::set<std::string> dll_list;//所有dll列表
    hFind = FindFirstFile(szPath.c_str(), &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return;
    }
    std::string configFilePath;
    string oldFileName;
    oldFileName.clear();
    do
    {
        /*
        int fileSize = WideCharToMultiByte(CP_ACP, NULL, wfd.cFileName, -1, NULL, 0, NULL, FALSE);
        char *sp = new char[fileSize];
        memset(sp, 0, fileSize);
        WideCharToMultiByte(CP_ACP, NULL, wfd.cFileName, -1, sp, fileSize, NULL, FALSE);
        */

        if (strcmp( wfd.cFileName, ".") == 0) //文件 "." 固定的格式，但是不需要
            continue;
        if (strcmp(wfd.cFileName, "..") == 0) //文件 ".." 固定的格式，但是不需要
            continue;
        if (strstr(wfd.cFileName, ".zip") != NULL) //文件 "." 固定的格式，但是不需要
            continue;
        if (strstr(wfd.cFileName, ".tar") != NULL) //文件 "." 固定的格式，但是不需要
            continue;
        if (!oldFileName.empty() && strstr(wfd.cFileName, oldFileName.c_str()) != NULL) //文件 ".." 固定的格式，但是不需要
            continue;
        if ( stricmp("filePathConfig.txt", wfd.cFileName) == 0) //动态库配置文件
        {
            configFilePath = pystring::os::path::join(myST_FRAME_CONFIG_INFO.downloadPath, wfd.cFileName);
            dll_list.insert(configFilePath);
            continue;
        }
        string dllName;
        char *pTemp = strstr(wfd.cFileName, ".dll.");
        if (pTemp != NULL) //文件 ".." 固定的格式，但是不需要
        {
            char name[256] = {0};
            memcpy(name, wfd.cFileName, strlen(wfd.cFileName) - strlen(pTemp));
            dllName.append(name);
            dllName.append(".dll");
        }
        else
        {
            dllName = wfd.cFileName;
        }
        //MessageBox(0,dllName.c_str(),"",0);
        std::string updPath = pystring::os::path::join(myST_FRAME_CONFIG_INFO.downloadPath, dllName);
        updPath = pystring::lower(updPath);
        std::string dstPath = pystring::os::path::join(moduleDir, dllName);
        dstPath = pystring::lower(dstPath);

        dll_list.insert(updPath);

        list<string>::iterator it; //bu neng  fu  NULL
        bool findFlag = false;
        for (it = allDllList.begin(); it != allDllList.end(); it++)
        {
            if (stricmp(dstPath.c_str(), it->c_str()) == 0)
            {
                findFlag = true;
                break;
            }
        }

        if (findFlag ) //组件库，放入loadLib,//ptr->d_name  .so.1
        {
            //首先得判断是否可升级，如果能够升级，先卸载动态库，再移动文件，
            printf("[#CommonFrameMain::tryUpdateDLL#]移动动态库 %s 到 %s ", wfd.cFileName, LOAD_LIB_PATH);
            string findDllName = LOAD_LIB_PATH;
            findDllName.append(dllName);
            //MessageBox(0,dllName.c_str(),"",0);
            dll_check_list.insert(dstPath);

            // NameMap::iterator nameMapIt = nameMap.find(findDllName);
            // if (nameMapIt != nameMap.end())
            // {
            //     int key = nameMapIt->second;
            //     int state = send2Bus(key, NULL, 0, KEY_COMMON_FRAME); //获取组件的是否可升级状态
            //     if (state == 1)
            //     {
            //         unLoadLib(dllName);
            //         moveFile2loadLib(dllName);
            //         printf("dllName=%s.\n", dllName.c_str());
            //         updateDll(dllName); //这里进行升级操作
            //     }
            //     else
            //     {
            //         printf("[#CommonFrameMain::tryUpdateDLL#] 组件%s正在做业务，请等待！", wfd.cFileName);
            //     }
            // }
            // else
            // {
            //     printf("[##CommonFrameMain::tryUpdateDLL##]:can not update dllName=%s,there have not this service\n", dllName.c_str());
            // }
        }
        else
        {
            //moveFile2Lib(wfd.cFileName);
        }
        //delete[] sp;
    }
    while (FindNextFile(hFind, &wfd));
    FindClose(hFind);

    //如果有新的配置文件，则需要根据配置文件判断哪些内容需要删除
    if (!configFilePath.empty())
    {
        std::set<std::string> newConfigDll = this->ParseConfigDll(configFilePath);
        if (!newConfigDll.empty())
        {
            for (std::list<std::string>::iterator it = allDllList.begin(); it != allDllList.end(); ++it)
            {
                if ( newConfigDll.count(*it) != 0)
                {
                    dll_check_list.insert(*it);
                }
            }
        }
    }

    // add
    while (true)
    {
        bool canUpdate = true;
        for (std::set<std::string>::iterator it =  dll_check_list.begin(); it != dll_check_list.end(); ++it)
        {
            //MessageBox(0,it->c_str(),"222",0);
            NameMap::iterator nameMapIt = nameMap.find(*it);
            if (nameMapIt != nameMap.end())
            {
                int key = nameMapIt->second;

                char chs_debug[10] = {0};
                sprintf(chs_debug, "%d", key);
                //MessageBox(0,chs_debug,"",0);

                int state = send2Bus(key, NULL, 0, KEY_COMMON_FRAME); //获取组件的是否可升级状态
                if (state == 0)
                {
                    //MessageBox(0,"what dddd","",0);
                    canUpdate = false;
                    break;
                }
            }
        }
        if (canUpdate)
        {
            break;
        }
        Sleep(1000 * 1);

    }
    //g_debug_interface = true;
    //Sleep(10*1000);
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);//wait for dll idle

    //MessageBox(0,"unload dll","",0);
    for (std::set<std::string>::iterator it =  dll_check_list.begin(); it != dll_check_list.end(); ++it)
    {
        //MessageBox(0, it->c_str(), "111", 0);
        unLoadLib(*it);//dll_check_list中的都是需要卸载的
        //updateDll(*it); //这里进行升级操作
    }

    std::set<std::string> dllBak_queue;
    for (std::set<std::string>::iterator it =  dll_list.begin(); it != dll_list.end(); ++it)
    {
        //MessageBox(0,it->c_str(),"222",0);

        //add 2018-01 首先，备份之前的文件，然后，将新文件移动过去
        std::string dll_name = pystring::os::path::basename(*it);
        std::string old_name = pystring::os::path::join(m_thisModulePath, dll_name);
        std::string back_name = old_name + ".dllbak";
        dllBak_queue.insert(back_name);
        if (!MoveFileEx(old_name.c_str(), back_name.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
        {
            //error
        }
        moveFile2loadLib(*it);
        //updateDll(*it); //这操作
    }
    bool load_flag = true;
    //MessageBox(0,"load dll","",0);
    loadLib("", load_flag);
    int update_state = 3;
    if (!load_flag)
    {

        for (std::set<std::string>::iterator it =  dll_check_list.begin(); it != dll_check_list.end(); ++it)
        {
            unLoadLib(*it);//dll_check_list中的都是需要卸载的
        }
        //恢复之前的版本
        for (std::set<std::string>::iterator it = dllBak_queue.begin(); it != dllBak_queue.end(); ++it)
        {
            size_t pos = it->find(".dllbak");
            std::string final_name = it->substr(0, pos);
            if (!MoveFileEx(it->c_str(), final_name.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
            {
                //error
            }
        }
        Sleep(1000);//2018-01-03 如果此处不进行Sleep，会出现崩溃
        loadLib("", load_flag);
        update_state = 4;
    }
    else
    {
        //删除dllbak文件
        for (std::set<std::string>::iterator it = dllBak_queue.begin(); it != dllBak_queue.end(); ++it)
        {
            remove(it->c_str());
        }
    }

    /*
    for (std::set<std::string>::iterator it =  dll_check_list.begin(); it != dll_check_list.end(); ++it)
    {
        //MessageBox(0,it->c_str(),"333",0);
        updateDll(*it); //这里进行升级操作

    }*/
    Json::Value rootN;
    rootN["code"] = Json::Value(updCode);
    rootN["result"] = Json::Value(update_state);
    Json::FastWriter jfw;
    std::string code_str = jfw.write(rootN);

    std::ofstream file_tmp;

    std::string flag_path = pystring::os::path::join(moduleDir, "updateflag.txt");
    file_tmp.open(flag_path.c_str());
    file_tmp.write(code_str.c_str(), code_str.size());
    file_tmp.close();

    //init
    InitNetPay();

    //
    Sleep(2 * 1000);

    //this->send2Bus(KEY_AUTO_UPDATE_MANAGER, tmpCode, strlen(tmpCode), KEY_COMMON_FRAME);




    //delete[] wp;
}
void CommonFrameMain::moveFile2loadLib(string fileName)
{

    std::string destPath = pystring::os::path::join(moduleDir, pystring::os::path::basename(fileName));

    if (!MoveFileEx(fileName.c_str(), destPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
    {
        return ;
        char debug_chs[100] = {0};
        sprintf(debug_chs, "%d", GetLastError());
        //MessageBox(0,debug_chs,"",0);
        return ;//error
    }
    return ;
    //rename(fileName.c_str(),destPath.c_str());
    return ;
    string cmd = "move /Y ";//
    cmd.append(fileName);
    cmd.append(" ");
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(destPath);
    //MessageBox(0,cmd.c_str(),"",0);
    system(cmd.c_str());
    printf(cmd.c_str());

}

void CommonFrameMain::moveFile2Lib(string fileName)
{
    printf("[#CommonFrameMain::moveFile#]移动 %s 到 %s", fileName.c_str(), ".");
    string cmd = "move /Y ";
    cmd.append(DOWNLOAD_FILE_PATH);
    cmd.append(fileName);
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(" ."); //组件依赖库，放入lib
    system(cmd.c_str());
}
void CommonFrameMain::updateDll(string fileDllName)
{
    string fileName =  fileDllName;

    /*remove
    int bufSize = MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, NULL, 0);
    wchar_t* wp = new wchar_t[bufSize];
    memset(wp, 0, bufSize);
    MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, wp, bufSize);
    hDLL = LoadLibrary(wp);
    */
    hDLL = LoadLibrary(fileName.c_str());
    if (hDLL < (HMODULE)HINSTANCE_ERROR)
    {
        //delete[] wp;
        // wp  = NULL;
        return;
    }
    libMap.insert(PairLibMap(fileName, hDLL));
    getUnitInstance pGetInstance = (getUnitInstance)GetProcAddress(hDLL, "getInstance");
    if (pGetInstance)
        pGetInstance();
    else
        printf("The commpents must surrport getInstance funtion api can load\n");

    //delete[] wp;
    //wp  = NULL;
}

void CommonFrameMain::loadLib(string filePath, bool& load_flag)
{
    //remove this info ChangeWorkDirOnce tmpChange(this->m_thisModulePath);
    readFileName();
    list<string>::iterator it;
    for (it = allDllList.begin(); it != allDllList.end(); it++)
    {
        // MessageBox(0,it->c_str(),"",0);
        string fileName = *it; //= filePath;
        // fileName.append(*it);
        //int bufSize = MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, NULL, 0);
        /*
        wchar_t* wp = new wchar_t[bufSize];
        memset(wp, 0, bufSize);
        MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, wp, bufSize);
        hDLL = LoadLibrary(wp);
        */
        hDLL = LoadLibrary(fileName.c_str());

        if (hDLL == NULL)
        {
            char chs_dbg[100] = {0};
            sprintf(chs_dbg, "%d", GetLastError());

            printf("can't load fileName=.\n", fileName);

            load_flag = false;
            //delete [] wp;
            // wp = NULL;
            continue;
        }
        LibMap::iterator libIt = libMap.find(fileName);

        if (libIt != libMap.end())
        {
            printf("commpents have loaded,%s can't load again！\n", fileName);
            FreeLibrary(hDLL);
            // delete [] wp;
            // wp = NULL;
            continue;
        }
        else
        {
            //MessageBox(0,fileName.c_str(),"加载完成",0);
            libMap.insert(PairLibMap(fileName, hDLL));
            getUnitInstance pGetInstance = (getUnitInstance)GetProcAddress(hDLL, "getInstance");
            if (pGetInstance)
            {
                CommonBus *pCommonBus = pGetInstance();
                int key = pCommonBus->commpentsKey;
                nameMap.insert(PairNameMap(fileName, key));
                //MessageBox(0,fileName.c_str(),"222",0);
            }
            else
            {
                printf("The commpents must surrport getInstance funtion api can load\n");
                printf("%s\n", fileName.c_str());
                //load_flag = false;
            }

        }
        //delete [] wp;
        //wp = NULL;
        hDLL = NULL;
    }
    //    send2Bus(1,"qwe",3,1);
}

void CommonFrameMain::unLoadLib(string fileName)
{
    string dirPath = LOAD_LIB_PATH;
    string filePath = dirPath + fileName;
    LibMap::iterator libIt = libMap.find(fileName);

    std::string dll_base_name = pystring::os::path::basename(fileName);
    //业务库直接调用
    if (stricmp(dll_base_name.c_str(), "twsdnetpaycore_v1.dll") == 0
            || stricmp(dll_base_name.c_str(), "twsdnetpaycore_v2.dll") == 0)
    {
        ::IF_Destroy();
    }
    else//其他库，动态调用
    {
        LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(fileName);
        if ( libIt != CommonFrameMain::instance()->libMap.end())
        {
            CommonFrame_Destroy pGetInstance = (CommonFrame_Destroy)dlsym(libIt->second, "IF_Destroy");
            if (pGetInstance)
                pGetInstance();
        }
    }

    if (libIt != libMap.end())
    {
        printf("start unload commpents %s.\n", fileName.c_str());
        HINSTANCE hDLL = libIt->second;
        deleteUnitInstance pDeleteInstance = (deleteUnitInstance)GetProcAddress(hDLL, "deleteInstance");
        //MessageBox(0,"unload","",0);
        if (1 || pDeleteInstance)//去掉依赖
        {
            //MessageBox(0,"unload over","",0);
            libMap.erase(libIt);//warning 此处需要添加挥斥锁，避免其他线程同步操作
            //pDeleteInstance();
            FreeLibrary(hDLL);

        }
        else
            printf("The commpents must surrport deleteInstance funtion api can unload\n");
    }
    else
        printf("the commpents has not load or had unloaded.\n");
}

void CommonFrameMain::readFileName()
{
    allDllList.clear();
    //MessageBox(0,myST_FRAME_CONFIG_INFO.configFilePath,"",0);
    std::set<std::string> validDlls = this->ParseConfigDll(myST_FRAME_CONFIG_INFO.configFilePath);

    std::copy(validDlls.begin(), validDlls.end(), std::back_inserter<std::list<std::string>>(allDllList));
    return ;

    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    int i = 0;
    /*
    int bufSize = MultiByteToWideChar(CP_ACP, 0, szPath.c_str(), -1, NULL, 0);

    wchar_t* wp = new wchar_t[bufSize];
    memset(wp, 0, bufSize);
    MultiByteToWideChar(CP_ACP, 0, szPath.c_str(), -1, wp, bufSize);
    //wcout.imbue(locale("chs"));
    hFind = FindFirstFile(wp, &wfd);
    */
    std::string searchFile = pystring::os::path::join(moduleDir, "*.dll");

    hFind = FindFirstFile(searchFile.c_str(), &wfd);
    do
    {
        /*
        int fileSize = WideCharToMultiByte(CP_ACP, NULL, wfd.cFileName, -1, NULL, 0, NULL, FALSE);
        char *sp = new char[fileSize];
        memset(sp, 0, fileSize);
        WideCharToMultiByte(CP_ACP, NULL, wfd.cFileName, -1, sp, fileSize, NULL, FALSE);
        //    strcpy(fileList[i],sp); //存储到数组
        allDllList.push_back(sp);
        delete[] sp;
        */

        //MessageBox(0,wfd.cFileName,"",0);
        for (std::set<std::string>::iterator it = validDlls.begin(); it != validDlls.end(); ++it)
        {
            std::string dllBaseName = pystring::os::path::basename(*it);
            if (stricmp(dllBaseName.c_str(), wfd.cFileName) == 0)
            {
                //MessageBox(0,wfd.cFileName,"",0);

                allDllList.push_back(pystring::os::path::join(moduleDir, wfd.cFileName));
            }
        }


        //        if (++i>=LIB_NUM)
        //            break;
    }
    while (FindNextFile(hFind, &wfd));
    FindClose(hFind);

    // delete[] wp;
}
#else


#endif

//=========================================================



bool  WINAPI IF_InitEnvironment (IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID)
{
	OutPutDbg("init1");
    //TryCreateCoreThr();
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);

    InitUpdateCore(nThreadID, hWnd, nNotifyMsgID, szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);

    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {

        HINSTANCE unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment pGetInstance = (CommonFrame_InitEnvironment)dlsym(unLoadLibDp, "IF_InitEnvironment");
        if (pGetInstance)
        {
            printf("init:%s\n", netpayPath.c_str());
            //return false;
            bRet =  pGetInstance( nThreadID, hWnd,  nNotifyMsgID, IN  szAreaInfo,  szLoaclStation,  szLoaclLaneID, szServerInfo, iProvinceID);
            //MessageBox(0,bRet?"ok":"fail","",0);
        }
    }

    //add 2018-04 ITC
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if ( bRet && libIt != CommonFrameMain::instance()->libMap.end())
    {
        HINSTANCE unLoadLibDp = libIt->second;
        CommonFrame_InitEnvironment pGetInstance = (CommonFrame_InitEnvironment)dlsym(unLoadLibDp, "IF_InitEnvironment");
        if (pGetInstance)
        {
            bRet =  pGetInstance( nThreadID, hWnd,  nNotifyMsgID, IN  szAreaInfo,  szLoaclStation,  szLoaclLaneID, szServerInfo, iProvinceID);
        }
    }

    if (CommonFrameMain::instance()->initNumber == -1)
    {
        CommonFrameMain::instance()->initNumber = 1;
        CommonFrameMain::instance()->mInitInfo1.nThreadID = nThreadID;
        CommonFrameMain::instance()->mInitInfo1.hWnd = hWnd;
        CommonFrameMain::instance()->mInitInfo1.nNotifyMsgID = nNotifyMsgID;
        CommonFrameMain::instance()->mInitInfo1.szAreaInfo = szAreaInfo;
        CommonFrameMain::instance()->mInitInfo1.szLoaclStationID = szLoaclStation;
        CommonFrameMain::instance()->mInitInfo1.szLoaclLaneID = szLoaclLaneID;
        CommonFrameMain::instance()->mInitInfo1.szServerInfo = szServerInfo;
        CommonFrameMain::instance()->mInitInfo1.iProvinceID = iProvinceID;
    }

    //CommonFrameMain::instance()->tryUpdateDLL();
    return bRet;
}
bool  WINAPI IF_InitEnvironment2 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, void (WINAPI *fun)(int option, int result))
{   
	
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);

    if (CommonFrameMain::instance()->initNumber == -1)
    {
        CommonFrameMain::instance()->initNumber = 2;
        CommonFrameMain::instance()->mInitInfo2.nThreadID = nThreadID;
        CommonFrameMain::instance()->mInitInfo2.hWnd = hWnd;
        CommonFrameMain::instance()->mInitInfo2.szAreaInfo = szAreaInfo;
        CommonFrameMain::instance()->mInitInfo2.szLoaclStation = szLoaclStation;
        CommonFrameMain::instance()->mInitInfo2.szLoaclLaneID = szLoaclLaneID;
        CommonFrameMain::instance()->mInitInfo2.szServerInfo = szServerInfo;
        CommonFrameMain::instance()->mInitInfo2.iProvinceID = iProvinceID;
        CommonFrameMain::instance()->mInitInfo2.fun = fun;
    }
    InitUpdateCore(nThreadID, hWnd, 0, szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        HINSTANCE unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment2 pGetInstance = (CommonFrame_InitEnvironment2)dlsym(unLoadLibDp, "IF_InitEnvironment2");
        if (pGetInstance)
        {
            bRet =  pGetInstance(  nThreadID,  hWnd,  szAreaInfo,  szLoaclStation, IN  szLoaclLaneID,  szServerInfo, iProvinceID, fun);
        }

    }

    //add 2018-04 for itc
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if (bRet &&  libIt != CommonFrameMain::instance()->libMap.end())
    {
        HINSTANCE unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment2 pGetInstance = (CommonFrame_InitEnvironment2)dlsym(unLoadLibDp, "IF_InitEnvironment2");
        if (pGetInstance)
        {
            bRet =  pGetInstance(  nThreadID,  hWnd,  szAreaInfo,  szLoaclStation, IN  szLoaclLaneID,  szServerInfo, iProvinceID, fun);
        }

    }

    return bRet;
}
bool   WINAPI IF_InitEnvironment3 (IN const UINT& nThreadID, IN const HWND& hWnd, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID, IN OUT void* pIndPtr, void (WINAPI *fun)(int option, int result, void* pIndPtr))
{
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    if (CommonFrameMain::instance()->initNumber == -1)
    {
        CommonFrameMain::instance()->initNumber = 3;
        CommonFrameMain::instance()->mInitInfo3.nThreadID = nThreadID;
        CommonFrameMain::instance()->mInitInfo3.hWnd = hWnd;
        CommonFrameMain::instance()->mInitInfo3.szAreaInfo = szAreaInfo;
        CommonFrameMain::instance()->mInitInfo3.szLoaclStation = szLoaclStation;
        CommonFrameMain::instance()->mInitInfo3.szLoaclLaneID = szLoaclLaneID;
        CommonFrameMain::instance()->mInitInfo3.szServerInfo = szServerInfo;
        CommonFrameMain::instance()->mInitInfo3.iProvinceID = iProvinceID;
        CommonFrameMain::instance()->mInitInfo3.pIndPtr = pIndPtr;
        CommonFrameMain::instance()->mInitInfo3.fun = fun;
    }
    InitUpdateCore(nThreadID, hWnd, 0, szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        HINSTANCE unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment3 pGetInstance = (CommonFrame_InitEnvironment3)dlsym(unLoadLibDp, "IF_InitEnvironment3");
        if (pGetInstance)
        {
            bRet =  pGetInstance( nThreadID,  hWnd,  szAreaInfo,  szLoaclStation,  szLoaclLaneID,  szServerInfo, iProvinceID,  pIndPtr, fun);
        }

    }

    //add 2018-04 for itc
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if (bRet && libIt != CommonFrameMain::instance()->libMap.end())
    {
        HINSTANCE unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment3 pGetInstance = (CommonFrame_InitEnvironment3)dlsym(unLoadLibDp, "IF_InitEnvironment3");
        if (pGetInstance)
        {
            bRet =  pGetInstance( nThreadID,  hWnd,  szAreaInfo,  szLoaclStation,  szLoaclLaneID,  szServerInfo, iProvinceID,  pIndPtr, fun);
        }

    }


    return bRet;
}



//add 2018-04 所有组件，都调用该接口
//清理资源          -- 接口2
bool WINAPI IF_Destroy(void)
{
	OutPutDbg("IF_Destroy");
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_Destroy pGetInstance = (CommonFrame_Destroy)dlsym(libIt->second, "IF_Destroy");
        if (pGetInstance)
            bRet =  pGetInstance();
    }

    //add 2018-04 for itc
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_Destroy pGetInstance = (CommonFrame_Destroy)dlsym(libIt->second, "IF_Destroy");
        if (pGetInstance)
            bRet =  pGetInstance();
    }
    return bRet;
}

//获取上一次错误信息       -- 接口3
const char* WINAPI IF_GetLastErrorDesc(void)
{
	OutPutDbg("IF_GetLastErrorDesc");
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    static string strError;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetLastErrorDesc pGetInstance = (CommonFrame_GetLastErrorDesc)dlsym(libIt->second, "IF_GetLastErrorDesc");
        if (pGetInstance)
            strError =  pGetInstance();
    }

    return strError.c_str();

}

//启动获取用户信息操作        -- 接口4
bool WINAPI IF_StartGetAccountInfo( const int& iOvertime)
{   
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_StartGetAccountInfo pGetInstance = (CommonFrame_StartGetAccountInfo)dlsym(libIt->second, "IF_StartGetAccountInfo");
        if (pGetInstance)
            bRet =  pGetInstance(iOvertime);
    }

    return bRet;

}

//停止获取用户信息操作        -- 接口5
bool  WINAPI IF_StopGetAccountInfo(void)
{   CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_StopGetAccountInfo pGetInstance = (CommonFrame_StopGetAccountInfo)dlsym(libIt->second, "IF_StopGetAccountInfo");
        if (pGetInstance)
            bRet =  pGetInstance();
    }

    return bRet;

}

//获取用户信息                -- 接口6
bool WINAPI  IF_GetAccountInfo(char* szParamContext, int& iParamSize, const int& iFormatType)
{   CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetAccountInfo pGetInstance = (CommonFrame_GetAccountInfo)dlsym(libIt->second, "IF_GetAccountInfo");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

//执行扣款操作                -- 接口7
bool WINAPI IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{

    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_DebitMoney pGetInstance = (CommonFrame_DebitMoney)dlsym(libIt->second, "IF_DebitMoney");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;

}

//执行撤单操作                -- 接口8
bool  WINAPI IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{   CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_DebitCancel pGetInstance = (CommonFrame_DebitCancel)dlsym(libIt->second, "IF_DebitCancel");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

//获取扣款结果                -- 接口9
bool WINAPI IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType)
{   CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetDebitResult pGetInstance = (CommonFrame_GetDebitResult)dlsym(libIt->second, "IF_GetDebitResult");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;

}

//设置交互界面                -- 接口10
bool WINAPI IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
	OutPutDbg("IF_SetMMI");
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_SetMMI pGetInstance = (CommonFrame_SetMMI)dlsym(libIt->second, "IF_SetMMI");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

//获取硬件参数                -- 接口11
bool WINAPI IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize)
{   CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);


    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetParam pGetInstance = (CommonFrame_GetParam)dlsym(libIt->second, "IF_GetParam");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType, szResultContext, iResultSize);
    }

    return bRet;
}

//检测组件状态                -- 接口12
bool WINAPI IF_GetComponentStatus( unsigned int& uStatus)
{   
	OutPutDbg("IF_GetComponentStatus");
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet;

    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetComponentStatus pGetInstance = (CommonFrame_GetComponentStatus)dlsym(libIt->second, "IF_GetComponentStatus");
        if (pGetInstance)
            bRet =  pGetInstance(uStatus);
    }

    return bRet;
}

//执行传输关键信息数据操作                  -- 接口13
bool  WINAPI IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_TranslateData pGetInstance = (CommonFrame_TranslateData)dlsym(libIt->second, "IF_TranslateData");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}



//
bool  WINAPI IF_TransferImage(IN const char* szImageInfo, IN const int& iSize, IN const int& iFormatType)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_TransferImage pGetInstance = (CommonFrame_TransferImage)dlsym(libIt->second, "IF_TransferImage");
        if (pGetInstance)
            bRet =  pGetInstance( szImageInfo, iSize,  iFormatType);
    }

    return bRet;
}

bool  WINAPI IF_QueryAgentDebit(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, IN OUT int& iResult)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_AgencyDebitQuery pGetInstance = (CommonFrame_AgencyDebitQuery)dlsym(libIt->second, "IF_AgencyDebitQuery");
        if (pGetInstance)
            bRet =  pGetInstance( szParamContext, iParamSize,  iFormatType, iResult);
    }

    return bRet;
}




void InitUpdateCore(IN const UINT& nThreadID, IN const HWND& hWnd, IN const unsigned int& nNotifyMsgID, IN const char* szAreaInfo, IN const char* szLoaclStation, IN const char* szLoaclLaneID, IN const char* szServerInfo, IN const int& iProvinceID)
{
    string updateCorePath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.updateCorePath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(updateCorePath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        HINSTANCE unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment pGetInstance = (CommonFrame_InitEnvironment)dlsym(unLoadLibDp, "IF_InitEnvironment");
        if (pGetInstance)
        {
            pGetInstance( nThreadID, hWnd, nNotifyMsgID, szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID);
        }

    }
}

void CommonFrameMain::InitNetPay()
{
    if (initNumber == 1)
    {
        IF_InitEnvironment(mInitInfo1.nThreadID, mInitInfo1.hWnd, mInitInfo1.nNotifyMsgID,
                           mInitInfo1.szAreaInfo.c_str(), mInitInfo1.szLoaclStationID.c_str(),
                           mInitInfo1.szLoaclLaneID.c_str(), mInitInfo1.szServerInfo.c_str(),
                           mInitInfo1.iProvinceID);
    }
    else if (initNumber == 2)
    {

        IF_InitEnvironment2(mInitInfo2.nThreadID, mInitInfo2.hWnd,
                            mInitInfo2.szAreaInfo.c_str(), mInitInfo2.szLoaclStation.c_str(),
                            mInitInfo2.szLoaclLaneID.c_str(), mInitInfo2.szServerInfo.c_str(),
                            mInitInfo2.iProvinceID, mInitInfo2.fun);
    }
    else if (initNumber == 3)
    {
        IF_InitEnvironment3(mInitInfo3.nThreadID, mInitInfo3.hWnd,
                            mInitInfo3.szAreaInfo.c_str(), mInitInfo3.szLoaclStation.c_str(),
                            mInitInfo3.szLoaclLaneID.c_str(), mInitInfo3.szServerInfo.c_str(),
                            mInitInfo3.iProvinceID, mInitInfo3.pIndPtr, mInitInfo3.fun);
    }
}


void CommonFrameMain::loadCommonBus()
{
    std::string commonBusPath = pystring::os::path::join(m_thisModulePath, "CommonBus.dll");

    HMODULE tmp_module = ::LoadLibrary(commonBusPath.c_str());
    if (tmp_module == NULL)
    {
        return ; //no such dll
    }
    //MessageBox(0,commonBusPath.c_str(),"",0);
    GenerateMainBus tmp_main = (GenerateMainBus)::GetProcAddress(tmp_module, "GenerateMainBus");
    m_send_core = (send2Bus_Target)::GetProcAddress(tmp_module, "send2Bus_Target");
    m_register_core = (register2Bus_Target)::GetProcAddress(tmp_module, "register2Bus_Target");
    m_unRegister_core = (unRegister2Bus_Target)::GetProcAddress(tmp_module, "unRegister2Bus_Target");
    if (tmp_main != NULL)
    {
        //MessageBox(0,"tmp_main","",0);
        m_bushandle_core = tmp_main(recvFromBus_X);
    }
    if (m_send_core == NULL
            || m_register_core == NULL
            || m_unRegister_core == NULL)
    {
        //MessageBox(0,"all","",0);
    }


}
void CommonFrameMain::register2Bus(int key)
{
    if (m_register_core != NULL)
    {
        m_register_core(m_bushandle_core, key);
    }
}
void CommonFrameMain::unRegister2Bus(int key)
{
    if (m_unRegister_core != NULL)
    {
        m_unRegister_core(m_bushandle_core, key);
    }

}
int CommonFrameMain::send2Bus(int destKey, char *msg, int len, int srcKey)
{
    if (m_send_core != NULL)
    {
        //MessageBox(0,"send2bus","00",0);
        return m_send_core(m_bushandle_core, destKey, msg, len, srcKey);
    }
    return -1;
}


//add 2018-04 新增的ITC相关的接口

int WINAPI IF_ITCEntryTrade(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType, OUT char szResponse[2048], IN int iTimeout)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    int bRet = -1;
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_ITCEntryTrade pGetInstance = (CommonFrame_ITCEntryTrade)dlsym(libIt->second, "IF_ITCEntryTrade");
        if (pGetInstance)
            bRet =  pGetInstance( szParamContext,  iParamSize, iFormatType, szResponse,  iTimeout);
    }

    return bRet;
}

int WINAPI IF_ITCExitPreTrade (IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, OUT char szEntryInfo[2048], IN int iTimeout)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    int bRet = -1;
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_ITCExitPreTrade pGetInstance = (CommonFrame_ITCExitPreTrade)dlsym(libIt->second, "IF_ITCExitPreTrade");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType,  szEntryInfo, iTimeout);
    }

    return bRet;
}

int WINAPI IF_ITCExitTrade (IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType, OUT char szResponse[2048], IN int iTimeout)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    int bRet = -1;
    string itc_path = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.itc_path;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(itc_path);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_ITCExitTrade pGetInstance = (CommonFrame_ITCExitTrade)dlsym(libIt->second, "IF_ITCExitTrade");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize,  iFormatType, szResponse, iTimeout);
    }

    return bRet;
}


bool WINAPI IF_CompletionTicketNo(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{

    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_CompletionTicketNo pGetInstance = (CommonFrame_CompletionTicketNo)dlsym(libIt->second, "IF_CompletionTicketNo");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

bool WINAPI IF_SetVehicleStatus(IN const char* szParamContext, IN const int& iParamSize, IN const int& iFormatType)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);

    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_CompletionTicketNo pGetInstance = (CommonFrame_CompletionTicketNo)dlsym(libIt->second, "IF_SetVehicleStatus");
        if (pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

bool WINAPI IF_PrinterTicket(IN  const char* szParamContext, IN  int& iParamSize, IN const int& iFormatType)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_CompletionTicketNo pGetInstance = (CommonFrame_CompletionTicketNo)dlsym(libIt->second, "IF_PrinterTicket");
        if (pGetInstance)
            bRet = pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}


bool WINAPI IF_ReadCardInfo(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_ReadCardInfo pGetInstance = (CommonFrame_ReadCardInfo)dlsym(libIt->second, "IF_ReadCardInfo");
        if (pGetInstance)
            bRet = pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

bool WINAPI IF_WriteCardInfo(IN OUT char* szParamContext, IN OUT int& iParamSize, IN const int& iFormatType)
{
    CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if ( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_WriteCardInfo pGetInstance = (CommonFrame_WriteCardInfo)dlsym(libIt->second, "IF_WriteCardInfo");
        if (pGetInstance)
            bRet = pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}
void CommonFrameMain::SwitchToTarget(const std::string& targetVer)
{
    if (targetVer == m_CurrSystemVersion)
    {
        return ;
    }
    CScopeLock tmpL(&m_switchMutex);
    m_NeedSwitch = true;
}
bool CommonFrameMain::HasSwitchRequest()
{
    CScopeLock tmpL(&m_switchMutex);
    return m_NeedSwitch;
    // return false;
}

void CommonFrameMain::ResetSwitchRequest()
{
	CScopeLock tmpL(&m_switchMutex);
    m_NeedSwitch = false;
}
void CommonFrameMain::ExcuteSwitch()
{
	CScopeLock tmpL(&CommonFrameMain::instance()->dllMutex);
    Sleep(1000);
    if (m_CurrSystemVersion == "1.5")
    {
        unLoadLib(myST_FRAME_CONFIG_INFO.netPayPath);
        strcpy(myST_FRAME_CONFIG_INFO.netPayPath, myST_FRAME_CONFIG_INFO.netPayPath_V2);
        LoadTargetDll(myST_FRAME_CONFIG_INFO.netPayPath);
        InitNetPay();
        m_CurrSystemVersion = "2.0";
    }
    else if (m_CurrSystemVersion == "2.0")
    {
        unLoadLib(myST_FRAME_CONFIG_INFO.netPayPath);
        strcpy(myST_FRAME_CONFIG_INFO.netPayPath, myST_FRAME_CONFIG_INFO.netPayPath_V1);
        LoadTargetDll(myST_FRAME_CONFIG_INFO.netPayPath);
        InitNetPay();
        m_CurrSystemVersion = "1.5";
    }

	
}

void CommonFrameMain::LoadTargetDll(const std::string& path)
{
    std::string fileName = path;
	HMODULE dllHandle = LoadLibrary(fileName.c_str());
    if (dllHandle == NULL)
    {
        return ;
    }
    LibMap::iterator libIt = libMap.find(fileName);

    if (libIt != libMap.end())
    {
        FreeLibrary(hDLL);
        return;
    }
    libMap.insert(PairLibMap(fileName, dllHandle));

}

