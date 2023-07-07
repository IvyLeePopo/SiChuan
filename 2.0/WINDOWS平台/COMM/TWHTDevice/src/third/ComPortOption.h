/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: ComPortOption.h
* 摘   要:  定义串口通讯中使用的参数数据
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-07
* 修改记录: 
*/

#define COM_BAUD_9600    9600         //波特率9600

#define COM_CHECK_NONE   0           //无校验
#define COM_CHECK_ODD    1           //基数校验
#define COM_CHECK_EVEN   2           //偶数校验
#define COM_CHECK_MARK   3           //校验位始终1
#define COM_CHECK_SPACE  4           //校验位始终0

#define COM_STOP_1       0           //一个停止位
#define COM_STOP_1_half  1           //1.5个停止位
#define COM_STOP_2       2           //两个停止位

#define COM_DATA_8       8           //八个数据位

#define COM_FLUSH_ALL    0           //读缓冲区和写缓冲区
#define COM_FLUSH_READ   1           //读缓冲区
#define COM_FLUSH_WRITE  2           //写缓冲区
