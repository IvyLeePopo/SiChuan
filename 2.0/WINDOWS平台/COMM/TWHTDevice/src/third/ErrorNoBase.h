/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: ErrorNoBase.h
* 摘   要:  根据智能硬件内部通讯协议(TR300) 附录8(错误代码)，章节生成
* 定义系统指令交互的错误分类
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-06
* 修改记录:                         
*/
#define OP_SUCCESS                   0x00000000         //操作成功
#define OP_INPUT_PASSWORD            0x00000001         //正在输入密码
#define OP_NO_MONEY                  0x00000002         //账户余额不足
#define OP_QRCODE_ILLEGAL            0x00000003         //二维码非法
#define OP_QRCODE_OT                 0x00000004         //二维码超出有效期
#define OP_NO_ACCOUNT_INFO           0x00000005         //支付凭证异常
#define OP_DEBIT_OT                  0x00000006         //扣款超时
#define OP_PSW_ERR                   0x00000007         //密码错误
#define OP_PARAM_ERR                 0x00000008         //参数错误
#define OP_LAST_TRADE_NOT_END        0x00000009         //上次交易未结束
#define OP_COM_NOT_INIT              0x0000000A         //组件未初始化
#define OP_COM_ILLEGAL               0x0000000B         //组件校验非法
#define OP_PAY_PLATFORM_ERR          0x0000000C         //支付平台网络故障
#define OP_SET_HEART_TIME_ERR        0x0000000D         //设置终端自动心跳时间间隔失败
#define OP_SET_INIT_ERR              0x0000000E         //本地初始化信息失败
#define OP_GET_UPPER_TIME_ERR        0x00000010         //获取上位机时间异常
#define OP_READ_CANID_ERR            0x00000011         //获取本地CAN 口ID 出错
#define OP_READ_PRI_ERR              0x00000012         //获取终端生产批次出错
#define OP_READ_HW_FIRM_ERR          0x00000013         //获取固件版本信息出错
#define OP_READ_SCAN_TIME_ERR        0x00000014         //获取终端扫码限定时间出错
#define OP_READ_SERIAL_NO            0x00000015         //获取终端设备唯一序列号出错
#define OP_READ_WEL_ID_ERR           0x00000016         //获取终端设备开机画面出错
#define OP_READ_ETH0_IP_ERR          0x00000017         //获取终端网口0 的IP 出错
#define OP_READ_ETH0_SUB_ERR         0x00000018         //获取终端网口0 的子网掩码出错
#define OP_READ_ETH0_GATE_ERR        0x00000019         //获取终端网口0 的网关出错
#define OP_READ_ETH1_IP_ERR          0x0000001A         //获取终端网口1 的IP 出错
#define OP_READ_ETH1_SUB_ERR         0x0000001B         //获取终端网口1 的子网掩码出错
#define OP_READ_ETH1_GATE_ERR        0x0000001C         //获取终端网口1 的网关出错
#define OP_READ_DEF_GATE_ERR         0x0000001E         //获取终端默认网关出错
#define OP_READ_SOFT_VER_ERR         0x00000020         //获取应用软件版本信息出错
#define OP_READ_HW_TMP_ERR           0x00000021         //获取终端温度数值出错
#define OP_READ_HW_TIME_ERR          0x00000022         //获取终端当前时间出错
#define OP_WRITE_CAN_ID_ERR          0x00000023         //更新终端CAN 口出错
#define OP_WRITE_SCAN_TIME_ERR       0x00000024         //更新终端扫码限定时间出错
#define OP_WRITE_EVE_PARAM_ERR       0x00000025         //更新终端运行环境参数出错
#define OP_WRITE_ETH0_IP_ERR         0x00000026         //更新终端网口0 的IP 出错
#define OP_WRITE_ETH0_SUB_ERR        0x00000027         //更新终端网口0 的子网掩码出错
#define OP_WRITE_ETH0_GATE_ERR       0x00000028         //更新终端网口0 的网关出错
#define OP_WRITE_ETH1_IP_ERR         0x00000029         //更新终端网口1 的IP 出错
#define OP_WRITE_ETH1_SUB_ERR        0x0000002A         //更新终端网口1 的子网掩码出错
#define OP_WRITE_ETH1_GATE_ERR       0x0000002B         //更新终端网口1 的网关出错
#define OP_WRITE_DEF_GATE_ERR        0x0000002C         //更新终端网口的默认网关出错
#define OP_WRITE_SCREEN_ERR          0x0000002D         //更新终端显示屏变量信息出错
#define OP_WRITE_HW_VER_ERR          0x0000002E         //更新终端固件版本信息出错
#define OP_WRITE_MAX_ERR             0x00000030         //更新终端生产信息出错
#define OP_WRITE_SERIAL_NO_ERR       0x00000031         //更新终端唯一序号出错
#define OP_WRITE_WEL_ID_ERR          0x00000032         //更新终端欢迎界面ID 出错
#define OP_TEST_HW_ERR               0x00000033         //测试终端硬件显示屏节目出错
#define OP_HW_ILLEGAL                0x00000034         //终端合法性校验失败
#define OP_UPPER_STATE_ERR           0x00000035         //获取上位机状态信息出错
#define OP_UPPER_VER_ERR             0x00000036         //获取上位机软件版本出错

#define OP_DEBIT_CANCEL_ERR 0x00000037		//撤单失败
#define OP_WRITE_HARDWAREVER_ERR 0x00000038		//读硬件版本失败
#define OP_WRITE_FEIXIAN_ERR 0x00000039		//控制交互费显失败
#define OP_CONFIG_DISABLE_ERR 0x00000040   //操作不被允许
