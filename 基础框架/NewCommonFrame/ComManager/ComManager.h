// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� COMMANAGER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// COMMANAGER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
//#ifdef COMMANAGER_EXPORTS
//#define COMMANAGER_API __declspec(dllexport)
//#else
//#define COMMANAGER_API __declspec(dllimport)
//#endif
//
//// �����Ǵ� ComManager.dll ������
//class COMMANAGER_API CComManager {
//public:
//	CComManager(void);
//	// TODO: �ڴ�������ķ�����
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
extern "C" COMMANAGERSHARED_EXPORT CommonBus* getInstance(); //��ȡ��TestDll�Ķ���
extern "C" COMMANAGERSHARED_EXPORT CommonBus* deleteInstance(); //��ȡ��TestDll�Ķ���


#endif // COMMANAGER_H






