#ifndef REMOTE_ACCESS_CORE_H
#define REMOTE_ACCESS_CORE_H


#include "HttpConnection.h"
#include "ProtocolHelper.h"
#include <list>
#include <set>
#include <map>


class RemoteAccessCore
{
public:
    RemoteAccessCore();
    ~RemoteAccessCore();
public:
    void UpgradeFirmware(); //升级固件
    void RemoteInfo(const std::string& ip_str,int port);
	int SetLaneCodeInfo(const int& provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId,const std::string& baseDir);
	std::list<std::string> TakeUpgradePara();
	void TellUpgradeResult(	WPARAM wParam);
private:
    void Query();
    void Download();
    void Confirm();
	void Confirm_Core(RemoteAdsUpdateCmd& rauc,const std::string& fileType);
	void UnusualConfirm();


    void DownloadFiles(RemoteAdsUpdateCmd& RAUC);
    bool DownloadFile(AdsFileInfo& AFI);

	void MakeUpdParaStr(RemoteAdsUpdateCmd& RAUC);

	void NewIssueId(const std::string& id,const std::string& fileId);
	bool HasIssue(const std::string&id);

    void ParseQueryResult(const std::string& info);

	std::string ChooseDownloadPath(const std::string& fileType);
	std::string ChooseConfirmPath(const std::string fileType);

    bool SatisfyConfirmCondition(const RemoteAdsUpdateCmd& rauc,const std::string& fileId);



private:
	CRITICAL_SECTION m_accessMutex;
	std::list<std::string> m_UpdParaList;
	std::set<std::string> m_RecentIssue;//最近的issue列表，此处指下载成功的
    std::map<std::string,std::set<std::string >> m_IssueFilesState;//标志每个任务中文件的状态



	std::set<std::string> m_UpdingList;//正在更新的列表,此列表中的内容，在收到更新结果之后，会被删除，
    std::list<RemoteAdsUpdateCmd> m_CmdUpdateQueue;
    LaneCodePara m_LCP;

    HttpConnection m_HttpLink;
    std::string m_unusualConfirmPath;
    std::string m_queryPath;
    std::string m_downloadPath;
    std::string m_confirmPath;
	std::string m_baseDir;

    std::string m_freeDisplayQueryPath;
    std::string m_freeDisplayDownloadPath;
    std::string m_freeDisplayConfirmPath;
	time_t m_lastTaskStartTime;
    
private:
    RemoteAccessCore(const RemoteAccessCore&);
    RemoteAccessCore& operator = (const RemoteAccessCore&);

};

/**
 * 固件升级和样式更新使用了不同的URL地址访问，此处都放在一起处理
 * 查询：依次查询两个地址，获取文件更新列表
 * 下载：根据类型不同，去不同的地址下载
 * 确认：根据不同的确认类型，去不同的地址确认
 */

#endif // REMOTE_ACCESS_CORE_H
