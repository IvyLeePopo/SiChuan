#ifndef JSON_HELPER_H
#define JSON_HELPER_H
#include "json.h"

bool ReadJsonInt(const Json::Value& node,const std::string& childName,int&val);
bool ReadJsonInt(const Json::Value& node,const std::string& childName,unsigned char&val);
bool ReadJsonInt(const Json::Value& node,const std::string& childName,unsigned short&val);

bool ReadJsonStr(const Json::Value& node,const std::string& childName,std::string&val);
bool ReadJsonChars(const Json::Value& node,const std::string& childName,char*val,size_t val_size);
bool ReadJsonSubNode(const Json::Value& node,const std::string& subNodeName,Json::Value& subNode);



#endif // JSON_HELPER_H
