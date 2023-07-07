/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: base64.h
* ժ   Ҫ:  http://www.cnblogs.com/hoodlum1980/archive/2012/05/28/2521500.html
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-08-11
* �޸ļ�¼: 
*/
#ifndef BASE64_H
#define BASE64_H
#include <list>

size_t Base64_Decode(char *pDest, const char *pSrc, size_t srclen);
size_t Base64_Encode(char *pDest, const char *pSrc, size_t srclen);

void Base64_Decode(std::list<char>& lDest,const std::string& info);


#endif // BASE64_H

