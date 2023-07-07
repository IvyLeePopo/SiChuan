// CommonBus.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"
//#include "CommonBus.h"
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
//COMMONBUS_API int nCommonBus=0;
//
//// ���ǵ���������һ��ʾ����
//COMMONBUS_API int fnCommonBus(void)
//{
//	return 42;
//}
//
//// �����ѵ�����Ĺ��캯����
//// �й��ඨ�����Ϣ������� CommonBus.h
//CCommonBus::CCommonBus()
//{
//	return;
//}
//
//
//




#include <afx.h>
#include "CommonBus.h"
#include "CommonBusSource.h"
#include <stdio.h>
#include <windows.h>


CommonBus::CommonBus()
{

}

CommonBus::~CommonBus()
{

}


void CommonBus::register2Bus(int key)
{
	
	CommonMap::iterator myMapIt = CommonBusSource::myMap.find(key);
	if(myMapIt!=CommonBusSource::myMap.end())
	{
		printf("[##CommonBus->register2Bus##]:this key=%d is exsited,can't add!\n",key);
		return;
	}
	else
	{
		printf("[##CommonBus->register2Bus##]:This key=%d register successful!\n",key);
		CommonBusSource::myMap.insert(PairCommonMap(key,this));
	}
}

void CommonBus::unRegister2Bus(int key)
{
	CommonMap::iterator myMapIt = CommonBusSource::myMap.find(key);
	if(myMapIt!=CommonBusSource::myMap.end())
	{
		CommonBusSource::myMap.erase(myMapIt);
		printf("[##CommonBus->register2Bus##]: Have removed key %d\n",key);
	}
}

int CommonBus::send2Bus(int destKey, char *msg, int len,int srcKey)
{
	CommonMap::iterator myMapIt = CommonBusSource::myMap.find(destKey);

	
	if(myMapIt!=CommonBusSource::myMap.end())
	{
		CommonBus *baseServer = myMapIt->second;
		return baseServer->recvFromBus(destKey,msg,len,srcKey);
	}
	else
	{
		printf("[##CommonBus->register2Bus##]: This key %d register error!\n",destKey);
	}
	return 0;
}

int CommonBus::recvFromBus(int destKey, char *msg, int len,int srcKey)
{
	return 0;	
}

CommonBus_ForBase::CommonBus_ForBase()
{
	core_recv_fun = NULL;
}
CommonBus_ForBase::~CommonBus_ForBase()
{
}
int CommonBus_ForBase::recvFromBus(int destKey,char *msg,int len, int srcKey)
{
	if(core_recv_fun!=NULL)
	{
		return core_recv_fun(destKey,msg,len,srcKey);
	}
	return -1;
}

void CommonBus_ForBase::set_recv_fun(recvFromBus_X_def fun)
{
	core_recv_fun = fun;
}




std::map<int,CommonBus_ForBase*> g_mainCore;



//�˴�Ϊ���������ƽ̨�ļ�Ӽ��أ���Ϊ�˲��ּ��ݣ��������߳�֧�֣�
int GenerateMainBus(recvFromBus_X_def fun)
{
	int thr_id = ::GetCurrentThreadId();
	if( g_mainCore.empty())
	{
		g_mainCore[thr_id] = new CommonBus_ForBase();
		g_mainCore[thr_id]->set_recv_fun(fun);
	}
	return thr_id;
	
}

//add 2018-03 ȫ�������ö�̬���ص�ģʽ
std::map<int,CommonBus_ForBase*> g_busConnectors;

//�������˳����صģ�������ڶ��̷߳��ʣ��˴��������߳�֧��
//ͨ�������������������
int NewBusConnector(recvFromBus_X_def fun)
{
	srand(NULL);
	int rand_num=0;
	for(int i =0;i<100;++i)
	{
		int tmp_num = rand()%10000+1;
		if( g_busConnectors.find(tmp_num)==g_busConnectors.end())
		{
			rand_num=tmp_num;
			break;
		}
	}
	if(rand_num==0)
	{
		return 0;
	}
	g_busConnectors[rand_num]=new CommonBus_ForBase();
	g_busConnectors[rand_num]->set_recv_fun(fun);
	return rand_num;
}
void RemoveBusConnector(int conn_handle)
{
	std::map<int,CommonBus_ForBase*>::iterator it = g_busConnectors.find(conn_handle);
	if(it==g_busConnectors.end())
	{
		return ;
	}
	delete it->second;
	g_busConnectors.erase(it);
}


int send2Bus_Target(int main_handle,int destKey, char *msg, int len,int srcKey)
{
	std::map<int,CommonBus_ForBase*>::iterator it = g_mainCore.find(main_handle);
	if(it==g_mainCore.end())
	{
		it = g_busConnectors.find(main_handle);
		if(it==g_busConnectors.end())
		{
			return -1;
		}
		return it->second->send2Bus(destKey,msg,len,srcKey);
	}
	return it->second->send2Bus(destKey,msg,len,srcKey);
}

void register2Bus_Target(int main_handle,int key)
{
	
	//char chs[100]={0};
	//sprintf(chs,"reg key:%d",key);
	//::MessageBox(0,chs,"",0);
	std::map<int,CommonBus_ForBase*>::iterator it = g_mainCore.find(main_handle);

	if(it==g_mainCore.end())
	{
		it = g_busConnectors.find(main_handle);
		if(it==g_busConnectors.end())
		{
			return ;
		}

		
		it->second->register2Bus(key);
		return ; //no such handle
	}
	it->second->register2Bus(key);
}
void unRegister2Bus_Target(int main_handle,int key)
{
	std::map<int,CommonBus_ForBase*>::iterator it = g_mainCore.find(main_handle);
	if(it==g_mainCore.end())
	{
		it = g_busConnectors.find(main_handle);
		if(it==g_busConnectors.end())
		{
			return ;   
		}
		it->second->unRegister2Bus(key);
	}
	it->second->unRegister2Bus(key);
}