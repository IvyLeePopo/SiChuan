/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: TypeHelper.cpp
* ժ   Ҫ:  ��������ת��
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-12
* �޸ļ�¼: 
*/

#include "TypeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//��д�� scanmachine
std::string Bin2Hex(const unsigned char* bin_buf,size_t buf_len)
{
    CString strHex;
    CString strTemp;
    if(NULL != bin_buf)
    {
        for ( size_t i = 0; i < buf_len; ++i )
        {
            strTemp.Format(_T("%02X"), bin_buf[i] );
            strHex += strTemp;
        }
    }
    return strHex.GetBuffer(0);
}
//��д�� scanmachine
std::string Bin2Hex(const std::vector<unsigned char> src)
{
    CString strHex;
    CString strTemp;
    if(!src.empty())
    {
        for ( DWORD i = 0; i < src.size(); ++i )
        {
            strTemp.Format(_T("%02X "), src[i] );
            strHex += strTemp;
        }
    }
    return strHex.GetBuffer(0);
}
std::string Bin2Hex(const std::list<unsigned char> src)
{
    std::vector<unsigned char> tmp;
    tmp.insert(tmp.end(),src.begin(),src.end());
    return Bin2Hex(tmp);
}
//��д����scanmachine
bool Hex2Bin(const char* hex_buf,char* bin_buf,size_t hex_len)
{
    if(hex_len%2)
        return false;

    CString strTemp(hex_buf); 
    strTemp.MakeUpper();

    USES_CONVERSION;
    for(int i=0;i<strTemp.GetLength()/2;i++)
    {
        BYTE a = (BYTE)(strTemp.GetAt(i*2)>='A')?(BYTE)(strTemp.GetAt(i*2)-'A'+10):(BYTE)(strTemp.GetAt(i*2)-'0');
        BYTE b = (BYTE)(strTemp.GetAt(i*2+1)>='A')?(BYTE)(strTemp.GetAt(i*2+1)-'A'+10):(BYTE)(strTemp.GetAt(i*2+1)-'0');
        bin_buf[i] = (BYTE)(a * 0x10 + b);
    }
    return true;
}

std::string Int2Str(int info)
{
    char numStr[30]={0};
    itoa(info,numStr,10);
    return numStr;
}

int Str2Int(const std::string& info)
{
    return atoi(info.c_str());
}

//////////////////////////////////////////////////////////////
// ��������:Str2Datetime
//
// ��������:ת�������ַ��������ͣ�ֻ֧�̶ֹ���ʽ������
//
// �������:const std::string& info,
// �������:time_t 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
time_t Str2Datetime(const std::string& info)
{
    tm tm_;  
    int year, month, day, hour, minute,second;
    year = month = day = hour = minute = second = 0;
    sscanf(info.c_str(),"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);  
    tm_.tm_year  = year-1900;  
    tm_.tm_mon   = month-1;  
    tm_.tm_mday  = day;  
    tm_.tm_hour  = hour;  
    tm_.tm_min   = minute;  
    tm_.tm_sec   = second;  
    tm_.tm_isdst = 0;  
  
    time_t t_ = mktime(&tm_); //�Ѿ�����8��ʱ��  
    return t_;
}

std::string Datetime2Str(time_t dt)
{
	if(dt<=0)
	{
		return "1970-01-01 00:00:00";
	}
    struct tm tm1;
    char result[50]={0};
    tm1 = *localtime(&dt);
    sprintf(result,"%04d-%02d-%02d %02d:%02d:%02d.1",tm1.tm_year+1900,tm1.tm_mon+1,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);

    return std::string(result);
}

//////////////////////////////////////////////////////////////
// ��������:String2IP
//
// ��������:ת���ַ�����ʽ(xxx.xxx.xxx.xxx)��IP��int
//
// �������:const std::string& info,
// �������:unsigned int 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-06-09
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
unsigned int String2IP(const std::string& info)
{
    unsigned int result = 0;
    unsigned int first,second,third,four;
    first = second = third = four = 0;
    sscanf(info.c_str(),"%d.%d.%d.%d",&first,&second,&third,&four);
	unsigned char cFirst,cSecond,cThird,cFour;
	cFirst = first & 0x000000FF;
	cSecond = second & 0x000000FF;
	cThird = third & 0x000000FF;
	cFour = four & 0x000000FF;
    result = (cFirst<<24)|(cSecond<<16)|(cThird<<8)|cFour;
    return result;
}