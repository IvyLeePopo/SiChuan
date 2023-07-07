/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: SystemTrace.cpp
* ժ   Ҫ:
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-13
* �޸ļ�¼:
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

	INIT_LOG(dirStr.c_str(), "");/*�ļ�����֧���Զ���*/
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
		result = "[�ӿ���Ӧ�߳�]";
	}
	else if (msgType == TW_DIARY_MSG_THREAD_COMM)
	{
		result = "[����ͨѶ�߳�]";
	}
	else if (msgType == TW_DIARY_MSG_THREAD_MAIN)
	{
		result = "[��ҵ���߳�]";
	}
	else if (msgType == TW_DIARY_MSG_FRAME_RECV)
	{
		result = "[���ݽ����߳�]";
	}
	else if (msgType == TW_DIARY_MSG_FRAME_SEND)
	{
		result = "[���ݷ����߳�]";
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