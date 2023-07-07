// ComManager.cpp : 定义 DLL 应用程序的入口点。
//

//#include "stdafx.h"
//#include "ComManager.h"
//
//
//#ifdef _MANAGED
//#pragma managed(push, off)
//#endif
//
//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//    return TRUE;
//}
//
//#ifdef _MANAGED
//#pragma managed(pop)
//#endif
//
//// 这是导出变量的一个示例
//COMMANAGER_API int nComManager=0;
//
//// 这是导出函数的一个示例。
//COMMANAGER_API int fnComManager(void)
//{
//	return 42;
//}
//
//// 这是已导出类的构造函数。
//// 有关类定义的信息，请参阅 ComManager.h
//CComManager::CComManager()
//{
//	return;
//}
//





#include "ComManager.h"

#include <stdio.h>

//zhizh指针一定要放在类的前面
ComManager *ComManager::self=NULL;
ComManager *ComManager::instance()
{
	if(self==NULL)
		self =  new ComManager();
	return self;
}

ComManager::~ComManager()
{
	unRegister2Bus(1);
	printf("unload ComManager successful!.\n");
	if(self!=NULL)
	{
		printf("ComManager is running!.\n");
	}
	self = NULL;
}

void ComManager::recvFromBus(int destKey,char *msg,int len, int srcKey)
{
	printf("qwe.\n");
}

ComManager::ComManager()
{
	register2Bus(1);
}

CommonBus *getInstance()
{
	return ComManager::instance();
}

CommonBus *deleteInstance()
{
	delete ComManager::instance();

	return NULL;
}
