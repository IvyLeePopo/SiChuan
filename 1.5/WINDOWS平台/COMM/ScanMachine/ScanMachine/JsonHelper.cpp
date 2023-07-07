/*
* @Author: ÖìÑÇ±ù
* @Date:   2017-12-07
* @Last Modified by:   windf
* @Last Modified time: 2017-12-07
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
bool ReadJsonString(const Json::Value& node,const std::string& childName,std::string&val)
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