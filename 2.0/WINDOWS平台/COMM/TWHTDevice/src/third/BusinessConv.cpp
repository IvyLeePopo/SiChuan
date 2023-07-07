/*
* Copyright(C) 2017,特微智能 保留所有权利。( All rights reserved. )
* 文件名称: BusinessConv.cpp
* 摘   要:
* 当前版本:1.0.0.0
* 作   者: 朱亚冰
* 创建时间: 2017-04-18
* 修改记录:
*/
#include "stdafx.h"
#include "BusinessConv.h"
#include "json\json.h"
#include "ErrorHelper.h"
#include "base64.h"
#include "SystemTrace.h"
#include "WorkingPara.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool ReadJsonStr(Json::Value& Node, const std::string& Field, std::string& data)
{
    Json::Value filedV = Node[Field];
    if (filedV.type() != Json::stringValue)
    {
        return false;
    }
    data = filedV.asString();
    return true;
}

bool ReadJsonInt(Json::Value& Node, const std::string& Field, int& data)
{
    Json::Value filedV = Node[Field];
    if (filedV.type() != Json::intValue)
    {
        return false;
    }
    data = filedV.asInt();
    return true;
}



bool FindJsonStrValue(const Json::Value node, const std::string& name, std::map<std::string, std::string>& result, std::string newName = "");
//////////////////////////////////////////////////////////////
// 函数名称:FindJsonIntValue
//
// 功能描述:获取Json节点中指定name的值，并转换为int
//
// 输入参数:const Json::Value node, const std::string& name, std::map<std::string, std::string>& result,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool FindJsonIntValue(const Json::Value node, const std::string& name, std::map<std::string, std::string>& result)
{
    if (Json::intValue == node[name].type())
    {
        result[name] = Int2Str(node[name].asInt());
    }
    else
    {
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:FindJsonStrValue
//
// 功能描述:
//
// 输入参数:const Json::Value node, const std::string& name, std::map<std::string, std::string>& result,std::string newName,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-06-09
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool FindJsonStrValue(const Json::Value node, const std::string& name, std::map<std::string, std::string>& result, std::string newName)
{
    if (newName.empty())
    {
        newName = name;
    }
    if (Json::stringValue == node[name].type())
    {
        result[newName] = node[name].asString();
    }
    else if (Json::intValue == node[name].type())
    {
        result[newName] = Int2Str(node[name].asInt());
    }
    else
    {
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////
// 函数名称:Json2MapData
//
// 功能描述:提取json数据中的某些关键信息，到map中，便捷访问
//
// 输入参数:const std::string& jsonStr, const int& businessType, std::map<std::string, std::string>& result,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool Json2MapData(const std::string& jsonStr, const int& businessType, std::map<std::string, std::string>& result)
{
    result.clear();
    Json::Reader reader;
    Json::Value root;
    std::map<std::string, std::string> unuseInfo;

    bool flag;
    if (!reader.parse(jsonStr.c_str(), root))
    {
        return false;
    }
    Json::Value context;

    context = root["MMIParamContext"];


    if (businessType == BUSINESSCONV_INITPARA)
    {
        context = root["DeviceParamContext"];
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "ProvinceID", result);
        flag = flag ? FindJsonStrValue(vData, "AreaID", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "RoadID", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "StationID", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "LaneID", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "StationName", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "ServerType", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "ServerIP", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "DevicePayMode", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "LaneTime", result) : flag;
        return flag;
    }
    else if (businessType == BUSINESSCONV_STARTPAY)
    {
        context = root["DebitParamContext"];
        Json::Value vData = context["Data"];
        Json::Value vDebit = vData["Debit"];
        Json::Value vEntry = vData["Entry"];
        Json::Value vOperation = vData["Operation"];
        Json::Value vVehicle = vData["Vehicle"];
		Json::Value autoPlateN = vVehicle["AutoPlate"];

        flag = FindJsonStrValue(vData, "OperationMode", result);
        flag = flag ? FindJsonStrValue(vData, "OverTime", result) : flag;
		FindJsonStrValue(vData, "PayModel", result);

        flag = flag ? FindJsonStrValue(vDebit, "Money", result) : flag;
        flag = flag ? FindJsonStrValue(vDebit, "PayCertificateCode", result) : flag;
        flag = flag ? FindJsonStrValue(vDebit, "PayCertificateType", result) : flag;
        flag = flag ? FindJsonStrValue(vDebit, "PayIdentifier", result) : flag;
        flag = flag ? FindJsonStrValue(vDebit, "SubTime", result) : flag;
        flag = flag ? FindJsonStrValue(vDebit, "Type", result) : flag;

        flag = flag ? FindJsonStrValue(vEntry, "AreaID", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "LaneID", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "License", result,"EntryLicense") : flag;
        flag = flag ? FindJsonStrValue(vEntry, "RoadID", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "StationID", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "StationName", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "Time", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "VClass", result) : flag;
        flag = flag ? FindJsonStrValue(vEntry, "VColor", result,"EntryVColor") : flag;
        flag = flag ? FindJsonStrValue(vEntry, "VType", result) : flag;

        flag = flag ? FindJsonStrValue(vOperation, "CardID", result) : flag;
        flag = flag ? FindJsonStrValue(vOperation, "PassCertificateType", result) : flag;
        flag = flag ? FindJsonStrValue(vOperation, "ShiftID", result) : flag;
        flag = flag ? FindJsonStrValue(vOperation, "TollDate", result) : flag;

        /*
        flag = flag ? FindJsonStrValue(vOperation, "VCashierID", result) : flag;
        flag = flag ? FindJsonStrValue(vOperation, "VcashierName", result) : flag;
        */;
        FindJsonStrValue(vOperation, "OperatorID", result) ;
        FindJsonStrValue(vOperation, "OperatorName", result) ;
		flag = flag ? FindJsonStrValue(vOperation, "TickNo", result):false ;


        flag = flag ? FindJsonStrValue(vVehicle, "AxisCount", result) : flag;
        flag = flag ? FindJsonStrValue(vVehicle, "Class", result, "VehClass") : flag;

        flag = flag ? FindJsonStrValue(vVehicle, "License", result) : flag;
        flag = flag ? FindJsonStrValue(vVehicle, "Type", result, "VehType") : flag;
        flag = flag ? FindJsonStrValue(vVehicle, "VLColor", result) : flag;
        flag = flag ? FindJsonStrValue(vVehicle, "Weight", result) : flag;
		flag = flag ? FindJsonStrValue(autoPlateN, "AutoPlate", result) : flag;
		flag = flag ? FindJsonStrValue(autoPlateN, "AutoPlateColor", result) : flag;
        return flag;

    }
    else if (businessType == BUSINESSCONV_CANCLEPAY)
    {
        Json::Value vContext = root["DebitCancelParamContext"];
        Json::Value vData = vContext["Data"];
        Json::Value vKeyItem = vData["TradeKeyItem"];
        flag = FindJsonStrValue(vKeyItem, "PayIdentifier", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_LASTERROR)
    {

    }
    else if (businessType == BUSINESSCONV_RESPONSE_201)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_202)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_203)
    {

        Json::Value vData = context["Data"];//只用来校验格式
        flag = FindJsonStrValue(vData, "Money", result);
        flag = flag ? FindJsonStrValue(vData, "VType", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "VClass", result) : flag;
        // flag = flag ? FindJsonStrValue(vData, "VColor", result) : flag;
        //flag = flag ? FindJsonStrValue(vData, "License", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "Balance", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "StationName", result) : flag;
        //flag = flag ? FindJsonStrValue(vData, "CroseNum", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "AxisCount", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "Weight", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "LimitWeight", result) : flag;
        //flag = flag ? FindJsonStrValue(vData, "Program", result) : flag;
        //flag = flag ? FindJsonStrValue(vData, "Remark1", result) : flag;
        //flag = flag ? FindJsonStrValue(vData, "Remark2", result) : flag;
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_204)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", unuseInfo);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_205)
    {
        Json::Value vData = context["Data"];
        flag = false;
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_206)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_207)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "IP", result);
        flag = flag ? FindJsonStrValue(vData, "Mask", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "Gateway", result) : flag;
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_208)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "IP", result);
        flag = flag ? FindJsonStrValue(vData, "Mask", result) : flag;
        flag = flag ? FindJsonStrValue(vData, "Gateway", result) : flag;
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_301)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Batch", result);
        flag = flag ? FindJsonStrValue(vData, "BurnTime", result) : flag;
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_302)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_303)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_304)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_305)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "Value", result);
        return flag;
    }
    else if (businessType == BUSINESSCONV_RESPONSE_306)
    {
        Json::Value vData = context["Data"];
        flag = FindJsonStrValue(vData, "PortNo", result);
        flag = flag ? FindJsonStrValue(vData, "TransData", result) : flag;
        return flag;
    }

    return false;
}
//////////////////////////////////////////////////////////////
// 函数名称:JsonGetTargetValue
//
// 功能描述:从json中提取指定位置的内容，nameList中保存了名字路径
//
// 输入参数:const std::string& jsonStr, const std::vector<std::string> nameList, std::string&val,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool JsonGetTargetValue(const std::string& jsonStr, const std::vector<std::string> nameList, std::string&val)
{
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(jsonStr.c_str(), root))
    {
        return false;
    }
    Json::Value findValue = root;
    for (size_t i = 0; i < nameList.size(); ++i)
    {
        if (findValue[nameList[i]].type() == Json::nullValue)
        {
            return false;
        }
        findValue = findValue[nameList[i]];
    }
    if (Json::stringValue == findValue.type())
    {
        val = findValue.asString();
    }
    else if (Json::intValue == findValue.type())
    {
        val = Int2Str(findValue.asInt());
    }
    return true;
}

int ConvResponseType(int respType)
{
    switch (respType)
    {
    case 101:
        return BUSINESSCONV_RESPONSE_101;
    case 102:
        return BUSINESSCONV_RESPONSE_102;
    case 103:
        return BUSINESSCONV_RESPONSE_103;
    case 104:
        return BUSINESSCONV_RESPONSE_104;
    case 105:
        return BUSINESSCONV_RESPONSE_105;
    case 106:
        return BUSINESSCONV_RESPONSE_106;
    case 107:
        return BUSINESSCONV_RESPONSE_107;
    case 108:
        return BUSINESSCONV_RESPONSE_108;
    case 109:
        return BUSINESSCONV_RESPONSE_109;
    case 110:
        return BUSINESSCONV_RESPONSE_110;
    case 111:
        return BUSINESSCONV_RESPONSE_111;
    case 112:
        return BUSINESSCONV_RESPONSE_112;
    case 201:
        return BUSINESSCONV_RESPONSE_201;
    case 202:
        return BUSINESSCONV_RESPONSE_202;
    case 203:
        return BUSINESSCONV_RESPONSE_203;
    case 204:
        return BUSINESSCONV_RESPONSE_204;
    case 205:
        return BUSINESSCONV_RESPONSE_205;
    case 206:
        return BUSINESSCONV_RESPONSE_206;
    case 207:
        return BUSINESSCONV_RESPONSE_207;
    case 208:
        return BUSINESSCONV_RESPONSE_208;
    case 301:
        return BUSINESSCONV_RESPONSE_301;
    case 302:
        return BUSINESSCONV_RESPONSE_302;
    case 303:
        return BUSINESSCONV_RESPONSE_303;
    case 304:
        return BUSINESSCONV_RESPONSE_304;
    case 305:
        return BUSINESSCONV_RESPONSE_305;
    case 306:
        return BUSINESSCONV_RESPONSE_306;
    case 5:
        return BUSINESSCONV_LASTERROR;
    default:
        return -1;
    }

}

//////////////////////////////////////////////////////////////
// 函数名称:MakeDebitCancleJsonStr
//
// 功能描述:更改撤单json中的Type字段，表示是否需要回馈
//
// 输入参数:const std::string& payID, std::string& paraStr,int needBack,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool MakeDebitCancleJsonStr(const std::string& payID, std::string& paraStr, int needBack)
{
    Json::FastWriter writer;
    Json::Value root;
    Json::Value data;
    Json::Value context;
    Json::Value keyItem;
    //data["PayIdentifier"] = Json::Value(payID);

    keyItem["PayIdentifier"] = Json::Value(payID);
    keyItem["Type"] = Json::Value(needBack);
    data["TradeKeyItem"] = keyItem;

    context["Data"] = data;
    context["Version"] = Json::Value("1.0");
    root["DebitCancelParamContext"] = context;
    paraStr = writer.write(root);
    return true;

}

bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_GreenChannel>& src, const TWCmdKey& whichKey, std::string&result)
{
	std::map<TWCmdKey, TWProtocolData_B_GreenChannel>::const_iterator it = src.find(whichKey);
    if (it == src.end())
    {
        return false;
    }


    if (it->second.SUBCMD == TWPC_B_GREENCHANNEL_NFC)
    {
		//MessageBox(0,"abc","",0);
		result = std::string(it->second.data.NFCParaInfo);
		return true;
		/*
		//std::copy(it->second.data.NFCParaInfo,it->second.data.NFCParaInfo+strlen(it->second.data.NFCParaInfo)-1,std::back_inserter(result));
		int len = strlen(it->second.data.NFCParaInfo);
		//MessageBox(0,Int2Str(len).c_str(),"",0);
		for(int i =0;i< len;++i)
		{
			//MessageBox(0,Int2Str(i).c_str(),"",0);
			result+=it->second.data.NFCParaInfo[i];
		}
		*/
        return true;
    }
   

    return false;
}

bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_Beat>& src, const TWCmdKey& whichKey, std::string&result)
{
    return false;
}
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_Interactive>& src, const TWCmdKey& whichKey, std::string&result)
{
    return false;
}
//////////////////////////////////////////////////////////////
// 函数名称:MakeReturnJsonString
//
// 功能描述:此处生成结果字符串，此结果串是回馈给业务动态库的，回馈的格式由业务动态库和通讯动态库的接口决定
//
// 输入参数:const std::map<TWCmdKey, TWProtocolData_B_StartPay>& src, const TWCmdKey& whichKey, std::string&result,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_StartPay>& src, const TWCmdKey& whichKey, std::string&result)
{
    std::map<TWCmdKey, TWProtocolData_B_StartPay>::const_iterator it = src.find(whichKey);
    if (it == src.end())
    {
        return false;
    }


    if (it->second.SUBCMD == TWPC_B_STARTPAY_PAYJSON)
    {
        result = it->second.data.pDataJson;
        return true;
    }
    else if (it->second.SUBCMD == TWPC_B_STARTPAY_PAYBLOCK)
    {
        if (it->second.data.DataBlock.Result != OP_SUCCESS)
        {
            TW_SET_ERROR(it->second.data.DataBlock.Result);
            return false;
        }
        Json::FastWriter jWriter;
        Json::Value jRoot;
        Json::Value jContext;
        Json::Value jData;
        Json::Value jTradeItem;
        char tmpChs[100] = {0};

        jTradeItem["ConsumeTime"] = Json::Value(Int2Str(it->second.data.DataBlock.ConsumeTime));
        strncpy(tmpChs, it->second.data.DataBlock.OrderNO, 32);
        jTradeItem["DebitOrder"] = Json::Value(tmpChs);
        jTradeItem["DebitTime"] = Json::Value(Datetime2Str(it->second.data.DataBlock.TransTime));
        strncpy(tmpChs, it->second.data.DataBlock.TransactionIDCode, 40);
        jTradeItem["PayIdentifier"] = Json::Value(tmpChs);
        jTradeItem["PayPlatformType"] = Json::Value(Int2Str(it->second.data.DataBlock.PayType));
        strncpy(tmpChs, it->second.data.DataBlock.TransSecurityCode, 40);
        jTradeItem["TradeSecurityCode"] = Json::Value(tmpChs);

        jData["TradeKeyItem"] = jTradeItem;
        jData["ResultType"] = Json::Value(0);
        jData["ExecuteDesc"] = Json::Value("");

        jContext["Data"] = jData;

        jRoot["DebitResultContext"] = jContext;
        result = jWriter.write(jRoot);
        return true;

    }

    return false;
}
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_CANCLE>& src, const TWCmdKey& whichKey, std::string&result)
{
    return false;
}
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_SendData>& src, const TWCmdKey& whichKey, std::string&result)
{
    return false;
}

//////////////////////////////////////////////////////////////
// 函数名称:MakeReturnJsonString
//
// 功能描述:同属于MakeReturnJsonString系列函数
//
// 输入参数:const std::map<TWCmdKey, TWProtocolData_B_HardwareConfig>& src, const TWCmdKey& whichKey, std::string&result,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_HardwareConfig>& src, const TWCmdKey& whichKey, std::string&result)
{

    std::map<TWCmdKey, TWProtocolData_B_HardwareConfig>::const_iterator it = src.find(whichKey);
    if (it == src.end())
    {
        return false;
    }


    Json::FastWriter jWriter;
    Json::Value jRoot;
    Json::Value jContext;
    Json::Value jData;
    //目前根据A指令中的内容确定返回的内容组成，而不是B指令
    if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_CANID)
    {
        jData["Value"] = Json::Value(Int2Str(it->second.data.CANID.Value));
        jData["Result"] = Json::Value(Int2Str(it->second.data.CANID.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_MAX)
    {
        char tmpBatch[30] = {0};
        char tmpTime[30] = {0};
        strncpy(tmpBatch, it->second.data.MAX.ProductInfo.Batch, 8);
        strncpy(tmpTime, it->second.data.MAX.ProductInfo.BurnTime, 14);
        jData["Batch"] = Json::Value(tmpBatch);
        jData["BurnTime"] = Json::Value(tmpTime);
        jData["Result"] = Json::Value(Int2Str(it->second.data.MAX.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_FIRMWARE)
    {
        char tmpStr[100] = {0};
        strcpy(tmpStr, it->second.data.FireWare.pValue);

        jData["Value"] = Json::Value(tmpStr);
        jData["Result"] = Json::Value(Int2Str(it->second.data.FireWare.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_SCANTIME)
    {
        jData["Value"] = Json::Value(Int2Str(it->second.data.ScanTime.Value));
        jData["Result"] = Json::Value(Int2Str(it->second.data.ScanTime.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ID)
    {
        char tmpStr[30] = {0};
        strncpy(tmpStr, it->second.data.DeviceID.Value, 20);
        jData["Value"] = Json::Value(tmpStr);
        jData["Result"] = Json::Value(Int2Str(it->second.data.DeviceID.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_BOOTSCREEN )
    {
        jData["Value"] = Json::Value(Int2Str(it->second.data.BootScreenID.Value));
        jData["Result"] = Json::Value(Int2Str(it->second.data.BootScreenID.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_HARDWARETEMP)
    {
        jData["Value"] = Json::Value(it->second.data.HardTemp.pValue);
        jData["Result"] = Json::Value(Int2Str(it->second.data.HardTemp.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_HARDWARETIME)
    {
        jData["Value"] = Json::Value(it->second.data.TerminalTime.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.TerminalTime.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ETH0IP)
    {
        jData["IP"] = Json::Value(it->second.data.IPInfo.IP.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.IPInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ETH0MASK)
    {
        jData["Mask"] = Json::Value(it->second.data.IPInfo.IP.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.IPInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ETH0GATEWAY)
    {
        jData["Gateway"] = Json::Value(it->second.data.IPInfo.IP.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.IPInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ETH1IP)
    {
        jData["IP"] = Json::Value(it->second.data.IPInfo.IP.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.IPInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ETH1MASK)
    {
        jData["Mask"] = Json::Value(it->second.data.IPInfo.IP.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.IPInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_ETH1GATEWAY)
    {
        jData["Gateway"] = Json::Value(it->second.data.IPInfo.IP.Value);
        jData["Result"] = Json::Value(Int2Str(it->second.data.IPInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }
    else if (it->second.SUBCMD == TWPC_A_HARDWARECONFIG_HARDVERSION)
    {
        jData["Value"] = Json::Value(it->second.data.VersionInfo.Val);
        jData["Result"] = Json::Value(Int2Str(it->second.data.VersionInfo.Result));
        jContext["Data"] = jData;
        jRoot["MMIParamContext"] = jContext;
        jRoot["Version"] = Json::Value("1.0"); //Json:Value("1.0");
        result = jWriter.write(jRoot);
        return true;
    }


    return false;
}
bool MakeReturnJsonString(const std::map<TWCmdKey, TWProtocolData_B_Auth>& src, const TWCmdKey& whichKey, std::string&result)
{
    return false;
}
//////////////////////////////////////////////////////////////
// 函数名称:MergeIPJsonString
//
// 功能描述:将IP 地址，网关，掩码等信息合并到一个
//
// 输入参数:const std::string& jsonStr1, const std::string jsonStr2, const std::string& jsonStr3, std::string& result,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool MergeIPJsonString(const std::string& jsonStr1, const std::string jsonStr2, const std::string& jsonStr3, std::string& result)
{
    Json::Reader r1, r2, r3;
    Json::Value v1, v2, v3;
    Json::Value vRoot, vData, vContext;
    Json::FastWriter writer;
    if (!r1.parse(jsonStr1.c_str(), v1)
            || !r2.parse(jsonStr2.c_str(), v2)
            || !r3.parse(jsonStr3.c_str(), v3))
    {
        return false;
    }
    v1 = v1["MMIParamContext"];
    v1 = v1["Data"];
    v2 = v2["MMIParamContext"];
    v2 = v2["Data"];
    v3 = v3["MMIParamContext"];
    v3 = v3["Data"];

    vData.append(v1);
    vData.append(v2);
    vData.append(v3);
    vContext["Data"] = vData;
    vRoot["MMIParamContext"] = vContext;

    result = writer.write(vRoot);
    return true;




}

//////////////////////////////////////////////////////////////
// 函数名称:ModifyInitStrTime
//
// 功能描述:修改初始化字符串中的时间字段--CurrentTime，在重发该参数的时候需要用到
//
// 输入参数:std::string initStr,
// 输出参数:std::string
// 编写人员:朱亚冰
// 编写时间:2017-07-25
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
std::string ModifyInitStrTime(std::string initStr)
{
    std::string result;
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(initStr.c_str(), root))
    {
        return result ;
    }
    Json::Value jContext = root["DeviceParamContext"];
    Json::Value jData = jContext["Data"];
    jData["CurrentTime"] = Json::Value(Datetime2Str(time(NULL)));

    Json::FastWriter jWriter;
    jContext["Data"] = jData;
    root["DeviceParamContext"] = jContext;
    result = jWriter.write(root);
    return result;
}



bool ParseSoundPara(const std::string& soundJson, std::string& msg)
{
    Json::Reader  jsonReader;
    Json::Value   rootN;
    if (!jsonReader.parse(soundJson.c_str(), rootN))
    {
        return false;
    }
    Json::Value voice = rootN["VoicePara"];
    if (voice == Json::nullValue)
    {
        return false;
    }
    if (voice["PlayCode"] != Json::intValue)
    {
        return false;
    }
    int code = voice["PlayCode"].asInt();
    if (code != 1)
    {
        return false;
    }
    if (voice["Content"] != Json::stringValue)
    {
        return false;
    }
    msg = voice["Content"].asString();
    return true;

}

bool ParseFreeTextPara(const std::string& paraStr, TWProtocolData_A_Interactive& displayInfo)
{
    Json::Reader  jsonReader;
    Json::Value   rootN;
    if (!jsonReader.parse(paraStr.c_str(), rootN))
    {
        return false;
    }
    Json::Value context = rootN["LedDisplayContext"];
    if (context == Json::nullValue)
    {
        return false;
    }
    Json::Value rowData = context["Row1Display"];
    if (rowData != Json::nullValue)
    {
        std::string rowText = (rowData["Text"] == Json::stringValue) ? rowData["Text"].asString() : "";
        int isScroll = (rowData["Scroll"] == Json::intValue) ? rowData["Scroll"].asInt() : 0;
        strncpy(displayInfo.data.FreeText.Row1Data, rowText.c_str(), 8);
        displayInfo.data.FreeText.Row1Scroll = isScroll;
    }

    rowData = context["Row2Display"];
    if (rowData != Json::nullValue)
    {
        std::string rowText = (rowData["Text"] == Json::stringValue) ? rowData["Text"].asString() : "";
        int isScroll = (rowData["Scroll"] == Json::intValue) ? rowData["Scroll"].asInt() : 0;
        strncpy(displayInfo.data.FreeText.Row2Data, rowText.c_str(), 8);
        displayInfo.data.FreeText.Row2Scroll = isScroll;
    }

    rowData = context["Row3Display"];
    if (rowData != Json::nullValue)
    {
        std::string rowText = (rowData["Text"] == Json::stringValue) ? rowData["Text"].asString() : "";
        int isScroll = (rowData["Scroll"] == Json::intValue) ? rowData["Scroll"].asInt() : 0;
        strncpy(displayInfo.data.FreeText.Row3Data, rowText.c_str(), 8);
        displayInfo.data.FreeText.Row3Scroll = isScroll;
    }

    rowData = context["Row4Display"];
    if (rowData != Json::nullValue)
    {
        std::string rowText = (rowData["Text"] == Json::stringValue) ? rowData["Text"].asString() : "";
        int isScroll = (rowData["Scroll"] == Json::intValue) ? rowData["Scroll"].asInt() : 0;
        strncpy(displayInfo.data.FreeText.Row4Data, rowText.c_str(), 8);
        displayInfo.data.FreeText.Row4Scroll = isScroll;
    }

    return true;
}


bool Json2BusinessData(const std::string& jsonStr, TWProtocolData_A_AgencyDebit& data)
{
    static int Max_image_size = SystemParaImageMaxSize();
    Json::Reader  jsonReader;
    Json::Value   rootN;
    if (!jsonReader.parse(jsonStr.c_str(), rootN))
    {
        return false;
    }
    if (data.SUBCMD == TWPC_A_AGENCYDEBIT_QUERY)
    {
        Json::Value contextN = rootN["DebitQueryParamContext"];
        if (contextN == Json::nullValue)
        {
            return false;
        }
        Json::Value dataN = contextN["Data"];
        if (dataN == Json::nullValue)
        {
            return false;
        }
        Json::Value vehicleN = dataN["Vehicle"];
        if (vehicleN == Json::nullValue)
        {
            return false;
        }
        bool readOk = true;
        std::string License;
        int AxisCount = 0;
        int Class = 0;
        int LimitWeight = 0;
        int Type = 0;
        int VLColor = 0;
        int Weight = 0;
        readOk = ReadJsonStr(vehicleN, "License", License);
        readOk = readOk ? ReadJsonInt(vehicleN, "AxisCount", AxisCount) : false;
        readOk = readOk ? ReadJsonInt(vehicleN, "Class", Class) : false;
        readOk = readOk ? ReadJsonInt(vehicleN, "LimitWeight", LimitWeight) : false;
        readOk = readOk ? ReadJsonInt(vehicleN, "Type", Type) : false;
        readOk = readOk ? ReadJsonInt(vehicleN, "VLColor", VLColor) : false;
        readOk = readOk ? ReadJsonInt(vehicleN, "Weight", Weight) : false;
        if (!readOk)
        {
            return false;
        }
        strncpy(data.data.QueryData.VehPlate, License.c_str(), sizeof(data.data.QueryData.VehPlate));
        data.data.QueryData.VehType = Class;
        data.data.QueryData.VehKind = Type;
        data.data.QueryData.VehColor = VLColor ;
        data.data.QueryData.AxleCount = AxisCount;
        data.data.QueryData.Weight = Weight;
        data.data.QueryData.WeightLimit = LimitWeight;
    }
    else if (data.SUBCMD == TWPC_A_AGENCYDEBIT_TRANSFERIMAGE)
    {
        Json::Value debitImageN = rootN["DebitImage"];
        if (debitImageN == Json::nullValue)
        {
            TW_TRACE_DIARY("debitimage not found");
            return false;
        }
        Json::Value dataN = debitImageN["Data"];
        if (dataN == Json::nullValue)
        {
            TW_TRACE_DIARY("data not found");
            return false;
        }
        int imageType = 1;
        int imageClass;
        int dataFormat;
        std::string imageFormat, payID, imageContent;

        bool readOk = true;
        readOk = ReadJsonStr(dataN, "Image", imageContent);
        readOk = readOk ? ReadJsonInt(dataN, "Type", imageType) : false;
        readOk = readOk ? ReadJsonStr(dataN, "PayIdentifier", payID) : false;
        readOk = readOk ? ReadJsonInt(dataN, "DataFormat", dataFormat) : false;
        if (!readOk)
        {
            TW_TRACE_DIARY("one or many not found");
            return false;
        }


        if (dataFormat == 1)
        {
            std::string imageData = imageContent;
            //hex
            if ( (imageData.size() / 2) > Max_image_size)
            {
                TW_TRACE_DIARY("image too long");
                return false;//image too long
            }
            if (!Hex2Bin(imageData.c_str(), (char*)data.data.ImageData.ImageContent, imageData.size()))
            {
                TW_TRACE_DIARY("hex conv error");
                return false;
            }
            data.data.ImageData.ImageLen = imageData.size() / 2;
            imageFormat = "bmp";
        }
        else if (dataFormat == 2)
        {
            std::string base64_head = "data:image/";
            size_t image_pos = imageContent.find(base64_head);
            size_t base64_pos = imageContent.find("base64,", image_pos);
            if (image_pos == std::string::npos
                    || base64_pos == std::string::npos)
            {
                TW_TRACE_DIARY("image or base64 str not found");
                return false;
            }
            std::string imageData = imageContent.substr(base64_pos + 7);

            //base64
            if ( (imageData.size() * 2 / 3) > Max_image_size )
            {
                TW_TRACE_DIARY("image too long");
                return false;//image too long
            }

            imageFormat = imageContent.substr(image_pos + base64_head.size(), base64_pos - image_pos - base64_head.size() - 1);
            data.data.ImageData.ImageLen = Base64_Decode((char*)data.data.ImageData.ImageContent, imageData.c_str(), imageData.size());
        }
        else
        {
            return false;
        }
        data.data.ImageData.ImageClass = imageClass;
        strncpy(data.data.ImageData.PayIdentifier, payID.c_str(), sizeof(data.data.ImageData.PayIdentifier));
        strncpy(data.data.ImageData.ImageFormat, imageFormat.c_str(), sizeof(data.data.ImageData.ImageFormat));
    }
    else
    {
        return false;
    }

    return true;
}


bool ParseImageFile(const std::string& paraStr, std::vector<unsigned char>& byteArray, std::string& fileProp)
{
    fileProp="file property unsupperted";
    Json::Reader  jsonReader;
    Json::Value   rootN;
    if (!jsonReader.parse(paraStr.c_str(), rootN))
    {
        return false;
    }

    Json::Value debitImageN = rootN["DebitImage"];
    if (debitImageN == Json::nullValue)
    {
        TW_TRACE_DIARY("debitimage not found");
        return false;
    }
    Json::Value dataN = debitImageN["Data"];
    if (dataN == Json::nullValue)
    {
        TW_TRACE_DIARY("data not found");
        return false;
    }
    int imageType = 1;
    int imageClass;
    int dataFormat;
    std::string imageFormat, payID, imageContent;

    bool readOk = true;
    readOk = ReadJsonStr(dataN, "Image", imageContent);
    readOk = readOk ? ReadJsonInt(dataN, "Type", imageType) : false;
    readOk = readOk ? ReadJsonStr(dataN, "PayIdentifier", payID) : false;
    readOk = readOk ? ReadJsonInt(dataN, "DataFormat", dataFormat) : false;
    if (!readOk)
    {
        TW_TRACE_DIARY("one or many not found");
        return false;
    }
    if(imageContent.size() > 100*1024 )//宽泛的要求，不能大于100k
    {
        TW_TRACE_DIARY("Large Image Unsupported");
        return false;
    }
    int imageLen =0 ;
    std::list<unsigned char> imageBuf;//足够存储图像
    // memset(imageBuf,0,100*1024*3);

    if (dataFormat == 1)
    {
        std::string imageData = imageContent;
        //hex
        if (!Hex2Bin(imageData.c_str(), imageBuf, imageData.size()))
        {
            TW_TRACE_DIARY("hex conv error");
            return false;
        }
        imageLen = imageData.size() / 2;
        imageFormat = "bmp";
    }
    else if (dataFormat == 2)
    {
        std::string base64_head = "data:image/";
        size_t image_pos = imageContent.find(base64_head);
        size_t base64_pos = imageContent.find("base64,", image_pos);
        if (image_pos == std::string::npos
                || base64_pos == std::string::npos)
        {
            TW_TRACE_DIARY("image or base64 str not found");
            return false;
        }
        std::string imageData = imageContent.substr(base64_pos + 7);

        imageFormat = imageContent.substr(image_pos + base64_head.size(), base64_pos - image_pos - base64_head.size() - 1);

		Base64_Decode(imageBuf, imageData);
		imageLen =imageBuf.size();
    }
    else
    {
        return false;
    }
    
    std::copy(imageBuf.begin(),imageBuf.end(),std::back_inserter(byteArray));

	Json::Value prop_root;
	prop_root["PayIdentifier"]=Json::Value(payID);
	prop_root["fileFormat"]=Json::Value("PNG");

	Json::FastWriter jfw;

	fileProp = jfw.write(prop_root);

    return true;
}