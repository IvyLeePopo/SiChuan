/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: PipeOut.h
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-06-10
* 修改记录:
*/

#include "SerialPort.h"
#include "ThreadBase.h"
#include <list>
#include "TWProtocolData.h"
#ifndef PIPE_OUT_H
#define PIPE_OUT_H

class CPipeOut: public CWinThread
{
	DECLARE_DYNCREATE(CPipeOut)
public:
	CPipeOut();
	virtual ~CPipeOut();
public:
	bool SetPipe(CSerialPort* outereFra);
	bool WriteOneCmd(const TWProtocol_BaseFrame& dst);
	bool WriteOneSelfCmd(const std::string& info);
public:
	virtual afx_msg BOOL InitInstance();
	virtual afx_msg int ExitInstance();
	virtual afx_msg BOOL OnIdle(LONG lCount);
private:
	void Stop();
private:
	CSerialPort* coreWrite;
	bool m_bExit;
private:
	CPipeOut(const CPipeOut&);
	CPipeOut& operator =(const CPipeOut&);

};


#endif // PIPE_OUT_H

