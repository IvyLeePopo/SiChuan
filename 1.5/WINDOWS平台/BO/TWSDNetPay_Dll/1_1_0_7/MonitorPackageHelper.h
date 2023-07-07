#ifndef MONITOR_PACKAGE_HELPER_H
#define MONITOR_PACKAGE_HELPER_H

#include <string>
#include <vector>
#include <set>
#include <utility>
#include "ProtocolHelper.h"
/**

 * 协议处理函数集
 * 
 * 参考《终端运营见监控概要设计说明书（获取日志）》第三章中的内容
 * 1、根据参数，生成发送报文（形式是Packxxx）
 * 2、解析回馈报文，形式是(UnPackxxx)
 *
 * Tips:
 * 此处函数的虽说是完成报文的解析和封装，但是有些函数名称并不能直接看出和定义中的关系
 * 如果有更好更容易理解的名字，可以考虑使用新的函数名称
 *
 *
 * warnig:
 */

struct LogCmdParam
{
    std::string startTime;
    std::string endTime;
    std::set<std::string> modules;

};
struct LogCmdDetail
{
    std::string cmdType;
    LogCmdParam cmdParam;
};

/*
struct LaneCodePara
{
	std::string laneId;
	std::string stationId;
	std::string roadId;
	std::string devNo;
	std::string devType;
	int provinceId;
};
*/

typedef std::pair<std::string,LogCmdDetail> LogCmdPair;

//std::string PackMonitorBeat(const int& provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId);/*生成心跳报文*/
std::string PackMonitorBeat(const LaneCodePara& lcp);/*生成心跳报文*/


std::string PackMonitorCmdParse(const std::string& cmdId,const LaneCodePara& lcp);  /*生成命令查询报文*/
std::string PackMonitorLogInfo(const std::string& cmdId, const int& fileType, const std::string& fileName, const int& fileSize,const LaneCodePara& lcp);/*日志基本信息*/
std::string PackMonitorFilePiece(const std::string& cmdId,const int& fileSeq,const std::string& fileContent,const LaneCodePara& lcp);
std::string PackMonitorConfirm(const std::string& cmdId,const LaneCodePara& lcp,const std::string& errorCode="0000",const std::string& msg="");


std::vector<std::string> UnPackBeatResponse(const std::string& Package); /*获取命令列表*/
LogCmdPair UnPackCmdParseResponse(const std::string& Package,const std::string& id);
int UnPackLogInfoResponse(const std::string& Package);

bool UnPackPieceInfoResponse(const std::string& Package);

/*
struct LogCmdStreamParser
{
    std::vector<LogCmdPair> cmdDetailList;
    void operator()(const std::string& cmdPack)
    {
        LogCmdPair lcp = UnPackCmdParseResponse(cmdPack);
        cmdDetailArray.push_back(lcp);
    }
};
*/

#endif // MONITOR_PACKAGE_HELPER_H
