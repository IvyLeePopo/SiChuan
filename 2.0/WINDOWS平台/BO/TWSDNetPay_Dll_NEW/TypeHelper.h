/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: TypeHelper.h
* 摘   要:  
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-12
* 修改记录: 
*/
#ifndef TYPE_HELPER_H
#define TYPE_HELPER_H
#include <string>
#include <vector>
#include <list>


std::string Bin2Hex(const unsigned char* bin_buf,size_t buf_len);

std::string Bin2Hex(const std::vector<unsigned char> src);
std::string Bin2Hex(const std::list<unsigned char> src);
std::string Int2Str(int info);

int Str2Int(const std::string& info);
bool Hex2Bin(const char* hex_buf,char* bin_buf,size_t hex_len);
void RemoveWhiteSpaceInStr(std::string&str);

bool Hex2Bin(const char* hex_buf,std::list<unsigned char>&bin_buf,size_t hex_len);

time_t Str2Datetime(const std::string& info);
std::string Datetime2Str(time_t dt);

unsigned int String2IP(const std::string& info);


template<typename T>
std::vector<unsigned char> ConvertNetBytes(const T& t,const int T_size = sizeof(T))
{
	std::vector<unsigned char> result(T_size);
    const unsigned char *  pc = (const unsigned char*)&t;
    std::reverse_copy(pc,pc+T_size,result.begin());
    return result;
}
template<typename Con1,typename Con2>
void JoinContainer(Con1& c1,Con2& c2)
{
	c1.insert(c1.end(),c2.begin(),c2.end());
}



#endif // TYPE_HELPER_H
