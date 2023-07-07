// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 COMMANAGER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// COMMANAGER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
//#ifdef COMMANAGER_EXPORTS
//#define COMMANAGER_API __declspec(dllexport)
//#else
//#define COMMANAGER_API __declspec(dllimport)
//#endif
//
//// 此类是从 ComManager.dll 导出的
//class COMMANAGER_API CComManager {
//public:
//	CComManager(void);
//	// TODO: 在此添加您的方法。
//};
//
//extern COMMANAGER_API int nComManager;
//
//COMMANAGER_API int fnComManager(void);
//
//




#ifndef COMMANAGER_H
#define COMMANAGER_H


#ifdef WIN32
#if defined(COMMANAGER_LIBRARY)
#  define COMMANAGERSHARED_EXPORT __declspec(dllexport)
#else
#  define COMMANAGERSHARED_EXPORT __declspec(dllimport)
#endif
#else
#  define COMMANAGERSHARED_EXPORT
#endif

#include <CommonBus.h>


class COMMANAGERSHARED_EXPORT ComManager:public CommonBus
{
public:
	static ComManager* instance();
	static ComManager*self;
	virtual ~ComManager();
	void recvFromBus(int destKey,char *msg,int len, int srcKey=0);

public:
	ComManager();



};
extern "C" COMMANAGERSHARED_EXPORT CommonBus* getInstance(); //获取类TestDll的对象
extern "C" COMMANAGERSHARED_EXPORT CommonBus* deleteInstance(); //获取类TestDll的对象


#endif // COMMANAGER_H






