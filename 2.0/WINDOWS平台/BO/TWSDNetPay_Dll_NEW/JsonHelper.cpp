/*
* @Author: ÖìÑÇ±ù
* @Date:   2017-12-07
* @Last Modified by:   windf
* @Last Modified time: 2018-05-07
*/
#include "JsonHelper.h"

bool ReadJsonInt(const Json::Value& node,const std::string& childName,int&val)
{
    if(childName.empty())
    {
        return false;
    }
    if (Json::intValue != node[childName].type())
    {
        return false;
    }
    val = node[childName].asInt();

    return true;
}
bool ReadJsonStr(const Json::Value& node,const std::string& childName,std::string&val)
{
     if(childName.empty())
    {
        return false;
    }
    if (Json::stringValue != node[childName].type())
    {
        return false;
    }
    val = node[childName].asString();

    return true;
}

bool ReadJsonSubNode(const Json::Value& node,const std::string& subNodeName,Json::Value& subNode)
{
    if(subNodeName.empty())
    {
        return false;
    }
    if(!node.isMember(subNodeName))
    {
        return false;
    }
    subNode = node[subNodeName];
    return true;
}

bool ReadJsonInt(const Json::Value& node,const std::string& childName,unsigned char&val)
{
    int intVal;
    if(!ReadJsonInt(node,childName,intVal))
    {
        return false;
    }
    val = intVal;
    return true;
}
bool ReadJsonInt(const Json::Value& node,const std::string& childName,unsigned short&val)
{
    int intVal;
    if(!ReadJsonInt(node,childName,intVal))
    {
        return false;
    }
    val = intVal;
    return true;
}

bool ReadJsonChars(const Json::Value& node,const std::string& childName,char*val,size_t val_size)
{
    if(val_size<2)
    {
        return false;
    }
    std::string strVal;
    if(!ReadJsonStr(node,childName,strVal))
    {
        return false;
    }
    strncpy(val,strVal.c_str(),val_size-1);
    return true;
}