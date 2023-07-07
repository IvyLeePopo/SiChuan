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



using namespace std;

CommonFrameMain *CommonFrameMain::self=NULL;

typedef CommonBus* (*getUnitInstance)();//定义函数指针,获取类CommonBus对象;
typedef CommonBus* (*deleteUnitInstance)();//定义函数指针,获取类CommonBus对象;


CommonBus *getInstance()
{
    return CommonFrameMain::instance();
}

CommonBus *deleteInstance()
{
    delete CommonFrameMain::self;
    return NULL;
}


CommonFrameMain *CommonFrameMain::instance()
{
    if(self==NULL)
        self =  new CommonFrameMain();
    return self;
}

CommonFrameMain::~CommonFrameMain()
{
    register2Bus(KEY_COMMON_FRAME);
    self = NULL;
}


CommonFrameMain::CommonFrameMain()
{
    isStateFlag = false;
    commpentsKey = KEY_COMMON_FRAME;
    register2Bus(KEY_COMMON_FRAME);
    linuxLib = NULL;
#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
    hDLL = NULL;
#endif
    memset(&myST_FRAME_CONFIG_INFO,0,sizeof(ST_FRAME_CONFIG_INFO));
    if(false==myFrameConfigFileOperate.parseFilePathConfig(&myST_FRAME_CONFIG_INFO))
    {
        printf("[#CommonFrameMain#] read %s failed.",FILE_PATH_CONFIG_PATH);
        exit(0);
    }
    loadLib(myST_FRAME_CONFIG_INFO.loadLibPath);
    createThreadEx();
}

#if defined(WIN32) || defined(WIN64) || defined(_WINDOWS)

int CommonFrameMain::recvFromBus(int destKey,char *msg,int len, int srcKey)
{
    isStateFlag = true;
}

void CommonFrameMain::createThreadEx()
{
    _beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))threadFun,(LPVOID)this, 0,NULL);

}
int CommonFrameMain::threadFun(void* arg)
{
    CommonFrameMain *pCommonFrameMain = (CommonFrameMain *)arg;

    while(true)
    {
        if(pCommonFrameMain->isStateFlag)
        {
            pCommonFrameMain->tryUpdateDLL();
        }
        pCommonFrameMain->isStateFlag = false;
        Sleep(1000*2);
    }

}

void CommonFrameMain::tryUpdateDLL()
{
    string szPath= myST_FRAME_CONFIG_INFO.downloadPath;
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    int i=0;
    int bufSize=MultiByteToWideChar(CP_ACP,0,szPath.c_str(),-1,NULL,0);
    wchar_t* wp=new wchar_t[bufSize];
    memset(wp,0,bufSize);
    MultiByteToWideChar(CP_ACP,0,szPath.c_str(),-1,wp,bufSize);
    //wcout.imbue(locale("chs"));
    hFind = FindFirstFile(wp,&wfd);
    string oldFileName;
    oldFileName.clear();
    do
    {
        int fileSize = WideCharToMultiByte(CP_ACP,NULL,wfd.cFileName,-1,NULL,0,NULL,FALSE);
        char *sp=new char[fileSize];
        memset(sp,0,fileSize);
        WideCharToMultiByte(CP_ACP,NULL,wfd.cFileName,-1,sp,fileSize,NULL,FALSE);
        if(strcmp( sp,".")==0)  //文件 "." 固定的格式，但是不需要
            continue;
        if(strcmp(sp,"..")==0)//文件 ".." 固定的格式，但是不需要
            continue;
        if(strstr(sp,".zip")!=NULL)  //文件 "." 固定的格式，但是不需要
            continue;
        if(strstr(sp,".tar")!=NULL)  //文件 "." 固定的格式，但是不需要
            continue;
        if(!oldFileName.empty() &&strstr(sp,oldFileName.c_str())!=NULL)//文件 ".." 固定的格式，但是不需要
            continue;
        string dllName;
        char *pTemp = strstr(sp,".dll.");
        if(pTemp!=NULL)//文件 ".." 固定的格式，但是不需要
        {
            char name[256]={0};
            memcpy(name,sp,strlen(sp)-strlen(pTemp));
            dllName.append(name);
            dllName.append(".dll");
        }
        else
        {
            dllName = sp;
        }
        list<string>::iterator it; //bu neng  fu  NULL
        bool findFlag = false;
        for(it = allDllList.begin();it!=allDllList.end();it++)
        {
            if(dllName.compare(*it)==0)
            {
                findFlag = true;
                oldFileName.clear();
                oldFileName= dllName;
                break;
            }
        }
        if(findFlag)//组件库，放入loadLib,//ptr->d_name  .so.1
        {
            //首先得判断是否可升级，如果能够升级，先卸载动态库，再移动文件，
            string findDllName = myST_FRAME_CONFIG_INFO.loadLibPath;;
            findDllName.append(dllName);
            NameMap::iterator nameMapIt = nameMap.find(findDllName);
            if(nameMapIt!=nameMap.end())
            {
                int key = nameMapIt->second;
                int state = send2Bus(key,NULL,0,KEY_COMMON_FRAME);//获取组件的是否可升级状态
                if(state==1)
                {
                    unLoadLib(dllName);
                    moveFile2loadLib(dllName);
                    printf("dllName=%s.\n",dllName.c_str());
                    updateDll(dllName); //这里进行升级操作
                }
                else
                {
                   printf("[#CommonFrameMain::tryUpdateDLL#] 组件%s正在做业务，请等待！",sp);
                }
            }
            else
            {
                printf("[##CommonFrameMain::tryUpdateDLL##]:can not update dllName=%s,there have not this service\n",dllName.c_str());
            }
        }
        else
        {
            moveFile2Lib(sp);
        }
        delete[] sp;
    }
    while (FindNextFile(hFind, &wfd));
    FindClose(hFind);
    delete[] wp;
}
void CommonFrameMain::moveFile2loadLib(string fileName)
{
    string cmd = "move /Y ";
    cmd.append(myST_FRAME_CONFIG_INFO.downloadPath);
    cmd.append(fileName);
    cmd.append("*");
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(myST_FRAME_CONFIG_INFO.loadLibPath);
    system(cmd.c_str());
    printf(cmd.c_str());
}

void CommonFrameMain::moveFile2Lib(string fileName)
{
    printf("[#CommonFrameMain::moveFile#]移动 %s 到 %s",fileName.c_str(),myST_FRAME_CONFIG_INFO.libPath);
    string cmd = "move /Y ";
    cmd.append(myST_FRAME_CONFIG_INFO.downloadPath);
    cmd.append(fileName);
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(myST_FRAME_CONFIG_INFO.libPath); //组件依赖库，放入lib
    system(cmd.c_str());
}


void CommonFrameMain::moveFile2Etc(string fileName)
{
    printf("[#CommonFrameMain::moveFile#]移动 %s 到 %s",fileName.c_str(),myST_FRAME_CONFIG_INFO.etcPath);
    string cmd = "move /Y ";
    cmd.append(myST_FRAME_CONFIG_INFO.downloadPath);
    cmd.append(fileName);
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(myST_FRAME_CONFIG_INFO.etcPath); //组件依赖库，放入lib
    system(cmd.c_str());
}

void CommonFrameMain::updateDll(string fileDllName)
{
    string dirPath = myST_FRAME_CONFIG_INFO.loadLibPath;//LOAD_LIB_PATH;
    string fileName = dirPath+fileDllName;

    int bufSize=MultiByteToWideChar(CP_ACP,0,fileName.c_str(),-1,NULL,0);
    wchar_t* wp=new wchar_t[bufSize];
    memset(wp,0,bufSize);
    MultiByteToWideChar(CP_ACP,0,fileName.c_str(),-1,wp,bufSize);
    hDLL = LoadLibrary(wp);
    if(hDLL <(HMODULE)HINSTANCE_ERROR)
    {
        delete[] wp;
        wp  = NULL;
        return;
    }
    libMap.insert(PairLibMap(fileName,hDLL));
    getUnitInstance pGetInstance = (getUnitInstance)GetProcAddress(hDLL,"getInstance");
    if(pGetInstance)
        pGetInstance();
    else
        printf("The commpents must surrport getInstance funtion api can load\n");

    delete[] wp;
    wp  = NULL;
}

void CommonFrameMain::loadLib(string filePath)
{
    this->filePath = myST_FRAME_CONFIG_INFO.loadLibPath;LOAD_LIB_PATH;
    readFileName();
    list<string>::iterator it;
    for(it = allDllList.begin();it!=allDllList.end();it++)
    {
        string fileName = filePath;
        fileName.append(*it);
        int bufSize=MultiByteToWideChar(CP_ACP,0,fileName.c_str(),-1,NULL,0);
        wchar_t* wp=new wchar_t[bufSize];
        memset(wp,0,bufSize);
        MultiByteToWideChar(CP_ACP,0,fileName.c_str(),-1,wp,bufSize);
        hDLL = LoadLibrary(wp);
        if(hDLL <(HMODULE)HINSTANCE_ERROR)
        {
            printf("can't load fileName=.\n",fileName);
            delete [] wp;
            wp = NULL;
            continue;
        }
        LibMap::iterator libIt=libMap.find(fileName);
        if(libIt!=libMap.end())
        {
            printf("commpents have loaded,%s can't load again！\n",fileName);
            FreeLibrary(hDLL);
            delete [] wp;
            wp = NULL;
            continue;
        }
        else
        {
            libMap.insert(PairLibMap(fileName,hDLL));
            getUnitInstance pGetInstance = (getUnitInstance)GetProcAddress(hDLL,"getInstance");
            if(pGetInstance)
            {
                CommonBus *pCommonBus = pGetInstance();
                int key = pCommonBus->commpentsKey;
                nameMap.insert(PairNameMap(fileName,key));
            }
            else
                printf("The commpents must surrport getInstance funtion api can load\n");
        }
        delete [] wp;
        wp = NULL;
        hDLL = NULL;
    }
    //    send2Bus(1,"qwe",3,1);
}

void CommonFrameMain::unLoadLib(string fileName)
{
    string dirPath =myST_FRAME_CONFIG_INFO.loadLibPath;//LOAD_LIB_PATH;
    string filePath = dirPath+fileName;
    LibMap::iterator libIt=libMap.find(fileName);
    if(libIt!=libMap.end())
    {
        printf("start unload commpents %s.\n",fileName.c_str());
        HINSTANCE hDLL = libIt->second;
        deleteUnitInstance pDeleteInstance = (deleteUnitInstance)GetProcAddress(hDLL,"deleteInstance");
        if(pDeleteInstance)
        {
            pDeleteInstance();
            FreeLibrary(hDLL);
            libMap.erase(libIt);
        }
        else
            printf("The commpents must surrport deleteInstance funtion api can unload\n");
    }
    else
        printf("the commpents has not load or had unloaded.\n");
}

void CommonFrameMain::readFileName()
{
    string szPath=filePath;
    szPath.append("*.dll");
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    int i=0;
    int bufSize=MultiByteToWideChar(CP_ACP,0,szPath.c_str(),-1,NULL,0);
    wchar_t* wp=new wchar_t[bufSize];
    memset(wp,0,bufSize);
    MultiByteToWideChar(CP_ACP,0,szPath.c_str(),-1,wp,bufSize);
    //wcout.imbue(locale("chs"));
    hFind = FindFirstFile(wp,&wfd);
    do
    {
        int fileSize = WideCharToMultiByte(CP_ACP,NULL,wfd.cFileName,-1,NULL,0,NULL,FALSE);
        char *sp=new char[fileSize];
        memset(sp,0,fileSize);
        WideCharToMultiByte(CP_ACP,NULL,wfd.cFileName,-1,sp,fileSize,NULL,FALSE);
        //    strcpy(fileList[i],sp); //存储到数组
        allDllList.push_back(sp);
        delete[] sp;
        //        if (++i>=LIB_NUM)
        //            break;
    }
    while (FindNextFile(hFind, &wfd));
    FindClose(hFind);
    delete[] wp;
}
#else
void CommonFrameMain::readFileName()
{
    string szPath=filePath;
    printf("[#CommonFrameMain::readFileName#]%s\n",szPath.c_str());
    DIR * dir = NULL;
    struct dirent * ptr;
    int i=0;
    dir = opendir(szPath.c_str()); //打开一个目录
    if(dir==NULL)
    {
        printf("[#CommonFrameMain::readFileName#]dir is NULL.\n");
        return;
    }
    while((ptr = readdir(dir)) != NULL) //循环读取目录数据
    {
        if(strcmp( ptr->d_name,".")==0)  //文件 "." 固定的格式，但是不需要
            continue;
        if(strcmp( ptr->d_name,"..")==0)//文件 ".." 固定的格式，但是不需要
            continue;
        if(strstr(ptr->d_name,".so")==NULL)
            continue;
        if(strstr(ptr->d_name,".so.")!=NULL)
            continue;
        printf("[#CommonFrameMain::readFileName#]d_name : %s\n", ptr->d_name); //输出文件名
        allDllList.push_back(ptr->d_name);
    }
    closedir(dir);//关闭目录指针
}

void CommonFrameMain::loadLib(string filePath)
{
    this->filePath = filePath;
    readFileName();
    list<string>::iterator it; //bu neng  fu  NULL
    for(it = allDllList.begin();it!=allDllList.end();it++)
    {
        string fileName = filePath;
        fileName.append(*it);
        linuxLib = dlopen(fileName.c_str(),RTLD_LAZY  );
        printf("[#CommonFrameMain::loadLib#] fileName=%s.\n",fileName.c_str());
        if(linuxLib==NULL)
        {
            printf("[#CommonFrameMain::loadLib#]can't load commpents fileName=%s.\n",fileName.c_str());
            continue;
        }
        LibMap::iterator libIt=libMap.find(fileName.c_str());
        if(libIt!=libMap.end())
        {
            printf("[#CommonFrameMain::loadLib#]commpents had loaded：%s,can't load again\n",fileName.c_str());
            dlclose(linuxLib);
            continue;
        }
        else
        {
            libMap.insert(PairLibMap(fileName,linuxLib));
            getUnitInstance pGetInstance = (getUnitInstance)dlsym(linuxLib, "getInstance");
            if(pGetInstance)
            {
                CommonBus *pCommonBus = pGetInstance();
                int key = pCommonBus->commpentsKey;
                printf("key=%d.\n",key);
                printf("fileName=%s.\n",fileName.c_str());
                nameMap.insert(PairNameMap(fileName,key));
            }
            else
                printf("[#CommonFrameMain::loadLib#]commpents must surrport getInstance function api,can load.\n");
        }
        linuxLib = NULL;
    }
}
void CommonFrameMain::unLoadLib(string fileName)
{
    string dirPath =myST_FRAME_CONFIG_INFO.loadLibPath;
    string filePath = dirPath+fileName;
    LibMap::iterator libIt=libMap.find(filePath.c_str());
    if(libIt!=libMap.end())
    {
        printf("[#CommonFrameMain::unLoadLib#]start unload commpents %s.\n",filePath.c_str());
        void *unLoadLibDp = libIt->second;
        deleteUnitInstance pDeleteInstance = (deleteUnitInstance)dlsym(unLoadLibDp, "deleteInstance");
        if(pDeleteInstance)
        {
            pDeleteInstance();
            int flag = dlclose(unLoadLibDp);
            printf("[#CommonFrameMain::unLoadLib#]falg=%d.\n",flag);
            printf("[#CommonFrameMain::unLoadLib#]falg=%d.\n",flag);
            printf("[#CommonFrameMain::unLoadLib#]falg=%d.\n",flag);
            printf("[#CommonFrameMain::unLoadLib#]falg=%d.\n",flag);
             flag = dlclose(unLoadLibDp);
             printf("[#CommonFrameMain::unLoadLib#]falg=%d.\n",flag);
             flag = dlclose(unLoadLibDp);
             printf("[#CommonFrameMain::unLoadLib#]falg=%d.\n",flag);
             printf("dlclose - %sn", dlerror());
            libMap.erase(libIt);
        }
        else
            printf("[#CommonFrameMain::unLoadLib#]commpents must surrport deleteInstance function api,can loading %s.\n",filePath.c_str());
    }
    else
        printf("[#CommonFrameMain::unLoadLib#]commpents has not load or had unloaded %s.\n",filePath.c_str());
}

void CommonFrameMain::createThreadEx()
{
    pthread_t m_thread_t;
    pthread_create(&m_thread_t, 0, threadFun, this);

}

int CommonFrameMain::recvFromBus(int destKey,char *msg,int len, int srcKey)
{
    isStateFlag = true;
}

void *CommonFrameMain::threadFun(void* arg)
{
    CommonFrameMain *pCommonFrameMain = (CommonFrameMain *)arg;

    while(true)
    {
        if(pCommonFrameMain->isStateFlag)
        {
            pCommonFrameMain->tryUpdateDLL();
        }
        pCommonFrameMain->isStateFlag = false;
        sleep(2);
    }

}
void CommonFrameMain::tryUpdateDLL()
{
    string szPath=myST_FRAME_CONFIG_INFO.downloadPath;
    DIR * dir = NULL;
    struct dirent * ptr;
    int i=0;
    dir = opendir(szPath.c_str()); //打开一个目录
    if(dir==NULL)
    {
        printf("dir is NULL.\n");
        return;
    }
    string oldFileName;
    oldFileName.clear();
    while((ptr = readdir(dir)) != NULL) //循环读取目录数据
    {
        if(strcmp( ptr->d_name,".")==0)  //文件 "." 固定的格式，但是不需要
            continue;
        if(strcmp( ptr->d_name,"..")==0)//文件 ".." 固定的格式，但是不需要
            continue;
        if(strstr( ptr->d_name,".zip")!=NULL)  //文件 "." 固定的格式，但是不需要
            continue;
        if(strstr( ptr->d_name,".tar")!=NULL)  //文件 "." 固定的格式，但是不需要
            continue;
        if(!oldFileName.empty() &&strstr( ptr->d_name,oldFileName.c_str())!=NULL)//文件 ".." 固定的格式，但是不需要
            continue;

        string dllName;
        char *pTemp = strstr( ptr->d_name,".so.");
        if(pTemp!=NULL)//文件 ".." 固定的格式，但是不需要
        {
            char name[256]={0};
            memcpy(name,ptr->d_name,strlen(ptr->d_name)-strlen(pTemp));
            dllName.append(name);
            dllName.append(".so");
        }
        else
        {
            dllName = ptr->d_name;
        }
        list<string>::iterator it; //bu neng  fu  NULL
        bool findFlag = false;
        for(it = allDllList.begin();it!=allDllList.end();it++)
        {
            if(dllName.compare(*it)==0)
            {
                findFlag = true;
                oldFileName.clear();
                oldFileName= dllName;
                break;
            }
        }
        if(findFlag)//组件库，放入loadLib,//ptr->d_name  .so.1
        {
            //首先得判断是否可升级，如果能够升级，先卸载动态库，再移动文件，
            string findDllName = myST_FRAME_CONFIG_INFO.loadLibPath;
            findDllName.append(dllName);
            NameMap::iterator nameMapIt = nameMap.find(findDllName);
            if(nameMapIt!=nameMap.end())
            {
                int key = nameMapIt->second;
                int state = send2Bus(key,NULL,0,KEY_COMMON_FRAME);//获取组件的是否可升级状态
                if(state==1)
                {
                    unLoadLib(dllName);
                    moveFile2loadLib(dllName);
                    updateDll(dllName); //这里进行升级操作
                }
                else
                {
                   printf("[#CommonFrameMain::tryUpdateDLL#] 组件%s正在做业务，请等待！.\n",ptr->d_name);
                }
            }
            else
            {
                printf("[##CommonFrameMain::tryUpdateDLL##]:can not update dllName=%s,there have not this service\n",findDllName.c_str());
            }
        }
        else if(strstr(ptr->d_name,"version")!=NULL || strstr(ptr->d_name,".txt")!=NULL)
        {
            moveFile2Etc(ptr->d_name);
        }
        else
        {
            moveFile2Lib(ptr->d_name);
        }
    }
    closedir(dir);//关闭目录指针
}


void CommonFrameMain::updateDll(string fileDllName)
{
    string dirPath = myST_FRAME_CONFIG_INFO.loadLibPath;
    string filePath = dirPath+fileDllName;
    linuxLib = dlopen(filePath.c_str(),RTLD_LAZY );
    printf("[#CommonFrameMain::updateDll#]filePath=%s.\n",filePath.c_str());

    if(linuxLib==NULL)
    {
        printf("[#CommonFrameMain::updateDll#]can't load commpents fileName=%s.\n",filePath.c_str());
        return;
    }
    libMap.insert(PairLibMap(filePath,linuxLib));
    getUnitInstance pGetInstance = (getUnitInstance)dlsym(linuxLib, "getInstance");
    if(pGetInstance)
        pGetInstance();
    else
        printf("[#CommonFrameMain::updateDll#]commpents must surrport getInstance function api, can loading.\n");
}

void CommonFrameMain::moveFile2loadLib(string fileName)
{
    string cmd = "mv -f ";
    cmd.append(myST_FRAME_CONFIG_INFO.downloadPath);
    cmd.append(fileName);
    cmd.append("*");
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(myST_FRAME_CONFIG_INFO.loadLibPath);
    system(cmd.c_str());
    printf("[#CommonFrameMain::moveFile2loadLib#]%s.\n",cmd.c_str());
}

void CommonFrameMain::moveFile2Lib(string fileName)
{
    string libPath = myST_FRAME_CONFIG_INFO.libPath;
    printf("[#CommonFrameMain::moveFile#]移动 %s 到 %s",fileName.c_str(),libPath.c_str());
    string cmd = "mv -f ";
    cmd.append(myST_FRAME_CONFIG_INFO.downloadPath);
    cmd.append(fileName);
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(libPath.c_str()); //组件依赖库，放入lib
    system(cmd.c_str());
}
void CommonFrameMain::moveFile2Etc(string fileName)
{
    string etcPath = myST_FRAME_CONFIG_INFO.etcPath;
    printf("[#CommonFrameMain::moveFile#]移动 %s 到 %s",fileName.c_str(),etcPath.c_str());
    string cmd = "mv -f ";
    cmd.append(myST_FRAME_CONFIG_INFO.downloadPath);
    cmd.append(fileName);
    cmd.append(" ");//命令之间需要 空格  mv -f lib.so(空格)../loadLib/
    cmd.append(etcPath.c_str()); //组件依赖库，放入lib
    system(cmd.c_str());
}



//初始化组件环境		    -- 接口1
bool IF_InitEnvironment(
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
        )
{
     bool bRet = false;
     string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
    LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        void *unLoadLibDp = libIt->second;

        CommonFrame_InitEnvironment pGetInstance = (CommonFrame_InitEnvironment)dlsym(unLoadLibDp, "NetPay_IF_InitEnvironment");
        if(pGetInstance)
        {
                     bRet =  pGetInstance(tty, baudRate, serverIP, serverPort, isOpenServerPort, logFlag, logSaveTime, szAreaInfo, szLoaclStation, szLoaclLaneID, szServerInfo, iProvinceID, fun);
        }

    }

    return bRet;
}


//清理资源			-- 接口2
bool  IF_Destroy(void)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_Destroy pGetInstance = (CommonFrame_Destroy)dlsym(libIt->second, "NetPay_IF_Destroy");
        if(pGetInstance)
            bRet =  pGetInstance();
    }

    return bRet;
}

//获取上一次错误信息		-- 接口3
const char*  IF_GetLastErrorDesc(void)
{
    static string strError;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetLastErrorDesc pGetInstance = (CommonFrame_GetLastErrorDesc)dlsym(libIt->second, "NetPay_IF_GetLastErrorDesc");
        if(pGetInstance)
            strError =  pGetInstance();
    }

    return strError.c_str();

}

//启动获取用户信息操作		-- 接口4
bool  IF_StartGetAccountInfo( const int& iOvertime)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_StartGetAccountInfo pGetInstance = (CommonFrame_StartGetAccountInfo)dlsym(libIt->second, "NetPay_IF_StartGetAccountInfo");
        if(pGetInstance)
            bRet =  pGetInstance(iOvertime);
    }

    return bRet;

}

//停止获取用户信息操作		-- 接口5
bool  IF_StopGetAccountInfo(void)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_StopGetAccountInfo pGetInstance = (CommonFrame_StopGetAccountInfo)dlsym(libIt->second, "NetPay_IF_StopGetAccountInfo");
        if(pGetInstance)
            bRet =  pGetInstance();
    }

    return bRet;

}

//获取用户信息				-- 接口6
bool  IF_GetAccountInfo(char* szParamContext, int& iParamSize,const int& iFormatType)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetAccountInfo pGetInstance = (CommonFrame_GetAccountInfo)dlsym(libIt->second, "NetPay_IF_GetAccountInfo");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

//执行扣款操作				-- 接口7
bool  IF_DebitMoney( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_DebitMoney pGetInstance = (CommonFrame_DebitMoney)dlsym(libIt->second, "NetPay_IF_DebitMoney");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;

}

//执行撤单操作				-- 接口8
bool  IF_DebitCancel( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_DebitCancel pGetInstance = (CommonFrame_DebitCancel)dlsym(libIt->second, "NetPay_IF_DebitCancel");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

//获取扣款结果				-- 接口9
bool  IF_GetDebitResult(char* szParamContext, int& iParamSize, const int& iFormatType)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetDebitResult pGetInstance = (CommonFrame_GetDebitResult)dlsym(libIt->second, "NetPay_IF_GetDebitResult");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;

}

//设置交互界面				-- 接口10
bool  IF_SetMMI(const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_SetMMI pGetInstance = (CommonFrame_SetMMI)dlsym(libIt->second, "NetPay_IF_SetMMI");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

//获取硬件参数				-- 接口11
bool  IF_GetParam( const char* szParamContext, const int& iParamSize, const int& iFormatType, char* szResultContext, int& iResultSize)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetParam pGetInstance = (CommonFrame_GetParam)dlsym(libIt->second, "NetPay_IF_GetParam");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType, szResultContext, iResultSize);
    }

    return bRet;
}

//检测组件状态				-- 接口12
bool  IF_GetComponentStatus( unsigned int& uStatus)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_GetComponentStatus pGetInstance = (CommonFrame_GetComponentStatus)dlsym(libIt->second, "NetPay_IF_GetComponentStatus");
        if(pGetInstance)
            bRet =  pGetInstance(uStatus);
    }

    return bRet;
}

//执行传输关键信息数据操作                  -- 接口13
bool  IF_TranslateData( const char* szParamContext, const int& iParamSize, const int& iFormatType)
{
    bool bRet = false;
    string netpayPath = CommonFrameMain::instance()->myST_FRAME_CONFIG_INFO.netPayPath;
   LibMap::iterator libIt =  CommonFrameMain::instance()->libMap.find(netpayPath);
    if( libIt != CommonFrameMain::instance()->libMap.end())
    {
        CommonFrame_TranslateData pGetInstance = (CommonFrame_TranslateData)dlsym(libIt->second, "NetPay_IF_TranslateData");
        if(pGetInstance)
            bRet =  pGetInstance(szParamContext, iParamSize, iFormatType);
    }

    return bRet;
}

#endif


