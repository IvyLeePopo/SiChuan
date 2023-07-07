/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: string_func.h
* ժ   Ҫ:  
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-08-11
* �޸ļ�¼: 
*/
#ifndef STRING_FUNC_H
#define STRING_FUNC_H
#include <list>
#include <string>


std::string Int2str ( int val );
int Str2int ( const std::string& val );
double Str2double ( const std::string& val );
float Str2float ( const std::string& val );
std::string Extract_file_full_name ( const std::string& info );
std::string Extract_file_full_name_without_ext ( const std::string& info );
std::string Extract_file_path ( const std::string& info );
std::string Filt_target_char ( std::string src, char target );
bool Is_all_digit ( const std::string& val );
std::list<std::string> Split_str (  std::string& val, char ch );

std::string erase_blank ( std::string data );
std::string trans2dateformat ( std::string str );

std::wstring Unicode2widechar ( const char* buf );
std::string Widechar2unicode ( const wchar_t* buf );
std::wstring Multibyte2widechar ( const char* buf );
std::string Widechar2multibyte ( const wchar_t* buf );

std::string Upper_case ( std::string str );
std::string Lower_case ( std::string str );
std::string erase_blank ( const char* data );
std::string erase_blank ( std::string data );

std::string replace_all_string ( std::string src_str, const std::string& old_str, const std::string& new_str );


#endif // STRING_FUNC_H
