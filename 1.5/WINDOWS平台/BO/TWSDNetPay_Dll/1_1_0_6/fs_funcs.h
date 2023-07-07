/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: fs_funcs.h
* 摘   要:  
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-21
* 修改记录: 
*/

#ifndef FS_FUNCS_H
#define FS_FUNCS_H
#include <string>

std::string fs_GetModulePath();
std::string fs_SplitBasePath(const std::string& path);
std::string fs_PathJoin(const std::string& path1,const std::string& path2 );
std::string fs_SplitBaseName(const std::string& baseinfo);
std::string fs_thisModulePath();
bool fs_pathIsExist(const std::string&);

#include <string>
//#include <dir.h>
#include <io.h>
#include <time.h>
#include <list>
using namespace std;
#ifndef MAX_PATH
#define MAX_PATH 260
#endif


struct Search_file_info
{
    char root_catalog[MAX_PATH + 1];
    char pattern[MAX_PATH + 1];
    time_t create_before;
    time_t create_after;
    bool recursion;                 /*递归查找*/
    bool filter_locked_file;        /*过滤正在被占用的文件*/
    size_t max_file_cnt;
    bool enable_wildcard;           /*是否支持通配符*/

    Search_file_info()
    {
        memset ( this, 0, sizeof ( Search_file_info ) );
    }

};
struct Move_file_info
{
    char src[MAX_PATH + 1];
    char dest[MAX_PATH + 1];
    bool keep_src;          /*是否保留原文件*/

    Move_file_info()
    {
        memset ( this, 0, sizeof ( Move_file_info ) );
    }
};

list<string> Search_files ( const Search_file_info& search_info );
list<string> Search_catalogs ( const Search_file_info& catalog );
bool Move_file_to ( const Move_file_info& move_info );
bool Delete_file ( const string& file_name );
bool Delete_files ( const list<string>& files );
bool Move_file_to_target_catalog ( Move_file_info& move_info, string dest_catalog );

std::string Make_directory(const char* path);
std::string Make_full_directory(std::string path);



#endif // FS_FUNCS_H
