#ifndef LOG_UPLOADER_H
#define LOG_UPLOADER_H

/**
 * ��־�ϴ�����
 */
#include <list>
#include <utility>  /*pair*/
#include "MonitorPackageHelper.h"
#include "HttpConnection.h"
#include "LogFileExplorer.h"
#include <map>
#include <windows.h>
#include "ProtocolHelper.h"

/**
 * ʵ������־��ȡ����
 * �ò���ʵ�֡��ն���Ӫ��ظ�Ҫ���˵����(��ȡ��־)���ᵽ�Ĺ���
 * �ṩUploadLog�ӿڣ��ýӿڰ�������־�ϴ�������������̣��ϴ���ѯ��ָ��������ļ���Ƭ�ϴ���ȷ�ϻ�����
 * SetLaneCodeInfo�������ó����ȱ�����Ϣ���ϴ�������Ҫ��
 * SetUploadServer�������÷������Ϣ���ϴ�������Ҫ��
 *
 * ����˵����
 * 
 * 
 */
class FilePieceAdapter;

class LogUploader
{

public:
    LogUploader();
    ~LogUploader();

public:
	int SetLaneCodeInfo(const int& provinceId, const std::string& roadId, const std::string& stationId, const std::string& laneId,const std::string& devType);
    void SetUploadServer(const std::string& ip_str,int port);
	void UploadLog(const std::string& dev_no);
private:
    void AskCmdId();
    void ParseCmdIds();
    void ExcuteCmds();

private:
    void ParseTargetCmdId(const std::string cmdId);
    void UploadFile(const LogCmdPair lcp);
    void UploadFilePiece(const FilePieceAdapter pieceData);
    /**
     * ����UploadFile�ȳ�Ա������û��ʹ�ó������õ�˵��( const std::string  VS const std::string& )
     * �����Ա�����������˵�������ʹ��mem_fun��bind1st����Ա������Ϊ������
     * ͬʱʹ��mem_fun��bind1st���ڳ�Ա����ʹ�����ò���������£���������������ͻ���⣬��ȡ��������
     * ȱ�㣺
     * �������п������ģ�Ȼ���ڵ��ô����Ͳ��������С��ԭ�򣬴����Ŀ��Խ���
     * �޸ķ�������������޸ĳ����ò�����
     * 1������mem_fun��bind1st��ʵ�֣�֧�����ò�����
     * 2������mem_fun��bind1st�ĵ���
     */
private:
    std::list<std::string> m_CmdIdQueue;
    std::list<LogCmdPair> m_CmdQueue;
    HttpConnection m_HttpLink;
    LogFileExplorer m_LogExplorer;
	std::string m_devno;
private:
	/*remove
    int m_provinceId;
    std::string m_roadId;
    std::string m_stationId;
    std::string m_laneId;
	*/
	LaneCodePara m_LCP;
private:
    bool mb_FilePieceUploadContinue;/*�Ƿ���Լ�����Ƭ�ϴ���һ����һ����Ƭ�ϴ������ʱ�򣬾����ø�ֵ*/
    int m_FilePieceSequence;
    std::string m_FilePieceCmdId;


private:
    LogUploader(const LogUploader&);
    LogUploader& operator = (const LogUploader&);

};


/**
 * �ļ��ֿ��ȡ������
 * ����֧��ֱ�Ӵ����ж�ȡ��Ӧ��С�����ݿ�
 */
class FilePieceAdapter
{
public:
    FilePieceAdapter()
    {
        Data = NULL;
        DataSize_Bytes = 0;
        PieceSize= 0;
    }
    ~FilePieceAdapter()
    {
       delete Data;
       Data = NULL;
    }

    size_t PieceSize;
    char* Data;
    size_t DataSize_Bytes;
    FilePieceAdapter(const FilePieceAdapter& other)
    {
        PieceSize = other.PieceSize;
        DataSize_Bytes = other.DataSize_Bytes;
        Data = new char[DataSize_Bytes];
        if(Data)
        {
            memcpy(Data,other.Data,other.DataSize_Bytes);
        }
    }

public:
    /**
     * �������ݵ������Ǳ��ֶ��߳��µķ�����ȷ��
     * �����ļ��ķ�Ƭ��С���ǹ̶��ģ�������Ҫ��ÿ�η���ǰ���÷�Ƭ�Ĵ�С
     * �ڶ��̻߳����£� �߳�֮��ķ�Ƭ��С�������໥���ţ���˷���
     */
    static void SetFilePieceSize(size_t pieceSize_Bytes)
    {
        threadsFilePieceSize[GetCurrentThreadId()] = pieceSize_Bytes;
    }
    static size_t FilePieceSize_Bytes()
    {
        return threadsFilePieceSize[GetCurrentThreadId()];
    }

private:
    static std::map<DWORD, size_t> threadsFilePieceSize;


};



#endif // LOG_UPLOADER_H
