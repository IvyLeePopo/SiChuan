/*
* @Author: ÖìÑÇ±ù
* @Date:   2017-12-13
* @Last Modified by:   windf
* @Last Modified time: 2018-01-09
*/

#include "ProtocolHelper.h"
#include "md5.h"

#include <ctime>



bool PackSimilarlyContent_Upgrade(Json::Value& rootV, const LaneCodePara& lcp);
bool LoadAdsResponsePackage(const std::string& Package, Json::Value& dataCache);





std::string PackAdsBeat(const LaneCodePara& lcp)
{
    Json::Value rootV;

    PackSimilarlyContent_Upgrade(rootV, lcp);
    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
std::string PackAdsQuery(const LaneCodePara& lcp)
{
    return PackAdsBeat(lcp);
}
std::string PackAdsFilePieceDownload(const std::string& fileId, int offset, const LaneCodePara& lcp)
{
    Json::Value rootV;
    PackSimilarlyContent_Upgrade(rootV, lcp);

    Json::Value infosV;
    Json::Value eachFileV;
    eachFileV["fileId"] = Json::Value(fileId);
    eachFileV["offset"] = Json::Value(offset);
    infosV.append(eachFileV);

	rootV["infos"]=infosV;

    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;
}
std::string PackAdsConfirm(const RemoteAdsUpdateCmd& cmd, const LaneCodePara& lcp)
{
    Json::Value rootV;
    PackSimilarlyContent_Upgrade(rootV, lcp);

    rootV["infoId"] = Json::Value(cmd.infoId);
    rootV["updateState"] = Json::Value(cmd.updateState);
	rootV["issueId"]= Json::Value(cmd.issueId);

    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;

}

bool UnPackAdsHeart(const std::string& beatResponse)
{
    Json::Value rootN;
    if (!LoadAdsResponsePackage(beatResponse, rootN))
    {
        return false;
    }
    return true;
}
std::list<RemoteAdsUpdateCmd> UnPackAdsQueryResult(const std::string& responseJson)
{
    std::list<RemoteAdsUpdateCmd> cmdList;
    Json::Value rootN;
    if (!LoadAdsResponsePackage(responseJson, rootN))
    {
        return cmdList;
    }
    Json::Value infosV = rootN["infos"];
    if (infosV.type() != Json::arrayValue)
    {
        return cmdList;
    }
    for (size_t it = 0; it < infosV.size(); ++it)
    {
        RemoteAdsUpdateCmd RAU;
	
        bool readInfo_ok = ReadJsonStr(infosV[it], "infoId", RAU.infoId);
        readInfo_ok = readInfo_ok ? ReadJsonInt(infosV[it], "infoMainType", RAU.infoMainType) : false;
        readInfo_ok = readInfo_ok ? ReadJsonInt(infosV[it], "infoSubType", RAU.infoSubType) : false;
		readInfo_ok = readInfo_ok ? ReadJsonStr(infosV[it], "issueId", RAU.issueId) : false;
        if (!readInfo_ok)
        {
            return cmdList;//error info
        }
        Json::Value fileInfosV = infosV[it]["fileInfos"];
        if (fileInfosV.type() != Json::arrayValue)
        {
            return cmdList;//error type
        }
	
        //read file info
        for (size_t file_it = 0; file_it < fileInfosV.size(); ++file_it)
        {
            AdsFileInfo AFI;
            readInfo_ok = ReadJsonStr(fileInfosV[file_it], "fileId", AFI.fileId);
            readInfo_ok = readInfo_ok ? ReadJsonStr(fileInfosV[file_it], "md5", AFI.md5) : false;
            readInfo_ok = readInfo_ok ? ReadJsonInt(fileInfosV[file_it], "fileSize", AFI.fileSize) : false;
            readInfo_ok = readInfo_ok ? ReadJsonStr(fileInfosV[file_it], "fileName", AFI.fileName) : false;
			readInfo_ok = readInfo_ok ? ReadJsonStr(fileInfosV[file_it], "fileType", AFI.fileType) : false;

            if (!readInfo_ok)
            {
                return cmdList;//error file info
            }
			RAU.fileInfos.push_back(AFI);
			
        }
		cmdList.push_back(RAU);
    }

    return cmdList;

}

std::list<AdsFilePiece> UnPackAdsFilePiece(const std::string& pieceJson,const std::string& fileType)
{
    std::list<AdsFilePiece> pieceList;
    Json::Value rootN;
    if (!LoadAdsResponsePackage(pieceJson, rootN))
    {
        return pieceList;
    }
    Json::Value dataList = rootN["dateTypes"];//spell error for 'dateTypes'

    if (dataList.type() != Json::arrayValue)
    {
        return pieceList;
    }
    bool readInfo_ok ;
    for (int it = 0; it < dataList.size(); ++it)
    {
        AdsFilePiece AFP;
        //readInfo_ok = ReadJsonStr(dataList[it],"fileType",AFP.fileType);
		AFP.fileType = fileType;
		readInfo_ok = true;
        readInfo_ok =readInfo_ok? ReadJsonStr(dataList[it],"fileContent",AFP.fileContent):false;
        readInfo_ok =readInfo_ok? ReadJsonInt(dataList[it],"offset",AFP.offset):false;
        readInfo_ok =readInfo_ok? ReadJsonStr(dataList[it],"md5",AFP.md5):false;
        if(!readInfo_ok)
        {
            return pieceList;
        }
        pieceList.push_back(AFP);

    }

    return pieceList;

}


bool PackSimilarlyContent_Upgrade(Json::Value& rootV, const LaneCodePara& lcp)
{
    rootV["laneId"] = Json::Value(lcp.laneId);
    rootV["provinceId"] = Json::Value(lcp.provinceId);
    rootV["roadId"] = Json::Value(lcp.roadId);
    rootV["stationId"] = Json::Value(lcp.stationId);
    rootV["state"] = Json::Value(0);
    rootV["deviceFlag"] = Json::Value(lcp.deviceFlag);
    rootV["softFlag"] = Json::Value(lcp.softFlag);

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
    rootV["queryTime"] = Json::Value(yyyy_mm_dd_hh_mm_ss_sss_Time);
    rootV["system"] = Json::Value(2);
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


    rootV["md5"] = Json::Value(md5(md5Input));/*undefined*/

    return true;

}

bool LoadAdsResponsePackage(const std::string& Package, Json::Value& dataCache)
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


bool ExtractUpgradePara(const std::string& json ,std::string& infoId,std::string&issueId,std::string& fileType,std::string& fileId)
{
	Json::Reader  jsonReader;
	Json::Value rootN;
    if (!jsonReader.parse(json.c_str(), rootN))
    {
        return false;
    }
	Json::Value firmInfo = rootN["FirmwareInfo"];
	bool read_ok=true;
	read_ok = ReadJsonStr(firmInfo,"infoId",infoId);
	read_ok = read_ok ?ReadJsonStr(firmInfo,"issueId",issueId):false;
	read_ok = read_ok ?ReadJsonStr(firmInfo,"FirmwareType",fileType):false;
	read_ok = read_ok ? ReadJsonStr(firmInfo,"fileId",fileId):false;
	return read_ok;
	
}