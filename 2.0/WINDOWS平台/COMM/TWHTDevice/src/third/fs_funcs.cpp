/*
* Copyright(C) 2017,��΢���� ��������Ȩ����( All rights reserved. )
* �ļ�����: fs_funcs.cpp
* ժ   Ҫ:  �ļ�������صĺ�����
* ��ǰ�汾:1.0.0.0
* ��   ��: ���Ǳ�
* ����ʱ��: 2017-04-21
* �޸ļ�¼: 
*/
#include "stdafx.h"
#include "fs_funcs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////
// ��������:fs_GetModulePath
//
// ��������:��ȡ����·��
//
// �������:
// �������:std::string 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
std::string fs_GetModulePath()
{
    char tmpChs[300]={0};
    GetModuleFileName(NULL,tmpChs,250);
    return std::string(tmpChs);
}
//////////////////////////////////////////////////////////////
// ��������:fs_SplitBasePath
//
// ��������:��ȡ���ļ���
//
// �������:const std::string& path,
// �������:std::string 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
//
//////////////////////////////////////////////////////////////
std::string fs_SplitBasePath(const std::string& path)
{
    size_t pos = path.find_last_of('\\');
    return path.substr(0,pos);
}
//////////////////////////////////////////////////////////////
// ��������:fs_PathJoin
//
// ��������:�ϲ�����·����Ϣ����Ҫ����ƴ��
//
// �������:const std::string& path1,const std::string& path2,
// �������:std::string 
// ��д��Ա:���Ǳ�
// ��дʱ��:2017-07-25
// �޸���Ա:
// �޸�ʱ��:
// �����汾:1.0.0.0
// ��ע˵��:
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

