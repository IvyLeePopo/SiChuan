/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: SystemTrace.h
* 摘   要:  进行系统运行状态的跟踪
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-13
* 修改记录: 
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
#define TW_DIARY_MSG_HARDWARE_COM_RECV 3   /*零散数据*/
#define TW_DIARY_MSG_HARDWARE_COM_SEND 4
#define TW_DIARY_MSG_FRAME_RECV 5          /*完整数据*/
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
		std::string loginfo = "调用接口:"+loc_name+"["+regId+"]"+para;
		TW_TRACE_DIARY_TYPE(loginfo, TW_DIARY_MSG_THREAD_API);
	}
	~CInterfaceTraceInfo()
	{
		std::string loginfo = "接口调用结束:"+loc_name+"["+regId+"]";
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
 * 日志记录内容
 * 1、外部调用DLL接口
 * 2、串口数据的发送和接收
 * 3、串口数据的业务处理
 * 4、DLL被加载，初始化内容记录
 * 5、DLL被卸载，资源清理内容
 */

#endif // SYSTEM_TRACE_H
