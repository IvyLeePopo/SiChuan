// proj_tr.h : proj_tr DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
//#include "..\..\src\third\unexcept\CatchDump.h"

// Cproj_trApp
// �йش���ʵ�ֵ���Ϣ������� proj_tr.cpp
//

class Cproj_trApp : public CWinApp
{
public:
	Cproj_trApp();
private:
//	CCatchDump* m_pCatchDump;
private:
	static CString hahah;
// ��д
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};
