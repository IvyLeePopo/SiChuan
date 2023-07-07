/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: PipeIn.h
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-06-10
* 修改记录:
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
	CSerialPort* coreRead;/*用于读信息*/
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
