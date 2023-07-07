/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: DataAuth.h
* 摘   要:  
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-11
* 修改记录: 
*/
#ifndef DATA_AUTH_H
#define DATA_AUTH_H
#include <vector>
unsigned short crc16(unsigned char*buf, unsigned short len );
unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,const std::vector<unsigned char>::const_iterator& it_second);


#endif // DATA_AUTH_H


