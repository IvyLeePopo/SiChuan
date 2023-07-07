#ifndef MONITOR_PACKAGE_HELPER_H
#define MONITOR_PACKAGE_HELPER_H

#include <string>
#include <vector>
#include <set>
#include <utility>
#include "ProtocolHelper.h"
/**

 * Э�鴦������
 * 
 * �ο����ն���Ӫ����ظ�Ҫ���˵���飨��ȡ��־�����������е�����
 * 1�����ݲ��������ɷ��ͱ��ģ���ʽ��Packxxx��
 * 2�������������ģ���ʽ��(UnPackxxx)
 *
 * Tips:
 * �˴���������˵����ɱ��ĵĽ����ͷ�װ��������Щ�������Ʋ�����ֱ�ӿ����Ͷ����еĹ�ϵ
 * ����и��ø������������֣����Կ���ʹ���µĺ�������
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

//std::string PackMonitorBeat(const int& provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId);/*������������*/
std::string PackMonitorBeat(const LaneCodePara& lcp);/*������������*/


std::string PackMonitorCmdParse(const std::string& cmdId,const LaneCodePara& lcp);  /*���������ѯ����*/
std::string PackMonitorLogInfo(const std::string& cmdId, const int& fileType, const std::string& fileName, const int& fileSize,const LaneCodePara& lcp);/*��־������Ϣ*/
std::string PackMonitorFilePiece(const std::string& cmdId,const int& fileSeq,const std::string& fileContent,const LaneCodePara& lcp);
std::string PackMonitorConfirm(const std::string& cmdId,const LaneCodePara& lcp,const std::string& errorCode="0000",const std::string& msg="");


std::vector<std::string> UnPackBeatResponse(const std::string& Package); /*��ȡ�����б�*/
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
