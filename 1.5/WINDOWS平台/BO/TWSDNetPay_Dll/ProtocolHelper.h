#ifndef PROTOCOL_HELPER_H
#define PROTOCOL_HELPER_H
//云端和中间件的协议，中间件和终端的协议
//对协议进行转换解析
#include <string>
#include <list>
#include "json.h"

struct LaneCodePara
{
    std::string laneId;
    std::string stationId;
    std::string roadId;
    std::string devNo;
    int provinceId;
    std::string devType;
    int systemType;
    std::string deviceFlag;
    std::string softFlag;
};

struct AdsFilePiece
{
	std::string fileType;
    int offset;
    std::string fileContent;
    std::string md5;
};

struct AdsFileInfo
{
    std::string fileId;
    std::string md5;
    int fileSize;
    std::string fileName;
	std::string fileType;//
	
};

//更新信息
struct RemoteAdsUpdateCmd
{
    std::string infoId;
    int infoMainType;  //unused
    int infoSubType;    //unused
    std::list<AdsFileInfo> fileInfos;
    int updateState;//unused
	std::string issueId;
};

std::string PackAdsBeat(const LaneCodePara& lcp);
std::string PackAdsQuery(const LaneCodePara& lcp);
std::string PackAdsFilePieceDownload(const std::string& fileId,int offset,const LaneCodePara& lcp);
std::string PackAdsConfirm(const RemoteAdsUpdateCmd& cmd,const LaneCodePara& lcp);

bool UnPackAdsHeart(const std::string& beatResponse);
std::list<RemoteAdsUpdateCmd> UnPackAdsQueryResult(const std::string& responseJson);

std::list<AdsFilePiece> UnPackAdsFilePiece(const std::string& pieceJso,const std::string& fileTypen);

bool ExtractUpgradePara(const std::string& json ,std::string& infoId,std::string&issueId,std::string& fileType,std::string& fileId);

bool ReadJsonStr(Json::Value& Node, const std::string& Field, std::string& data);
bool ReadJsonInt(Json::Value& Node, const std::string& Field, int& data);



#endif // PROTOCOL_HELPER_H





