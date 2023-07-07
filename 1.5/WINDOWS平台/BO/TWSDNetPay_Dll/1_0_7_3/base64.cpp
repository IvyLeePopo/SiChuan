/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: base64.cpp
* 摘   要:  
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-08-11
* 修改记录: 
*/
#include "stdafx.h"
#include "base64.h"
#include <ctype.h>
#define BYTE unsigned char
BYTE Decode_GetByte(char c);
char Encode_GetChar(BYTE num);

//===================================
//    Base64 解码
//===================================
BYTE Decode_GetByte(char c)
{
    if(c == '+')
        return 62;
    else if(c == '/')
        return 63;
    else if(c <= '9')
        return (BYTE)(c - '0' + 52);
    else if(c == '=')
        return 64;
    else if(c <= 'Z')
        return (BYTE)(c - 'A');
    else if(c <= 'z')
        return (BYTE)(c - 'a' + 26);
    return 64;
}


static const std::string base64_chars =   
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"  
             "abcdefghijklmnopqrstuvwxyz"  
             "0123456789+/";  
  
  
static inline bool is_base64(unsigned char c) {  
  return (isalnum(c) || (c == '+') || (c == '/'));  
} 

//解码
size_t Base64_Decode(char *pDest, const char *pSrc, size_t srclen)
{
    BYTE input[4];
    size_t i, index = 0;
    for(i = 0; i < srclen; i += 4)
    {
        //byte[0]
        input[0] = Decode_GetByte(pSrc[i]);
        input[1] = Decode_GetByte(pSrc[i + 1]);
        pDest[index++] = (input[0] << 2) + (input[1] >> 4);
        
        //byte[1]
        if(pSrc[i + 2] != '=')
        {
            input[2] = Decode_GetByte(pSrc[i + 2]);
            pDest[index++] = ((input[1] & 0x0f) << 4) + (input[2] >> 2);
        }

        //byte[2]
        if(pSrc[i + 3] != '=')
        {
            input[3] = Decode_GetByte(pSrc[i + 3]);
            pDest[index++] = ((input[2] & 0x03) << 6) + (input[3]);
        }            
    }

    //null-terminator
    pDest[index] = 0;
    return index;
}

//===================================
//    Base64 编码
//===================================
char Encode_GetChar(BYTE num)
{
    return 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/="[num];
}

//编码
size_t Base64_Encode(char *pDest, const char *pSrc, size_t srclen)
{
    BYTE input[3], output[4];
    size_t i, index_src = 0, index_dest = 0;
    for(i = 0; i < srclen; i += 3)
    {
        //char [0]
        input[0] = pSrc[index_src++];
        output[0] = (BYTE)(input[0] >> 2);
        pDest[index_dest++] = Encode_GetChar(output[0]);

        //char [1]
        if(index_src < srclen)
        {
            input[1] = pSrc[index_src++];
            output[1] = (BYTE)(((input[0] & 0x03) << 4) + (input[1] >> 4));
            pDest[index_dest++] = Encode_GetChar(output[1]);
        }
        else
        {
            output[1] = (BYTE)((input[0] & 0x03) << 4);
            pDest[index_dest++] = Encode_GetChar(output[1]);
            pDest[index_dest++] = '=';
            pDest[index_dest++] = '=';
            break;
        }
        
        //char [2]
        if(index_src < srclen)
        {
            input[2] = pSrc[index_src++];
            output[2] = (BYTE)(((input[1] & 0x0f) << 2) + (input[2] >> 6));
            pDest[index_dest++] = Encode_GetChar(output[2]);
        }
        else
        {
            output[2] = (BYTE)((input[1] & 0x0f) << 2);
            pDest[index_dest++] = Encode_GetChar(output[2]);
            pDest[index_dest++] = '=';
            break;
        }

        //char [3]
        output[3] = (BYTE)(input[2] & 0x3f);
        pDest[index_dest++] = Encode_GetChar(output[3]);
    }
    //null-terminator
    pDest[index_dest] = 0;
    return index_dest;
}


 
void Base64_Decode(std::list<char>& pDest,const std::string& encoded_string) {  
   const char DecodeTable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };
    int DataByte = encoded_string.size();
    const char* Data = encoded_string.c_str();
    //返回值
    //string strDecode;
    int nValue;
    int i= 0;
    while (i < DataByte)
    {
        if (*Data != '\r' && *Data!='\n')
        {
            nValue = DecodeTable[*Data++] << 18;
            nValue += DecodeTable[*Data++] << 12;
            pDest.push_back((nValue & 0x00FF0000) >> 16);
            // OutByte++;
            if (*Data != '=')
            {
                nValue += DecodeTable[*Data++] << 6;
                pDest.push_back((nValue & 0x0000FF00) >> 8);
                // OutByte++;
                if (*Data != '=')
                {
                    nValue += DecodeTable[*Data++];
                    pDest.push_back(nValue & 0x000000FF);
                    // OutByte++;
                }
            }
            i += 4;
        }
        else// 回车换行,跳过
        {
            Data++;
            i++;
        }
     }
    // return strDecode;

}  