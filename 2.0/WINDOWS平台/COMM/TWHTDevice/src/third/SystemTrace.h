/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: SystemTrace.h
* ժ   Ҫ:  ����ϵͳ����״̬�ĸ���
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-13
* �޸ļ�¼: 
*/
#ifndef SYSTEM_TRACE_H
#define SYSTEM_TRACE_H
#include <string>
#include "TypeHelper.h"

#define WM_TRACE_DIARY_INFO WM_APP + 3456

#define TW_TRACE_SET_WINDOW(para) (sys_set_trace_window((para)))
#define TW_TRACE_DIARY(loginfo) (sys_trace_info((loginfo)))
#define TW_TRACE_DIARY_TYPE(loginfo,msgType) (sys_trace_info((loginfo),(msgType)))

#define TW_DIARY_MSG_UNSPECIFIED 1
#define TW_DIARY_MSG_API_CALL 2
#define TW_DIARY_MSG_HARDWARE_COM_RECV 3   /*��ɢ����*/
#define TW_DIARY_MSG_HARDWARE_COM_SEND 4
#define TW_DIARY_MSG_FRAME_RECV 5          /*��������*/
#define TW_DIARY_MSG_FRAME_SEND 6
#define TW_DIARY_MSG_FRAME_DISTRIBUTE 7
#define TW_DIARY_MSG_DATA_CONV  8
#define TW_DIARY_MSG_BUSINESS 9
#define TW_DIARY_MSG_THREAD_API 10
#define TW_DIARY_MSG_THREAD_COMM 11
#define TW_DIARY_MSG_THREAD_MAIN 12
#define TW_DIARY_MSG_DEBUG  13

void sys_set_trace_window(HWND hwd);
void sys_trace_info(const std::string& some_info,int msgType = TW_DIARY_MSG_UNSPECIFIED );

class CInterfaceTraceInfo
{
public:
	CInterfaceTraceInfo(const std::string& name,const std::string & para="")
	{
		loc_name = name;
		regId = Int2Str((int)time(NULL));
		std::string loginfo = "���ýӿ�:"+loc_name+"["+regId+"]"+para;
		TW_TRACE_DIARY_TYPE(loginfo, TW_DIARY_MSG_THREAD_API);
	}
	~CInterfaceTraceInfo()
	{
		std::string loginfo = "�ӿڵ��ý���:"+loc_name+"["+regId+"]";
		TW_TRACE_DIARY_TYPE(loginfo, TW_DIARY_MSG_THREAD_API);
	}
public:
	std::string loc_name;
	std::string regId;
private:
	CInterfaceTraceInfo(const CInterfaceTraceInfo&);
	CInterfaceTraceInfo& operator = (const CInterfaceTraceInfo&);
};
/**
 * ��־��¼����
 * 1���ⲿ����DLL�ӿ�
 * 2���������ݵķ��ͺͽ���
 * 3���������ݵ�ҵ����
 * 4��DLL�����أ���ʼ�����ݼ�¼
 * 5��DLL��ж�أ���Դ��������
 */

#endif // SYSTEM_TRACE_H
