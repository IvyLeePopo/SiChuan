/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: DataAuth.cpp
* ժ   Ҫ:
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-11
* �޸ļ�¼:
*/
#include "stdafx.h"
#include "DataAuth.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


unsigned short crc16(const std::vector<unsigned char>& src)
{
	if(src.empty())
	{
		return 0;
	}
	BYTE    CRC16Hi = 0xFF;
    BYTE    CRC16Lo = 0xFF;
    BYTE    CH = 0xA0;
    BYTE    CL = 0x01;
    BYTE    SaveHi = 0;
    BYTE    SaveLo = 0;

    for (size_t i = 0; i < src.size(); i++)
    {
        CRC16Lo = CRC16Lo ^ src[i];

        for (int Flag = 0; Flag < 8; Flag++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi / 2; //��λ����һλ
            CRC16Lo = CRC16Lo / 2; //��λ����һλ
            if ((SaveHi & 1) == 1)
                CRC16Lo |= 0x80;
            if ((SaveLo & 1) == 1)
            {
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }//for(int Flag = 0;Flag < 8;Flag++)
    }//for(int i = 0;i < nLenght;i++)
    return (WORD)(CRC16Hi * 0x100 + CRC16Lo);

}
unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,const std::vector<unsigned char>::const_iterator& it_second)
{
    if(it_first==it_second)
    {
        return 0;
    }
    BYTE    CRC16Hi = 0xFF;
    BYTE    CRC16Lo = 0xFF;
    BYTE    CH = 0xA0;
    BYTE    CL = 0x01;
    BYTE    SaveHi = 0;
    BYTE    SaveLo = 0;

    for(std::vector<unsigned char>::const_iterator it = it_first;it!=it_second;++it)
    {
         CRC16Lo = CRC16Lo ^ (*it);

        for (int Flag = 0; Flag < 8; Flag++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi / 2; //��λ����һλ
            CRC16Lo = CRC16Lo / 2; //��λ����һλ
            if ((SaveHi & 1) == 1)
                CRC16Lo |= 0x80;
            if ((SaveLo & 1) == 1)
            {
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }//for(int Flag = 0;Flag < 8;Flag++)
    }

    return (WORD)(CRC16Hi * 0x100 + CRC16Lo);
}

unsigned short crc16(unsigned char*lpData, unsigned short wLength )
{
    //���´�������ԭ�й���
    //
    BYTE    CRC16Hi = 0xFF;
    BYTE    CRC16Lo = 0xFF;
    BYTE    CH = 0xA0;
    BYTE    CL = 0x01;
    BYTE    SaveHi = 0;
    BYTE    SaveLo = 0;

    if (NULL == lpData)
        return 0;

    for (int i = 0; i < wLength; i++)
    {
        CRC16Lo = CRC16Lo ^ lpData[i];

        for (int Flag = 0; Flag < 8; Flag++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi / 2; //��λ����һλ
            CRC16Lo = CRC16Lo / 2; //��λ����һλ
            if ((SaveHi & 1) == 1)
                CRC16Lo |= 0x80;
            if ((SaveLo & 1) == 1)
            {
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }//for(int Flag = 0;Flag < 8;Flag++)
    }//for(int i = 0;i < nLenght;i++)
    return (WORD)(CRC16Hi * 0x100 + CRC16Lo);
}
