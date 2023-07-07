/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: string_func.cpp
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-08-11
* 修改记录:
*/
#include "stdafx.h"
#include "string_func.h"
#include <vector>


std::list<std::string> split_str(const std::string& my_info, char sep_ch)
{
	if (my_info.empty())
	{
		return std::list<std::string>();
	}
	std::list<std::string> result;
	int pre_pos , next_pos;
	pre_pos = next_pos = 0;
	
	while(my_info[pre_pos]==sep_ch && pre_pos < my_info.size())
	{
		++pre_pos;
	}
	while (true)
	{
		next_pos = my_info.find(sep_ch, pre_pos);

		if (next_pos == std::string::npos)
		{
			if(pre_pos != (my_info.size()-1))
			{
				result.push_back(my_info.substr(pre_pos));
			}
			return result;
		}
		if(next_pos == pre_pos )
		{
			++pre_pos;
			continue;
		}
		result.push_back(my_info.substr(pre_pos,next_pos-pre_pos));
		pre_pos=next_pos+1;
	}
	return result;


}



std::string Int2str ( int val )
{
	char chs[100] = {0};
	itoa ( val, chs, 10 );
	return std::string ( chs );
}


int Str2int ( const std::string& val )
{
	return atoi ( val.c_str() );
}

double Str2double ( const std::string& val )
{
	return atof ( val.c_str() );
}


float Str2float ( const std::string& val )
{
	return atof ( val.c_str() );
}


std::string Extract_file_full_name ( const std::string& info )
{
	size_t pos = info.find_last_of ( '\\' );
	if ( pos == std::string::npos )
	{
		pos = 0;
	}
	return info.substr ( pos + 1 );
}


std::string Extract_file_full_name_without_ext ( const std::string& info )
{
	std::string tmp_str = Extract_file_full_name ( info );
	size_t pos = tmp_str.find_last_of ( '.' );
	if ( pos == std::string::npos )
	{
		pos = tmp_str.size();
	}
	return tmp_str.substr ( 0, pos );
}

bool Is_all_digit ( const std::string& val )
{
	if ( val.empty() )
		return false;
	if ( val.find_first_not_of ( "0123456789" ) != std::string::npos )
	{
		return false;
	}
	return true;
}


std::list<std::string> Split_str (  std::string& my_info, char sep_ch )
{
	if (my_info.empty())
	{
		return std::list<std::string>();
	}
	std::list<std::string> result;
	int pre_pos , next_pos;
	pre_pos = next_pos = 0;
	
	while(my_info[pre_pos]==sep_ch && pre_pos < my_info.size())
	{
		++pre_pos;
	}
	while (true)
	{
		next_pos = my_info.find(sep_ch, pre_pos);

		if (next_pos == std::string::npos)
		{
			if(pre_pos != (my_info.size()-1))
			{
				result.push_back(my_info.substr(pre_pos));
			}
			return result;
		}
		if(next_pos == pre_pos )
		{
			++pre_pos;
			continue;
		}
		result.push_back(my_info.substr(pre_pos,next_pos-pre_pos));
		pre_pos=next_pos+1;
	}
	return result;

}


std::string Filt_target_char ( std::string src, char target )
{
	while ( !src.empty() )
	{
		size_t pos = src.find ( target );
		if ( pos == std::string::npos )
			break;
		src.erase ( pos, 1 );
	}
	return src;
}


std::string Upper_case ( std::string str )
{
	for ( size_t i = 0; i < str.size(); ++i )
	{
		if ( str[i] >= 'a' && str[i] <= 'z' )
		{
			str[i] -= 32;
		}
	}
	return str;
}
std::string Lower_case ( std::string str )
{
	for ( size_t i = 0; i < str.size(); ++i )
	{
		if ( str[i] >= 'A' && str[i] <= 'Z' )
		{
			str[i] += 32;
		}
	}
	return str;
}

std::wstring UT2WC ( const char* buf );
std::string WC2UT ( const wchar_t* buf );
std::wstring MB2WC ( const char* buf );
std::string WC2MB ( const wchar_t* buf );

std::wstring Unicode2widechar ( const char* buf )
{
	return UT2WC ( buf );
}
std::string Widechar2unicode ( const wchar_t* buf )
{
	return WC2UT ( buf );
}
std::wstring Multibyte2widechar ( const char* buf )
{
	return MB2WC ( buf );
}
std::string Widechar2multibyte ( const wchar_t* buf )
{
	return WC2MB ( buf );
}


std::wstring UT2WC ( const char* buf )
{
	int len = MultiByteToWideChar ( CP_UTF8, 0, buf, -1, NULL, 0 );
	std::vector<wchar_t> unicode ( len );
	MultiByteToWideChar ( CP_UTF8, 0, buf, -1, &unicode[0], len );
	return std::wstring ( &unicode[0] );
}

std::string WC2UT ( const wchar_t* buf )
{
	int len = WideCharToMultiByte ( CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL );
	std::vector<char> utf8 ( len );
	WideCharToMultiByte ( CP_UTF8, 0, buf, -1, &utf8[0], len, NULL, NULL );
	return std::string ( &utf8[0] );
}

std::wstring MB2WC ( const char* buf )
{
	int len = MultiByteToWideChar ( CP_ACP, 0, buf, -1, NULL, 0 );
	std::vector<wchar_t> unicode ( len );
	MultiByteToWideChar ( CP_ACP, 0, buf, -1, &unicode[0], len );
	return std::wstring ( &unicode[0] );
}

std::string WC2MB ( const wchar_t* buf )
{
	int len = WideCharToMultiByte ( CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL );
	std::vector<char> utf8 ( len );
	WideCharToMultiByte ( CP_ACP, 0, buf, -1, &utf8[0], len, NULL, NULL );
	return std::string ( &utf8[0] );
}



std::string erase_blank ( std::string data )
{
	std::size_t found = data.find_first_of ( ' ' );
	while ( found !=std::string::npos )
	{
		data.erase ( found, 1 );
		found = data.find_first_of ( ' ' );
	}
	return data;
}


//将yyyymmdd转为yyyy-mm-dd
std::string trans2dateformat ( std::string str )
{
	if ( str.length() >= 8 )
	{
		if ( str == "00000000" )
		{
			return "";
		}
		str.insert ( 4, 1, '-' );
		str.insert ( 7, 1, '-' );
		return str;
	}
	else
		return "";
}



std::string replace_all_string ( std::string src_str, const std::string& old_str, const std::string& new_str )
{
	size_t old_len = old_str.size();
	size_t new_len = new_str.size();

	if ( old_len == 0 )
	{
		return src_str;
	}

	size_t find_start = 0;
	while ( true )
	{
		size_t find_pos = src_str.find ( old_str, find_start );
		if ( find_pos == std::string::npos )
		{
			break;
		}
		src_str.replace ( find_pos, old_len, new_str );
		find_start = find_pos + new_len ;
	}

	return src_str;
}

std::string Extract_file_path ( const std::string& info )
{
	size_t pos = info.find_last_of ( '\\' );
	if ( pos == std::string::npos )
	{
		return ".\\";
	}
	return info.substr ( 0, pos + 1 );

}


