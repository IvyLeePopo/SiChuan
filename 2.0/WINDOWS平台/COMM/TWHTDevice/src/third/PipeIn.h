/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: PipeIn.h
* ժ   Ҫ:
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-06-10
* �޸ļ�¼:
*/
#ifndef PIPE_IN_H
#define PIPE_IN_H

#include "SerialPort.h"
#include "ThreadBase.h"
#include <list>
#include "TWProtocolData.h"



class CPipeIn : public CWinThread
{
	DECLARE_DYNCREATE(CPipeIn)
public:
	CPipeIn();
	virtual ~CPipeIn();
public:
	bool SetPipe(CSerialPort* outer);
	bool ReadOneCmd(TWProtocol_BaseFrame& dst);
public:
	virtual afx_msg BOOL InitInstance();
    virtual afx_msg int ExitInstance();
	virtual afx_msg BOOL OnIdle(LONG lCount);
private:
	  void Stop();
	  void ComDataProc();
private:
	CSerialPort* coreRead;/*���ڶ���Ϣ*/
	bool m_bExit;
	CRITICAL_SECTION mComReadMutex;
	CRITICAL_SECTION mCmdsMutex;
private:
	std::list<TWProtocol_BaseFrame> mUnreadCmds;
	std::list<std::list<unsigned char> > mCompleteComData;
	std::list<unsigned char> mLastData;
private:
	CPipeIn(const CPipeIn&);
	CPipeIn& operator = (const CPipeIn&);
protected:
	virtual afx_msg void OnDataReceived(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


#endif // PIPE_IN_H
