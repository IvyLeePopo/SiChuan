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


#endif // FS_FUNCS_H
