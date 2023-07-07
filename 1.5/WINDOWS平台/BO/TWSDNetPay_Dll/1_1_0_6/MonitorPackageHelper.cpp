/*
* @Author: 朱亚冰
* @Date:   2017-11-02
* @Last Modified by:   windf
* @Last Modified time: 2017-11-23
*/
#include "MonitorPackageHelper.h"
#include "pystring.h"
#include "json.h"
#include "md5.h"
#include "TypeHelper.h"
#include <ctime>
#include <fstream>


bool ReadJsonStr(Json::Value& Node, const std::string& Field, std::string& data);
bool ReadJsonInt(Json::Value& Node, const std::string& Field, int& data);

bool LoadMonitorResponsePackage(const std::string& Package, Json::Value& dataCache);

bool ParseLogCmdPara(const std::string& Package, LogCmdParam& para );
bool PackSimilarlyContent(Json::Value& rootV, const LaneCodePara& lcp);

/* remove
std::string PackMonitorBeat(const int& provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId)
{

    Json::Value rootV;
    rootV["laneId"] = Json::Value(laneId);
    rootV["provinceId"] = Json::Value(provinceId);
    rootV["roadId"] = Json::Value(roadId);
    rootV["state"] = Json::Value(0);
    rootV["stationId"] = Json::Value(stationId);

    PackSimilarlyContent(rootV,);

    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
*/

std::string PackMonitorBeat(const LaneCodePara& lcp)
{
    Json::Value rootV;
    rootV["state"] = Json::Value(0);/*此字段本身的含义有问题，文档中并无提到什么时候需要修改该值*/

    PackSimilarlyContent(rootV, lcp);

    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
std::string PackMonitorCmdParse(const std::string& cmdId, const LaneCodePara& lcp)
{
    Json::Value rootV;
    rootV["cmdId"] = Json::Value(cmdId);
    PackSimilarlyContent(rootV, lcp);


    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
std::string PackMonitorLogInfo(const std::string& cmdId, const int& fileType, const std::string& fileName, const int& fileSize, const LaneCodePara& lcp)
{

    Json::Value rootV;

    rootV["cmdId"] = Json::Value(cmdId);
    rootV["fileName"] = Json::Value(pystring::os::path::basename(fileName));
    rootV["fileSize"] = Json::Value(fileSize);
    rootV["fileType"] = Json::Value(1);
	std::string md5_result;
	std::ifstream  upd_file(fileName.c_str(),std::ios::binary);
	file_md5(upd_file,md5_result);
	upd_file.close();
	rootV["fileMd5"] =Json::Value(md5_result);

    PackSimilarlyContent(rootV, lcp);

    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
std::string PackMonitorFilePiece(const std::string& cmdId, const int& fileSeq, const std::string& fileContent, const LaneCodePara& lcp)
{
    Json::Value rootV;
    rootV["cmdId"] = Json::Value( cmdId);

    rootV["fileContent"] = Json::Value( fileContent);
    rootV["fileMd5"] = Json::Value( md5(fileContent));
    rootV["fileSeq"] = Json::Value( fileSeq);
    PackSimilarlyContent(rootV, lcp);


    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
std::string PackMonitorConfirm(const std::string& cmdId, const LaneCodePara& lcp, const std::string& errorCode, const std::string& msg)
{

    Json::Value rootV;

    rootV["cmdId"] = Json::Value( cmdId);
    rootV["updateState"] = Json::Value( "3");
    Json::Value cmdResp;
    cmdResp["errorCode"] = Json::Value(errorCode);
    cmdResp["errorMsg"] = Json::Value(msg);
    rootV["cmdResp"] = cmdResp;

    PackSimilarlyContent(rootV, lcp);


    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}


std::vector<std::string> UnPackBeatResponse(const std::string& Package)
{
    std::vector<std::string> cmdList;
    Json::Value rootN;
    if (!LoadMonitorResponsePackage(Package, rootN))
    {
        return cmdList;
    }
    bool readOK = true;
    std::string cmdsValue;
    readOK = ReadJsonStr(rootN, "cmdList", cmdsValue);
    if (!readOK)
    {
        return cmdList;
    }
    if (cmdsValue.empty())
    {
        return cmdList;
    }
    pystring::split(cmdsValue, cmdList, ",");
    return cmdList;

}
//////////////////////////////////////////////////////////////
// 函数名称:UnPackCmdParseResponse
//
// 功能描述:解析日志文件上传包
//
// 输入参数:const std::string& Package,const std::string& id,
// 输出参数:LogCmdPair
// 编写人员:朱亚冰
// 编写时间:2017-11-03
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
LogCmdPair UnPackCmdParseResponse(const std::string& Package, const std::string& id)
{
    LogCmdPair oneCmd;
    Json::Value rootN;
    if (!LoadMonitorResponsePackage(Package, rootN))
    {
        return oneCmd;
    }
    bool readOK = true;
    std::string cmdType, cmdPara;
    readOK = ReadJsonStr(rootN, "cmdType", cmdType);
    readOK = readOK ? ReadJsonStr(rootN, "cmdParam", cmdPara) : readOK;
    if (!readOK)
    {
        return oneCmd;
    }
    LogCmdParam lcp;
    if (!ParseLogCmdPara(cmdPara, lcp))
    {
        return oneCmd;
    }
    oneCmd.second.cmdParam = lcp;
    oneCmd.second.cmdType = cmdType;
    oneCmd.first = id;


    return oneCmd;
}

//////////////////////////////////////////////////////////////
// 函数名称:UnPackLogInfoResponse
//
// 功能描述:获取文件的分片大小
//
// 输入参数:const std::string& Package,
// 输出参数:int
// 编写人员:朱亚冰
// 编写时间:2017-11-10
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
int UnPackLogInfoResponse(const std::string& Package)
{
    int piece = -1;
    Json::Value rootN;
    if (!LoadMonitorResponsePackage(Package, rootN))
    {
        return piece;
    }
    bool readOK = true;
    std::string pieceStr = "-1";
    readOK = ReadJsonInt(rootN, "piece", piece) ;//此为调试过程中的代码，该字段变成了string类型，可以删除该部分的逻辑|| ReadJsonStr(rootN, "piece", pieceStr);
    if (!readOK)
    {
        return piece;
    }
    if (pieceStr != "-1")
    {
        piece = Str2Int(pieceStr);
    }
    return piece;

}

//////////////////////////////////////////////////////////////
// 函数名称:UnPackPieceInfoResponse
//
// 功能描述:判断分片文件上传信息，是否有错误返回
//
// 输入参数:const std::string& Package,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-11-06
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool UnPackPieceInfoResponse(const std::string& Package)
{
    Json::Value rootN;
    if (!LoadMonitorResponsePackage(Package, rootN))
    {
        return false;
    }
    return true;
}

bool LoadMonitorResponsePackage(const std::string& Package, Json::Value& dataCache)
{
    Json::Reader  jsonReader;
    if (!jsonReader.parse(Package.c_str(), dataCache))
    {
        return false;
    }
    if ( dataCache["errorCode"].type() != Json::stringValue
            || dataCache["errorCode"].asString() != "0000")
    {
        return false;
    }
    return true;
}

/*
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
*/

//////////////////////////////////////////////////////////////
// 函数名称:PackSimilarlyContent
//
// 功能描述:有些内容，是所有包类似的，统一在此处生成
//
// 输入参数:Json::Value& rootV,
// 输出参数:bool
// 编写人员:朱亚冰
// 编写时间:2017-11-07
// 修改人员:
// 修改时间:
// 函数版本:1.0.0.0
// 备注说明:
//
//////////////////////////////////////////////////////////////
bool PackSimilarlyContent(Json::Value& rootV, const LaneCodePara& lcp)
{
    rootV["laneId"] = Json::Value(lcp.laneId);
    rootV["provinceId"] = Json::Value(lcp.provinceId);
    rootV["roadId"] = Json::Value(lcp.roadId);
    rootV["stationId"] = Json::Value(lcp.stationId);

    time_t currTime = time(NULL);
    struct tm * timeinfo;
    timeinfo = localtime(&currTime);
    char timeStr[50] = { 0 };
    std::strftime(timeStr, 40, "%Y-%m-%d %H:%M:%S.000", timeinfo);
    std::string yyyy_mm_dd_hh_mm_ss_sss_Time = timeStr;
    std::strftime(timeStr, 40, "%Y%m%d%H%M%S", timeinfo);
    std::string yyyymmddhhmmss_Time = timeStr;

    rootV["time"] = Json::Value(yyyy_mm_dd_hh_mm_ss_sss_Time);
    rootV["aes"] = Json::Value("aes");/*undefined*/
	rootV["deviceNo"] = Json::Value(lcp.devNo);/*reserved*/
	rootV["deviceType"] = Json::Value(lcp.devType);/*reserved*/
    rootV["packageType"] = Json::Value(0);/*undefined*/
    rootV["version"] = Json::Value("1.0");
	rootV["heartbeatTime"] = Json::Value(yyyy_mm_dd_hh_mm_ss_sss_Time);
    rootV["terminal"] = Json::Value("2");

	


    //一些数据可能不包含省份编码等信息，需要使用默认替代值


	/*
    if (rootV["provinceId"] == Json::nullValue)
    {
        rootV["provinceId"] = Json::Value(0);

    }
	*/
    int  md5Input_provinceId = rootV["provinceId"].asInt();

    std::string md5Input_version = rootV["version"].asString();
    std::string md5Input_devNo = rootV["deviceNo"].asString();
    std::string md5Input_devType = rootV["deviceType"].asString();

    char md5Input[100] = {0};
    sprintf(md5Input, "%05d%010s%020s%010s%014s",
            md5Input_provinceId,
            md5Input_version.c_str() ,
            md5Input_devNo.c_str(),
            md5Input_devType.c_str(),
            yyyymmddhhmmss_Time.c_str());

    //rootV["debug"]=Json::Value(md5Input);

    rootV["md5"] = Json::Value(md5(md5Input));/*undefined*/

    return true;
}

bool ParseLogCmdPara(const std::string& Package, LogCmdParam& para )
{
    Json::Value rootN;
    Json::Reader  jsonReader;
    if (!jsonReader.parse(Package.c_str(), rootN))
    {
        return false;
    }
    bool readOK = true;
    std::string startTime;
    std::string endTime;
    std::string modules;
    readOK = readOK ? ReadJsonStr(rootN, "startTime", startTime) : false;
    readOK = readOK ? ReadJsonStr(rootN, "endTime", endTime) : false;
    readOK = readOK ? ReadJsonStr(rootN, "modules", modules) : false;
    if (!readOK)
    {
        return false;
    }
    para.startTime = startTime;
    para.endTime = endTime;
    std::vector<std::string> moduleList;
    pystring::split(modules, moduleList, ",");
    std::copy(moduleList.begin(), moduleList.end(), std::inserter(para.modules, para.modules.begin()));

    return true;
}