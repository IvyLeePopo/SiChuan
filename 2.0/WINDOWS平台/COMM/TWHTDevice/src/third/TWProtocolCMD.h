/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: TWProtocolCMD.h
* 摘   要: 根据 智能硬件内部通讯协议(TR300)完成，主要包含各个协议命令的数据位对应
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-07
* 修改记录: 
*/
/**
 * 保留内容：
 * 1、协议5.2指令描述中，提到了指令具有优先级，但是目前没有发现具体的应用，故此处不体现
 */
#ifndef TW_PROTOCOL_CMD_H
#define TW_PROTOCOL_CMD_H
// TWPC for TWProtocolCMD
// 基本构成为：TWPC_指令大类_指令作用_二级指令

/**
 * A指令
 */
// A1
#define TWPC_A_BEAT             0xA1         //握手,心跳
#define TWPC_A_BEAT_HANDSHAKE      0x01         //主动握手（上位机发起）
#define TWPC_A_BEAT_SET         0x02         //调整下位机自动心跳时间

// A2
#define TWPC_A_INTERACTIVE      0xA2         //信息交互
#define TWPC_A_INTERACTIVE_WELCOME      0x01    //设置欢迎信息模式
#define TWPC_A_INTERACTIVE_INTERFACEBLOCK      0x02    //显示交互信息（数据块格式）
#define TWPC_A_INTERACTIVE_BYE          0x03    //显示欢送信息（无参数）
#define TWPC_A_INTERACTIVE_04    0x04    //未定义清楚，暂停使用
#define TWPC_A_INTERACTIVE_SCREEN   0x05    //屏幕节目控制
#define TWPC_A_INTERACTIVE_INTERCACEJSON      0x06       //显示交互信息（JSON 格式）
#define TWPC_A_INTERACTIVE_TTS      0x07       //TTS语音播报
#define TWPC_A_INTERACTIVE_FREE_DISPLAY      0x08       //自由文字显示

// A3
#define TWPC_A_STARTPAY         0xA3         //启动支付
#define TWPC_A_STARTPAY_PAYBLOCK         0x01         //启动支付流程（数据块格式）
#define TWPC_A_STARTPAY_PAYJSON        0x02        //启动支付流程（JSON 格式）


// A4
#define TWPC_A_CANCLE           0xA4         //撤单退款
#define TWPC_A_CANCLE_DATABLOCK 0x01        //启动撤单流程（数据块格式）
#define TWPC_A_CANCLE_JSON      0x02             //启动撤单流程（JSON 格式）

// A5
#define TWPC_A_SENDDATA         0xA5         //数据传输
#define TWPC_A_SENDDATA_INITJSON    0x00        //传输初始化信息（JSON 格式）
#define TWPC_A_SENDDATA_LANETIME    0x01        //传输上位机的当前时间
#define TWPC_A_SENDDATA_LASTERROR      0x02        //要求提供上次错误信息
#define TWPC_A_SENDDATA_INITBLOCK        0x03        //传输初始化信息（数据块模式）
#define TWPC_A_SENDDATA_FILETRANS        0x04        //文件传输指令
#define TWPC_A_SENDDATA_TICKNOTRANS      0x05         //tickno


// A6
#define TWPC_A_HARDWARECONFIG   0xA6         //硬件配置
#define TWPC_A_HARDWARECONFIG_HARDVERSION   0x00    //读硬件版本
#define TWPC_A_HARDWARECONFIG_CANID         0x01    //读CANID
#define TWPC_A_HARDWARECONFIG_MAX           0x02    //读MAX 码（生产批次，烧写日期）
#define TWPC_A_HARDWARECONFIG_FIRMWARE      0x03    //读取硬件固件版本号
#define TWPC_A_HARDWARECONFIG_SCANTIME      0x04    //读扫码时间的设置值
#define TWPC_A_HARDWARECONFIG_ID            0x05    //读设备唯一序列号
#define TWPC_A_HARDWARECONFIG_BOOTSCREEN    0x06    //读开机界面
#define TWPC_A_HARDWARECONFIG_ETH0IP        0x07    //读网口0（eth0） ip 地址
#define TWPC_A_HARDWARECONFIG_ETH0MASK      0x08    //读网口0（eth0） 子网掩码
#define TWPC_A_HARDWARECONFIG_ETH0GATEWAY   0x09    //读网口0（eth0） 网关地址
#define TWPC_A_HARDWARECONFIG_ETH1IP        0x0A    //读网口1（eth1） ip 地址
#define TWPC_A_HARDWARECONFIG_ETH1MASK      0x0B    //读网口1（eth1） 子网掩码
#define TWPC_A_HARDWARECONFIG_ETH1GATEWAY   0x0C    //读网口1（eth1） 网关地址
#define TWPC_A_HARDWARECONFIG_DEFAULTGATEWAY    0x0D    //读默认网关（eth0，eth1，ppp0）
#define TWPC_A_HARDWARECONFIG_SOFTVERSION   0x0E        //读硬件的软件版本
#define TWPC_A_HARDWARECONFIG_HARDWARETEMP  0x0F        //读硬件温度
#define TWPC_A_HARDWARECONFIG_HARDWARETIME  0x10        //读硬件当前时间
#define TWPC_A_HARDWARECONFIG_SETCANID    0x31        //更新CANID
#define TWPC_A_HARDWARECONFIG_SETSCANTIME   0x32        //更新每轮扫码最长时间，单位秒
#define TWPC_A_HARDWARECONFIG_SETENV        0x33        //更新应用环境的设置
#define TWPC_A_HARDWARECONFIG_SETETH0IP     0x40        //更新网口0（eth0） ip 地址
#define TWPC_A_HARDWARECONFIG_SETETH0MASK   0x41        //更新网口0（eth0） 子网掩码
#define TWPC_A_HARDWARECONFIG_SETETH0GATEWAY    0x42    //更新网口0（eth0） 网关地址
#define TWPC_A_HARDWARECONFIG_SETETH1IP     0x43        //更新网口0（eth1） ip 地址
#define TWPC_A_HARDWARECONFIG_SETETH1MASK   0x44        //更新网口0（eth1） 子网掩码
#define TWPC_A_HARDWARECONFIG_SETETH1GATEWAY    0x45    //更新网口0（eth1） 网关地址
#define TWPC_A_HARDWARECONFIG_SETLED        0x50        //配置LED 屏显示屏的8 个变量
#define TWPC_A_HARDWARECONFIG_SETHARDVERSION 0x60       //更新硬件版本信息
#define TWPC_A_HARDWARECONFIG_SETMAX         0x61       //更新MAX 码（生产批次，烧写日期）
#define TWPC_A_HARDWARECONFIG_SETID         0x62        //更新设备唯一序列号
#define TWPC_A_HARDWARECONFIG_SETBOOTSCREENID  0x63     //更新开机界面ID
#define TWPC_A_HARDWARECONFIG_TESTLED        0x70       //LED 节目测试，轮询一遍，间隔2s
#define TWPC_A_HARDWARECONFIG_DATATRANS      0x90       //将上位机的数据，透传到指定串口

// A7
#define TWPC_A_AUTHCONTROL      0xA7         //权限控制
#define TWPC_A_AUTHCONTROL_CHECK 0x00       //终端合法性校验


// AA
#define TWPC_A_AGENCYDEBIT      0xAA         //代扣相关内容
#define TWPC_A_AGENCYDEBIT_QUERY 0x01        //代扣查询相关
#define TWPC_A_AGENCYDEBIT_TRANSFERIMAGE 0x02        //图象回馈


//AF，add2018-04 ，主要用内部测试
#define TWPC_A_GREENCHANNEL    0xAF
#define TWPC_A_GREENCHANNEL_NFC 0x01     


/**
 * B指令
 */
// B1
#define TWPC_B_BEAT             0xB1            //
#define TWPC_B_BEAT_AUTO        0x00            //自动上传心跳信息
#define TWPC_B_BEAT_FEEDBACK    0x01            //回馈握手（上位机发起结果）
#define TWPC_B_BEAT_SET         0x02            //回馈调整自动心跳时间结果

// B2
#define TWPC_B_INTERACTIVE      0xB2         //信息交互
#define TWPC_B_INTERACTIVE_WELCOME      0x01    //回馈设置欢迎信息模式
#define TWPC_B_INTERACTIVE_INTERFACEBLOCK      0x02    //回馈显示交互信息（数据块格式）
#define TWPC_B_INTERACTIVE_BYE          0x03    //显回馈示欢送信息（无参数）
#define TWPC_B_INTERACTIVE_04    0x04    //未定义清楚，暂停使用
#define TWPC_B_INTERACTIVE_SCREEN   0x05    //回馈屏幕节目控制
#define TWPC_B_INTERACTIVE_INTERCACEJSON      0x06       //回馈显示交互信息（JSON 格式）


// B3
#define TWPC_B_STARTPAY         0xB3         //启动支付
#define TWPC_B_STARTPAY_PAYBLOCK         0x01         //回馈启动支付流程（数据块格式）
#define TWPC_B_STARTPAY_PAYJSON         0x02        //回馈启动支付流程（JSON 格式）

// B4
#define TWPC_B_CANCLE           0xB4         //撤单
#define TWPC_B_CANCLE_DATABLOCK 0x01        //回馈启动撤单流程（数据块格式）
#define TWPC_B_CANCLE_JSON      0x02             //回馈启动撤单流程（JSON 格式）

// B5
#define TWPC_B_SENDDATA         0xB5         //数据传输
#define TWPC_B_SENDDATA_INITJSON    0x00        //传输初始化信息（JSON 格式）
#define TWPC_B_SENDDATA_LANETIME    0x01        //传输上位机的当前时间
#define TWPC_B_SENDDATA_LASTERROR      0x02        //要求提供上次错误信息
#define TWPC_B_SENDDATA_INITBLOCK        0x03        //传输初始化信息（数据块模式）
#define TWPC_B_SENDDATA_FILETRANS        0x04        //文件传输
#define TWPC_B_SENDDATA_TICKNOTRANS        0x05        //tickno

// B6
#define TWPC_B_HARDWARECONFIG   0xB6         //硬件配置
#define TWPC_B_HARDWARECONFIG_HARDVERSION   0x00    //读硬件版本
#define TWPC_B_HARDWARECONFIG_CANID         0x01    //读CANID
#define TWPC_B_HARDWARECONFIG_MAX           0x02    //读MAX 码（生产批次，烧写日期）
#define TWPC_B_HARDWARECONFIG_FIRMWARE      0x03    //读取硬件固件版本号
#define TWPC_B_HARDWARECONFIG_SCANTIME      0x04    //读扫码时间的设置值
#define TWPC_B_HARDWARECONFIG_ID            0x05    //读设备唯一序列号
#define TWPC_B_HARDWARECONFIG_BOOTSCREEN    0x06    //读开机界面
#define TWPC_B_HARDWARECONFIG_ETH0IP        0x07    //读网口0（eth0） ip 地址
#define TWPC_B_HARDWARECONFIG_ETH0MASK      0x08    //读网口0（eth0） 子网掩码
#define TWPC_B_HARDWARECONFIG_ETH0GATEWAY   0x09    //读网口0（eth0） 网关地址
#define TWPC_B_HARDWARECONFIG_ETH1IP        0x0A    //读网口1（eth1） ip 地址
#define TWPC_B_HARDWARECONFIG_ETH1MASK      0x0B    //读网口1（eth1） 子网掩码
#define TWPC_B_HARDWARECONFIG_ETH1GATEWAY   0x0C    //读网口1（eth1） 网关地址
#define TWPC_B_HARDWARECONFIG_DEFAULTGATEWAY    0x0D    //读默认网关（eth0，eth1，ppp0）
#define TWPC_B_HARDWARECONFIG_SOFTVERSION   0x0E        //读硬件的软件版本
#define TWPC_B_HARDWARECONFIG_HARDWARETEMP  0x0F        //读硬件温度
#define TWPC_B_HARDWARECONFIG_HARDWARETIME  0x10        //读硬件当前时间
#define TWPC_B_HARDWARECONFIG_SETCANID    0x31        //更新CANID
#define TWPC_B_HARDWARECONFIG_SETSCANTIME   0x32        //更新每轮扫码最长时间，单位秒
#define TWPC_B_HARDWARECONFIG_SETENV        0x33        //更新应用环境的设置
#define TWPC_B_HARDWARECONFIG_SETETH0IP     0x40        //更新网口0（eth0） ip 地址
#define TWPC_B_HARDWARECONFIG_SETETH0MASK   0x41        //更新网口0（eth0） 子网掩码
#define TWPC_B_HARDWARECONFIG_SETETH0GATEWAY    0x42    //更新网口0（eth0） 网关地址
#define TWPC_B_HARDWARECONFIG_SETETH1IP     0x43        //更新网口0（eth1） ip 地址
#define TWPC_B_HARDWARECONFIG_SETETH1MASK   0x44        //更新网口0（eth1） 子网掩码
#define TWPC_B_HARDWARECONFIG_SETETH1GATEWAY    0x45    //更新网口0（eth1） 网关地址
#define TWPC_B_HARDWARECONFIG_SETLED        0x50        //配置LED 屏显示屏的8 个变量
#define TWPC_B_HARDWARECONFIG_SETHARDVERSION 0x60       //更新硬件版本信息
#define TWPC_B_HARDWARECONFIG_SETMAX         0x61       //更新MAX 码（生产批次，烧写日期）
#define TWPC_B_HARDWARECONFIG_SETID         0x62        //更新设备唯一序列号
#define TWPC_B_HARDWARECONFIG_SETBOOTSCREENID  0x63     //更新开机界面ID
#define TWPC_B_HARDWARECONFIG_TESTLED        0x70       //LED 节目测试，轮询一遍，间隔2s
#define TWPC_B_HARDWARECONFIG_DATATRANS      0x90       //将上位机的数据，透传到指定串口

// B7

#define TWPC_B_AUTHCONTROL      0xB7         //权限控制
#define TWPC_B_AUTHCONTROL_CHECK 0x00       //终端合法性校验


// BA

#define TWPC_B_AGENCYDEBIT     0xBA      //代扣回馈
#define TWPC_B_AGENCYDEBIT_QUERY 0x01   //代扣查询回馈
#define TWPC_B_AGENCYDEBIT_TRANSFERIMAGE 0x02        //图象回馈

//BF，add2018-04 ，主要用内部测试
#define TWPC_B_GREENCHANNEL    0xBF
#define TWPC_B_GREENCHANNEL_NFC 0x01     

/**
 * C指令
 */
#define TWPC_C_REPLY             0xC0         //握手,心跳
#define TWPC_C_REPLY_SUCCESS            0x00            //数据校验正常
#define TWPC_C_REPLY_INVALIDFORMAT      0x01            //校验失败（标识长度错误）
#define TWPC_C_REPLY_INVALIDDATACHECK   0x02            //校验失败（校验码错误)
#define TWPC_C_REPLY_INVALIDDATAUNDEF   0x03            //校验失败（未知错误）
#define TWPC_C_REPLY_INVALIDCMD         0x04            //校验失败（主命令非法）



/**
 * D指令
 */
// D1
#define TWPC_D_QUERY 0xD1       //下位机查询
#define TWPC_D_QUERY_STATUS 0x00 //查询上位机状态
#define TWPC_D_QUERY_VERSION 0x01 //查询上位机版本

// D2
#define TWPC_D_SELFCHECK 0xD2   //下位机自检
#define TWPC_D_SELFCHECK_POWER  0x00 //上电自检

/**
 * E指令
 */
// E1 
#define TWPC_E_QUERY 0xE1       //下位机查询
#define TWPC_E_QUERY_STATUS 0x00 //查询上位机状态
#define TWPC_E_QUERY_VERSION 0x01 //查询上位机版本




#endif // TW_PROTOCOL_CMD_H

