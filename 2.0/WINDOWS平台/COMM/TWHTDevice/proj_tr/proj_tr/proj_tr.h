// proj_tr.h : proj_tr DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
//#include "..\..\src\third\unexcept\CatchDump.h"

// Cproj_trApp
// 有关此类实现的信息，请参阅 proj_tr.cpp
//

class Cproj_trApp : public CWinApp
{
public:
	Cproj_trApp();
private:
//	CCatchDump* m_pCatchDump;
private:
	static CString hahah;
// 重写
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};
