// ComManager.cpp : ���� DLL Ӧ�ó������ڵ㡣
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
//// ���ǵ���������һ��ʾ��
//COMMANAGER_API int nComManager=0;
//
//// ���ǵ���������һ��ʾ����
//COMMANAGER_API int fnComManager(void)
//{
//	return 42;
//}
//
//// �����ѵ�����Ĺ��캯����
//// �й��ඨ�����Ϣ������� ComManager.h
//CComManager::CComManager()
//{
//	return;
//}
//





#include "ComManager.h"

#include <stdio.h>

//zhizhָ��һ��Ҫ�������ǰ��
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
