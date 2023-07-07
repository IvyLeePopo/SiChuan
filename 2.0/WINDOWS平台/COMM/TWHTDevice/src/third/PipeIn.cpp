/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: PipeIn.cpp
* ժ   Ҫ: ��ģ�����ڽ���ͨѶ���ݣ�
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-06-10
* �޸ļ�¼:
*/
#include "stdafx.h"
#include "PipeIn.h"
#include "ThreadHelper.h"
#include <algorithm>
#include "TypeHelper.h"
#include "RawFrameConv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNCREATE(CPipeIn, CWinThread)
CPipeIn::CPipeIn()
{
	coreRead = NULL;
	this->m_bExit = false;	
	::InitializeCriticalSection(&mComReadMutex);
	::InitializeCriticalSection(&mCmdsMutex);
}
CPipeIn::~CPipeIn()
{
	Stop();
	::DeleteCriticalSection(&mComReadMutex);
	::DeleteCriticalSection(&mCmdsMutex);
}

BEGIN_MESSAGE_MAP(CPipeIn, CWinThread)
	ON_THREAD_MESSAGE(ON_COM_RECEIVE, CPipeIn::OnDataReceived)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// ��������:CPipeIn::SetPipe
//
// ��������:��������Դ
//
// �������:CSerialPort* outer,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CPipeIn::SetPipe(CSerialPort* outer)
{
	coreRead = outer;
	if (coreRead)
	{
		coreRead->SetThreadOwner(this->m_nThreadID);
	}
	return true;
}
//////////////////////////////////////////////////////////////
// ��������:CPipeIn::ReadOneCmd
//
// ��������:������Դ�У���ȡһ������
//
// �������:TWProtocol_BaseFrame& dst,
// �������:bool 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
bool CPipeIn::ReadOneCmd(TWProtocol_BaseFrame& dst)
{
	CScopeLock cmdL(&mCmdsMutex);
	if(mUnreadCmds.empty())
	{
		return false;
	}
	dst = mUnreadCmds.front();
	mUnreadCmds.pop_front();
	return true;
}


BOOL CPipeIn::InitInstance()
{
	return true;
}
int CPipeIn::ExitInstance()
{
	return CWinThread::ExitInstance();
}
BOOL CPipeIn::OnIdle(LONG lCount)
{
	if (m_bExit)
	{
		::PostQuitMessage(0);
		return false;
	}
	ComDataProc();
	Sleep(10);
	return true;
}

//////////////////////////////////////////////////////////////
// ��������:CPipeIn::ComDataProc
//
// ��������:�Ӵ��������У���ȡһ��ת��������
//
// �������:
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void CPipeIn::ComDataProc()
{
	CScopeLock readL(&mComReadMutex);
	CScopeLock cmdL(&mCmdsMutex);
	TWProtocol_BaseFrame newFrame, replyFrame;
	for(std::list<std::list<unsigned char> >::iterator it = mCompleteComData.begin();it!=mCompleteComData.end();++it)
	{
		if (RawDataToFrame(*it, newFrame, replyFrame))
		{
			mUnreadCmds.push_back(newFrame);
		}
	}
	mCompleteComData.clear();
}
void CPipeIn::Stop()
{
	TW_TRACE_DIARY_TYPE("ֹͣ���ݽ����߳�" , TW_DIARY_MSG_THREAD_MAIN);
	if (this->m_bExit)
	{
		TW_TRACE_DIARY_TYPE("���ݽ����߳��Ѿ�ֹͣ" , TW_DIARY_MSG_THREAD_MAIN);
		return ;
	}
	this->m_bExit = true;
	::PostThreadMessage(this->m_nThreadID, WM_QUIT, 0, 0);
	//::WaitForSingleObject(this->m_hThread,INFINITE);//ֱ��ʹ�õȴ��������޵ȴ�����ʹ������ķ���MsgWaitForMultipleObjects
	DWORD dRet;
	MSG msg;
	TW_TRACE_DIARY_TYPE("�ȴ����ݽ����߳̽���" , TW_DIARY_MSG_THREAD_MAIN);
	while (1)
	{
		dRet = ::MsgWaitForMultipleObjects(1, &this->m_hThread, FALSE, INFINITE, QS_ALLINPUT);

		if (dRet == WAIT_OBJECT_0 + 1)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			break;
		}
	}
	TW_TRACE_DIARY_TYPE("���ݽ����߳�ֹͣ�ɹ�" , TW_DIARY_MSG_THREAD_MAIN);
}
//////////////////////////////////////////////////////////////
// ��������:CPipeIn::OnDataReceived
//
// ��������:���ڽ��մ������ݵ���Ϣ֪ͨ��
//
// �������:WPARAM wParam, LPARAM lParam,
// �������:void 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
void CPipeIn::OnDataReceived(WPARAM wParam, LPARAM lParam)
{
	CScopeLock tmpL(&mComReadMutex);

	std::list<unsigned char> tmpArr;

	char data[2000] = {0};
	int len = coreRead->ComRead(data, 1000);
	while (len > 0)
	{
		std::copy(data, data + len, std::back_insert_iterator<std::list<unsigned char>>(tmpArr));
		len = coreRead->ComRead(data, 1000);
	}
	mLastData.insert(mLastData.end(), tmpArr.begin(), tmpArr.end());
	bool has_new_data = false;
	/*��ȡ������������Ϣ����DF ---------FD*/
	while (true)
	{
		if (mLastData.empty())
		{
			break;
		}
		std::list<unsigned char>::iterator pos = std::find(mLastData.begin(), mLastData.end(), TWPD_STARTFLAG);
		if (pos == mLastData.end())
		{
			break;
		}
		else if (pos != mLastData.begin()) /*��һ��Ԫ�ز���Э��Ŀ�ͷ���ݣ���Ҫɾ��*/
		{
			std::list<unsigned char> unuseData;
			unuseData.insert(unuseData.end(), mLastData.begin(), pos);
			std::string log_info = "ɾ����Ч����" + Bin2Hex(unuseData);
			TW_TRACE_DIARY(log_info);
			//std::list<unsigned char>::iterator tmp_it = pos ;
			//std::advance(tmp_it, 1); /**/
			
			mLastData.erase(mLastData.begin(), pos/*tmp_it*/);


			//TW_TRACE_DIARY(Bin2Hex(mLastData));
			continue;
		}
		std::list<unsigned char>::iterator end_pos = std::find(mLastData.begin(), mLastData.end(), TWPD_ENDFLAG);
		if (end_pos == mLastData.end())
		{
			break;
		}
		std::list<unsigned char> oneData;
		std::list<unsigned char>::iterator realEndPos = end_pos;
		std::advance(realEndPos, 1);
		oneData.insert(oneData.end(), mLastData.begin(), realEndPos);
		mCompleteComData.push_back(oneData);
		mLastData.erase(mLastData.begin(), realEndPos);
		std::string info = "�յ��豸����(" + Int2Str((int)oneData.size()) + "�ֽ�)" + Bin2Hex(oneData);
        TW_TRACE_DIARY_TYPE(info, TW_DIARY_MSG_FRAME_RECV);
	}
}