/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVDebitCancel.cpp
 * ժ    Ҫ�������������ݽṹ��������ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��8��25��
 */
#include "StdAfx.h"
#include "DVDebitCancel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDVDebitCancel::CDVDebitCancel(void)
	:ProvinceID(0)
	,RoadID(0)
	,StationID(0)
	,ProcessMark(0)
	,PayIdentifier(_T(""))
	,SubTime(_T(""))
	,VerifyCode(_T(""))
{
}

CDVDebitCancel::~CDVDebitCancel(void)
{
}
