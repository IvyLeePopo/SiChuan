/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: SystemTrace.cpp
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-13
* 修改记录:
*/

#include "stdafx.h"

#include "SystemTrace.h"
#include <windows.h>
#include <fstream>
#include "SuperLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static HWND g_trace_window = NULL;

std::string TmpGetExeDir()
{
	char modePath[300] = {0};
	memset(modePath,0,260);
	GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);
;
	std::string pathStr = modePath;
	int pos = pathStr.find_last_of('\\');
	if (pos == std::string::npos)
	{
		return "";
	}
	pathStr = pathStr.substr(0, pos) + "\\TWSDNetPayLog";
	
	return pathStr;
}

int InitThirdLog()
{
	std::string dirStr = TmpGetExeDir();
	
	CreateDirectory(dirStr.c_str(),NULL);

	INIT_LOG(dirStr.c_str(), "");/*文件名不支持自定义*/
	return 0;
}

void sys_set_trace_window(HWND hwd)
{
	g_trace_window = hwd;
}

std::string GetTraceMsgHeader(int msgType)
{
	std::string result;
	if (msgType == TW_DIARY_MSG_THREAD_API)
	{
		result = "[接口响应线程]";
	}
	else if (msgType == TW_DIARY_MSG_THREAD_COMM)
	{
		result = "[串口通讯线程]";
	}
	else if (msgType == TW_DIARY_MSG_THREAD_MAIN)
	{
		result = "[主业务线程]";
	}
	else if (msgType == TW_DIARY_MSG_FRAME_RECV)
	{
		result = "[数据接收线程]";
	}
	else if (msgType == TW_DIARY_MSG_FRAME_SEND)
	{
		result = "[数据发送线程]";
	}
	else if (msgType == TW_DIARY_MSG_DEBUG)
	{
		result = "[Debug]";
	}
	return result;
}
void sys_trace_info(const std::string& some_info, int msgType)
{
	static int unuse = InitThirdLog();
	std::string final_info = GetTraceMsgHeader(msgType) + some_info;

	if (g_trace_window)
	{
		//::SendMessage(g_trace_window,WM_TRACE_DIARY_INFO,(WPARAM)some_info.c_str(),NULL);
	}
	WRITE_LOG(final_info.c_str());
#ifdef _DEBUG
	OutputDebugString((final_info+"\n").c_str());
	
#endif
	/*
	time_t timep;
	char s[30]={0};

	time(&timep);

	strcpy(s,ctime(&timep));
	s[strlen(s)-1]='\0';
	OutputDebugString((s + std::string(" ")+some_info+"\n").c_str());
	std::ofstream dd("TR300Diary.txt",std::ios::app);
	dd<<(s + std::string(" ")+some_info+"\n");
	*/

}