/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: ErrorHelper.cpp
* 摘   要:用于定义和错误信息相关的内容，比如错误码和描述信息
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-10
* 修改记录:
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
    result[OP_SUCCESS]             = "操作成功";
    result[OP_INPUT_PASSWORD]      = "正在输入密码";
    result[OP_NO_MONEY]            = "账户余额不足";
    result[OP_QRCODE_ILLEGAL]      = "二维码非法";
    result[OP_QRCODE_OT]           = "二维码超出有效期";
    result[OP_NO_ACCOUNT_INFO]     = "支付凭证异常";
    result[OP_DEBIT_OT]            = "扣款超时";
    result[OP_PSW_ERR]             = "密码错误";
    result[OP_PARAM_ERR]           = "参数错误";
    result[OP_LAST_TRADE_NOT_END]  = "上次交易未结束";
    result[OP_COM_NOT_INIT]        = "组件未初始化";
    result[OP_COM_ILLEGAL]         = "组件校验非法";
    result[OP_PAY_PLATFORM_ERR]    = "支付平台网络故障";
    result[OP_SET_HEART_TIME_ERR]  = "设置终端自动心跳时间间隔失败";
    result[OP_SET_INIT_ERR]        = "本地初始化信息失败";
    result[OP_GET_UPPER_TIME_ERR]  = "获取上位机时间异常";
    result[OP_READ_CANID_ERR]      = "获取本地CAN 口ID 出错";
    result[OP_READ_PRI_ERR]        = "获取终端生产批次出错";
    result[OP_READ_HW_FIRM_ERR]    = "获取固件版本信息出错";
    result[OP_READ_SCAN_TIME_ERR]  = "获取终端扫码限定时间出错";
    result[OP_READ_SERIAL_NO]      = "获取终端设备唯一序列号出错";
    result[OP_READ_WEL_ID_ERR]     = "获取终端设备开机画面出错";
    result[OP_READ_ETH0_IP_ERR]    = "获取终端网口0 的IP 出错";
    result[OP_READ_ETH0_SUB_ERR]   = "获取终端网口0 的子网掩码出错";
    result[OP_READ_ETH0_GATE_ERR]  = "获取终端网口0 的网关出错";
    result[OP_READ_ETH1_IP_ERR]    = "获取终端网口1 的IP 出错";
    result[OP_READ_ETH1_SUB_ERR]   = "获取终端网口1 的子网掩码出错";
    result[OP_READ_ETH1_GATE_ERR]  = "获取终端网口1 的网关出错";
    result[OP_READ_DEF_GATE_ERR]   = "获取终端默认网关出错";
    result[OP_READ_SOFT_VER_ERR]   = "获取应用软件版本信息出错";
    result[OP_READ_HW_TMP_ERR]     = "获取终端温度数值出错";
    result[OP_READ_HW_TIME_ERR]    = "获取终端当前时间出错";
    result[OP_WRITE_CAN_ID_ERR]    = "更新终端CAN 口出错";
    result[OP_WRITE_SCAN_TIME_ERR] = "更新终端扫码限定时间出错";
    result[OP_WRITE_EVE_PARAM_ERR] = "更新终端运行环境参数出错";
    result[OP_WRITE_ETH0_IP_ERR]   = "更新终端网口0 的IP 出错";
    result[OP_WRITE_ETH0_SUB_ERR]  = "更新终端网口0 的子网掩码出错";
    result[OP_WRITE_ETH0_GATE_ERR] = "更新终端网口0 的网关出错";
    result[OP_WRITE_ETH1_IP_ERR]   = "更新终端网口1 的IP 出错";
    result[OP_WRITE_ETH1_SUB_ERR]  = "更新终端网口1 的子网掩码出错";
    result[OP_WRITE_ETH1_GATE_ERR] = "更新终端网口1 的网关出错";
    result[OP_WRITE_DEF_GATE_ERR]  = "更新终端网口的默认网关出错";
    result[OP_WRITE_SCREEN_ERR]    = "更新终端显示屏变量信息出错";
    result[OP_WRITE_HW_VER_ERR]    = "更新终端固件版本信息出错";
    result[OP_WRITE_MAX_ERR]       = "更新终端生产信息出错";
    result[OP_WRITE_SERIAL_NO_ERR] = "更新终端唯一序号出错";
    result[OP_WRITE_WEL_ID_ERR]    = "更新终端欢迎界面ID 出错";
    result[OP_TEST_HW_ERR]         = "测试终端硬件显示屏节目出错";
    result[OP_HW_ILLEGAL]          = "终端合法性校验失败";
    result[OP_UPPER_STATE_ERR]     = "获取上位机状态信息出错";
    result[OP_UPPER_VER_ERR]       = "获取上位机软件版本出错";

    result[OP_DEBIT_CANCEL_ERR]              = "撤单失败";
    result[OP_WRITE_HARDWAREVER_ERR]         = "读硬件版本失败";
    result[OP_WRITE_FEIXIAN_ERR]             = "控制交互费显失败";

    result[TW_ERROR_OPERATION_NOT_SUPPORTED] = "操作不支持";
    result[TW_ERROR_SYSTEM_MEMORY_ALLOC]     = "内存分配失败";
    result[TW_ERROR_OPERATOR_CANCLE]         = "操作被取消";
    result[TW_ERROR_COMMUNICATION]         = "与下位机通讯异常";
    result[TW_ERROR_WAIT_RESULT]         = "等待结果错误";

	result[OP_CONFIG_DISABLE_ERR]=      "操作不允许，请检查配置";


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
        return "未定义错误";
    }
    return it->second;
}