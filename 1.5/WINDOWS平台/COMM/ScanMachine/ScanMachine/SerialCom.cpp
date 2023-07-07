// SerialCom.cpp: implementation of the CSerialCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerialCom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialCom::CSerialCom()
	:m_byPort(0)
	,m_hCom(INVALID_HANDLE_VALUE)
	,m_dwMainThreadID(0)
	,m_hMainWnd(NULL)
	,m_dwNotifyNum(1)
	,m_dwMaskEvent(DEFAULT_COM_MASK_EVENT)
	,m_bRunFlag(FALSE)
	,m_byParentMode(0)
	,m_hThreadHandle(0)
	,m_dwIOMode(FILE_FLAG_OVERLAPPED|FILE_FLAG_WRITE_THROUGH)
{
	Init();
}

CSerialCom::~CSerialCom()
{
	Close(); 
	Destroy();
}