#ifndef LOG_UPLOADER_H
#define LOG_UPLOADER_H

/**
 * 日志上传处理
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
 * 实现了日志提取功能
 * 该部分实现《终端运营监控概要设计说明书(获取日志)》提到的功能
 * 提供UploadLog接口，该接口包含了日志上传的整个处理过程（上传查询，指令解析，文件分片上传，确认回馈）
 * SetLaneCodeInfo用于设置车道等编码信息（上传报文需要）
 * SetUploadServer用于设置服务端信息（上传报文需要）
 *
 * 其他说明：
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
     * 关于UploadFile等成员函数，没有使用常量引用的说明( const std::string  VS const std::string& )
     * 此类成员函数均采用了迭代处理，使用mem_fun和bind1st将成员函数绑定为处理器
     * 同时使用mem_fun和bind1st，在成员函数使用引用参数的情况下，会出现类型适配冲突问题，故取消了引用
     * 缺点：
     * 参数会有拷贝消耗，然由于调用次数和参数本身大小的原因，此消耗可以接受
     * 修改方法：（如果想修改成引用参数）
     * 1、增加mem_fun或bind1st的实现，支持引用参数，
     * 2、废弃mem_fun和bind1st的调用
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
    bool mb_FilePieceUploadContinue;/*是否可以继续分片上传，一般在一个分片上传错误的时候，就设置该值*/
    int m_FilePieceSequence;
    std::string m_FilePieceCmdId;


private:
    LogUploader(const LogUploader&);
    LogUploader& operator = (const LogUploader&);

};


/**
 * 文件分块读取适配器
 * 用于支持直接从流中读取相应大小的数据块
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
     * 下面内容的作用是保持多线程下的访问正确性
     * 由于文件的分片大小不是固定的，所以需要在每次访问前设置分片的大小
     * 在多线程环境下， 线程之间的分片大小参数会相互干扰，因此分离
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
