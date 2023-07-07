#ifndef JSON_HELPER_H
#define JSON_HELPER_H
#include "json/json.h"

bool ReadJsonInt(const Json::Value& node,const std::string& childName,int&val);
bool ReadJsonString(const Json::Value& node,const std::string& childName,std::string&val);



#endif // JSON_HELPER_H
