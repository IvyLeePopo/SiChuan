/*
 * Copyright(C) 2016,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�DVXMLBase.cpp
 * ժ    Ҫ������XML���ݸ�ʽ�Ĺ��û��࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2016��3��12��
 */
#include "StdAfx.h"
#include "DVXMLBase.h"

CDVXMLBase::CDVXMLBase(void)
	:XMLHead(_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>"))
	,XMLFunctionDesc(_T(""))
	,XMLVersion(_T("1.0"))
	,XMLVerifyCode(_T(""))
	,m_strLastErrorDesc(_T(""))
{
}

CDVXMLBase::~CDVXMLBase(void)
{
}
