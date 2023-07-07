/*
 * Copyright(C) 2015,SQUIRREL ���˹����� ��������Ȩ����( All rights reserved. )
 * 
 * �ļ����ƣ�ScanMachineIF.cpp
 * ժ    Ҫ��ɨ�������ܶ�������࣬ʵ���ļ�
 * ��ǰ�汾��1.0.0.1
 * ��    �ߣ�ROCY
 * �������ڣ�2015��11��27��
 */
#include "StdAfx.h"
#include "ScanMachineIF.h"

CScanMachineIF::CScanMachineIF()
{
}

CScanMachineIF::CScanMachineIF(IN const int& iSMType)
	:m_iSMType(0)
	,m_dwCodeLen(0)
	,m_iLastCommandType(SM_CMD_UNKNOW)
	,m_strProductName(_T(""))
	,m_strBootRevision(_T(""))
	,m_strSerialNumber(_T(""))
	,m_strFirmwareVersion(_T(""))
{
}

CScanMachineIF::~CScanMachineIF(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ƣ�CScanMachineIF::IsNeedReadVersionInfo
//     
// ����������
//     
// ���������void
// ���������const
// ��д��Ա��ROCY
// ��дʱ�䣺2016��1��3��
// �޸���Ա��
// �޸�ʱ�䣺
// �����汾��1.0.0.1
// ��ע˵����
//     
////////////////////////////////////////////////////////////////////////////////////////////////
const BOOL CScanMachineIF::IsNeedReadVersionInfo(void)
{
	if(m_strProductName.IsEmpty() || m_strBootRevision.IsEmpty() || m_strSerialNumber.IsEmpty() || m_strFirmwareVersion.IsEmpty())
	{
		return TRUE;
	}
	return FALSE;
}