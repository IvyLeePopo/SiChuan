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
    void UpgradeFirmware(); //�����̼�
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
	std::set<std::string> m_RecentIssue;//�����issue�б��˴�ָ���سɹ���
    std::map<std::string,std::set<std::string >> m_IssueFilesState;//��־ÿ���������ļ���״̬



	std::set<std::string> m_UpdingList;//���ڸ��µ��б�,���б��е����ݣ����յ����½��֮�󣬻ᱻɾ����
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
 * �̼���������ʽ����ʹ���˲�ͬ��URL��ַ���ʣ��˴�������һ����
 * ��ѯ�����β�ѯ������ַ����ȡ�ļ������б�
 * ���أ��������Ͳ�ͬ��ȥ��ͬ�ĵ�ַ����
 * ȷ�ϣ����ݲ�ͬ��ȷ�����ͣ�ȥ��ͬ�ĵ�ַȷ��
 */

#endif // REMOTE_ACCESS_CORE_H
