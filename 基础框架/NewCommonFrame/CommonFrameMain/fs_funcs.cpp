/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: fs_funcs.cpp
* 摘   要:  文件操作相关的函数集
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-21
* 修改记录:
*/

#include "fs_funcs.h"
#include "string_func.h"

#include <stdio.h>
#include <io.h>
#include <fstream>

//#define _WIN32_WINNT 0x0400

//#include <winuser.h>
//#include <Wtsapi32.h>
#include <WinBase.h>





#include <direct.h>
#if _MSC_VER
#define snprintf _snprintf
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/**
 * 进行文件的移动
 * @param  src  源文件的路径
 * @param  dest 目的文件的路径
 * @return      不解释
 */
bool Move_file(const string &src, const string &dest)
{
    if (!MoveFileEx(src.c_str(), dest.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
    {
        return false;
    }
    return true;
}

/**
 * 将文件从一个路径拷贝到指定路径
 * @param  src  原文件路径
 * @param  dest 目的文件路径
 * @return      不解释
 */
bool Copy_file(const string &src, const string &dest)
{
    int un_use = 0;
    if (!CopyFileEx(src.c_str(), dest.c_str(), NULL, NULL, &un_use, COPY_FILE_ALLOW_DECRYPTED_DESTINATION))
    {
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:fs_GetModulePath
//
// 功能描述:获取程序路径
//
// 输入参数:
// 输出参数:std::string
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string fs_GetModulePath()
{
    char tmpChs[300] = {0};
    GetModuleFileName(NULL, tmpChs, 250);
    return std::string(tmpChs);
}
//////////////////////////////////////////////////////////////
// 函数名称:fs_SplitBasePath
//
// 功能描述:获取短文件名
//
// 输入参数:const std::string& path,
// 输出参数:std::string
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string fs_SplitBasePath(const std::string& path)
{
    size_t pos = path.find_last_of('\\');
    return path.substr(0, pos);
}
//////////////////////////////////////////////////////////////
// 函数名称:fs_PathJoin
//
// 功能描述:合并两个路径信息，主要用于拼接
//
// 输入参数:const std::string& path1,const std::string& path2,
// 输出参数:std::string
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string fs_PathJoin(const std::string& path1, const std::string& path2 )
{
    if (path1.empty())
    {
        return path2;
    }
    else if (path2.empty())
    {
        return path1;
    }

    bool addSepFlag = true;
    if (path1[path1.size() - 1] == '\\' || path2[path2.size() - 1] == '\\')
    {
        addSepFlag = false;
    }
    return path1 + (addSepFlag ? "\\" : "") + path2;
}



/*
* @Author: 朱亚冰
* @Date:   2015-01-05 11:29:58
* @Last Modified by:   windf
* @Last Modified time: 2017-08-17
* 功能说明：和业务无关的文件操作
*/



bool Satisfy_search_condition ( const _finddata_t& find_data, const Search_file_info& f_info );
bool Curr_or_parent ( const char* dir );

/**
 * 只查找文件夹，不查找文件,基本内容和查找文件
 * 方式一样，可以考虑合并
 */
list<string> Search_catalogs ( const Search_file_info& search_info )
{
    string root_dir = Make_directory ( search_info.root_catalog );
    string final_pattern = string ( search_info.pattern );
    if ( !search_info.enable_wildcard )
    {
        final_pattern =  Filt_target_char ( final_pattern, '*' );
    }

    list<string> all_catalogs;
    list<string> dirs;

    dirs.push_back ( root_dir );

    while ( !dirs.empty() )
    {

        string curr_dir = Make_directory ( dirs.front().c_str() );
        dirs.pop_front();
        char description[MAX_PATH + 1] = {0};

        snprintf ( description, MAX_PATH, "%s%s", curr_dir.c_str(), final_pattern.c_str() );

        long file_handle;
        struct _finddata_t find_info;

        file_handle = _findfirst ( description, &find_info );

        if ( file_handle == -1L )
        {
            _findclose ( file_handle );
            return all_catalogs;
        }

        if ( ( find_info.attrib & _A_SUBDIR ) == _A_SUBDIR && !Curr_or_parent ( find_info.name ) ) /*子文件夹*/
        {
            if ( search_info.recursion )    /*递归遍历*/
            {
                dirs.push_back ( curr_dir + find_info.name );
            }
            if ( Satisfy_search_condition ( find_info, search_info ) )
            {
                all_catalogs.push_back ( curr_dir + find_info.name );
            }
        }

        while ( _findnext ( file_handle, &find_info ) == 0 )
        {
            if ( search_info.max_file_cnt != 0 && all_catalogs.size() >= search_info.max_file_cnt )
            {
                break;
            }
            if ( ( find_info.attrib & _A_SUBDIR ) == _A_SUBDIR && !Curr_or_parent ( find_info.name ) )    /*子文件夹*/
            {
                if ( search_info.recursion )     /*递归遍历*/
                {
                    dirs.push_back ( curr_dir + find_info.name );
                }
                if ( Satisfy_search_condition ( find_info, search_info ) )
                {
                    all_catalogs.push_back ( curr_dir + find_info.name );
                }
            }

        }

        _findclose ( file_handle );

        if ( search_info.max_file_cnt != 0 && all_catalogs.size() >= search_info.max_file_cnt )
        {
            break;
        }
    }

    return all_catalogs;

}


/**
 * 根据结构体查找满足要求的文件
 */
list<string> Search_files ( const Search_file_info& search_info )
{
    string root_dir = Make_directory ( search_info.root_catalog );
    string final_pattern = string ( search_info.pattern );
    if ( !search_info.enable_wildcard )
    {
        final_pattern =  Filt_target_char ( final_pattern, '*' );
    }


    list<string> dirs;
    list<string> all_files;

    dirs.push_back ( root_dir );

    while ( !dirs.empty() )
    {

        string curr_dir = Make_directory ( dirs.front().c_str() );
        dirs.pop_front();
        char description[MAX_PATH + 1] = {0};

        snprintf ( description, MAX_PATH, "%s%s", curr_dir.c_str(), final_pattern .c_str() );

        long file_handle;
        struct _finddata_t find_info;

        file_handle = _findfirst ( description, &find_info );

        if ( file_handle == -1L )
        {
            _findclose ( file_handle );
            return all_files;
        }

        if ( ( find_info.attrib & _A_SUBDIR ) == _A_SUBDIR ) /*子文件夹*/
        {
            if ( search_info.recursion && !Curr_or_parent ( find_info.name ) ) /*递归遍历*/
            {
                dirs.push_back ( curr_dir + find_info.name );
            }
        }
        else if ( Satisfy_search_condition ( find_info, search_info ) )
        {
            all_files.push_back ( curr_dir + find_info.name );
        }

        while ( _findnext ( file_handle, &find_info ) == 0 )
        {
            if ( search_info.max_file_cnt != 0 && all_files.size() >= search_info.max_file_cnt )
            {
                break;
            }

            if ( ( find_info.attrib & _A_SUBDIR ) == _A_SUBDIR )        /*子文件夹*/
            {
                if ( search_info.recursion && !Curr_or_parent ( find_info.name ) ) /*递归遍历*/
                {
                    dirs.push_back ( curr_dir + find_info.name );
                }
                continue;
            }

            if ( Satisfy_search_condition ( find_info, search_info ) )
            {
                all_files.push_back ( curr_dir + find_info.name );
            }
        }

        _findclose ( file_handle );

        if ( search_info.max_file_cnt != 0 && all_files.size() >= search_info.max_file_cnt )
        {
            break;
        }

    }
    return all_files;
}




/**
 * 查找返回结构体是否满足指定条件
 * @param  find_data
 * @param  f_info
 * @return
 */
bool Satisfy_search_condition ( const _finddata_t& find_data, const Search_file_info& f_info )
{
    if ( f_info.create_before > 0 && find_data.time_create > f_info.create_before )
        return false;

    if ( f_info.create_after > 0 && find_data.time_create < f_info.create_after )
        return false;

    return true;
}

/**
 * 当前目录以及父目录的判定
 * @param  dir
 * @return
 */
bool Curr_or_parent ( const char* dir )
{
    if ( strcmp ( dir, "." ) == 0 || strcmp ( dir, ".." ) == 0 )
        return true;

    return false;
}

/**
 * 删除指定文件
 * @param  file_name
 * @return           不解释
 */
bool Delete_file ( const string& file_name )
{
    if ( remove ( file_name.c_str() ) != 0 )
    {
        return false;
    }

    return true;
}
/**
 * 删除一系列的文件
 * @param  files
 * @return       一个文件的删除失败不会影响后续文件的删除
 * 所以如果需要保证整个过程成功，那么不要使用这个功能
 */
bool Delete_files ( const list<string>& files )
{
    bool flag = true;
    list<string>::const_iterator it ;

    for ( it = files.begin(); it != files.end(); ++it )
    {
        flag &= Delete_file ( *it );

        if ( !flag )
        {
            flag = true;
        }
    }

    return flag;
}



std::string fs_thisModulePath()
{
    char modePath[300] = {0};
    GetModuleFileName(AfxGetInstanceHandle(), modePath, 260);
    std::string result = fs_SplitBasePath(modePath);
    return result;
}


string Make_directory(const char *path)
{
    string local_path(path);
    char pre_cwd[MAX_PATH + 1] = { 0 };

    /**
    保存当前的工作目录，因为中间创建目录的时候可能需要改变，所以为了之后
    的还原工作目录，进行保存
    */
    _getcwd(pre_cwd, MAX_PATH);

    /*
    相对路径
    */
    if (local_path.find(':') == string::npos)
    {
        if (local_path[0] == '/' || local_path[0] == '\\')         /*相对本盘符的根目录*/
        {
            local_path.insert(0, pre_cwd, 2);                       /*插入盘符*/
        }
        else if (local_path[0] == '.')                              /*相对于当前路径*/
        {
            local_path.erase(0, 1);
            local_path.insert(0, pre_cwd);

        }
        else
        {
            local_path.insert(0, "\\");
            local_path.insert(0, pre_cwd);
        }
    }

    /*
     转换斜杠为反斜杠
    */
    while (true)
    {
        int pos = local_path.find('/');
        if (pos == string::npos)
            break;
        local_path.replace(pos, 1, "\\");
    }

    /*
      将  "\\dir" 的形式补充完整为 "\\dir\\"
    */
    if (local_path[local_path.size() - 1] != '\\')
    {
        local_path += "\\";
    }
    string full_path = Make_full_directory(local_path);

    /*
    还原工作目录
    */
    chdir(pre_cwd);
    return full_path;
}



/**
 *  创建路径信息，返回全路径字符串
 * @param  path 要创建的路径
 * @return      如果创建失败，是返回空串，这里并不会返回一个标识值来确定失败或者成功
 * 可以考虑进行更改
 */
string Make_full_directory(string path)
{
    string local_path = path;
    int colon_pos = local_path.find(':');
    string drive = local_path.substr(0, colon_pos + 1);

    /**
    盘符不存在
    */
    if (GetDriveType(drive.c_str()) == DRIVE_NO_ROOT_DIR)
    {
        return "\0";
    }
    drive += "\\";

    char pre_cwd[MAX_PATH + 1] = {0};
    if (_getcwd(pre_cwd, MAX_PATH) == NULL)
    {
        return "\0";
    }

    /*首先需要改变工作目录*/
    if (chdir(drive.c_str()) != 0)
    {
        return "\0";
    }

    local_path = local_path.substr(3);
    int apart_pos = -1;
    bool deal_flag = true;
    while ( true)
    {
        apart_pos = local_path.find('\\');
        if (apart_pos == string::npos)
            break;
        string curr_view = local_path.substr(0, apart_pos);
        if (_access(curr_view.c_str(), 0) != 0)
        {
            CreateDirectory(curr_view.c_str(), NULL);
        }
        if (_access(curr_view.c_str(), 0) != 0)
        {
            deal_flag = false;
            break;
        }

        /*
          如果创建成功则更改当前工作目录
        */
        chdir(curr_view.c_str());
        local_path = local_path.substr(apart_pos + 1);
    }
    string full_path = "\0";
    char full_path_chs[MAX_PATH + 1] = { 0 };
    if (!deal_flag)
    {
    }
    else if (_getcwd(full_path_chs, MAX_PATH) == NULL)
    {
    }
    else
    {
        full_path = string(full_path_chs) + "\\";
    }
    if (chdir(pre_cwd) != 0)
    {
        return "\0";
    }

    return full_path;
}

bool Move_file_to ( const Move_file_info& move_info )
{
    if ( move_info.keep_src )
    {
        return Copy_file ( move_info.src, move_info.dest );
    }
    else
    {
        return Move_file ( move_info.src, move_info.dest );
    }
    return false;
}

bool Move_file_to_target_catalog ( Move_file_info& move_info, string dest_catalog )
{
    if ( dest_catalog.empty() )
    {
        return false;
    }

    if ( dest_catalog[dest_catalog.size() - 1] != '\\' )
    {
        dest_catalog += "\\";
    }

    string src_str = string ( move_info.src );
    size_t pos = src_str.find_last_of ( '\\' );
    string file_name;

    if ( pos == string::npos )
    {
        file_name = src_str;
    }
    else
    {
        file_name = src_str.substr ( pos + 1 );
    }

    strncpy ( move_info.dest, ( dest_catalog + file_name ).c_str(), MAX_PATH );

    return Move_file_to ( move_info );
}

std::string fs_SplitBaseName(const std::string& info)
{
    size_t pos = info.find_last_of ( '\\' );
    if ( pos == string::npos )
    {
        pos = 0;
    }
    return info.substr ( pos + 1 );
}

bool fs_pathIsExist(const std::string&path)
{
    if(access(path.c_str(),0)==0)
    {
        return true;
    }
    return false;
}