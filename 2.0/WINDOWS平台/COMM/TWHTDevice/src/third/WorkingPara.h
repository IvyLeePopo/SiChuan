/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: WorkingPara.h
* 摘   要:  定义系统运行时候使用的参数
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-07
* 修改记录: 
*/
#ifndef WORKING_PARA_H
#define WORKING_PARA_H


size_t SystemParaComRetryCount();   //串口重发次数
size_t SystemParaComOvertime();      //串口超时等待，单位：秒
size_t SystemParaComPort();     //串口号
size_t SystemParaComInBufferSize();
size_t SystemParaComOutBufferSize();

int SystemParaComBaud();

std::string SystemParaDBPath();

bool SystemParaEnableTransferImage();
bool SystemParaForceImageSize();

int SystemParaImageMaxSize();

#endif // WORKING_PARA_H
