/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: DataAuth.h
* ժ   Ҫ:  
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-11
* �޸ļ�¼: 
*/
#ifndef DATA_AUTH_H
#define DATA_AUTH_H
#include <vector>
unsigned short crc16(unsigned char*buf, unsigned short len );
unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,const std::vector<unsigned char>::const_iterator& it_second);


#endif // DATA_AUTH_H


