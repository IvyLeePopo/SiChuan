/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: fs_funcs.cpp
* 摘   要:  文件操作相关的函数集
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-21
* 修改记录: 
*/
#include "stdafx.h"
#include "fs_funcs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
    char tmpChs[300]={0};
    GetModuleFileName(NULL,tmpChs,250);
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
    return path.substr(0,pos);
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
std::string fs_PathJoin(const std::string& path1,const std::string& path2 )
{
    if(path1.empty())
    {
        return path2;
    }
    else if(path2.empty())
    {
        return path1;
    }

    bool addSepFlag = true;
    if(path1[path1.size()-1]=='\\' || path2[path2.size()-1]=='\\')
    {
        addSepFlag = false;
    }
    return path1+ (addSepFlag?"\\":"")+path2;
}

