/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: ErrorHelper.h
* 摘   要:  用于记录和查询错误信息
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-10
* 修改记录: 
*/
#ifndef ERROR_HELPER_H
#define ERROR_HELPER_H
#include <string>
#include "ErrorNo.h"


#define TW_SET_ERROR(x) (SetErrorNo(x))
#define TW_RESET_ERROR() (ResetErrorNO())


typedef unsigned int TW_ERROR_TYPE;
void SetErrorNo(const TW_ERROR_TYPE err);
TW_ERROR_TYPE GetErrorNo();
void ResetErrorNO();
std::string GetErrorDesc();



#endif // ERROR_HELPER_H
