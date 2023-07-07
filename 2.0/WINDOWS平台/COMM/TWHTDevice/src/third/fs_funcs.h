/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: fs_funcs.h
* ժ   Ҫ:  
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-21
* �޸ļ�¼: 
*/

#ifndef FS_FUNCS_H
#define FS_FUNCS_H
#include <string>
std::string fs_GetModulePath();
std::string fs_SplitBasePath(const std::string& path);
std::string fs_PathJoin(const std::string& path1,const std::string& path2 );


#endif // FS_FUNCS_H
