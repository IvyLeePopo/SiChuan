/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: ErrorNo.h
* 摘   要:  定义系统所使用的错误码信息
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-06
* 修改记录: 
*/

#include "ErrorNoBase.h"
#define TW_ERROR_OPERATION_NOT_SUPPORTED  0x00000100   /*操作不支持*/
#define TW_ERROR_SYSTEM_MEMORY_ALLOC      0x00000101   /*内存分配失败*/
#define TW_ERROR_OPERATOR_CANCLE		  0x00000102	/*操作被取消*/
#define TW_ERROR_COMMUNICATION            0x00000103	/*与下位机通讯异常*/
#define TW_ERROR_WAIT_RESULT			  0x00000104   /*等待结果错误*/