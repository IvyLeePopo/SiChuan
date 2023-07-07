/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: PipeOut.h
* ժ   Ҫ:
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-06-10
* �޸ļ�¼:
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

