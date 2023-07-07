/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: ErrorHelper.cpp
* ժ   Ҫ:���ڶ���ʹ�����Ϣ��ص����ݣ�����������������Ϣ
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-10
* �޸ļ�¼:
*/
#include "stdafx.h"
#include "ErrorHelper.h"
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static TW_ERROR_TYPE g_SysError = OP_SUCCESS;

std::map<TW_ERROR_TYPE, std::string> ErrorExplainInfo()
{
    std::map<TW_ERROR_TYPE, std::string> result;
    result[OP_SUCCESS]             = "�����ɹ�";
    result[OP_INPUT_PASSWORD]      = "������������";
    result[OP_NO_MONEY]            = "�˻�����";
    result[OP_QRCODE_ILLEGAL]      = "��ά��Ƿ�";
    result[OP_QRCODE_OT]           = "��ά�볬����Ч��";
    result[OP_NO_ACCOUNT_INFO]     = "֧��ƾ֤�쳣";
    result[OP_DEBIT_OT]            = "�ۿʱ";
    result[OP_PSW_ERR]             = "�������";
    result[OP_PARAM_ERR]           = "��������";
    result[OP_LAST_TRADE_NOT_END]  = "�ϴν���δ����";
    result[OP_COM_NOT_INIT]        = "���δ��ʼ��";
    result[OP_COM_ILLEGAL]         = "���У��Ƿ�";
    result[OP_PAY_PLATFORM_ERR]    = "֧��ƽ̨�������";
    result[OP_SET_HEART_TIME_ERR]  = "�����ն��Զ�����ʱ����ʧ��";
    result[OP_SET_INIT_ERR]        = "���س�ʼ����Ϣʧ��";
    result[OP_GET_UPPER_TIME_ERR]  = "��ȡ��λ��ʱ���쳣";
    result[OP_READ_CANID_ERR]      = "��ȡ����CAN ��ID ����";
    result[OP_READ_PRI_ERR]        = "��ȡ�ն��������γ���";
    result[OP_READ_HW_FIRM_ERR]    = "��ȡ�̼��汾��Ϣ����";
    result[OP_READ_SCAN_TIME_ERR]  = "��ȡ�ն�ɨ���޶�ʱ�����";
    result[OP_READ_SERIAL_NO]      = "��ȡ�ն��豸Ψһ���кų���";
    result[OP_READ_WEL_ID_ERR]     = "��ȡ�ն��豸�����������";
    result[OP_READ_ETH0_IP_ERR]    = "��ȡ�ն�����0 ��IP ����";
    result[OP_READ_ETH0_SUB_ERR]   = "��ȡ�ն�����0 �������������";
    result[OP_READ_ETH0_GATE_ERR]  = "��ȡ�ն�����0 �����س���";
    result[OP_READ_ETH1_IP_ERR]    = "��ȡ�ն�����1 ��IP ����";
    result[OP_READ_ETH1_SUB_ERR]   = "��ȡ�ն�����1 �������������";
    result[OP_READ_ETH1_GATE_ERR]  = "��ȡ�ն�����1 �����س���";
    result[OP_READ_DEF_GATE_ERR]   = "��ȡ�ն�Ĭ�����س���";
    result[OP_READ_SOFT_VER_ERR]   = "��ȡӦ������汾��Ϣ����";
    result[OP_READ_HW_TMP_ERR]     = "��ȡ�ն��¶���ֵ����";
    result[OP_READ_HW_TIME_ERR]    = "��ȡ�ն˵�ǰʱ�����";
    result[OP_WRITE_CAN_ID_ERR]    = "�����ն�CAN �ڳ���";
    result[OP_WRITE_SCAN_TIME_ERR] = "�����ն�ɨ���޶�ʱ�����";
    result[OP_WRITE_EVE_PARAM_ERR] = "�����ն����л�����������";
    result[OP_WRITE_ETH0_IP_ERR]   = "�����ն�����0 ��IP ����";
    result[OP_WRITE_ETH0_SUB_ERR]  = "�����ն�����0 �������������";
    result[OP_WRITE_ETH0_GATE_ERR] = "�����ն�����0 �����س���";
    result[OP_WRITE_ETH1_IP_ERR]   = "�����ն�����1 ��IP ����";
    result[OP_WRITE_ETH1_SUB_ERR]  = "�����ն�����1 �������������";
    result[OP_WRITE_ETH1_GATE_ERR] = "�����ն�����1 �����س���";
    result[OP_WRITE_DEF_GATE_ERR]  = "�����ն����ڵ�Ĭ�����س���";
    result[OP_WRITE_SCREEN_ERR]    = "�����ն���ʾ��������Ϣ����";
    result[OP_WRITE_HW_VER_ERR]    = "�����ն˹̼��汾��Ϣ����";
    result[OP_WRITE_MAX_ERR]       = "�����ն�������Ϣ����";
    result[OP_WRITE_SERIAL_NO_ERR] = "�����ն�Ψһ��ų���";
    result[OP_WRITE_WEL_ID_ERR]    = "�����ն˻�ӭ����ID ����";
    result[OP_TEST_HW_ERR]         = "�����ն�Ӳ����ʾ����Ŀ����";
    result[OP_HW_ILLEGAL]          = "�ն˺Ϸ���У��ʧ��";
    result[OP_UPPER_STATE_ERR]     = "��ȡ��λ��״̬��Ϣ����";
    result[OP_UPPER_VER_ERR]       = "��ȡ��λ������汾����";

    result[OP_DEBIT_CANCEL_ERR]              = "����ʧ��";
    result[OP_WRITE_HARDWAREVER_ERR]         = "��Ӳ���汾ʧ��";
    result[OP_WRITE_FEIXIAN_ERR]             = "���ƽ�������ʧ��";

    result[TW_ERROR_OPERATION_NOT_SUPPORTED] = "������֧��";
    result[TW_ERROR_SYSTEM_MEMORY_ALLOC]     = "�ڴ����ʧ��";
    result[TW_ERROR_OPERATOR_CANCLE]         = "������ȡ��";
    result[TW_ERROR_COMMUNICATION]         = "����λ��ͨѶ�쳣";
    result[TW_ERROR_WAIT_RESULT]         = "�ȴ��������";

	result[OP_CONFIG_DISABLE_ERR]=      "������������������";


    return result;
}

void SetErrorNo(const TW_ERROR_TYPE err)
{
    g_SysError = err;
}
TW_ERROR_TYPE GetErrorNo()
{
    return g_SysError;
}
void ResetErrorNO()
{
    g_SysError = OP_SUCCESS;
}

std::string GetErrorDesc()
{
    const static std::map<TW_ERROR_TYPE, std::string> msgMap = ErrorExplainInfo();
    std::map<TW_ERROR_TYPE, std::string>::const_iterator it = msgMap.find(GetErrorNo());
    if (it == msgMap.end())
    {
        return "δ�������";
    }
    return it->second;
}