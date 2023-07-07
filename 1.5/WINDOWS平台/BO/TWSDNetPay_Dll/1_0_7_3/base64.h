/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: base64.h
* 摘   要:  http://www.cnblogs.com/hoodlum1980/archive/2012/05/28/2521500.html
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-08-11
* 修改记录: 
*/
#ifndef BASE64_H
#define BASE64_H
#include <list>

size_t Base64_Decode(char *pDest, const char *pSrc, size_t srclen);
size_t Base64_Encode(char *pDest, const char *pSrc, size_t srclen);

void Base64_Decode(std::list<char>& lDest,const std::string& info);


#endif // BASE64_H

