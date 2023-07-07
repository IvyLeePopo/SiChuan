/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: BusinessConv.h
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-18
* 修改记录:
*/
#ifndef BUSINESS_CONV_H
#define BUSINESS_CONV_H
#include <string>
#include <map>
#include <vector>
#include "TWProtocolData.h"
#include "SerialPort.h"
#include "CommondDak.h"

#define BUSINESSCONV_RESPONSE_101              1001
#define BUSINESSCONV_RESPONSE_102              1002
#define BUSINESSCONV_RESPONSE_103              1003
#define BUSINESSCONV_RESPONSE_104              1004
#define BUSINESSCONV_RESPONSE_105              1005
#define BUSINESSCONV_RESPONSE_106              1006
#define BUSINESSCONV_RESPONSE_107              1007
#define BUSINESSCONV_RESPONSE_108              1008
#define BUSINESSCONV_RESPONSE_109              1009
#define BUSINESSCONV_RESPONSE_110              1010
#define BUSINESSCONV_RESPONSE_111              1011
#define BUSINESSCONV_RESPONSE_112              1012
#define BUSINESSCONV_RESPONSE_201              1013
#define BUSINESSCONV_RESPONSE_202              1014
#define BUSINESSCONV_RESPONSE_203              1015
#define BUSINESSCONV_RESPONSE_204              1016
#define BUSINESSCONV_RESPONSE_205              1017
#define BUSINESSCONV_RESPONSE_206              1018
#define BUSINESSCONV_RESPONSE_207              1019
#define BUSINESSCONV_RESPONSE_208              1020
#define BUSINESSCONV_RESPONSE_301              1021
#define BUSINESSCONV_RESPONSE_302              1022
#define BUSINESSCONV_RESPONSE_303              1023
#define BUSINESSCONV_RESPONSE_304              1024
#define BUSINESSCONV_RESPONSE_305              1025
#define BUSINESSCONV_RESPONSE_306              1026
#define BUSINESSCONV_INITPARA                  1027
#define BUSINESSCONV_STARTPAY                  1028
#define BUSINESSCONV_CANCLEPAY                 1029
#define BUSINESSCONV_LASTERROR                 1030



bool Json2BusinessData(const std::string& jsonStr,TWProtocolData_A_AgencyDebit& data);


bool Json2MapData(const std::string& jsonStr, const int& businessType, std::map<std::string, std::string>& result);
bool JsonGetTargetValue(const std::string& jsonStr, const std::vector<std::string> nameList, std::string&val);

int ConvResponseType(int respType);
bool MakeDebitCancleJsonStr(const std::string& payID, std::string& paraStr,int needBack=TWPD_A_CANCLE_FEEDBACK_YES);

//此处直接根据发送的A指令，查找B指令中的内容，因为目前的SN是对应起来的
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_Beat>& src, const TWCmdKey& relativeSend, std::string&result);
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_Interactive>& src, const TWCmdKey& relativeSend, std::string&result);
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_StartPay>& src, const TWCmdKey& relativeSend, std::string&result);
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_CANCLE>& src, const TWCmdKey& relativeSend, std::string&result);
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_SendData>& src, const TWCmdKey& relativeSend, std::string&result);
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_HardwareConfig>& src, const TWCmdKey& relativeSend, std::string&result);
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_Auth>& src, const TWCmdKey& relativeSend, std::string&result);

bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_GreenChannel>& src, const TWCmdKey& relativeSend, std::string&result);

bool MergeIPJsonString(const std::string& jsonStr1, const std::string jsonStrr2, const std::string& jsonStr3, std::string&result);


std::string ModifyInitStrTime(std::string initStr);

bool ParseSoundPara(const std::string& soundJson, std::string& msg);
bool ParseFreeTextPara(const std::string& paraStr,TWProtocolData_A_Interactive& displayInfo);

bool ParseImageFile(const std::string& paraStr,std::vector<unsigned char>& byteArray,std::string& fileProp);

#endif // BUSINESS_CONV_H
