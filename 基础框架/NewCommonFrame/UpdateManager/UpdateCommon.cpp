#include "UpdateCommon.h"

#include "UpdateManagerGlobal.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include<dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <QDebug>
QList<QString> UpdateCommon::dllNameList;

UpdateCommon::UpdateCommon()
{
}


std::string UpdateCommon::datetime2Str(time_t dt)
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

void UpdateCommon::readFileName()
{
    qDebug()<<"[#UpdateCommon::readFileName#]d_name :\n"; //输出文件名

    QString szPath=DLL_PATH;
    DIR * dir = NULL;
    struct dirent * ptr;
    int i=0;
    dir = opendir(szPath.toLocal8Bit().data()); //打开一个目录
    if(dir==NULL)
    {
        printf("[#UpdateCommon::readFileName#]dir is NULL.\n");
        return;
    }
    while((ptr = readdir(dir)) != NULL) //循环读取目录数据
    {
        if(strcmp( ptr->d_name,".")==0)  //文件 "." 固定的格式，但是不需要
            continue;
        if(strcmp( ptr->d_name,"..")==0)//文件 ".." 固定的格式，但是不需要
            continue;
        if(strstr(ptr->d_name,".so")==NULL)
            continue;
        if(strstr(ptr->d_name,".so.")!=NULL)
            continue;
        printf("[#UpdateCommon::readFileName#]d_name : %s\n", ptr->d_name); //输出文件名
        dllNameList.push_back( ptr->d_name);
        printf("[#UpdateCommon::readFileName#]d_name : %s\n", ptr->d_name); //输出文件名
    }
    closedir(dir);//关闭目录指针
}
