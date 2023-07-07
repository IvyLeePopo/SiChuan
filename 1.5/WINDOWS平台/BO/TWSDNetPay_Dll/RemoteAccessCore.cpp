/*
* @Author: 朱亚冰
* @Date:   2017-12-08
* @Last Modified by:   windf
* @Last Modified time: 2018-01-02
*/
#include "stdafx.h"
#include "RemoteAccessCore.h"
#include "md5.h"
#include "Base64.h"
#include "json.h"
#include <fstream>
#include "pystring.h"
#include "TWSDNetPay_Dll.h"


void WriteLog(const std::string&info)
{
    std::string dbgInfo = "[Remote Access Thread]" + info;
    GetGlobalApp()->RecordLog(dbgInfo.c_str());
}

std::string GetDevSerialNo()
{
    char tmp_chs[100] = {0};
    strncpy(tmp_chs, GetGlobalApp()->m_strTWProductSerialNo.GetBuffer(0), 50);
	//GetGlobalApp()->RecordLog( (std::string("XXX:")+tmp_chs).c_str()  );
    return std::string(tmp_chs);
}

std::string GetDeviceFlag()
{
    char ver_chs[100] = {0};
    strncpy(ver_chs, GetGlobalApp()->m_strTWProductFirmwareVer.GetBuffer(0), 50);
    char slave_ver_chs[100] = {0};
    strncpy(slave_ver_chs, GetGlobalApp()->m_strTWProductSlaveFirmwareVer.GetBuffer(0), 50);
    //if (ver_chs[0] == '\0' || slave_ver_chs[0] == '\0')
    //{
    //    return std::string();
    //}
	if(ver_chs[0]=='\0')
	{
		return std::string();
	}

	std::string slave_str="No Ack";
	if(slave_ver_chs[0]!='\0')
	{
		slave_str=slave_ver_chs;
	}

    Json::Value rootV;
    rootV["masterDeviceFlag"] = Json::Value(ver_chs);
    rootV["slaveDeviceFlag"] = Json::Value(slave_str);

    Json::FastWriter jfw;
    std::string result = jfw.write(rootV);
    return result;

}

std::string GetSoftFlag()
{
    char ver_chs[100] = {0};
    strncpy(ver_chs, GetGlobalApp()->m_strTWProductFirmwareVer.GetBuffer(0), 50);
    return std::string(ver_chs);
}



RemoteAccessCore::RemoteAccessCore()
{
    ::InitializeCriticalSection(&m_accessMutex);
    m_queryPath = "/trawe-itms/remoteUpdate/inquiry";
    m_downloadPath = "/trawe-itms/remoteUpdate/downloadFiles";
    m_confirmPath = "/trawe-itms/remoteUpdate/normalConfirm";
    m_unusualConfirmPath = "/trawe-itms/remoteUpdate/unusualConfirm";


    m_freeDisplayQueryPath = "/trawe-itms/remote/play/query";
    m_freeDisplayDownloadPath = "/trawe-itms/remote/play/download";
    m_freeDisplayConfirmPath = "/trawe-itms/remote/play/confirm";

    m_lastTaskStartTime = time(NULL);




}
RemoteAccessCore::~RemoteAccessCore()
{
    ::DeleteCriticalSection(&m_accessMutex);
}

int RemoteAccessCore::SetLaneCodeInfo(const int& provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId, const std::string& baseDir)
{
    CScopeLock apiLock(&m_accessMutex);
    m_LCP.laneId = laneId;
    m_LCP.roadId = roadId;
    m_LCP.stationId = stationId;
    m_LCP.provinceId = provinceId;
    m_LCP.systemType = 2; //windows
    m_LCP.deviceFlag = "deviceFlag";
    m_LCP.devNo = "deviceNo";
    m_LCP.devType = "TR200-V2.0-M1";
    m_baseDir = baseDir;
    return 0;
}

void RemoteAccessCore::RemoteInfo(const std::string& ip_str, int port)
{
    CScopeLock apiLock(&m_accessMutex);
    m_HttpLink.ChangeRemotePeer(ip_str, port);
}

void RemoteAccessCore::UpgradeFirmware()
{
    CScopeLock apiLock(&m_accessMutex);
    Query();
    Download();
    Confirm();
    m_CmdUpdateQueue.clear();
}
void RemoteAccessCore::ParseQueryResult(const std::string& info)
{
    std::list<RemoteAdsUpdateCmd> RAU = UnPackAdsQueryResult(info);

    for ( std::list<RemoteAdsUpdateCmd>::iterator it = RAU.begin(); it != RAU.end(); ++it)
    {
        if (HasIssue( it->issueId))
        {
            WriteLog("already proccess:" + it->issueId);
            continue;
        }
        m_CmdUpdateQueue.push_back(*it);
    }
}
void RemoteAccessCore::Query()
{
    m_LCP.devNo = GetDevSerialNo(); //获取设备编号
    m_LCP.deviceFlag = GetDeviceFlag();
    m_LCP.softFlag = GetSoftFlag();
    std::string queryPackage = PackAdsQuery(m_LCP);
    std::string queryRespnosePackage;

    if (!m_HttpLink.Post(m_queryPath, queryPackage, queryRespnosePackage))
    {
        return;
    }
    ParseQueryResult(queryRespnosePackage);
    if (!m_HttpLink.Post(m_freeDisplayQueryPath, queryPackage, queryRespnosePackage))
    {
        return;
    }
    ParseQueryResult(queryRespnosePackage);


    return ;
}

//此处会执行所有的下载命令
void RemoteAccessCore::Download()
{
    for (std::list<RemoteAdsUpdateCmd>::iterator it = m_CmdUpdateQueue.begin(); it != m_CmdUpdateQueue.end(); ++it)
    {

        DownloadFiles(*it);
    }
}

//对下载完成的确认，（更新失败与否不是此处内容）
void RemoteAccessCore::Confirm()
{
    for (std::list<RemoteAdsUpdateCmd>::iterator it = m_CmdUpdateQueue.begin(); it != m_CmdUpdateQueue.end(); ++it)
    {
        std::string fileType;
        if (!it->fileInfos.empty())
        {
            fileType = it->fileInfos.front().fileType;
        }
        Confirm_Core(*it, fileType);

        MakeUpdParaStr(*it);

    }
}
void RemoteAccessCore::Confirm_Core(RemoteAdsUpdateCmd& rauc, const std::string& fileType)
{
    std::string confirmPackage = PackAdsConfirm(rauc, m_LCP);
    std::string unuseStr;
    if (!m_HttpLink.Post( ChooseConfirmPath(fileType), confirmPackage, unuseStr))
    {
        ;//error
    }
}

void RemoteAccessCore::UnusualConfirm()
{
}

void RemoteAccessCore::DownloadFiles(RemoteAdsUpdateCmd& RAUC)
{

    for (std::list<AdsFileInfo>::iterator it = RAUC.fileInfos.begin(); it != RAUC.fileInfos.end(); ++it)
    {
        it->fileName = pystring::os::path::join(m_baseDir, it->fileName); //expand to fulll path
        if (!DownloadFile(*it))
        {
            // how to deal success file
            RAUC.updateState = 4;
            return;
        }
		NewIssueId(RAUC.issueId,it->fileId);
    }
  


    RAUC.updateState = 2;
    WriteLog("Download success");
}


bool RemoteAccessCore::DownloadFile(AdsFileInfo& AFI)
{
    int offset = 0 ;
    std::ofstream cacheFile;
    cacheFile.open(AFI.fileName.c_str(), std::ios_base::binary);
    if (!cacheFile.is_open())
    {
        WriteLog("open file failed:" + AFI.fileName);
        return false;
    }
    WriteLog("Downloading:" + AFI.fileName);
    while ( offset < AFI.fileSize)
    {
        std::string piecePackage = PackAdsFilePieceDownload(AFI.fileId, offset, m_LCP);
        std::string pieceResponsePackage ;
        char chs[1000] = {0};
        sprintf(chs, "Download process#id:%d/%d", offset, AFI.fileSize);
        WriteLog(chs);

        //下载多次
        bool post_success = false;
        for (int retry_it = 0; retry_it < 3; ++retry_it)
        {
            if (!m_HttpLink.Post(ChooseDownloadPath(AFI.fileType), piecePackage, pieceResponsePackage))
            {
                WriteLog("Post failed");
                //return false;
                Sleep(500);
                continue;
            }
            post_success = true;
            break;
        }


        if (!post_success)
        {
            WriteLog("download piece fail");
            return false;
        }

        std::list<AdsFilePiece> AFPList = UnPackAdsFilePiece(pieceResponsePackage, AFI.fileType);
        if (AFPList.empty())
        {
            WriteLog("empty piece list");
            return false;
        }
        AdsFilePiece AFP = AFPList.front();
        if ( offset != AFP.offset)
        {
            WriteLog("offset error");
            return false;
        }
        //check MD5
        std::string chk_md5 = md5(AFP.fileContent);
        if (chk_md5 != AFP.md5)
        {
            WriteLog("piece md5 error");
            return false;//wrong md5 check
        }

        //write file
        const int byteCacheSize = AFP.fileContent.size() * 2 + 5;
        char * byteBuf = new char[byteCacheSize];
        if (byteBuf == NULL)
        {
            WriteLog("memory quest error");
            return false;
        }
        size_t  byteSize = Base64_Decode(byteBuf, AFP.fileContent.c_str(), AFP.fileContent.size());
        cacheFile.write(byteBuf, byteSize);
        offset += byteSize;
        delete byteBuf;
        byteBuf = NULL;

    }
    if (offset != AFI.fileSize )
    {
        WriteLog("file size not equal");
        return false;
    }
    cacheFile.close();

    //total MD5
    std::ifstream tmpRead( AFI.fileName.c_str(), std::ios::binary);
    std::string tmpMD5;
    file_md5(tmpRead, tmpMD5);
    tmpRead.close();
    if (tmpMD5 != AFI.md5)
    {
        WriteLog("file md5 different,local#" + tmpMD5 + ",remote#" + AFI.md5);
        return false;
    }

    return true;
}

//此处进行处理，每次仅获取一个更新项，在收到前一个更新节目的结果之后，才会进行下一个条目的更新
std::list<std::string> RemoteAccessCore::TakeUpgradePara()
{
    CScopeLock apiLock(&m_accessMutex);
    if (!m_UpdingList.empty())
    {
        //此处检查任务是否超时
        if ( ( time(NULL) - m_lastTaskStartTime) > 30 * 60)
        {
            WriteLog("Task over time:go to next");
            TellUpgradeResult(0);

        }
        return std::list<std::string>();

    }
    if (m_UpdParaList.empty())
    {
        return std::list<std::string>();
    }
    std::string UpdPara = m_UpdParaList.front();
    m_UpdParaList.pop_front();

    std::list<std::string> tmpList;
    tmpList.push_back(UpdPara);
    m_UpdingList.insert(UpdPara);
    m_lastTaskStartTime = time(NULL);


    return tmpList;
}

void RemoteAccessCore::MakeUpdParaStr(RemoteAdsUpdateCmd& RAUC)
{
    if ( RAUC.infoMainType == 13 || 1) //firmware info
    {
        for (std::list<AdsFileInfo>::iterator it = RAUC.fileInfos.begin(); it != RAUC.fileInfos.end(); ++it)
        {

            Json::Value rootN;
            rootN["Version"] = Json::Value("1.0");
            Json::Value firmwareN;
            firmwareN["FirmwareType"] = Json::Value(it->fileType);
            firmwareN["FirmwarePath"] = Json::Value(it->fileName);
            firmwareN["issueId"] = Json::Value(RAUC.issueId);
            firmwareN["infoId"] = Json::Value(RAUC.infoId);
			firmwareN["fileId"] = Json::Value(it->fileId);
            rootN["FirmwareInfo"] = firmwareN;

            Json::FastWriter JFW;
            std::string result = JFW.write(rootN);
            m_UpdParaList.push_back(result);
        }
    }
}



void RemoteAccessCore::NewIssueId(const std::string& id,const std::string& fileId)
{
	m_IssueFilesState[id].insert(fileId);
	if(HasIssue(id))
	{
		return ;
	}
    while (m_RecentIssue.size() > 100)
    {
		std::string key = (*m_RecentIssue.begin());
        m_RecentIssue.erase(key);
		m_IssueFilesState.erase(key);
    }
    m_RecentIssue.insert(id);
}
bool RemoteAccessCore::HasIssue(const std::string&id)
{
    return m_RecentIssue.count(id) > 0;
}

void RemoteAccessCore::TellUpgradeResult(  WPARAM wParam)
{
    CScopeLock apiLock(&m_accessMutex);
    if (m_UpdingList.empty())
    {
        return ;
    }
    std::string paraInfo = (*m_UpdingList.begin());
    m_UpdingList.clear();//目前的集合中，仅仅保存一个更新项，因此可以删除

    RemoteAdsUpdateCmd rauc;
    std::string fileType;
	std::string fileId;

    ExtractUpgradePara(paraInfo, rauc.infoId, rauc.issueId, fileType,fileId);

    DWORD result = wParam;
    WORD type = (result & 0xFF00) >> 8;
    WORD value = result & 0x00FF;
    /*
    char chs[100]={0};
    sprintf(chs,"type:%d,value:%d,%d",type,value,wParam);
    WriteLog(chs);
    */
    rauc.updateState = 4;
    if (type == 1)
    {
        if (value == 1 || value == 2)
        {
            rauc.updateState = 3;
        }
    }
    else if (type == 2)
    {
        if (value == 3)
        {
            rauc.updateState = 3;
        }
    }

    /**
     * add 2018-01
     * 前提：一个任务包含多个文件，确认是针对任务的确认，而不是文件
     * 处理方法：
     * 1、收到失败反馈后，会立马确认相应的任务为失败，后续和该任务相关的返回不会再处理
     * 2、收到成功反馈，会先确认该反馈是否是最后一个反馈，如果是，则进行成功确认
     */
	if(!SatisfyConfirmCondition(rauc,fileId))
	{
		WriteLog("任务已经确认过或者不满足确认条件，不发送确认数据包。IssueId: " +rauc.issueId + ",fileId:"+fileId);
		return ;
	}
	std::string confirmResult = "确认任务："+rauc.issueId + ",结果 :";
	confirmResult += rauc.updateState==4?"失败":"成功";
	WriteLog(confirmResult);

    Confirm_Core(rauc, fileType);
}

std::string RemoteAccessCore::ChooseDownloadPath(const std::string& fileType)
{
    if (fileType.find("FW") == 0)
    {
        return m_downloadPath;
    }
    else if (fileType.find_first_of("SAC") == 0)
    {
        return m_freeDisplayDownloadPath;
    }
    return "error_download_path";
}

std::string RemoteAccessCore::ChooseConfirmPath(const std::string fileType)
{
    if (fileType.find("FW") == 0)
    {
        return m_confirmPath;
    }
    else if (fileType.find_first_of("SAC") == 0)
    {
        return m_freeDisplayConfirmPath;
    }
    return "error_confirm_path";
}

bool RemoteAccessCore::SatisfyConfirmCondition(const RemoteAdsUpdateCmd& rauc, const std::string& fileId)
{
	std::string issueId = rauc.issueId;
	if(m_IssueFilesState.find(issueId)==m_IssueFilesState.end())
	{
		return false;
	}
	if(m_IssueFilesState[issueId].empty())
	{
		return false;//
	}
	if(rauc.updateState==4)
	{
		m_IssueFilesState[issueId].clear();
		return true;//遇到失败，直接失败
	}
	m_IssueFilesState[issueId].erase(fileId);
	if(m_IssueFilesState[issueId].empty())
	{
		return true;//已经是最后一个成功的文件了
	}
	return false;

}